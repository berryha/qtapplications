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


#include <QDataStream>
#include <QHostAddress>
#include <QDebug>

#include "../../sharedms/global_shared.h"
#include "resourcesmanagerinstance.h"


#include "HHSharedCore/hcryptography.h"



namespace HEHUI {


class ControlCenterPacketsParser : public QObject{
    Q_OBJECT
public:
    ControlCenterPacketsParser(ResourcesManagerInstance *manager, QObject *parent = 0);
    virtual ~ControlCenterPacketsParser();

public slots:

    void parseIncomingPacketData(Packet *packet);



    bool sendClientLookForServerPacket(const QString &targetAddress = QString(IP_MULTICAST_GROUP_ADDRESS)){
        qDebug()<<"----sendClientLookForServerPacket(...) "<<" targetAddress:"<<targetAddress;

        QHostAddress address = QHostAddress(targetAddress);
        if(address.isNull()){
            address = QHostAddress(QString(IP_MULTICAST_GROUP_ADDRESS));
        }

        Packet *packet = PacketHandlerBase::getPacket();

        packet->setPacketType(quint8(MS::ClientLookForServer));
        packet->setTransmissionProtocol(TP_UDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_localComputerName << m_udpServer->localPort() << QString(APP_VERSION);
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_udpServer->sendDatagram(ba, address, quint16(IP_MULTICAST_GROUP_PORT));

    }

    bool sendAdminOnlineStatusChangedPacket(int socketID, const QString &clientName, const QString &adminName, bool online){
        qDebug()<<"----sendAdminOnlineStatusChangedPacket(...)";

        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(online?quint8(MS::AdminOnline):quint8(MS::AdminOffline));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << clientName << adminName;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_rtp->sendReliableData(socketID, &ba);

    }




    bool sendRequestClientDetailedInfoPacket(int socketID, const QString &computerName, bool rescan){

        Packet *packet = PacketHandlerBase::getPacket(socketID);
        packet->setPacketType(quint8(MS::ClientDetailedInfoRequested));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << localUDTListeningPort << computerName << rescan;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_rtp->sendReliableData(socketID, &ba);

    }

    bool sendAdminRequestRemoteConsolePacket(int socketID, const QString &computerName, const QString &applicationPath, const QString &adminID, bool startProcess = true){
        qDebug()<<"----sendServerRequestRemoteConsolePacket(...)";

        Packet *packet = PacketHandlerBase::getPacket(socketID);
        packet->setPacketType(quint8(MS::AdminRequestRemoteConsole));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << computerName << applicationPath << adminID << startProcess;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendRemoteConsoleCMDFromAdminPacket(int socketID, const QString &computerName, const QString &command){
        qDebug()<<"----sendRemoteConsoleCMDFromServerPacket(...)";

        Packet *packet = PacketHandlerBase::getPacket(socketID);
        packet->setPacketType(quint8(MS::RemoteConsoleCMDFromAdmin));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << computerName << command;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendSetupUSBSDPacket(int socketID, const QString &computerName, const QString &userName, bool enable, bool temporarilyAllowed, const QString &adminName){

        Packet *packet = PacketHandlerBase::getPacket(socketID);
        packet->setPacketType(quint8(MS::AdminRequestSetupUSBSD));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << computerName << userName << enable << temporarilyAllowed << adminName;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendSetupProgramesPacket(int socketID, const QString &computerName, const QString &userName, bool enable, bool temporarilyAllowed, const QString &adminName){

        Packet *packet = PacketHandlerBase::getPacket(socketID);
        packet->setPacketType(quint8(MS::AdminRequestSetupProgrames));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << computerName << userName << enable << temporarilyAllowed << adminName;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendShowAdminPacket(int socketID, const QString &computerName, const QString &userName, bool show){

        Packet *packet = PacketHandlerBase::getPacket(socketID);
        packet->setPacketType(quint8(MS::ShowAdmin));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << computerName << userName << show;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendModifyAdminGroupUserPacket(int socketID, const QString &computerName, const QString &userName, bool addToAdminGroup, const QString &adminName){

        Packet *packet = PacketHandlerBase::getPacket(socketID);
        packet->setPacketType(quint8(MS::ModifyAdminGroupUser));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << computerName << userName << addToAdminGroup  << adminName;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendRenameComputerPacket(int socketID, const QString &oldComputerName, const QString &newComputerName, const QString &adminName){

        Packet *packet = PacketHandlerBase::getPacket(socketID);
        packet->setPacketType(quint8(MS::RenameComputer));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << oldComputerName << newComputerName << adminName;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendJoinOrUnjoinDomainPacket(int socketID, const QString &computerName, const QString &adminName, bool join, const QString &domainOrWorkgroupName){

        Packet *packet = PacketHandlerBase::getPacket(socketID);
        packet->setPacketType(quint8(MS::JoinOrUnjoinDomain));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << computerName << adminName << join << domainOrWorkgroupName;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_rtp->sendReliableData(socketID, &ba);
    }


    bool sendAdminRequestConnectionToClientPacket(int socketID, const QString &computerName, const QString &users){

        Packet *packet = PacketHandlerBase::getPacket(socketID);
        packet->setPacketType(quint8(MS::AdminRequestConnectionToClient));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << computerName << users ;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_rtp->sendReliableData(socketID, &ba);
    }
    
    bool sendAdminSearchClientPacket(const QHostAddress &targetAddress, const QString &computerName, const QString &userName, const QString &workgroup, const QString &macAddress, const QString &ipAddress, const QString &osVersion, const QString &adminName){

        Packet *packet = PacketHandlerBase::getPacket();

        packet->setPacketType(quint8(MS::AdminSearchClient));
        packet->setTransmissionProtocol(TP_UDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << computerName << userName << workgroup << macAddress << ipAddress << osVersion << adminName;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_udpServer->sendDatagram(ba, targetAddress, quint16(IP_MULTICAST_GROUP_PORT));
    }
    
    bool sendRemoteAssistancePacket(int socketID, const QString &computerName, const QString &adminName){

        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::AdminRequestRemoteAssistance));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << computerName << adminName;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendUpdateMSUserPasswordPacket(const QString &peerAddress, quint16 peerPort, const QString &workgroupName, const QString &adminName){

        Packet *packet = PacketHandlerBase::getPacket();
        QHostAddress targetAddress = QHostAddress(peerAddress);
        if(targetAddress.isNull()){
            targetAddress = ipmcGroupAddress;
        }

        packet->setPacketType(quint8(MS::UpdateMSWUserPassword));
        packet->setTransmissionProtocol(TP_UDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << workgroupName << adminName;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_udpServer->sendDatagram(ba, targetAddress, peerPort);

    }
    
    bool sendUpdateMSUserPasswordPacket(int socketID, const QString &workgroupName, const QString &adminName){

        Packet *packet = PacketHandlerBase::getPacket(socketID);
        packet->setPacketType(quint8(MS::UpdateMSWUserPassword));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << workgroupName << adminName;
        packet->setPacketData(ba);


        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_rtp->sendReliableData(socketID, &ba);

    }


    bool sendInformUpdatePasswordPacket(const QString &peerAddress, quint16 peerPort, const QString &workgroupName, const QString &adminName){

        Packet *packet = PacketHandlerBase::getPacket();
        QHostAddress targetAddress = QHostAddress(peerAddress);
        if(targetAddress.isNull()){
            targetAddress = ipmcGroupAddress;
        }

        packet->setPacketType(quint8(MS::InformUserNewPassword));
        packet->setTransmissionProtocol(TP_UDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << workgroupName <<  adminName  ;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_udpServer->sendDatagram(ba, targetAddress, peerPort);

    }
    
    bool sendInformUpdatePasswordPacket(int socketID, const QString &workgroupName, const QString &adminName){

        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::InformUserNewPassword));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << workgroupName <<  adminName  ;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_rtp->sendReliableData(socketID, &ba);

    }


    bool sendAnnouncementPacket(const QString &peerAddress, quint16 peerPort, const QString &groupName, const QString &computerName, quint32 announcementID, const QString &announcement, const QString &adminName, bool mustRead = true){
        qDebug()<<"--sendAnnouncementPacket(...) "<<" peerAddress:"<<peerAddress<<" computerName:"<<computerName;
        Packet *packet = PacketHandlerBase::getPacket();
        QHostAddress targetAddress = QHostAddress(peerAddress);
        if(targetAddress.isNull()){
            targetAddress = ipmcGroupAddress;
        }
        
        packet->setPacketType(quint8(MS::ServerAnnouncement));
        packet->setTransmissionProtocol(TP_UDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << groupName << computerName << announcementID << announcement << adminName << (mustRead?quint8(1):quint8(0));
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_udpServer->sendDatagram(ba, targetAddress, peerPort);

    }

    bool sendAnnouncementPacket(int socketID, const QString &groupName, const QString &computerName, quint32 announcementID, const QString &announcement, const QString &adminName, bool mustRead = true){

        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::ServerAnnouncement));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << groupName << computerName << announcementID << announcement << adminName << (mustRead?quint8(1):quint8(0));
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_rtp->sendReliableData(socketID, &ba);

    }




    //////////////////////////////
    bool requestFileSystemInfo(int socketID, const QString &parentDirPath){
        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::RequestFileSystemInfo));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_localComputerName << parentDirPath;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool requestUploadFile(int socketID, const QByteArray &fileMD5Sum, const QString &fileName, quint64 size, const QString &remoteFileSaveDir = "./"){
        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::RequestUploadFile));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_localComputerName << fileMD5Sum << fileName << size << remoteFileSaveDir;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool requestDownloadFile(int socketID, const QString &remoteBaseDir, const QString &remoteFileName, const QString &localFileSaveDir){
        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::RequestDownloadFile));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_localComputerName << remoteBaseDir << remoteFileName << localFileSaveDir;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool acceptFileDownloadRequest(int socketID, const QString &fileName, bool accepted, const QByteArray &fileMD5Sum, quint64 size, const QString &remoteFileSaveDir){

        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::ResponseFileDownloadRequest));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_localComputerName << fileName << accepted << fileMD5Sum << size << remoteFileSaveDir;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_rtp->sendReliableData(socketID, &ba);
    }
    bool denyFileDownloadRequest(int socketID, const QString &fileName, bool accepted, const QString &message){
        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::ResponseFileDownloadRequest));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_localComputerName << fileName << accepted << message;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool responseFileUploadRequest(int socketID, const QByteArray &fileMD5Sum, bool accepted, const QString &message){
        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::ResponseFileUploadRequest));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_localComputerName << fileMD5Sum << accepted << message;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool requestFileData(int socketID, const QByteArray &fileMD5, int startPieceIndex, int endPieceIndex){
        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::RequestFileData));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_localComputerName << fileMD5 << startPieceIndex << endPieceIndex;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendFileData(int socketID, const QByteArray &fileMD5, int pieceIndex, const QByteArray *data, const QByteArray *sha1){
        qDebug()<<"--sendFileData(...)";

        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::FileData));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_localComputerName << fileMD5 << pieceIndex << *data << *sha1 ;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool fileTXStatusChanged(int socketID, const QByteArray &fileMD5, quint8 status){
        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::FileTXStatusChanged));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_localComputerName << fileMD5 << status ;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool fileTXError(int socketID, const QByteArray &fileMD5, quint8 errorCode, const QString &errorString){
        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::FileTXError));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_localComputerName << fileMD5 << errorCode << errorString ;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_rtp->sendReliableData(socketID, &ba);
    }


    QString lastErrorMessage(){return m_rtp->lastErrorString();}



signals:
    void  signalHeartbeatPacketReceived(const QString &computerName);
    void  signalConfirmationOfReceiptPacketReceived(quint16 packetSerialNumber1, quint16 packetSerialNumber2);

    //    void signalClientLookForServerPacketReceived(const QHostAddress clientAddress, quint16 clientPort, const QString &clientName);
    void signalServerDeclarePacketReceived(const QString &serverAddress, quint16 serverUDTListeningPort, quint16 serverTCPListeningPort, const QString &serverName, const QString &version, int serverInstanceID);

    //    void signalClientOnlinePacketReceived(const QHostAddress clientAddress, quint16 clientPort, const QString &clientName);
    //    void signalClientOfflinePacketReceived(const QHostAddress clientAddress, quint16 clientPort, const QString &clientName);

    void signalServerOnlinePacketReceived(const QHostAddress serverAddress, quint16 serverPort, const QString &serverName);
    void signalServerOfflinePacketReceived(const QHostAddress serverAddress, quint16 serverPort, const QString &serverName);

    void signalClientOnlineStatusChanged(int socketID, const QString &clientName, bool online);

    //    void signalAdminLoggedOnToServerRequestPacketReceived(const QHostAddress adminAddress, quint16 adminPort, const QString &adminID);
    //    void signalServerRequestRemoteConsolePacketReceived(const QString &adminID);
    void signalClientResponseRemoteConsoleStatusPacketReceived(const QString &computerName, bool accept, const QString &extraMessage, quint8 messageType);
    //    void signalRemoteConsoleCMDFromServerPacketReceived(const QString &command);
    void signalRemoteConsoleCMDResultFromClientPacketReceived(const QString &computerName, const QString &result);



    void signalClientResponseClientDetailedInfoPacketReceived(const QString &computerName, const QString &clientInfo);
    void signalClientResponseUSBInfoPacketReceived(int socketID, const QString &computerName, const QString &usbInfo);
    void signalClientResponseProgramesInfoPacketReceived(int socketID, const QString &computerName, const QString &usbInfo);



    void signalClientRequestSoftwareVersionPacketReceived(const QString &softwareName);
    void signalServerResponseSoftwareVersionPacketReceived(const QString &softwareName, const QString &version);

    //    void  signalServerAnnouncementPacketReceived(const QString &groupName, const QString &computerName, const QString &announcement, bool mustRead = true);


    void signalClientResponseAdminConnectionResultPacketReceived(int socketID, const QString &computerName, bool result, const QString &message);
    void signalClientMessagePacketReceived(const QString &computerName, const QString &message, quint8 clientMessageType);

    void signalUserResponseRemoteAssistancePacketReceived(const QString &userName, const QString &computerName, bool accept);
    void signalNewPasswordRetrevedByUserPacketReceived(const QString &userName, const QString &computerName);
    
    void signalClientResponseClientSummaryInfoPacketReceived(const QString &computerName, const QString &workgroupName, const QString &networkInfo, const QString &usersInfo, const QString &osInfo, bool usbsdEnabled, bool programesEnabled, const QString &admins, bool isJoinedToDomain, const QString &clientVersion);


    ///////////////////////////
    void signalFileSystemInfoReceived(int socketID, const QString &parentDirPath, const QByteArray &fileSystemInfoData);

    void signalAdminRequestUploadFile(int socketID, const QByteArray &fileMD5Sum, const QString &fileName, quint64 size, const QString &localFileSaveDir);
    void signalAdminRequestDownloadFile(int socketID, const QString &localBaseDir, const QString &fileName, const QString &remoteFileSaveDir);

    void signalFileDownloadRequestAccepted(int socketID, const QString &remoteFileName, const QByteArray &fileMD5Sum, quint64 size, const QString &localFileSaveDir);
    void signalFileDownloadRequestDenied(int socketID, const QString &remoteFileName, const QString &message);
    void signalFileUploadRequestResponsed(int socketID, const QByteArray &fileMD5Sum, bool accepted, const QString &message);

    void signalFileDataRequested(int socketID, const QByteArray &fileMD5, int startPieceIndex, int endPieceIndex);
    void signalFileDataReceived(int socketID, const QByteArray &fileMD5, int pieceIndex, const QByteArray &data, const QByteArray &sha1);
    void signalFileTXStatusChanged(int socketID, const QByteArray &fileMD5, quint8 status);
    void signalFileTXError(int socketID, const QByteArray &fileMD5, quint8 errorCode, const QString &errorString);


public slots:
    //HeartbeatPacket: PacketType+ComputerName+IP
    //    void startHeartbeat(int interval = HEARTBEAT_TIMER_INTERVAL);
    //    void heartbeat();
    //    void confirmPacketReceipt(quint16 packetSerialNumber);

private:
    quint16 getLastReceivedPacketSN(const QString &peerID);

private:
    QHostAddress serverAddress;
    quint16 serverUDTListeningPort;


    QString serverName;

    QHostAddress ipmcGroupAddress;
    quint16 ipmcListeningPort;

    quint16 localUDTListeningPort;
    quint16 m_localTCPServerListeningPort;


    PacketHandlerBase *m_packetHandlerBase;
    //NetworkManagerInstance *networkManager;


    QString m_localComputerName;
    QString m_localID;

    QMultiHash<QString /*Peer ID*/, QPair<quint16 /*Packet Serial Number*/, QDateTime/*Received Time*/> > m_receivedPacketsHash;

    ResourcesManagerInstance *m_resourcesManager;
    UDPServer *m_udpServer;
    RTP *m_rtp;
    UDTProtocol *m_udtProtocol;
    TCPServer *m_tcpServer;



};

}

#endif /* CONTROLCENTERPACKETSPARSER_H_ */
