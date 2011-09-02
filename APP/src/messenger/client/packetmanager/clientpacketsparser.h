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
    ClientPacketsParser(ClientNetworkManager *networkManager, QObject *parent = 0);
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


    void startHeartbeat(int interval = HEARTBEAT_TIMER_INTERVAL);
    void stopHeartbeat();

public slots:


    quint16 sendHeartbeatPacket(){
        qDebug()<<"----sendHeartbeatPacket()";

        if(m_serverAddress.isNull()){
            m_serverAddress = QHostAddress::Broadcast;
        } 

        //UDPPacket *packet = new UDPPacket(serverAddress.toString(), quint16(IP_MULTICAST_GROUP_PORT), localUDPListeningAddress.toString(), localUDPListeningPort);
        Packet *packet = m_packetHandlerBase->getPacket(m_serverAddress, m_serverUDPListeningPort, localUDPListeningAddress, localUDPListeningPort);

        packet->setPacketType(quint8(HEHUI::HeartbeatPacket));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_myUserID;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

        return packet->getPacketSerialNumber();

    }


    quint16 sendConfirmationOfReceiptPacket(const QHostAddress peerAddress, quint16 peerPort, quint16 packetSerialNumber, const QString &peerID){
        qDebug()<<"----sendConfirmationOfReceiptPacket(...)";

        //        UDPPacket *packet = new UDPPacket(peerAddress, peerPort, localUDPListeningAddress, localUDPListeningPort);
        //UDPPacket *packet = new UDPPacket(peerAddress, peerPort);
        //Packet *packet = packetHandlerBase->getPacket(peerAddress, peerPort);
        Packet *packet = m_packetHandlerBase->getPacket(peerAddress, peerPort, localUDPListeningAddress, localUDPListeningPort);
        
        
        
        packet->setPacketType(quint8(HEHUI::ConfirmationOfReceiptPacket));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_myUserID << packetSerialNumber << getLastReceivedPacketSN(peerID);
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

        return packet->getPacketSerialNumber();
    }

    quint16 sendClientLookForServerPacket(const QHostAddress &targetAddress, quint16 targetPort){
        qDebug()<<"----sendClientLookForServerPacket(...)";

        //UDPPacket *packet = new UDPPacket(QString(IP_MULTICAST_GROUP_ADDRESS), quint16(IP_MULTICAST_GROUP_PORT), localUDPListeningAddress.toString(), localUDPListeningPort);
        //UDPPacket *packet = new UDPPacket(address, quint16(IP_MULTICAST_GROUP_PORT), localUDPListeningAddress, localUDPListeningPort);
        Packet *packet = m_packetHandlerBase->getPacket(targetAddress, targetPort, localUDPListeningAddress, localUDPListeningPort);
        
        packet->setPacketType(quint8(IM::ClientLookForServer));
        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES*3));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_myUserID;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

        clientLookForServerPacketSerialNumber = packet->getPacketSerialNumber();
        return clientLookForServerPacketSerialNumber;
        //return packet->getPacketSerialNumber();

    }

    quint16 requestRegistration(const QString &serverHostAddress, quint16 serverHostPort){
        qWarning()<<"--CLIENT_REQUEST_REGISTRATION(...)";

        m_serverAddress = QHostAddress(serverHostAddress);
        m_serverUDPListeningPort = serverHostPort;

        Packet *packet = m_packetHandlerBase->getPacket(m_serverAddress, m_serverUDPListeningPort, localUDPListeningAddress, localUDPListeningPort);
        packet->setPacketType(quint8(IM::CLIENT_REQUEST_REGISTRATION));
        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);

        out << m_myUserID;

        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

        return packet->getPacketSerialNumber();
    }
    quint16 registration(const QString &serverHostAddress, quint16 serverHostPort, const QString &userID, const QString &password, const QString &email){
        qWarning()<<"--registration(...)";

        m_serverAddress = QHostAddress(serverHostAddress);
        m_serverUDPListeningPort = serverHostPort;
        
        Packet *packet = m_packetHandlerBase->getPacket(m_serverAddress, m_serverUDPListeningPort, localUDPListeningAddress, localUDPListeningPort);
        packet->setPacketType(quint8(IM::CLIENT_REGISTRATION));
        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        
        out << m_myUserID << userID << password << email;
        
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

        return packet->getPacketSerialNumber();
    }


    quint16 updatePassword(const QString &newPassword){
        qDebug()<<"--updatePassword(...)";
        
        QByteArray encryptedNewPassword;
        cryptography->teaCrypto(&encryptedNewPassword, newPassword.toUtf8(), sessionEncryptionKey, true);

        Packet *packet = m_packetHandlerBase->getPacket(m_serverAddress, m_serverUDPListeningPort, localUDPListeningAddress, localUDPListeningPort);
        packet->setPacketType(quint8(IM::CLIENT_REQUEST_UPDATE_PASSWORD));
        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_myUserID << encryptedNewPassword;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

        return packet->getPacketSerialNumber();
    }

    quint16 requestLogin(const QHostAddress &serverHostAddress, quint16 serverHostPort, const QString &clientVersion = APP_VERSION){
        qDebug()<<"--requestLogin(...)";
        
        m_myUserID = user->getUserID();
        sessionEncryptionKey = cryptography->SHA1(user->getPassword().toUtf8());
        Q_ASSERT_X(!m_myUserID.isEmpty(), "requestLogin", "Invalid User ID!");
        if(m_myUserID.isEmpty()){
            qCritical()<<"Invalid User ID!";
            return 0;
        }
        qWarning()<<"m_myUserID:"<<m_myUserID;
        m_serverAddress = serverHostAddress;
        m_serverUDPListeningPort = serverHostPort;

        Packet *packet = m_packetHandlerBase->getPacket(serverHostAddress, serverHostPort, localUDPListeningAddress, localUDPListeningPort);

        packet->setPacketType(quint8(IM::CLIENT_REQUEST_LOGIN));
        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_myUserID << clientVersion;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

        return packet->getPacketSerialNumber();
    }


    //quint16 changeMyOnlineState(IM::OnlineState onlineState){
    quint16 changeMyOnlineState(quint8 onlineStateCode){
        qDebug()<<"--changeMyOnlineState(...)";

        Packet *packet = m_packetHandlerBase->getPacket(m_serverAddress, m_serverUDPListeningPort, localUDPListeningAddress, localUDPListeningPort);
        packet->setPacketType(quint8(IM::ONLINE_STATE_CHANGED));
        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
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

        return packet->getPacketSerialNumber();
    }

    quint16 logout(){
        qDebug()<<"--logout(...)";
        //        Packet *packet = packetHandlerBase->getPacket(serverAddress, serverUDPListeningPort, localUDPListeningAddress, localUDPListeningPort);
        //        packet->setPacketType(quint8(IM::CLIENT_LOGOUT));
        //        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        //        QByteArray ba;
        //        QDataStream out(&ba, QIODevice::WriteOnly);
        //        out.setVersion(QDataStream::Qt_4_7);
        //        out << myUserID << user->getPassword();
        //        packet->setPacketData(ba);
        //        packetHandlerBase->appendOutgoingPacket(packet);

        //        return packet->getPacketSerialNumber();

        return changeMyOnlineState(IM::ONLINESTATE_OFFLINE);

    }


    quint16 requestContactInfo(const QString &contactID){
        qDebug()<<"--requestContactInfo(...)";
        
        QByteArray encryptedID;
        cryptography->teaCrypto(&encryptedID, contactID.toUtf8(), sessionEncryptionKey, true);

        Packet *packet = m_packetHandlerBase->getPacket(m_serverAddress, m_serverUDPListeningPort, localUDPListeningAddress, localUDPListeningPort);
        packet->setPacketType(quint8(IM::CLIENT_REQUEST_USER_SUMMARY_INFO));
        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
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

        return packet->getPacketSerialNumber();
    }

    quint16 requestPersonalInfo(){
        qDebug()<<"--requestPersonalInfo(...)";
        
        return requestContactInfo(m_myUserID);

    }



    quint16 searchContact(const QString &propertiesString, bool matchExactly, bool searchOnlineUsersOnly){
        qDebug()<<"--searchContact(...)";
        qWarning()<<"localUDPListeningPort:"<<localUDPListeningPort;

        Packet *packet = m_packetHandlerBase->getPacket(m_serverAddress, m_serverUDPListeningPort, localUDPListeningAddress, localUDPListeningPort);
        packet->setPacketType(quint8(IM::CLIENT_REQUEST_SEARCH_CONTACTS));
        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
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

        return packet->getPacketSerialNumber();
    }

    quint16 addContact(const QString &contactID, const QString &verificationMessage = "", const QString &groupName = ""){
        qWarning()<<"--addContact(...)";
        
        Packet *packet = m_packetHandlerBase->getPacket(m_serverAddress, m_serverUDPListeningPort, localUDPListeningAddress, localUDPListeningPort);
        packet->setPacketType(quint8(IM::CLIENT_REQUEST_ADD_CONTACT));
        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
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

        return packet->getPacketSerialNumber();

    }
    
    quint16 moveContactToGroup(const QString &contactID, const QString &oldGroupName, const QString &newGroupName){
        qDebug()<<"--moveContactToGroup(...)";

        Packet *packet = m_packetHandlerBase->getPacket(m_serverAddress, m_serverUDPListeningPort, localUDPListeningAddress, localUDPListeningPort);
        packet->setPacketType(quint8(IM::CLIENT_REQUEST_MOVE_CONTACT));
        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
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

        return packet->getPacketSerialNumber();

    }

    quint16 responseAddContactRequestFromUser(const QString &userID, bool acceptRequest, const QString &extraMessage = ""){
        qDebug()<<"--responseAddContactRequestFromUser(...)";
        
        Packet *packet = m_packetHandlerBase->getPacket(m_serverAddress, m_serverUDPListeningPort, localUDPListeningAddress, localUDPListeningPort);
        packet->setPacketType(quint8(IM::CLIENT_RESPONSE_ADD_CONTACT_REQUEST));
        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
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

        return packet->getPacketSerialNumber();

    }

    
    
    quint16 deleteContact(const QString &contactID, const QString &groupName, bool deleteMeFromOpposition = false){
        qDebug()<<"--deleteContact(...)";
        
        Packet *packet = m_packetHandlerBase->getPacket(m_serverAddress, m_serverUDPListeningPort, localUDPListeningAddress, localUDPListeningPort);
        packet->setPacketType(quint8(IM::CLIENT_REQUEST_DELETE_CONTACT));
        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
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

        return packet->getPacketSerialNumber();

    }

    quint16 addOrDeleteBlacklistedContact(const QString &contactID,  bool addToBlacklist = true){
        qDebug()<<"--addOrDeleteBlacklistedContact(...)";

        Packet *packet = m_packetHandlerBase->getPacket(m_serverAddress, m_serverUDPListeningPort, localUDPListeningAddress, localUDPListeningPort);
        packet->setPacketType(quint8(IM::CLIENT_REQUEST_ADD_OR_DELETE_BLACKLISTED_CONTACT));
        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
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

        return packet->getPacketSerialNumber();

    }

    quint16 requestInterestGroupsList(){
        qDebug()<<"------------------requestInterestGroupsList(...)";

        Packet *packet = m_packetHandlerBase->getPacket(m_serverAddress, m_serverUDPListeningPort, localUDPListeningAddress, localUDPListeningPort);
        packet->setPacketType(quint8(IM::CLIENT_REQUEST_INTEREST_GROUPS_LIST));
        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_myUserID;

        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

        return packet->getPacketSerialNumber();
    }

    quint16 requestInterestGroupInfo(quint32 groupID){

        Packet *packet = m_packetHandlerBase->getPacket(m_serverAddress, m_serverUDPListeningPort, localUDPListeningAddress, localUDPListeningPort);
        packet->setPacketType(quint8(IM::CLIENT_REQUEST_INTEREST_GROUP_INFO));
        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
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

        return packet->getPacketSerialNumber();

    }

    quint16 requestInterestGroupMembersInfo(quint32 groupID){
        qDebug()<<"----requestInterestGroupMembersInfo(...)";

        Packet *packet = m_packetHandlerBase->getPacket(m_serverAddress, m_serverUDPListeningPort, localUDPListeningAddress, localUDPListeningPort);
        packet->setPacketType(quint8(IM::CLIENT_REQUEST_INTEREST_GROUP_MEMBERS_INFO));
        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
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

        return packet->getPacketSerialNumber();

    }



    quint16 requestBlacklistInfo(){
        qDebug()<<"--requestBlacklistInfo(...)";

        Packet *packet = m_packetHandlerBase->getPacket(m_serverAddress, m_serverUDPListeningPort, localUDPListeningAddress, localUDPListeningPort);
        packet->setPacketType(quint8(IM::BLACKLIST_INFO));
        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_myUserID;

        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

        return packet->getPacketSerialNumber();
    }

    quint16 updateContactRemarkInfo(quint32 contactID, const QString &remarkName, const QString &extraRemarkInfo){
        qDebug()<<"--updateContactRemarkInfo(...)";
        
        Packet *packet = m_packetHandlerBase->getPacket(m_serverAddress, m_serverUDPListeningPort, localUDPListeningAddress, localUDPListeningPort);
        packet->setPacketType(quint8(IM::CLIENT_REQUEST_MODIFY_CONTACT_REMARK));
        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
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

        return packet->getPacketSerialNumber();

    }

    quint16 renameContactGroup(const QString &oldGroupName, const QString &newGroupName){
        qDebug()<<"--renameContactGroup(...)";

        Packet *packet = m_packetHandlerBase->getPacket(m_serverAddress, m_serverUDPListeningPort, localUDPListeningAddress, localUDPListeningPort);
        packet->setPacketType(quint8(IM::RENAME_CONTACT_GROUP));
        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
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

        return packet->getPacketSerialNumber();

    }

    quint16 createOrDeleteContactGroup(const QString &groupName, bool create = true){
        qDebug()<<"--createOrDeleteContactGroup(...)";

        Packet *packet = m_packetHandlerBase->getPacket(m_serverAddress, m_serverUDPListeningPort, localUDPListeningAddress, localUDPListeningPort);
        packet->setPacketType(quint8(IM::CREATE_OR_DELETE_CONTACT_GROUP));
        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
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

        return packet->getPacketSerialNumber();

    }


    //    void requestPersonalInterestGroups();
    //    void requestInterestGroupInfo(quint32 groupID);
    //    void createInterestGroup(quint32 groupTypeID, const QString &groupName);
    //    void searchInterestGroupByName(const QString &groupName);
    //    void searchInterestGroupByTypeID(quint32 groupTypeID);
    //    void joinInterestGroup(quint32 groupID);
    //    void quitInterestGroup(quint32 groupID);

    quint16 requestSessionEncryptionKeyWithContact(quint32 contactID){
        qDebug()<<"--requestSessionEncryptionKeyWithContact(...)";
        
        Packet *packet = m_packetHandlerBase->getPacket(m_serverAddress, m_serverUDPListeningPort, localUDPListeningAddress, localUDPListeningPort);
        packet->setPacketType(quint8(IM::SESSION_ENCRYPTION_KEY_WITH_CONTACT));
        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
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

        return packet->getPacketSerialNumber();

    }

    quint16 sendChatMessageToContact(const QString &contactID, const QString &message, const QString &contactHostAddress, quint16 contactHostPort){
        qDebug()<<"--sendChatMessageToContact(...)";
        
        //        QHostAddress targetHostAddress = QHostAddress(contactHostAddress);
        //        quint16 targetHostPort = contactHostPort;
        //TODO:判断在线状态及加密KEY

        Packet *packet = m_packetHandlerBase->getPacket(QHostAddress(contactHostAddress), contactHostPort, localUDPListeningAddress, localUDPListeningPort);
        packet->setPacketType(quint8(IM::CHAT_MESSAGE_FROM_CONTACT));
        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
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

        return packet->getPacketSerialNumber();
    }

    quint16 sendChatMessageToServer(const QString &contactID, const QString &message){
        qDebug()<<"--sendChatMessageToServer(...)";

        Packet *packet = m_packetHandlerBase->getPacket(m_serverAddress, m_serverUDPListeningPort, localUDPListeningAddress, localUDPListeningPort);
        packet->setPacketType(quint8(IM::CHAT_MESSAGES_CACHED_ON_SERVER));
        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
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

        return packet->getPacketSerialNumber();
    }

    quint16 sendChatMessageToContact(const QString &contactID, const QString &message, const QStringList &imagePathList, const QString &contactHostAddress, quint16 contactHostPort){
        
        sendChatMessageToContact(contactID, message, contactHostAddress, contactHostPort);

        return sendImageFileToContact(contactID, imagePathList, contactHostAddress, contactHostPort);
    }

    quint16 sendInterestGroupChatMessageToContact(const QString &contactID, quint32 interestGroupID ,const QString &message, const QString &contactHostAddress, quint16 contactHostPort){
        qDebug()<<"--sendInterestGroupChatMessageToContact(...)";

        Packet *packet = m_packetHandlerBase->getPacket(QHostAddress(contactHostAddress), contactHostPort, localUDPListeningAddress, localUDPListeningPort);
        packet->setPacketType(quint8(IM::GROUP_CHAT_MESSAGE));
        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
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

        return packet->getPacketSerialNumber();
    }

    quint16 sendInterestGroupChatMessageToServer(quint32 interestGroupID ,const QString &message){
        qDebug()<<"--sendInterestGroupChatMessageToServer(...)";

        Packet *packet = m_packetHandlerBase->getPacket(m_serverAddress, m_serverUDPListeningPort, localUDPListeningAddress, localUDPListeningPort);
        packet->setPacketType(quint8(IM::GROUP_CHAT_MESSAGE));
        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
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

        return packet->getPacketSerialNumber();
    }


    quint16 sendImageFileToContact(const QString &contactID, const QStringList &filePathList, const QString &contactHostAddress, quint16 contactHostPort){
        //TODO:
        return 0;
    }

    quint16 requestSendFileToContact(const QString &contactID, const QStringList &filePathList){
        //TODO:
        return 0;
    }

    quint16 sendChatMessageToContacts(const QStringList &contacts, const QString &message, const QStringList &imagePathList){
        //TODO:
        return 0;
    }

    //    quint16 sendChatMessageToInterestGroup(quint32 interestGroupID, const QString &message, const QStringList &imagePathList){
    //        //TODO:
    //        return 0;
    //    }
    



private slots:


    //    int crypto(QByteArray *destination, const QByteArray &source, bool encrypt);

    ///////////////////////////////////////////////
    //quint16 login(const QString &serverAddress, quint16 serverPort){
    quint16 login(){
        //qWarning()<<"--login() "<<m_serverAddress.toString()<<":"<<m_serverUDPListeningPort;
    	//TODO:密码保存方式
        sessionEncryptionKey = user->encryptedPassword();
        QByteArray encryptedPassword;
        cryptography->teaCrypto(&encryptedPassword, QByteArray::fromBase64(user->getPassword().toUtf8()), sessionEncryptionKey, true);     
        
        Packet *packet = m_packetHandlerBase->getPacket(m_serverAddress, m_serverUDPListeningPort, localUDPListeningAddress, localUDPListeningPort);
        packet->setPacketType(quint8(IM::CLIENT_LOGIN_INFO));
        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_myUserID << encryptedPassword << quint8(user->getStateAfterLoggedin());
        //        qDebug()<<"----quint8(user->getStateAfterLoggedin()):"<<quint8(user->getStateAfterLoggedin());
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

        return packet->getPacketSerialNumber();

    }

    quint16 updatePersonalContactGroupsInfo(bool uploadToServer){
        //        qWarning()<<"--updatePersonalContactGroupsInfo(...)";
        

        Packet *packet = m_packetHandlerBase->getPacket(m_serverAddress, m_serverUDPListeningPort, localUDPListeningAddress, localUDPListeningPort);
        packet->setPacketType(quint8(IM::CONTACT_GROUPS_INFO));
        packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
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

        return packet->getPacketSerialNumber();

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





    void signalServerDeclarePacketReceived(const QString &m_serverAddress, quint16 serverUDPListeningPort, quint16 m_serverTCPListeningPort, const QString &serverName, const QString &version);
    void signalServerOnlinePacketReceived(const QHostAddress m_serverAddress, quint16 serverPort, const QString &serverName);
    void signalServerOfflinePacketReceived(const QHostAddress m_serverAddress, quint16 serverPort, const QString &serverName);
    void signalServerAnnouncementPacketReceived(const QString &announcement, bool mustRead = true);

private:
    quint16 getLastReceivedPacketSN(const QString &peerID);

private:
    QHostAddress m_serverAddress;
    quint16 m_serverTCPListeningPort;
    quint16 m_serverUDPListeningPort;
    QString serverName;

    quint16 clientLookForServerPacketSerialNumber;

    QHostAddress localUDPListeningAddress;
    quint16 localUDPListeningPort;

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
