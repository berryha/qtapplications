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


    quint16 sendHeartbeatPacket(){
        qDebug()<<"----sendHeartbeatPacket()";

        if(serverAddress.isNull()){
            serverAddress = QHostAddress::Broadcast;
        }

        UDPPacket *packet = new UDPPacket(QString(IP_MULTICAST_GROUP_ADDRESS), quint16(IP_MULTICAST_GROUP_PORT), localIPMCListeningAddress.toString(), localIPMCListeningPort);
        packet->setPacketType(quint8(HEHUI::HeartbeatPacket));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_serverName;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

        return packet->getPacketSerialNumber();

    }


    quint16 sendConfirmationOfReceiptPacket(const QHostAddress peerAddress, quint16 peerPort, quint16 packetSerialNumber, const QString &peerID){
        qDebug()<<"----sendConfirmationOfReceiptPacket(...)";

        //UDPPacket *packet = new UDPPacket(peerAddress.toString(), peerPort, localIPMCListeningAddress.toString(), localIPMCListeningPort);
        //UDPPacket *packet = packetHandlerBase->getUDPPacket(peerAddress, peerPort, localIPMCListeningAddress, localIPMCListeningPort);
        Packet *packet = m_packetHandlerBase->getPacket(peerAddress, peerPort, localIPMCListeningAddress, localIPMCListeningPort);

        packet->setPacketType(quint8(HEHUI::ConfirmationOfReceiptPacket));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_serverName << packetSerialNumber << getLastReceivedPacketSN(peerID);
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

        m_receivedPacketsHash.insert(peerID, qMakePair(packetSerialNumber, QDateTime::currentDateTime()));


        return packet->getPacketSerialNumber();
    }

    quint16 sendServerDeclarePacket(const QHostAddress peerAddress, quint16 peerPort, const QHostAddress serverAddress, quint16 serverPort){
        //qWarning()<<"----sendServerDeclarePacket(...)"<<" Peer Address:"<<peerAddress.toString()<<":"<<peerPort;

        //UDPPacket *packet = new UDPPacket(peerAddress.toString(), peerPort, localIPMCListeningAddress.toString(), localIPMCListeningPort);
        //UDPPacket *packet = packetHandlerBase->getUDPPacket(peerAddress, peerPort, localIPMCListeningAddress, localIPMCListeningPort);
        Packet *packet = m_packetHandlerBase->getPacket(peerAddress, peerPort, localIPMCListeningAddress, localIPMCListeningPort);

        packet->setPacketType(quint8(MS::ServerDeclare));
        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_serverName << serverAddress.toString() << serverPort << QString(APP_VERSION);
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

        return packet->getPacketSerialNumber();

    }

    quint16 sendServerOnlinePacket(const QHostAddress serverAddress, quint16 serverTCPListeningPort){
        qDebug()<<"----sendServerOnlinePacket(...)";

        //UDPPacket *packet = new UDPPacket(QString(IP_MULTICAST_GROUP_ADDRESS), quint16(IP_MULTICAST_GROUP_PORT), localIPMCListeningAddress.toString(), localIPMCListeningPort);
        //UDPPacket *packet = packetHandlerBase->getUDPPacket(QHostAddress(IP_MULTICAST_GROUP_ADDRESS), quint16(IP_MULTICAST_GROUP_PORT), localIPMCListeningAddress, localIPMCListeningPort);
        Packet *packet = m_packetHandlerBase->getPacket(QHostAddress(IP_MULTICAST_GROUP_ADDRESS), quint16(IP_MULTICAST_GROUP_PORT), localIPMCListeningAddress, localIPMCListeningPort);

        packet->setPacketType(quint8(MS::ServerOnline));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_serverName << serverAddress.toString() << serverTCPListeningPort;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

        return packet->getPacketSerialNumber();

    }

    quint16 sendServerOfflinePacket(const QHostAddress serverAddress, quint16 serverTCPListeningPort){
        qDebug()<<"----sendServerOfflinePacket(...)";

        //UDPPacket *packet = new UDPPacket(QString(IP_MULTICAST_GROUP_ADDRESS), quint16(IP_MULTICAST_GROUP_PORT), localIPMCListeningAddress.toString(), localIPMCListeningPort);
        //UDPPacket *packet = packetHandlerBase->getUDPPacket(QHostAddress(IP_MULTICAST_GROUP_ADDRESS), quint16(IP_MULTICAST_GROUP_PORT), localIPMCListeningAddress, localIPMCListeningPort);
        Packet *packet = m_packetHandlerBase->getPacket(QHostAddress(IP_MULTICAST_GROUP_ADDRESS), quint16(IP_MULTICAST_GROUP_PORT), localIPMCListeningAddress, localIPMCListeningPort);

        packet->setPacketType(quint8(MS::ServerOffline));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_serverName << serverAddress.toString() << serverTCPListeningPort;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

        return packet->getPacketSerialNumber();

    }

    //    quint16 sendServerResponseAdminLoggedOnToServerRequestPacket(const QHostAddress adminAddress, quint16 adminPort, const QString &adminID, bool accept, const QString &extraMessage){
    //        qDebug()<<"----sendServerResponseAdminLoggedOnToServerRequestPacket(...)";

    //        UDPPacket *packet = new UDPPacket(adminAddress.toString(), adminPort, localIPMCListeningAddress.toString(), localIPMCListeningPort);
    //        packet->setPacketType(quint8(MS::ClientResponseClientInfoToAdminRequest));
    //        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
    //        QByteArray ba;
    //        QDataStream out(&ba, QIODevice::WriteOnly);
    //        out.setVersion(QDataStream::Qt_4_6);
    //        out << adminID << (accept?quint8(1):quint8(0)) << extraMessage;
    //        packet->setPacketData(ba);
    //        packetHandlerBase->appendOutgoingPacket(packet);

    //        return packet->getPacketSerialNumber();

    //    }

    //    quint16 sendAdminRequestRemoteConsolePacket(const QHostAddress &peerAddress, quint16 peerPort, const QString &adminID){
    //        qDebug()<<"----sendServerRequestRemoteConsolePacket(...)";

    //        UDPPacket *packet = new UDPPacket(peerAddress.toString(), peerPort, localIPMCListeningAddress.toString(), localIPMCListeningPort);
    //        packet->setPacketType(quint8(MS::AdminRequestRemoteConsole));
    //        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
    //        QByteArray ba;
    //        QDataStream out(&ba, QIODevice::WriteOnly);
    //        out.setVersion(QDataStream::Qt_4_6);
    //        out << adminID;
    //        packet->setPacketData(ba);
    //        packetHandlerBase->appendOutgoingPacket(packet);

    //        return packet->getPacketSerialNumber();

    //    }

    //    quint16 sendRemoteConsoleCMDFromAdminPacket(const QHostAddress &peerAddress, quint16 peerPort, const QString &command){
    //        qDebug()<<"----sendRemoteConsoleCMDFromServerPacket(...)";

    //        UDPPacket *packet = new UDPPacket(peerAddress.toString(), peerPort, localIPMCListeningAddress.toString(), localIPMCListeningPort);
    //        packet->setPacketType(quint8(MS::RemoteConsoleCMDFromAdmin));
    //        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
    //        QByteArray ba;
    //        QDataStream out(&ba, QIODevice::WriteOnly);
    //        out.setVersion(QDataStream::Qt_4_6);
    //        out << command;
    //        packet->setPacketData(ba);
    //        packetHandlerBase->appendOutgoingPacket(packet);

    //        return packet->getPacketSerialNumber();

    //    }

    quint16 sendServerRequestClientSummaryInfoPacket(const QString &groupName, const QString &computerName, const QString &userName, const QString &clientAddress = QString(IP_MULTICAST_GROUP_ADDRESS), quint16 clientPort = quint16(IP_MULTICAST_GROUP_PORT)){
        qDebug()<<"----sendServerRequestClientInfoPacket(...)";

        //        UDPPacket *packet = new UDPPacket(QString(IP_MULTICAST_GROUP_ADDRESS), quint16(IP_MULTICAST_GROUP_PORT), localIPMCListeningAddress.toString(), localIPMCListeningPort);
        //UDPPacket *packet = new UDPPacket(clientAddress, clientPort, localIPMCListeningAddress.toString(), localIPMCListeningPort);
        //UDPPacket *packet = packetHandlerBase->getUDPPacket(QHostAddress(clientAddress), clientPort, localIPMCListeningAddress, localIPMCListeningPort);
        Packet *packet = m_packetHandlerBase->getPacket(QHostAddress(clientAddress), clientPort, localIPMCListeningAddress, localIPMCListeningPort);

        packet->setPacketType(quint8(MS::ServerRequestClientSummaryInfo));
        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_serverName << groupName << computerName << userName;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

        return packet->getPacketSerialNumber();

    }

    quint16 sendRequestClientDetailedInfoPacket(const QString &peerAddress = QString(IP_MULTICAST_GROUP_ADDRESS), const QString &computerName = "", bool rescan = true){
        //qWarning()<<"peerAddress:"<<peerAddress.toString()<<" ipmcListeningPort"<<ipmcListeningPort<<" localUDPListeningPort"<<localUDPListeningPort;

        UDPPacket *packet = new UDPPacket(QHostAddress(peerAddress), quint16(IP_MULTICAST_GROUP_PORT), localIPMCListeningAddress, localIPMCListeningPort);
        packet->setPacketType(quint8(MS::ClientDetailedInfoRequested));
        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);

        out << m_serverName << computerName << rescan;

        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);
        qWarning()<<"sendRequestClientDetailedInfoPacket";
        return packet->getPacketSerialNumber();
    }





    quint16 sendServerResponseSoftwareVersionPacket(const QHostAddress &peerAddress, quint16 peerPort, const QString &softwareName, const QString &version){
        qDebug()<<"----sendServerResponseSoftwareVersionPacket(...)";

        //UDPPacket *packet = new UDPPacket(peerAddress.toString(), peerPort, localIPMCListeningAddress.toString(), localIPMCListeningPort);
        //UDPPacket *packet = packetHandlerBase->getUDPPacket(peerAddress, peerPort, localIPMCListeningAddress, localIPMCListeningPort);
        Packet *packet = m_packetHandlerBase->getPacket(peerAddress, peerPort, localIPMCListeningAddress, localIPMCListeningPort);

        packet->setPacketType(quint8(MS::ServerResponseSoftwareVersion));
        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_serverName << softwareName << version;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

        return packet->getPacketSerialNumber();

    }

    //    quint16 sendServerAnnouncementPacket(const QString &groupName, const QString &computerName, const QString &announcement, bool mustRead = true){
    //        qDebug()<<"----sendServerAnnouncementPacket(...)";

    //        //UDPPacket *packet = new UDPPacket(QString(IP_MULTICAST_GROUP_ADDRESS), quint16(IP_MULTICAST_GROUP_PORT), localIPMCListeningAddress.toString(), localIPMCListeningPort);
    //        UDPPacket *packet = packetHandlerBase->getUDPPacket(QHostAddress(IP_MULTICAST_GROUP_ADDRESS), quint16(IP_MULTICAST_GROUP_PORT), localIPMCListeningAddress, localIPMCListeningPort);
    //        //Packet *packet = packetHandlerBase->getPacket(QHostAddress(IP_MULTICAST_GROUP_ADDRESS), quint16(IP_MULTICAST_GROUP_PORT), localIPMCListeningAddress, localIPMCListeningPort);

    //        packet->setPacketType(quint8(MS::ServerAnnouncement));
    //        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
    //        QByteArray ba;
    //        QDataStream out(&ba, QIODevice::WriteOnly);
    //        out.setVersion(QDataStream::Qt_4_6);
    //        out << groupName << computerName << announcement << (mustRead?quint8(1):quint8(0));
    //        packet->setPacketData(ba);
    //        packetHandlerBase->appendOutgoingPacket(packet);

    //        return packet->getPacketSerialNumber();

    //    }

    quint16 sendUpdateClientSoftwarePacket(){
        qDebug()<<"----sendUpdateClientSoftwarePacket(...)";

        //UDPPacket *packet = new UDPPacket(QString(IP_MULTICAST_GROUP_ADDRESS), quint16(IP_MULTICAST_GROUP_PORT), localIPMCListeningAddress.toString(), localIPMCListeningPort);
        //UDPPacket *packet = packetHandlerBase->getUDPPacket(QHostAddress(IP_MULTICAST_GROUP_ADDRESS), quint16(IP_MULTICAST_GROUP_PORT), localIPMCListeningAddress, localIPMCListeningPort);
        Packet *packet = m_packetHandlerBase->getPacket(QHostAddress(IP_MULTICAST_GROUP_ADDRESS), quint16(IP_MULTICAST_GROUP_PORT), localIPMCListeningAddress, localIPMCListeningPort);

        packet->setPacketType(quint8(MS::Update));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_serverName;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

        return packet->getPacketSerialNumber();

    }



private slots:
    //HeartbeatPacket: PacketType+ComputerName
    void startHeartbeat(int interval = HEARTBEAT_TIMER_INTERVAL);
    void stopHeartbeat();



signals:
    void  signalHeartbeatPacketReceived(const QString &clientAddress, const QString &computerName);
    void  signalConfirmationOfReceiptPacketReceived(quint16 packetSerialNumber, quint16 packetSerialNumbe2);

    void signalClientLookForServerPacketReceived(const QHostAddress &clientAddress, quint16 clientTCPListeningPort, const QString &clientName);
    //    void signalServerDeclarePacketReceived(const QHostAddress serverAddress, quint16 serverPort, const QString &serverName);

    void signalClientOnlinePacketReceived(const QHostAddress &clientAddress, quint16 clientPort, const QString &clientName);
    void signalClientOfflinePacketReceived(const QHostAddress &clientAddress, quint16 clientPort, const QString &clientName);

    //    void signalServerOnlinePacketReceived(const QHostAddress serverAddress, quint16 serverPort, const QString &serverName);
    //    void signalServerOfflinePacketReceived(const QHostAddress serverAddress, quint16 serverPort, const QString &serverName);

    //    void signalAdminLoggedOnToServerRequestPacketReceived(const QHostAddress &adminAddress, quint16 adminPort, const QString &adminID, const QByteArray &encryptedPasswordArray);
    //    void signalServerRequestRemoteConsolePacketReceived(const QString &adminID);
    //    void signalClientResponseRemoteConsolePacketReceived(const QHostAddress &clientAddress, quint16 clientPort, const QString &adminID, bool accept, const QString &extraMessage);
    //    void signalRemoteConsoleCMDFromServerPacketReceived(const QString &command);
    //    void signalRemoteConsoleCMDResultFromClientPacketReceived(quint16 packetSerialNumber, const QString &result);

    //    void signalServerRequestClientSummaryInfoPacketReceived(const QString &groupName, const QString &computerName);
    void signalClientResponseClientSummaryInfoPacketReceived(const QString &computerName, const QString &workgroupName, const QString &networkInfo, const QString &usersInfo, const QString &osInfo, bool usbsdEnabled, bool programesEnabled, const QString &admins, const QString &clientVersion);
    void signalClientResponseClientDetailedInfoPacketReceived(const QString &computerName, const QString &clientInfo);


    void signalClientRequestSoftwareVersionPacketReceived(const QString &softwareName);
    //    void signalServerResponseSoftwareVersionPacketReceived(const QString &softwareName, const QString &version);

    //    bool signalServerAnnouncementPacketReceived(const QString &announcement, bool mustRead = true);

    void signalClientLogReceived(const QString &computerName, const QString &users, const QString &clientAddress, quint8 logType, const QString &log, const QString &clientTime);

    void signalClientOnlineStatusChanged(const QString &clientTCPListeningAddress, quint16 clientTCPListeningPort, const QString &clientName, bool online, bool isAdmin);


private:
    quint16 getLastReceivedPacketSN(const QString &peerID);

private:
    QHostAddress serverAddress;
    quint16 serverTCPListeningPort;
    QString m_serverName;

    QHostAddress localIPMCListeningAddress;
    quint16 localIPMCListeningPort;

    QTimer *heartbeatTimer;
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
