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


#include <QDataStream>
#include <QHostAddress>
#include <QHostInfo>
#include <QDebug>
#include <QFile>
#include <QCryptographicHash>

#include "../clientresourcesmanager.h"

#include "HHSharedNetwork/hnetworkutilities.h"


namespace HEHUI {


class ClientPacketsParser : public QObject{
    Q_OBJECT
public:
    ClientPacketsParser(ClientResourcesManager *manager, QObject *parent = 0);
    virtual ~ClientPacketsParser();




public slots:
    void parseIncomingPacketData(Packet *packet);
    void localUserOffline(int socketID);



    bool sendClientLookForServerPacket(const QString &targetAddress = QString(IP_MULTICAST_GROUP_ADDRESS)){
        qDebug()<<"----sendClientLookForServerPacket(...) targetAddress:"<<targetAddress;

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

        return m_udpServer->sendDatagram(ba, address, IP_MULTICAST_GROUP_PORT);

    }

    bool sendClientOnlineStatusChangedPacket(int socketID, const QString &clientName, bool online){
        qDebug()<<"----sendClientOnlineStatusChangedPacket(...)";

        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(online?quint8(MS::ClientOnline):quint8(MS::ClientOffline));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localComputerName << clientName;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_rtp->sendReliableData(socketID, &ba);

    }

    bool sendClientResponseRemoteConsoleStatusPacket(int adminSocketID, bool running, const QString &extraMessage, quint8 messageType = quint8(MS::MSG_Information)){
        qDebug()<<"----sendClientResponseRemoteConsolePacket(...)";

        Packet *packet = PacketHandlerBase::getPacket(adminSocketID);

        packet->setPacketType(quint8(MS::ClientResponseRemoteConsoleStatus));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_localComputerName << (running?quint8(1):quint8(0)) << extraMessage << messageType;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_rtp->sendReliableData(adminSocketID, &ba);

    }

    bool sendRemoteConsoleCMDResultFromClientPacket(int adminSocketID, const QString &result){
        qDebug()<<"----sendRemoteConsoleCMDResultFromClientPacket(...)";


        Packet *packet = PacketHandlerBase::getPacket(adminSocketID);

        packet->setPacketType(quint8(MS::RemoteConsoleCMDResultFromClient));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_localComputerName << result;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_rtp->sendReliableData(adminSocketID, &ba);

    }

    bool sendClientResponseClientSummaryInfoPacket(int socketID, const QString &workgroupName, const QString &networkInfo, const QString &usersInfo, const QString &osInfo, bool usbsdEnabled, bool programesEnabled, const QString &admins, bool isJoinedToDomain){
        //qWarning()<<"----sendClientResponseClientInfoPacket(...)"<<" socketID:"<<socketID;

        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::ClientResponseClientSummaryInfo));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_localComputerName << workgroupName << networkInfo << usersInfo << osInfo <<usbsdEnabled << programesEnabled << admins << isJoinedToDomain << QString(APP_VERSION);
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_rtp->sendReliableData(socketID ,&ba);

    }

    bool sendClientResponseClientSummaryInfoPacket(const QString &targetAddress, quint16 targetPort, const QString &workgroupName, const QString &networkInfo, const QString &usersInfo, const QString &osInfo, bool usbsdEnabled, bool programesEnabled, const QString &admins, bool isJoinedToDomain){
        //qWarning()<<"----sendClientResponseClientInfoPacket(...)"<<" targetAddress:"<<targetAddress<<" targetPort:"<<targetPort;

        Packet *packet = PacketHandlerBase::getPacket();

        packet->setPacketType(quint8(MS::ClientResponseClientSummaryInfo));
        packet->setTransmissionProtocol(TP_UDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_localComputerName << workgroupName << networkInfo << usersInfo << osInfo <<usbsdEnabled << programesEnabled << admins << isJoinedToDomain << QString(APP_VERSION);
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_udpServer->sendDatagram(ba, QHostAddress(targetAddress), targetPort);

    }

    bool sendClientRequestSoftwareVersionPacket(int socketID, const QString &softwareName){
        qDebug()<<"----sendClientRequestSoftwareVersionPacket(...)";

        Packet *packet = PacketHandlerBase::getPacket(socketID);
        
        packet->setPacketType(quint8(MS::ClientRequestSoftwareVersion));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_localComputerName << softwareName;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendClientLogPacket(int socketID, const QString &users, quint8 logType, const QString &log){
        //qWarning()<<"----sendClientLogPacket(...) log:"<<log;



        Packet *packet = PacketHandlerBase::getPacket(socketID);
        
        packet->setPacketType(quint8(MS::ClientLog));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_localComputerName << users << logType << log << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendClientResponseClientDetailedInfoPacket(int socketID, const QString &systemInfoFilePath){

        QFile file(systemInfoFilePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
            //TODO
            //sendClientMessagePacket(peerAddress, peerPort, localComputerName, "Can not read system info file!");
            return false;
        }
        QTextStream in(&file);
        QString info = in.readAll();

        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::ClientResponseClientDetailedInfo));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_localComputerName << info;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_rtp->sendReliableData(socketID, &ba);

    }

    bool sendClientResponseAdminConnectionResultPacket(int socketID, bool result, const QString &message){
        //qWarning()<<"----sendClientResponseAdminConnectionResultPacket(...):"<<adminAddress.toString()<<" "<<adminPort;

        Packet *packet = PacketHandlerBase::getPacket(socketID);
        
        packet->setPacketType(quint8(MS::ClientResponseAdminConnectionResult));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_localComputerName << result << message;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendClientMessagePacket(int adminSocketID, const QString &message, quint8 clientMessageType = quint8(MS::MSG_Information)){


        Packet *packet = PacketHandlerBase::getPacket(adminSocketID);
        
        packet->setPacketType(quint8(MS::ClientMessage));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_localComputerName << message <<clientMessageType;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_rtp->sendReliableData(adminSocketID, &ba);
    }

    bool sendClientResponseUSBInfoPacket(int socketID, const QString &info){
        //qWarning()<<"----sendClientResponseSetupUSBInfoPacket(...):"<<adminAddress.toString()<<" "<<adminPort;

        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::ClientResponseUSBInfo));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_localComputerName << info;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendClientResponseProgramesInfoPacket(int socketID, const QString &info){
        //qWarning()<<"----ClientResponseProgramesInfo(...):"<<adminAddress.toString()<<" "<<adminPort;

        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::ClientResponseProgramesInfo));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_localComputerName << info;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_rtp->sendReliableData(socketID, &ba);
    }



    bool sendLocalServiceServerDeclarePacket(int userSocketID){


        Packet *packet = PacketHandlerBase::getPacket(userSocketID);
        
        packet->setPacketType(quint8(MS::LocalServiceServerDeclare));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_localComputerName  ;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_rtp->sendReliableData(userSocketID, &ba);
    }
    
    void requestRemoteAssistance(const QString &adminAddress, quint16 adminPort, const QString &adminName){

        foreach (int socketID, m_localUserSocketsHash.keys()) {
            sendRequestRemoteAssistancePacket(socketID, adminAddress, adminPort, adminName);
        }
        
    }

    bool sendRequestRemoteAssistancePacket( int userSocketID, const QString &adminAddress, quint16 adminPort, const QString &adminName){


        Packet *packet = PacketHandlerBase::getPacket(userSocketID);
        
        packet->setPacketType(quint8(MS::AdminRequestRemoteAssistance));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_localComputerName << adminAddress << adminPort << adminName ;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_rtp->sendReliableData(userSocketID, &ba);
    }


    bool sendInformUpdatePasswordPacket(const QString &userName, const QString &adminAddress, quint16 adminPort, const QString &adminName, const QString &oldPassword, const QString &newPassword){
        //qWarning()<<"sendInformUpdatePasswordPacket(...)"<<" userName:"<<userName<<" Port:"<<localUsersHash.value(userName) << " newPassword:"<<newPassword;
        
        int socketID = INVALID_SOCK_ID;
        if(!m_localUserSocketsHash.values().contains(userName)){
            return false;
        }
        socketID = m_localUserSocketsHash.key(userName);


        Packet *packet = PacketHandlerBase::getPacket(socketID);
        
        packet->setPacketType(quint8(MS::InformUserNewPassword));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_localComputerName << adminAddress << adminPort << adminName << oldPassword << newPassword ;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_rtp->sendReliableData(socketID, &ba);
    }
    
    void sendAnnouncement(const QString &adminName, quint32 announcementID, const QString &announcement){
        qDebug()<<"--sendAnnouncement(...)"<<" m_localUserSocketsHash.size():"<<m_localUserSocketsHash.size();

        foreach (int socketID, m_localUserSocketsHash.keys()) {
            sendServerAnnouncementPacket(socketID, adminName, announcementID, announcement);
        }

    }
    
    bool sendServerAnnouncementPacket(int userSocketID, const QString &adminName, quint32 announcementID, const QString &serverAnnouncement){
        qDebug()<<"--sendServerAnnouncementPacket(...)"<<" userSocketID:"<<userSocketID<<" adminName:"<<adminName<<" serverAnnouncement:"<<serverAnnouncement;
        
        Packet *packet = PacketHandlerBase::getPacket(userSocketID);
        
        packet->setPacketType(quint8(MS::ServerAnnouncement));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_localComputerName << adminName << announcementID << serverAnnouncement ;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_rtp->sendReliableData(userSocketID, &ba);
    }



//////////////////////////////
    bool responseFileSystemInfo(int socketID, QString parentDirPath, const QByteArray &fileSystemInfoData){
        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::ResponseFileSystemInfo));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_localComputerName << parentDirPath << fileSystemInfoData;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool requestUploadFile(int socketID, const QByteArray &fileMD5Sum, const QString &fileName, quint64 size, const QString &remoteFileSaveDir = ""){
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
        qDebug()<<"--requestFileData(...) "<<" startPieceIndex:"<<startPieceIndex<<" endPieceIndex:"<<endPieceIndex;

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
    
private slots:


signals:
    void  signalHeartbeatPacketReceived(const QString &computerName);
    void  signalConfirmationOfReceiptPacketReceived(quint16 packetSerialNumber1, quint16 packetSerialNumber2);

    void signalServerDeclarePacketReceived(const QString &serverAddress, quint16 serverUDTListeningPort, quint16 serverTCPListeningPort, const QString &serverName, const QString &version, int serverInstanceID);

    void signalServerOnlinePacketReceived(const QHostAddress serverAddress, quint16 serverPort, const QString &serverName);
    void signalServerOfflinePacketReceived(const QHostAddress serverAddress, quint16 serverPort, const QString &serverName);

    void signalClientDetailedInfoRequestedPacketReceived(const QString &computerName, bool rescan, int socketID);
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
    void signalRenameComputerPacketReceived(const QString &newComputerName, const QString &adminName, const QString &adminAddress, quint16 adminPort);
    void signalJoinOrUnjoinDomainPacketReceived(const QString &adminName, bool join, const QString &domainOrWorkgroupName, const QString &adminAddress, quint16 adminPort);

    void signalAdminRequestConnectionToClientPacketReceived(int socketID, const QString &computerName, const QString &users);
    void signalAdminSearchClientPacketReceived(const QString &adminAddress, quint16 adminPort, const QString &computerName, const QString &userName, const QString &workgroup, const QString &macAddress, const QString &ipAddress, const QString &osVersion, const QString &adminName);
    
    void signalAdminRequestRemoteAssistancePacketReceived(const QString &computerName, const QString &adminName, const QString &adminAddress, quint16 adminPort);
    void signalAdminRequestUpdateMSUserPasswordPacketReceived(const QString &workgroup, const QString &adminName, const QString &adminAddress, quint16 adminPort);
    void signalAdminRequestInformUserNewPasswordPacketReceived(const QString &workgroup, const QString &adminName, const QString &adminAddress, quint16 adminPort);

    void signalLocalUserOnlineStatusChanged(int socketID, const QString &userName, bool online);

///////////////////////////
    void signalFileSystemInfoRequested(int socketID, const QString &parentDirPath);

    void signalAdminRequestUploadFile(int socketID, const QByteArray &fileMD5Sum, const QString &fileName, quint64 size, const QString &localFileSaveDir);
    void signalAdminRequestDownloadFile(int socketID, const QString &localBaseDir, const QString &fileName, const QString &remoteFileSaveDir);
    void signalFileDataRequested(int socketID, const QByteArray &fileMD5, int startPieceIndex, int endPieceIndex);
    void signalFileDataReceived(int socketID, const QByteArray &fileMD5, int pieceIndex, const QByteArray &data, const QByteArray &sha1);
    void signalFileTXStatusChanged(int socketID, const QByteArray &fileMD5, quint8 status);
    void signalFileTXError(int socketID, const QByteArray &fileMD5, quint8 errorCode, const QString &errorString);

private:


private:
    QHostAddress serverAddress;
    quint16 serverUDTListeningPort;
    QString serverName;


    QHostAddress m_localUDTListeningAddress;
    quint16 m_localUDTServerListeningPort;
    quint16 m_localTCPServerListeningPort;



    QString m_localComputerName;

    ClientResourcesManager *m_resourcesManager;

    UDPServer *m_udpServer;

    RTP *m_rtp;
    UDTProtocol *m_udtProtocol;
    TCPServer *m_tcpServer;

    QHash<int /*UDT Socket ID*/, QString /*User Name*/> m_localUserSocketsHash;

};

}

#endif /* CLIENTPACKETSPARSER_H_ */
