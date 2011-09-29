/*
 ****************************************************************************
 * clientpacketsparser.h
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

#ifndef CLIENTPACKETSPARSER_H_
#define CLIENTPACKETSPARSER_H_



#include <QHostAddress>
#include <QHostInfo>
#include <QDebug>
#include <QFile>


#include "../../sharedms/global_shared.h"
//#include "../../sharedms/networkmanager.h"
#include "../clientnetworkmanager.h"

//#include "../../../shared/network/packethandler/packetparserbase.h"

#include "HHSharedCore/hcryptography.h"
#include "HHSharedNetwork/hpacketparserbase.h"
#include "HHSharedNetwork/hnetworkutilities.h"



namespace HEHUI {


class ClientPacketsParser : public PacketsParserBase{
    Q_OBJECT
public:
    ClientPacketsParser(ClientNetworkManager *networkManager, QObject *parent = 0);
    virtual ~ClientPacketsParser();


    void run();
    void parseIncomingPacketData(Packet *packet);

    void startparseIncomingPackets();
    void startprocessOutgoingPackets();

    QHostAddress getServerAddress() const;




public slots:


//    quint16 sendHeartbeatPacket(){
//        qDebug()<<"----sendHeartbeatPacket()";

//        if(serverAddress.isNull()){
//            serverAddress = QHostAddress::Broadcast;
//        }

//        Packet *packet = m_packetHandlerBase->getPacket(serverAddress, quint16(IP_MULTICAST_GROUP_PORT), localIPMCListeningAddress, localIPMCListeningPort);
//        packet->setPacketType(quint8(HEHUI::HeartbeatPacket));
//        QByteArray ba;
//        QDataStream out(&ba, QIODevice::WriteOnly);
//        out.setVersion(QDataStream::Qt_4_6);
//        out << m_localComputerName;
//        packet->setPacketData(ba);
//        m_packetHandlerBase->appendOutgoingPacket(packet);

//        return packet->getPacketSerialNumber();

//    }


//    quint16 sendConfirmationOfReceiptPacket(const QHostAddress peerAddress, quint16 peerPort, quint16 packetSerialNumber, const QString &peerID){
//        qDebug()<<"----sendConfirmationOfReceiptPacket(...)";

//        //        UDPPacket *packet = new UDPPacket(peerAddress, peerPort, localIPMCListeningAddress, localIPMCListeningPort);
//        //UDPPacket *packet = new UDPPacket(peerAddress, peerPort);
//        Packet *packet = m_packetHandlerBase->getPacket(peerAddress, peerPort);
        
//        packet->setPacketType(quint8(HEHUI::ConfirmationOfReceiptPacket));
//        QByteArray ba;
//        QDataStream out(&ba, QIODevice::WriteOnly);
//        out.setVersion(QDataStream::Qt_4_6);
//        out << m_localComputerName << packetSerialNumber << getLastReceivedPacketSN(peerID);
//        packet->setPacketData(ba);
//        m_packetHandlerBase->appendOutgoingPacket(packet);

//        m_receivedPacketsHash.insert(peerID, qMakePair(packetSerialNumber, QDateTime::currentDateTime()));

//        return packet->getPacketSerialNumber();
//    }

    void sendClientLookForServerPacket(const QString &targetAddress = QString(IP_MULTICAST_GROUP_ADDRESS)){
        qDebug()<<"----sendClientLookForServerPacket(...)";

        QHostAddress address = QHostAddress(targetAddress);
        if(address.isNull()){
            address = QHostAddress(QString(IP_MULTICAST_GROUP_ADDRESS));
        }
        
        Packet *packet = m_packetHandlerBase->getPacket(address, quint16(IP_MULTICAST_GROUP_PORT), localIPMCListeningAddress, localIPMCListeningPort);
        //Packet *packet = m_packetHandlerBase->getPacket(address, quint16(IP_MULTICAST_GROUP_PORT), m_localRUDPListeningAddress, m_localRUDPListeningPort);


        packet->setPacketType(quint8(MS::ClientLookForServer));
        packet->setTransmissionProtocol(TP_UDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES*3));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localComputerName << m_localRUDPListeningPort;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

    }

    void sendClientOnlinePacket(const QHostAddress clientRUDPListeningAddress, quint16 clientRUDPListeningPort, const QString &clientName, bool isAdmin){
        qDebug()<<"----sendClientOnlinePacket(...)";

        Packet *packet = m_packetHandlerBase->getPacket(serverAddress, localIPMCListeningPort, localIPMCListeningAddress, localIPMCListeningPort);
        //Packet *packet = m_packetHandlerBase->getPacket(serverAddress, localIPMCListeningPort, m_localRUDPListeningAddress, m_localRUDPListeningPort);

        packet->setPacketType(quint8(MS::ClientOnline));
        packet->setTransmissionProtocol(TP_UDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localComputerName << clientRUDPListeningAddress.toString() << clientRUDPListeningPort <<clientName << isAdmin;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

    }

    void sendClientOfflinePacket(const QHostAddress clientRUDPListeningAddress, quint16 clientRUDPListeningPort, const QString &clientName, bool isAdmin){
        qDebug()<<"----sendClientOfflinePacket(...)";

        Packet *packet = m_packetHandlerBase->getPacket(serverAddress, localIPMCListeningPort, localIPMCListeningAddress, localIPMCListeningPort);
        //Packet *packet = m_packetHandlerBase->getPacket(serverAddress, localIPMCListeningPort, m_localRUDPListeningAddress, m_localRUDPListeningPort);

        packet->setPacketType(quint8(MS::ClientOffline));
        packet->setTransmissionProtocol(TP_UDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localComputerName << clientRUDPListeningAddress.toString() << clientRUDPListeningPort << clientName << isAdmin;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

    }

    void sendClientResponseRemoteConsoleStatusPacket(const QHostAddress adminAddress, quint16 adminPort, bool running, const QString &extraMessage){
        qDebug()<<"----sendClientResponseRemoteConsolePacket(...)";

        //后两个参数必须为空，才能发送给本机
        //The last two parameters must be null to send the packet to itself!
        //Packet *packet = new Packet(adminAddress, adminPort);
        Packet *packet = m_packetHandlerBase->getPacket(adminAddress, adminPort, m_localRUDPListeningAddress, m_localRUDPListeningPort);

        packet->setPacketType(quint8(MS::ClientResponseRemoteConsoleStatus));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localComputerName << (running?quint8(1):quint8(0)) << extraMessage;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

    }

    void sendRemoteConsoleCMDResultFromClientPacket(const QHostAddress adminAddress, quint16 adminPort, const QString &result){
        qDebug()<<"----sendRemoteConsoleCMDResultFromClientPacket(...)";

        //后两个参数必须为空，才能发送给本机
        //The last two parameters must be null to send the packet to itself!
        //Packet *packet = new Packet(adminAddress, adminPort);
        Packet *packet = m_packetHandlerBase->getPacket(adminAddress, adminPort, m_localRUDPListeningAddress, m_localRUDPListeningPort);

        packet->setPacketType(quint8(MS::RemoteConsoleCMDResultFromClient));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setPacketSerialNumber(packetSerialNumber);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localComputerName << result;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

    }

    void sendClientResponseClientSummaryInfoPacket(const QString &targetAddress, quint16 targetPort, const QString &workgroupName, const QString &networkInfo, const QString &usersInfo, const QString &osInfo, bool usbsdEnabled, bool programesEnabled, const QString &admins){
        //qWarning()<<"----sendClientResponseClientInfoPacket(...)"<<" targetAddress:"<<targetAddress<<" targetPort:"<<targetPort;

        QHostAddress address = QHostAddress(targetAddress);
        quint16 port = targetPort;
        Packet *packet = 0;

//        if(address == QHostAddress::LocalHost){
//            packet = m_packetHandlerBase->getPacket(address, port);
//        }else{
//            if(address.isNull()){
//                address = serverAddress;
//                port = localIPMCListeningPort;
//            }
//            packet = m_packetHandlerBase->getPacket(address, port, localIPMCListeningAddress, localIPMCListeningPort);
//        }

        if(address.isNull()){
            address = serverAddress;
            port = serverRUDPListeningPort;
        }
        if(address.isNull()){
            return;
        }

        packet = m_packetHandlerBase->getPacket(address, port, m_localRUDPListeningAddress, m_localRUDPListeningPort);

        packet->setPacketType(quint8(MS::ClientResponseClientSummaryInfo));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES)*2);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localComputerName << workgroupName << networkInfo << usersInfo << osInfo <<usbsdEnabled << programesEnabled << admins << QString(APP_VERSION);
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

    }

    void sendClientRequestSoftwareVersionPacket(const QString &softwareName){
        qDebug()<<"----sendClientRequestSoftwareVersionPacket(...)";

        Packet *packet = m_packetHandlerBase->getPacket(serverAddress, serverRUDPListeningPort, m_localRUDPListeningAddress, m_localRUDPListeningPort);
        
        packet->setPacketType(quint8(MS::ClientRequestSoftwareVersion));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localComputerName << softwareName;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

    }

    void sendClientLogPacket(const QString &users, quint8 logType, const QString &log){
        //qWarning()<<"----sendClientLogPacket(...) log:"<<log;

        Packet *packet = m_packetHandlerBase->getPacket(serverAddress, serverRUDPListeningPort, m_localRUDPListeningAddress, m_localRUDPListeningPort);
        
        packet->setPacketType(quint8(MS::ClientLog));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localComputerName << users << logType << log << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

    }

    void sendClientResponseClientDetailedInfoPacket(const QString &systemInfoFilePath, const QString &peerAddress = "", quint16 peerPort = quint16(RUDP_LISTENING_PORT)){

        QFile file(systemInfoFilePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
            //TODO
            //sendClientMessagePacket(peerAddress, peerPort, localComputerName, "Can not read system info file!");
            return ;
        }
        QTextStream in(&file);
        QString info = in.readAll();

        //后两个参数必须为空，才能发送给本机
        //The last two parameters must be null to send the packet to itself!
        //Packet *packet = packetHandlerBase->getPacket(peerAddress, peerPort);

        QHostAddress address = QHostAddress(peerAddress);
        quint16 port = peerPort;
        if(address.isNull()){
            if(serverAddress.isNull()){
                return ;
            }
            address = serverAddress;
            port = serverRUDPListeningPort;
        }
        if(port == 0){
            port = quint16(RUDP_LISTENING_PORT);
        }

        Packet *packet = m_packetHandlerBase->getPacket(address, port, m_localRUDPListeningAddress, m_localRUDPListeningPort);

        packet->setPacketType(quint8(MS::ClientResponseClientDetailedInfo));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES)*2);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);

        out << m_localComputerName << info;

        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

        qDebug()<<"----sendClientResponseClientDetailedInfoPacket(...) Host:"<<address.toString()<<" Port:"<<port;

    }

    void sendClientResponseAdminConnectionResultPacket(const QHostAddress adminAddress, quint16 adminPort, bool result, const QString &message){
        //qWarning()<<"----sendClientResponseVerifyInfoResultPacket(...):"<<adminAddress.toString()<<" "<<adminPort;

        //后两个参数必须为空，才能发送给本机
        //The last two parameters must be null to send the packet to itself!
        //Packet *packet = new Packet(adminAddress, adminPort);
        Packet *packet = m_packetHandlerBase->getPacket(adminAddress, adminPort, m_localRUDPListeningAddress, m_localRUDPListeningPort);
        
        packet->setPacketType(quint8(MS::ClientResponseAdminConnectionResult));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);

        out << m_localComputerName << result << message;

        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

    }

    void sendClientMessagePacket(const QHostAddress adminAddress, quint16 adminPort, const QString &message){

        //后两个参数必须为空，才能发送给本机
        //The last two parameters must be null to send the packet to itself!
        //Packet *packet = new Packet(adminAddress, adminPort);
        Packet *packet = m_packetHandlerBase->getPacket(adminAddress, adminPort, m_localRUDPListeningAddress, m_localRUDPListeningPort);
        
        packet->setPacketType(quint8(MS::ClientMessage));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);

        out << m_localComputerName << message  ;

        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

    } 

    void sendLocalServiceServerDeclarePacket(const QHostAddress userAddress, quint16 userPort){

        //后两个参数必须为空，才能发送给本机
        //The last two parameters must be null to send the packet to itself!
        //Packet *packet = new Packet(adminAddress, adminPort);
        Packet *packet = m_packetHandlerBase->getPacket(userAddress, userPort, m_localRUDPListeningAddress, m_localRUDPListeningPort);
        
        packet->setPacketType(quint8(MS::LocalServiceServerDeclare));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);

        out << m_localComputerName  ;

        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

    }
    
    void requestRemoteAssistance(const QString &adminAddress, quint16 adminPort, const QString &adminName){

        foreach (QString userName, localUsersHash.keys()) {
            if(userName.trimmed().isEmpty()){
                continue;
            }
            sendRequestRemoteAssistancePacket(QHostAddress::LocalHost, localUsersHash.value(userName), adminAddress, adminPort, adminName);

        }
        
    }

    void sendRequestRemoteAssistancePacket( const QHostAddress userAddress, quint16 userPort, const QString &adminAddress, quint16 adminPort, const QString &adminName){

        //后两个参数必须为空，才能发送给本机
        //The last two parameters must be null to send the packet to itself!
        //Packet *packet = new Packet(adminAddress, adminPort);
        Packet *packet = m_packetHandlerBase->getPacket(userAddress, userPort, m_localRUDPListeningAddress, m_localRUDPListeningPort);
        
        packet->setPacketType(quint8(MS::AdminRequestRemoteAssistance));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);

        out << m_localComputerName << adminAddress << adminPort << adminName ;

        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

    }

    void sendInformUpdatePasswordPacket(const QString &userName, const QString &adminAddress, quint16 adminPort, const QString &adminName, const QString &oldPassword, const QString &newPassword){
        //qWarning()<<"sendInformUpdatePasswordPacket(...)"<<" userName:"<<userName<<" Port:"<<localUsersHash.value(userName) << " newPassword:"<<newPassword;
        
        //后两个参数必须为空，才能发送给本机
        //The last two parameters must be null to send the packet to itself!
        //Packet *packet = new Packet(adminAddress, adminPort);
        Packet *packet = m_packetHandlerBase->getPacket(QHostAddress::LocalHost, localUsersHash.value(userName), m_localRUDPListeningAddress, m_localRUDPListeningPort);
        
        packet->setPacketType(quint8(MS::InformUserNewPassword));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);

        out << m_localComputerName << adminAddress << adminPort << adminName << oldPassword << newPassword ;

        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

    }
    
    void sendAnnouncement(const QString &adminName, quint32 announcementID, const QString &announcement){
        //qWarning()<<"sendAnnouncement(...)";
        //qWarning()<<"localUsersHash.size():"<<localUsersHash.size();
        foreach (QString userName, localUsersHash.keys()) {
            if(userName.trimmed().isEmpty()){
                continue;
            }
            sendServerAnnouncementPacket(QHostAddress::LocalHost, localUsersHash.value(userName), adminName, announcementID, announcement);

        }

    }
    
    void sendServerAnnouncementPacket(const QHostAddress userAddress, quint16 userPort, const QString &adminName, quint32 announcementID, const QString &serverAnnouncement){
        //qWarning()<<"sendServerAnnouncementPacket(...)"<<" userAddress:"<<userAddress.toString()<<" Port:"<<userPort;
        
        //后两个参数必须为空，才能发送给本机
        //The last two parameters must be null to send the packet to itself!
        //UDPPacket *packet = new UDPPacket(adminAddress, adminPort);
        Packet *packet = m_packetHandlerBase->getPacket(userAddress, userPort, m_localRUDPListeningAddress, m_localRUDPListeningPort);
        
        packet->setPacketType(quint8(MS::ServerAnnouncement));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);

        out << m_localComputerName << adminName << announcementID << serverAnnouncement ;

        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

    }
    
    
    
private slots:
//    void startHeartbeat(int interval = HEARTBEAT_TIMER_INTERVAL);
//    void stopHeartbeat();


signals:
    void  signalHeartbeatPacketReceived(const QString &computerName);
    void  signalConfirmationOfReceiptPacketReceived(quint16 packetSerialNumber1, quint16 packetSerialNumber2);

    void signalServerDeclarePacketReceived(const QString &serverAddress, quint16 serverRUDPListeningPort, const QString &serverName, const QString &version);

    void signalServerOnlinePacketReceived(const QHostAddress serverAddress, quint16 serverPort, const QString &serverName);
    void signalServerOfflinePacketReceived(const QHostAddress serverAddress, quint16 serverPort, const QString &serverName);

    void signalClientDetailedInfoRequestedPacketReceived(const QString &computerName, bool rescan, const QString &adminAddress, quint16 adminPort);
    void signalAdminRequestRemoteConsolePacketReceived(const QString &computerName, const QString &applicationPath, const QString &adminID, bool startProcess, const QString &adminAddress, quint16 adminPort);
    void signalRemoteConsoleCMDFromServerPacketReceived(const QString &computerName, const QString &command, const QString &adminAddress, quint16 adminPort);

    void signalServerRequestClientSummaryInfoPacketReceived(const QString &groupName, const QString &computerName, const QString &userName);

    void signalClientRequestSoftwareVersionPacketReceived(const QString &softwareName);
    void signalServerResponseSoftwareVersionPacketReceived(const QString &softwareName, const QString &version);

    void signalServerAnnouncementPacketReceived(const QString &workgroupName, const QString &computerName, quint32 announcementID, const QString &announcement, const QString &adminName, bool mustRead);

    void signalUpdateClientSoftwarePacketReceived();

    void signalSetupUSBSDPacketReceived(const QString &computerName, const QString &users, bool enable, bool temporarilyAllowed, const QString &adminName, const QString &adminAddress, quint16 adminPort);
    void signalSetupProgramesPacketReceived(const QString &computerName, const QString &users, bool enable, bool temporarilyAllowed, const QString &adminName, const QString &adminAddress, quint16 adminPort);
    void signalShowAdminPacketReceived(const QString &computerName, const QString &users, bool show);
    void signalModifyAdminGroupUserPacketReceived(const QString &computerName, const QString &userName, bool addToAdminGroup, const QString &adminName, const QString &adminAddress, quint16 adminPort);
    void signalAdminRequestConnectionToClientPacketReceived(const QString &adminAddress, quint16 adminPort, const QString &computerName, const QString &users);
    void signalAdminSearchClientPacketReceived(const QString &adminAddress, quint16 adminPort, const QString &computerName, const QString &userName, const QString &workgroup, const QString &macAddress, const QString &ipAddress, const QString &osVersion, const QString &adminName);
    
    void signalAdminRequestRemoteAssistancePacketReceived(const QString &computerName, const QString &adminName, const QString &adminAddress, quint16 adminPort);
    void signalAdminRequestUpdateMSUserPasswordPacketReceived(const QString &workgroup, const QString &adminName, const QString &adminAddress, quint16 adminPort);
    void signalAdminRequestInformUserNewPasswordPacketReceived(const QString &workgroup, const QString &adminName, const QString &adminAddress, quint16 adminPort);


private:
    quint16 getLastReceivedPacketSN(const QString &peerID);


private:
    QHostAddress serverAddress;
    quint16 serverRUDPListeningPort;
    QString serverName;

    QHostAddress localIPMCListeningAddress;
    quint16 localIPMCListeningPort;

    QHostAddress m_localRUDPListeningAddress;
    quint16 m_localRUDPListeningPort;


    QTimer *heartbeatTimer;

    PacketHandlerBase *m_packetHandlerBase;

    QDateTime serverLastOnlineTime;

    QMutex mutex;

    QString m_localComputerName;

    QHash<QString /*User Name*/, quint16 /*Port*/> localUsersHash;

    QMultiHash<QString /*Peer ID*/, QPair<quint16 /*Packet Serial Number*/, QDateTime/*Received Time*/> > m_receivedPacketsHash;




};

}

#endif /* CLIENTPACKETSPARSER_H_ */
