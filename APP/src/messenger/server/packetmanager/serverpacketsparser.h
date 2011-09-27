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
#include "../networkmanager/servernetworkmanager.h"
#include "../usersmanager/usersmanager.h"

//#include "../../../shared/network/packethandler/packetparserbase.h"

#include "HHSharedCore/hcryptography.h"
#include "HHSharedNetwork/hpacketparserbase.h"



namespace HEHUI {


class ServerPacketsParser : public PacketsParserBase, public UsersManager{
    Q_OBJECT
public:
    ServerPacketsParser(ServerNetworkManager *networkManager, QObject *parent = 0);
    virtual ~ServerPacketsParser();

//    void setLocalRUDPListeningAddress(const QHostAddress &address = QHostAddress::Any);
//    void setLocalRUDPListeningPort(quint16 port);



    void parseIncomingPacketData(Packet *packet);
    void run();

    void startparseIncomingPackets();
    void startprocessOutgoingPackets();

//    void startCheckIMUsersOnlineStateTimer();




public slots:


//    void sendHeartbeatPacket(){
//        qDebug()<<"----sendHeartbeatPacket()";

//        if(serverTCPListeningAddress.isNull()){
//            serverTCPListeningAddress = QHostAddress::Broadcast;
//        }

//        Packet *packet = new Packet(QString(IM_SERVER_IPMC_ADDRESS), quint16(IM_SERVER_RUDP_LISTENING_PORT), localIPMCListeningAddress.toString(), localIPMCListeningPort);
//        packet->setPacketType(quint8(HEHUI::HeartbeatPacket));
//        QByteArray ba;
//        QDataStream out(&ba, QIODevice::WriteOnly);
//        out.setVersion(QDataStream::Qt_4_7);
//        out << m_serverName;
//        packet->setPacketData(ba);
//        m_packetHandlerBase->appendOutgoingPacket(packet);


//    }


//    void sendConfirmationOfReceiptPacket(const QHostAddress peerAddress, quint16 peerPort, quint16 packetSerialNumber, const QString &peerID){
//        qDebug()<<"----sendConfirmationOfReceiptPacket(...)";

//        //Packet *packet = packetHandlerBase->getPacket(peerAddress, peerPort, localIPMCListeningAddress, localIPMCListeningPort);
//        Packet *packet = m_packetHandlerBase->getPacket(peerAddress, peerPort, localUDPListeningAddress, localUDPListeningPort);
        
//        packet->setPacketType(quint8(HEHUI::ConfirmationOfReceiptPacket));
//        QByteArray ba;
//        QDataStream out(&ba, QIODevice::WriteOnly);
//        out.setVersion(QDataStream::Qt_4_7);
//        out << m_serverName << packetSerialNumber << getLastReceivedPacketSN(peerID);
//        packet->setPacketData(ba);
//        m_packetHandlerBase->appendOutgoingPacket(packet);

//    }

    void sendServerDeclarePacket(const QHostAddress peerAddress, quint16 peerPort){
        qWarning()<<"--sendServerDeclarePacket(...)"<<" Peer Address:"<<peerAddress.toString()<<":"<<peerPort;

        Packet *packet = 0;
        //Packet *packet = m_packetHandlerBase->getPacket(peerAddress, peerPort, localRUDPListeningAddress, localRUDPListeningPort);
        if(peerAddress == QHostAddress(IM_SERVER_IPMC_ADDRESS) || peerAddress == QHostAddress::Broadcast){
            packet = m_packetHandlerBase->getPacket(peerAddress, peerPort, localIPMCListeningAddress, localIPMCListeningPort);
            packet->setTransmissionProtocol(TP_UDP);
        }else{
            packet = m_packetHandlerBase->getPacket(peerAddress, peerPort, localRUDPListeningAddress, localRUDPListeningPort);
            packet->setTransmissionProtocol(TP_RUDP);
        }

        
        packet->setPacketType(quint8(IM::ServerDeclare));
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_serverName << localRUDPListeningAddress.toString() << localRUDPListeningPort << QString(APP_VERSION);
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

    }

    void sendServerOnlinePacket(){
        qDebug()<<"----sendServerOnlinePacket(...)";

        Packet *packet = m_packetHandlerBase->getPacket(QHostAddress(IM_SERVER_IPMC_ADDRESS), quint16(IM_SERVER_IPMC_LISTENING_PORT), localIPMCListeningAddress, localIPMCListeningPort);

        packet->setPacketType(quint8(IM::ServerOnline));
        packet->setTransmissionProtocol(TP_UDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_serverName << localRUDPListeningAddress.toString() << localRUDPListeningPort;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);


    }

    void sendServerOfflinePacket(){
        qDebug()<<"----sendServerOfflinePacket(...)";

        Packet *packet = m_packetHandlerBase->getPacket(QHostAddress(IM_SERVER_IPMC_ADDRESS), quint16(IM_SERVER_IPMC_LISTENING_PORT), localIPMCListeningAddress, localIPMCListeningPort);
        packet->setPacketType(quint8(IM::ServerOffline));
        packet->setTransmissionProtocol(TP_UDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_serverName << localRUDPListeningAddress.toString() << localRUDPListeningPort;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);


    }

    void sendServerAnnouncementPacket(const QHostAddress peerAddress, quint16 peerPort, const QString &announcement, bool mustRead = true){
        qDebug()<<"----sendServerAnnouncementPacket(...)";

        Packet *packet = m_packetHandlerBase->getPacket(peerAddress, peerPort, localIPMCListeningAddress, localIPMCListeningPort);
        packet->setPacketType(quint8(IM::ServerAnnouncement));
        packet->setTransmissionProtocol(TP_UDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_serverName << announcement << (mustRead?quint8(1):quint8(0));
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);


    }








    void sendClientRegistrationResultPacket(quint8 errorTypeCode, const QString &message, const QHostAddress &userHostAddress, quint16 userHostPort){
        qWarning()<<"--sendClientRegistrationResultPacket(...)";
        
        Packet *packet = m_packetHandlerBase->getPacket(userHostAddress, userHostPort, localRUDPListeningAddress, localRUDPListeningPort);
        packet->setPacketType(quint8(IM::SERVER_RESPONSE_CLIENT_REGISTRATION));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_serverName << errorTypeCode << message;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);


    }
    
    void sendClientUpdatePasswordResultPacket(quint8 errorTypeCode, const QString &message, const QHostAddress &userHostAddress, quint16 userHostPort){
        qDebug()<<"--sendClientUpdatePasswordResultPacket(...)";
        
        Packet *packet = m_packetHandlerBase->getPacket(userHostAddress, userHostPort, localRUDPListeningAddress, localRUDPListeningPort);
        packet->setPacketType(quint8(IM::SERVER_RESPONSE_CLIENT_REQUEST_UPDATE_PASSWORD));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_serverName << errorTypeCode << message;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);


    }

    void sendClientCanNotLoginPacket(const QString &userID, quint8 errorTypeCode, const QHostAddress &userHostAddress, quint16 userHostPort){
        qDebug()<<"--sendClientCanNotLoginPacket(...)";
        
        Packet *packet = m_packetHandlerBase->getPacket(userHostAddress, userHostPort, localRUDPListeningAddress, localRUDPListeningPort);
        packet->setPacketType(quint8(IM::SERVER_RESPONSE_CLIENT_REQUEST_LOGIN));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        //out << userID << quint8(0) << quint8(errorType);
        out << m_serverName << quint8(0) << errorTypeCode;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);


    }

    void sendClientCanLoginPacket(const QString &userID, const QHostAddress &serverAddress, quint16 serverPort, const QHostAddress &userHostAddress, quint16 userHostPort){
        qDebug()<<"--sendClientCanLoginPacket(...)";

        Packet *packet = m_packetHandlerBase->getPacket(userHostAddress, userHostPort, localRUDPListeningAddress, localRUDPListeningPort);
        packet->setPacketType(quint8(IM::SERVER_RESPONSE_CLIENT_REQUEST_LOGIN));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        //out << userID << quint8(1) << serverAddress << serverPort;
        out << m_serverName << quint8(1) << serverAddress.toString() << serverPort;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);


    }

    void sendClientLoginRedirectionPacket(const QString &serverAddress, quint16 serverPort, const QString &userHostAddress, quint16 userHostPort){
        qDebug()<<"--sendClientLoginRedirectionPacket(...)";
        
        Packet *packet = m_packetHandlerBase->getPacket(QHostAddress(userHostAddress), userHostPort, localRUDPListeningAddress, localRUDPListeningPort);
        packet->setPacketType(quint8(IM::SERVER_RESPONSE_CLIENT_LOGIN_REDIRECTION));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_serverName << serverAddress << serverPort;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);


    }

    void sendClientLoginFailedPacket(const QString &userID, quint8 errorTypeCode, const QHostAddress &userHostAddress, quint16 userHostPort){
        qDebug()<<"--sendClientLoginFailedPacket(...)";
        
        Packet *packet = m_packetHandlerBase->getPacket(userHostAddress, userHostPort, localRUDPListeningAddress, localRUDPListeningPort);
        packet->setPacketType(quint8(IM::SERVER_RESPONSE_CLIENT_LOGIN_RESULT));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        //out << userID << quint8(0) << quint8(errorType);
        out << m_serverName << quint8(0) << errorTypeCode;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);


    }

    void sendClientLoginSucceededPacket(const QString &userID, const QByteArray &encryptedPassword, const QByteArray &sessionEncryptionKey,
                                           quint32 personalInfoVersionOnServer, quint32 personalContactGroupsInfoVersionOnServer,
                                           quint32 interestGroupInfoVersionOnServer, quint32 blacklistInfoVersionOnServer,
                                           const QHostAddress &targetHostAddress, quint16 targetHostPort){

        qDebug()<<"--sendClientLoginSucceededPacket(...)";
        
        Packet *packet = m_packetHandlerBase->getPacket(targetHostAddress, targetHostPort, localRUDPListeningAddress, localRUDPListeningPort);
        packet->setPacketType(quint8(IM::SERVER_RESPONSE_CLIENT_LOGIN_RESULT));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);

        out << sessionEncryptionKey;
        QByteArray encryptedData;
        crypto(&encryptedData, ba, encryptedPassword, true);
        ba.clear();
        out.device()->seek(0);
        out << m_serverName << quint8(1) << encryptedData <<personalInfoVersionOnServer << personalContactGroupsInfoVersionOnServer << interestGroupInfoVersionOnServer << blacklistInfoVersionOnServer;
        qWarning()<<"---encryptedData.size():"<<encryptedData.size() <<"--encryptedPassword:"<<encryptedPassword.toBase64();
        qWarning()<<"---sessionEncryptionKey From Server:"<<sessionEncryptionKey.toBase64();

        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);


    }

    void sendUserInfoPacket(const QString &userID, const QString &userInfo, const QByteArray &sessionEncryptionKey, const QHostAddress &targetHostAddress, quint16 targetHostPort){
    	qDebug()<<"--sendUserInfoPacket(...)";
        
        //TODO:用户信息的格式
        Packet *packet = m_packetHandlerBase->getPacket(targetHostAddress, targetHostPort, localRUDPListeningAddress, localRUDPListeningPort);
        packet->setPacketType(quint8(IM::SERVER_RESPONSE_USER_SUMMARY_INFO));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);

        out << userInfo;
        QByteArray encryptedData;
        crypto(&encryptedData, ba, sessionEncryptionKey, true);
        ba.clear();
        out.device()->seek(0);
        out << m_serverName << userID << encryptedData;

        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);


    }

    void sendPersonalContactGroupsInfoPacket(const QString &contactGroupsInfo, quint32 personalContactGroupsInfoVersionOnServer, const QByteArray &sessionEncryptionKey, const QHostAddress &targetHostAddress, quint16 targetHostPort){
        qDebug()<<"--sendPersonalContactGroupsInfoPacket(...)";
        
        Packet *packet = m_packetHandlerBase->getPacket(targetHostAddress, targetHostPort, localRUDPListeningAddress, localRUDPListeningPort);
        packet->setPacketType(quint8(IM::CONTACT_GROUPS_INFO));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);

        out << contactGroupsInfo << personalContactGroupsInfoVersionOnServer;
        QByteArray encryptedData;
        crypto(&encryptedData, ba, sessionEncryptionKey, true);
        ba.clear();
        out.device()->seek(0);
        out << m_serverName << encryptedData;

        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);


    }

    void sendContactOnlineStatusChangedPacket(const QString &contactID, quint8 onlineStateCode, const QByteArray &sessionEncryptionKey, const QString &contactHostAddress, quint16 contactHostPort, const QString &targetHostAddress, quint16 targetHostPort){
        qDebug()<<"--sendContactOnlineStatusChangedPacket(...)";
        
        Packet *packet = m_packetHandlerBase->getPacket(QHostAddress(targetHostAddress), targetHostPort, localRUDPListeningAddress, localRUDPListeningPort);
        packet->setPacketType(quint8(IM::ONLINE_STATE_CHANGED));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);

        out << contactID << onlineStateCode << contactHostAddress << contactHostPort;
        QByteArray encryptedData;
        crypto(&encryptedData, ba, sessionEncryptionKey, true);
        ba.clear();
        out.device()->seek(0);
        out << m_serverName << encryptedData;

        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);


    }

    void processUserOnlineStatusChanged(const  QString &userID, quint8 onlineStateCode, const QString &userHostAddress, quint16 userHostPort){
        //qDebug()<<"processUserOnlineStatusChanged(...)";
        
        UserInfo *userInfo = getUserInfo(userID);        
        processUserOnlineStatusChanged(userInfo, onlineStateCode, userHostAddress, userHostPort);

    }
    
    void processUserOnlineStatusChanged(UserInfo *userInfo, quint8 onlineStateCode, const QString &userHostAddress, quint16 userHostPort){
        //qDebug()<<"processUserOnlineStatusChanged(...)";
        
        
        if(!userInfo){return;}
        userInfo->setOnlineState(IM::OnlineState(onlineStateCode));
        if(onlineStateCode == quint8(IM::ONLINESTATE_OFFLINE)){
            userOffline(userInfo);
            saveUserLoginInfo(userInfo, userHostAddress, false);
            //saveUserInfoToDatabase(userInfo);
        }else{
            userOnline(userInfo);
            saveUserLoginInfo(userInfo, userHostAddress, true);
        }

        //通知所有在线联系人
        QStringList contacts = userInfo->getContacts();
        foreach (QString contact, contacts) {
            UserInfo *contactInfo = getOnlineUserInfo(contact);
            if(contactInfo){
                sendContactOnlineStatusChangedPacket(userInfo->getUserID(), onlineStateCode, contactInfo->getSessionEncryptionKey(), userHostAddress, userHostPort, contactInfo->getLastLoginHostAddress(), contactInfo->getLastLoginHostPort());
            }
        }


    }

    void sendContactsOnlineInfo(UserInfo *userInfo){
        if(!userInfo){
            return;
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
                    onlineInfo.append(info->getLastLoginHostAddress());
                    onlineInfo.append(QString::number(info->getLastLoginHostPort()));
                    contactsOnlineInfo.append(onlineInfo.join(QString(PACKET_DATA_SEPARTOR)));
                }

            }
        }
        if(contactsOnlineInfo.isEmpty()){
            return;
        }

        Packet *packet = m_packetHandlerBase->getPacket(QHostAddress(userInfo->getLastLoginHostAddress()), userInfo->getLastLoginHostPort(), localRUDPListeningAddress, localRUDPListeningPort);
        packet->setPacketType(quint8(IM::CONTACTS_ONLINE_INFO));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);

        out << contactsOnlineInfo.join(QString(UNIT_SEPARTOR));
        QByteArray encryptedData;
        crypto(&encryptedData, ba, userInfo->getSessionEncryptionKey(), true);
        ba.clear();
        out.device()->seek(0);
        out << m_serverName << encryptedData;

        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);



    }

    void sendSearchResultPacket(const QStringList &users, const QByteArray &sessionEncryptionKey, const QHostAddress &targetHostAddress, quint16 targetHostPort){
    	qDebug()<<"--sendSearchResultPacket(...)";
        
        //TODO:搜索结果的格式
        Packet *packet = m_packetHandlerBase->getPacket(targetHostAddress, targetHostPort, localRUDPListeningAddress, localRUDPListeningPort);
        packet->setPacketType(quint8(IM::SERVER_RESPONSE_SEARCH_CONTACTS));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);

        out << users.join(QString(PACKET_DATA_SEPARTOR));
        QByteArray encryptedData;
        crypto(&encryptedData, ba, sessionEncryptionKey, true);
        ba.clear();
        out.device()->seek(0);
        out << m_serverName << encryptedData;

        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);


    }
    
    void sendAddContactRequestFromUserPacket(const QString &userID, const QString &userNickName, const QString &userFace, const QString &verificationMessage, const QByteArray &sessionEncryptionKey, const QHostAddress &targetHostAddress, quint16 targetHostPort){
        qDebug()<<"--sendAddContactRequestFromUserPacket(...)";
        
        Packet *packet = m_packetHandlerBase->getPacket(targetHostAddress, targetHostPort, localRUDPListeningAddress, localRUDPListeningPort);
        packet->setPacketType(quint8(IM::CLIENT_REQUEST_ADD_CONTACT));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);

        out << userID << userNickName << userFace << verificationMessage;
        QByteArray encryptedData;
        crypto(&encryptedData, ba, sessionEncryptionKey, true);
        ba.clear();
        out.device()->seek(0);
        out << m_serverName << encryptedData;

        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);


    }

    void sendAddContactResultPacket(const QString &contactID, const QString &contactNickName, const QString &contactFace, quint8 errorTypeCode, const QString &reasonMessage, const QByteArray &sessionEncryptionKey, const QString &targetHostAddress, quint16 targetHostPort){
        qDebug()<<"--sendAddContactResultPacket(...)";
        
        Packet *packet = m_packetHandlerBase->getPacket(QHostAddress(targetHostAddress), targetHostPort, localRUDPListeningAddress, localRUDPListeningPort);
        packet->setPacketType(quint8(IM::CLIENT_RESPONSE_ADD_CONTACT_REQUEST));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);

        out << contactID << contactNickName << contactFace << errorTypeCode << reasonMessage;
        QByteArray encryptedData;
        crypto(&encryptedData, ba, sessionEncryptionKey, true);
        ba.clear();
        out.device()->seek(0);
        out << m_serverName << encryptedData;

        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);


    }

    void sendUserInterestGroupsListPacket(UserInfo *userInfo, const QHostAddress &targetHostAddress, quint16 targetHostPort){

        //TODO:用户信息的格式
        Packet *packet = m_packetHandlerBase->getPacket(targetHostAddress, targetHostPort, localRUDPListeningAddress, localRUDPListeningPort);
        packet->setPacketType(quint8(IM::SERVER_RESPONSE_INTEREST_GROUPS_LIST));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);

        out << getInterestGroupsListForUser(userInfo) << userInfo->getInterestGroupInfoVersion();
        QByteArray encryptedData;
        crypto(&encryptedData, ba, userInfo->getSessionEncryptionKey(), true);
        ba.clear();
        out.device()->seek(0);
        out << m_serverName << encryptedData;

        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);


    }
    
    void sendUserInterestGroupInfoPacket(UserInfo *userInfo, quint32 groupID, const QHostAddress &targetHostAddress, quint16 targetHostPort){

        //TODO:用户信息的格式
        Packet *packet = m_packetHandlerBase->getPacket(targetHostAddress, targetHostPort, localRUDPListeningAddress, localRUDPListeningPort);
        packet->setPacketType(quint8(IM::SERVER_RESPONSE_INTEREST_GROUP_INFO));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);

        out << getInterestGroupInfoStringForUser(userInfo, groupID) << groupID;
        QByteArray encryptedData;
        crypto(&encryptedData, ba, userInfo->getSessionEncryptionKey(), true);
        ba.clear();
        out.device()->seek(0);
        out << m_serverName << encryptedData;

        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);


    }

    void sendUserInterestGroupMembersInfoPacket(UserInfo *userInfo, quint32 groupID, const QHostAddress &targetHostAddress, quint16 targetHostPort){
        //qWarning()<<"----sendUserInterestGroupMembersInfoPacket(...)"<<" groupID"<<groupID;

        Group *group = getGroup(groupID);
        if(!group){
            return;
        }

        //TODO:用户信息的格式
        Packet *packet = m_packetHandlerBase->getPacket(targetHostAddress, targetHostPort, localRUDPListeningAddress, localRUDPListeningPort);
        packet->setPacketType(quint8(IM::SERVER_RESPONSE_INTEREST_GROUP_MEMBERS_INFO));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);

        out << getInterestGroupMembersInfoStringForUser(userInfo, group) << group->getMemberListInfoVersion() << groupID;
        QByteArray encryptedData;
        crypto(&encryptedData, ba, userInfo->getSessionEncryptionKey(), true);
        ba.clear();
        out.device()->seek(0);
        out << m_serverName << encryptedData;

        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);


    }
    
    
    void sendUserBlacklistInfoPacket(UserInfo *userInfo, const QHostAddress &targetHostAddress, quint16 targetHostPort){
        qDebug()<<"--sendUserBlacklistInfoPacket(...)";

        //TODO:用户信息的格式
        Packet *packet = m_packetHandlerBase->getPacket(targetHostAddress, targetHostPort, localRUDPListeningAddress, localRUDPListeningPort);
        packet->setPacketType(quint8(IM::BLACKLIST_INFO));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);

        out << userInfo->getBlacklistInfoString() << userInfo->getBlacklistInfoVersion();
        QByteArray encryptedData;
        crypto(&encryptedData, ba, userInfo->getSessionEncryptionKey(), true);
        ba.clear();
        out.device()->seek(0);
        out << m_serverName << encryptedData;

        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);


    }


    void sendSessionEncryptionKeyWithContact(const QString &contactID, const QByteArray &encryptedPassword, const QByteArray &sessionEncryptionKeyWithContact, const QString &targetHostAddress, quint16 targetHostPort){
        qDebug()<<"--sendSessionEncryptionKeyWithContact(...)";
        
        Packet *packet = m_packetHandlerBase->getPacket(QHostAddress(targetHostAddress), targetHostPort, localRUDPListeningAddress, localRUDPListeningPort);
        packet->setPacketType(quint8(IM::SESSION_ENCRYPTION_KEY_WITH_CONTACT));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);

        out << contactID << sessionEncryptionKeyWithContact;
        QByteArray encryptedData;
        crypto(&encryptedData, ba, encryptedPassword, true);
        ba.clear();
        out.device()->seek(0);
        out << m_serverName << encryptedData;

        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);


    }



    void sendCachedChatMessagesPacket(const QStringList &messages, const QByteArray &sessionEncryptionKey, const QHostAddress &targetHostAddress, quint16 targetHostPort){
    	qDebug()<<"--sendCachedChatMessagesPacket(...)";
        
        //TODO:缓存消息的格式
        Packet *packet = m_packetHandlerBase->getPacket(targetHostAddress, targetHostPort, localRUDPListeningAddress, localRUDPListeningPort);
        packet->setPacketType(quint8(IM::CHAT_MESSAGES_CACHED_ON_SERVER));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);

        out << messages.join(QString(UNIT_SEPARTOR));
        QByteArray encryptedData;
        crypto(&encryptedData, ba, sessionEncryptionKey, true);
        ba.clear();
        out.device()->seek(0);
        out << m_serverName << encryptedData;

        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);


    }

    void sendCachedInterestGroupChatMessagesPacket(const QStringList &messages, const QByteArray &sessionEncryptionKey, const QHostAddress &targetHostAddress, quint16 targetHostPort){
        qDebug()<<"--sendCachedInterestGroupChatMessagesPacket(...)";

        //TODO:缓存消息的格式
        Packet *packet = m_packetHandlerBase->getPacket(targetHostAddress, targetHostPort, localRUDPListeningAddress, localRUDPListeningPort);
        packet->setPacketType(quint8(IM::GROUP_CHAT_MESSAGES_CACHED_ON_SERVER));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);

        out << messages.join(QString(UNIT_SEPARTOR));
        QByteArray encryptedData;
        crypto(&encryptedData, ba, sessionEncryptionKey, true);
        ba.clear();
        out.device()->seek(0);
        out << m_serverName << encryptedData;

        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);


    }


    void userExceptionalOffline(const QString &peerAddress, quint16 peerPort);




private slots:
    //HeartbeatPacket: PacketType+ComputerName
    void startHeartbeat(int interval = HEARTBEAT_TIMER_INTERVAL);
    void stopHeartbeat();

    int crypto(QByteArray *destination, const QByteArray &source, const QByteArray &key, bool encrypt);

    bool encrypeData(const QString &userID, QByteArray *destination, const QByteArray &source);
    bool decryptData(const QString &userID, QByteArray *destination, const QByteArray &source);

//    void slotCheckIMUsersOnlineStatus();


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
    quint16 getLastReceivedPacketSN(const QString &peerID);


private:
//    QHostAddress serverTCPListeningAddress;
//    quint16 serverTCPListeningPort;
    QString m_serverName;

    QHostAddress localIPMCListeningAddress;
    quint16 localIPMCListeningPort;

    QHostAddress localRUDPListeningAddress;
    quint16 localRUDPListeningPort;

    QTimer *heartbeatTimer;
    //        QTimer *processWaitingForReplyPacketsTimer;

    PacketHandlerBase *m_packetHandlerBase;
    //ServerNetworkManager *networkManager;
    UsersManager usersManager;
    Cryptography *cryptography;

    QMutex mutex;

    QHash<QString, QByteArray> sessionEncryptionKeysHash;

    QTimer *checkIMUsersOnlineStateTimer;


    QMultiHash<QString /*Peer ID*/, QPair<quint16 /*Packet Serial Number*/, QDateTime/*Received Time*/> > m_receivedPacketsHash;



};

}

#endif /* SERVERPACKETSPARSER_H_ */
