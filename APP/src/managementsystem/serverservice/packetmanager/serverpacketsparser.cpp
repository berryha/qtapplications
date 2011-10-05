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


ServerPacketsParser::ServerPacketsParser(NetworkManagerInstance *networkManager, QObject *parent)
    :PacketsParserBase(networkManager, parent), m_networkManager(networkManager)
{

    Q_ASSERT_X(m_networkManager, "ServerPacketsParser::ServerPacketsParser(...)", "Invalid NetworkManagerInstance!");

    m_packetHandlerBase = m_networkManager->getPacketHandler();
    Q_ASSERT_X(m_packetHandlerBase, "ServerPacketsParser::ServerPacketsParser(...)", "Invalid PacketHandlerBase!");


//    heartbeatTimer = 0;

    localRUDPListeningAddress = m_networkManager->localRUDPListeningAddress();
    localRUDPListeningPort = m_networkManager->localRUDPListeningPort();
    //m_serverName = m_networkManager->hostName();
    m_serverName = QHostInfo::localHostName().toLower();

    localIPMCListeningAddress = m_networkManager->localIPMCListeningAddress();
    localIPMCListeningPort = m_networkManager->localIPMCListeningPort();



}

ServerPacketsParser::~ServerPacketsParser() {
    // TODO Auto-generated destructor stub

    QMutexLocker locker(&mutex);




}



void ServerPacketsParser::run(){

    QMutexLocker locker(&mutex);


//    QTimer processWaitingForReplyPacketsTimer;
//    processWaitingForReplyPacketsTimer.setSingleShot(false);
//    processWaitingForReplyPacketsTimer.setInterval(UDP_PACKET_WAITING_FOR_REPLY_TIMEOUT/2);
//    connect(&processWaitingForReplyPacketsTimer, SIGNAL(timeout()), this, SLOT(processWaitingForReplyPackets()));
//    connect(this, SIGNAL(signalAboutToQuit()), &processWaitingForReplyPacketsTimer, SLOT(stop()));
//    processWaitingForReplyPacketsTimer.start();

    while(!isAboutToQuit()){
        //QCoreApplication::processEvents();
        parseIncomingPackets();
        processOutgoingPackets();
        msleep(1);
    }

//    processWaitingForReplyPacketsTimer.stop();

    processOutgoingPackets();



}

void ServerPacketsParser::startparseIncomingPackets(){

    while(!isAboutToQuit()){
        //QCoreApplication::processEvents();
        parseIncomingPackets();
        msleep(10);
    }

}

void ServerPacketsParser::startprocessOutgoingPackets(){

//    QTimer processWaitingForReplyPacketsTimer;
//    processWaitingForReplyPacketsTimer.setSingleShot(false);
//    //processWaitingForReplyPacketsTimer.setInterval(UDP_PACKET_WAITING_FOR_REPLY_TIMEOUT);
//    processWaitingForReplyPacketsTimer.setInterval(UDP_PACKET_WAITING_FOR_REPLY_TIMEOUT/2);
//    connect(&processWaitingForReplyPacketsTimer, SIGNAL(timeout()), this, SLOT(processWaitingForReplyPackets()));
//    connect(this, SIGNAL(signalAboutToQuit()), &processWaitingForReplyPacketsTimer, SLOT(stop()));
//    processWaitingForReplyPacketsTimer.start();

    while(!isAboutToQuit()){
        //QCoreApplication::processEvents();
        processOutgoingPackets();
        msleep(10);
    }

//    processWaitingForReplyPacketsTimer.stop();

    processOutgoingPackets();

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

        quint16 peerRUDPListeningPort;
        in >> peerRUDPListeningPort;

        sendServerDeclarePacket(peerAddress, peerRUDPListeningPort );
        //emit signalClientLookForServerPacketReceived(peerAddress, peerPort, peerName);
//        qDebug()<<"~~ClientLookForServer--"<<" peerAddress:"<<peerAddress.toString()<<"   peerPort:"<<peerPort<<" peerRUDPListeningPort:"<<peerRUDPListeningPort;
    }
    break;
    //    case quint8(MS::ServerDeclare):
    //        break;
    case quint8(MS::ClientOnline):
    {
        QString peerRUDPListeningAddress;
        quint16 peerRUDPListeningPort;
        QString peerName;
        bool isAdmin;
        in >> peerRUDPListeningAddress >> peerRUDPListeningPort >> peerName >> isAdmin;
        //emit signalClientOnlineStatusChanged(peerRUDPListeningAddress, peerRUDPListeningPort, peerName, true, isAdmin);
        emit signalClientOnlineStatusChanged(peerAddress.toString(), peerRUDPListeningPort, peerName, true, isAdmin);

        qDebug()<<"~~ClientOnline--"<<" peerAddress:"<<peerAddress<<"   peerName:"<<peerName;
    }
    break;
    case quint8(MS::ClientOffline):
    {
        QString peerRUDPListeningAddress;
        quint16 peerRUDPListeningPort;
        QString peerName;
        bool isAdmin;
        in >> peerRUDPListeningAddress >> peerRUDPListeningPort >> peerName >> isAdmin;
        //emit signalClientOnlineStatusChanged(peerRUDPListeningAddress, peerRUDPListeningPort, peerName, false, isAdmin);
        emit signalClientOnlineStatusChanged(peerAddress.toString(), peerRUDPListeningPort, peerName, false, isAdmin);
        qDebug()<<"~~ClientOffline--"<<" peerAddress:"<<peerAddress.toString()<<"   peerName:"<<peerName;;
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
        bool usbsdEnabled = false, programesEnabled = false;
        in >> workgroupName >> networkInfo >> usersInfo >> osInfo >> usbsdEnabled >> programesEnabled >> admins >> clientVersion;
        emit signalClientResponseClientSummaryInfoPacketReceived(peerID, workgroupName, networkInfo, usersInfo, osInfo, usbsdEnabled, programesEnabled, admins, clientVersion);
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
        qDebug()<<"~~UpdateAdministratorPasswordFailed";
    }
    break;
    default:
        qWarning()<<"Unknown Packet Type: "<<packetType
                 //<<"    Serial Number: "<<packetSerialNumber
                <<"    From: "<<peerAddress.toString()
               <<":"<<peerPort;
        break;

    }



}




//void ServerPacketsParser::startHeartbeat(int interval){
//    if(NULL == heartbeatTimer){
//        heartbeatTimer = new QTimer();
//        heartbeatTimer->setSingleShot(false);
//        heartbeatTimer->setInterval(interval);
//        connect(heartbeatTimer, SIGNAL(timeout()), this, SLOT(heartbeat()));

//    }else{
//        heartbeatTimer->stop();
//        heartbeatTimer->setInterval(interval);

//    }

//    heartbeatTimer->start();


//}

//void ServerPacketsParser::stopHeartbeat(){

//    heartbeatTimer->stop();
//}

quint16 ServerPacketsParser::getLastReceivedPacketSN(const QString &peerID){
    quint16 lastpacketSN = 0;

    QList< QPair<quint16 /*Packet Serial Number*/, QDateTime/*Received Time*/> > list = m_receivedPacketsHash.values(peerID);
    if(list.isEmpty()){
        return lastpacketSN;
    }

    QDateTime lastpacketTime(QDate(1970, 1, 1));
    for(int i=0; i<list.size(); i++){
        QPair<quint16, QDateTime> pair = list.at(i);
        QDateTime time = pair.second;
        if(time.addSecs(UDP_PACKET_WAITING_FOR_REPLY_TIMEOUT) < QDateTime::currentDateTime()){
            m_receivedPacketsHash.remove(peerID, pair);
        }else{
            if(time > lastpacketTime){
                lastpacketTime = time;
                lastpacketSN = pair.first;
            }
        }
    }
    //    foreach ( QPair<quint16, QDateTime> pair, list) {
    //        QDateTime time = pair.second;
    //        if(time.addSecs(UDP_PACKET_WAITING_FOR_REPLY_TIMEOUT) < QDateTime::currentDateTime()){
    //            m_receivedPacketsHash.remove(peerID, pair);
    //            list.removeOne(pair);
    //        }else{
    //            if(time > lastpacketTime){
    //                lastpacketTime = time;
    //                lastpacketSN = pair.first;
    //            }
    //        }
    //    }

    //TODO:TX Rate

    return lastpacketSN;


}


























} //namespace HEHUI
