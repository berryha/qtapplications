/*
 ****************************************************************************
 * bulletinboardpacketsparser.cpp.h
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

#ifndef BULLETINBOARDPACKETSPARSER_H_
#define BULLETINBOARDPACKETSPARSER_H_



#include <QHostAddress>
#include <QDebug>

#include "../../sharedms/global_shared.h"
#include "networkmanagerinstance.h"


#include "HHSharedCore/hcryptography.h"
#include "HHSharedNetwork/hpacketparserbase.h"
#include "HHSharedCore/hutilities.h"


namespace HEHUI {


class BulletinBoardPacketsParser : public PacketsParserBase{
    Q_OBJECT

public:
    BulletinBoardPacketsParser(NetworkManagerInstance *networkManager, QObject *parent = 0);
    virtual ~BulletinBoardPacketsParser();

    void setLocalRUDPListeningAddress(const QHostAddress &address = QHostAddress::Any);
    void setLocalRUDPListeningPort(quint16 port);


    //	void parseIncomingPackets();

    virtual void parseIncomingPacketData(Packet *packet);
    virtual void run();




//    void sendConfirmationOfReceiptPacket(const QHostAddress peerAddress, quint16 peerPort, quint16 packetSerialNumber, const QString &peerID){
//        qDebug()<<"----sendConfirmationOfReceiptPacket(...)";

//        //UDPPacket *packet = new UDPPacket(peerAddress.toString(), peerPort, localUDPListeningAddress.toString(), localUDPListeningPort);
//        Packet *packet = m_packetHandlerBase->getPacket(peerAddress, peerPort);

//        packet->setPacketType(quint8(HEHUI::ConfirmationOfReceiptPacket));
//        QByteArray ba;
//        QDataStream out(&ba, QIODevice::WriteOnly);
//        out.setVersion(QDataStream::Qt_4_6);
//        out << m_localID << packetSerialNumber << getLastReceivedPacketSN(peerID);
//        packet->setPacketData(ba);
//        m_packetHandlerBase->appendOutgoingPacket(packet);

//        m_receivedPacketsHash.insert(peerID, qMakePair(packetSerialNumber, QDateTime::currentDateTime()));


////        return packet->getPacketSerialNumber();
//    }

    void sendClientLookForLocalServiceServerPacket(){
        qDebug()<<"----sendClientLookForLocalServiceServerPacket(...)";

        //Packet *packet = m_packetHandlerBase->getPacket(QHostAddress::LocalHost, ipmcListeningPort, localRUDPListeningAddress, localRUDPListeningPort);
        Packet *packet = m_packetHandlerBase->getPacket(QHostAddress::LocalHost, RUDP_LISTENING_PORT, localRUDPListeningAddress, localRUDPListeningPort);

        packet->setPacketType(quint8(MS::ClientLookForLocalServiceServer));
        packet->setTransmissionProtocol(TP_UDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES*10));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << localRUDPListeningPort << m_userName << m_localComputerName;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);


    }

    void sendUserOnlinePacket(){
        //qWarning()<<"----sendUserOnlinePacket(...)";

        Packet *packet = m_packetHandlerBase->getPacket(QHostAddress::LocalHost, localServerRUDPListeningPort, localRUDPListeningAddress, localRUDPListeningPort);

        packet->setPacketType(quint8(MS::UserOnline));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << m_userName << m_localComputerName;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

    }

    void sendUserOfflinePacket(){
        qDebug()<<"----sendUserOfflinePacket(...)";

        Packet *packet = m_packetHandlerBase->getPacket(QHostAddress::LocalHost, localServerRUDPListeningPort, localRUDPListeningAddress, localRUDPListeningPort);

        packet->setPacketType(quint8(MS::UserOffline));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << m_userName << m_localComputerName;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);


    }

    void sendUserResponseRemoteAssistancePacket(const QString &adminAddress, quint16 adminPort, bool accept){

        Packet *packet = m_packetHandlerBase->getPacket(QHostAddress(adminAddress), adminPort, localRUDPListeningAddress, localRUDPListeningPort);
        
        packet->setPacketType(quint8(MS::UserResponseRemoteAssistance));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << m_userName << m_localComputerName << accept;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

    }

    void sendNewPasswordRetrevedByUserPacket(const QString &adminAddress, quint16 adminPort){

        Packet *packet = m_packetHandlerBase->getPacket(QHostAddress(adminAddress), adminPort, localRUDPListeningAddress, localRUDPListeningPort);
        
        packet->setPacketType(quint8(MS::NewPasswordRetrevedByUser));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << m_userName << m_localComputerName;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

    }

    
    

    /////////////////////////////////////////////////////








signals:
    void  signalHeartbeatPacketReceived(const QString &computerName);
    void  signalConfirmationOfReceiptPacketReceived(quint16 packetSerialNumber1, quint16 packetSerialNumber2);

    void signalLocalServiceServerDeclarePacketReceived(const QString &localComputerName);

    void signalAdminRequestRemoteAssistancePacketReceived(const QString &adminAddress, quint16 adminPort, const QString &adminName );
    void signalAdminInformUserNewPasswordPacketReceived(const QString &adminAddress, quint16 adminPort, const QString &adminName, const QString &oldPassword, const QString &newPassword);
    void signalAnnouncementPacketReceived(const QString &adminName, quint32 announcementID, const QString &announcement);






public slots:
    //HeartbeatPacket: PacketType+ComputerName+IP
//    void startHeartbeat(int interval = HEARTBEAT_TIMER_INTERVAL);
//    void heartbeat();
//    void confirmPacketReceipt(quint16 packetSerialNumber);

private:
    quint16 getLastReceivedPacketSN(const QString &peerID);

private:
    //QHostAddress serverAddress;
    quint16 localServerRUDPListeningPort;
    //QString serverName;

    //QHostAddress ipmcGroupAddress;
    quint16 ipmcListeningPort;

    QHostAddress localRUDPListeningAddress;
    quint16 localRUDPListeningPort;

    QTimer *heartbeatTimer;

    PacketHandlerBase *m_packetHandlerBase;
    //NetworkManagerInstance *networkManager;

    QMutex mutex;

    QString m_userName;
    QString m_localComputerName;
    QString m_localID;


    QMultiHash<QString /*Peer ID*/, QPair<quint16 /*Packet Serial Number*/, QDateTime/*Received Time*/> > m_receivedPacketsHash;




};

}

#endif /* BULLETINBOARDPACKETSPARSER_H_ */
