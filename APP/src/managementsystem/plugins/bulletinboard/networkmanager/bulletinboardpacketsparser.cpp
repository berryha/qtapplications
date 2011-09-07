/*
 ****************************************************************************
 * bulletinboardpacketsparser.cpp
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

#include "bulletinboardpacketsparser.h"

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


BulletinBoardPacketsParser::BulletinBoardPacketsParser(NetworkManagerInstance *networkManager, QObject *parent)
    :PacketsParserBase(networkManager, parent)
{

    Q_ASSERT_X(networkManager, "BulletinBoardPacketsParser::BulletinBoardPacketsParser(...)", "Invalid NetworkManagerInstance!");

    m_packetHandlerBase = networkManager->getPacketHandler();
    Q_ASSERT_X(m_packetHandlerBase, "BulletinBoardPacketsParser::BulletinBoardPacketsParser(...)", "Invalid PacketHandlerBase!");


    serverAddress = QHostAddress::Null;
    serverTCPListeningPort = 0;
    serverName = "";

    heartbeatTimer = 0;
    //    processWaitingForReplyPacketsTimer = 0;
    //    processWaitingForReplyPacketsTimer = new QTimer();

    //m_packetHandlerBase = new PacketHandlerBase(this);

    networkManager = NetworkManagerInstance::instance();

    //ipmcGroupAddress = networkManager->ipMCGroupAddress();
    ipmcListeningPort = networkManager->localIPMCListeningPort();

    //    localUDPListeningAddress = networkManager->localUDPListeningAddress();
    //    localUDPListeningPort = networkManager->localUDPListeningPort();

    localUDPListeningAddress = QHostAddress::Any;
    localUDPListeningPort = 0;

    m_userName = Utilities::currentUserNameOfOS();
    m_localComputerName = QHostInfo::localHostName().toLower();
    m_localID = m_localComputerName + "/BulletinBoard";



    //    emit signalAnnouncementPacketReceived("ADMIN", "TEST!");
    
}

BulletinBoardPacketsParser::~BulletinBoardPacketsParser() {
    // TODO Auto-generated destructor stub
    qDebug()<<"~BulletinBoardPacketsParser()";

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

void BulletinBoardPacketsParser::setLocalUDPListeningAddress(const QHostAddress &address){

    this->localUDPListeningAddress = address;
}
void BulletinBoardPacketsParser::setLocalUDPListeningPort(quint16 port){

    this->localUDPListeningPort = port;
}


void BulletinBoardPacketsParser::run(){
    QMutexLocker locker(&mutex);


    //    QTimer *processWaitingForReplyPacketsTimer = new QTimer();
    //    processWaitingForReplyPacketsTimer->setSingleShot(false);
    //    processWaitingForReplyPacketsTimer->setInterval(UDP_PACKET_WAITING_FOR_REPLY_TIMEOUT + 500);
    //    connect(processWaitingForReplyPacketsTimer, SIGNAL(timeout()), this, SLOT(processWaitingForReplyPackets()));

    //    connect(this, SIGNAL(signalAboutToQuit()), processWaitingForReplyPacketsTimer, SLOT(stop()));
    //    connect(this, SIGNAL(signalAboutToQuit()), processWaitingForReplyPacketsTimer, SLOT(deleteLater()));
    //    connect(processWaitingForReplyPacketsTimer, SIGNAL(destroyed(QObject *)), this, SLOT(destroyed(QObject *)));

    //    processWaitingForReplyPacketsTimer->start();

    QTimer processWaitingForReplyPacketsTimer;
    processWaitingForReplyPacketsTimer.setSingleShot(false);
    processWaitingForReplyPacketsTimer.setInterval(UDP_PACKET_WAITING_FOR_REPLY_TIMEOUT + 2000);
    connect(&processWaitingForReplyPacketsTimer, SIGNAL(timeout()), this, SLOT(processWaitingForReplyPackets()));
    connect(this, SIGNAL(signalAboutToQuit()), &processWaitingForReplyPacketsTimer, SLOT(stop()));
    processWaitingForReplyPacketsTimer.start();

    while(!isAboutToQuit()){
        parseIncomingPackets();
        processOutgoingPackets();
        //processWaitingForReplyPackets();
        msleep(500);
        qApp->processEvents();
    }

    processWaitingForReplyPacketsTimer.stop();

    processOutgoingPackets();



    //    delete processWaitingForReplyPacketsTimer;
    //    processWaitingForReplyPacketsTimer = 0;

}

void BulletinBoardPacketsParser::parseIncomingPacketData(Packet *packet){
    qDebug()<<"----BulletinBoardPacketsParser::parseIncomingPacketData(Packet *packet)";
    
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

    QString peerID = "";
    in >> peerID;

//    quint16 packetSerialNumber = packet->getPacketSerialNumber();
    quint8 packetType = packet->getPacketType();
//    qDebug()<<"--BulletinBoardPacketsParser::parseIncomingPacketData(...) "<<" peerID:"<<peerID<<" packetSerialNumber:"<<packetSerialNumber<<" packetType:"<<packetType;

    switch(packetType){
//    case quint8(HEHUI::HeartbeatPacket):
//    {
//        QString computerName;
//        in >> computerName;
//        emit signalHeartbeatPacketReceived(computerName);
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
    case quint8(MS::LocalServiceServerDeclare):
    {
//        sendConfirmationOfReceiptPacket(QHostAddress::LocalHost, ipmcListeningPort, packetSerialNumber, peerID);
        
        //QString localComputerName = "";
        //in >> localComputerName;

        emit signalLocalServiceServerDeclarePacketReceived(peerID);
        
    }
    break;

    case quint8(MS::AdminRequestRemoteAssistance):
    {
//        sendConfirmationOfReceiptPacket(QHostAddress::LocalHost, ipmcListeningPort, packetSerialNumber, peerID);
        
        QString adminAddress = "", adminName = "";
        quint16 adminPort = 0;

        in >> adminAddress >> adminPort >> adminName; 
        emit signalAdminRequestRemoteAssistancePacketReceived(adminAddress, adminPort, adminName);

        qDebug()<<"~~AdminRequestRemoteAssistance";
        
    }
    break;

    case quint8(MS::InformUserNewPassword):
    {

//        sendConfirmationOfReceiptPacket(QHostAddress::LocalHost, ipmcListeningPort, packetSerialNumber, peerID);
        
        QString adminAddress = "", adminName = "",  oldPassword = "",  newPassword = "";
        quint16 adminPort = 0;
        in >> adminAddress >> adminPort >> adminName >> oldPassword >> newPassword;
        emit signalAdminInformUserNewPasswordPacketReceived(adminAddress, adminPort, adminName, oldPassword, newPassword);
        
        qDebug()<<"~~InformUserNewPassword";

    }
    break;
    
    case quint8(MS::ServerAnnouncement):
    {
//        sendConfirmationOfReceiptPacket(QHostAddress::LocalHost, ipmcListeningPort, packetSerialNumber, peerID);
        
        QString adminName = "", serverAnnouncement = "";
        quint32 announcementID = 0;
        in >> adminName >> announcementID >> serverAnnouncement;
        emit signalAnnouncementPacketReceived(adminName, announcementID, serverAnnouncement);
        //qDebug()<<"~~ServerAnnouncement"<<" Admin:"<<adminName<<" Msg:"<<serverAnnouncement<<" SN:"<<packet->getPacketSerialNumber();

    }
    break;


    default:
        qWarning()<<"BulletinBoardPacketsParser! Unknown Packet Type:"<<packetType
//                 <<" Serial Number:"<<packetSerialNumber
                <<" From:"<<packet->getPeerHostAddress().toString()
               <<":"<<packet->getPeerHostPort()
              <<" Local Port:"<<localUDPListeningPort;

        break;

    }

    qApp->processEvents();

}



//void BulletinBoardPacketsParser::startHeartbeat(int interval){
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

//void BulletinBoardPacketsParser::heartbeat(){

//    static QString computerName = "";
//    if(computerName.isEmpty()){
//        computerName = QHostInfo::localHostName().toLower() ;
//    }

//    if(serverAddress.isNull()){
//        serverAddress = QHostAddress::Broadcast;
//    }

//    //UDPPacket *packet = new UDPPacket(serverAddress.toString(), serverTCPListeningPort, localUDPListeningAddress.toString(), localUDPListeningPort);
//    Packet *packet = m_packetHandlerBase->getPacket(QHostAddress::LocalHost, ipmcListeningPort, localUDPListeningAddress, localUDPListeningPort);

//    packet->setPacketType(quint8(MS::UserHeartbeat));
//    QByteArray ba;
//    QDataStream out(&ba, QIODevice::WriteOnly);
//    out.setVersion(QDataStream::Qt_4_6);
//    out << computerName;
//    packet->setPacketData(ba);
//    m_packetHandlerBase->appendOutgoingPacket(packet);


//}

//void BulletinBoardPacketsParser::confirmPacketReceipt(quint16 packetSerialNumber){

//}


quint16 BulletinBoardPacketsParser::getLastReceivedPacketSN(const QString &peerID){
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
