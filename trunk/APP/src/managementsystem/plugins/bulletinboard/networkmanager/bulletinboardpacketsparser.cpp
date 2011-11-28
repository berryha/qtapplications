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


BulletinBoardPacketsParser::BulletinBoardPacketsParser(UDTProtocol *udtProtocol, QObject *parent)
    :QObject(parent), m_udtProtocol(udtProtocol)
{

    Q_ASSERT_X(m_udtProtocol, "BulletinBoardPacketsParser::BulletinBoardPacketsParser(...)", "Invalid UDTProtocol!");

    connect(m_udtProtocol, SIGNAL(packetReceived(Packet*)), this, SLOT(parseIncomingPacketData(Packet*)));


    //serverAddress = QHostAddress::Null;
    localServerRUDPListeningPort = 0;
    //serverName = "";

    heartbeatTimer = 0;



    localRUDPListeningAddress = QHostAddress::Any;
    localRUDPListeningPort = 0;

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

void BulletinBoardPacketsParser::setLocalRUDPListeningAddress(const QHostAddress &address){

    this->localRUDPListeningAddress = address;
}
void BulletinBoardPacketsParser::setLocalRUDPListeningPort(quint16 port){

    this->localRUDPListeningPort = port;
}

void BulletinBoardPacketsParser::parseIncomingPacketData(Packet *packet){
    qDebug()<<"----BulletinBoardPacketsParser::parseIncomingPacketData(Packet *packet)";
    

    QByteArray packetData = packet->getPacketData();
    QDataStream in(&packetData, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_6);

    QString peerID = "";
    in >> peerID;

    quint8 packetType = packet->getPacketType();
    int socketID = packet->getSocketID();
    PacketHandlerBase::recylePacket(packet);

    switch(packetType){

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
              <<" Local Port:"<<localRUDPListeningPort;

        break;

    }

    qApp->processEvents();

}
























} //namespace HEHUI
