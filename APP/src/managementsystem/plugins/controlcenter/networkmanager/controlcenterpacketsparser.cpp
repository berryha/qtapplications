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


ControlCenterPacketsParser::ControlCenterPacketsParser(NetworkManagerInstance *networkManager, QObject *parent)
    :PacketsParserBase(networkManager, parent)
{

    Q_ASSERT_X(networkManager, "ControlCenterPacketsParser::ControlCenterPacketsParser(...)", "Invalid NetworkManagerInstance!");

    m_packetHandlerBase = networkManager->getPacketHandler();
    Q_ASSERT_X(m_packetHandlerBase, "ControlCenterPacketsParser::ControlCenterPacketsParser(...)", "Invalid PacketHandlerBase!");

    serverAddress = QHostAddress::Null;
    serverRUDPListeningPort = 0;
    serverName = "";

    heartbeatTimer = 0;
    //    processWaitingForReplyPacketsTimer = 0;
    //    processWaitingForReplyPacketsTimer = new QTimer();

    //packetHandlerBase = new PacketHandlerBase(this);

    networkManager = NetworkManagerInstance::instance();

    ipmcGroupAddress = networkManager->ipMCGroupAddress();
    ipmcListeningPort = networkManager->localIPMCListeningPort();

    //    localUDPListeningAddress = networkManager->localUDPListeningAddress();
    //    localUDPListeningPort = networkManager->localUDPListeningPort();
//    localRUDPListeningAddress = QHostAddress::Any;
//    localRUDPListeningPort = 0;
    localRUDPListeningAddress = networkManager->localRUDPListeningAddress();
    localRUDPListeningPort = networkManager->localRUDPListeningPort();


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

    //    if(processWaitingForReplyPacketsTimer){
    //        processWaitingForReplyPacketsTimer->stop();
    //    }
    //    delete processWaitingForReplyPacketsTimer;
    //    processWaitingForReplyPacketsTimer = 0;




}

//void ControlCenterPacketsParser::setLocalUDPListeningAddress(const QHostAddress &address){

//    this->localUDPListeningAddress = address;
//}
//void ControlCenterPacketsParser::setLocalUDPListeningPort(quint16 port){

//    this->localUDPListeningPort = port;
//}

void ControlCenterPacketsParser::run(){
    QMutexLocker locker(&mutex);


//    QTimer processWaitingForReplyPacketsTimer;
//    processWaitingForReplyPacketsTimer.setSingleShot(false);
//    processWaitingForReplyPacketsTimer.setInterval(UDP_PACKET_WAITING_FOR_REPLY_TIMEOUT + 2000);
//    connect(&processWaitingForReplyPacketsTimer, SIGNAL(timeout()), this, SLOT(processWaitingForReplyPackets()));
//    connect(this, SIGNAL(signalAboutToQuit()), &processWaitingForReplyPacketsTimer, SLOT(stop()));
//    processWaitingForReplyPacketsTimer.start();

    while(!isAboutToQuit()){
        parseIncomingPackets();
        processOutgoingPackets();
        //processWaitingForReplyPackets();
        msleep(500);
        qApp->processEvents();
    }

//    processWaitingForReplyPacketsTimer.stop();

    processOutgoingPackets();


}

void ControlCenterPacketsParser::parseIncomingPacketData(Packet *packet){
    qDebug()<<"----ControlCenterPacketsParser::parseIncomingPacketData(Packet *packet)";

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
    //qDebug()<<"~~packet->getPeerHostAddress():"<<packet->getPeerHostAddress().toString();

    QByteArray packetData = packet->getPacketData();
    QDataStream in(&packetData, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_6);

    QString peerName = "";
    in >> peerName;

    QHostAddress peerAddress = packet->getPeerHostAddress();
    quint16 peerPort = packet->getPeerHostPort();
//    quint16 packetSerialNumber = packet->getPacketSerialNumber();
    quint8 packetType = packet->getPacketType();
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
        serverRUDPListeningPort = port;
        serverName = peerName;
        emit signalServerDeclarePacketReceived(serverAddress.toString(), serverRUDPListeningPort, serverName, version, serverInstanceID);

        qDebug()<<"~~ServerDeclare"<<" serverAddress:"<<serverAddress<<" servername:"<<peerName <<" serverRUDPListeningPort:"<<serverRUDPListeningPort;
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
        serverRUDPListeningPort = port;
        serverName = peerName;
        emit signalServerOnlinePacketReceived(serverAddress, serverRUDPListeningPort, serverName);
    }
    break;
    case quint8(MS::ServerOffline):
    {
        QString address = "";
        quint16 port = 0;
        in >> address >> port;
        serverAddress = QHostAddress::Null;
        serverRUDPListeningPort = 0;
        serverName = peerName;
        emit signalServerOfflinePacketReceived(serverAddress, serverRUDPListeningPort, serverName);
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
        emit signalClientResponseAdminConnectionResultPacketReceived(peerName, result, message);
        qWarning()<<"~~ClientResponseAdminConnectionResult";
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

//void PacketsParser::fillOutgoingPacketData(Packet *packet){
//    QByteArray packetData = packet->getPacketData();
//    QDataStream out(&packetData, QIODevice::ReadOnly);
//    out.setVersion(QDataStream::Qt_4_6);

//    switch(packet->getPacketType()){
//    case quint8(HeartbeatPacket):
//        {
//        QString computerName;
//        out >> computerName;
//        emit signalHeartbeatPacketReceived(computerName);
//        }
//        break;
//    default:
//        break;

//    }

//}

//void PacketsParser::parseIncomingPackets(){


//    while(1){
//        Packet *packet = PacketHandlerBase::takeIncomingPacket();
//            if (!packet) {
//                    //continue;
//                    break;
//            }
//            if (!packet->isValid()) {
//                    delete packet;
//                    packet = 0;
//                    continue;
//            }

//            qWarning()<<"PacketType:"<<packet->getPacketType();
//            qWarning()<<"SerialNumber:"<<packet->getPacketSerialNumber();

//            TransmissionProtocol transmissionProtocol = packet->getTransmissionProtocol();
//            quint8 packetType = packet->getPacketType();

//            if(transmissionProtocol == TP_UDP){
//                if(packetType == quint8(HeartbeatPacket)){

//                }
//            }else{

//            }

//    }


//}


//void ControlCenterPacketsParser::startHeartbeat(int interval){
//    if(NULL == heartbeatTimer){
//        heartbeatTimer = new QTimer(this);
//        heartbeatTimer->setSingleShot(false);
//        heartbeatTimer->setInterval(interval);
//        connect(heartbeatTimer, SIGNAL(timeout()), this, SLOT(heartbeat()));
//    }else{
//        heartbeatTimer->stop();
//        heartbeatTimer->setInterval(interval);

//    }

//    heartbeatTimer->start();


//}

//void ControlCenterPacketsParser::heartbeat(){

//    qDebug()<<"----sendHeartbeatPacket()";

//    static QString computerName = "";
//    if(computerName.isEmpty()){
//        computerName = QHostInfo::localHostName().toLower() ;
//    }

//    if(serverAddress.isNull()){
//        serverAddress = QHostAddress::Broadcast;
//    }

//    Packet *packet = new Packet(serverAddress.toString(), serverTCPListeningPort, localUDPListeningAddress.toString(), localUDPListeningPort);
//    packet->setPacketType(quint8(HEHUI::HeartbeatPacket));
//    QByteArray ba;
//    QDataStream out(&ba, QIODevice::WriteOnly);
//    out.setVersion(QDataStream::Qt_4_6);
//    out << computerName;
//    packet->setPacketData(ba);
//    m_packetHandlerBase->appendOutgoingPacket(packet);


//}

//void ControlCenterPacketsParser::confirmPacketReceipt(quint16 packetSerialNumber){

//}



quint16 ControlCenterPacketsParser::getLastReceivedPacketSN(const QString &peerID){
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


    //TODO:TX Rate

    return lastpacketSN;


}






















} //namespace HEHUI
