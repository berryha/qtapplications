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


    heartbeatTimer = 0;

    serverAddress = m_networkManager->localTCPListeningAddress();
    serverTCPListeningPort = m_networkManager->localTCPListeningPort();
    //m_serverName = m_networkManager->hostName();
    m_serverName = QHostInfo::localHostName().toLower();

    localIPMCListeningAddress = m_networkManager->localIPMCListeningAddress();
    localIPMCListeningPort = m_networkManager->localIPMCListeningPort();



}

ServerPacketsParser::~ServerPacketsParser() {
    // TODO Auto-generated destructor stub

    QMutexLocker locker(&mutex);

    if(heartbeatTimer){
        heartbeatTimer->stop();
    }
    delete heartbeatTimer;
    heartbeatTimer = 0;

    //    if(processWaitingForReplyPacketsTimer){
    //        processWaitingForReplyPacketsTimer->stop();
    //    }
    //    delete processWaitingForReplyPacketsTimer;
    //    processWaitingForReplyPacketsTimer = 0;




}



void ServerPacketsParser::run(){

    QMutexLocker locker(&mutex);

    //    processWaitingForReplyPacketsTimer = new QTimer();
    //    processWaitingForReplyPacketsTimer->setSingleShot(false);
    //    processWaitingForReplyPacketsTimer->setInterval(UDP_PACKET_WAITING_FOR_REPLY_TIMEOUT + 100);
    //    connect(processWaitingForReplyPacketsTimer, SIGNAL(timeout()), this, SLOT(processWaitingForReplyPackets()));
    //    processWaitingForReplyPacketsTimer->start();

    QTimer processWaitingForReplyPacketsTimer;
    processWaitingForReplyPacketsTimer.setSingleShot(false);
    processWaitingForReplyPacketsTimer.setInterval(UDP_PACKET_WAITING_FOR_REPLY_TIMEOUT/2);
    connect(&processWaitingForReplyPacketsTimer, SIGNAL(timeout()), this, SLOT(processWaitingForReplyPackets()));
    connect(this, SIGNAL(signalAboutToQuit()), &processWaitingForReplyPacketsTimer, SLOT(stop()));
    processWaitingForReplyPacketsTimer.start();

    while(!isAboutToQuit()){
        //QCoreApplication::processEvents();
        parseIncomingPackets();
        processOutgoingPackets();
        msleep(50);
    }

    processWaitingForReplyPacketsTimer.stop();

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

    QTimer processWaitingForReplyPacketsTimer;
    processWaitingForReplyPacketsTimer.setSingleShot(false);
    //processWaitingForReplyPacketsTimer.setInterval(UDP_PACKET_WAITING_FOR_REPLY_TIMEOUT);
    processWaitingForReplyPacketsTimer.setInterval(UDP_PACKET_WAITING_FOR_REPLY_TIMEOUT/2);
    connect(&processWaitingForReplyPacketsTimer, SIGNAL(timeout()), this, SLOT(processWaitingForReplyPackets()));
    connect(this, SIGNAL(signalAboutToQuit()), &processWaitingForReplyPacketsTimer, SLOT(stop()));
    processWaitingForReplyPacketsTimer.start();

    while(!isAboutToQuit()){
        //QCoreApplication::processEvents();
        processOutgoingPackets();
        msleep(10);
    }

    processWaitingForReplyPacketsTimer.stop();

    processOutgoingPackets();

}


void ServerPacketsParser::parseIncomingPacketData(Packet *packet){

    //    qDebug()<<"----ServerPacketsParser::parseIncomingPacketData(Packet *packet)";

    //    if((packet->getTransmissionProtocol() == TP_UDP)
    //        && (networkManager->isLocalAddress(packet->getPeerHostAddress()))
    //        && (packet->getPeerHostPort() == localIPMCListeningPort)){
    //        qDebug()<<"~~Packet is been discarded!";
    //        return;
    //    }else if((packet->getTransmissionProtocol() == TP_TCP)
    //        && (packet->getPeerHostAddress() == networkManager->localTCPListeningAddress())
    //        && (packet->getPeerHostPort() == networkManager->localTCPListeningPort())){
    //        qDebug()<<"~~Packet is been discarded!";
    //        return;
    //    }

    //qDebug()<<"~~networkManager->localAddress():"<<networkManager->localTCPListeningAddress().toString();
    //qDebug()<<"~~localIPMCListeningAddress.toString():"<<localIPMCListeningAddress.toString();
    //qWarning()<<"~~packet->getPeerHostAddress():"<<packet->getPeerHostAddress().toString();

    //    static int dp = 0;
    //    QHostAddress add = packet->getPeerHostAddress();
    //    quint16 p = packet->getPeerHostPort();
    //    quint16 sn = packet->getPacketSerialNumber();
    
    //    QString str = add.toString() + ":" + QString::number(p) + ":" + QString::number(sn);
    //    if(receivedPackets.contains(str)){
    //        sendConfirmationOfReceiptPacket(add, p, sn);

    //        qCritical()<<"!!!!! Packet already received! ADD.:"<<add.toString()<<" Port:"<<p<<" SN.:"<<sn;
    //        qCritical()<<"Duplicate Packet:"<<++dp;
    //        return;
    //    }else{
    //        receivedPackets.append(str);
    //        qCritical()<<"Total Packet:"<<receivedPackets.size();

    //    }
    
    QByteArray packetData = packet->getPacketData();
    QDataStream in(&packetData, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_6);

    QString peerID = "";
    in >> peerID;

    QHostAddress peerAddress = packet->getPeerHostAddress();
    quint16 peerPort = packet->getPeerHostPort();
    quint16 packetSerialNumber = packet->getPacketSerialNumber();
    quint8 packetType = packet->getPacketType();
    qDebug()<<"--ServerPacketsParser::parseIncomingPacketData(...) "<<" peerID:"<<peerID<<" peerAddress:"<<peerAddress<<" peerPort:"<<peerPort<<" packetSerialNumber:"<<packetSerialNumber<<" packetType:"<<packetType;

    switch(packetType){
    case quint8(HEHUI::HeartbeatPacket):
    {
        //QString computerName;
        //in >> computerName;
        emit signalHeartbeatPacketReceived(packet->getPeerHostAddress().toString(), peerID);
        qDebug()<<"~~HeartbeatPacket--"<<" peerID:"<<peerID;
    }
    break;
    case quint8(HEHUI::ConfirmationOfReceiptPacket):
    {
        quint16 packetSerialNumber1 = 0, packetSerialNumber2 = 0;
        in >> packetSerialNumber1 >> packetSerialNumber2;
        m_packetHandlerBase->removeWaitingForReplyPacket(packetSerialNumber1, packetSerialNumber2);
        emit signalConfirmationOfReceiptPacketReceived(packetSerialNumber1, packetSerialNumber2);
        qDebug()<<"~~ConfirmationOfReceiptPacket--"<<packetSerialNumber1<<" "<<packetSerialNumber2;
    }
    break;
    case quint8(MS::ClientLookForServer):
    {
        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerID);

        sendServerDeclarePacket(peerAddress, peerPort, serverAddress, serverTCPListeningPort);
        //emit signalClientLookForServerPacketReceived(peerAddress, peerPort, peerName);
        qDebug()<<"~~ClientLookForServer--"<<" peerAddress:"<<peerAddress<<"   peerPort:"<<peerPort;
    }
    break;
    //    case quint8(MS::ServerDeclare):
    //        break;
    case quint8(MS::ClientOnline):
    {
        QString peerTCPListeningAddress;
        quint16 peerTCPListeningPort;
        QString peerName;
        bool isAdmin;
        in >> peerTCPListeningAddress >> peerTCPListeningPort >> peerName >> isAdmin;
        emit signalClientOnlineStatusChanged(peerTCPListeningAddress, peerTCPListeningPort, peerName, true, isAdmin);
        //emit signalClientOnlinePacketReceived(QHostAddress(peerAddress), peerPort, peerName);
        qDebug()<<"~~ClientOnline--"<<" peerAddress:"<<peerTCPListeningAddress<<"   peerName:"<<peerName;
    }
    break;
    case quint8(MS::ClientOffline):
    {
        QString peerTCPListeningAddress;
        quint16 peerTCPListeningPort;
        QString peerName;
        bool isAdmin;
        in >> peerTCPListeningAddress >> peerTCPListeningPort >> peerName >> isAdmin;
        emit signalClientOnlineStatusChanged(peerTCPListeningAddress, peerTCPListeningPort, peerName, false, isAdmin);
        //emit signalClientOfflinePacketReceived(QHostAddress(peerAddress), peerPort, peerName);
        qDebug()<<"~~ClientOffline--"<<" peerAddress:"<<peerTCPListeningAddress<<"   peerName:"<<peerName;;
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
        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerID);


        //            QHostAddress add = packet->getPeerHostAddress();
        //            quint16 p = packet->getPeerHostPort();
        //            quint16 sn = packet->getPacketSerialNumber();
        //            sendConfirmationOfReceiptPacket(add, p, sn);

        //            QString str = add.toString() + ":" + QString::number(p) + ":" + QString::number(sn);
        //            if(receivedPackets.contains(str)){

        //                qCritical()<<"!!!!! CSI Packet already received! ADD.:"<<add.toString()<<" Port:"<<p<<" SN.:"<<sn<<" Time:"<<QDateTime::currentDateTime().toString("hh:mm:ss:zzz");
        //                qCritical()<<"Duplicate CSI Packet:"<<++dp;
        //                return;
        //            }else{
        //                receivedPackets.append(str);
        //                qCritical()<<"CSI Packet received! ADD.:"<<add.toString()<<" Port:"<<p<<" SN.:"<<sn<<" Time:"<<QDateTime::currentDateTime().toString("hh:mm:ss:zzz");
        //                qCritical()<<"Total CSI Packet:"<<receivedPackets.size();

        //            }



        QString workgroupName = "", networkInfo = "", usersInfo = "", osInfo = "", admins = "", clientVersion = "";
        bool usbsdEnabled = false, programesEnabled = false;
        in >> workgroupName >> networkInfo >> usersInfo >> osInfo >> usbsdEnabled >> programesEnabled >> admins >> clientVersion;
        emit signalClientResponseClientSummaryInfoPacketReceived(peerID, workgroupName, networkInfo, usersInfo, osInfo, usbsdEnabled, programesEnabled, admins, clientVersion);
        qDebug()<<"~~ClientResponseClientInfo";
    }
    break;

    case quint8(MS::ClientResponseClientDetailedInfo):
    {
        //sendConfirmationOfReceiptPacket(packet->getPeerHostAddress(), quint16(IP_MULTICAST_GROUP_PORT), packet->getPacketSerialNumber(), peerID);
        sendConfirmationOfReceiptPacket(peerAddress, quint16(IP_MULTICAST_GROUP_PORT), packetSerialNumber, peerID);

        QString systemInfo = "";
        in >> systemInfo;
        emit signalClientResponseClientDetailedInfoPacketReceived(peerID, systemInfo);
        qDebug()<<"~~ClientResponseClientDetailedInfo";

    }
    break;

    case quint8(MS::ClientRequestSoftwareVersion):
    {
        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerID);

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
        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerID);

        QString users = "", log = "", clientTime = "";
        quint8 logType = 0;
        in >> users >> logType >> log >> clientTime;
        emit signalClientLogReceived(peerID, users, packet->getPeerHostAddress().toString(), logType, log, clientTime);
        qDebug()<<"~~UpdateAdministratorPasswordFailed";
    }
    break;
    default:
        qWarning()<<"Unknown Packet Type: "<<packetType
                 <<"    Serial Number: "<<packetSerialNumber
                <<"    From: "<<peerAddress.toString()
               <<":"<<peerPort;
        break;

    }



}




void ServerPacketsParser::startHeartbeat(int interval){
    if(NULL == heartbeatTimer){
        heartbeatTimer = new QTimer();
        heartbeatTimer->setSingleShot(false);
        heartbeatTimer->setInterval(interval);
        connect(heartbeatTimer, SIGNAL(timeout()), this, SLOT(heartbeat()));

    }else{
        heartbeatTimer->stop();
        heartbeatTimer->setInterval(interval);

    }

    heartbeatTimer->start();


}

void ServerPacketsParser::stopHeartbeat(){

    heartbeatTimer->stop();
}

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
