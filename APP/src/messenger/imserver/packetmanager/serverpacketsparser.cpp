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



namespace HEHUI {


ServerPacketsParser::ServerPacketsParser(ResourcesManagerInstance *resourcesManager, QObject *parent)
    :QObject(parent), UsersManager(parent)
{

    Q_ASSERT(resourcesManager);

    m_ipmcServer = resourcesManager->getIPMCServer();
    Q_ASSERT_X(m_ipmcServer, "ClientPacketsParser::ClientPacketsParser(...)", "Invalid IP MC Server!");
    connect(m_ipmcServer, SIGNAL(signalNewUDPPacketReceived(Packet*)), this, SLOT(parseIncomingPacketData(Packet*)), Qt::QueuedConnection);


    m_rtp = resourcesManager->getRTP();
    Q_ASSERT(m_rtp);
    connect(m_rtp, SIGNAL(disconnected(int)), this, SLOT(peerDisconnected(int)));

    m_udtProtocol = m_rtp->getUDTProtocol();
    Q_ASSERT(m_udtProtocol);
    connect(m_udtProtocol, SIGNAL(packetReceived(Packet*)), this, SLOT(parseIncomingPacketData(Packet*)), Qt::QueuedConnection);
    m_udtProtocol->startWaitingForIOInOneThread(1);
    //m_udtProtocol->startWaitingForIOInSeparateThread(100, 1000);

    m_tcpServer = m_rtp->getTCPServer();
    Q_ASSERT(m_tcpServer);
    connect(m_tcpServer, SIGNAL(packetReceived(Packet*)), this, SLOT(parseIncomingPacketData(Packet*)), Qt::QueuedConnection);





    //    heartbeatTimer = 0;


    //m_serverName = networkManager->hostName();
    m_serverName = QHostInfo::localHostName().toLower();
    m_localRTPListeningPort = m_rtp->getTCPServerPort();



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

    //    if(heartbeatTimer){
    //        heartbeatTimer->stop();
    //    }
    //    delete heartbeatTimer;
    //    heartbeatTimer = 0;


    delete cryptography;
    cryptography = 0;

}



void ServerPacketsParser::parseIncomingPacketData(Packet *packet){

    //    qDebug()<<"----ServerPacketsParser::parseIncomingPacketData(Packet *packet)";


    QByteArray packetData = packet->getPacketData();
    QDataStream in(&packetData, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_8);

    QString peerID = "";
    in >> peerID;

    QHostAddress peerAddress = packet->getPeerHostAddress();
    quint16 peerPort = packet->getPeerHostPort();
    quint8 packetType = packet->getPacketType();
    qDebug()<<"--ServerPacketsParser::parseIncomingPacketData(...) "<<" peerID:"<<peerID<<" peerAddress:"<<peerAddress.toString()<<" peerPort:"<<peerPort<<" packetType:"<<packetType;
    int socketID = packet->getSocketID();

    PacketHandlerBase::recylePacket(packet);
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
        qDebug()<<"~~ClientLookForServer";

        QString clientVersion = "";
        in >> clientVersion;
        //TODO:Version

        sendServerDeclarePacket(peerAddress, peerPort);
        //emit signalClientLookForServerPacketReceived(peerAddress, peerPort, peerName);
        //qDebug()<<"~~ClientLookForServer--"<<" peerAddress:"<<peerAddress.toString()<<"   peerPort:"<<peerPort;
    }
        break;

    case quint8(IM::CLIENT_REQUEST_REGISTRATION_SERVER_INFO):
    {
        qDebug()<<"~~CLIENT_REQUEST_REGISTRATION_SERVER_INFO";

        QString clientVersion = "";
        in >> clientVersion;


        //TODO
        IM::ErrorType errorype = IM::ERROR_NoError;
        bool canRegister = true;
        QString extraMessage = "TEST";
        IM::RegistrationMode regMode = IM::RM_UserDefineAll;
        QString regServerAddress = "";
        bool requireActivation = false;

        sendRegistrationServerInfoPacket(socketID, quint8(errorype), extraMessage, canRegister, regMode, regServerAddress, requireActivation);

    }
        break;

    case quint8(IM::CLIENT_REGISTRATION):
    {
        qDebug()<<"~~CLIENT_REGISTRATION";
        
        QString userID = "", password = "";
        in >> userID >> password;

        IM::ErrorType errorype = IM::ERROR_UnKnownError;
        QString message = "";
        quint32 sysID = 0;
        registerNewUser(userID, password, &errorype, &sysID, &message);
        sendClientRegistrationResultPacket(socketID, quint8(errorype), sysID, message);

    }
        break;

    case quint8(IM::CLIENT_REQUEST_UPDATE_PASSWORD):
    {

        qDebug()<<"~~CLIENT_REQUEST_UPDATE_PASSWORD";

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
        sendClientUpdatePasswordResultPacket(socketID, quint8(errorype), message);

    }
        break;

    case quint8(IM::CLIENT_REQUEST_LOGIN):
    {
        qDebug()<<"~~CLIENT_REQUEST_LOGIN";

        QString userID = peerID;

        QString clientVersion = "";
        in >> clientVersion;


        IM::ErrorType errorType = IM::ERROR_UnKnownError;
        if(canUserLogin(userID, clientVersion, &errorType)){
            QHostAddress loginServerAddress = QHostAddress::Null;
            quint16 loginServerPort = 0;
            getUserLoginServer(userID, &loginServerAddress, &loginServerPort);
            sendClientCanLoginPacket(socketID, userID, loginServerAddress, loginServerPort);
        }else{
            sendClientCanNotLoginPacket(socketID, userID, quint8(errorType));
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
        in >> encryptedPassword >> onlineStateCode ;
        //        qDebug()<<"----onlineStateCode:"<<onlineStateCode;

        IM::ErrorType errorType = IM::ERROR_UnKnownError;
        UserInfo *userInfo = logUserIn(userID, encryptedPassword, IM::OnlineState(onlineStateCode), &errorType);
        if(userInfo){

            QByteArray sessionEncryptionKey = userInfo->getSessionEncryptionKey();
            sendClientLoginSucceededPacket(socketID, userID, userInfo->encryptedPassword(), sessionEncryptionKey, userInfo->getPersonalSummaryInfoVersion(),
                                           userInfo->getPersonalDetailInfoVersion(), userInfo->getPersonalContactGroupsVersion(),
                                           userInfo->getInterestGroupInfoVersion(), userInfo->getBlacklistInfoVersion());

            processUserOnlineStatusChanged(userInfo, onlineStateCode, peerAddress.toString(), peerPort);
            sendContactsOnlineInfo(socketID, userInfo);

            QStringList messagesCachedOnServer = cachedChatMessagesForIMUser(userInfo);
            if(!messagesCachedOnServer.isEmpty()){
                sendCachedChatMessagesPacket(socketID, messagesCachedOnServer, sessionEncryptionKey, peerAddress, peerPort);
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
                        sendAddContactResultPacket(socketID, receiverID, receiver->getNickName(), receiver->getFace(), IM::ERROR_NoError, message, sessionEncryptionKey, peerAddress.toString(), userInfo->getLastLoginExternalHostPort() );
                    }else{
                        sendAddContactResultPacket(socketID, receiverID, receiver->getNickName(), receiver->getFace(), IM::ERROR_RequestDenied, message, sessionEncryptionKey, peerAddress.toString(), userInfo->getLastLoginExternalHostPort() );
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
                        sendAddContactRequestFromUserPacket(socketID, senderID, sender->getNickName(), sender->getFace(), message, sessionEncryptionKey, peerAddress, peerPort );
                    }else if(faResult == UserInfo::FAR_ACCEPTED ){
                        sendAddContactResultPacket(socketID, senderID, sender->getNickName(), sender->getFace(), IM::ERROR_NoError, message, sessionEncryptionKey, peerAddress.toString(), peerPort);
                    }/*else{
                        sendAddContactResultPacket(senderID, sender->getNickName(), sender->getFace(), IM::ERROR_RequestDenied, message, sessionEncryptionKey, clientAddress.toString(), clientPort );
                    }*/

                }
            }

            QStringList interestgroupChatMessagesCachedOnServer = cachedInterestGroupChatMessagesForIMUser(userInfo);
            if(!interestgroupChatMessagesCachedOnServer.isEmpty()){
                sendCachedInterestGroupChatMessagesPacket(socketID, interestgroupChatMessagesCachedOnServer, sessionEncryptionKey, peerAddress, peerPort);
            }

            m_userSocketsHash.insert(socketID, userInfo);

            qWarning()<<QString("User %1 logged in!").arg(userID);
        }else{
            sendClientLoginFailedPacket(socketID, userID, quint8(errorType));
            qWarning()<<QString("User %1 Login Failed!").arg(userID);

        }

        //        emit signalUserLoginInfoPacketReceived(userID, encryptedPassword, IM::OnlineState(onlineStateCode));
        //        qWarning()<<"~~CLIENT_LOGIN_INFO--"<<" peerAddress:"<<clientAddress.toString()<<"   peerPort:"<<clientPort;

    }
        break;

    case quint8(IM::ONLINE_STATE_CHANGED):
    {
        qDebug()<<"~~ONLINE_STATE_CHANGED";

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
        qDebug()<<"~~CONTACT_GROUPS_INFO";

        QString userID =peerID;

        UserInfo *userInfo = getOnlineUserInfo(userID);
        if(!userInfo){return;}

        sendPersonalContactGroupsInfoPacket(socketID, userInfo->getContactGroupsInfoString(), userInfo->getPersonalContactGroupsVersion(), userInfo->getSessionEncryptionKey());
    }
        break;

    case quint8(IM::CLIENT_REQUEST_USER_INFO):
    {
        qDebug()<<"~~CLIENT_REQUEST_USER_INFO";

        QString userID = peerID;
        QByteArray encryptedData;
        in >> encryptedData;
        //        qWarning()<<"userID:"<<userID << encryptedUserID.size();
        //解密数据
        QByteArray decryptedData;
        if(!decryptData(userID, &decryptedData, encryptedData)){return;}
        
        //QBuffer buf(&decryptedData);
        //buf.open(QIODevice::ReadOnly);
        //in.setDevice(&buf);
        //in.device()->seek(0);
        //QString contactID = "";
        //in >> contactID;
        
        QDataStream stream(&decryptedData, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_7);
        QString contactID = "";
        quint8 requestSummaryInfo = 1;
        stream >> contactID >> requestSummaryInfo;
        //        qWarning()<<"contactID:"<<contactID;

        QString userInfoString = "";
        UserInfo *userInfo = getUserInfo(userID);
        if(!userInfo){return;}
        if(userID == contactID){
            userInfoString = userInfo->getPersonalInfoString(requestSummaryInfo);
        }else{
            UserInfo *contactInfo = getUserInfo(contactID);
            if(!contactInfo){return;}
            userInfoString = contactInfo->getPersonalInfoString(requestSummaryInfo);
        }
        sendUserInfoPacket(socketID, contactID, userInfoString, userInfo->getSessionEncryptionKey(), requestSummaryInfo);

        //        emit signalUserRequestUserInfo(userID, encryptedUserID);
    }
        break;



    case quint8(IM::CLIENT_REQUEST_SEARCH_CONTACTS):
    {
        qDebug()<<"~~CLIENT_REQUEST_SEARCH_CONTACTS";

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
            sendSearchResultPacket(socketID, usersList, userInfo->getSessionEncryptionKey(), peerAddress, peerPort);
        }
        
    }
        break;

    case quint8(IM::CLIENT_REQUEST_ADD_CONTACT):
    {
        qDebug()<<"--CLIENT_REQUEST_ADD_CONTACT";
        

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
            sendAddContactResultPacket(socketID, contactID, contactInfo->getNickName(), contactInfo->getFace(), IM::ERROR_NoError, "", userInfo->getSessionEncryptionKey(), userInfo->getLastLoginExternalHostAddress(), userInfo->getLastLoginExternalHostPort());

            contactInfo->addOrDeleteContact(userID, contactInfo->getDefaultGroupName(), true);
            if(contactInfo->getOnlineState() != IM::ONLINESTATE_OFFLINE){
                sendAddContactResultPacket(socketID, userID, userInfo->getNickName(), userInfo->getFace(), IM::ERROR_NoError, "", contactInfo->getSessionEncryptionKey(), contactInfo->getLastLoginExternalHostAddress(), contactInfo->getLastLoginExternalHostPort());
            }else{
                //TODO:保存消息到数据库
                saveFriendshipApplyRequest(userID, contactID, verificationMessage, UserInfo::FAR_ACCEPTED, true);
            }
        }else{
            if(contactInfo->getOnlineState() == IM::ONLINESTATE_OFFLINE){
                //TODO:保存请求到数据库
                saveFriendshipApplyRequest(userID, contactID, verificationMessage);
            }else{
                sendAddContactRequestFromUserPacket(socketID, userID, userInfo->getNickName(), userInfo->getFace(), verificationMessage, userInfo->getSessionEncryptionKey(), peerAddress, peerPort);
            }
        }
        

    }
        break;

    case quint8(IM::CLIENT_REQUEST_MOVE_CONTACT):
    {
        qDebug()<<"~~CLIENT_REQUEST_MOVE_CONTACT";

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
        
    }
        break;

    case quint8(IM::CLIENT_RESPONSE_ADD_CONTACT_REQUEST):
    {
        qDebug()<<"~~CLIENT_RESPONSE_ADD_CONTACT_REQUEST";

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
                sendAddContactResultPacket(socketID, userID, userInfo->getNickName(), userInfo->getFace(), IM::ERROR_NoError, "", contactInfo->getSessionEncryptionKey(), contactInfo->getLastLoginExternalHostAddress(), contactInfo->getLastLoginExternalHostPort());
            }

        }else{
            if(!extraMessage.trimmed().isEmpty()){
                if(contactInfo->getOnlineState() == IM::ONLINESTATE_OFFLINE){
                    //TODO:保存请求到数据库
                    saveFriendshipApplyRequest(contactID, userID, extraMessage, UserInfo::FAR_DENIED, false, true);
                    return;
                }else{
                    sendAddContactResultPacket(socketID, userID, userInfo->getNickName(), userInfo->getFace(), IM::ERROR_RequestDenied, extraMessage, contactInfo->getSessionEncryptionKey(), contactInfo->getLastLoginExternalHostAddress(), contactInfo->getLastLoginExternalHostPort());
                }
            }
            

        }
        //TODO:从数据库删除请求
        deleteFriendshipApplyRequest(contactID, userID);
        
    }
        break;

    case quint8(IM::CLIENT_REQUEST_DELETE_CONTACT):
    {
        qDebug()<<"~~CLIENT_REQUEST_DELETE_CONTACT";

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

    }
        break;

    case quint8(IM::CLIENT_REQUEST_ADD_OR_DELETE_BLACKLISTED_CONTACT):
    {

        qDebug()<<"~~CLIENT_REQUEST_ADD_OR_DELETE_BLACKLISTED_CONTACT";

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

    }
        break;

    case quint8(IM::CLIENT_REQUEST_INTEREST_GROUPS_LIST):
    {
        qDebug()<<"~~CLIENT_REQUEST_INTEREST_GROUPS_LIST";

        QString userID = peerID;
        //in >> userID ;

        UserInfo *userInfo = getUserInfo(userID);
        if(!userInfo){return;}
        sendUserInterestGroupsListPacket(socketID, userInfo, peerAddress, peerPort);

    }
        break;

    case quint8(IM::CLIENT_REQUEST_INTEREST_GROUP_INFO):
    {
        qDebug()<<"~~CLIENT_REQUEST_INTEREST_GROUP_INFO";

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

        sendUserInterestGroupInfoPacket(socketID, userInfo, groupID, peerAddress, peerPort);

    }
        break;

    case quint8(IM::CLIENT_REQUEST_INTEREST_GROUP_MEMBERS_INFO):
    {

        qDebug()<<"~~CLIENT_REQUEST_INTEREST_GROUP_MEMBERS_INFO";

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

        sendUserInterestGroupMembersInfoPacket(socketID, userInfo, groupID, peerAddress, peerPort);

    }
        break;


    case quint8(IM::BLACKLIST_INFO):
    {
        qDebug()<<"~~BLACKLIST_INFO";

        QString userID = peerID;
        //in >> userID ;

        UserInfo *userInfo = getUserInfo(userID);
        if(!userInfo){return;}
        sendUserBlacklistInfoPacket(socketID, userInfo, peerAddress, peerPort);

    }
        break;

    case quint8(IM::CLIENT_REQUEST_MODIFY_CONTACT_REMARK):
    {
        qDebug()<<"~~CLIENT_REQUEST_MODIFY_CONTACT_REMARK";

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
        qDebug()<<"~~RENAME_CONTACT_GROUP";

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
        quint32 groupID = ;
        QString newGroupName = "";
        stream >> groupID >> newGroupName;

        updateUserPersonalContactGroupName(groupID, newGroupName);

    }
        break;

    case quint8(IM::CREATE_OR_DELETE_CONTACT_GROUP):
    {
        qDebug()<<"--CREATE_OR_DELETE_CONTACT_GROUP";



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

        userInfo->createOrDeleteContactGroup(groupName, create);

    }
        break;

    case quint8(IM::SESSION_ENCRYPTION_KEY_WITH_CONTACT):
    {
        qDebug()<<"--SESSION_ENCRYPTION_KEY_WITH_CONTACT";


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
            sendSessionEncryptionKeyWithContact(socketID, userID, contactInfo->getSessionEncryptionKey(), key, contactInfo->getLastLoginExternalHostAddress(), contactInfo->getLastLoginExternalHostPort());
            sendSessionEncryptionKeyWithContact(socketID, contactID, userInfo->getSessionEncryptionKey(), key, peerAddress.toString(), peerPort);
        }else{
            sendContactOnlineStatusChangedPacket(socketID, contactID, quint8(IM::ONLINESTATE_OFFLINE), userInfo->getSessionEncryptionKey(), contactInfo->getLastLoginExternalHostAddress(), contactInfo->getLastLoginExternalHostPort(), peerAddress.toString(), peerPort);

        }


    }
        break;

    case quint8(IM::CHAT_MESSAGES_CACHED_ON_SERVER):
    {
        qDebug()<<"--CHAT_MESSAGES_CACHED_ON_SERVER";



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
        qDebug()<<"--GROUP_CHAT_MESSAGE";



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


void ServerPacketsParser::userExceptionalOffline(const QString &peerAddress, quint16 peerPort){
    qDebug()<<"--userExceptionalOffline(...)"<<" peerAddress:"<<peerAddress<<" peerPort:"<<peerPort;

    QList<UserInfo*>  users = onlineUsers();
    foreach(UserInfo *info, users){
        if(info->getLastLoginExternalHostAddress() == peerAddress && info->getLastLoginExternalHostPort() == peerPort){
            processUserOnlineStatusChanged(info, IM::ONLINESTATE_OFFLINE, peerAddress, peerPort);
        }
    }

}

//void ServerPacketsParser::startHeartbeat(int interval){
//    if(NULL == heartbeatTimer){
//        heartbeatTimer = new QTimer();
//        heartbeatTimer->setSingleShot(false);
//        heartbeatTimer->setInterval(interval);
//        connect(heartbeatTimer, SIGNAL(timeout()), this, SLOT(heartbeat()));

//    }else{
//        heartbeatTimer->stop();
//        heartbeatTimer->setInterval(interval);

//    }

//    heartbeatTimer->start();


//}

//void ServerPacketsParser::stopHeartbeat(){

//    heartbeatTimer->stop();
//}

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

void ServerPacketsParser::peerDisconnected(int socketID){
    //TODO

    if(m_userSocketsHash.contains(socketID)){

        UserInfo *info = m_userSocketsHash.value(socketID);
        if(!info){return;}

        QString peerAddress = info->getLastLoginExternalHostAddress();
        quint16 peerPort = info->getLastLoginExternalHostPort();
        processUserOnlineStatusChanged(info, IM::ONLINESTATE_OFFLINE, peerAddress, peerPort);

        qCritical()<<QString("ERROR! Peer %1:%2 Closed Unexpectedly!").arg(peerAddress).arg(peerPort);

    }

    //m_userSocketsHash.remove(socketID);

}

//void ServerPacketsParser::startCheckIMUsersOnlineStateTimer(){
//    qWarning()<<"--startCheckIMUsersOnlineStateTimer()";

//    if(!checkIMUsersOnlineStateTimer){
//        checkIMUsersOnlineStateTimer = new QTimer(this);
//        checkIMUsersOnlineStateTimer->setInterval(HEARTBEAT_TIMER_INTERVAL); //1 minute
//        checkIMUsersOnlineStateTimer->setSingleShot(false);
//        connect(checkIMUsersOnlineStateTimer, SIGNAL(timeout()), this, SLOT(slotCheckIMUsersOnlineStatus()));
//    }

//    //    if(!checkIMUsersOnlineStateTimer->isActive()){
//    //    }

//    checkIMUsersOnlineStateTimer->start();

//}

//void ServerPacketsParser::slotCheckIMUsersOnlineStatus(){
//    qWarning()<<"--slotCheckIMUsersOnlineStatus()";
//    QList<UserInfo*>  users = onlineUsers();
//    foreach(UserInfo *info, users){
//        if(!info->isOnLine()){
//            processUserOnlineStatusChanged(info, IM::ONLINESTATE_OFFLINE, "", 0);
//        }
//    }

//}






















} //namespace HEHUI
