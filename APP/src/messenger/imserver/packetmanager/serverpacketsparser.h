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

#include "../app_constants_server.h"
#include "../resourcesmanagerinstance.h"
#include "../usersmanager/usersmanager.h"


#include "HHSharedCore/hcryptography.h"



namespace HEHUI {


class ServerPacketsParser : public QObject, public UsersManager{
    Q_OBJECT
public:
    ServerPacketsParser(ResourcesManagerInstance *resourcesManager, QObject *parent = 0);
    virtual ~ServerPacketsParser();








public slots:
    void parseIncomingPacketData(Packet *packet);


    //    void sendHeartbeatPacket(){
    //        qDebug()<<"----sendHeartbeatPacket()";

    //        if(serverTCPListeningAddress.isNull()){
    //            serverTCPListeningAddress = QHostAddress::Broadcast;
    //        }

    //        Packet *packet = new Packet(QString(IM_SERVER_IPMC_ADDRESS), quint16(IM_SERVER_RUDP_LISTENING_PORT), localIPMCListeningAddress.toString(), localIPMCListeningPort);
    //        packet->setPacketType(quint8(HEHUI::HeartbeatPacket));
    //        QByteArray ba;
    //        QDataStream out(&ba, QIODevice::WriteOnly);
    //        out.setVersion(QDataStream::Qt_4_8);
    //        out << m_serverName;
    //        packet->setPacketData(ba);
    //        m_packetHandlerBase->appendOutgoingPacket(packet);


    //    }


    //    void sendConfirmationOfReceiptPacket(const QHostAddress peerAddress, quint16 peerPort, quint16 packetSerialNumber, const QString &peerID){
    //        qDebug()<<"----sendConfirmationOfReceiptPacket(...)";

    //        //Packet *packet = packetHandlerBase->getPacket(peerAddress, peerPort, localIPMCListeningAddress, localIPMCListeningPort);
    //        Packet *packet = PacketHandlerBase::getPacket(peerAddress, peerPort, localUDPListeningAddress, localUDPListeningPort);

    //        packet->setPacketType(quint8(HEHUI::ConfirmationOfReceiptPacket));
    //        QByteArray ba;
    //        QDataStream out(&ba, QIODevice::WriteOnly);
    //        out.setVersion(QDataStream::Qt_4_8);
    //        out << m_serverName << packetSerialNumber << getLastReceivedPacketSN(peerID);
    //        packet->setPacketData(ba);
    //        m_packetHandlerBase->appendOutgoingPacket(packet);

    //    }

    bool sendServerDeclarePacket(const QHostAddress peerAddress, quint16 peerPort){
        qWarning()<<"--sendServerDeclarePacket(...)"<<" Peer Address:"<<peerAddress.toString()<<":"<<peerPort;

        Packet *packet = PacketHandlerBase::getPacket();
        packet->setPacketType(quint8(IM::ServerDeclare));
        packet->setTransmissionProtocol(TP_UDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_serverName << m_localRTPListeningPort << QString(APP_VERSION);
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;
        return m_ipmcServer->sendDatagram(ba, peerAddress, peerPort);

    }

    bool sendServerOnlinePacket(const QHostAddress &targetAddress = QHostAddress(IM_SERVER_IPMC_ADDRESS), quint16 targetPort = quint16(IM_SERVER_IPMC_LISTENING_PORT)){
        qDebug()<<"----sendServerOnlinePacket(...)";

        Packet *packet = PacketHandlerBase::getPacket();

        packet->setPacketType(quint8(IM::ServerOnline));
        packet->setTransmissionProtocol(TP_UDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_serverName << m_localRTPListeningPort;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;
        return m_ipmcServer->sendDatagram(ba, targetAddress, targetPort);

    }

    bool sendServerOfflinePacket(const QHostAddress &targetAddress = QHostAddress(IM_SERVER_IPMC_ADDRESS), quint16 targetPort = quint16(IM_SERVER_IPMC_LISTENING_PORT)){
        qDebug()<<"----sendServerOfflinePacket(...)";

        Packet *packet = PacketHandlerBase::getPacket();
        packet->setPacketType(quint8(IM::ServerOffline));
        packet->setTransmissionProtocol(TP_UDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_serverName << m_localRTPListeningPort;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;
        return m_ipmcServer->sendDatagram(ba, targetAddress, targetPort);


    }

    bool sendServerAnnouncementPacket(const QHostAddress peerAddress, quint16 peerPort, const QString &announcement, bool mustRead = true){
        qDebug()<<"----sendServerAnnouncementPacket(...)";

        Packet *packet = PacketHandlerBase::getPacket();
        packet->setPacketType(quint8(IM::ServerAnnouncement));
        packet->setTransmissionProtocol(TP_UDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_serverName << announcement << (mustRead?quint8(1):quint8(0));
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;
        return m_ipmcServer->sendDatagram(ba, peerAddress, peerPort);

    }






    bool sendRegistrationServerInfoPacket(int peerSocketID, quint8 errorTypeCode, const QString &extraMessage, bool canRegister, IM::RegistrationMode regMode, const QString &regServerAddress, bool requireActivation){
        qDebug()<<"--sendRegistrationServerInfoPacket(...)";

        Packet *packet = PacketHandlerBase::getPacket(peerSocketID);
        packet->setPacketType(quint8(IM::SERVER_RESPONSE_CLIENT_REQUEST_REGISTRATION_SERVER_INFO));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_serverName << errorTypeCode << extraMessage << quint8(canRegister?1:0) << quint8(regMode) << regServerAddress << quint8(requireActivation?1:0);
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;
        return m_rtp->sendReliableData(peerSocketID, &ba);

    }

    bool sendClientRegistrationResultPacket(int peerSocketID, quint8 errorTypeCode, quint32 sysID, const QString &message){
        qWarning()<<"--sendClientRegistrationResultPacket(...)";
        
        Packet *packet = PacketHandlerBase::getPacket(peerSocketID);
        packet->setPacketType(quint8(IM::SERVER_RESPONSE_CLIENT_REGISTRATION));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_serverName << errorTypeCode << sysID << message;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;
        return m_rtp->sendReliableData(peerSocketID, &ba);

    }
    
    bool sendClientUpdatePasswordResultPacket(int peerSocketID, quint8 errorTypeCode, const QString &message){
        qDebug()<<"--sendClientUpdatePasswordResultPacket(...)";
        
        Packet *packet = PacketHandlerBase::getPacket(peerSocketID);
        packet->setPacketType(quint8(IM::SERVER_RESPONSE_CLIENT_REQUEST_UPDATE_PASSWORD));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_serverName << errorTypeCode << message;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;
        return m_rtp->sendReliableData(peerSocketID, &ba);

    }

    bool sendClientCanNotLoginPacket(int peerSocketID, const QString &userID, quint8 errorTypeCode){
        qDebug()<<"--sendClientCanNotLoginPacket(...)";
        
        Packet *packet = PacketHandlerBase::getPacket(peerSocketID);
        packet->setPacketType(quint8(IM::SERVER_RESPONSE_CLIENT_REQUEST_LOGIN));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        //out << userID << quint8(0) << quint8(errorType);
        out << m_serverName << quint8(0) << errorTypeCode;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;
        return m_rtp->sendReliableData(peerSocketID, &ba);

    }

    bool sendClientCanLoginPacket(int peerSocketID, const QString &userID, const QHostAddress &serverAddress, quint16 serverPort){
        qDebug()<<"--sendClientCanLoginPacket(...)";

        Packet *packet = PacketHandlerBase::getPacket(peerSocketID);
        packet->setPacketType(quint8(IM::SERVER_RESPONSE_CLIENT_REQUEST_LOGIN));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        //out << userID << quint8(1) << serverAddress << serverPort;
        out << m_serverName << quint8(1) << serverAddress.toString() << serverPort;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;
        return m_rtp->sendReliableData(peerSocketID, &ba);

    }

    bool sendClientLoginRedirectionPacket(int peerSocketID, const QString &serverAddress, quint16 serverPort, const QString &userHostAddress, quint16 userHostPort){
        qDebug()<<"--sendClientLoginRedirectionPacket(...)";
        
        Packet *packet = PacketHandlerBase::getPacket(peerSocketID);
        packet->setPacketType(quint8(IM::SERVER_RESPONSE_CLIENT_LOGIN_REDIRECTION));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_serverName << serverAddress << serverPort;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;
        return m_rtp->sendReliableData(peerSocketID, &ba);

    }

    bool sendClientLoginFailedPacket(int peerSocketID, const QString &userID, quint8 errorTypeCode){
        qDebug()<<"--sendClientLoginFailedPacket(...)";
        
        Packet *packet = PacketHandlerBase::getPacket(peerSocketID);
        packet->setPacketType(quint8(IM::SERVER_RESPONSE_CLIENT_LOGIN_RESULT));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        //out << userID << quint8(0) << quint8(errorType);
        out << m_serverName << quint8(0) << errorTypeCode;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;
        return m_rtp->sendReliableData(peerSocketID, &ba);

    }

    bool sendClientLoginSucceededPacket(int peerSocketID, const QString &userID, const QByteArray &encryptedPassword, const QByteArray &sessionEncryptionKey, quint32 personalSummaryInfoVersion,
                                        quint32 personalDetailInfoVersionOnServer, quint32 personalContactGroupsInfoVersionOnServer,
                                        quint32 interestGroupInfoVersionOnServer, quint32 blacklistInfoVersionOnServer){

        qDebug()<<"--sendClientLoginSucceededPacket(...)";
        
        Packet *packet = PacketHandlerBase::getPacket(peerSocketID);
        packet->setPacketType(quint8(IM::SERVER_RESPONSE_CLIENT_LOGIN_RESULT));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);

        out << sessionEncryptionKey;
        QByteArray encryptedData;
        crypto(&encryptedData, ba, encryptedPassword, true);
        ba.clear();
        out.device()->seek(0);
        out << m_serverName << quint8(1) << encryptedData << personalSummaryInfoVersion << personalDetailInfoVersionOnServer << personalContactGroupsInfoVersionOnServer << interestGroupInfoVersionOnServer << blacklistInfoVersionOnServer;
        //qWarning()<<"---encryptedData.size():"<<encryptedData.size() <<"--encryptedPassword:"<<encryptedPassword.toBase64();
        //qWarning()<<"---sessionEncryptionKey From Server:"<<sessionEncryptionKey.toBase64();

        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;
        return m_rtp->sendReliableData(peerSocketID, &ba);

    }

    bool sendUserInfoPacket(int peerSocketID, const QString &userID, const QString &userInfo, const QByteArray &sessionEncryptionKey, bool summaryInfo){
        qDebug()<<"--sendUserInfoPacket(...)";
        
        //TODO:用户信息的格式
        Packet *packet = PacketHandlerBase::getPacket();
        packet->setPacketType(quint8(IM::SERVER_RESPONSE_USER_INFO));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);

        out << userInfo << quint8(summaryInfo?1:0);
        QByteArray encryptedData;
        crypto(&encryptedData, ba, sessionEncryptionKey, true);
        ba.clear();
        out.device()->seek(0);
        out << m_serverName << userID << encryptedData;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;
        return m_rtp->sendReliableData(peerSocketID, &ba);

    }

    bool sendPersonalContactGroupsInfoPacket(int peerSocketID, const QString &contactGroupsInfo, quint32 personalContactGroupsInfoVersionOnServer, const QByteArray &sessionEncryptionKey){
        qDebug()<<"--sendPersonalContactGroupsInfoPacket(...)";
        
        Packet *packet = PacketHandlerBase::getPacket(peerSocketID);
        packet->setPacketType(quint8(IM::CONTACT_GROUPS_INFO));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);

        out << contactGroupsInfo << personalContactGroupsInfoVersionOnServer;
        QByteArray encryptedData;
        crypto(&encryptedData, ba, sessionEncryptionKey, true);
        ba.clear();
        out.device()->seek(0);
        out << m_serverName << encryptedData;

        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;
        return m_rtp->sendReliableData(peerSocketID, &ba);

    }

    bool sendContactOnlineStatusChangedPacket(int peerSocketID, const QString &contactID, quint8 onlineStateCode, const QByteArray &sessionEncryptionKey, const QString &contactHostAddress, quint16 contactHostPort, const QString &targetHostAddress, quint16 targetHostPort){
        qDebug()<<"--sendContactOnlineStatusChangedPacket(...)";
        
        Packet *packet = PacketHandlerBase::getPacket(peerSocketID);
        packet->setPacketType(quint8(IM::ONLINE_STATE_CHANGED));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);

        out << contactID << onlineStateCode << contactHostAddress << contactHostPort;
        QByteArray encryptedData;
        crypto(&encryptedData, ba, sessionEncryptionKey, true);
        ba.clear();
        out.device()->seek(0);
        out << m_serverName << encryptedData;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;
        return m_rtp->sendReliableData(peerSocketID, &ba);

    }

    void processUserOnlineStatusChanged(const  QString &userID, quint8 onlineStateCode, const QString &userHostAddress, quint16 userHostPort){
        //qDebug()<<"processUserOnlineStatusChanged(...)";
        
        UserInfo *userInfo = getUserInfo(userID);
        return processUserOnlineStatusChanged(userInfo, onlineStateCode, userHostAddress, userHostPort);

    }
    
    void processUserOnlineStatusChanged(UserInfo *userInfo, quint8 onlineStateCode, const QString &userHostAddress, quint16 userHostPort){
        //qDebug()<<"processUserOnlineStatusChanged(...)";
        
        
        if(!userInfo){return;}
        userInfo->setOnlineState(IM::OnlineState(onlineStateCode));
        if(onlineStateCode == quint8(IM::ONLINESTATE_OFFLINE)){
            userOffline(userInfo);
            saveUserLastLogoutInfo(userInfo);
            //saveUserInfoToDatabase(userInfo);
            m_userSocketsHash.remove(userInfo->getSocketID());
        }else{
            userOnline(userInfo);
            saveUserLastLoginInfo(userInfo, userHostAddress, userHostPort);
        }

        //通知所有在线联系人
        QStringList contacts = userInfo->getContacts();
        foreach (QString contact, contacts) {
            UserInfo *contactInfo = getOnlineUserInfo(contact);
            if(contactInfo){
                sendContactOnlineStatusChangedPacket(contactInfo->getSocketID(), userInfo->getUserID(), onlineStateCode, contactInfo->getSessionEncryptionKey(), userHostAddress, userHostPort, contactInfo->getLastLoginExternalHostAddress(), contactInfo->getLastLoginExternalHostPort());
            }
        }


    }

    bool sendContactsOnlineInfo(int peerSocketID, UserInfo *userInfo){
        if(!userInfo){
            return false;
        }

        QStringList contacts = userInfo->getContacts();
        QStringList contactsOnlineInfo;
        foreach (QString contact, contacts) {
            UserInfo *info = getOnlineUserInfo(contact);
            if(info){
                IM::OnlineState state = info->getOnlineState();
                if(state != IM::ONLINESTATE_OFFLINE && state != IM::ONLINESTATE_INVISIBLE){
                    QStringList onlineInfo;
                    onlineInfo.append(contact);
                    onlineInfo.append(QString::number(quint8(state)));
                    onlineInfo.append(info->getLastLoginExternalHostAddress());
                    onlineInfo.append(QString::number(info->getLastLoginExternalHostPort()));
                    contactsOnlineInfo.append(onlineInfo.join(QString(PACKET_DATA_SEPARTOR)));
                }

            }
        }
        if(contactsOnlineInfo.isEmpty()){
            return false;
        }

        Packet *packet = PacketHandlerBase::getPacket(peerSocketID);
        packet->setPacketType(quint8(IM::CONTACTS_ONLINE_INFO));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);

        out << contactsOnlineInfo.join(QString(UNIT_SEPARTOR));
        QByteArray encryptedData;
        crypto(&encryptedData, ba, userInfo->getSessionEncryptionKey(), true);
        ba.clear();
        out.device()->seek(0);
        out << m_serverName << encryptedData;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;
        return m_rtp->sendReliableData(peerSocketID, &ba);


    }

    bool sendSearchResultPacket(int peerSocketID, const QStringList &users, const QByteArray &sessionEncryptionKey, const QHostAddress &targetHostAddress, quint16 targetHostPort){
        qDebug()<<"--sendSearchResultPacket(...)";
        
        //TODO:搜索结果的格式
        Packet *packet = PacketHandlerBase::getPacket(peerSocketID);
        packet->setPacketType(quint8(IM::SERVER_RESPONSE_SEARCH_CONTACTS));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);

        out << users.join(QString(PACKET_DATA_SEPARTOR));
        QByteArray encryptedData;
        crypto(&encryptedData, ba, sessionEncryptionKey, true);
        ba.clear();
        out.device()->seek(0);
        out << m_serverName << encryptedData;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;
        return m_rtp->sendReliableData(peerSocketID, &ba);
    }
    
    bool sendAddContactRequestFromUserPacket(int peerSocketID, const QString &userID, const QString &userNickName, const QString &userFace, const QString &verificationMessage, const QByteArray &sessionEncryptionKey, const QHostAddress &targetHostAddress, quint16 targetHostPort){
        qDebug()<<"--sendAddContactRequestFromUserPacket(...)";
        
        Packet *packet = PacketHandlerBase::getPacket(peerSocketID);
        packet->setPacketType(quint8(IM::CLIENT_REQUEST_ADD_CONTACT));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);

        out << userID << userNickName << userFace << verificationMessage;
        QByteArray encryptedData;
        crypto(&encryptedData, ba, sessionEncryptionKey, true);
        ba.clear();
        out.device()->seek(0);
        out << m_serverName << encryptedData;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;
        return m_rtp->sendReliableData(peerSocketID, &ba);
    }

    bool sendAddContactResultPacket(int peerSocketID, const QString &contactID, const QString &contactNickName, const QString &contactFace, quint8 errorTypeCode, const QString &reasonMessage, const QByteArray &sessionEncryptionKey, const QString &targetHostAddress, quint16 targetHostPort){
        qDebug()<<"--sendAddContactResultPacket(...)";
        
        Packet *packet = PacketHandlerBase::getPacket(peerSocketID);
        packet->setPacketType(quint8(IM::CLIENT_RESPONSE_ADD_CONTACT_REQUEST));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);

        out << contactID << contactNickName << contactFace << errorTypeCode << reasonMessage;
        QByteArray encryptedData;
        crypto(&encryptedData, ba, sessionEncryptionKey, true);
        ba.clear();
        out.device()->seek(0);
        out << m_serverName << encryptedData;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;
        return m_rtp->sendReliableData(peerSocketID, &ba);
    }

    bool sendUserInterestGroupsListPacket(int peerSocketID, UserInfo *userInfo, const QHostAddress &targetHostAddress, quint16 targetHostPort){

        //TODO:用户信息的格式
        Packet *packet = PacketHandlerBase::getPacket(peerSocketID);
        packet->setPacketType(quint8(IM::SERVER_RESPONSE_INTEREST_GROUPS_LIST));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);

        out << getInterestGroupsListForUser(userInfo) << userInfo->getInterestGroupInfoVersion();
        QByteArray encryptedData;
        crypto(&encryptedData, ba, userInfo->getSessionEncryptionKey(), true);
        ba.clear();
        out.device()->seek(0);
        out << m_serverName << encryptedData;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;
        return m_rtp->sendReliableData(peerSocketID, &ba);

    }
    
    bool sendUserInterestGroupInfoPacket(bool peerSocketID, UserInfo *userInfo, quint32 groupID, const QHostAddress &targetHostAddress, quint16 targetHostPort){

        //TODO:用户信息的格式
        Packet *packet = PacketHandlerBase::getPacket(peerSocketID);
        packet->setPacketType(quint8(IM::SERVER_RESPONSE_INTEREST_GROUP_INFO));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);

        out << getInterestGroupInfoStringForUser(userInfo, groupID) << groupID;
        QByteArray encryptedData;
        crypto(&encryptedData, ba, userInfo->getSessionEncryptionKey(), true);
        ba.clear();
        out.device()->seek(0);
        out << m_serverName << encryptedData;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;
        return m_rtp->sendReliableData(peerSocketID, &ba);
    }

    bool sendUserInterestGroupMembersInfoPacket(int peerSocketID, UserInfo *userInfo, quint32 groupID, const QHostAddress &targetHostAddress, quint16 targetHostPort){
        //qWarning()<<"----sendUserInterestGroupMembersInfoPacket(...)"<<" groupID"<<groupID;

        InterestGroup *group = getInterestGroup(groupID);
        if(!group){
            return false;
        }

        //TODO:用户信息的格式
        Packet *packet = PacketHandlerBase::getPacket(peerSocketID);
        packet->setPacketType(quint8(IM::SERVER_RESPONSE_INTEREST_GROUP_MEMBERS_INFO));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);

        out << getInterestGroupMembersInfoStringForUser(userInfo, group) << group->getGroupMemberListInfoVersion() << groupID;
        QByteArray encryptedData;
        crypto(&encryptedData, ba, userInfo->getSessionEncryptionKey(), true);
        ba.clear();
        out.device()->seek(0);
        out << m_serverName << encryptedData;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;
        return m_rtp->sendReliableData(peerSocketID, &ba);

    }
    
    
    bool sendUserBlacklistInfoPacket(bool peerSocketID, UserInfo *userInfo, const QHostAddress &targetHostAddress, quint16 targetHostPort){
        qDebug()<<"--sendUserBlacklistInfoPacket(...)";

        //TODO:用户信息的格式
        Packet *packet = PacketHandlerBase::getPacket(peerSocketID);
        packet->setPacketType(quint8(IM::BLACKLIST_INFO));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);

        out << userInfo->getBlacklistInfoString() << userInfo->getBlacklistInfoVersion();
        QByteArray encryptedData;
        crypto(&encryptedData, ba, userInfo->getSessionEncryptionKey(), true);
        ba.clear();
        out.device()->seek(0);
        out << m_serverName << encryptedData;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;
        return m_rtp->sendReliableData(peerSocketID, &ba);

    }


    bool sendSessionEncryptionKeyWithContact(int peerSocketID, const QString &contactID, const QByteArray &encryptedPassword, const QByteArray &sessionEncryptionKeyWithContact, const QString &targetHostAddress, quint16 targetHostPort){
        qDebug()<<"--sendSessionEncryptionKeyWithContact(...)";
        
        Packet *packet = PacketHandlerBase::getPacket(peerSocketID);
        packet->setPacketType(quint8(IM::SESSION_ENCRYPTION_KEY_WITH_CONTACT));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);

        out << contactID << sessionEncryptionKeyWithContact;
        QByteArray encryptedData;
        crypto(&encryptedData, ba, encryptedPassword, true);
        ba.clear();
        out.device()->seek(0);
        out << m_serverName << encryptedData;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;
        return m_rtp->sendReliableData(peerSocketID, &ba);

    }



    bool sendCachedChatMessagesPacket(int peerSocketID, const QStringList &messages, const QByteArray &sessionEncryptionKey, const QHostAddress &targetHostAddress, quint16 targetHostPort){
        qDebug()<<"--sendCachedChatMessagesPacket(...)";
        
        //TODO:缓存消息的格式
        Packet *packet = PacketHandlerBase::getPacket(peerSocketID);
        packet->setPacketType(quint8(IM::CHAT_MESSAGES_CACHED_ON_SERVER));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);

        out << messages.join(QString(UNIT_SEPARTOR));
        QByteArray encryptedData;
        crypto(&encryptedData, ba, sessionEncryptionKey, true);
        ba.clear();
        out.device()->seek(0);
        out << m_serverName << encryptedData;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;
        return m_rtp->sendReliableData(peerSocketID, &ba);

    }

    bool sendCachedInterestGroupChatMessagesPacket(int peerSocketID, const QStringList &messages, const QByteArray &sessionEncryptionKey, const QHostAddress &targetHostAddress, quint16 targetHostPort){
        qDebug()<<"--sendCachedInterestGroupChatMessagesPacket(...)";

        //TODO:缓存消息的格式
        Packet *packet = PacketHandlerBase::getPacket(peerSocketID);
        packet->setPacketType(quint8(IM::GROUP_CHAT_MESSAGES_CACHED_ON_SERVER));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);

        out << messages.join(QString(UNIT_SEPARTOR));
        QByteArray encryptedData;
        crypto(&encryptedData, ba, sessionEncryptionKey, true);
        ba.clear();
        out.device()->seek(0);
        out << m_serverName << encryptedData;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;
        return m_rtp->sendReliableData(peerSocketID, &ba);

    }


    void userExceptionalOffline(const QString &peerAddress, quint16 peerPort);




private slots:
    //HeartbeatPacket: PacketType+ComputerName
    //    void startHeartbeat(int interval = HEARTBEAT_TIMER_INTERVAL);
    //    void stopHeartbeat();

    int crypto(QByteArray *destination, const QByteArray &source, const QByteArray &key, bool encrypt);

    bool encrypeData(const QString &userID, QByteArray *destination, const QByteArray &source);
    bool decryptData(const QString &userID, QByteArray *destination, const QByteArray &source);

    //    void slotCheckIMUsersOnlineStatus();

    void peerDisconnected(int socketID);


signals:
    void  signalHeartbeatPacketReceived(const QString &clientAddress, const QString &userID);
    void  signalConfirmationOfReceiptPacketReceived(quint16 packetSerialNumber1, quint16 packetSerialNumber2);

    void signalClientLookForServerPacketReceived(const QHostAddress &clientAddress, quint16 clientTCPListeningPort, const QString &clientName);

    //    void signalUserRequestLoginPacketReceived(const QString &userID, const QString &clientVersion);
    //    void signalUserLoginInfoPacketReceived(const QString &userID, QByteArray encryptedPassword, IM::OnlineState state);
    //    void signalUserOnlineStateChanged(const QString &userID, const QByteArray &encryptedOnlineState);
    //    void signalUserRequestUserInfo(const QString &userID, const QByteArray &encryptedUserID);

    //    void signalUserUpdatePassword(const QString &userID, const QByteArray &encryptedNewPassword);


private:


private:
    //    QHostAddress serverTCPListeningAddress;
    //    quint16 serverTCPListeningPort;
    QString m_serverName;
    quint16 m_localRTPListeningPort;




    //    QTimer *heartbeatTimer;
    //        QTimer *processWaitingForReplyPacketsTimer;

    UDPServer *m_ipmcServer;
    RTP *m_rtp;
    UDTProtocol *m_udtProtocol;
    TCPServer *m_tcpServer;
    QHash<int/*Socket ID*/, UserInfo*/*UserInfo*/> m_userSocketsHash;


    UsersManager usersManager;
    Cryptography *cryptography;

    QMutex mutex;

    QHash<QString, QByteArray> sessionEncryptionKeysHash;

    QTimer *checkIMUsersOnlineStateTimer;





};

}

#endif /* SERVERPACKETSPARSER_H_ */
