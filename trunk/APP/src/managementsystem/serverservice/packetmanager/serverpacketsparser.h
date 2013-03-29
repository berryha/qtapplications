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


#include <QDataStream>
#include <QHostAddress>
#include <QHostInfo>
#include <QDebug>

#include "../../sharedms/global_shared.h"
#include "../resourcesmanagerinstance.h"

#include "HHSharedCore/hcryptography.h"



namespace HEHUI {


class ServerPacketsParser : public QObject{
    Q_OBJECT
public:
    ServerPacketsParser(ResourcesManagerInstance *manager, QObject *parent = 0);
    virtual ~ServerPacketsParser();


public slots:

    void parseIncomingPacketData(Packet *packet);


    bool sendServerDeclarePacket(const QHostAddress peerAddress, quint16 peerPort){

        qsrand(QDateTime::currentDateTime().toTime_t());
        static int serverInstanceID = qrand();
        //qDebug()<<"Server Instance ID:"<<serverInstanceID;

        Packet *packet = PacketHandlerBase::getPacket();

        packet->setPacketType(quint8(MS::ServerDeclare));
        packet->setTransmissionProtocol(TP_UDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_serverName << localUDTListeningAddress.toString() << localUDTListeningPort << m_localTCPServerListeningPort << QString(APP_VERSION) << serverInstanceID;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_udpServer->sendDatagram(ba, peerAddress, peerPort);

    }

    bool sendServerOnlinePacket(const QString &targetAddress = QString(IP_MULTICAST_GROUP_ADDRESS), quint16 targetPort = quint16(IP_MULTICAST_GROUP_PORT)){
        qDebug()<<"----sendServerOnlinePacket(...)";

        Packet *packet = PacketHandlerBase::getPacket();

        packet->setPacketType(quint8(MS::ServerOnline));
        packet->setTransmissionProtocol(TP_UDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_serverName << localUDTListeningAddress.toString() << localUDTListeningPort;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_udpServer->sendDatagram(ba, QHostAddress(targetAddress), targetPort);

    }

    bool sendServerOfflinePacket(const QString &targetAddress = QString(IP_MULTICAST_GROUP_ADDRESS), quint16 targetPort = quint16(IP_MULTICAST_GROUP_PORT)){
        qDebug()<<"----sendServerOfflinePacket(...)";

        Packet *packet = PacketHandlerBase::getPacket();

        packet->setPacketType(quint8(MS::ServerOffline));
        packet->setTransmissionProtocol(TP_UDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_serverName << localUDTListeningAddress.toString() << localUDTListeningPort;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_udpServer->sendDatagram(ba, QHostAddress(targetAddress), targetPort);

    }


    bool sendServerRequestClientSummaryInfoPacket(const QString &groupName, const QString &computerName, const QString &userName, const QString &clientAddress = QString(IP_MULTICAST_GROUP_ADDRESS), quint16 clientPort = quint16(IP_MULTICAST_GROUP_PORT)){
        qDebug()<<"----sendServerRequestClientInfoPacket(...)";

        QHostAddress targetAddress = QHostAddress(clientAddress);
        Packet *packet = PacketHandlerBase::getPacket();
        packet->setTransmissionProtocol(TP_UDP);

        packet->setPacketType(quint8(MS::ServerRequestClientSummaryInfo));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_serverName << groupName << computerName << userName;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_udpServer->sendDatagram(ba, targetAddress, clientPort);

    }

    bool sendServerRequestClientSummaryInfoPacket(int socketID, const QString &groupName, const QString &computerName, const QString &userName){
        qDebug()<<"----sendServerRequestClientInfoPacket(...)";

        Packet *packet = PacketHandlerBase::getPacket(socketID);
        packet->setTransmissionProtocol(TP_UDT);

        packet->setPacketType(quint8(MS::ServerRequestClientSummaryInfo));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_serverName << groupName << computerName << userName;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_rtp->sendReliableData(socketID, &ba);

    }

    bool sendRequestClientDetailedInfoPacket(const QString &peerAddress = QString(IP_MULTICAST_GROUP_ADDRESS), quint16 clientPort = quint16(IP_MULTICAST_GROUP_PORT), const QString &computerName = "", bool rescan = true){

        QHostAddress targetAddress = QHostAddress(peerAddress);
        Packet *packet = PacketHandlerBase::getPacket();
        packet->setTransmissionProtocol(TP_UDP);

        packet->setPacketType(quint8(MS::ClientDetailedInfoRequested));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_serverName << localUDTListeningPort << computerName << rescan;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_udpServer->sendDatagram(ba, targetAddress, clientPort);

    }

    bool sendRequestClientDetailedInfoPacket(int socketID, const QString &computerName = "", bool rescan = true){

        Packet *packet = PacketHandlerBase::getPacket(socketID);
        packet->setTransmissionProtocol(TP_UDT);

        packet->setPacketType(quint8(MS::ClientDetailedInfoRequested));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_serverName << localUDTListeningPort << computerName << rescan;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_rtp->sendReliableData(socketID, &ba);
    }


    bool sendServerResponseSoftwareVersionPacket(int socketID, const QString &softwareName, const QString &version){
        qDebug()<<"----sendServerResponseSoftwareVersionPacket(...)";

        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::ServerResponseSoftwareVersion));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_serverName << softwareName << version;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_rtp->sendReliableData(socketID, &ba);

    }

    bool sendUpdateClientSoftwarePacket(const QString &targetAddress = QString(IP_MULTICAST_GROUP_ADDRESS), quint16 targetPort = quint16(IP_MULTICAST_GROUP_PORT)){
        qDebug()<<"----sendUpdateClientSoftwarePacket(...)";

        Packet *packet = PacketHandlerBase::getPacket();

        packet->setPacketType(quint8(MS::Update));
        packet->setTransmissionProtocol(TP_UDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_serverName;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_udpServer->sendDatagram(ba, QHostAddress(targetAddress), targetPort);

    }



private slots:


signals:
    //void  signalHeartbeatPacketReceived(const QString &clientAddress, const QString &computerName);
    //void  signalConfirmationOfReceiptPacketReceived(quint16 packetSerialNumber, quint16 packetSerialNumbe2);

    void signalClientLookForServerPacketReceived(const QHostAddress &clientAddress, quint16 clientTCPListeningPort, const QString &clientName);
    void signalClientDeclarePacketReceived(int socketID, const QString &clientName, bool isAdmin);

    //void signalClientOnlinePacketReceived(const QHostAddress &clientAddress, quint16 clientPort, const QString &clientName);
    //void signalClientOfflinePacketReceived(const QHostAddress &clientAddress, quint16 clientPort, const QString &clientName);

    void signalClientResponseClientSummaryInfoPacketReceived(const QString &computerName, const QString &workgroupName, const QString &networkInfo, const QString &usersInfo, const QString &osInfo, bool usbsdEnabled, bool programesEnabled, const QString &admins, bool isJoinedToDomain, const QString &clientVersion);
    void signalClientResponseClientDetailedInfoPacketReceived(const QString &computerName, const QString &clientInfo);


    void signalClientRequestSoftwareVersionPacketReceived(const QString &softwareName);

    void signalClientLogReceived(const QString &computerName, const QString &users, const QString &clientAddress, quint8 logType, const QString &log, const QString &clientTime);

    void signalClientOnlineStatusChanged(int socketID, const QString &clientName, bool online);

    void signalAdminOnlineStatusChanged(int socketID, const QString &clientName, const QString &adminName, bool online);

private:

private:
    QHostAddress localUDTListeningAddress;
    quint16 localUDTListeningPort;
    quint16 m_localTCPServerListeningPort;

    QString m_serverName;

    quint16 localIPMCListeningPort;


    ResourcesManagerInstance *m_resourcesManager;
    UDPServer *m_udpServer;

    RTP *m_rtp;
    UDTProtocol *m_udtProtocol;
    TCPServer *m_tcpServer;


};

}

#endif /* SERVERPACKETSPARSER_H_ */
