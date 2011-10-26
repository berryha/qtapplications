/*
 ****************************************************************************
 * controlcenterpacketsparser.h
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

#ifndef CONTROLCENTERPACKETSPARSER_H_
#define CONTROLCENTERPACKETSPARSER_H_



#include <QHostAddress>
#include <QDebug>

#include "../../sharedms/global_shared.h"
#include "networkmanagerinstance.h"


#include "HHSharedCore/hcryptography.h"
#include "HHSharedNetwork/hpacketparserbase.h"



namespace HEHUI {


class ControlCenterPacketsParser : public PacketsParserBase{
    Q_OBJECT
public:
    ControlCenterPacketsParser(NetworkManagerInstance *networkManager, QObject *parent = 0);
    virtual ~ControlCenterPacketsParser();

//    void setLocalUDPListeningAddress(const QHostAddress &address = QHostAddress::Any);
//    void setLocalUDPListeningPort(quint16 port);


    //	void parseIncomingPackets();

    virtual void parseIncomingPacketData(Packet *packet);
    virtual void run();




//    void sendConfirmationOfReceiptPacket(const QHostAddress peerAddress, quint16 peerPort, quint16 packetSerialNumber, const QString &peerID){
//        qDebug()<<"----sendConfirmationOfReceiptPacket(...)";

//        Packet *packet = new Packet(peerAddress.toString(), peerPort, localUDPListeningAddress.toString(), localUDPListeningPort);
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

//    void sendClientLookForServerPacket(const QHostAddress &localAddress, quint16 localPort, const QString &peerAddress = QString(IP_MULTICAST_GROUP_ADDRESS), quint16 peerPort = quint16(IP_MULTICAST_GROUP_PORT)){
//        qDebug()<<"----sendClientLookForServerPacket(...)";
//        qDebug()<<" ipmcGroupAddress:"<<ipmcGroupAddress.toString()<<" ipmcListeningPort:"<<ipmcListeningPort<<" localRUDPListeningPort:"<<localRUDPListeningPort;

//        QHostAddress targetAddress = QHostAddress(peerAddress);
//        quint16 targetPort = peerPort;
//        if(targetAddress.isNull()){
//            targetAddress = QHostAddress(QString(IP_MULTICAST_GROUP_ADDRESS));
//            targetPort = quint16(IP_MULTICAST_GROUP_PORT);
//        }

//        Packet *packet = m_packetHandlerBase->getPacket(targetAddress, targetPort, localAddress, localPort);

//        packet->setPacketType(quint8(MS::ClientLookForServer));
//        packet->setTransmissionProtocol(TP_UDP);
//        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES*10));
//        QByteArray ba;
//        QDataStream out(&ba, QIODevice::WriteOnly);
//        out.setVersion(QDataStream::Qt_4_6);
//        out << m_localID << localRUDPListeningPort;
//        packet->setPacketData(ba);
//        m_packetHandlerBase->appendOutgoingPacket(packet);

//    }

    void sendClientOnlinePacket(const QHostAddress clientRUDPListeningAddress, quint16 clientRUDPListeningPort, const QString &clientName, bool isAdmin){
        qDebug()<<"----sendClientOnlinePacket(...)";

        QHostAddress targetAddress = serverAddress;
        if(targetAddress.isNull()){
            targetAddress = QHostAddress(QString(IP_MULTICAST_GROUP_ADDRESS));
        }

        //Packet *packet = new Packet(serverAddress, ipmcListeningPort, localRUDPListeningAddress, localRUDPListeningPort);
        Packet *packet = new Packet(targetAddress, ipmcListeningPort);

        packet->setPacketType(quint8(MS::ClientOnline));
        packet->setTransmissionProtocol(TP_UDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << clientRUDPListeningAddress.toString() << clientRUDPListeningPort <<clientName << isAdmin;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

    }

    void sendClientOfflinePacket(const QHostAddress clientRUDPListeningAddress, quint16 clientRUDPListeningPort, const QString &clientName, bool isAdmin){
        qDebug()<<"----sendClientOfflinePacket(...)";

        //Packet *packet = new Packet(serverAddress, ipmcListeningPort, localRUDPListeningAddress, localRUDPListeningPort);
        Packet *packet = new Packet(serverAddress, ipmcListeningPort);

        packet->setPacketType(quint8(MS::ClientOffline));
        packet->setTransmissionProtocol(TP_UDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << clientRUDPListeningAddress.toString() << clientRUDPListeningPort << clientName << isAdmin;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

    }


    /////////////////////////////////////////////////////

    void sendRequestClientDetailedInfoPacket(const QHostAddress &peerAddress, quint16 peerPort, const QString &computerName, bool rescan){
        //qWarning()<<"peerAddress:"<<peerAddress.toString()<<" ipmcListeningPort"<<ipmcListeningPort<<" localUDPListeningPort"<<localUDPListeningPort;

        Packet *packet = new Packet(peerAddress, peerPort, localRUDPListeningAddress, localRUDPListeningPort);
        packet->setPacketType(quint8(MS::ClientDetailedInfoRequested));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);

        out << m_localID << localRUDPListeningPort << computerName << rescan;

        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

    }

    void sendAdminRequestRemoteConsolePacket(const QHostAddress &peerAddress, quint16 peerPort, const QString &computerName, const QString &applicationPath, const QString &adminID, bool startProcess = true){
        qDebug()<<"----sendServerRequestRemoteConsolePacket(...)";

        Packet *packet = new Packet(peerAddress, peerPort, localRUDPListeningAddress, localRUDPListeningPort);
        packet->setPacketType(quint8(MS::AdminRequestRemoteConsole));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);

        out << m_localID << computerName << applicationPath << adminID << startProcess;

        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);


    }

    void sendRemoteConsoleCMDFromAdminPacket(const QHostAddress &peerAddress, quint16 peerPort, const QString &computerName, const QString &command){
        qDebug()<<"----sendRemoteConsoleCMDFromServerPacket(...)";

        Packet *packet = new Packet(peerAddress, peerPort, localRUDPListeningAddress, localRUDPListeningPort);
        packet->setPacketType(quint8(MS::RemoteConsoleCMDFromAdmin));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);

        out << m_localID << computerName << command;

        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

    }

    void sendSetupUSBSDPacket(const QHostAddress &peerAddress, quint16 peerPort, const QString &computerName, const QString &userName, bool enable, bool temporarilyAllowed, const QString &adminName){

        Packet *packet = new Packet(peerAddress, peerPort, localRUDPListeningAddress, localRUDPListeningPort);
        packet->setPacketType(quint8(MS::SetupUSBSD));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << computerName << userName << enable << temporarilyAllowed << adminName;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

    }

    void sendSetupProgramesPacket(const QHostAddress &peerAddress, quint16 peerPort, const QString &computerName, const QString &userName, bool enable, bool temporarilyAllowed, const QString &adminName){

        Packet *packet = new Packet(peerAddress, peerPort, localRUDPListeningAddress, localRUDPListeningPort);
        packet->setPacketType(quint8(MS::SetupProgrames));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << computerName << userName << enable << temporarilyAllowed << adminName;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

    }

    void sendShowAdminPacket(const QHostAddress &peerAddress, quint16 peerPort, const QString &computerName, const QString &userName, bool show){
        Packet *packet = new Packet(peerAddress, peerPort, localRUDPListeningAddress, localRUDPListeningPort);
        packet->setPacketType(quint8(MS::ShowAdmin));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << computerName << userName << show;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

    }

    void sendModifyAdminGroupUserPacket(const QHostAddress &peerAddress, quint16 peerPort, const QString &computerName, const QString &userName, bool addToAdminGroup, const QString &adminName){
        Packet *packet = new Packet(peerAddress, peerPort, localRUDPListeningAddress, localRUDPListeningPort);
        packet->setPacketType(quint8(MS::ModifyAdminGroupUser));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << computerName << userName << addToAdminGroup  << adminName;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

    }

    void sendAdminRequestConnectionToClientPacket(const QHostAddress &peerAddress, quint16 peerPort, const QString &computerName, const QString &users){

        Packet *packet = new Packet(peerAddress, peerPort, localRUDPListeningAddress, localRUDPListeningPort);
        packet->setPacketType(quint8(MS::AdminRequestConnectionToClient));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES)*2);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << computerName << users ;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);


    }
    
    void sendAdminSearchClientPacket(const QHostAddress &targetAddress, const QString &computerName, const QString &userName, const QString &workgroup, const QString &macAddress, const QString &ipAddress, const QString &osVersion, const QString &adminName){

        //Packet *packet = new Packet(targetAddress, ipmcListeningPort, localRUDPListeningAddress, localRUDPListeningPort);
        Packet *packet = new Packet(targetAddress, ipmcListeningPort);

        packet->setPacketType(quint8(MS::AdminSearchClient));
        packet->setTransmissionProtocol(TP_UDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << localRUDPListeningPort << computerName << userName << workgroup << macAddress << ipAddress << osVersion << adminName;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

    }
    
    void sendRemoteAssistancePacket(const QHostAddress &peerAddress, quint16 peerPort, const QString &computerName, const QString &adminName){

        Packet *packet = new Packet(peerAddress, peerPort, localRUDPListeningAddress, localRUDPListeningPort);

        packet->setPacketType(quint8(MS::AdminRequestRemoteAssistance));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << computerName << adminName;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

    }

    void sendUpdateMSUserPasswordPacket(const QString &peerAddress, quint16 peerPort, const QString &workgroupName, const QString &adminName){

        Packet *packet = 0;
        QHostAddress targetAddress = QHostAddress(peerAddress);
        if(targetAddress.isNull()){
            targetAddress = ipmcGroupAddress;
            packet = new Packet(targetAddress, ipmcListeningPort);
            packet->setTransmissionProtocol(TP_UDP);
        }else{
            packet = new Packet(targetAddress, peerPort, localRUDPListeningAddress, localRUDPListeningPort);
            packet->setTransmissionProtocol(TP_RUDP);
        }

        packet->setPacketType(quint8(MS::UpdateMSWUserPassword));
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << workgroupName << adminName;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

//        return packet->getPacketSerialNumber();
    }
    
    void sendInformUpdatePasswordPacket(const QString &peerAddress, quint16 peerPort, const QString &workgroupName, const QString &adminName){

        Packet *packet = 0;
        QHostAddress targetAddress = QHostAddress(peerAddress);
        if(targetAddress.isNull()){
            targetAddress = ipmcGroupAddress;
            packet = new Packet(targetAddress, ipmcListeningPort);
            packet->setTransmissionProtocol(TP_UDP);
        }else{
            packet = new Packet(targetAddress, peerPort, localRUDPListeningAddress, localRUDPListeningPort);
            packet->setTransmissionProtocol(TP_RUDP);
        }
        
        //后两个参数必须为空，才能发送给本机
        //The last two parameters must be null to send the packet to itself!
        //Packet *packet = new Packet(adminAddress, adminPort);
        //Packet *packet = m_packetHandlerBase->getPacket(targetAddress, ipmcListeningPort, localRUDPListeningAddress, localRUDPListeningPort);
        
        packet->setPacketType(quint8(MS::InformUserNewPassword));

        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);

        out << m_localID << workgroupName <<  adminName  ;

        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);


    }
    
    void sendAnnouncementPacket(const QString &peerAddress, quint16 peerPort, const QString &groupName, const QString &computerName, quint32 announcementID, const QString &announcement, const QString &adminName, bool mustRead = true){

        Packet *packet = 0;
        QHostAddress targetAddress = QHostAddress(peerAddress);
        if(targetAddress.isNull()){
            targetAddress = ipmcGroupAddress;
            packet = new Packet(targetAddress, ipmcListeningPort);
            packet->setTransmissionProtocol(TP_UDP);
        }else{
            packet = new Packet(targetAddress, peerPort, localRUDPListeningAddress, localRUDPListeningPort);
            packet->setTransmissionProtocol(TP_RUDP);
        }
        
        //Packet *packet = packetHandlerBase->getUDPPacket(targetAddress, ipmcListeningPort, localUDPListeningAddress, localUDPListeningPort);
        //Packet *packet = m_packetHandlerBase->getPacket(targetAddress, ipmcListeningPort, localRUDPListeningAddress, localRUDPListeningPort);
        
        //Packet *packet = packetHandlerBase->getPacket(QHostAddress(IP_MULTICAST_GROUP_ADDRESS), quint16(IP_MULTICAST_GROUP_PORT), localIPMCListeningAddress, localIPMCListeningPort);

        packet->setPacketType(quint8(MS::ServerAnnouncement));
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << groupName << computerName << announcementID << announcement << adminName << (mustRead?quint8(1):quint8(0));
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);


    }





signals:
    void  signalHeartbeatPacketReceived(const QString &computerName);
    void  signalConfirmationOfReceiptPacketReceived(quint16 packetSerialNumber1, quint16 packetSerialNumber2);

    //    void signalClientLookForServerPacketReceived(const QHostAddress clientAddress, quint16 clientPort, const QString &clientName);
    void signalServerDeclarePacketReceived(const QString &serverAddress, quint16 serverRUDPListeningPort, const QString &serverName, const QString &version, int serverInstanceID);

    //    void signalClientOnlinePacketReceived(const QHostAddress clientAddress, quint16 clientPort, const QString &clientName);
    //    void signalClientOfflinePacketReceived(const QHostAddress clientAddress, quint16 clientPort, const QString &clientName);

    void signalServerOnlinePacketReceived(const QHostAddress serverAddress, quint16 serverPort, const QString &serverName);
    void signalServerOfflinePacketReceived(const QHostAddress serverAddress, quint16 serverPort, const QString &serverName);

    //    void signalAdminLoggedOnToServerRequestPacketReceived(const QHostAddress adminAddress, quint16 adminPort, const QString &adminID);
    //    void signalServerRequestRemoteConsolePacketReceived(const QString &adminID);
    void signalClientResponseRemoteConsoleStatusPacketReceived(const QString &computerName, bool accept, const QString &extraMessage);
    //    void signalRemoteConsoleCMDFromServerPacketReceived(const QString &command);
    void signalRemoteConsoleCMDResultFromClientPacketReceived(const QString &computerName, const QString &result);



    void signalClientResponseClientDetailedInfoPacketReceived(const QString &computerName, const QString &clientInfo);

    void signalClientRequestSoftwareVersionPacketReceived(const QString &softwareName);
    void signalServerResponseSoftwareVersionPacketReceived(const QString &softwareName, const QString &version);

    //    void  signalServerAnnouncementPacketReceived(const QString &groupName, const QString &computerName, const QString &announcement, bool mustRead = true);


    void signalClientResponseAdminConnectionResultPacketReceived(const QString &computerName, bool result, const QString &message);
    void signalClientMessagePacketReceived(const QString &computerName, const QString &message);

    void signalUserResponseRemoteAssistancePacketReceived(const QString &userName, const QString &computerName, bool accept);
    void signalNewPasswordRetrevedByUserPacketReceived(const QString &userName, const QString &computerName);
    
    void signalClientResponseClientSummaryInfoPacketReceived(const QString &computerName, const QString &workgroupName, const QString &networkInfo, const QString &usersInfo, const QString &osInfo, bool usbsdEnabled, bool programesEnabled, const QString &admins, const QString &clientVersion);



public slots:
    //HeartbeatPacket: PacketType+ComputerName+IP
//    void startHeartbeat(int interval = HEARTBEAT_TIMER_INTERVAL);
//    void heartbeat();
//    void confirmPacketReceipt(quint16 packetSerialNumber);

private:
    quint16 getLastReceivedPacketSN(const QString &peerID);

private:
    QHostAddress serverAddress;
    quint16 serverRUDPListeningPort;
    QString serverName;

    QHostAddress ipmcGroupAddress;
    quint16 ipmcListeningPort;

    QHostAddress localRUDPListeningAddress;
    quint16 localRUDPListeningPort;

    QTimer *heartbeatTimer;

    PacketHandlerBase *m_packetHandlerBase;
    //NetworkManagerInstance *networkManager;

    QMutex mutex;

    QString m_localComputerName;
    QString m_localID;

    QMultiHash<QString /*Peer ID*/, QPair<quint16 /*Packet Serial Number*/, QDateTime/*Received Time*/> > m_receivedPacketsHash;





};

}

#endif /* CONTROLCENTERPACKETSPARSER_H_ */
