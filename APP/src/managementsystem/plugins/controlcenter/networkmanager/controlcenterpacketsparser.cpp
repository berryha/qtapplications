/*
 ****************************************************************************
 * controlcenterpacketsparser.cpp
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
 * Last Modified On: 2010-7-13
 * Last Modified By: 贺辉
 ***************************************************************************
 */


#include <QHostInfo>
#include <QDebug>

#include "controlcenterpacketsparser.h"

#ifdef Q_CC_MSVC
#include <windows.h>
#include "HHSharedWindowsManagement/hwindowsmanagement.h"
#define msleep(x) Sleep(x)
#endif

#ifdef Q_CC_GNU
#include <unistd.h>
#define msleep(x) usleep(x*1000)
#endif


namespace HEHUI {


ControlCenterPacketsParser::ControlCenterPacketsParser(UDPServer *udpServer, UDTProtocol *udtProtocol, QObject *parent)
    :QObject(parent), m_udpServer(udpServer), m_udtProtocol(udtProtocol)
{

    Q_ASSERT_X(m_udpServer, "ControlCenterPacketsParser::ControlCenterPacketsParser(...)", "Invalid UDPServer!");
    Q_ASSERT_X(m_udtProtocol, "ControlCenterPacketsParser::ControlCenterPacketsParser(...)", "Invalid UDTProtocol!");

    connect(m_udpServer, SIGNAL(signalNewUDPPacketReceived(Packet*)), this, SLOT(parseIncomingPacketData(Packet*)), Qt::QueuedConnection);
    connect(m_udtProtocol, SIGNAL(packetReceived(Packet*)), this, SLOT(parseIncomingPacketData(Packet*)), Qt::QueuedConnection);

    serverAddress = QHostAddress::Null;
    serverUDTListeningPort = 0;
    serverName = "";

    heartbeatTimer = 0;


    ipmcGroupAddress = QString(IP_MULTICAST_GROUP_ADDRESS);
    ipmcListeningPort = quint16(IP_MULTICAST_GROUP_PORT);


    localRUDPListeningPort = m_udtProtocol->getUDTListeningPort();


    m_localComputerName = QHostInfo::localHostName().toLower();
    m_localID = m_localComputerName + "/ControlCenter";



}

ControlCenterPacketsParser::~ControlCenterPacketsParser() {
    // TODO Auto-generated destructor stub
    qDebug()<<"~ControlCenterPacketsParser()";

    QMutexLocker locker(&mutex);


    if(heartbeatTimer){
        heartbeatTimer->stop();
    }
    delete heartbeatTimer;
    heartbeatTimer = 0;



}


void ControlCenterPacketsParser::parseIncomingPacketData(Packet *packet){
    qDebug()<<"----ControlCenterPacketsParser::parseIncomingPacketData(Packet *packet)";


    QByteArray packetData = packet->getPacketData();
    QDataStream in(&packetData, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_6);

    QString peerName = "";
    in >> peerName;

    QHostAddress peerAddress = packet->getPeerHostAddress();
    quint16 peerPort = packet->getPeerHostPort();
//    quint16 packetSerialNumber = packet->getPacketSerialNumber();
    quint8 packetType = packet->getPacketType();
    int socketID = packet->getSocketID();

    PacketHandlerBase::recylePacket(packet);
//    qDebug()<<"--ControlCenterPacketsParser::parseIncomingPacketData(...) "<<" peerName:"<<peerName<<" peerAddress:"<<peerAddress<<" peerPort:"<<peerPort<<" packetSerialNumber:"<<packetSerialNumber<<" packetType:"<<packetType;

    switch(packetType){
//    case quint8(HEHUI::HeartbeatPacket):
//    {
//        //QString computerName;
//        //in >> computerName;
//        emit signalHeartbeatPacketReceived(peerName);
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
    case quint8(MS::ServerDeclare):
    {

        QString address = "";
        quint16 port = 0;
        QString version;
        int serverInstanceID = 0;
        in >> address >> port >> version >> serverInstanceID;
        serverAddress = peerAddress;
        serverUDTListeningPort = port;
        serverName = peerName;
        emit signalServerDeclarePacketReceived(serverAddress.toString(), serverUDTListeningPort, serverName, version, serverInstanceID);

        qDebug()<<"~~ServerDeclare"<<" serverAddress:"<<serverAddress<<" servername:"<<peerName <<" serverRUDPListeningPort:"<<serverUDTListeningPort;
    }
    break;
    //    case quint8(MS::ClientOnline):
    //        break;
    //    case quint8(MS::ClientOffline):
    //        break;
    case quint8(MS::ServerOnline):
    {
        QString address = "";
        quint16 port;
        in >> address >> port;
        serverAddress = peerAddress;
        serverUDTListeningPort = port;
        serverName = peerName;
        emit signalServerOnlinePacketReceived(serverAddress, serverUDTListeningPort, serverName);
    }
    break;
    case quint8(MS::ServerOffline):
    {
        QString address = "";
        quint16 port = 0;
        in >> address >> port;
        serverAddress = QHostAddress::Null;
        serverUDTListeningPort = 0;
        serverName = peerName;
        emit signalServerOfflinePacketReceived(serverAddress, serverUDTListeningPort, serverName);
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
    case quint8(MS::ClientResponseClientDetailedInfo):
    {
        qDebug()<<"ClientResponseClientInfoToAdminRequest";

        QString systemInfo = "";
        in >> systemInfo;
        emit signalClientResponseClientDetailedInfoPacketReceived(peerName, systemInfo);

    }
    break;

    case quint8(MS::ClientResponseUSBInfo):
    {
        qDebug()<<"ClientResponseUSBInfo";

        QString info = "";
        in >> info;
        emit signalClientResponseUSBInfoPacketReceived(socketID, peerName, info);

    }
    break;
    case quint8(MS::ClientResponseProgramesInfo):
    {
        qDebug()<<"ClientResponseUSBInfo";

        QString info = "";
        in >> info;
        emit signalClientResponseProgramesInfoPacketReceived(socketID, peerName, info);

    }
    break;
    //    case quint8(MS::ServerResponseAdminLoggedOnToServerRequest):
    //        break;
    //    case quint8(MS::AdminRequestRemoteConsole):
    //        {
    //            QString adminID;
    //            in >> adminID;
    //            emit signalServerRequestRemoteConsolePacketReceived(adminID);
    //        }
    //        break;
    case quint8(MS::ClientResponseRemoteConsoleStatus):
    {
        QString extraMessage = "";
        quint8 running = false;
        in >> running >> extraMessage;

        emit signalClientResponseRemoteConsoleStatusPacketReceived(peerName, ((running == 0)?false:true), extraMessage);
        qDebug()<<"~~ClientResponseRemoteConsole";
    }
    break;
    //    case quint8(MS::RemoteConsoleCMDFromAdmin):
    //        {
    //            QString command;
    //            in >> command;
    //            emit signalRemoteConsoleCMDFromServerPacketReceived(command);
    //        }
    //        break;
    case quint8(MS::RemoteConsoleCMDResultFromClient):
    {
        QString result = "";
        in >> result;
        emit signalRemoteConsoleCMDResultFromClientPacketReceived(peerName, result);
        qDebug()<<"~~RemoteConsoleCMDResultFromClient";
    }
    break;
    //    case quint8(MS::ServerRequestClientInfo):
    //        {
    //            QString groupName, computerName;
    //            QChar separtor;
    //            in >> groupName >> separtor >> computerName;
    //            emit signalServerRequestClientSummaryInfoPacketReceived(groupName, computerName);
    //        }
    //        break;
    case quint8(MS::ServerResponseSoftwareVersion):
    {
        QString softwareName, version;
        QChar separtor;
        in >> softwareName >> separtor >>version;
        emit signalServerResponseSoftwareVersionPacketReceived(softwareName, softwareName);
    }
    break;

    case quint8(MS::ClientResponseClientSummaryInfo):
    {
        QString workgroupName = "", networkInfo = "", usersInfo = "", osInfo = "", admins = "", clientVersion = "";
        bool usbsdEnabled = false, programesEnabled = false;
        in >> workgroupName >> networkInfo >> usersInfo >> osInfo >> usbsdEnabled >> programesEnabled >> admins >> clientVersion;
        emit signalClientResponseClientSummaryInfoPacketReceived(peerName, workgroupName, networkInfo, usersInfo, osInfo, usbsdEnabled, programesEnabled, admins, clientVersion);
        //qWarning()<<"~~ClientResponseClientSummaryInfo";
    }
    break;
    case quint8(MS::ClientResponseAdminConnectionResult):
    {
        QString message = "";
        bool result = false;
        in >> result >> message;
        emit signalClientResponseAdminConnectionResultPacketReceived(socketID, peerName, result, message);
        qDebug()<<"~~ClientResponseAdminConnectionResult";
    }
    break;
    case quint8(MS::ClientMessage):
    {
        //一定要用ipmcListeningPort
//        sendConfirmationOfReceiptPacket(peerAddress, ipmcListeningPort, packetSerialNumber, peerName);

        QString message = "";
        in >> message;
        emit signalClientMessagePacketReceived(peerName, message);
    }
    break;

    case quint8(MS::UserResponseRemoteAssistance):
    {

        QString userName = "", computerName = "";
        bool accept = false;
        in >> userName >> computerName >> accept;
        
        emit signalUserResponseRemoteAssistancePacketReceived(userName, computerName, accept);
        
    }
    break;

    case quint8(MS::NewPasswordRetrevedByUser):
    {

//        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerName);

        QString userName = "", computerName = "";
        in >> userName >> computerName ;
        
        emit signalNewPasswordRetrevedByUserPacketReceived(userName, computerName);
        
    }
    break;




    default:
        qWarning()<<"ControlCenterPacketsParser! Unknown Packet Type: "<<packetType
//                 <<"    Serial Number: "<<packetSerialNumber
                <<"    From: "<<peerAddress.toString()
               <<":"<<peerPort;
        break;

    }



}























} //namespace HEHUI
