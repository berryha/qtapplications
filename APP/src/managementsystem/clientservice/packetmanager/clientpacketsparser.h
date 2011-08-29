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


    quint16 sendHeartbeatPacket(){
        qDebug()<<"----sendHeartbeatPacket()";

        //        static QString computerName = "";
        //        if(computerName.isEmpty()){
        //            computerName = QHostInfo::localHostName() ;
        //        }

        if(serverAddress.isNull()){
            serverAddress = QHostAddress::Broadcast;
        }

        //UDPPacket *packet = new UDPPacket(serverAddress.toString(), quint16(IP_MULTICAST_GROUP_PORT), localIPMCListeningAddress.toString(), localIPMCListeningPort);
        Packet *packet = m_packetHandlerBase->getPacket(serverAddress, quint16(IP_MULTICAST_GROUP_PORT), localIPMCListeningAddress, localIPMCListeningPort);

        packet->setPacketType(quint8(HEHUI::HeartbeatPacket));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localComputerName;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

        return packet->getPacketSerialNumber();

    }


    quint16 sendConfirmationOfReceiptPacket(const QHostAddress peerAddress, quint16 peerPort, quint16 packetSerialNumber, const QString &peerID){
        qDebug()<<"----sendConfirmationOfReceiptPacket(...)";

        //        UDPPacket *packet = new UDPPacket(peerAddress, peerPort, localIPMCListeningAddress, localIPMCListeningPort);
        //UDPPacket *packet = new UDPPacket(peerAddress, peerPort);
        Packet *packet = m_packetHandlerBase->getPacket(peerAddress, peerPort);
        
        packet->setPacketType(quint8(HEHUI::ConfirmationOfReceiptPacket));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localComputerName << packetSerialNumber << getLastReceivedPacketSN(peerID);
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

        m_receivedPacketsHash.insert(peerID, qMakePair(packetSerialNumber, QDateTime::currentDateTime()));

        return packet->getPacketSerialNumber();
    }

    quint16 sendClientLookForServerPacket(const QString &targetAddress = QString(IP_MULTICAST_GROUP_ADDRESS)){
        qDebug()<<"----sendClientLookForServerPacket(...)";

        //UDPPacket *packet = new UDPPacket(QString(IP_MULTICAST_GROUP_ADDRESS), quint16(IP_MULTICAST_GROUP_PORT), localIPMCListeningAddress.toString(), localIPMCListeningPort);
        QHostAddress address = QHostAddress(targetAddress);
        if(address.isNull()){
            address = QHostAddress(QString(IP_MULTICAST_GROUP_ADDRESS));
        }
        
        //UDPPacket *packet = new UDPPacket(address, quint16(IP_MULTICAST_GROUP_PORT), localIPMCListeningAddress, localIPMCListeningPort);
        Packet *packet = m_packetHandlerBase->getPacket(address, quint16(IP_MULTICAST_GROUP_PORT), localIPMCListeningAddress, localIPMCListeningPort);
        
        packet->setPacketType(quint8(MS::ClientLookForServer));
        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES*3));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        //out << clientTCPListeningPort << clientName;
        out << m_localComputerName;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

        clientLookForServerPacketSerialNumber = packet->getPacketSerialNumber();
        return clientLookForServerPacketSerialNumber;
        //return packet->getPacketSerialNumber();

    }

    quint16 sendClientOnlinePacket(const QHostAddress clientTCPListeningAddress, quint16 clientTCPListeningPort, const QString &clientName, bool isAdmin){
        qDebug()<<"----sendClientOnlinePacket(...)";

        //UDPPacket *packet = new UDPPacket(serverAddress.toString(), localIPMCListeningPort, localIPMCListeningAddress.toString(), localIPMCListeningPort);
        Packet *packet = m_packetHandlerBase->getPacket(serverAddress, localIPMCListeningPort, localIPMCListeningAddress, localIPMCListeningPort);
        
        packet->setPacketType(quint8(MS::ClientOnline));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localComputerName << clientTCPListeningAddress.toString() << clientTCPListeningPort <<clientName << isAdmin;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

        return packet->getPacketSerialNumber();
    }

    quint16 sendClientOfflinePacket(const QHostAddress clientTCPListeningAddress, quint16 clientTCPListeningPort, const QString &clientName, bool isAdmin){
        qDebug()<<"----sendClientOfflinePacket(...)";

        //UDPPacket *packet = new UDPPacket(serverAddress.toString(), localIPMCListeningPort, localIPMCListeningAddress.toString(), localIPMCListeningPort);
        Packet *packet = m_packetHandlerBase->getPacket(serverAddress, localIPMCListeningPort, localIPMCListeningAddress, localIPMCListeningPort);
        
        packet->setPacketType(quint8(MS::ClientOffline));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localComputerName << clientTCPListeningAddress.toString() << clientTCPListeningPort << clientName << isAdmin;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

        return packet->getPacketSerialNumber();

    }

    quint16 sendClientResponseRemoteConsoleStatusPacket(const QHostAddress adminAddress, quint16 adminPort, bool running, const QString &extraMessage){
        qDebug()<<"----sendClientResponseRemoteConsolePacket(...)";

        //UDPPacket *packet = new UDPPacket(serverAddress.toString(), localIPMCListeningPort, localIPMCListeningAddress.toString(), localIPMCListeningPort);

        //后两个参数必须为空，才能发送给本机
        //The last two parameters must be null to send the packet to itself!
        //UDPPacket *packet = new UDPPacket(adminAddress, adminPort);
        Packet *packet = m_packetHandlerBase->getPacket(adminAddress, adminPort);

        packet->setPacketType(quint8(MS::ClientResponseRemoteConsoleStatus));
        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localComputerName << (running?quint8(1):quint8(0)) << extraMessage;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

        return packet->getPacketSerialNumber();
    }

    quint16 sendRemoteConsoleCMDResultFromClientPacket(const QHostAddress adminAddress, quint16 adminPort, const QString &result){
        qDebug()<<"----sendRemoteConsoleCMDResultFromClientPacket(...)";

        //UDPPacket *packet = new UDPPacket(serverAddress.toString(), localIPMCListeningPort, localIPMCListeningAddress.toString(), localIPMCListeningPort);

        //后两个参数必须为空，才能发送给本机
        //The last two parameters must be null to send the packet to itself!
        //UDPPacket *packet = new UDPPacket(adminAddress, adminPort);
        Packet *packet = m_packetHandlerBase->getPacket(adminAddress, adminPort);

        packet->setPacketType(quint8(MS::RemoteConsoleCMDResultFromClient));
        //packet->setPacketSerialNumber(packetSerialNumber);
        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localComputerName << result;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

        return packet->getPacketSerialNumber();
    }

    quint16 sendClientResponseClientSummaryInfoPacket(const QString &targetAddress, quint16 targetPort, const QString &workgroupName, const QString &networkInfo, const QString &usersInfo, const QString &osInfo, bool usbsdEnabled, bool programesEnabled, const QString &admins){
        //qWarning()<<"----sendClientResponseClientInfoPacket(...)"<<" targetAddress:"<<targetAddress<<" targetPort:"<<targetPort;

        QHostAddress address = QHostAddress(targetAddress);
        quint16 port = targetPort;
        //UDPPacket *packet = 0;
        Packet *packet = 0;

        if(address == QHostAddress::LocalHost){
            //packet = new UDPPacket(address, port);
            packet = m_packetHandlerBase->getPacket(address, port);
        }else{
            if(address.isNull()){
                address = serverAddress;
                port = localIPMCListeningPort;
            }
            //packet = new UDPPacket(address, port, localIPMCListeningAddress, localIPMCListeningPort);
            packet = m_packetHandlerBase->getPacket(address, port, localIPMCListeningAddress, localIPMCListeningPort);
        }


        packet->setPacketType(quint8(MS::ClientResponseClientSummaryInfo));
        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES)*2);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localComputerName << workgroupName << networkInfo << usersInfo << osInfo <<usbsdEnabled << programesEnabled << admins << QString(APP_VERSION);
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

        return packet->getPacketSerialNumber();

    }


    quint16 sendClientRequestSoftwareVersionPacket(const QString &softwareName){
        qDebug()<<"----sendClientRequestSoftwareVersionPacket(...)";

        //UDPPacket *packet = new UDPPacket(serverAddress.toString(), localIPMCListeningPort, localIPMCListeningAddress.toString(), localIPMCListeningPort);
        Packet *packet = m_packetHandlerBase->getPacket(serverAddress, localIPMCListeningPort, localIPMCListeningAddress, localIPMCListeningPort);
        
        packet->setPacketType(quint8(MS::ClientRequestSoftwareVersion));
        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localComputerName << softwareName;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

        return packet->getPacketSerialNumber();

    }

    quint16 sendClientLogPacket(const QString &users, quint8 logType, const QString &log){
        //qWarning()<<"----sendClientLogPacket(...) log:"<<log;

        //UDPPacket *packet = new UDPPacket(serverAddress.toString(), localIPMCListeningPort, localIPMCListeningAddress.toString(), localIPMCListeningPort);
        Packet *packet = m_packetHandlerBase->getPacket(serverAddress, localIPMCListeningPort, localIPMCListeningAddress, localIPMCListeningPort);
        
        packet->setPacketType(quint8(MS::ClientLog));
        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localComputerName << users << logType << log << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

        return packet->getPacketSerialNumber();

    }



    quint16 sendClientResponseClientDetailedInfoPacket(const QString &systemInfoFilePath, const QString &peerAddress = "", quint16 peerPort = quint16(IP_MULTICAST_GROUP_PORT)){

        QFile file(systemInfoFilePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
            //TODO
            //sendClientMessagePacket(peerAddress, peerPort, localComputerName, "Can not read system info file!");
            return 0;
        }
        QTextStream in(&file);
        QString info = in.readAll();

        //后两个参数必须为空，才能发送给本机
        //The last two parameters must be null to send the packet to itself!
        //UDPPacket *packet = new UDPPacket(adminAddress, adminPort);
        //Packet *packet = packetHandlerBase->getPacket(peerAddress, peerPort);

        QHostAddress address = QHostAddress(peerAddress);
        quint16 port = peerPort;
        if(address.isNull()){
            if(serverAddress.isNull()){
                return 0;
            }
            address = serverAddress;
            port = localIPMCListeningPort;
        }
        if(port == 0){
            port = quint16(IP_MULTICAST_GROUP_PORT);
        }

        Packet *packet = m_packetHandlerBase->getPacket(address, port, localIPMCListeningAddress, localIPMCListeningPort);

        
        //TCPPacket *packet = new TCPPacket(adminAddress, adminPort, localIPMCListeningAddress.toString(), localIPMCListeningPort);

        packet->setPacketType(quint8(MS::ClientResponseClientDetailedInfo));
        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES)*2);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);

        out << m_localComputerName << info;

        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

        qDebug()<<"----sendClientResponseClientDetailedInfoPacket(...) Host:"<<address.toString()<<" Port:"<<port;


        return packet->getPacketSerialNumber();

    }

    quint16 sendClientResponseAdminConnectionResultPacket(const QHostAddress adminAddress, quint16 adminPort, bool result, const QString &message){
        //qWarning()<<"----sendClientResponseVerifyInfoResultPacket(...):"<<adminAddress.toString()<<" "<<adminPort;

        //后两个参数必须为空，才能发送给本机
        //The last two parameters must be null to send the packet to itself!
        //UDPPacket *packet = new UDPPacket(adminAddress, adminPort);
        Packet *packet = m_packetHandlerBase->getPacket(adminAddress, adminPort);
        
        packet->setPacketType(quint8(MS::ClientResponseAdminConnectionResult));
        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);

        out << m_localComputerName << result << message;

        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

        return packet->getPacketSerialNumber();

    }

    quint16 sendClientMessagePacket(const QHostAddress adminAddress, quint16 adminPort, const QString &message){

        //后两个参数必须为空，才能发送给本机
        //The last two parameters must be null to send the packet to itself!
        //UDPPacket *packet = new UDPPacket(adminAddress, adminPort);
        Packet *packet = m_packetHandlerBase->getPacket(adminAddress, adminPort);
        
        packet->setPacketType(quint8(MS::ClientMessage));
        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);

        out << m_localComputerName << message  ;

        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

        return packet->getPacketSerialNumber();

    } 

    
    
    
    
    quint16 sendLocalServiceServerDeclarePacket(const QHostAddress userAddress, quint16 userPort){

        //后两个参数必须为空，才能发送给本机
        //The last two parameters must be null to send the packet to itself!
        //UDPPacket *packet = new UDPPacket(adminAddress, adminPort);
        Packet *packet = m_packetHandlerBase->getPacket(userAddress, userPort);
        
        packet->setPacketType(quint8(MS::LocalServiceServerDeclare));
        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);

        out << m_localComputerName  ;

        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

        return packet->getPacketSerialNumber();

    }
    
    void requestRemoteAssistance(const QString &adminAddress, quint16 adminPort, const QString &adminName){

        foreach (QString userName, localUsersHash.keys()) {
            if(userName.trimmed().isEmpty()){
                continue;
            }
            sendRequestRemoteAssistancePacket(QHostAddress::LocalHost, localUsersHash.value(userName), adminAddress, adminPort, adminName);

        }
        
    }

    quint16 sendRequestRemoteAssistancePacket( const QHostAddress userAddress, quint16 userPort, const QString &adminAddress, quint16 adminPort, const QString &adminName){

        //后两个参数必须为空，才能发送给本机
        //The last two parameters must be null to send the packet to itself!
        //UDPPacket *packet = new UDPPacket(adminAddress, adminPort);
        Packet *packet = m_packetHandlerBase->getPacket(userAddress, userPort);
        
        packet->setPacketType(quint8(MS::AdminRequestRemoteAssistance));
        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);

        out << m_localComputerName << adminAddress << adminPort << adminName ;

        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

        return packet->getPacketSerialNumber();

    }

    quint16 sendInformUpdatePasswordPacket(const QString &userName, const QString &adminAddress, quint16 adminPort, const QString &adminName, const QString &oldPassword, const QString &newPassword){
        //qWarning()<<"sendInformUpdatePasswordPacket(...)"<<" userName:"<<userName<<" Port:"<<localUsersHash.value(userName) << " newPassword:"<<newPassword;
        
        //后两个参数必须为空，才能发送给本机
        //The last two parameters must be null to send the packet to itself!
        //UDPPacket *packet = new UDPPacket(adminAddress, adminPort);
        Packet *packet = m_packetHandlerBase->getPacket(QHostAddress::LocalHost, localUsersHash.value(userName));
        
        packet->setPacketType(quint8(MS::InformUserNewPassword));
        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);

        out << m_localComputerName << adminAddress << adminPort << adminName << oldPassword << newPassword ;

        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

        return packet->getPacketSerialNumber();

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
    
    quint16 sendServerAnnouncementPacket(const QHostAddress userAddress, quint16 userPort, const QString &adminName, quint32 announcementID, const QString &serverAnnouncement){
        //qWarning()<<"sendServerAnnouncementPacket(...)"<<" userAddress:"<<userAddress.toString()<<" Port:"<<userPort;
        
        //后两个参数必须为空，才能发送给本机
        //The last two parameters must be null to send the packet to itself!
        //UDPPacket *packet = new UDPPacket(adminAddress, adminPort);
        Packet *packet = m_packetHandlerBase->getPacket(userAddress, userPort);
        
        packet->setPacketType(quint8(MS::ServerAnnouncement));
        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);

        out << m_localComputerName << adminName << announcementID << serverAnnouncement ;

        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

        return packet->getPacketSerialNumber();

    }
    
    
    
private slots:
    void startHeartbeat(int interval = HEARTBEAT_TIMER_INTERVAL);
    void stopHeartbeat();


signals:
    void  signalHeartbeatPacketReceived(const QString &computerName);
    void  signalConfirmationOfReceiptPacketReceived(quint16 packetSerialNumber1, quint16 packetSerialNumber2);

    //    void signalClientLookForServerPacketReceived(const QHostAddress clientAddress, quint16 clientPort, const QString &clientName);
    void signalServerDeclarePacketReceived(const QString &serverAddress, quint16 serverTCPListeningPort, const QString &serverName, const QString &version);

    //    void signalClientOnlinePacketReceived(const QHostAddress clientAddress, quint16 clientPort, const QString &clientName);
    //    void signalClientOfflinePacketReceived(const QHostAddress clientAddress, quint16 clientPort, const QString &clientName);

    void signalServerOnlinePacketReceived(const QHostAddress serverAddress, quint16 serverPort, const QString &serverName);
    void signalServerOfflinePacketReceived(const QHostAddress serverAddress, quint16 serverPort, const QString &serverName);

    void signalClientDetailedInfoRequestedPacketReceived(const QString &computerName, bool rescan, const QString &adminAddress, quint16 adminPort);
    void signalAdminRequestRemoteConsolePacketReceived(const QString &computerName, const QString &applicationPath, const QString &adminID, bool startProcess, const QString &adminAddress, quint16 adminPort);
    //    void signalClientResponseRemoteConsolePacketReceived(bool accept);
    void signalRemoteConsoleCMDFromServerPacketReceived(const QString &computerName, const QString &command, const QString &adminAddress, quint16 adminPort);
    //    void signalRemoteConsoleCMDResultFromClientPacketReceived(const QString &result);

    void signalServerRequestClientSummaryInfoPacketReceived(const QString &groupName, const QString &computerName, const QString &userName);
    //    void signalClientResponseClientInfoPacketReceived(const QString &clientInfo);

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
    quint16 serverTCPListeningPort;
    QString serverName;

    quint16 clientLookForServerPacketSerialNumber;

    QHostAddress localIPMCListeningAddress;
    quint16 localIPMCListeningPort;

    QTimer *heartbeatTimer;
    //        QTimer *processWaitingForReplyPacketsTimer;

    PacketHandlerBase *m_packetHandlerBase;
    //ClientNetworkManager *networkManager;

    QDateTime serverLastOnlineTime;

    QMutex mutex;

    QString m_localComputerName;

    QHash<QString /*User Name*/, quint16 /*Port*/> localUsersHash;

    QMultiHash<QString /*Peer ID*/, QPair<quint16 /*Packet Serial Number*/, QDateTime/*Received Time*/> > m_receivedPacketsHash;




};

}

#endif /* CLIENTPACKETSPARSER_H_ */
