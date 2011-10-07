/*
 ****************************************************************************
 * serverpacketsparser.h
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

#ifndef SERVERPACKETSPARSER_H_
#define SERVERPACKETSPARSER_H_



#include <QHostAddress>
#include <QHostInfo>
#include <QDebug>

#include "../../sharedms/global_shared.h"
#include "../networkmanagerinstance.h"

//#include "../../../shared/network/packethandler/packetparserbase.h"

#include "HHSharedCore/hcryptography.h"
#include "HHSharedNetwork/hpacketparserbase.h"



namespace HEHUI {


class ServerPacketsParser : public PacketsParserBase{
    Q_OBJECT
public:
    ServerPacketsParser(NetworkManagerInstance *networkManager, QObject *parent = 0);
    virtual ~ServerPacketsParser();


    void parseIncomingPacketData(Packet *packet);
    void run();

    void startparseIncomingPackets();
    void startprocessOutgoingPackets();



public slots:


//    quint16 sendHeartbeatPacket(){
//        qDebug()<<"----sendHeartbeatPacket()";

//        if(serverAddress.isNull()){
//            serverAddress = QHostAddress::Broadcast;
//        }

//        UDPPacket *packet = new UDPPacket(QString(IP_MULTICAST_GROUP_ADDRESS), quint16(IP_MULTICAST_GROUP_PORT), localIPMCListeningAddress.toString(), localIPMCListeningPort);
//        packet->setPacketType(quint8(HEHUI::HeartbeatPacket));
//        QByteArray ba;
//        QDataStream out(&ba, QIODevice::WriteOnly);
//        out.setVersion(QDataStream::Qt_4_6);
//        out << m_serverName;
//        packet->setPacketData(ba);
//        m_packetHandlerBase->appendOutgoingPacket(packet);

//        return packet->getPacketSerialNumber();

//    }


//    quint16 sendConfirmationOfReceiptPacket(const QHostAddress peerAddress, quint16 peerPort, quint16 packetSerialNumber, const QString &peerID){
//        qDebug()<<"----sendConfirmationOfReceiptPacket(...)";

//        //UDPPacket *packet = new UDPPacket(peerAddress.toString(), peerPort, localIPMCListeningAddress.toString(), localIPMCListeningPort);
//        //UDPPacket *packet = packetHandlerBase->getUDPPacket(peerAddress, peerPort, localIPMCListeningAddress, localIPMCListeningPort);
//        Packet *packet = m_packetHandlerBase->getPacket(peerAddress, peerPort, localIPMCListeningAddress, localIPMCListeningPort);

//        packet->setPacketType(quint8(HEHUI::ConfirmationOfReceiptPacket));
//        QByteArray ba;
//        QDataStream out(&ba, QIODevice::WriteOnly);
//        out.setVersion(QDataStream::Qt_4_6);
//        out << m_serverName << packetSerialNumber << getLastReceivedPacketSN(peerID);
//        packet->setPacketData(ba);
//        m_packetHandlerBase->appendOutgoingPacket(packet);

//        m_receivedPacketsHash.insert(peerID, qMakePair(packetSerialNumber, QDateTime::currentDateTime()));

//        return packet->getPacketSerialNumber();
//    }

    void sendServerDeclarePacket(const QHostAddress peerAddress, quint16 peerPort){

        Packet *packet = 0;

        QHostAddress targetAddress = peerAddress;
        quint16 targetPort = peerPort;
//        if(targetAddress == QHostAddress(IP_MULTICAST_GROUP_ADDRESS) || targetAddress == QHostAddress::Broadcast || targetAddress.isNull()){
//            targetAddress = QHostAddress(IP_MULTICAST_GROUP_ADDRESS);
//            targetPort = quint16(IP_MULTICAST_GROUP_PORT);

//            packet = m_packetHandlerBase->getPacket(targetAddress, targetPort, localIPMCListeningAddress, localIPMCListeningPort );
//            packet->setTransmissionProtocol(TP_UDP);
//        }else{
//            packet = m_packetHandlerBase->getPacket(targetAddress, targetPort, localRUDPListeningAddress, localRUDPListeningPort);
//            packet->setTransmissionProtocol(TP_RUDP);
//        }
        if(targetAddress == QHostAddress(IP_MULTICAST_GROUP_ADDRESS) || targetAddress == QHostAddress::Broadcast || targetAddress.isNull()){
            targetAddress = QHostAddress(IP_MULTICAST_GROUP_ADDRESS);
        }
        targetPort = quint16(IP_MULTICAST_GROUP_PORT);
        packet = m_packetHandlerBase->getPacket(targetAddress, targetPort, localIPMCListeningAddress, localIPMCListeningPort );
        packet->setTransmissionProtocol(TP_UDP);


        packet->setPacketType(quint8(MS::ServerDeclare));
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_serverName << localRUDPListeningAddress.toString() << localRUDPListeningPort << QString(APP_VERSION);
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);


    }

    void sendServerOnlinePacket(){
        qDebug()<<"----sendServerOnlinePacket(...)";

        //Packet *packet = m_packetHandlerBase->getPacket(QHostAddress(IP_MULTICAST_GROUP_ADDRESS), quint16(IP_MULTICAST_GROUP_PORT), serverAddress, localRUDPListeningPort);
        Packet *packet = m_packetHandlerBase->getPacket(QHostAddress(IP_MULTICAST_GROUP_ADDRESS), quint16(IP_MULTICAST_GROUP_PORT), localIPMCListeningAddress, localIPMCListeningPort);

        packet->setPacketType(quint8(MS::ServerOnline));
        packet->setTransmissionProtocol(TP_UDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_serverName << localRUDPListeningAddress.toString() << localRUDPListeningPort;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

    }

    void sendServerOfflinePacket(){
        qDebug()<<"----sendServerOfflinePacket(...)";

        //Packet *packet = m_packetHandlerBase->getPacket(QHostAddress(IP_MULTICAST_GROUP_ADDRESS), quint16(IP_MULTICAST_GROUP_PORT), serverAddress, localRUDPListeningPort);
        Packet *packet = m_packetHandlerBase->getPacket(QHostAddress(IP_MULTICAST_GROUP_ADDRESS), quint16(IP_MULTICAST_GROUP_PORT), localIPMCListeningAddress, localIPMCListeningPort);

        packet->setPacketType(quint8(MS::ServerOffline));
        packet->setTransmissionProtocol(TP_UDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_serverName << localRUDPListeningAddress.toString() << localRUDPListeningPort;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);


    }


    void sendServerRequestClientSummaryInfoPacket(const QString &groupName, const QString &computerName, const QString &userName, const QString &clientAddress = QString(IP_MULTICAST_GROUP_ADDRESS), quint16 clientPort = quint16(IP_MULTICAST_GROUP_PORT)){
        qDebug()<<"----sendServerRequestClientInfoPacket(...)";

        Packet *packet = 0;

        QHostAddress targetAddress = QHostAddress(clientAddress);
        quint16 targetPort = clientPort;
        if(targetAddress == QHostAddress(IP_MULTICAST_GROUP_ADDRESS) || targetAddress == QHostAddress::Broadcast || targetAddress.isNull()){
            targetAddress = QHostAddress(IP_MULTICAST_GROUP_ADDRESS);
            targetPort = quint16(IP_MULTICAST_GROUP_PORT);

            packet = m_packetHandlerBase->getPacket(targetAddress, targetPort, localIPMCListeningAddress, localIPMCListeningPort);
            packet->setTransmissionProtocol(TP_UDP);
        }else{
            packet = m_packetHandlerBase->getPacket(targetAddress, targetPort, localRUDPListeningAddress, localRUDPListeningPort);
            packet->setTransmissionProtocol(TP_RUDP);
        }


        packet->setPacketType(quint8(MS::ServerRequestClientSummaryInfo));
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_serverName << groupName << computerName << userName;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);


    }

    void sendRequestClientDetailedInfoPacket(const QString &peerAddress = QString(IP_MULTICAST_GROUP_ADDRESS), quint16 clientPort = quint16(IP_MULTICAST_GROUP_PORT), const QString &computerName = "", bool rescan = true){

        Packet *packet = 0;

        QHostAddress targetAddress = QHostAddress(peerAddress);
        quint16 targetPort = clientPort;
        if(targetAddress == QHostAddress(IP_MULTICAST_GROUP_ADDRESS) || targetAddress == QHostAddress::Broadcast || targetAddress.isNull()){
            targetAddress = QHostAddress(IP_MULTICAST_GROUP_ADDRESS);
            targetPort = quint16(IP_MULTICAST_GROUP_PORT);

            packet = m_packetHandlerBase->getPacket(targetAddress, targetPort, localIPMCListeningAddress, localIPMCListeningPort);
            packet->setTransmissionProtocol(TP_UDP);
        }else{
            packet = m_packetHandlerBase->getPacket(targetAddress, targetPort, localRUDPListeningAddress, localRUDPListeningPort);
            packet->setTransmissionProtocol(TP_RUDP);
        }

        packet->setPacketType(quint8(MS::ClientDetailedInfoRequested));
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);

        out << m_serverName << localRUDPListeningPort << computerName << rescan;

        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

    }





    void sendServerResponseSoftwareVersionPacket(const QHostAddress &peerAddress, quint16 peerPort, const QString &softwareName, const QString &version){
        qDebug()<<"----sendServerResponseSoftwareVersionPacket(...)";

        Packet *packet = m_packetHandlerBase->getPacket(peerAddress, peerPort, localRUDPListeningAddress, localRUDPListeningPort);

        packet->setPacketType(quint8(MS::ServerResponseSoftwareVersion));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_serverName << softwareName << version;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);


    }

    void sendUpdateClientSoftwarePacket(){
        qDebug()<<"----sendUpdateClientSoftwarePacket(...)";

        //Packet *packet = m_packetHandlerBase->getPacket(QHostAddress(IP_MULTICAST_GROUP_ADDRESS), quint16(IP_MULTICAST_GROUP_PORT), localRUDPListeningAddress, localRUDPListeningPort);
        Packet *packet = m_packetHandlerBase->getPacket(QHostAddress(IP_MULTICAST_GROUP_ADDRESS), quint16(IP_MULTICAST_GROUP_PORT), localIPMCListeningAddress, localIPMCListeningPort);

        packet->setPacketType(quint8(MS::Update));
        packet->setTransmissionProtocol(TP_UDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_serverName;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);


    }



private slots:
//    void startHeartbeat(int interval = HEARTBEAT_TIMER_INTERVAL);
//    void stopHeartbeat();



signals:
    void  signalHeartbeatPacketReceived(const QString &clientAddress, const QString &computerName);
    void  signalConfirmationOfReceiptPacketReceived(quint16 packetSerialNumber, quint16 packetSerialNumbe2);

    void signalClientLookForServerPacketReceived(const QHostAddress &clientAddress, quint16 clientTCPListeningPort, const QString &clientName);

    void signalClientOnlinePacketReceived(const QHostAddress &clientAddress, quint16 clientPort, const QString &clientName);
    void signalClientOfflinePacketReceived(const QHostAddress &clientAddress, quint16 clientPort, const QString &clientName);

    void signalClientResponseClientSummaryInfoPacketReceived(const QString &computerName, const QString &workgroupName, const QString &networkInfo, const QString &usersInfo, const QString &osInfo, bool usbsdEnabled, bool programesEnabled, const QString &admins, const QString &clientVersion);
    void signalClientResponseClientDetailedInfoPacketReceived(const QString &computerName, const QString &clientInfo);


    void signalClientRequestSoftwareVersionPacketReceived(const QString &softwareName);

    void signalClientLogReceived(const QString &computerName, const QString &users, const QString &clientAddress, quint8 logType, const QString &log, const QString &clientTime);

    void signalClientOnlineStatusChanged(const QString &clientRUDPListeningAddress, quint16 clientRUDPListeningPort, const QString &clientName, bool online, bool isAdmin);


private:
    quint16 getLastReceivedPacketSN(const QString &peerID);

private:
    QHostAddress localRUDPListeningAddress;
    quint16 localRUDPListeningPort;
    QString m_serverName;

    QHostAddress localIPMCListeningAddress;
    quint16 localIPMCListeningPort;

//    QTimer *heartbeatTimer;
    //        QTimer *processWaitingForReplyPacketsTimer;

    PacketHandlerBase *m_packetHandlerBase;
    NetworkManagerInstance *m_networkManager;

    QMutex mutex;

    //QHash<QString/*IP:Port*/, QList<quint32> > receivedPackets;
    QStringList receivedPackets; //IP:Port:SerialNumber


    QMultiHash<QString /*Peer ID*/, QPair<quint16 /*Packet Serial Number*/, QDateTime/*Received Time*/> > m_receivedPacketsHash;



};

}

#endif /* SERVERPACKETSPARSER_H_ */
