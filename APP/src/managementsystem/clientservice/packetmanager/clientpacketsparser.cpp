/*
 ****************************************************************************
 * clientpacketsparser.cpp
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



#include "clientpacketsparser.h"

#ifdef Q_CC_MSVC
#include <windows.h>
//#include "HHSharedWindowsManagement/hwindowsmanagement.h"
#define msleep(x) Sleep(x)
#endif

#ifdef Q_CC_GNU
#include <unistd.h>
#define msleep(x) usleep(x*1000)
#endif

//#include "HHSharedCore/hutilities.h"


namespace HEHUI {


ClientPacketsParser::ClientPacketsParser(ClientNetworkManager *networkManager, QObject *parent)
    :PacketsParserBase(networkManager, parent)
{

    Q_ASSERT_X(networkManager, "ClientPacketsParser::ClientPacketsParser(...)", "Invalid ClientNetworkManager!");

    m_packetHandlerBase = networkManager->getPacketHandler();
    Q_ASSERT_X(m_packetHandlerBase, "ClientPacketsParser::ClientPacketsParser(...)", "Invalid PacketHandlerBase!");


    serverAddress = QHostAddress::Null;
    serverRUDPListeningPort = 0;
    serverName = "";


    heartbeatTimer = 0;

    localIPMCListeningAddress = ClientNetworkManager::instance()->localIPMCListeningAddress();
    localIPMCListeningPort = ClientNetworkManager::instance()->localIPMCListeningPort();

    m_localRUDPListeningAddress = ClientNetworkManager::instance()->localRUDPListeningAddress();
    m_localRUDPListeningPort = ClientNetworkManager::instance()->localRUDPListeningPort();


    serverLastOnlineTime = QDateTime();

    m_localComputerName = QHostInfo::localHostName().toLower();
    



}

ClientPacketsParser::~ClientPacketsParser() {
    // TODO Auto-generated destructor stub

    qDebug()<<"ClientPacketsParser::~ClientPacketsParser() ";

    QMutexLocker locker(&mutex);

    if(heartbeatTimer){
        heartbeatTimer->stop();
    }
    delete heartbeatTimer;
    heartbeatTimer = 0;



}

void ClientPacketsParser::run(){

    QMutexLocker locker(&mutex);

//    QTimer processWaitingForReplyPacketsTimer;
//    processWaitingForReplyPacketsTimer.setSingleShot(false);
//    processWaitingForReplyPacketsTimer.setInterval(UDP_PACKET_WAITING_FOR_REPLY_TIMEOUT);
//    connect(&processWaitingForReplyPacketsTimer, SIGNAL(timeout()), this, SLOT(processWaitingForReplyPackets()));
//    connect(this, SIGNAL(signalAboutToQuit()), &processWaitingForReplyPacketsTimer, SLOT(stop()));
//    processWaitingForReplyPacketsTimer.start();

    while(1){

        QCoreApplication::processEvents();
        parseIncomingPackets();
        processOutgoingPackets();

        if(isAboutToQuit()){
            break;
        }
        msleep(500);
    }


//    processWaitingForReplyPacketsTimer.stop();

    processOutgoingPackets();


}

void ClientPacketsParser::startparseIncomingPackets(){

    while(!isAboutToQuit()){
        QCoreApplication::processEvents();
        parseIncomingPackets();
        msleep(500);
    }

}

void ClientPacketsParser::startprocessOutgoingPackets(){

//    QTimer processWaitingForReplyPacketsTimer;
//    processWaitingForReplyPacketsTimer.setSingleShot(false);
//    processWaitingForReplyPacketsTimer.setInterval(UDP_PACKET_WAITING_FOR_REPLY_TIMEOUT/2);
//    connect(&processWaitingForReplyPacketsTimer, SIGNAL(timeout()), this, SLOT(processWaitingForReplyPackets()));
//    connect(this, SIGNAL(signalAboutToQuit()), &processWaitingForReplyPacketsTimer, SLOT(stop()));
//    processWaitingForReplyPacketsTimer.start();

    while(!isAboutToQuit()){
        //QCoreApplication::processEvents();
        processOutgoingPackets();
        msleep(500);
    }

//    processWaitingForReplyPacketsTimer.stop();

    processOutgoingPackets();

}

void ClientPacketsParser::parseIncomingPacketData(Packet *packet){
    //    qDebug()<<"----ClientPacketsParser::parseIncomingPacketData(Packet *packet)";

    //        if((packet->getTransmissionProtocol() == TP_UDP)
    //            && (networkManager->isLocalAddress(packet->getPeerHostAddress()))
    //            && (packet->getPeerHostPort() == localIPMCListeningPort)){
    //            qDebug()<<"~~Packet is been discarded!";
    //            return;
    //        }else if((packet->getTransmissionProtocol() == TP_TCP)
    //            && (packet->getPeerHostAddress() == networkManager->localTCPListeningAddress())
    //            && (packet->getPeerHostPort() == networkManager->localTCPListeningPort())){
    //            qDebug()<<"~~Packet is been discarded!";
    //            return;
    //        }

    //    qDebug()<<"~~networkManager->localAddress():"<<networkManager->localTCPListeningAddress().toString();
    //    qDebug()<<"~~packet->getPeerHostAddress():"<<packet->getPeerHostAddress();

    QByteArray packetData = packet->getPacketData();
    QDataStream in(&packetData, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_6);

    QString peerName = "";
    in >> peerName;

    QHostAddress peerAddress = packet->getPeerHostAddress();
    quint16 peerPort = packet->getPeerHostPort();
//    quint16 packetSerialNumber = packet->getPacketSerialNumber();
    quint8 packetType = packet->getPacketType();
//    qDebug()<<"--ClientPacketsParser::parseIncomingPacketData(...) "<<" peerName:"<<peerName<<" peerAddress:"<<peerAddress<<" peerPort:"<<peerPort<<" packetSerialNumber:"<<packetSerialNumber<<" packetType:"<<packetType;

    switch(packetType){
//    case quint8(HEHUI::HeartbeatPacket):
//    {
//        emit signalHeartbeatPacketReceived(peerName);
//        qDebug()<<"~~HeartbeatPacket--"<<" computerName:"<<peerName;
//    }
//    break;
//    case quint8(HEHUI::ConfirmationOfReceiptPacket):
//    {
//        quint16 packetSerialNumber1 = 0, packetSerialNumber2 = 0;
//        in >> packetSerialNumber1 >> packetSerialNumber2;
//        m_packetHandlerBase->removeWaitingForReplyPacket(packetSerialNumber1, packetSerialNumber2);
//        emit signalConfirmationOfReceiptPacketReceived(packetSerialNumber1, packetSerialNumber2);
//        qDebug()<<"~~ConfirmationOfReceiptPacket:"<<packetSerialNumber1<<", "<<packetSerialNumber2;
//    }
//    break;
    case quint8(MS::ServerDeclare):
    {

        QString address = "";
        quint16 port = 0;
        QString version = "";
        in >> address >> port >> version;
        serverAddress = peerAddress;
        serverRUDPListeningPort = port;
        serverName = peerName;

        serverLastOnlineTime = QDateTime::currentDateTime();

//        startHeartbeat();

        emit signalServerDeclarePacketReceived(serverAddress.toString(), serverRUDPListeningPort, serverName, version);
        qDebug()<<"~~ServerDeclare"<<" serverAddress:"<<serverAddress<<" servername:"<<serverName <<" serverRUDPListeningPort:"<<serverRUDPListeningPort;
    }
    break;
    //    case quint8(MS::ClientOnline):
    //        break;
    //    case quint8(MS::ClientOffline):
    //        break;
    case quint8(MS::ServerOnline):
    {
        QString address = "";
        quint16 port = 0;
        in >> address >> port;
        serverAddress = peerAddress;
        serverRUDPListeningPort = port;
        serverName = peerName;

        serverLastOnlineTime = QDateTime::currentDateTime();

        emit signalServerOnlinePacketReceived(serverAddress, serverRUDPListeningPort, serverName);
        qDebug()<<"~~ServerOnline"<<" serverAddress:"<<serverAddress<<" servername:"<<serverName <<" serverRUDPListeningPort:"<<serverRUDPListeningPort;

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

//        stopHeartbeat();

        emit signalServerOfflinePacketReceived(serverAddress, serverRUDPListeningPort, serverName);
        qDebug()<<"~~ServerOffline";
    }
    break;
    case quint8(MS::ClientDetailedInfoRequested):
    {
        quint16 peerRUDPListeningPort = 0;
        QString computerName = "";
        bool rescan = false;
        in >> peerRUDPListeningPort >> computerName >> rescan;

        if(!computerName.isEmpty()){
            if(computerName.toLower() != m_localComputerName){
                return;
            }
        }

        emit signalClientDetailedInfoRequestedPacketReceived(computerName, rescan, peerAddress.toString(), peerRUDPListeningPort);
        qDebug()<<"~~ClientDetailedInfoRequested";
    }
    break;
    case quint8(MS::AdminRequestRemoteConsole):
    {

//        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerName);

        QString computerName = "", applicationPath = "", adminID = "";
        bool startProcess = true;
        in >> computerName >> applicationPath >> adminID >> startProcess;

        if(computerName.toLower() != m_localComputerName){
            return;
        }

        emit signalAdminRequestRemoteConsolePacketReceived(computerName, applicationPath, adminID, startProcess, peerAddress.toString(), peerPort);

        qDebug()<<"~~AdminRequestRemoteConsole";
    }
    break;
    //    case quint8(MS::ClientResponseRemoteConsole):
    //        break;
    case quint8(MS::RemoteConsoleCMDFromAdmin):
    {

//        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerName);

        QString computerName = "", command = "";
        in >> computerName >> command;

        if(computerName.toLower() != m_localComputerName){
            return;
        }

        emit signalRemoteConsoleCMDFromServerPacketReceived(computerName, command, peerAddress.toString(), peerPort);
        qDebug()<<"~~RemoteConsoleCMDFromAdmin";
    }
    break;
    //    case quint8(MS::RemoteConsoleCMDResultFromClient):
    //        break;
    case quint8(MS::ServerRequestClientSummaryInfo):
    {
//        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerName);

        QString groupName = "", computerName = "", userName = "";
        in >> groupName >> computerName >> userName;

        if(computerName.toLower() != m_localComputerName){
            return;
        }

        emit signalServerRequestClientSummaryInfoPacketReceived(groupName, computerName, userName);
        qDebug()<<"~~ServerRequestClientInfo";
    }
    break;
    //    case quint8(MS::ClientResponseClientInfo):
    //        break;
    //    case quint8(MS::ClientRequestSoftwareVersion):
    //        break;
    case quint8(MS::ServerResponseSoftwareVersion):
    {
//        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerName);

        QString softwareName, version;
        in >> softwareName >> version;
        emit signalServerResponseSoftwareVersionPacketReceived(softwareName, softwareName);
        qDebug()<<"~~ServerResponseSoftwareVersion";
    }
    break;
    case quint8(MS::ServerAnnouncement):
    {
//        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerName);

        QString groupName = "", computerName = "", announcement = "", adminName = "";
        quint32 announcementID = 0;
        quint8 mustRead = 1;
        in >> groupName >> computerName >> announcementID >> announcement >> adminName >> mustRead ;

        if(!computerName.trimmed().isEmpty()){
            if(!m_localComputerName.contains(computerName, Qt::CaseInsensitive)){
                return;
            }
        }

        emit signalServerAnnouncementPacketReceived(groupName, computerName, announcementID, announcement, adminName, (mustRead == quint8(0))?false:true);

        qDebug()<<"~~ServerAnnouncement"<<"groupName:"<<groupName<<"computerName:"<<computerName<<"announcement:"<<announcement<<"mustRead:"<<mustRead;
        
        //            foreach (QString userName, localUsersHash.keys()) {
        //                if(userName.trimmed().isEmpty()){
        //                    continue;
        //                }
        //                sendServerAnnouncementPacket(QHostAddress::LocalHost, localUsersHash.value(userName), adminName, announcement);

        //            }

        //            qWarning()<<"~~ServerAnnouncement";

    }
    break;
    case quint8(MS::Update):
        emit signalUpdateClientSoftwarePacketReceived();
        qDebug()<<"~~Update";
        break;
    case quint8(MS::SetupUSBSD):
    {

//        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerName);

        QString computerName = "", users = "", adminName = "";
        bool enable = false;
        bool temporarilyAllowed = true;
        in >> computerName >> users >> enable >> temporarilyAllowed >> adminName;

        if(computerName.toLower() != m_localComputerName){
            return;
        }

        emit signalSetupUSBSDPacketReceived(computerName, users, enable, temporarilyAllowed, adminName, peerAddress.toString(), peerPort);
        qDebug()<<"~~SetupUSBSD";
    }
    break;
    case quint8(MS::SetupProgrames):
    {

//        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerName);

        QString computerName = "", users = "", adminName = "";
        bool enable = false;
        bool temporarilyAllowed = true;
        in >> computerName >> users >> enable >> temporarilyAllowed >> adminName;

        if(computerName.toLower() != m_localComputerName){
            return;
        }

        emit signalSetupProgramesPacketReceived(computerName, users, enable, temporarilyAllowed, adminName, peerAddress.toString(), peerPort);
        qDebug()<<"~~SetupProgrames";
    }
    break;
    case quint8(MS::ShowAdmin):
    {
//        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerName);

        QString computerName = "", users = "";
        bool show = false;
        in >> computerName >> users >> show;

        if(computerName.toLower() != m_localComputerName){
            return;
        }

        emit signalShowAdminPacketReceived(computerName, users, show);
        qDebug()<<"~~ShowAdmin";
    }
    break;
    case quint8(MS::ModifyAdminGroupUser):
    {

//        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerName);

        QString computerName = "", userName = "", adminName = "";
        bool addToAdminGroup = false;
        in >> computerName >> userName >> addToAdminGroup >> adminName;

        if(computerName.toLower() != m_localComputerName){
            return;
        }

        emit signalModifyAdminGroupUserPacketReceived(computerName, userName, addToAdminGroup, adminName, peerAddress.toString(), peerPort);
        qDebug()<<"~~ModifyAdminGroupUser";
    }
    break;
    case quint8(MS::AdminRequestConnectionToClient):
    {
//        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerName);

        QString computerName = "", users = "" ;
        in >> computerName >> users ;

        if(computerName.toLower() != m_localComputerName){
            return;
        }

        emit signalAdminRequestConnectionToClientPacketReceived(packet->getPeerHostAddress().toString(), packet->getPeerHostPort(), computerName, users);
        qDebug()<<"~~AdminRequestConnectionToClient";
    }
    break;
    case quint8(MS::AdminSearchClient):
    {
        //sendConfirmationOfReceiptPacket(packet->getPeerHostAddress(), packet->getPeerHostPort(), packet->getPacketSerialNumber());

        quint16 peerRUDPListeningPort = 0;
        QString computerName = "", userName = "", workgroup = "", macAddress = "", ipAddress = "", osVersion = "", adminName = "" ;
        in >> peerRUDPListeningPort >> computerName >> userName >> workgroup >> macAddress >> ipAddress >> osVersion >> adminName;

        if(!computerName.isEmpty()){
            if(computerName.toLower() != m_localComputerName){
                return;
            }
        }

        emit signalAdminSearchClientPacketReceived(peerAddress.toString(), peerRUDPListeningPort, computerName, userName, workgroup, macAddress, ipAddress, osVersion, adminName);

        qDebug()<<"~~AdminSearchClient";
    }
    break;


    case quint8(MS::AdminRequestRemoteAssistance):
    {

        QString computerName = "", adminName = "";
        in >> computerName >> adminName;
        //emit signalAdminRequestRemoteAssistancePacketReceived(peerName, adminName, peerAddress.toString(), peerPort);


        if(computerName.toLower() != m_localComputerName){
            return;
        }

        foreach (QString userName, localUsersHash.keys()) {
            if(userName.trimmed().isEmpty()){
                continue;
            }
            sendRequestRemoteAssistancePacket(QHostAddress::LocalHost, localUsersHash.value(userName), peerAddress.toString(), peerPort, adminName);

        }

        qDebug()<<"~~AdminRequestRemoteAssistance";
    }
    break;
    case quint8(MS::UpdateMSWUserPassword):
    {

//        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerName);

        QString workgroup = "", adminName = "";
        in >> workgroup >> adminName;
        emit signalAdminRequestUpdateMSUserPasswordPacketReceived(workgroup, adminName, peerAddress.toString(), peerPort);
        qDebug()<<"~~UpdateMSWUserPassword";
    }
    break;

    case quint8(MS::InformUserNewPassword):
    {

//        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerName);

        QString workgroup = "", adminName = "";
        in >> workgroup >> adminName;

        emit signalAdminRequestInformUserNewPasswordPacketReceived(workgroup, adminName, peerAddress.toString(), peerPort);

        qDebug()<<"~~InformUserNewPassword";


    }
    break;

    case quint8(MS::ClientLookForLocalServiceServer):
    {
        quint16 peerRUDPListeningPort = 0;
        QString userName = "", computerName = "";
        in >> peerRUDPListeningPort >> userName >> computerName;
        
        if(computerName.toLower() != m_localComputerName){
            return;
        }

        sendLocalServiceServerDeclarePacket(peerAddress, peerRUDPListeningPort);
        localUsersHash.insert(userName, peerRUDPListeningPort);

        qDebug()<<"~~ClientLookForLocalServiceServer"<<" userName:"<<userName<<" computerName:"<<computerName;
        
    }
    break;

    case quint8(MS::UserOnline):
    {
        QString userName = "", computerName = "";
        in >> userName >> computerName;
        
        if(computerName.toLower() != m_localComputerName){
            return;
        }
        localUsersHash.insert(userName, peerPort);
        
        qDebug()<<"~~UserOnline"<<"Name:"<<userName<<" Port:"<<peerPort;
        
        
    }
    break;

    case quint8(MS::UserOffline):
    {
        QString userName = "", computerName = "";
        in >> userName >> computerName;
        
        if(computerName.toLower() != m_localComputerName){
            return;
        }
        localUsersHash.insert(userName, 0);
        
        qDebug()<<"~~UserOffline";
        
    }
    break;






    //    case quint8(MS::RemoteAssistanceRejectedByUser):
    //    {
    //        QHostAddress userAddress = packet->getPeerHostAddress();
    //        quint16 userPort = packet->getPeerHostPort();
    //        sendConfirmationOfReceiptPacket(userAddress, userPort, packet->getPacketSerialNumber());

    //        QString userName = "", computerName = "";
    //        in >> userName >> computerName;

    //        if(computerName.toLower() != m_localComputerName){
    //            return;
    //        }
    //        //sendLocalServiceServerDeclarePacket(userAddress, userPort, m_localComputerName);
    //        localUsersHash.insert(userName, 0);

    //    }
    //        break;


    //    case quint8(MS::AdminLookForClient):
    //        {
    //            //sendConfirmationOfReceiptPacket(packet->getPeerHostAddress(), packet->getPeerHostPort(), packet->getPacketSerialNumber());

    //            QString clientComputerName, workgroupName, macAddress, usersInfo, osInfo, clientVersion;
    //            in >> computerName >> users ;
    //            in >> clientComputerName >> workgroupName >> macAddress >> usersInfo >> osInfo >> usbsdEnabled >> programesEnabled >> clientVersion ;

    //            emit signalAdminLookForClientPacketReceived(packet->getPeerHostAddress().toString(), packet->getPeerHostPort(), computerName, users);
    //            qDebug()<<"~~VerifyClientInfo:"<<"packet->getPeerHostAddress():"<<packet->getPeerHostAddress().toString()<<" packet->getPeerHostPort():"<<packet->getPeerHostPort();
    //        }
    //        break;


    default:
//        qWarning()<<"Unknown Packet Type:"<<packetType//<<" Serial Number:"<<packetSerialNumber
//                <<" From:"<<peerAddress.toString()<<":"<<peerPort
//                <<" (ClientPacketsParser)";

        break;

    }



}

QHostAddress ClientPacketsParser::getServerAddress() const{
    return serverAddress;
}

//void ClientPacketsParser::startHeartbeat(int interval){
//    if(NULL == heartbeatTimer){
//        heartbeatTimer = new QTimer();
//        heartbeatTimer->setSingleShot(false);
//        heartbeatTimer->setInterval(interval);
//        connect(heartbeatTimer, SIGNAL(timeout()), this, SLOT(sendHeartbeatPacket()));

//    }else{
//        heartbeatTimer->stop();
//        heartbeatTimer->setInterval(interval);
//    }

//    heartbeatTimer->start();


//}

//void ClientPacketsParser::stopHeartbeat(){
//    if(heartbeatTimer){
//        heartbeatTimer->stop();
//    }

//}


quint16 ClientPacketsParser::getLastReceivedPacketSN(const QString &peerID){
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
