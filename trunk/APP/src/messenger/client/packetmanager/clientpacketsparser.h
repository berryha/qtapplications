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


#include "../sharedim/constants_global_shared.h"
#include "../networkmanager/clientnetworkmanager.h"
#include "imuser.h"

//#include "../../../shared/network/packethandler/packetparserbase.h"

#include "HHSharedCore/hcryptography.h"
#include "HHSharedNetwork/hpacketparserbase.h"
#include "HHSharedNetwork/hnetworkutilities.h"



namespace HEHUI {


class ClientPacketsParser : public PacketsParserBase{
    Q_OBJECT
public:
    ClientPacketsParser(ClientResourcesManager *networkManager, QObject *parent = 0);
    virtual ~ClientPacketsParser();

    void setLocalUDPListeningAddress(const QHostAddress &address = QHostAddress::Any);
    void setLocalUDPListeningPort(quint16 port);


    void run();
    void parseIncomingPacketData(Packet *packet);

    bool readyToQuit();
    void startparseIncomingPackets();
    void startprocessOutgoingPackets();



    //void setServerAddress(const QHostAddress&serverAddress);
    QHostAddress getServerAddress() const;
    //void setServerPort(quint16 serverPort);
    quint16 getServerPort() const;

    void serverOffline();

//    void startHeartbeat(int interval = HEARTBEAT_TIMER_INTERVAL);
//    void stopHeartbeat();

public slots:


//    void sendHeartbeatPacket(){
//        qDebug()<<"----sendHeartbeatPacket()";

//        if(m_serverAddress.isNull()){
//            m_serverAddress = QHostAddress::Broadcast;
//        }

//        Packet *packet = m_packetHandlerBase->getPacket(m_serverAddress, m_serverRUDPListeningPort, localRUDPListeningAddress, localRUDPListeningPort);

//        packet->setPacketType(quint8(HEHUI::HeartbeatPacket));
//        packet->setTransmissionProtocol(TP_RUDP);
//        QByteArray ba;
//        QDataStream out(&ba, QIODevice::WriteOnly);
//        out.setVersion(QDataStream::Qt_4_7);
//        out << m_myUserID;
//        packet->setPacketData(ba);
//        m_packetHandlerBase->appendOutgoingPacket(packet);


//    }


//    void sendConfirmationOfReceiptPacket(const QHostAddress peerAddress, quint16 peerPort, quint16 packetSerialNumber, const QString &peerID){
//        qDebug()<<"----sendConfirmationOfReceiptPacket(...)";

//        Packet *packet = m_packetHandlerBase->getPacket(peerAddress, peerPort, localRUDPListeningAddress, localRUDPListeningPort);
        
//        packet->setPacketType(quint8(HEHUI::ConfirmationOfReceiptPacket));
//        QByteArray ba;
//        QDataStream out(&ba, QIODevice::WriteOnly);
//        out.setVersion(QDataStream::Qt_4_7);
//        out << m_myUserID << packetSerialNumber << getLastReceivedPacketSN(peerID);
//        packet->setPacketData(ba);
//        m_packetHandlerBase->appendOutgoingPacket(packet);

//    }

    void sendClientLookForServerPacket(const QHostAddress &targetAddress, quint16 targetPort){
        //qDebug()<<"----sendClientLookForServerPacket(...)";

        Packet *packet = m_packetHandlerBase->getPacket(targetAddress, targetPort);
        
        packet->setPacketType(quint8(IM::ClientLookForServer));
        packet->setTransmissionProtocol(TP_UDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES*3));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_myUserID << localRUDPListeningPort;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

    }

    void requestRegistration(const QString &serverHostAddress, quint16 serverHostPort){

        m_serverAddress = QHostAddress(serverHostAddress);
        m_serverRUDPListeningPort = serverHostPort;

        Packet *packet = m_packetHandlerBase->getPacket(m_serverAddress, m_serverRUDPListeningPort, localRUDPListeningAddress, localRUDPListeningPort);
        packet->setPacketType(quint8(IM::CLIENT_REQUEST_REGISTRATION));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);

        out << m_myUserID;

        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

    }

    void registration(const QString &serverHostAddress, quint16 serverHostPort, const QString &userID, const QString &password, const QString &email){
        qWarning()<<"--registration(...)";

        m_serverAddress = QHostAddress(serverHostAddress);
        m_serverRUDPListeningPort = serverHostPort;
        
        Packet *packet = m_packetHandlerBase->getPacket(m_serverAddress, m_serverRUDPListeningPort, localRUDPListeningAddress, localRUDPListeningPort);
        packet->setPacketType(quint8(IM::CLIENT_REGISTRATION));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        
        out << m_myUserID << userID << password << email;
        
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

    }


    void updatePassword(const QString &newPassword){
        qDebug()<<"--updatePassword(...)";
        
        QByteArray encryptedNewPassword;
        cryptography->teaCrypto(&encryptedNewPassword, newPassword.toUtf8(), sessionEncryptionKey, true);

        Packet *packet = m_packetHandlerBase->getPacket(m_serverAddress, m_serverRUDPListeningPort, localRUDPListeningAddress, localRUDPListeningPort);
        packet->setPacketType(quint8(IM::CLIENT_REQUEST_UPDATE_PASSWORD));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_myUserID << encryptedNewPassword;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

    }

    void requestLogin(const QHostAddress &serverHostAddress, quint16 serverHostPort, const QString &clientVersion = APP_VERSION){
        qDebug()<<"--requestLogin(...)";
        
        m_myUserID = user->getUserID();
        sessionEncryptionKey = cryptography->SHA1(user->getPassword().toUtf8());
        Q_ASSERT_X(!m_myUserID.isEmpty(), "requestLogin", "Invalid User ID!");
        if(m_myUserID.isEmpty()){
            qCritical()<<"Invalid User ID!";
            return ;
        }
        qWarning()<<"m_myUserID:"<<m_myUserID;
        m_serverAddress = serverHostAddress;
        m_serverRUDPListeningPort = serverHostPort;

        Packet *packet = m_packetHandlerBase->getPacket(m_serverAddress, m_serverRUDPListeningPort, localRUDPListeningAddress, localRUDPListeningPort);

        packet->setPacketType(quint8(IM::CLIENT_REQUEST_LOGIN));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_myUserID << clientVersion;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

    }

    void changeMyOnlineState(quint8 onlineStateCode){
        qDebug()<<"--changeMyOnlineState(...)";

        Packet *packet = m_packetHandlerBase->getPacket(m_serverAddress, m_serverRUDPListeningPort, localRUDPListeningAddress, localRUDPListeningPort);
        packet->setPacketType(quint8(IM::ONLINE_STATE_CHANGED));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        
        out << onlineStateCode;
        QByteArray encryptedData;
        cryptography->teaCrypto(&encryptedData, ba, sessionEncryptionKey, true);
        ba.clear();
        out.device()->seek(0);
        out << m_myUserID << encryptedData;
        
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

    }

    void logout(){
        qDebug()<<"--logout(...)";
        changeMyOnlineState(IM::ONLINESTATE_OFFLINE);

    }

    void requestContactInfo(const QString &contactID){
        qDebug()<<"--requestContactInfo(...)";
        
        QByteArray encryptedID;
        cryptography->teaCrypto(&encryptedID, contactID.toUtf8(), sessionEncryptionKey, true);

        Packet *packet = m_packetHandlerBase->getPacket(m_serverAddress, m_serverRUDPListeningPort, localRUDPListeningAddress, localRUDPListeningPort);
        packet->setPacketType(quint8(IM::CLIENT_REQUEST_USER_SUMMARY_INFO));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        
        out << contactID;
        QByteArray encryptedData;
        cryptography->teaCrypto(&encryptedData, ba, sessionEncryptionKey, true);
        
        ba.clear();
        out.device()->seek(0);
        out << m_myUserID << encryptedData;
        
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

    }

    void requestPersonalInfo(){
        qDebug()<<"--requestPersonalInfo(...)";
        
        requestContactInfo(m_myUserID);

    }

    void searchContact(const QString &propertiesString, bool matchExactly, bool searchOnlineUsersOnly){
        qDebug()<<"--searchContact(...)";

        Packet *packet = m_packetHandlerBase->getPacket(m_serverAddress, m_serverRUDPListeningPort, localRUDPListeningAddress, localRUDPListeningPort);
        packet->setPacketType(quint8(IM::CLIENT_REQUEST_SEARCH_CONTACTS));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);

        out << propertiesString << matchExactly << searchOnlineUsersOnly;
        QByteArray encryptedData;
        cryptography->teaCrypto(&encryptedData, ba, sessionEncryptionKey, true);
        ba.clear();
        out.device()->seek(0);
        out << m_myUserID << encryptedData;

        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

    }

    void addContact(const QString &contactID, const QString &verificationMessage = "", const QString &groupName = ""){
        qWarning()<<"--addContact(...)";
        
        Packet *packet = m_packetHandlerBase->getPacket(m_serverAddress, m_serverRUDPListeningPort, localRUDPListeningAddress, localRUDPListeningPort);
        packet->setPacketType(quint8(IM::CLIENT_REQUEST_ADD_CONTACT));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);

        out << contactID << verificationMessage << groupName;
        QByteArray encryptedData;
        cryptography->teaCrypto(&encryptedData, ba, sessionEncryptionKey, true);
        ba.clear();
        out.device()->seek(0);
        out << m_myUserID << encryptedData;


        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);


    }
    
    void moveContactToGroup(const QString &contactID, const QString &oldGroupName, const QString &newGroupName){
        qDebug()<<"--moveContactToGroup(...)";

        Packet *packet = m_packetHandlerBase->getPacket(m_serverAddress, m_serverRUDPListeningPort, localRUDPListeningAddress, localRUDPListeningPort);
        packet->setPacketType(quint8(IM::CLIENT_REQUEST_MOVE_CONTACT));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);

        out << contactID << oldGroupName << newGroupName;
        QByteArray encryptedData;
        cryptography->teaCrypto(&encryptedData, ba, sessionEncryptionKey, true);
        ba.clear();
        out.device()->seek(0);
        out << m_myUserID << encryptedData;


        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);


    }

    void responseAddContactRequestFromUser(const QString &userID, bool acceptRequest, const QString &extraMessage = ""){
        qDebug()<<"--responseAddContactRequestFromUser(...)";
        
        Packet *packet = m_packetHandlerBase->getPacket(m_serverAddress, m_serverRUDPListeningPort, localRUDPListeningAddress, localRUDPListeningPort);
        packet->setPacketType(quint8(IM::CLIENT_RESPONSE_ADD_CONTACT_REQUEST));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);

        out << userID << acceptRequest << extraMessage;
        QByteArray encryptedData;
        cryptography->teaCrypto(&encryptedData, ba, sessionEncryptionKey, true);
        ba.clear();
        out.device()->seek(0);
        out << m_myUserID << encryptedData;


        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);


    }

    
    
    void deleteContact(const QString &contactID, const QString &groupName, bool deleteMeFromOpposition = false){
        qDebug()<<"--deleteContact(...)";
        
        Packet *packet = m_packetHandlerBase->getPacket(m_serverAddress, m_serverRUDPListeningPort, localRUDPListeningAddress, localRUDPListeningPort);
        packet->setPacketType(quint8(IM::CLIENT_REQUEST_DELETE_CONTACT));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);

        out << contactID << groupName << deleteMeFromOpposition;
        QByteArray encryptedData;
        cryptography->teaCrypto(&encryptedData, ba, sessionEncryptionKey, true);
        ba.clear();
        out.device()->seek(0);
        out << m_myUserID << encryptedData;

        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);


    }

    void addOrDeleteBlacklistedContact(const QString &contactID,  bool addToBlacklist = true){
        qDebug()<<"--addOrDeleteBlacklistedContact(...)";

        Packet *packet = m_packetHandlerBase->getPacket(m_serverAddress, m_serverRUDPListeningPort, localRUDPListeningAddress, localRUDPListeningPort);
        packet->setPacketType(quint8(IM::CLIENT_REQUEST_ADD_OR_DELETE_BLACKLISTED_CONTACT));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);

        out << contactID << addToBlacklist;
        QByteArray encryptedData;
        cryptography->teaCrypto(&encryptedData, ba, sessionEncryptionKey, true);
        ba.clear();
        out.device()->seek(0);
        out << m_myUserID << encryptedData;

        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);


    }

    void requestInterestGroupsList(){
        qDebug()<<"------------------requestInterestGroupsList(...)";

        Packet *packet = m_packetHandlerBase->getPacket(m_serverAddress, m_serverRUDPListeningPort, localRUDPListeningAddress, localRUDPListeningPort);
        packet->setPacketType(quint8(IM::CLIENT_REQUEST_INTEREST_GROUPS_LIST));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_myUserID;

        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

    }

    void requestInterestGroupInfo(quint32 groupID){

        Packet *packet = m_packetHandlerBase->getPacket(m_serverAddress, m_serverRUDPListeningPort, localRUDPListeningAddress, localRUDPListeningPort);
        packet->setPacketType(quint8(IM::CLIENT_REQUEST_INTEREST_GROUP_INFO));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);

        //out << m_myUserID << groupID;
        out << groupID;
        QByteArray encryptedData;
        cryptography->teaCrypto(&encryptedData, ba, sessionEncryptionKey, true);
        ba.clear();
        out.device()->seek(0);
        out << m_myUserID << encryptedData;

        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);


    }

    void requestInterestGroupMembersInfo(quint32 groupID){
        qDebug()<<"----requestInterestGroupMembersInfo(...)";

        Packet *packet = m_packetHandlerBase->getPacket(m_serverAddress, m_serverRUDPListeningPort, localRUDPListeningAddress, localRUDPListeningPort);
        packet->setPacketType(quint8(IM::CLIENT_REQUEST_INTEREST_GROUP_MEMBERS_INFO));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);

        //out << m_myUserID << groupID;
        out << groupID;
        QByteArray encryptedData;
        cryptography->teaCrypto(&encryptedData, ba, sessionEncryptionKey, true);
        ba.clear();
        out.device()->seek(0);
        out << m_myUserID << encryptedData;

        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);


    }



    void requestBlacklistInfo(){
        qDebug()<<"--requestBlacklistInfo(...)";

        Packet *packet = m_packetHandlerBase->getPacket(m_serverAddress, m_serverRUDPListeningPort, localRUDPListeningAddress, localRUDPListeningPort);
        packet->setPacketType(quint8(IM::BLACKLIST_INFO));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_myUserID;

        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

    }

    void updateContactRemarkInfo(quint32 contactID, const QString &remarkName, const QString &extraRemarkInfo){
        qDebug()<<"--updateContactRemarkInfo(...)";
        
        Packet *packet = m_packetHandlerBase->getPacket(m_serverAddress, m_serverRUDPListeningPort, localRUDPListeningAddress, localRUDPListeningPort);
        packet->setPacketType(quint8(IM::CLIENT_REQUEST_MODIFY_CONTACT_REMARK));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);

        out << contactID << remarkName << extraRemarkInfo;
        QByteArray encryptedData;
        cryptography->teaCrypto(&encryptedData, ba, sessionEncryptionKey, true);
        ba.clear();
        out.device()->seek(0);
        out << m_myUserID << encryptedData;

        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);


    }

    void renameContactGroup(const QString &oldGroupName, const QString &newGroupName){
        qDebug()<<"--renameContactGroup(...)";

        Packet *packet = m_packetHandlerBase->getPacket(m_serverAddress, m_serverRUDPListeningPort, localRUDPListeningAddress, localRUDPListeningPort);
        packet->setPacketType(quint8(IM::RENAME_CONTACT_GROUP));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);

        out << oldGroupName << newGroupName;
        QByteArray encryptedData;
        cryptography->teaCrypto(&encryptedData, ba, sessionEncryptionKey, true);
        ba.clear();
        out.device()->seek(0);
        out << m_myUserID << encryptedData;


        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);


    }

    void createOrDeleteContactGroup(const QString &groupName, bool create = true){
        qDebug()<<"--createOrDeleteContactGroup(...)";

        Packet *packet = m_packetHandlerBase->getPacket(m_serverAddress, m_serverRUDPListeningPort, localRUDPListeningAddress, localRUDPListeningPort);
        packet->setPacketType(quint8(IM::CREATE_OR_DELETE_CONTACT_GROUP));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);

        out << groupName << create;
        QByteArray encryptedData;
        cryptography->teaCrypto(&encryptedData, ba, sessionEncryptionKey, true);
        ba.clear();
        out.device()->seek(0);
        out << m_myUserID << encryptedData;


        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);


    }

    void requestSessionEncryptionKeyWithContact(quint32 contactID){
        qDebug()<<"--requestSessionEncryptionKeyWithContact(...)";
        
        Packet *packet = m_packetHandlerBase->getPacket(m_serverAddress, m_serverRUDPListeningPort, localRUDPListeningAddress, localRUDPListeningPort);
        packet->setPacketType(quint8(IM::SESSION_ENCRYPTION_KEY_WITH_CONTACT));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);

        out << contactID;
        QByteArray encryptedData;
        cryptography->teaCrypto(&encryptedData, ba, sessionEncryptionKey, true);
        ba.clear();
        out.device()->seek(0);
        out << m_myUserID << encryptedData;

        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);


    }

    void sendChatMessageToContact(const QString &contactID, const QString &message, const QString &contactHostAddress, quint16 contactHostPort){
        qDebug()<<"--sendChatMessageToContact(...)";
        
        //        QHostAddress targetHostAddress = QHostAddress(contactHostAddress);
        //        quint16 targetHostPort = contactHostPort;
        //TODO:判断在线状态及加密KEY

        Packet *packet = m_packetHandlerBase->getPacket(QHostAddress(contactHostAddress), contactHostPort, localRUDPListeningAddress, localRUDPListeningPort);
        packet->setPacketType(quint8(IM::CHAT_MESSAGE_FROM_CONTACT));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);

        out << message;
        QByteArray encryptedData;
        cryptography->teaCrypto(&encryptedData, ba, sessionEncryptionKeyWithContactHash.value(contactID), true);
        ba.clear();
        out.device()->seek(0);
        out << m_myUserID << encryptedData;

        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

    }

    void sendChatMessageToServer(const QString &contactID, const QString &message){
        qDebug()<<"--sendChatMessageToServer(...)";

        Packet *packet = m_packetHandlerBase->getPacket(m_serverAddress, m_serverRUDPListeningPort, localRUDPListeningAddress, localRUDPListeningPort);
        packet->setPacketType(quint8(IM::CHAT_MESSAGES_CACHED_ON_SERVER));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);

        out << contactID << message;
        QByteArray encryptedData;
        cryptography->teaCrypto(&encryptedData, ba, sessionEncryptionKey, true);
        ba.clear();
        out.device()->seek(0);
        out << m_myUserID << encryptedData;

        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

    }

    void sendChatMessageToContact(const QString &contactID, const QString &message, const QStringList &imagePathList, const QString &contactHostAddress, quint16 contactHostPort){
        
        sendChatMessageToContact(contactID, message, contactHostAddress, contactHostPort);

        sendImageFileToContact(contactID, imagePathList, contactHostAddress, contactHostPort);
    }

    void sendInterestGroupChatMessageToContact(const QString &contactID, quint32 interestGroupID ,const QString &message, const QString &contactHostAddress, quint16 contactHostPort){
        qDebug()<<"--sendInterestGroupChatMessageToContact(...)";

        Packet *packet = m_packetHandlerBase->getPacket(QHostAddress(contactHostAddress), contactHostPort, localRUDPListeningAddress, localRUDPListeningPort);
        packet->setPacketType(quint8(IM::GROUP_CHAT_MESSAGE));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);

        out << interestGroupID << message;
        QByteArray encryptedData;
        cryptography->teaCrypto(&encryptedData, ba, sessionEncryptionKeyWithContactHash.value(contactID), true);
        ba.clear();
        out.device()->seek(0);
        out << m_myUserID << encryptedData;

        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

    }

    void sendInterestGroupChatMessageToServer(quint32 interestGroupID ,const QString &message){
        qDebug()<<"--sendInterestGroupChatMessageToServer(...)";

        Packet *packet = m_packetHandlerBase->getPacket(m_serverAddress, m_serverRUDPListeningPort, localRUDPListeningAddress, localRUDPListeningPort);
        packet->setPacketType(quint8(IM::GROUP_CHAT_MESSAGE));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);

        out << interestGroupID << message;
        QByteArray encryptedData;
        cryptography->teaCrypto(&encryptedData, ba, sessionEncryptionKey, true);
        ba.clear();
        out.device()->seek(0);
        out << m_myUserID << encryptedData;

        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

    }


    void sendImageFileToContact(const QString &contactID, const QStringList &filePathList, const QString &contactHostAddress, quint16 contactHostPort){
        //TODO:
        return ;
    }

    void requestSendFileToContact(const QString &contactID, const QStringList &filePathList){
        //TODO:
        return ;
    }

    void sendChatMessageToContacts(const QStringList &contacts, const QString &message, const QStringList &imagePathList){
        //TODO:
        return ;
    }

    //    quint16 sendChatMessageToInterestGroup(quint32 interestGroupID, const QString &message, const QStringList &imagePathList){
    //        //TODO:
    //        return 0;
    //    }
    



private slots:


    //    int crypto(QByteArray *destination, const QByteArray &source, bool encrypt);

    ///////////////////////////////////////////////
    //quint16 login(const QString &serverAddress, quint16 serverPort){
    void login(){
        //qWarning()<<"--login() "<<m_serverAddress.toString()<<":"<<m_serverUDPListeningPort;
    	//TODO:密码保存方式
        sessionEncryptionKey = user->encryptedPassword();
        QByteArray encryptedPassword;
        cryptography->teaCrypto(&encryptedPassword, QByteArray::fromBase64(user->getPassword().toUtf8()), sessionEncryptionKey, true);     
        
        Packet *packet = m_packetHandlerBase->getPacket(m_serverAddress, m_serverRUDPListeningPort, localRUDPListeningAddress, localRUDPListeningPort);
        packet->setPacketType(quint8(IM::CLIENT_LOGIN_INFO));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_myUserID << encryptedPassword << quint8(user->getStateAfterLoggedin());
        //        qDebug()<<"----quint8(user->getStateAfterLoggedin()):"<<quint8(user->getStateAfterLoggedin());
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);


    }

    void updatePersonalContactGroupsInfo(bool uploadToServer){
        //        qWarning()<<"--updatePersonalContactGroupsInfo(...)";
        

        Packet *packet = m_packetHandlerBase->getPacket(m_serverAddress, m_serverRUDPListeningPort, localRUDPListeningAddress, localRUDPListeningPort);
        packet->setPacketType(quint8(IM::CONTACT_GROUPS_INFO));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);

        if(uploadToServer){
            out << user->getPersonalContactGroupsVersion() << user->getContactGroupsInfoString();
            QByteArray encryptedData;
            cryptography->teaCrypto(&encryptedData, ba, sessionEncryptionKey, true);
            ba.clear();
            out.device()->seek(0);
            out << m_myUserID << quint8(uploadToServer) << encryptedData;
        }else{
            out << m_myUserID << quint8(uploadToServer);
        }

        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);


    }
    


    ///////////////////////////////////////////////



signals:
    void  signalHeartbeatPacketReceived(const QString &contactID);
    void  signalConfirmationOfReceiptPacketReceived(quint16 packetSerialNumber1, quint16 packetSerialNumber2);

    void signalRegistrationResultReceived(quint8 errorTypeCode, const QString &message);
    void signalUpdatePasswordResultReceived(quint8 errorTypeCode, const QString &message);
    
    //void signalLoginResultReceived(IM::ErrorType errorType);
    void signalLoginResultReceived(quint8 errorType);
    //void signalContactStateChangedPacketReceived(const QString &contactID, IM::OnlineState onlineState, const QString &contactHostAddress, quint16 contactHostPort);
    void signalContactStateChangedPacketReceived(const QString &contactID, quint8 onlineState, const QString &contactHostAddress, quint16 contactHostPort);
    void signalContactsOnlineInfoPacketReceived(const QString &contactsOnlineInfoString);
    void signalUserInfoPacketReceived(const QString &userID, const QString &userInfo);
    void signalContactGroupsInfoPacketReceived(const QString &contactGroupsInfo, quint32 personalContactGroupsInfoVersionOnServer);

    void signalSearchContactsResultPacketReceived(const QStringList &users);
    //void signalAddContactResultPacketReceived(const QString &contactID, IM::ErrorType errorType);
    void signalAddContactRequestFromUserPacketReceived(const QString &userID, const QString &userNickName, const QString &userFace, const QString &verificationMessage);
    void signalAddContactResultPacketReceived(const QString &userID, const QString &userNickName, const QString &userFace, quint8 errorTypeCode, const QString &reasonMessage);

    void signalInterestGroupsListPacketReceived(const QString &systemGroupsListFromServer, quint32 systemInfoVersionOnServer);
    void signalInterestGroupInfoPacketReceived(const QString &interestGroupInfoFromServer, quint32 groupID);
    void signalInterestGroupMembersInfoPacketReceived(const QString &interestGroupMembersInfoFromServer, quint32 interestGroupMembersInfoVersionOnServer, quint32 groupID);
    
    
    void signalBlacklistInfoPacketReceived(const QString &blacklistOnServer, quint32 blacklistInfoVersionOnServer);

    void signalChatMessageReceivedFromContact(const QString &contactID, const QString &message, const QString &time);
    void signalChatMessageCachedOnServerReceived(const QStringList &messages);
    void signalChatImageReceived(const QString &fileName);

    void signalInterestGroupChatMessageReceivedFromContact(quint32 interestGroupID, const QString &contactID, const QString &message, const QString &time);
    void signalInterestGroupChatMessagesCachedOnServerReceived(const QStringList &messages);






    void contactGroupInfoPacketReceived();

    void interestGroupsListPacketReceived();
    void interestGroupInfoPacketReceived();

    void systemGroupsListPacketReceived();
    void systemGroupInfoPacketReceived();

    void signalServerResponseAddContactPacketReceived();
    void signalServerResponseDeleteContactPacketReceived();
    void signalServerResponseMoveContactPacketReceived();
    void signalServerResponseModifyContactRemarkPacketReceived();


    void signalClientSoftwareUpdatePacketReceived();





    void signalServerDeclarePacketReceived(const QString &m_serverAddress, quint16 serverRUDPListeningPort, const QString &serverName, const QString &version);
    void signalServerOnlinePacketReceived(const QHostAddress m_serverAddress, quint16 serverRUDPListeningPort, const QString &serverName);
    void signalServerOfflinePacketReceived(const QHostAddress m_serverAddress, quint16 serverRUDPListeningPort, const QString &serverName);
    void signalServerAnnouncementPacketReceived(const QString &announcement, bool mustRead = true);

private:
    quint16 getLastReceivedPacketSN(const QString &peerID);

private:
    QHostAddress m_serverAddress;
//    quint16 m_serverTCPListeningPort;
    quint16 m_serverRUDPListeningPort;
    QString serverName;

    quint16 clientLookForServerPacketSerialNumber;

    QHostAddress localRUDPListeningAddress;
    quint16 localRUDPListeningPort;

    QTimer *heartbeatTimer;
    //        QTimer *processWaitingForReplyPacketsTimer;

    //ClientNetworkManager *m_networkManager;
    PacketHandlerBase *m_packetHandlerBase;


    QDateTime serverLastOnlineTime;

    QMutex mutex;

    IMUser *user;
    QString m_myUserID;

    Cryptography *cryptography;
    QByteArray sessionEncryptionKey;

    QHash<QString/*Contact's ID*/, QByteArray/*Session Encryption Key*/> sessionEncryptionKeyWithContactHash;


    QMultiHash<QString /*Peer ID*/, QPair<quint16 /*Packet Serial Number*/, QDateTime/*Received Time*/> > m_receivedPacketsHash;


    int counter;
    QMutex counterMutex;




};

}

#endif /* CLIENTPACKETSPARSER_H_ */
