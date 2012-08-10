/*
 ****************************************************************************
 * serverpacketsparser.cpp
 *
 * Created On: 2010-7-13
 *     Author: 贺辉
 *    License: LGPL
 *    Comment:
 *
 *
 *    =============================  Usage  =============================
 *|
 *|
 *    ===================================================================
 *
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 ****************************************************************************
 */

/*
 ***************************************************************************
 * Last Modified On: 2010-9-3
 * Last Modified By: 贺辉
 ***************************************************************************
 */

#include <QDebug>

#include "serverpacketsparser.h"


//#ifdef Q_CC_MSVC
//#include <windows.h>
//#include "HHSharedWindowsManagement/hwindowsmanagement.h"
//#define msleep(x) Sleep(x)
//#endif

//#ifdef Q_CC_GNU
//#include <unistd.h>
//#define msleep(x) usleep(x*1000)
//#endif



namespace HEHUI {


ServerPacketsParser::ServerPacketsParser(ResourcesManagerInstance *manager, QObject *parent)
    :QObject(parent), m_resourcesManager(manager)
{

    Q_ASSERT(m_resourcesManager);

    m_udpServer = m_resourcesManager->getUDPServer();
    Q_ASSERT_X(m_udpServer, "ServerPacketsParser::ServerPacketsParser(...)", "Invalid UDPServer!");


    m_rtp = m_resourcesManager->getRTP();
    Q_ASSERT(m_rtp);

    m_udtProtocol = m_rtp->getUDTProtocol();
    Q_ASSERT(m_udtProtocol);

    m_tcpServer = m_rtp->getTCPServer();
    Q_ASSERT(m_tcpServer);

    connect(m_udpServer, SIGNAL(signalNewUDPPacketReceived(Packet*)), this, SLOT(parseIncomingPacketData(Packet*)), Qt::QueuedConnection);
    connect(m_udtProtocol, SIGNAL(packetReceived(Packet*)), this, SLOT(parseIncomingPacketData(Packet*)), Qt::QueuedConnection);
    connect(m_tcpServer, SIGNAL(packetReceived(Packet*)), this, SLOT(parseIncomingPacketData(Packet*)), Qt::QueuedConnection);



    localUDTListeningAddress = m_udtProtocol->getUDTListeningAddress();
    localUDTListeningPort = m_udtProtocol->getUDTListeningPort();
    m_localTCPServerListeningPort = m_tcpServer->getTCPServerListeningPort();

    m_serverName = QHostInfo::localHostName().toLower();

    localIPMCListeningPort = m_udpServer->localPort();



}

ServerPacketsParser::~ServerPacketsParser() {
    // TODO Auto-generated destructor stub





}



void ServerPacketsParser::parseIncomingPacketData(Packet *packet){

    //    qDebug()<<"----ServerPacketsParser::parseIncomingPacketData(Packet *packet)";


    
    QByteArray packetData = packet->getPacketData();
    QDataStream in(&packetData, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_6);

    QString peerID = "";
    in >> peerID;

    QHostAddress peerAddress = packet->getPeerHostAddress();
    quint16 peerPort = packet->getPeerHostPort();
//    quint16 packetSerialNumber = packet->getPacketSerialNumber();
    quint8 packetType = packet->getPacketType();
    int socketID = packet->getSocketID();
//    qDebug()<<"--ServerPacketsParser::parseIncomingPacketData(...) "<<" peerID:"<<peerID<<" peerAddress:"<<peerAddress<<" peerPort:"<<peerPort<<" packetSerialNumber:"<<packetSerialNumber<<" packetType:"<<packetType;

    switch(packetType){
//    case quint8(HEHUI::HeartbeatPacket):
//    {
//        emit signalHeartbeatPacketReceived(packet->getPeerHostAddress().toString(), peerID);
//        qDebug()<<"~~HeartbeatPacket--"<<" peerID:"<<peerID;
//    }
//    break;
//    case quint8(HEHUI::ConfirmationOfReceiptPacket):
//    {
//        quint16 packetSerialNumber1 = 0, packetSerialNumber2 = 0;
//        in >> packetSerialNumber1 >> packetSerialNumber2;
//        m_packetHandlerBase->removeWaitingForReplyPacket(packetSerialNumber1, packetSerialNumber2);
//        emit signalConfirmationOfReceiptPacketReceived(packetSerialNumber1, packetSerialNumber2);
//        qDebug()<<"~~ConfirmationOfReceiptPacket--"<<packetSerialNumber1<<" "<<packetSerialNumber2;
//    }
//    break;
    case quint8(MS::ClientLookForServer):
    {

        quint16 peerUDPListeningPort;
        QString version = "";
        in >> peerUDPListeningPort >> version;

        sendServerDeclarePacket(peerAddress, peerUDPListeningPort);
        //emit signalClientLookForServerPacketReceived(peerAddress, peerPort, peerName);
        qWarning()<<"~~ClientLookForServer--"<<" peerAddress:"<<peerAddress.toString()<<"   peerPort:"<<peerPort<<" peerUDPListeningPort:"<<peerUDPListeningPort <<" Version:"<<version;
    }
    break;
    case quint8(MS::ClientOnline):
    {
        QString peerName;
        in >> peerName;
        emit signalClientOnlineStatusChanged(socketID, peerName, true);

        qDebug()<<"~~ClientOnline--"<<" peerAddress:"<<peerAddress<<"   peerName:"<<peerName;
    }
    break;
    case quint8(MS::ClientOffline):
    {
        QString peerName;
        in >> peerName;
        emit signalClientOnlineStatusChanged(socketID, peerName, false);
        qDebug()<<"~~ClientOffline--"<<" peerAddress:"<<peerAddress.toString()<<"   peerName:"<<peerName;;
    }
    break;


    case quint8(MS::AdminOnline):
    {
        QString peerName = "", adminName = "";
        in >> peerName >> adminName;
        emit signalAdminOnlineStatusChanged(socketID, peerName, adminName, true);

        qDebug()<<"~~AdminOnline--"<<" peerAddress:"<<peerAddress<<"   peerName:"<<peerName <<" adminName:"<<adminName;

    }
    break;
    case quint8(MS::AdminOffline):
    {
        QString peerName = "", adminName = "";
        in >> peerName >> adminName;
        emit signalAdminOnlineStatusChanged(socketID, peerName, adminName, false);

        qDebug()<<"~~AdminOffline--"<<" peerAddress:"<<peerAddress<<"   peerName:"<<peerName <<" adminName:"<<adminName;

    }
    break;

    //    case quint8(MS::ServerOnline):
    //        break;
    //    case quint8(MS::ServerOffline):
    //        break;
    //    case quint8(MS::AdminRequestClientInfo):
    //        {
    //            sendConfirmationOfReceiptPacket(packet->getPeerHostAddress(), packet->getPeerHostPort(), packet->getPacketSerialNumber(), peerID);

    //            QString peerAddress;
    //            quint16 peerPort;
    //            QString adminID;
    //            QByteArray passwordArray;
    //            in >> peerAddress >> peerPort >> adminID >> passwordArray;
    //            emit signalAdminLoggedOnToServerRequestPacketReceived(QHostAddress(peerAddress), peerPort, adminID, passwordArray);
    //            qDebug()<<"~~AdminLoggedOnToServerRequest";
    //        }
    //        break;
    //    case quint8(MS::ServerResponseAdminLoggedOnToServerRequest):
    //        break;
    //    case quint8(MS::ServerRequestRemoteConsole):
    //        break;
    //    case quint8(MS::ClientResponseRemoteConsoleRequest):
    //        {
    //            sendConfirmationOfReceiptPacket(packet->getPeerHostAddress(), packet->getPeerHostPort(), packet->getPacketSerialNumber(), peerID);

    //            QString adminID;
    //            quint8 accept;
    //            QString extraMessage;
    //            in >> adminID >> accept >> extraMessage;
    //            emit signalClientResponseRemoteConsolePacketReceived(packet->getPeerHostAddress(), packet->getPeerHostPort(), adminID, ((accept == 0)?false:true), extraMessage);
    //            qDebug()<<"~~ClientResponseRemoteConsole";
    //        }
    //        break;
    //    case quint8(MS::RemoteConsoleCMDFromServer):
    //        break;
    //    case quint8(MS::RemoteConsoleCMDResultFromClient):
    //        {
    //            sendConfirmationOfReceiptPacket(packet->getPeerHostAddress(), packet->getPeerHostPort(), packet->getPacketSerialNumber());

    //            QString result;
    //            in >> result;
    //            emit signalRemoteConsoleCMDResultFromClientPacketReceived(packet->getPacketSerialNumber(), result);
    //            qDebug()<<"~~RemoteConsoleCMDResultFromClient";
    //        }
    //        break;
    //    case quint8(MS::ServerRequestClientInfo):
    //        break;
    case quint8(MS::ClientResponseClientSummaryInfo):
    {
        QString workgroupName = "", networkInfo = "", usersInfo = "", osInfo = "", admins = "", clientVersion = "";
        bool usbsdEnabled = false, programesEnabled = false, isJoinedToDomain = false;
        in >> workgroupName >> networkInfo >> usersInfo >> osInfo >> usbsdEnabled >> programesEnabled >> admins >> isJoinedToDomain >> clientVersion;
        emit signalClientResponseClientSummaryInfoPacketReceived(peerID, workgroupName, networkInfo, usersInfo, osInfo, usbsdEnabled, programesEnabled, admins, isJoinedToDomain, clientVersion);
        qDebug()<<"~~ClientResponseClientInfo";
    }
    break;

    case quint8(MS::ClientResponseClientDetailedInfo):
    {
        QString systemInfo = "";
        in >> systemInfo;
        emit signalClientResponseClientDetailedInfoPacketReceived(peerID, systemInfo);
        qDebug()<<"~~ClientResponseClientDetailedInfo";

    }
    break;

    case quint8(MS::ClientRequestSoftwareVersion):
    {
//        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerID);

        QString softwareName;
        in >> softwareName;
        emit signalClientRequestSoftwareVersionPacketReceived(softwareName);
        qDebug()<<"~~ClientRequestSoftwareVersion";
    }
    break;
    //    case quint8(MS::ServerResponseSoftwareVersion):
    //        break;
    //    case quint8(MS::ServerAnnouncement):
    //        break;
    case quint8(MS::ClientLog):
    {
//        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerID);

        QString users = "", log = "", clientTime = "";
        quint8 logType = 0;
        in >> users >> logType >> log >> clientTime;
        emit signalClientLogReceived(peerID, users, packet->getPeerHostAddress().toString(), logType, log, clientTime);
        qDebug()<<"~~ClientLog";
    }
    break;
    default:
//        qWarning()<<"Unknown Packet Type:"<<packetType
//                 //<<"    Serial Number: "<<packetSerialNumber
//                <<" From:"<<peerAddress.toString()
//               <<":"<<peerPort;
        break;

    }



}































} //namespace HEHUI
