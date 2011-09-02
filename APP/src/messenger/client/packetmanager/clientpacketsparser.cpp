/*
 ****************************************************************************
 * clientpacketsparser.cpp
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



#include "clientpacketsparser.h"

#ifdef Q_CC_MSVC
#include <windows.h>
//#include "HHSharedWindowsManagement/hwindowsmanagement.h"
#define msleep(x) Sleep(x)
#endif

#ifdef Q_CC_GNU
#include <unistd.h>
#define msleep(x) usleep(x*1000)
#endif


namespace HEHUI {


ClientPacketsParser::ClientPacketsParser(ClientNetworkManager *networkManager, QObject *parent)
    :PacketsParserBase(networkManager, parent)
{

    Q_ASSERT_X(networkManager, "ClientPacketsParser::ClientPacketsParser(...)", "Invalid ClientNetworkManager!");

    m_packetHandlerBase = networkManager->getPacketHandler();
    Q_ASSERT_X(m_packetHandlerBase, "ClientPacketsParser::ClientPacketsParser(...)", "Invalid PacketHandlerBase!");


    m_serverAddress = QHostAddress::Null;
    m_serverTCPListeningPort = 0;
    m_serverUDPListeningPort = quint16(IM_SERVER_UDP_LISTENING_PORT);
    serverName = "";

    clientLookForServerPacketSerialNumber = 0;

    heartbeatTimer = 0;
    //    processWaitingForReplyPacketsTimer = 0;

    //packetHandlerBase = new PacketHandlerBase(this);
    //m_packetHandlerBase = packetHandlerBase;


    //    m_networkManager = ClientNetworkManager::instance();
    //    connect(networkManager, SIGNAL(signalNewPacketReceived(Packet *)), this, SLOT(slotNewPacketReceived(Packet *)));
    //    processingPackets = false;

    //    localIPMCListeningAddress = NetworkManager::instance()->localIPMCListeningAddress();
    //    localIPMCListeningPort = NetworkManager::instance()->localIPMCListeningPort();

    
    localUDPListeningAddress = QHostAddress::Any;
    localUDPListeningPort = 0;


    serverLastOnlineTime = QDateTime();

    user = IMUser::instance();
    m_myUserID = user->getUserID();

    cryptography = new Cryptography();
    sessionEncryptionKey = QByteArray();
    
    counter = 0;

}

ClientPacketsParser::~ClientPacketsParser() {
    // TODO Auto-generated destructor stub

    qDebug()<<"ClientPacketsParser::~ClientPacketsParser() ";

    QMutexLocker locker(&mutex);

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


    if(cryptography){
        delete cryptography;
        cryptography = 0;
    }


    //networkManager = 0;

}

void ClientPacketsParser::setLocalUDPListeningAddress(const QHostAddress &address){

    this->localUDPListeningAddress = address;
}
void ClientPacketsParser::setLocalUDPListeningPort(quint16 port){

    this->localUDPListeningPort = port;
}

void ClientPacketsParser::run(){

    QMutexLocker locker(&mutex);


    //    processWaitingForReplyPacketsTimer = new QTimer();
    //    processWaitingForReplyPacketsTimer->setSingleShot(false);
    //    processWaitingForReplyPacketsTimer->setInterval(UDP_PACKET_WAITING_FOR_REPLY_TIMEOUT + 500);
    //    connect(processWaitingForReplyPacketsTimer, SIGNAL(timeout()), this, SLOT(processWaitingForReplyPackets()));
    //    processWaitingForReplyPacketsTimer->start();

    //qDebug()<<QString("run()-1:"+QDateTime::currentDateTime().toString("ss.zzz"));

    QTimer processWaitingForReplyPacketsTimer;
    processWaitingForReplyPacketsTimer.setSingleShot(false);
    processWaitingForReplyPacketsTimer.setInterval(UDP_PACKET_WAITING_FOR_REPLY_TIMEOUT);
    connect(&processWaitingForReplyPacketsTimer, SIGNAL(timeout()), this, SLOT(processWaitingForReplyPackets()));
    connect(this, SIGNAL(signalAboutToQuit()), &processWaitingForReplyPacketsTimer, SLOT(stop()));
    processWaitingForReplyPacketsTimer.start();

    //    do{
    //        qWarning()<<"ClientPacketsParser::run()---0";
    //        QCoreApplication::processEvents();
    //        parseIncomingPackets();
    //        processOutgoingPackets();
    //        msleep(500);
    //    } while(!isAboutToQuit());

    while(1){

        QCoreApplication::processEvents();
        parseIncomingPackets();
        processOutgoingPackets();

        if(isAboutToQuit()){
            break;
        }
        msleep(500);
    }


    processWaitingForReplyPacketsTimer.stop();

    processOutgoingPackets();


    //processWaitingForReplyPacketsTimer->stop();

}

bool ClientPacketsParser::readyToQuit(){
    QMutexLocker locker1(&mutex);
    QMutexLocker locker2(&counterMutex);
    return (counter == 0);

}

void ClientPacketsParser::startparseIncomingPackets(){

    {
        QMutexLocker locker(&counterMutex);
        counter++;
    }



    while(!isAboutToQuit()){
        QCoreApplication::processEvents();
        parseIncomingPackets();
        msleep(200);
    }


    {
        QMutexLocker locker(&counterMutex);
        counter--;
    }


}

void ClientPacketsParser::startprocessOutgoingPackets(){


    {
        QMutexLocker locker(&counterMutex);
        counter++;
    }


    QTimer processWaitingForReplyPacketsTimer;
    processWaitingForReplyPacketsTimer.setSingleShot(false);
    //processWaitingForReplyPacketsTimer.setInterval(UDP_PACKET_WAITING_FOR_REPLY_TIMEOUT);
    processWaitingForReplyPacketsTimer.setInterval(UDP_PACKET_WAITING_FOR_REPLY_TIMEOUT/2);
    connect(&processWaitingForReplyPacketsTimer, SIGNAL(timeout()), this, SLOT(processWaitingForReplyPackets()));
    connect(this, SIGNAL(signalAboutToQuit()), &processWaitingForReplyPacketsTimer, SLOT(stop()));
    processWaitingForReplyPacketsTimer.start();

    while(!isAboutToQuit()){
        //QCoreApplication::processEvents();
        processOutgoingPackets();
        msleep(200);
    }

    processWaitingForReplyPacketsTimer.stop();

    processOutgoingPackets();


    {
        QMutexLocker locker(&counterMutex);
        counter--;
    }



}

void ClientPacketsParser::parseIncomingPacketData(Packet *packet){
    //    qDebug()<<"----ClientPacketsParser::parseIncomingPacketData(Packet *packet)";

    //        if((packet->getTransmissionProtocol() == TP_UDP)
    //            && (networkManager->isLocalAddress(packet->getPeerHostAddress()))
    //            && (packet->getPeerHostPort() == localIPMCListeningPort)){
    //            qDebug()<<"~~Packet is been discarded!";
    //            return;
    //        }else if((packet->getTransmissionProtocol() == TP_TCP)
    //            && (packet->getPeerHostAddress() == networkManager->localTCPListeningAddress())
    //            && (packet->getPeerHostPort() == networkManager->localTCPListeningPort())){
    //            qDebug()<<"~~Packet is been discarded!";
    //            return;
    //        }

    //    qDebug()<<"~~networkManager->localAddress():"<<networkManager->localTCPListeningAddress().toString();
    //    qDebug()<<"~~packet->getPeerHostAddress():"<<packet->getPeerHostAddress();

    QByteArray packetData = packet->getPacketData();
    QDataStream in(&packetData, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_7);

    QString peerID = "";
    in >> peerID;

    QHostAddress peerAddress = packet->getPeerHostAddress();
    quint16 peerPort = packet->getPeerHostPort();
    quint16 packetSerialNumber = packet->getPacketSerialNumber();

    quint8 packetType = packet->getPacketType();
    switch(packetType){
    case quint8(HEHUI::HeartbeatPacket):
    {
        QString contactID;
        in >> contactID;
        emit signalHeartbeatPacketReceived(contactID);
        qDebug()<<"~~HeartbeatPacket--"<<" contactID:"<<contactID;
    }
    break;
    case quint8(HEHUI::ConfirmationOfReceiptPacket):
    {
        quint16 packetSerialNumber1 = 0, packetSerialNumber2 = 0;
        in >> packetSerialNumber1 >> packetSerialNumber2;
        m_packetHandlerBase->removeWaitingForReplyPacket(packetSerialNumber1, packetSerialNumber2);
        emit signalConfirmationOfReceiptPacketReceived(packetSerialNumber1, packetSerialNumber2);
        //qDebug()<<"~~ConfirmationOfReceiptPacket:"<<packetSerialNumber;
    }
    break;
    case quint8(IM::ServerDeclare):
    {
        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerID);

        QString address;
        quint16 udpPort = 0, tcpPort = 0;
        QString name;
        QString version;
        in >> address >> udpPort >> tcpPort >> name >> version;
        m_serverAddress = QHostAddress(address);
        m_serverUDPListeningPort = udpPort;
        m_serverTCPListeningPort = tcpPort;
        serverName = name;

        serverLastOnlineTime = QDateTime::currentDateTime();

        m_packetHandlerBase->removeWaitingForReplyPacket(clientLookForServerPacketSerialNumber, 0);
        //m_networkManager->serverFound(m_serverAddress.toString(), m_serverUDPListeningPort, m_serverTCPListeningPort, serverName, version);

        emit signalServerDeclarePacketReceived(m_serverAddress.toString(), m_serverUDPListeningPort, m_serverTCPListeningPort, serverName, version);


        //            qWarning()<<"~~ServerDeclare"<<" serverAddress:"<<address<<" servername:"<<name <<" serverTCPListeningPort:"<<serverTCPListeningPort;
    }
    break;
    case quint8(IM::ServerOnline):
    {
        QString address;
        quint16 port;
        QString name;
        in >> address >> port >> name;
        m_serverAddress = QHostAddress(address);
        m_serverTCPListeningPort = port;
        serverName = name;

        serverLastOnlineTime = QDateTime::currentDateTime();

        emit signalServerOnlinePacketReceived(m_serverAddress, m_serverTCPListeningPort, serverName);
        qDebug()<<"~~ServerOnline";
    }
    break;
    case quint8(IM::ServerOffline):
    {
        QString address;
        quint16 port;
        QString name;
        in >> address >> port >> name;
        m_serverAddress = QHostAddress::Null;
        m_serverTCPListeningPort = 0;
        serverName = "";

        stopHeartbeat();

        emit signalServerOfflinePacketReceived(m_serverAddress, m_serverTCPListeningPort, serverName);
        qWarning()<<"~~ServerOffline";
    }
    break;
    case quint8(IM::ServerAnnouncement):
    {
        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerID);

        QString groupName, computerName, announcement;
        quint8 mustRead;
        in >> announcement >> mustRead;
        emit signalServerAnnouncementPacketReceived(announcement, (mustRead == quint8(0))?false:true);
        qDebug()<<"~~ServerAnnouncement"<<"groupName:"<<groupName<<"computerName:"<<computerName<<"announcement:"<<announcement<<"mustRead:"<<mustRead;
    }
    break;

    case quint8(IM::SERVER_RESPONSE_CLIENT_REGISTRATION):
    {
        
        qWarning()<<"--SERVER_RESPONSE_CLIENT_REQUEST_REGISTRATION";
        
        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerID);

        
        quint8 errorTypeCode = quint8(IM::ERROR_UnKnownError);
        QString message = "";
        in >> errorTypeCode >> message;
        emit signalRegistrationResultReceived(errorTypeCode, message);
        
    }
    break;

    case quint8(IM::SERVER_RESPONSE_CLIENT_REQUEST_UPDATE_PASSWORD):
    {
        
        qWarning()<<"--SERVER_RESPONSE_CLIENT_REQUEST_UPDATE_PASSWORD";
        
        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerID);

        quint8 errorTypeCode = quint8(IM::ERROR_UnKnownError);
        QString message = "";
        in >> errorTypeCode >> message;
        emit signalUpdatePasswordResultReceived(errorTypeCode, message);
        
    }
    break;

    case quint8(IM::SERVER_RESPONSE_CLIENT_REQUEST_LOGIN):
    {
        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerID);

        quint8 requestLoginResultCode = 0;
        in >> requestLoginResultCode;
        if(requestLoginResultCode){
            QString serverAddress = "";
            quint16 serverPort = 0;
            in >> serverAddress >> serverPort;
            if(!QHostAddress(serverAddress).isNull()){
                m_serverAddress = QHostAddress(serverAddress);
                m_serverUDPListeningPort = serverPort;
            }

            login();
        }else{
            quint8 errorTypeCode = quint8(IM::ERROR_UnKnownError);
            in >> errorTypeCode;
            emit signalLoginResultReceived(errorTypeCode);

        }

        qWarning()<<"--SERVER_RESPONSE_CLIENT_REQUEST_LOGIN";
    }
    break;

    case quint8(IM::SERVER_RESPONSE_CLIENT_LOGIN_REDIRECTION):
    {
        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerID);

        QString serverAddress = "";
        quint16 serverPort = 0;
        in >> serverAddress >> serverPort;

        m_serverAddress = serverAddress;
        m_serverUDPListeningPort = serverPort;
        login();
    }
    break;

    case quint8(IM::SERVER_RESPONSE_CLIENT_LOGIN_RESULT):
    {
        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerID);

        quint8 loginResultCode = 10;
        in >> loginResultCode;
        quint8 errorTypeCode = quint8(IM::ERROR_UnKnownError);
        if(loginResultCode){
            QByteArray encryptedData;
            quint32 personalInfoVersionOnServer = 1, personalContactGroupsInfoVersionOnServer = 1, interestGroupsInfoVersionOnServer = 1, blacklistInfoVersionOnServer = 1;
            in >> encryptedData >> personalInfoVersionOnServer >> personalContactGroupsInfoVersionOnServer >> interestGroupsInfoVersionOnServer >> blacklistInfoVersionOnServer;

            QByteArray decryptedData;
            cryptography->teaCrypto(&decryptedData, encryptedData, sessionEncryptionKey, false);

            //TODO 验证数据是否解密成功！
            QDataStream stream(&decryptedData, QIODevice::ReadOnly);
            stream.setVersion(QDataStream::Qt_4_7);
            stream >> sessionEncryptionKey;

            user->setSessionEncryptionKey(sessionEncryptionKey);
            this->m_serverAddress = peerAddress;
            this->m_serverUDPListeningPort = peerPort;

            errorTypeCode = quint8(IM::ERROR_NoError);

            //TODO:载入本地信息
            user->loadMyInfoFromLocalDatabase();

            if(personalInfoVersionOnServer != user->getPersonalInfoVersion()){requestContactInfo(m_myUserID);}
            if(personalContactGroupsInfoVersionOnServer != user->getPersonalContactGroupsVersion()){updatePersonalContactGroupsInfo(false);}
            //if(interestGroupsInfoVersionOnServer != user->getInterestGroupInfoVersion()){requestInterestGroupsList();}
            requestInterestGroupsList();
            if(blacklistInfoVersionOnServer != user->getBlacklistInfoVersion()){requestBlacklistInfo();}

        }else{
            in >> errorTypeCode;
            //emit signalLoginResultReceived(IM::ErrorType(errorTypeCode));

        }

        emit signalLoginResultReceived(errorTypeCode);


        qWarning()<<"--SERVER_RESPONSE_CLIENT_LOGIN_RESULT";

    }
    break;

    case quint8(IM::ONLINE_STATE_CHANGED):
    {
        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerID);

        QByteArray encryptedData;
        in >> encryptedData;


        QByteArray decryptedData;
        cryptography->teaCrypto(&decryptedData, encryptedData, sessionEncryptionKey, false);
        //TODO 验证数据是否解密成功！
        QDataStream stream(&decryptedData, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_7);
        QString contactID = "", contactHostAddress = "";
        quint16 contactHostPort = 0;
        quint8 stateCode = quint8(IM::ONLINESTATE_OFFLINE);
        stream >> contactID >> stateCode >> contactHostAddress >> contactHostPort;

        //emit signalContactStateChangedPacketReceived(contactID, IM::OnlineState(stateCode), contactHostAddress, contactHostPort);
        emit signalContactStateChangedPacketReceived(contactID, stateCode, contactHostAddress, contactHostPort);

        qWarning()<<"--ONLINE_STATE_CHANGED";
    }
    break;

    case quint8(IM::CONTACTS_ONLINE_INFO):
    {
        qWarning()<<"--CONTACTS_ONLINE_INFO";

        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerID);

        QByteArray encryptedData;
        in >> encryptedData;


        QByteArray decryptedData;
        cryptography->teaCrypto(&decryptedData, encryptedData, sessionEncryptionKey, false);
        //TODO 验证数据是否解密成功！
        QDataStream stream(&decryptedData, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_7);
        QString contactsOnlineInfoString = "";
        stream >> contactsOnlineInfoString;



        emit signalContactsOnlineInfoPacketReceived(contactsOnlineInfoString);

    }
    break;


    case quint8(IM::SERVER_RESPONSE_USER_SUMMARY_INFO):
    {
        qWarning()<<"--SERVER_RESPONSE_USER_SUMMARY_INFO";
        
        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerID);

        //TODO:用户信息的格式

        QString userID = "";
        QByteArray encryptedData;
        in >> userID >> encryptedData;


        QByteArray decryptedData;
        cryptography->teaCrypto(&decryptedData, encryptedData, sessionEncryptionKey, false);
        //TODO 验证数据是否解密成功！
        QDataStream stream(&decryptedData, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_7);
        QString userInfo = "";
        stream >> userInfo;
        
        if(userID == user->getUserID()){
            user->setPersonalSummaryInfo(userInfo);
            user->saveMyInfoToLocalDatabase();
        }else{
            user->setContactSummaryInfo(userID, userInfo);
            user->saveContactInfoToLocalDatabase(userID);
        }

        emit signalUserInfoPacketReceived(userID, userInfo);

    }
    break;

    case quint8(IM::CONTACT_GROUPS_INFO):
    {
        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerID);

        QByteArray encryptedData;
        in >> encryptedData;

        QByteArray decryptedData;
        cryptography->teaCrypto(&decryptedData, encryptedData, sessionEncryptionKey, false);
        //TODO 验证数据是否解密成功！
        QDataStream stream(&decryptedData, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_7);
        QString groupsInfo = "";
        quint32 personalContactGroupsInfoVersionOnServer = 1;
        stream >> groupsInfo >> personalContactGroupsInfoVersionOnServer;
        
        //TODO: Save the data

        emit signalContactGroupsInfoPacketReceived(groupsInfo, personalContactGroupsInfoVersionOnServer);
        
        qWarning()<<"--CONTACT_GROUPS_INFO";

    }
    break;

    case quint8(IM::SERVER_RESPONSE_SEARCH_CONTACTS):
    {
        //TODO:搜索结果的格式
        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerID);

        QByteArray encryptedData;
        in >> encryptedData;

        QByteArray decryptedData;
        cryptography->teaCrypto(&decryptedData, encryptedData, sessionEncryptionKey, false);
        //TODO 验证数据是否解密成功！
        QDataStream stream(&decryptedData, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_7);
        QString result = "";
        stream >> result;

        emit signalSearchContactsResultPacketReceived(result.split(QString(PACKET_DATA_SEPARTOR)));

        qWarning()<<"--SERVER_RESPONSE_SEARCH_CONTACTS";
    }
    break;

    case quint8(IM::CLIENT_REQUEST_ADD_CONTACT):
    {
        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerID);

        QByteArray encryptedData;
        in >> encryptedData;

        QByteArray decryptedData;
        cryptography->teaCrypto(&decryptedData, encryptedData, sessionEncryptionKey, false);
        //TODO 验证数据是否解密成功！
        QDataStream stream(&decryptedData, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_7);
        QString userID = "", userNickName = "", userFace = "", verificationMessage = "";
        stream >> userID >> userNickName >> userFace >> verificationMessage;

        //emit signalAddContactResultPacketReceived(contactID, IM::ErrorType(errorTypeCode));
        emit signalAddContactRequestFromUserPacketReceived(userID, userNickName, userFace, verificationMessage);
        
    }
    break;

    case quint8(IM::CLIENT_RESPONSE_ADD_CONTACT_REQUEST):
    {
        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerID);

        QByteArray encryptedData;
        in >> encryptedData;

        QByteArray decryptedData;
        cryptography->teaCrypto(&decryptedData, encryptedData, sessionEncryptionKey, false);
        //TODO 验证数据是否解密成功！
        QDataStream stream(&decryptedData, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_7);
        QString userID = "", userNickName = "", userFace = "", reasonMessage = "";
        quint8 errorTypeCode = quint8(IM::ERROR_UnKnownError);
        
        stream >> userID >> userNickName >> userFace >> errorTypeCode >> reasonMessage;

        //emit signalAddContactResultPacketReceived(contactID, IM::ErrorType(errorTypeCode));
        emit signalAddContactResultPacketReceived(userID, userNickName, userFace, errorTypeCode, reasonMessage);
        
    }
    break;

    case quint8(IM::SERVER_RESPONSE_INTEREST_GROUPS_LIST):
    {
        qDebug()<<"--SERVER_RESPONSE_INTEREST_GROUPS_LIST";

        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerID);

        QByteArray encryptedData;
        in >> encryptedData;

        QByteArray decryptedData;
        cryptography->teaCrypto(&decryptedData, encryptedData, sessionEncryptionKey, false);
        //TODO 验证数据是否解密成功！
        QDataStream stream(&decryptedData, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_7);
        QString interestGroupsListOnServerForUser = "";
        quint32 interestGroupsInfoVersionOnServer = 1;

        stream >> interestGroupsListOnServerForUser >> interestGroupsInfoVersionOnServer;

        emit signalInterestGroupsListPacketReceived(interestGroupsListOnServerForUser, interestGroupsInfoVersionOnServer);

    }
    break;

    case quint8(IM::SERVER_RESPONSE_INTEREST_GROUP_INFO):
    {
        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerID);

        QByteArray encryptedData;
        in >> encryptedData;

        QByteArray decryptedData;
        cryptography->teaCrypto(&decryptedData, encryptedData, sessionEncryptionKey, false);
        //TODO 验证数据是否解密成功！
        QDataStream stream(&decryptedData, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_7);
        quint32 groupID = 0;
        QString interestGroupInfoStringFromServer = "";

        stream >> interestGroupInfoStringFromServer >> groupID;

        emit signalInterestGroupInfoPacketReceived(interestGroupInfoStringFromServer, groupID);

    }
    break;

    case quint8(IM::SERVER_RESPONSE_INTEREST_GROUP_MEMBERS_INFO):
    {
        qWarning()<<"----SERVER_RESPONSE_INTEREST_GROUP_MEMBERS_INFO";

        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerID);

        QByteArray encryptedData;
        in >> encryptedData;

        QByteArray decryptedData;
        cryptography->teaCrypto(&decryptedData, encryptedData, sessionEncryptionKey, false);
        //TODO 验证数据是否解密成功！
        QDataStream stream(&decryptedData, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_7);
        QString interestGroupMembersInfoStringFromServer = "";
        quint32 interestGroupMembersInfoVersionOnServer = 0, groupID = 0;

        stream >> interestGroupMembersInfoStringFromServer >> interestGroupMembersInfoVersionOnServer >> groupID;

        emit signalInterestGroupMembersInfoPacketReceived(interestGroupMembersInfoStringFromServer, interestGroupMembersInfoVersionOnServer, groupID);

    }
    break;

    case quint8(IM::BLACKLIST_INFO):
    {
        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerID);

        QByteArray encryptedData;
        in >> encryptedData;

        QByteArray decryptedData;
        cryptography->teaCrypto(&decryptedData, encryptedData, sessionEncryptionKey, false);
        //TODO 验证数据是否解密成功！
        QDataStream stream(&decryptedData, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_7);
        QString blacklistOnServer = "";
        quint32 blacklistInfoVersionOnServer = 1;

        stream >> blacklistOnServer >> blacklistInfoVersionOnServer;

        emit signalBlacklistInfoPacketReceived(blacklistOnServer, blacklistInfoVersionOnServer);

    }
    break;


    case quint8(IM::SESSION_ENCRYPTION_KEY_WITH_CONTACT):
    {
        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerID);

        QByteArray encryptedData;
        in >> encryptedData;

        QByteArray decryptedData;
        cryptography->teaCrypto(&decryptedData, encryptedData, sessionEncryptionKey, false);
        //TODO 验证数据是否解密成功！
        QDataStream stream(&decryptedData, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_7);
        QString contactID = "";
        QByteArray key;
        stream >> contactID >> key;

        sessionEncryptionKeyWithContactHash[contactID] = key;

    }
    break;

    case quint8(IM::CHAT_MESSAGE_FROM_CONTACT):
    {
        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerID);

        //TODO:判断加密KEY

        QByteArray encryptedData;
        QString contactID = peerID;
        in >> encryptedData;

        QByteArray decryptedData;
        cryptography->teaCrypto(&decryptedData, encryptedData, sessionEncryptionKeyWithContactHash.value(contactID), false);
        //TODO 验证数据是否解密成功！
        QDataStream stream(&decryptedData, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_7);
        QString message = "";
        stream  >> message;

        emit signalChatMessageReceivedFromContact(contactID, message, "");

    }
    break;

    case quint8(IM::CHAT_MESSAGES_CACHED_ON_SERVER):
    {
        //TODO:缓存消息的格式
        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerID);

        QByteArray encryptedData;
        in >> encryptedData;

        QByteArray decryptedData;
        cryptography->teaCrypto(&decryptedData, encryptedData, sessionEncryptionKey, false);
        //TODO 验证数据是否解密成功！
        QDataStream stream(&decryptedData, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_7);
        QString messagesString = "";
        stream >> messagesString;

        emit signalChatMessageCachedOnServerReceived(messagesString.split(QString(UNIT_SEPARTOR)));

    }
    break;

    case quint8(IM::CHAT_IMAGE):
    {
        //TODO:接收文件
    }
    break;

    case quint8(IM::GROUP_CHAT_MESSAGES_CACHED_ON_SERVER):
    {
        //TODO:缓存消息的格式
        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerID);

        QByteArray encryptedData;
        in >> encryptedData;

        QByteArray decryptedData;
        cryptography->teaCrypto(&decryptedData, encryptedData, sessionEncryptionKey, false);
        //TODO 验证数据是否解密成功！
        QDataStream stream(&decryptedData, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_7);
        QString messagesString = "";
        stream >> messagesString;

        emit signalInterestGroupChatMessagesCachedOnServerReceived(messagesString.split(QString(UNIT_SEPARTOR)));

    }
    break;

    case quint8(IM::GROUP_CHAT_MESSAGE):
    {
        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerID);

        QByteArray encryptedData;
        QString contactID = peerID;
        in >> encryptedData;

        QByteArray decryptedData;
        cryptography->teaCrypto(&decryptedData, encryptedData, sessionEncryptionKeyWithContactHash.value(contactID), false);
        //TODO 验证数据是否解密成功！
        QDataStream stream(&decryptedData, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_7);
        quint32 interestGroupID = 0;
        QString message = "";
        stream  >> interestGroupID >> message;

        emit signalInterestGroupChatMessageReceivedFromContact(interestGroupID, contactID, message, "");

    }
    break;
    case quint8(IM::REQUEST_FILE_TRANSMISSION):
    {
        //TODO:
    }
    break;
    case quint8(IM::ACCEPT_FILE_TRANSMISSION):
    {
        //TODO:
    }
    break;

    case quint8(IM::REFUSE_FILE_TRANSMISSION):
    {
        //TODO:
    }
    break;
    case quint8(IM::ABORT_FILE_TRANSMISSION):
    {
        //TODO:
    }
    break;
    case quint8(IM::FILE):
    {
        //TODO:
    }
    break;
    case quint8(IM::VIDEO):
    {
        //TODO:
    }
    break;
    case quint8(IM::AUDIO):
    {
        //TODO:
    }
    break;
    //    case quint8(IM::):
    //        {
    //            //TODO:
    //        }
    //        break;




    default:
        qWarning()<<"Unknown Packet Type:"<<packetType
                 <<" Serial Number:"<<packetSerialNumber
                <<" From:"<<peerAddress.toString()
               <<" Port:"<<peerPort;
        break;

    }



}

//void ClientPacketsParser::setServerAddress(const QHostAddress&serverAddress){
//    this->m_serverAddress = serverAddress;
//}

QHostAddress ClientPacketsParser::getServerAddress() const{
    return m_serverAddress;
}

//void ClientPacketsParser::setServerPort(quint16 serverPort){
//    this->m_serverUDPListeningPort = serverPort;
//}

quint16 ClientPacketsParser::getServerPort() const{
    return m_serverUDPListeningPort;
}

void ClientPacketsParser::startHeartbeat(int interval){
    if(NULL == heartbeatTimer){
        heartbeatTimer = new QTimer();
        heartbeatTimer->setSingleShot(false);
        heartbeatTimer->setInterval(interval);
        connect(heartbeatTimer, SIGNAL(timeout()), this, SLOT(sendHeartbeatPacket()));

    }else{
        heartbeatTimer->stop();
        heartbeatTimer->setInterval(interval);
    }

    heartbeatTimer->start();


}

void ClientPacketsParser::stopHeartbeat(){
    if(heartbeatTimer){
        heartbeatTimer->stop();
    }

}

//int ClientPacketsParser::crypto(QByteArray *destination, const QByteArray &source, bool encrypt){

//    return cryptography->teaCrypto(destination, source, sessionEncryptionKey, encrypt);

//}


quint16 ClientPacketsParser::getLastReceivedPacketSN(const QString &peerID){
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
