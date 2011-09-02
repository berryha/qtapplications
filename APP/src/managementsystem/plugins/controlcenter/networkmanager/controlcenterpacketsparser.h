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

    void setLocalUDPListeningAddress(const QHostAddress &address = QHostAddress::Any);
    void setLocalUDPListeningPort(quint16 port);


    //	void parseIncomingPackets();

    virtual void parseIncomingPacketData(Packet *packet);
    virtual void run();




    quint16 sendConfirmationOfReceiptPacket(const QHostAddress peerAddress, quint16 peerPort, quint16 packetSerialNumber, const QString &peerID){
        qDebug()<<"----sendConfirmationOfReceiptPacket(...)";

        UDPPacket *packet = new UDPPacket(peerAddress.toString(), peerPort, localUDPListeningAddress.toString(), localUDPListeningPort);
        packet->setPacketType(quint8(HEHUI::ConfirmationOfReceiptPacket));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << packetSerialNumber << getLastReceivedPacketSN(peerID);
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

        m_receivedPacketsHash.insert(peerID, qMakePair(packetSerialNumber, QDateTime::currentDateTime()));

        return packet->getPacketSerialNumber();
    }

    quint16 sendClientLookForServerPacket(){
        qDebug()<<"----sendClientLookForServerPacket(...)";
        //qWarning()<<"ipmcListeningAddress:"<<ipmcGroupAddress.toString()<<" ipmcListeningPort"<<ipmcListeningPort<<" localUDPListeningPort"<<localUDPListeningPort;


        UDPPacket *packet = new UDPPacket(ipmcGroupAddress, ipmcListeningPort, localUDPListeningAddress, localUDPListeningPort);
        packet->setPacketType(quint8(MS::ClientLookForServer));
        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES*10));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        //out << clientTCPListeningPort << clientName;
        out << m_localID;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

        return packet->getPacketSerialNumber();

    }

    quint16 sendClientOnlinePacket(const QHostAddress clientTCPListeningAddress, quint16 clientTCPListeningPort, const QString &clientName, bool isAdmin){
        qDebug()<<"----sendClientOnlinePacket(...)";

        UDPPacket *packet = new UDPPacket(serverAddress, ipmcListeningPort, localUDPListeningAddress, localUDPListeningPort);
        packet->setPacketType(quint8(MS::ClientOnline));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << clientTCPListeningAddress.toString() << clientTCPListeningPort <<clientName << isAdmin;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

        return packet->getPacketSerialNumber();
    }

    quint16 sendClientOfflinePacket(const QHostAddress clientTCPListeningAddress, quint16 clientTCPListeningPort, const QString &clientName, bool isAdmin){
        qDebug()<<"----sendClientOfflinePacket(...)";

        UDPPacket *packet = new UDPPacket(serverAddress, ipmcListeningPort, localUDPListeningAddress, localUDPListeningPort);
        packet->setPacketType(quint8(MS::ClientOffline));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << clientTCPListeningAddress.toString() << clientTCPListeningPort << clientName << isAdmin;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

        return packet->getPacketSerialNumber();

    }


    //    quint16 sendClientResponseRemoteConsolePacket(const QString &adminID, bool accept, const QString &extraMessage){
    //        Packet *packet = new Packet(serverAddress.toString(), serverPort);
    //        packet->setPacketType(quint8(MS::ClientResponseRemoteConsole));
    //        QByteArray ba;
    //        QDataStream out(&ba, QIODevice::WriteOnly);
    //        out.setVersion(QDataStream::Qt_4_6);
    //        out << adminID << (accept?quint8(1):quint8(0)) << extraMessage;
    //        packet->setPacketData(ba);
    //        packetHandlerBase->appendOutgoingPacket(packet);

    //        return packet->getPacketSerialNumber();

    //    }


    //    quint16 sendRemoteConsoleCMDResultFromClientPacket(quint16 packetSerialNumber, const QString &result){
    //        Packet *packet = new Packet(serverAddress.toString(), serverPort);
    //        packet->setPacketType(quint8(MS::RemoteConsoleCMDResultFromClient));
    //        packet->setPacketSerialNumber(packetSerialNumber);
    //        QByteArray ba;
    //        QDataStream out(&ba, QIODevice::WriteOnly);
    //        out.setVersion(QDataStream::Qt_4_6);
    //        out << result;
    //        packet->setPacketData(ba);
    //        packetHandlerBase->appendOutgoingPacket(packet);

    //        return packet->getPacketSerialNumber();

    //    }

    //    quint16 sendClientRequestSoftwareVersionPacket(const QString &softwareName){
    //        Packet *packet = new Packet(serverAddress.toString(), serverPort);
    //        packet->setPacketType(quint8(MS::ClientRequestSoftwareVersion));
    //        QByteArray ba;
    //        QDataStream out(&ba, QIODevice::WriteOnly);
    //        out.setVersion(QDataStream::Qt_4_6);
    //        out << softwareName;
    //        packet->setPacketData(ba);
    //        packetHandlerBase->appendOutgoingPacket(packet);

    //        return packet->getPacketSerialNumber();

    //    }

    //    quint16 sendServerAnnouncementPacket(const QString &groupName, const QString &computerName, const QString &announcement, bool mustRead = true){
    //        UDPPacket *packet = new UDPPacket(ipmcGroupAddress, ipmcListeningPort, localUDPListeningAddress, localUDPListeningPort);
    //        packet->setPacketType(quint8(MS::ServerAnnouncement));
    //        QByteArray ba;
    //        QDataStream out(&ba, QIODevice::WriteOnly);
    //        out.setVersion(QDataStream::Qt_4_6);
    //        out << groupName << QChar(PACKET_DATA_SEPARTOR) << computerName << QChar(PACKET_DATA_SEPARTOR) << announcement << (mustRead?quint8(1):quint8(0));
    //        packet->setPacketData(ba);
    //        packetHandlerBase->appendOutgoingPacket(packet);

    //        return packet->getPacketSerialNumber();

    //    }


    /////////////////////////////////////////////////////

    quint16 sendRequestClientDetailedInfoPacket(const QHostAddress &peerAddress, const QString &computerName, bool rescan){
        //qWarning()<<"peerAddress:"<<peerAddress.toString()<<" ipmcListeningPort"<<ipmcListeningPort<<" localUDPListeningPort"<<localUDPListeningPort;

        UDPPacket *packet = new UDPPacket(peerAddress, ipmcListeningPort, localUDPListeningAddress, localUDPListeningPort);
        packet->setPacketType(quint8(MS::ClientDetailedInfoRequested));
        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);

        out << m_localID << computerName << rescan;

        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);
        qDebug()<<"sendAdminRequestClientInfoPacket";
        return packet->getPacketSerialNumber();
    }

    quint16 sendAdminRequestRemoteConsolePacket(const QHostAddress &peerAddress, const QString &computerName, const QString &applicationPath, const QString &adminID, bool startProcess = true){
        qDebug()<<"----sendServerRequestRemoteConsolePacket(...)";

        UDPPacket *packet = new UDPPacket(peerAddress, ipmcListeningPort, localUDPListeningAddress, localUDPListeningPort);
        packet->setPacketType(quint8(MS::AdminRequestRemoteConsole));
        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);

        out << m_localID << computerName << applicationPath << adminID << startProcess;

        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

        return packet->getPacketSerialNumber();

    }

    quint16 sendRemoteConsoleCMDFromAdminPacket(const QHostAddress &peerAddress, const QString &computerName, const QString &command){
        qDebug()<<"----sendRemoteConsoleCMDFromServerPacket(...)";

        UDPPacket *packet = new UDPPacket(peerAddress, ipmcListeningPort, localUDPListeningAddress, localUDPListeningPort);
        packet->setPacketType(quint8(MS::RemoteConsoleCMDFromAdmin));
        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);

        out << m_localID << computerName << command;

        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

        return packet->getPacketSerialNumber();

    }

    quint16 sendSetupUSBSDPacket(const QHostAddress &peerAddress, const QString &computerName, const QString &userName, bool enable, bool temporarilyAllowed, const QString &adminName){
        //qWarning()<<"peerAddress:"<<peerAddress.toString()<<" ipmcListeningPort"<<ipmcListeningPort<<" localUDPListeningPort"<<localUDPListeningPort;

        UDPPacket *packet = new UDPPacket(peerAddress, ipmcListeningPort, localUDPListeningAddress, localUDPListeningPort);
        packet->setPacketType(quint8(MS::SetupUSBSD));
        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << computerName << userName << enable << temporarilyAllowed << adminName;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);
        qDebug()<<"sendSetupUSBSDPacket";
        return packet->getPacketSerialNumber();
    }

    quint16 sendSetupProgramesPacket(const QHostAddress &peerAddress, const QString &computerName, const QString &userName, bool enable, bool temporarilyAllowed, const QString &adminName){
        UDPPacket *packet = new UDPPacket(peerAddress, ipmcListeningPort, localUDPListeningAddress, localUDPListeningPort);
        packet->setPacketType(quint8(MS::SetupProgrames));
        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << computerName << userName << enable << temporarilyAllowed << adminName;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

        return packet->getPacketSerialNumber();
    }

    quint16 sendShowAdminPacket(const QHostAddress &peerAddress, const QString &computerName, const QString &userName, bool show){
        UDPPacket *packet = new UDPPacket(peerAddress, ipmcListeningPort, localUDPListeningAddress, localUDPListeningPort);
        packet->setPacketType(quint8(MS::ShowAdmin));
        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << computerName << userName << show;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

        return packet->getPacketSerialNumber();
    }

    quint16 sendModifyAdminGroupUserPacket(const QHostAddress &peerAddress, const QString &computerName, const QString &userName, bool addToAdminGroup, const QString &adminName){
        UDPPacket *packet = new UDPPacket(peerAddress, ipmcListeningPort, localUDPListeningAddress, localUDPListeningPort);
        packet->setPacketType(quint8(MS::ModifyAdminGroupUser));
        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << computerName << userName << addToAdminGroup  << adminName;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

        return packet->getPacketSerialNumber();
    }

    quint16 sendAdminRequestConnectionToClientPacket(const QHostAddress &peerAddress, const QString &computerName, const QString &users){

        UDPPacket *packet = new UDPPacket(peerAddress, ipmcListeningPort, localUDPListeningAddress, localUDPListeningPort);
        packet->setPacketType(quint8(MS::AdminRequestConnectionToClient));
        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES)*2);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << computerName << users ;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

        qWarning()<<"sendAdminRequestVerifyClientInfoPacket:"<<peerAddress.toString()<<" "<<ipmcListeningPort;

        return packet->getPacketSerialNumber();
    }
    
    quint16 sendAdminSearchClientPacket(const QHostAddress &targetAddress, const QString &computerName, const QString &userName, const QString &workgroup, const QString &macAddress, const QString &ipAddress, const QString &osVersion, const QString &adminName){
        //UDPPacket *packet = new UDPPacket(ipmcGroupAddress, ipmcListeningPort, localUDPListeningAddress, localUDPListeningPort);
        UDPPacket *packet = new UDPPacket(targetAddress, ipmcListeningPort, localUDPListeningAddress, localUDPListeningPort);
        packet->setPacketType(quint8(MS::AdminSearchClient));
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << computerName << userName << workgroup << macAddress << ipAddress << osVersion << adminName;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

        return packet->getPacketSerialNumber();
    }
    
    quint16 sendRemoteAssistancePacket(const QHostAddress &peerAddress, const QString &computerName, const QString &adminName){
        UDPPacket *packet = new UDPPacket(peerAddress, ipmcListeningPort, localUDPListeningAddress, localUDPListeningPort);
        packet->setPacketType(quint8(MS::AdminRequestRemoteAssistance));
        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << computerName << adminName;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

        return packet->getPacketSerialNumber();
    }

    quint16 sendUpdateMSUserPasswordPacket(const QString &peerAddress, const QString &workgroupName, const QString &adminName){
        QHostAddress targetAddress = QHostAddress(peerAddress);
        if(targetAddress.isNull()){
            targetAddress = ipmcGroupAddress;
        }
        UDPPacket *packet = new UDPPacket(targetAddress, ipmcListeningPort, localUDPListeningAddress, localUDPListeningPort);
        packet->setPacketType(quint8(MS::UpdateMSWUserPassword));
        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << workgroupName << adminName;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

        return packet->getPacketSerialNumber();
    }
    
    quint16 sendInformUpdatePasswordPacket(const QString &peerAddress, const QString &workgroupName, const QString &adminName){

        QHostAddress targetAddress = QHostAddress(peerAddress);
        if(targetAddress.isNull()){
            targetAddress = ipmcGroupAddress;
        }
        
        //后两个参数必须为空，才能发送给本机
        //The last two parameters must be null to send the packet to itself!
        //UDPPacket *packet = new UDPPacket(adminAddress, adminPort);
        Packet *packet = m_packetHandlerBase->getPacket(targetAddress, ipmcListeningPort, localUDPListeningAddress, localUDPListeningPort);
        
        packet->setPacketType(quint8(MS::InformUserNewPassword));
        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);

        out << m_localID << workgroupName <<  adminName  ;

        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

        return packet->getPacketSerialNumber();

    }
    
    quint16 sendAnnouncementPacket(const QString &peerAddress, const QString &groupName, const QString &computerName, quint32 announcementID, const QString &announcement, const QString &adminName, bool mustRead = true){

        QHostAddress targetAddress = QHostAddress(peerAddress);
        if(targetAddress.isNull()){
            targetAddress = ipmcGroupAddress;
        }
        
        //UDPPacket *packet = packetHandlerBase->getUDPPacket(targetAddress, ipmcListeningPort, localUDPListeningAddress, localUDPListeningPort);
        Packet *packet = m_packetHandlerBase->getPacket(targetAddress, ipmcListeningPort, localUDPListeningAddress, localUDPListeningPort);
        
        //Packet *packet = packetHandlerBase->getPacket(QHostAddress(IP_MULTICAST_GROUP_ADDRESS), quint16(IP_MULTICAST_GROUP_PORT), localIPMCListeningAddress, localIPMCListeningPort);

        packet->setPacketType(quint8(MS::ServerAnnouncement));
        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << groupName << computerName << announcementID << announcement << adminName << (mustRead?quint8(1):quint8(0));
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

        return packet->getPacketSerialNumber();

    }

    //    quint16 sendAdminLookForClientPacket(const QString &clientComputerName, const QString &workgroupName, const QString &ipAddress, const QString &macAddress, const QString &usersInfo, const QString &osInfo, bool usbsdEnabled, bool programesEnabled, const QString &clientVersion){
    //        qDebug()<<"----sendAdminLookForClientPacket(...)";

    //        QHostAddress address = QHostAddress(ipAddress);
    //        if(address.isNull()){
    //            address = QHostAddress(QString(IP_MULTICAST_GROUP_ADDRESS));
    //        }


    //        UDPPacket *packet = new UDPPacket(address, ipmcListeningPort, localIPMCListeningAddress, localIPMCListeningPort);
    //        packet->setPacketType(quint8(MS::AdminLookForClient));
    //        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES)*2);
    //        QByteArray ba;
    //        QDataStream out(&ba, QIODevice::WriteOnly);
    //        out.setVersion(QDataStream::Qt_4_6);

    //        out << clientComputerName << workgroupName << macAddress << usersInfo << osInfo <<usbsdEnabled << programesEnabled << clientVersion ;

    //        packet->setPacketData(ba);
    //        packetHandlerBase->appendOutgoingPacket(packet);

    //        return packet->getPacketSerialNumber();

    //    }





signals:
    void  signalHeartbeatPacketReceived(const QString &computerName);
    void  signalConfirmationOfReceiptPacketReceived(quint16 packetSerialNumber1, quint16 packetSerialNumber2);

    //    void signalClientLookForServerPacketReceived(const QHostAddress clientAddress, quint16 clientPort, const QString &clientName);
    void signalServerDeclarePacketReceived(const QString &serverAddress, quint16 serverTCPListeningPort, const QString &serverName, const QString &version);

    //    void signalClientOnlinePacketReceived(const QHostAddress clientAddress, quint16 clientPort, const QString &clientName);
    //    void signalClientOfflinePacketReceived(const QHostAddress clientAddress, quint16 clientPort, const QString &clientName);

    void signalServerOnlinePacketReceived(const QHostAddress serverAddress, quint16 serverPort, const QString &serverName);
    void signalServerOfflinePacketReceived(const QHostAddress serverAddress, quint16 serverPort, const QString &serverName);

    //    void signalAdminLoggedOnToServerRequestPacketReceived(const QHostAddress adminAddress, quint16 adminPort, const QString &adminID);
    //    void signalServerRequestRemoteConsolePacketReceived(const QString &adminID);
    void signalClientResponseRemoteConsoleStatusPacketReceived(const QString &computerName, bool accept, const QString &extraMessage);
    //    void signalRemoteConsoleCMDFromServerPacketReceived(const QString &command);
    void signalRemoteConsoleCMDResultFromClientPacketReceived(const QString &computerName, quint16 packetSerialNumber, const QString &result);



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
    void startHeartbeat(int interval = HEARTBEAT_TIMER_INTERVAL);
    void heartbeat();
    void confirmPacketReceipt(quint16 packetSerialNumber);

private:
    quint16 getLastReceivedPacketSN(const QString &peerID);

private:
    QHostAddress serverAddress;
    quint16 serverTCPListeningPort;
    QString serverName;

    QHostAddress ipmcGroupAddress;
    quint16 ipmcListeningPort;

    QHostAddress localUDPListeningAddress;
    quint16 localUDPListeningPort;

    QTimer *heartbeatTimer;
    //        QTimer *processWaitingForReplyPacketsTimer;

    PacketHandlerBase *m_packetHandlerBase;
    //NetworkManagerInstance *networkManager;

    QMutex mutex;

    QString m_localComputerName;
    QString m_localID;

    QMultiHash<QString /*Peer ID*/, QPair<quint16 /*Packet Serial Number*/, QDateTime/*Received Time*/> > m_receivedPacketsHash;





};

}

#endif /* CONTROLCENTERPACKETSPARSER_H_ */
