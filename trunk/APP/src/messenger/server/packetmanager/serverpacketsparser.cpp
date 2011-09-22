/*
 ****************************************************************************
 * serverpacketsparser.cpp
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

#include <QDebug>

#include "serverpacketsparser.h"
#include "../serverutilities.h"


#ifdef Q_CC_MSVC
#include <windows.h>
#include "HHSharedWindowsManagement/hwindowsmanagement.h"
#define msleep(x) Sleep(x)
#endif

#ifdef Q_CC_GNU
#include <unistd.h>
#define msleep(x) usleep(x*1000)
#endif



namespace HEHUI {


ServerPacketsParser::ServerPacketsParser(ServerNetworkManager *networkManager, QObject *parent)
    :PacketsParserBase(networkManager, parent), UsersManager(parent)
{

    Q_ASSERT_X(networkManager, "ServerPacketsParser::ServerPacketsParser(...)", "Invalid ServerNetworkManager!");

    m_packetHandlerBase = networkManager->getPacketHandler();
    Q_ASSERT_X(m_packetHandlerBase, "ServerPacketsParser::ServerPacketsParser(...)", "Invalid PacketHandlerBase!");


    heartbeatTimer = 0;
    //    processWaitingForReplyPacketsTimer = 0;

    //packetHandlerBase = new PacketHandlerBase(this);

    networkManager = ServerNetworkManager::instance();
//    serverTCPListeningAddress = networkManager->localTCPListeningAddress();
//    serverTCPListeningPort = networkManager->localTCPListeningPort();
    //m_serverName = networkManager->hostName();
    m_serverName = QHostInfo::localHostName().toLower();

    localIPMCListeningAddress = networkManager->localIPMCListeningAddress();
    localIPMCListeningPort = networkManager->localIPMCListeningPort();

    //    localUDPListeningAddress = networkManager->localUDPListeningAddress();
    //    localUDPListeningPort = networkManager->localUDPListeningPort();
    localRUDPListeningAddress = networkManager->localRUDPListeningAddress();
    localRUDPListeningPort = networkManager->localRUDPListeningPort();

    //    usersManager = UsersManager::instance();
    cryptography = new Cryptography();

    checkIMUsersOnlineStateTimer = 0;




}

ServerPacketsParser::~ServerPacketsParser() {
    // TODO Auto-generated destructor stub

    QMutexLocker locker(&mutex);
    
    if(checkIMUsersOnlineStateTimer){
        checkIMUsersOnlineStateTimer->stop();
    }
    delete checkIMUsersOnlineStateTimer;
    checkIMUsersOnlineStateTimer = 0;

    if(heartbeatTimer){
        heartbeatTimer->stop();
    }
    delete heartbeatTimer;
    heartbeatTimer = 0;

    //    if(processWaitingForReplyPacketsTimer){
    //        processWaitingForReplyPacketsTimer->stop();
    //    }
    //    delete processWaitingForReplyPacketsTimer;
    //    processWaitingForReplyPacketsTimer = 0;

    //    delete m_packetHandlerBase;
    //    m_packetHandlerBase = 0;


    delete cryptography;
    cryptography = 0;

}

//void ServerPacketsParser::setLocalRUDPListeningAddress(const QHostAddress &address){

//    this->localRUDPListeningAddress = address;
//}
//void ServerPacketsParser::setLocalRUDPListeningPort(quint16 port){

//    this->localRUDPListeningPort = port;
//}

void ServerPacketsParser::run(){

    QMutexLocker locker(&mutex);

//    QTimer processWaitingForReplyPacketsTimer;
//    processWaitingForReplyPacketsTimer.setSingleShot(false);
//    processWaitingForReplyPacketsTimer.setInterval(UDP_PACKET_WAITING_FOR_REPLY_TIMEOUT/2);
//    connect(&processWaitingForReplyPacketsTimer, SIGNAL(timeout()), this, SLOT(processWaitingForReplyPackets()));
//    connect(this, SIGNAL(signalAboutToQuit()), &processWaitingForReplyPacketsTimer, SLOT(stop()));
//    processWaitingForReplyPacketsTimer.start();

    while(!isAboutToQuit()){
        QCoreApplication::processEvents();
        parseIncomingPackets();
        processOutgoingPackets();
        msleep(50);
    }

//    processWaitingForReplyPacketsTimer.stop();

    processOutgoingPackets();



}

void ServerPacketsParser::startparseIncomingPackets(){

    while(!isAboutToQuit()){
        QCoreApplication::processEvents();
        parseIncomingPackets();
        msleep(50); 
    }

}

void ServerPacketsParser::startprocessOutgoingPackets(){

//    QTimer processWaitingForReplyPacketsTimer;
//    processWaitingForReplyPacketsTimer.setSingleShot(false);
//    processWaitingForReplyPacketsTimer.setInterval(UDP_PACKET_WAITING_FOR_REPLY_TIMEOUT/2);
//    connect(&processWaitingForReplyPacketsTimer, SIGNAL(timeout()), this, SLOT(processWaitingForReplyPackets()));
//    connect(this, SIGNAL(signalAboutToQuit()), &processWaitingForReplyPacketsTimer, SLOT(stop()));
//    processWaitingForReplyPacketsTimer.start();

    while(!isAboutToQuit()){
        //QCoreApplication::processEvents();
        processOutgoingPackets();
        msleep(50);
    }

//    processWaitingForReplyPacketsTimer.stop();

    processOutgoingPackets();

}


void ServerPacketsParser::parseIncomingPacketData(Packet *packet){

    //    qDebug()<<"----ServerPacketsParser::parseIncomingPacketData(Packet *packet)";


    QByteArray packetData = packet->getPacketData();
    QDataStream in(&packetData, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_7);

    QString peerID = "";
    in >> peerID;

    QHostAddress peerAddress = packet->getPeerHostAddress();
    quint16 peerPort = packet->getPeerHostPort();
    quint8 packetType = packet->getPacketType();
//    qDebug()<<"--ServerPacketsParser::parseIncomingPacketData(...) "<<" peerID:"<<peerID<<" peerAddress:"<<peerAddress<<" peerPort:"<<peerPort<<" packetType:"<<packetType;

    switch(packetType){
//    case quint8(HEHUI::HeartbeatPacket):
//    {
//        QString userID = peerID;
//        //in >> userID;

//        UserInfo *userInfo = getOnlineUserInfo(userID);
//        if(userInfo){
//            userInfo->updateRemainingOnlineStatusCheckingTimes(true);
//        }

//        emit signalHeartbeatPacketReceived(packet->getPeerHostAddress().toString(), userID);
//        qWarning()<<"~~HeartbeatPacket--"<<" userID:"<<userID;
//    }
//    break;
//    case quint8(HEHUI::ConfirmationOfReceiptPacket):
//    {
//        quint16 packetSerialNumber1 = 0, packetSerialNumber2 = 0;
//        in >> packetSerialNumber1 >> packetSerialNumber2;
//        m_packetHandlerBase->removeWaitingForReplyPacket(packetSerialNumber1, packetSerialNumber2);
//        emit signalConfirmationOfReceiptPacketReceived(packetSerialNumber1, packetSerialNumber2);
//        //qDebug()<<"~~ConfirmationOfReceiptPacket--"<<"packetSerialNumber:"<<packetSerialNumber;
//    }
//    break;
    case quint8(IM::ClientLookForServer):
    {
        quint16 peerRUDPListeningPort = 0;
        in >> peerRUDPListeningPort;

        sendServerDeclarePacket(peerAddress, peerRUDPListeningPort);
        //emit signalClientLookForServerPacketReceived(peerAddress, peerPort, peerName);
        //qDebug()<<"~~ClientLookForServer--"<<" peerAddress:"<<peerAddress.toString()<<"   peerPort:"<<peerPort;
    }
    break;

    case quint8(IM::CLIENT_REGISTRATION):
    {
        qWarning()<<"--CLIENT_REQUEST_REGISTRATION";
        
        QString userID = "", password = "", email = "";
        in >> userID >> password >> email;

        IM::ErrorType errorype = IM::ERROR_UnKnownError;
        QString message = "";
        registerNewUser(userID, password, email, &errorype, &message);
        sendClientRegistrationResultPacket(quint8(errorype), message, peerAddress, peerPort);

    }
    break;

    case quint8(IM::CLIENT_REQUEST_UPDATE_PASSWORD):
    {

        QString userID = peerID;
        QByteArray encryptedNewPassword;
        in >> encryptedNewPassword;

        //解密数据
        QByteArray decryptedData;
        if(!decryptData(userID, &decryptedData, encryptedNewPassword)){return;}
        QDataStream stream(&decryptedData, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_7);
        QString newPassword = "";
        stream >> newPassword;

        IM::ErrorType errorype = IM::ERROR_UnKnownError;
        QString message = "";
        updateUserPassword(userID, newPassword, &errorype, &message);
        sendClientUpdatePasswordResultPacket(quint8(errorype), message, peerAddress, peerPort);

    }
    break;

    case quint8(IM::CLIENT_REQUEST_LOGIN):
    {

        QString userID = peerID;

        QString clientVersion = "";
        in >> clientVersion;


        IM::ErrorType errorType = IM::ERROR_UnKnownError;
        if(canUserLogin(userID, clientVersion, &errorType)){
            QHostAddress loginServerAddress = QHostAddress::Null;
            quint16 loginServerPort = 0;
            getUserLoginServer(userID, &loginServerAddress, &loginServerPort);
            sendClientCanLoginPacket(userID, loginServerAddress, loginServerPort, peerAddress, peerPort);
        }else{
            sendClientCanNotLoginPacket(userID, quint8(errorType), peerAddress, peerPort);
        }

        //        emit signalUserRequestLoginPacketReceived(userID, clientVersion);
        //qWarning()<<"~~CLIENT_REQUEST_LOGIN--"<<" peerAddress:"<<clientAddress.toString()<<"   peerPort:"<<clientPort;

    }
    break;

    case quint8(IM::CLIENT_LOGIN_INFO):
    {

        qDebug()<<"--CLIENT_LOGIN_INFO";

        QString userID = peerID;
        QByteArray encryptedPassword;
        quint8 onlineStateCode = quint8(IM::ONLINESTATE_ONLINE);
        in >> encryptedPassword >> onlineStateCode;
        //        qDebug()<<"----onlineStateCode:"<<onlineStateCode;

        IM::ErrorType errorType = IM::ERROR_UnKnownError;
        UserInfo *userInfo = logUserIn(userID, encryptedPassword, IM::OnlineState(onlineStateCode), &errorType);
        if(userInfo){
            userInfo->setLastLoginHostAddress(peerAddress.toString());
            userInfo->setLastLoginHostPort(peerPort);
            QByteArray sessionEncryptionKey = userInfo->getSessionEncryptionKey();
            sendClientLoginSucceededPacket(userID, userInfo->encryptedPassword(), sessionEncryptionKey,
                                           userInfo->getPersonalInfoVersion(), userInfo->getPersonalContactGroupsVersion(),
                                           userInfo->getInterestGroupInfoVersion(), userInfo->getBlacklistInfoVersion(),
                                           peerAddress, peerPort);
            processUserOnlineStatusChanged(userInfo, onlineStateCode, peerAddress.toString(), peerPort);
            sendContactsOnlineInfo(userInfo);

            QStringList messagesCachedOnServer = cachedChatMessagesForIMUser(userInfo);
            if(!messagesCachedOnServer.isEmpty()){
                sendCachedChatMessagesPacket(messagesCachedOnServer, sessionEncryptionKey, peerAddress, peerPort);
            }

            QList<QStringList> sentApplicationList, receivedApplicationList;
            getFriendshipApplyRequest(userID, &sentApplicationList, &receivedApplicationList);
            if(!sentApplicationList.isEmpty()){
                foreach(QStringList infoList, sentApplicationList){
                    QString receiverID = infoList.at(0);
                    UserInfo *receiver = getUserInfo(receiverID);
                    if(!receiver){continue;}
                    UserInfo::FriendshipApplyResult faResult = UserInfo::FriendshipApplyResult(infoList.at(1).toUInt());
                    QString message = infoList.at(2);
                    if(faResult == UserInfo::FAR_ACCEPTED ){
                        sendAddContactResultPacket(receiverID, receiver->getNickName(), receiver->getFace(), IM::ERROR_NoError, message, sessionEncryptionKey, peerAddress.toString(), userInfo->getLastLoginHostPort() );
                    }else{
                        sendAddContactResultPacket(receiverID, receiver->getNickName(), receiver->getFace(), IM::ERROR_RequestDenied, message, sessionEncryptionKey, peerAddress.toString(), userInfo->getLastLoginHostPort() );
                    }
                }
            }

            if(!receivedApplicationList.isEmpty()){
                foreach(QStringList infoList, receivedApplicationList){
                    QString senderID = infoList.at(0);
                    UserInfo *sender = getUserInfo(senderID);
                    if(!sender){continue;}
                    UserInfo::FriendshipApplyResult faResult = UserInfo::FriendshipApplyResult(infoList.at(1).toUInt());
                    QString message = infoList.at(2);
                    if(faResult == UserInfo::FAR_UNKNOWN ){
                        sendAddContactRequestFromUserPacket(senderID, sender->getNickName(), sender->getFace(), message, sessionEncryptionKey, peerAddress, peerPort );
                    }else if(faResult == UserInfo::FAR_ACCEPTED ){
                        sendAddContactResultPacket(senderID, sender->getNickName(), sender->getFace(), IM::ERROR_NoError, message, sessionEncryptionKey, peerAddress.toString(), peerPort);
                    }/*else{
                        sendAddContactResultPacket(senderID, sender->getNickName(), sender->getFace(), IM::ERROR_RequestDenied, message, sessionEncryptionKey, clientAddress.toString(), clientPort );
                    }*/

                }
            }

            QStringList interestgroupChatMessagesCachedOnServer = cachedInterestGroupChatMessagesForIMUser(userInfo);
            if(!interestgroupChatMessagesCachedOnServer.isEmpty()){
                sendCachedInterestGroupChatMessagesPacket(interestgroupChatMessagesCachedOnServer, sessionEncryptionKey, peerAddress, peerPort);
            }

            qWarning()<<QString("User %1 logged in!").arg(userID);
        }else{
            sendClientLoginFailedPacket(userID, quint8(errorType), peerAddress, peerPort);
            qWarning()<<QString("User %1 Login Failed!").arg(userID);

        }

        //        emit signalUserLoginInfoPacketReceived(userID, encryptedPassword, IM::OnlineState(onlineStateCode));
        //        qWarning()<<"~~CLIENT_LOGIN_INFO--"<<" peerAddress:"<<clientAddress.toString()<<"   peerPort:"<<clientPort;

    }
    break;

    case quint8(IM::ONLINE_STATE_CHANGED):
    {


        QString userID = peerID;
        QByteArray encryptedOnlineState;
        in >> encryptedOnlineState;

        //解密数据
        QByteArray decryptedData;
        if(!decryptData(userID, &decryptedData, encryptedOnlineState)){return;}
        QDataStream stream(&decryptedData, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_7);
        quint8 onlineStateCode = quint8(IM::ONLINESTATE_ONLINE);
        stream >> onlineStateCode;

        processUserOnlineStatusChanged(userID, IM::OnlineState(onlineStateCode), peerAddress.toString(), peerPort);

        //        emit signalUserOnlineStateChanged(userID, encryptedOnlineState);

    }
    break;

    case quint8(IM::CONTACT_GROUPS_INFO):
    {


        QString userID =peerID;
        quint8 uploadToServer = 0;
        in >> uploadToServer;

        UserInfo *userInfo = getOnlineUserInfo(userID);
        if(!userInfo){return;}
        if(uploadToServer){
            QByteArray encryptedContactsGroupInfo;
            in >> encryptedContactsGroupInfo;

            //解密数据
            QByteArray decryptedData;
            if(!decryptData(userID, &decryptedData, encryptedContactsGroupInfo)){return;}
            QDataStream stream(&decryptedData, QIODevice::ReadOnly);
            stream.setVersion(QDataStream::Qt_4_7);
            quint32 personalContactGroupsVersion = 0;
            QString contactsGroupInfo = "";
            stream >> personalContactGroupsVersion >> contactsGroupInfo;
            if(personalContactGroupsVersion > userInfo->getPersonalContactGroupsVersion()){
                userInfo->setPersonalContactGroupsVersion(personalContactGroupsVersion);
                userInfo->setContactGroupsInfoString(contactsGroupInfo);
            }

        }else{
            sendPersonalContactGroupsInfoPacket(userInfo->getContactGroupsInfoString(), userInfo->getPersonalContactGroupsVersion(), userInfo->getSessionEncryptionKey(), peerAddress, peerPort);
        }
        qWarning()<<"--CONTACT_GROUPS_INFO";

    }
    break;

    case quint8(IM::CLIENT_REQUEST_USER_SUMMARY_INFO):
    {


        QString userID = peerID;
        QByteArray encryptedUserID;
        in >> encryptedUserID;
        //        qWarning()<<"userID:"<<userID << encryptedUserID.size();
        //解密数据
        QByteArray decryptedData;
        if(!decryptData(userID, &decryptedData, encryptedUserID)){return;}
        
        //QBuffer buf(&decryptedData);
        //buf.open(QIODevice::ReadOnly);
        //in.setDevice(&buf);
        //in.device()->seek(0);
        //QString contactID = "";
        //in >> contactID;
        
        QDataStream stream(&decryptedData, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_7);
        QString contactID = "";
        stream >> contactID;
        //        qWarning()<<"contactID:"<<contactID;

        QString userSummaryInfo = "";
        UserInfo *userInfo = getUserInfo(userID);
        if(!userInfo){return;}
        if(userID == contactID){
            userSummaryInfo = userInfo->getPersonalSummaryInfo();
        }else{
            UserInfo *contactInfo = getUserInfo(contactID);
            if(!contactInfo){return;}
            userSummaryInfo = contactInfo->getPersonalSummaryInfo();
        }
        sendUserInfoPacket(contactID, userSummaryInfo, userInfo->getSessionEncryptionKey(), peerAddress, peerPort);

        //        emit signalUserRequestUserInfo(userID, encryptedUserID);
        qWarning()<<"--CLIENT_REQUEST_USER_SUMMARY_INFO";
    }
    break;



    case quint8(IM::CLIENT_REQUEST_SEARCH_CONTACTS):
    {


        QString userID = peerID;
        QByteArray encryptedData;
        in >> encryptedData;
        
        UserInfo *userInfo = getUserInfo(userID);
        if(!userInfo){return;}

        //解密数据
        QByteArray decryptedData;
        if(!decryptData(userID, &decryptedData, encryptedData)){return;}
        QDataStream stream(&decryptedData, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_7);
        QString propertiesString = ""; 
        bool matchExactly = true, searchOnlineUsersOnly = true;
        stream >> propertiesString >> matchExactly >> searchOnlineUsersOnly;
        
        QStringList usersList = searchContact(propertiesString, matchExactly, searchOnlineUsersOnly);
        if(!usersList.isEmpty()){
            sendSearchResultPacket(usersList, userInfo->getSessionEncryptionKey(), peerAddress, peerPort);
        }
        
        qWarning()<<"--CLIENT_REQUEST_SEARCH_CONTACTS";
    }
    break;

    case quint8(IM::CLIENT_REQUEST_ADD_CONTACT):
    {
        qWarning()<<"--CLIENT_REQUEST_ADD_CONTACT";
        

        QString userID = peerID;
        QByteArray encryptedData;
        in >> encryptedData;
        
        UserInfo *userInfo = getUserInfo(userID);
        if(!userInfo){return;}

        //解密数据
        QByteArray decryptedData;
        if(!decryptData(userID, &decryptedData, encryptedData)){return;}
        QDataStream stream(&decryptedData, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_7);
        QString contactID = "", verificationMessage = ""; 
        stream >> contactID >> verificationMessage;
        
        if(userID == contactID){return;}
        
        UserInfo *contactInfo = getUserInfo(contactID);
        if(!contactInfo){return;}
        
        if(userInfo->hasContact(contactID)){
            //sendAddContactResultPacket(contactID, IM::ERROR_RequestDenied, "Contact already exists!", userInfo->getSessionEncryptionKey(), clientAddress, clientPort);
            //TODO:Send Error Message
            qWarning()<<QString("'%1' already has contact '%2' !").arg(userID).arg(contactID);
            return;
        }
        
        if(contactInfo->getFriendshipApply() == UserInfo::FA_AUTO_ACCEPT){
            userInfo->addOrDeleteContact(contactID, userInfo->getDefaultGroupName(), true);
            sendAddContactResultPacket(contactID, contactInfo->getNickName(), contactInfo->getFace(), IM::ERROR_NoError, "", userInfo->getSessionEncryptionKey(), userInfo->getLastLoginHostAddress(), userInfo->getLastLoginHostPort());

            contactInfo->addOrDeleteContact(userID, contactInfo->getDefaultGroupName(), true);
            if(contactInfo->getOnlineState() != IM::ONLINESTATE_OFFLINE){
                sendAddContactResultPacket(userID, userInfo->getNickName(), userInfo->getFace(), IM::ERROR_NoError, "", contactInfo->getSessionEncryptionKey(), contactInfo->getLastLoginHostAddress(), contactInfo->getLastLoginHostPort());
            }else{
                //TODO:保存消息到数据库
                saveFriendshipApplyRequest(userID, contactID, verificationMessage, UserInfo::FAR_ACCEPTED, true);
            }
        }else{
            if(contactInfo->getOnlineState() == IM::ONLINESTATE_OFFLINE){
                //TODO:保存请求到数据库
                saveFriendshipApplyRequest(userID, contactID, verificationMessage);
            }else{
                sendAddContactRequestFromUserPacket(userID, userInfo->getNickName(), userInfo->getFace(), verificationMessage, userInfo->getSessionEncryptionKey(), peerAddress, peerPort);
            }
        }
        

    }
    break;

    case quint8(IM::CLIENT_REQUEST_MOVE_CONTACT):
    {


        QString userID = peerID;
        QByteArray encryptedData;
        in >> encryptedData;
        
        UserInfo *userInfo = getUserInfo(userID);
        if(!userInfo){return;}

        //解密数据
        QByteArray decryptedData;
        if(!decryptData(userID, &decryptedData, encryptedData)){return;}
        QDataStream stream(&decryptedData, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_7);
        QString contactID = "", oldGroupName = "", newGroupName = ""; 
        stream >> contactID >> oldGroupName >> newGroupName;
        
        if(userID == contactID){return;}
        
        UserInfo *contactInfo = getUserInfo(contactID);
        if(!contactInfo){return;}
        if(!userInfo->hasContact(contactID)){return;}
        
        userInfo->moveContact(contactID, oldGroupName, newGroupName);
        
        qWarning()<<"--CLIENT_REQUEST_MOVE_CONTACT";
    }
    break;

    case quint8(IM::CLIENT_RESPONSE_ADD_CONTACT_REQUEST):
    {


        QString userID = peerID;
        QByteArray encryptedData;
        in >> encryptedData;
        
        UserInfo *userInfo = getUserInfo(userID);
        if(!userInfo){return;}

        //解密数据
        QByteArray decryptedData;
        if(!decryptData(userID, &decryptedData, encryptedData)){return;}
        QDataStream stream(&decryptedData, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_7);
        QString contactID = "", extraMessage = ""; 
        bool acceptRequest = true;
        stream >> contactID >> acceptRequest >> extraMessage;
        
        UserInfo *contactInfo = getUserInfo(contactID);
        if(!contactInfo){return;}
        
        //TODO:
        if(acceptRequest){
            contactInfo->addOrDeleteContact(userID, "", true);

            if(contactInfo->getOnlineState() == IM::ONLINESTATE_OFFLINE){
                //TODO:保存请求到数据库
                saveFriendshipApplyRequest(contactID, userID, extraMessage, UserInfo::FAR_ACCEPTED, false, true);
                return;
            }else{
                sendAddContactResultPacket(userID, userInfo->getNickName(), userInfo->getFace(), IM::ERROR_NoError, "", contactInfo->getSessionEncryptionKey(), contactInfo->getLastLoginHostAddress(), contactInfo->getLastLoginHostPort());
            }

        }else{
            if(!extraMessage.trimmed().isEmpty()){
                if(contactInfo->getOnlineState() == IM::ONLINESTATE_OFFLINE){
                    //TODO:保存请求到数据库
                    saveFriendshipApplyRequest(contactID, userID, extraMessage, UserInfo::FAR_DENIED, false, true);
                    return;
                }else{
                    sendAddContactResultPacket(userID, userInfo->getNickName(), userInfo->getFace(), IM::ERROR_RequestDenied, extraMessage, contactInfo->getSessionEncryptionKey(), contactInfo->getLastLoginHostAddress(), contactInfo->getLastLoginHostPort());
                }
            }
            

        }
        //TODO:从数据库删除请求
        deleteFriendshipApplyRequest(contactID, userID);
        
        qWarning()<<"--CLIENT_RESPONSE_ADD_CONTACT_REQUEST";
    }
    break;

    case quint8(IM::CLIENT_REQUEST_DELETE_CONTACT):
    {


        QString userID = peerID;
        QByteArray encryptedData;
        in >> encryptedData;

        UserInfo *userInfo = getUserInfo(userID);
        if(!userInfo){return;}

        //解密数据
        QByteArray decryptedData;
        if(!decryptData(userID, &decryptedData, encryptedData)){return;}
        QDataStream stream(&decryptedData, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_7);
        QString contactID = "", groupName = "";
        bool deleteMeFromOpposition = false;
        stream >> contactID >> groupName >> deleteMeFromOpposition;

        userInfo->addOrDeleteContact(contactID, groupName, false);
        if(deleteMeFromOpposition){
            //TODO:
            UserInfo *contactInfo = getUserInfo(contactID);
            if(!contactInfo){return;}
            contactInfo->addOrDeleteContact(userID, "", false);

            deleteFriendshipApplyRequest(contactID, userID);
        }
        //TODO:从数据库删除请求
        deleteFriendshipApplyRequest(userID, contactID);

        //qDebug()<<"------------contactID:"<<contactID<<" groupName:"<<groupName;

        qWarning()<<"--CLIENT_REQUEST_DELETE_CONTACT";
    }
    break;

    case quint8(IM::CLIENT_REQUEST_ADD_OR_DELETE_BLACKLISTED_CONTACT):
    {


        QString userID = peerID;
        QByteArray encryptedData;
        in >> encryptedData;

        UserInfo *userInfo = getUserInfo(userID);
        if(!userInfo){return;}

        //解密数据
        QByteArray decryptedData;
        if(!decryptData(userID, &decryptedData, encryptedData)){return;}
        QDataStream stream(&decryptedData, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_7);
        QString contactID = "";
        bool addToBlacklist = true;
        stream >> contactID >> addToBlacklist;

        userInfo->addOrDeleteBlacklistedContact(contactID, addToBlacklist);

        if(addToBlacklist){
            //TODO:
            UserInfo *contactInfo = getUserInfo(contactID);
            if(!contactInfo){return;}
            contactInfo->addOrDeleteContact(userID, "", false);

            //TODO:从数据库删除请求
            deleteFriendshipApplyRequest(userID, contactID);
            deleteFriendshipApplyRequest(contactID, userID);
        }

        qWarning()<<"--CLIENT_REQUEST_ADD_OR_DELETE_BLACKLISTED_CONTACT";
    }
    break;

    case quint8(IM::CLIENT_REQUEST_INTEREST_GROUPS_LIST):
    {


        QString userID = peerID;
        //in >> userID ;

        UserInfo *userInfo = getUserInfo(userID);
        if(!userInfo){return;}
        sendUserInterestGroupsListPacket(userInfo, peerAddress, peerPort);

        qWarning()<<"--CLIENT_REQUEST_INTEREST_GROUPS_LIST";
    }
    break;

    case quint8(IM::CLIENT_REQUEST_INTEREST_GROUP_INFO):
    {


        QString userID = peerID;
        QByteArray encryptedData;
        in >> encryptedData;

        UserInfo *userInfo = getUserInfo(userID);
        if(!userInfo){return;}

        //解密数据
        QByteArray decryptedData;
        if(!decryptData(userID, &decryptedData, encryptedData)){return;}
        QDataStream stream(&decryptedData, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_7);
        quint32 groupID = 0;
        stream >> groupID;

        sendUserInterestGroupInfoPacket(userInfo, groupID, peerAddress, peerPort);

        qWarning()<<"--CLIENT_REQUEST_INTEREST_GROUP_INFO";
    }
    break;

    case quint8(IM::CLIENT_REQUEST_INTEREST_GROUP_MEMBERS_INFO):
    {



        QString userID = peerID;
        QByteArray encryptedData;
        in >> encryptedData;

        UserInfo *userInfo = getUserInfo(userID);
        if(!userInfo){return;}

        //解密数据
        QByteArray decryptedData;
        if(!decryptData(userID, &decryptedData, encryptedData)){return;}
        QDataStream stream(&decryptedData, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_7);
        quint32 groupID = 0;
        stream >> groupID;

        sendUserInterestGroupMembersInfoPacket(userInfo, groupID, peerAddress, peerPort);

        qWarning()<<"--CLIENT_REQUEST_INTEREST_GROUP_MEMBERS_INFO";
    }
    break;


    case quint8(IM::BLACKLIST_INFO):
    {


        QString userID = peerID;
        //in >> userID ;

        UserInfo *userInfo = getUserInfo(userID);
        if(!userInfo){return;}
        sendUserBlacklistInfoPacket(userInfo, peerAddress, peerPort);

        qWarning()<<"--BLACKLIST_INFO";
    }
    break;

    case quint8(IM::CLIENT_REQUEST_MODIFY_CONTACT_REMARK):
    {


        QString userID = peerID;
        QByteArray encryptedData;
        in >> encryptedData;

        UserInfo *userInfo = getUserInfo(userID);
        if(!userInfo){return;}

        //解密数据
        QByteArray decryptedData;
        if(!decryptData(userID, &decryptedData, encryptedData)){return;}
        QDataStream stream(&decryptedData, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_7);
        QString contactID = "", remarkName = "", extraRemarkInfo = "";
        stream >> contactID >> remarkName >> extraRemarkInfo;

        //TODO:Save To DB!

    }
    break;

    case quint8(IM::RENAME_CONTACT_GROUP):
    {


        QString userID = peerID;
        QByteArray encryptedData;
        in >> encryptedData;

        UserInfo *userInfo = getUserInfo(userID);
        if(!userInfo){return;}

        //解密数据
        QByteArray decryptedData;
        if(!decryptData(userID, &decryptedData, encryptedData)){return;}
        QDataStream stream(&decryptedData, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_7);
        QString oldGroupName = "", newGroupName = "";
        stream >> oldGroupName >> newGroupName;

        userInfo->updateGroupName(oldGroupName, newGroupName);

        qWarning()<<"--RENAME_CONTACT_GROUP";
    }
    break;

    case quint8(IM::CREATE_OR_DELETE_CONTACT_GROUP):
    {
        qWarning()<<"--CREATE_OR_DELETE_CONTACT_GROUP";



        QString userID = peerID;
        QByteArray encryptedData;
        in >> encryptedData;

        UserInfo *userInfo = getUserInfo(userID);
        if(!userInfo){return;}

        //解密数据
        QByteArray decryptedData;
        if(!decryptData(userID, &decryptedData, encryptedData)){return;}
        QDataStream stream(&decryptedData, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_7);
        QString groupName = "";
        bool create = true;
        stream >> groupName >> create;

        userInfo->createOrDeleteGroup(groupName, create);

    }
    break;

    case quint8(IM::SESSION_ENCRYPTION_KEY_WITH_CONTACT):
    {
        qWarning()<<"--SESSION_ENCRYPTION_KEY_WITH_CONTACT";


        QString userID = peerID;
        QByteArray encryptedData;
        in >> encryptedData;

        UserInfo *userInfo = getUserInfo(userID);
        if(!userInfo){return;}

        //解密数据
        QByteArray decryptedData;
        if(!decryptData(userID, &decryptedData, encryptedData)){return;}
        QDataStream stream(&decryptedData, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_7);
        QString contactID = "";
        stream >> contactID;

        UserInfo *contactInfo = getUserInfo(contactID);
        if(!contactInfo){return;}
        if(contactInfo->isOnLine()){
            QByteArray key = ServerUtilities::generateSessionEncryptionKey();
            sendSessionEncryptionKeyWithContact(userID, contactInfo->getSessionEncryptionKey(), key, contactInfo->getLastLoginHostAddress(), contactInfo->getLastLoginHostPort());
            sendSessionEncryptionKeyWithContact(contactID, userInfo->getSessionEncryptionKey(), key, peerAddress.toString(), peerPort);
        }else{
            sendContactOnlineStatusChangedPacket(contactID, quint8(IM::ONLINESTATE_OFFLINE), userInfo->getSessionEncryptionKey(), contactInfo->getLastLoginHostAddress(), contactInfo->getLastLoginHostPort(), peerAddress.toString(), peerPort);

        }


    }
    break;

    case quint8(IM::CHAT_MESSAGES_CACHED_ON_SERVER):
    {
        qWarning()<<"--CHAT_MESSAGES_CACHED_ON_SERVER";



        QString userID = peerID;
        QByteArray encryptedData;
        in >> encryptedData;

        UserInfo *userInfo = getUserInfo(userID);
        if(!userInfo){return;}

        //解密数据
        QByteArray decryptedData;
        if(!decryptData(userID, &decryptedData, encryptedData)){return;}
        QDataStream stream(&decryptedData, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_7);
        QString contactID = "", message = "";
        stream >> contactID >> message;

        saveCachedChatMessageFromIMUser(userID, contactID, message);

    }
    break;

    case quint8(IM::GROUP_CHAT_MESSAGE):
    {
        qWarning()<<"--GROUP_CHAT_MESSAGE";



        QString userID = peerID;
        QByteArray encryptedData;
        in >> encryptedData;

        UserInfo *userInfo = getUserInfo(userID);
        if(!userInfo){return;}

        //解密数据
        QByteArray decryptedData;
        if(!decryptData(userID, &decryptedData, encryptedData)){return;}
        QDataStream stream(&decryptedData, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_7);
        quint32 interestGroupID = 0;
        QString  message = "";
        stream >> interestGroupID >> message;

        if(userInfo->getInterestGroups().contains(QString::number(interestGroupID))){
            saveCachedInterestGroupChatMessageFromIMUser(userID, interestGroupID, message);
        }else{
            qCritical()<<QString("User '%1' is not a member of group '%2'!").arg(userID).arg(interestGroupID);
        }


    }
    break;



    //    case quint8(IM::):
    //        {
    //        sendConfirmationOfReceiptPacket(packet->getPeerHostAddress(), packet->getPeerHostPort(), packet->getPacketSerialNumber());


    //        }
    //        break;
    //    case quint8(IM::):
    //        {
    //        sendConfirmationOfReceiptPacket(packet->getPeerHostAddress(), packet->getPeerHostPort(), packet->getPacketSerialNumber());


    //        }
    //        break;

    //    case quint8(IM::CLIENT_REQUEST_LOGIN):
    //        {
    //
    //        }
    //        break;




    default:
        qWarning()<<"Unknown Packet Type:"<<packetType                
                <<" From:"<<peerAddress.toString()
               <<" Port:"<<peerPort;
        break;

    }



}




void ServerPacketsParser::startHeartbeat(int interval){
    if(NULL == heartbeatTimer){
        heartbeatTimer = new QTimer();
        heartbeatTimer->setSingleShot(false);
        heartbeatTimer->setInterval(interval);
        connect(heartbeatTimer, SIGNAL(timeout()), this, SLOT(heartbeat()));

    }else{
        heartbeatTimer->stop();
        heartbeatTimer->setInterval(interval);

    }

    heartbeatTimer->start();


}

void ServerPacketsParser::stopHeartbeat(){

    heartbeatTimer->stop();
}

int ServerPacketsParser::crypto(QByteArray *destination, const QByteArray &source, const QByteArray &key, bool encrypt){
    return cryptography->teaCrypto(destination, source, key, encrypt);
}

bool ServerPacketsParser::encrypeData(const QString &userID, QByteArray *destination, const QByteArray &source){

    UserInfo *info = getUserInfo(userID);
    if(!info){
        return false;
    }
    return cryptography->teaCrypto(destination, source, info->getSessionEncryptionKey(), true);

}

bool ServerPacketsParser::decryptData(const QString &userID, QByteArray *destination, const QByteArray &source){

    UserInfo *info = getUserInfo(userID);
    if(!info){
        return false;
    }
    //TODO 验证数据是否解密成功  //qCritical()<<"Data Decryption Failed!";
    return cryptography->teaCrypto(destination, source, info->getSessionEncryptionKey(), false);

}

void ServerPacketsParser::startCheckIMUsersOnlineStateTimer(){
    qWarning()<<"--startCheckIMUsersOnlineStateTimer()";
    
    if(!checkIMUsersOnlineStateTimer){
        checkIMUsersOnlineStateTimer = new QTimer(this);
        checkIMUsersOnlineStateTimer->setInterval(HEARTBEAT_TIMER_INTERVAL); //1 minute
        checkIMUsersOnlineStateTimer->setSingleShot(false);
        connect(checkIMUsersOnlineStateTimer, SIGNAL(timeout()), this, SLOT(slotCheckIMUsersOnlineStatus()));
    }

    //    if(!checkIMUsersOnlineStateTimer->isActive()){
    //    }
    
    checkIMUsersOnlineStateTimer->start();

}

void ServerPacketsParser::slotCheckIMUsersOnlineStatus(){
    qWarning()<<"--slotCheckIMUsersOnlineStatus()";
    QList<UserInfo*>  users = onlineUsers();
    foreach(UserInfo *info, users){
        if(!info->isOnLine()){
            processUserOnlineStatusChanged(info, IM::ONLINESTATE_OFFLINE, "", 0);
        }
    }

}


quint16 ServerPacketsParser::getLastReceivedPacketSN(const QString &peerID){
    quint16 lastpacketSN = 0;

    QList< QPair<quint16 /*Packet Serial Number*/, QDateTime/*Received Time*/> > list = m_receivedPacketsHash.values(peerID);
    if(list.isEmpty()){
        return lastpacketSN;
    }

    QDateTime lastpacketTime(QDate(1970, 1, 1));
    for(int i=0; i<list.size(); i++){
        QPair<quint16, QDateTime> pair = list.at(i);
        QDateTime time = pair.second;
        if(time.addSecs(UDP_PACKET_WAITING_FOR_REPLY_TIMEOUT) < QDateTime::currentDateTime()){
            m_receivedPacketsHash.remove(peerID, pair);
        }else{
            if(time > lastpacketTime){
                lastpacketTime = time;
                lastpacketSN = pair.first;
            }
        }
    }
    //    foreach ( QPair<quint16, QDateTime> pair, list) {
    //        QDateTime time = pair.second;
    //        if(time.addSecs(UDP_PACKET_WAITING_FOR_REPLY_TIMEOUT) < QDateTime::currentDateTime()){
    //            m_receivedPacketsHash.remove(peerID, pair);
    //            list.removeOne(pair);
    //        }else{
    //            if(time > lastpacketTime){
    //                lastpacketTime = time;
    //                lastpacketSN = pair.first;
    //            }
    //        }
    //    }

    //TODO:TX Rate

    return lastpacketSN;


}





















} //namespace HEHUI
