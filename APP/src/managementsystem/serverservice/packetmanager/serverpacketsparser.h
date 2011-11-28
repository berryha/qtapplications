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
#include "../../sharedms/resourcesmanager.h"

#include "HHSharedCore/hcryptography.h"



namespace HEHUI {


class ServerPacketsParser : public QObject{
    Q_OBJECT
public:
    ServerPacketsParser(UDPServer *udpServer, UDTProtocol *udtProtocol, QObject *parent = 0);
    virtual ~ServerPacketsParser();


public slots:

    void parseIncomingPacketData(Packet *packet);


    bool sendServerDeclarePacket(const QHostAddress peerAddress, quint16 peerPort){

        qsrand(QDateTime::currentDateTime().toTime_t());
        static int serverInstanceID = qrand();
        qWarning()<<"Server Instance ID:"<<serverInstanceID;

        Packet *packet = PacketHandlerBase::getPacket();

        packet->setPacketType(quint8(MS::ServerDeclare));
        packet->setTransmissionProtocol(TP_UDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_serverName << localUDTListeningAddress.toString() << localUDTListeningPort << QString(APP_VERSION) << serverInstanceID;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_udpServer->sendUDPDatagram(peerAddress, peerPort, ba);

    }

    bool sendServerOnlinePacket(){
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

        return m_udpServer->sendUDPDatagram(QHostAddress(IP_MULTICAST_GROUP_ADDRESS), quint16(IP_MULTICAST_GROUP_PORT), ba);

    }

    bool sendServerOfflinePacket(){
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

        return m_udpServer->sendUDPDatagram(QHostAddress(IP_MULTICAST_GROUP_ADDRESS), quint16(IP_MULTICAST_GROUP_PORT), ba);

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

        return m_udpServer->sendUDPDatagram(targetAddress, clientPort, ba);

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

        return m_udtProtocol->sendUDTMessageData(socketID, &ba);

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

        return m_udpServer->sendUDPDatagram(targetAddress, clientPort, ba);

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

        return m_udtProtocol->sendUDTMessageData(socketID, &ba);
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

        return m_udtProtocol->sendUDTMessageData(socketID, &ba);

    }

    bool sendUpdateClientSoftwarePacket(){
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

        return m_udpServer->sendUDPDatagram(QHostAddress(IP_MULTICAST_GROUP_ADDRESS), quint16(IP_MULTICAST_GROUP_PORT), ba);

    }



private slots:


signals:
    //void  signalHeartbeatPacketReceived(const QString &clientAddress, const QString &computerName);
    //void  signalConfirmationOfReceiptPacketReceived(quint16 packetSerialNumber, quint16 packetSerialNumbe2);

    void signalClientLookForServerPacketReceived(const QHostAddress &clientAddress, quint16 clientTCPListeningPort, const QString &clientName);
    void signalClientDeclarePacketReceived(int socketID, const QString &clientName, bool isAdmin);

    //void signalClientOnlinePacketReceived(const QHostAddress &clientAddress, quint16 clientPort, const QString &clientName);
    //void signalClientOfflinePacketReceived(const QHostAddress &clientAddress, quint16 clientPort, const QString &clientName);

    void signalClientResponseClientSummaryInfoPacketReceived(const QString &computerName, const QString &workgroupName, const QString &networkInfo, const QString &usersInfo, const QString &osInfo, bool usbsdEnabled, bool programesEnabled, const QString &admins, const QString &clientVersion);
    void signalClientResponseClientDetailedInfoPacketReceived(const QString &computerName, const QString &clientInfo);


    void signalClientRequestSoftwareVersionPacketReceived(const QString &softwareName);

    void signalClientLogReceived(const QString &computerName, const QString &users, const QString &clientAddress, quint8 logType, const QString &log, const QString &clientTime);

    void signalClientOnlineStatusChanged(int socketID, const QString &clientName, bool online);

    void signalAdminOnlineStatusChanged(int socketID, const QString &clientName, const QString &adminName, bool online);

private:

private:
    QHostAddress localUDTListeningAddress;
    quint16 localUDTListeningPort;
    QString m_serverName;

    quint16 localIPMCListeningPort;



    UDPServer *m_udpServer;
    UDTProtocol *m_udtProtocol;


};

}

#endif /* SERVERPACKETSPARSER_H_ */
