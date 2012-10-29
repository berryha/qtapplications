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




namespace HEHUI {


ClientPacketsParser::ClientPacketsParser(ClientResourcesManager *manager, QObject *parent)
    :QObject(parent), m_resourcesManager(manager)
{

    Q_ASSERT(m_resourcesManager);

    m_udpServer = m_resourcesManager->getUDPServer();
    Q_ASSERT_X(m_udpServer, "ClientPacketsParser::ClientPacketsParser(...)", "Invalid UDPServer!");
    connect(m_udpServer, SIGNAL(signalNewUDPPacketReceived(Packet*)), this, SLOT(parseIncomingPacketData(Packet*)), Qt::QueuedConnection);


    m_rtp = m_resourcesManager->getRTP();
    Q_ASSERT(m_rtp);

    m_udtProtocol = m_rtp->getUDTProtocol();
    Q_ASSERT(m_udtProtocol);
    m_udtProtocol->startWaitingForIOInOneThread(10);
    //m_udtProtocol->startWaitingForIOInSeparateThread(100, 1000);
    connect(m_udtProtocol, SIGNAL(packetReceived(Packet*)), this, SLOT(parseIncomingPacketData(Packet*)), Qt::QueuedConnection);

    m_tcpServer = m_rtp->getTCPServer();
    Q_ASSERT(m_tcpServer);
    connect(m_tcpServer, SIGNAL(packetReceived(Packet*)), this, SLOT(parseIncomingPacketData(Packet*)), Qt::QueuedConnection);


    serverAddress = QHostAddress::Null;
    serverUDTListeningPort = 0;
    serverName = "";

    m_localUDTServerListeningPort = m_udtProtocol->getUDTListeningPort();
    m_localTCPServerListeningPort = m_tcpServer->getTCPServerListeningPort();

    m_localComputerName = QHostInfo::localHostName().toLower();
    
}

ClientPacketsParser::~ClientPacketsParser() {
    // TODO Auto-generated destructor stub

    qDebug()<<"ClientPacketsParser::~ClientPacketsParser() ";






}


void ClientPacketsParser::parseIncomingPacketData(Packet *packet){
    //    qDebug()<<"----ClientPacketsParser::parseIncomingPacketData(Packet *packet)";


    QByteArray packetData = packet->getPacketData();
    QDataStream in(&packetData, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_6);

    QString peerName = "";
    in >> peerName;

    QHostAddress peerAddress = packet->getPeerHostAddress();
    quint16 peerPort = packet->getPeerHostPort();
    quint8 packetType = packet->getPacketType();
    int socketID = packet->getSocketID();

    PacketHandlerBase::recylePacket(packet);

//    qDebug()<<"--ClientPacketsParser::parseIncomingPacketData(...) "<<" peerName:"<<peerName<<" peerAddress:"<<peerAddress<<" peerPort:"<<peerPort<<" socketID:"<<socketID<<" packetType:"<<packetType;

    switch(packetType){

    case quint8(MS::ServerDeclare):
    {

        QString address = "";
        quint16 udtPort = 0, tcpPort = 0;
        QString version = "";
        int serverInstanceID = 0;
        in >> address >> udtPort >> tcpPort >> version >> serverInstanceID;
        serverAddress = peerAddress;
        serverUDTListeningPort = udtPort;
        serverName = peerName;

        emit signalServerDeclarePacketReceived(serverAddress.toString(), serverUDTListeningPort, tcpPort, serverName, version, serverInstanceID);
        qDebug()<<"~~ServerDeclare"<<" serverAddress:"<<serverAddress.toString()<<" servername:"<<serverName <<" serverRUDPListeningPort:"<<serverUDTListeningPort << " serverTCPListeningPort:"<<tcpPort;
    }
    break;
    case quint8(MS::ServerOnline):
    {
        QString address = "";
        quint16 port = 0;
        in >> address >> port;
        serverAddress = peerAddress;
        serverUDTListeningPort = port;
        serverName = peerName;


        emit signalServerOnlinePacketReceived(serverAddress, serverUDTListeningPort, serverName);
        qDebug()<<"~~ServerOnline"<<" serverAddress:"<<serverAddress.toString()<<" servername:"<<serverName <<" serverRUDPListeningPort:"<<serverUDTListeningPort;

    }
    break;
    case quint8(MS::ServerOffline):
    {
        QString address = "";
        quint16 port = 0;
        in >> address >> port;
        serverAddress = QHostAddress::Null;
        serverUDTListeningPort = 0;
        serverName = "";

        //emit signalServerOfflinePacketReceived(serverAddress, serverRUDPListeningPort, serverName);
        qDebug()<<"~~ServerOffline"<<" peerAddress:"<<peerAddress.toString()<<" servername:"<<peerName <<" peerPort:"<<peerPort;
    }
    break;
    case quint8(MS::ClientDetailedInfoRequested):
    {
        quint16 peerRUDPListeningPort = 0;
        QString computerName = "";
        bool rescan = false;
        in >> peerRUDPListeningPort >> computerName >> rescan;

        if(!computerName.isEmpty()){
            if(computerName.toLower() != m_localComputerName){
                return;
            }
        }

        emit signalClientDetailedInfoRequestedPacketReceived(computerName, rescan, socketID);
        qDebug()<<"~~ClientDetailedInfoRequested";
    }
    break;
    case quint8(MS::AdminRequestRemoteConsole):
    {

//        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerName);

        QString computerName = "", applicationPath = "", adminID = "";
        bool startProcess = true;
        in >> computerName >> applicationPath >> adminID >> startProcess;

        if(computerName.toLower() != m_localComputerName){
            return;
        }

        emit signalAdminRequestRemoteConsolePacketReceived(computerName, applicationPath, adminID, startProcess, peerAddress.toString(), peerPort);

        qDebug()<<"~~AdminRequestRemoteConsole";
    }
    break;
    //    case quint8(MS::ClientResponseRemoteConsole):
    //        break;
    case quint8(MS::RemoteConsoleCMDFromAdmin):
    {

//        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerName);

        QString computerName = "", command = "";
        in >> computerName >> command;

        if(computerName.toLower() != m_localComputerName){
            return;
        }

        emit signalRemoteConsoleCMDFromServerPacketReceived(computerName, command, peerAddress.toString(), peerPort);
        qDebug()<<"~~RemoteConsoleCMDFromAdmin";
    }
    break;
    //    case quint8(MS::RemoteConsoleCMDResultFromClient):
    //        break;
    case quint8(MS::ServerRequestClientSummaryInfo):
    {
//        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerName);

        QString groupName = "", computerName = "", userName = "";
        in >> groupName >> computerName >> userName;

        if(computerName.toLower() != m_localComputerName){
            return;
        }

        emit signalServerRequestClientSummaryInfoPacketReceived(groupName, computerName, userName);
        qDebug()<<"~~ServerRequestClientInfo";
    }
    break;
    //    case quint8(MS::ClientResponseClientInfo):
    //        break;
    //    case quint8(MS::ClientRequestSoftwareVersion):
    //        break;
    case quint8(MS::ServerResponseSoftwareVersion):
    {
//        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerName);

        QString softwareName, version;
        in >> softwareName >> version;
        emit signalServerResponseSoftwareVersionPacketReceived(softwareName, softwareName);
        qDebug()<<"~~ServerResponseSoftwareVersion";
    }
    break;
    case quint8(MS::ServerAnnouncement):
    {
//        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerName);

        QString groupName = "", computerName = "", announcement = "", adminName = "";
        quint32 announcementID = 0;
        quint8 mustRead = 1;
        in >> groupName >> computerName >> announcementID >> announcement >> adminName >> mustRead ;

        if(!computerName.trimmed().isEmpty()){
            if(!m_localComputerName.contains(computerName, Qt::CaseInsensitive)){
                return;
            }
        }

        emit signalServerAnnouncementPacketReceived(groupName, computerName, announcementID, announcement, adminName, (mustRead == quint8(0))?false:true);

        qDebug()<<"~~ServerAnnouncement"<<"groupName:"<<groupName<<"computerName:"<<computerName<<"announcement:"<<announcement<<"mustRead:"<<mustRead;
        
        //            foreach (QString userName, localUsersHash.keys()) {
        //                if(userName.trimmed().isEmpty()){
        //                    continue;
        //                }
        //                sendServerAnnouncementPacket(QHostAddress::LocalHost, localUsersHash.value(userName), adminName, announcement);

        //            }

        //            qWarning()<<"~~ServerAnnouncement";

    }
    break;
    case quint8(MS::Update):
        emit signalUpdateClientSoftwarePacketReceived();
        qDebug()<<"~~Update";
        break;
    case quint8(MS::AdminRequestSetupUSBSD):
    {

//        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerName);

        QString computerName = "", users = "", adminName = "";
        bool enable = false;
        bool temporarilyAllowed = true;
        in >> computerName >> users >> enable >> temporarilyAllowed >> adminName;

        if(computerName.toLower() != m_localComputerName){
            return;
        }

        emit signalSetupUSBSDPacketReceived(computerName, users, enable, temporarilyAllowed, adminName, peerAddress.toString(), peerPort);
        qDebug()<<"~~SetupUSBSD";
    }
    break;
    case quint8(MS::AdminRequestSetupProgrames):
    {

//        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerName);

        QString computerName = "", users = "", adminName = "";
        bool enable = false;
        bool temporarilyAllowed = true;
        in >> computerName >> users >> enable >> temporarilyAllowed >> adminName;

        if(computerName.toLower() != m_localComputerName){
            return;
        }

        emit signalSetupProgramesPacketReceived(computerName, users, enable, temporarilyAllowed, adminName, peerAddress.toString(), peerPort);
        qDebug()<<"~~SetupProgrames";
    }
    break;
    case quint8(MS::ShowAdmin):
    {
//        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerName);

        QString computerName = "", users = "";
        bool show = false;
        in >> computerName >> users >> show;

        if(computerName.toLower() != m_localComputerName){
            return;
        }

        emit signalShowAdminPacketReceived(computerName, users, show);
        qDebug()<<"~~ShowAdmin";
    }
    break;
    case quint8(MS::ModifyAdminGroupUser):
    {

//        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerName);

        QString computerName = "", userName = "", adminName = "";
        bool addToAdminGroup = false;
        in >> computerName >> userName >> addToAdminGroup >> adminName;

        if(computerName.toLower() != m_localComputerName){
            return;
        }

        emit signalModifyAdminGroupUserPacketReceived(computerName, userName, addToAdminGroup, adminName, peerAddress.toString(), peerPort);
        qDebug()<<"~~ModifyAdminGroupUser";
    }
    break;
    case quint8(MS::RenameComputer):
    {
        QString oldComputerName = "", newComputerName = "", adminName = "";
        in >> oldComputerName >> newComputerName >> adminName;

        if(oldComputerName.toLower() != m_localComputerName){
            return;
        }

        emit signalRenameComputerPacketReceived(newComputerName, adminName, peerAddress.toString(), peerPort);
        qDebug()<<"~~RenameComputer";
    }
    break;
    case quint8(MS::JoinOrUnjoinDomain):
    {
        QString computerName = "", adminName = "", domainOrWorkgroupName = "";
        bool join = false;
        in >> computerName >> adminName >> join >> domainOrWorkgroupName;

        if(computerName.toLower() != m_localComputerName){
            return;
        }

        emit signalJoinOrUnjoinDomainPacketReceived(adminName, join, domainOrWorkgroupName, peerAddress.toString(), peerPort);
        qDebug()<<"~~JoinOrUnjoinDomain";
    }
    break;

    case quint8(MS::AdminRequestConnectionToClient):
    {

        QString computerName = "", users = "" ;
        in >> computerName >> users ;

        if(computerName.toLower() != m_localComputerName){
            m_udtProtocol->closeSocket(socketID);
            return;
        }

        emit signalAdminRequestConnectionToClientPacketReceived(socketID, computerName, users);
        qDebug()<<"~~AdminRequestConnectionToClient";
    }
    break;
    case quint8(MS::AdminSearchClient):
    {

        QString computerName = "", userName = "", workgroup = "", macAddress = "", ipAddress = "", osVersion = "", adminName = "" ;
        in >> computerName >> userName >> workgroup >> macAddress >> ipAddress >> osVersion >> adminName;

        if(!computerName.isEmpty()){
            if(computerName.toLower() != m_localComputerName){
                return;
            }
        }

        emit signalAdminSearchClientPacketReceived(peerAddress.toString(), peerPort, computerName, userName, workgroup, macAddress, ipAddress, osVersion, adminName);

        qDebug()<<"~~AdminSearchClient "<<" peerAddress:"<<peerAddress<<" peerPort:"<<peerPort;
    }
    break;


    case quint8(MS::AdminRequestRemoteAssistance):
    {

        QString computerName = "", adminName = "";
        in >> computerName >> adminName;
        //emit signalAdminRequestRemoteAssistancePacketReceived(peerName, adminName, peerAddress.toString(), peerPort);


        if(computerName.toLower() != m_localComputerName){
            return;
        }

        foreach (int socketID, m_localUserSocketsHash.keys()) {
            sendRequestRemoteAssistancePacket(socketID, peerAddress.toString(), peerPort, adminName);
        }

        qDebug()<<"~~AdminRequestRemoteAssistance";
    }
    break;
    case quint8(MS::UpdateMSWUserPassword):
    {

//        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerName);

        QString workgroup = "", adminName = "";
        in >> workgroup >> adminName;
        emit signalAdminRequestUpdateMSUserPasswordPacketReceived(workgroup, adminName, peerAddress.toString(), peerPort);
        qDebug()<<"~~UpdateMSWUserPassword";
    }
    break;

    case quint8(MS::InformUserNewPassword):
    {

//        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerName);

        QString workgroup = "", adminName = "";
        in >> workgroup >> adminName;

        emit signalAdminRequestInformUserNewPasswordPacketReceived(workgroup, adminName, peerAddress.toString(), peerPort);

        qDebug()<<"~~InformUserNewPassword";


    }
    break;


    case quint8(MS::LocalUserOnline):
    {
        QString userName = "", computerName = "";
        in >> userName >> computerName;
        
        if(computerName.toLower() != m_localComputerName){
            return;
        }
        m_localUserSocketsHash.insert(socketID, userName);
        
        emit signalLocalUserOnlineStatusChanged(socketID, userName, true);

        qDebug()<<"~~UserOnline"<<"Name:"<<userName<<" Port:"<<peerPort;
        
        
    }
    break;

    case quint8(MS::LocalUserOffline):
    {
        QString userName = "", computerName = "";
        in >> userName >> computerName;
        
        if(computerName.toLower() != m_localComputerName){
            return;
        }

        m_localUserSocketsHash.remove(socketID);

        emit signalLocalUserOnlineStatusChanged(socketID, userName, false);
        
        qDebug()<<"~~UserOffline";
        
    }
    break;


    case quint8(MS::RequestFileSystemInfo):
    {
        QString parentDirPath = "";
        in >> parentDirPath;

        emit signalFileSystemInfoRequested(socketID, parentDirPath);

        qDebug()<<"~~RequestFileSystemInfo";
    }
    break;
    case quint8(MS::RequestUploadFile):
    {
        QByteArray fileMD5Sum;
        QString fileName = "";
        quint64 size = 0;
        QString localFileSaveDir = "./";
        in >> fileMD5Sum >> fileName >> size >> localFileSaveDir ;

        emit signalAdminRequestUploadFile(socketID, fileMD5Sum, fileName, size, localFileSaveDir);

        qDebug()<<"~~RequestUploadFile";
    }
    break;
    case quint8(MS::RequestDownloadFile):
    {
        QString localBaseDir, fileName, remoteFileSaveDir;
        in >> localBaseDir >> fileName >> remoteFileSaveDir;

        emit signalAdminRequestDownloadFile(socketID, localBaseDir, fileName, remoteFileSaveDir);

        qDebug()<<"~~RequestDownloadFile";
    }
    break;
    case quint8(MS::RequestFileData):
    {
        QByteArray fileMD5;
        int startPieceIndex = 0, endPieceIndex = 0;
        in >> fileMD5 >> startPieceIndex >> endPieceIndex;

        emit signalFileDataRequested(socketID, fileMD5, startPieceIndex, endPieceIndex);

        qDebug()<<"~~RequestFileData";
    }
    break;
    case quint8(MS::FileData):
    {
        QByteArray fileMD5, data, sha1;;
        int pieceIndex = 0;


        in >> fileMD5 >> pieceIndex >> data >>sha1;

//        if(data.size() != size || sha1 != QCryptographicHash::hash(data, QCryptographicHash::Sha1)){
//            qCritical()<<"ERROR! Data Verification Failed!";
//            requestFileData(socketID, offset, size);
//            return;
//        }

        emit signalFileDataReceived(socketID, fileMD5, pieceIndex, data, sha1);

        //qDebug()<<"~~FileData";
    }
    break;
    case quint8(MS::FileTXStatusChanged):
    {
        QByteArray fileMD5;
        quint8 status;
        in >> fileMD5 >> status;

        emit signalFileTXStatusChanged(socketID, fileMD5, status);

        qDebug()<<"~~FileTXStatusChanged";
    }
    break;
    case quint8(MS::FileTXError):
    {
        QByteArray fileMD5;
        quint8 errorCode;
        QString message;
        in >> fileMD5 >> errorCode >> message;

        emit signalFileTXError(socketID, fileMD5, errorCode, message);

        qDebug()<<"~~FileTXStatusChanged";
    }
    break;





    default:
//        qWarning()<<"Unknown Packet Type:"<<packetType//<<" Serial Number:"<<packetSerialNumber
//                <<" From:"<<peerAddress.toString()<<":"<<peerPort
//                <<" (ClientPacketsParser)";

        break;

    }



}

void ClientPacketsParser::localUserOffline(int socketID){
    if(m_localUserSocketsHash.contains(socketID)){
        qWarning()<<tr("Local user %1 offine!").arg(m_localUserSocketsHash.value(socketID));
    }
    m_localUserSocketsHash.remove(socketID);

}
























} //namespace HEHUI
