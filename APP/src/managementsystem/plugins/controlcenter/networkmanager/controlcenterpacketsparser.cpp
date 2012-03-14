/*
 ****************************************************************************
 * controlcenterpacketsparser.cpp
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


#include <QHostInfo>
#include <QDebug>

#include "controlcenterpacketsparser.h"

//#ifdef Q_CC_MSVC
//#include <windows.h>
//#include "HHSharedWindowsManagement/hwindowsmanagement.h"
//#define msleep(x) Sleep(x)
//#endif

//#ifdef Q_CC_GNU
//#include <unistd.h>
//#define msleep(x) usleep(x*1000)
//#endif


namespace HEHUI {


ControlCenterPacketsParser::ControlCenterPacketsParser(ResourcesManagerInstance *manager, QObject *parent)
    :QObject(parent), m_resourcesManager(manager)
{

    Q_ASSERT(m_resourcesManager);

    m_udpServer = m_resourcesManager->getUDPServer();
    Q_ASSERT_X(m_udpServer, "ControlCenterPacketsParser::ControlCenterPacketsParser(...)", "Invalid UDPServer!");
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



    ipmcGroupAddress = QString(IP_MULTICAST_GROUP_ADDRESS);
    ipmcListeningPort = quint16(IP_MULTICAST_GROUP_PORT);


    localUDTListeningPort = m_udtProtocol->getUDTListeningPort();
    m_localTCPServerListeningPort = m_tcpServer->getTCPServerListeningPort();


    m_localComputerName = QHostInfo::localHostName().toLower();
    m_localID = m_localComputerName + "/ControlCenter";



}

ControlCenterPacketsParser::~ControlCenterPacketsParser() {
    // TODO Auto-generated destructor stub
    qDebug()<<"~ControlCenterPacketsParser()";

    disconnect();

}


void ControlCenterPacketsParser::parseIncomingPacketData(Packet *packet){
    qDebug()<<"----ControlCenterPacketsParser::parseIncomingPacketData(Packet *packet)";


    QByteArray packetData = packet->getPacketData();
    QDataStream in(&packetData, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_6);

    QString peerName = "";
    in >> peerName;

    QHostAddress peerAddress = packet->getPeerHostAddress();
    quint16 peerPort = packet->getPeerHostPort();
//    quint16 packetSerialNumber = packet->getPacketSerialNumber();
    quint8 packetType = packet->getPacketType();
    int socketID = packet->getSocketID();

    PacketHandlerBase::recylePacket(packet);
//    qDebug()<<"--ControlCenterPacketsParser::parseIncomingPacketData(...) "<<" peerName:"<<peerName<<" peerAddress:"<<peerAddress<<" peerPort:"<<peerPort<<" packetSerialNumber:"<<packetSerialNumber<<" packetType:"<<packetType;

    switch(packetType){
//    case quint8(HEHUI::HeartbeatPacket):
//    {
//        //QString computerName;
//        //in >> computerName;
//        emit signalHeartbeatPacketReceived(peerName);
//    }
//    break;
//    case quint8(HEHUI::ConfirmationOfReceiptPacket):
//    {
//        quint16 packetSerialNumber1 = 0, packetSerialNumber2 = 0;
//        in >> packetSerialNumber1 >> packetSerialNumber2;
//        m_packetHandlerBase->removeWaitingForReplyPacket(packetSerialNumber1, packetSerialNumber2);
//        emit signalConfirmationOfReceiptPacketReceived(packetSerialNumber1, packetSerialNumber2);
//        qDebug()<<"~~ConfirmationOfReceiptPacket--"<<packetSerialNumber1<<" "<<packetSerialNumber2;
//    }
//    break;
    case quint8(MS::ServerDeclare):
    {

        QString address = "";
        quint16 udtPort = 0, tcpPort = 0;
        QString version;
        int serverInstanceID = 0;
        in >> address >> udtPort >> tcpPort >> version >> serverInstanceID;
        serverAddress = peerAddress;
        serverUDTListeningPort = udtPort;
        serverName = peerName;
        emit signalServerDeclarePacketReceived(serverAddress.toString(), serverUDTListeningPort, tcpPort, serverName, version, serverInstanceID);

        qDebug()<<"~~ServerDeclare"<<" serverAddress:"<<serverAddress<<" servername:"<<peerName <<" serverRUDPListeningPort:"<<serverUDTListeningPort<<" serverTCPListeningPort:"<<tcpPort;
    }
    break;
    //    case quint8(MS::ClientOnline):
    //        break;
    //    case quint8(MS::ClientOffline):
    //        break;
    case quint8(MS::ServerOnline):
    {
        QString address = "";
        quint16 port;
        in >> address >> port;
        serverAddress = peerAddress;
        serverUDTListeningPort = port;
        serverName = peerName;
        emit signalServerOnlinePacketReceived(serverAddress, serverUDTListeningPort, serverName);
    }
    break;
    case quint8(MS::ServerOffline):
    {
        QString address = "";
        quint16 port = 0;
        in >> address >> port;
        serverAddress = QHostAddress::Null;
        serverUDTListeningPort = 0;
        serverName = peerName;
        emit signalServerOfflinePacketReceived(serverAddress, serverUDTListeningPort, serverName);
    }
    break;
    case quint8(MS::ClientOnline):
    {
        QString peerName;
        in >> peerName;
        emit signalClientOnlineStatusChanged(socketID, peerName, true);

        qDebug()<<"~~ClientOnline--"<<" peerAddress:"<<peerAddress<<"   peerName:"<<peerName;
    }
    break;
    case quint8(MS::ClientOffline):
    {
        QString peerName;
        in >> peerName;
        emit signalClientOnlineStatusChanged(socketID, peerName, false);
        qDebug()<<"~~ClientOffline--"<<" peerAddress:"<<peerAddress.toString()<<"   peerName:"<<peerName;;
    }
    break;
    case quint8(MS::ClientResponseClientDetailedInfo):
    {
        qDebug()<<"ClientResponseClientInfoToAdminRequest";

        QString systemInfo = "";
        in >> systemInfo;
        emit signalClientResponseClientDetailedInfoPacketReceived(peerName, systemInfo);

    }
    break;

    case quint8(MS::ClientResponseUSBInfo):
    {
        qDebug()<<"ClientResponseUSBInfo";

        QString info = "";
        in >> info;
        emit signalClientResponseUSBInfoPacketReceived(socketID, peerName, info);

    }
    break;
    case quint8(MS::ClientResponseProgramesInfo):
    {
        qDebug()<<"ClientResponseUSBInfo";

        QString info = "";
        in >> info;
        emit signalClientResponseProgramesInfoPacketReceived(socketID, peerName, info);

    }
    break;
    //    case quint8(MS::ServerResponseAdminLoggedOnToServerRequest):
    //        break;
    //    case quint8(MS::AdminRequestRemoteConsole):
    //        {
    //            QString adminID;
    //            in >> adminID;
    //            emit signalServerRequestRemoteConsolePacketReceived(adminID);
    //        }
    //        break;
    case quint8(MS::ClientResponseRemoteConsoleStatus):
    {
        quint8 running = false;
        QString extraMessage = "";
        quint8 messageType = quint8(MS::MSG_Information);
        in >> running >> extraMessage >> messageType;

        emit signalClientResponseRemoteConsoleStatusPacketReceived(peerName, ((running == 0)?false:true), extraMessage, messageType);
        qDebug()<<"~~ClientResponseRemoteConsole";
    }
    break;
    //    case quint8(MS::RemoteConsoleCMDFromAdmin):
    //        {
    //            QString command;
    //            in >> command;
    //            emit signalRemoteConsoleCMDFromServerPacketReceived(command);
    //        }
    //        break;
    case quint8(MS::RemoteConsoleCMDResultFromClient):
    {
        QString result = "";
        in >> result;
        emit signalRemoteConsoleCMDResultFromClientPacketReceived(peerName, result);
        qDebug()<<"~~RemoteConsoleCMDResultFromClient";
    }
    break;
    //    case quint8(MS::ServerRequestClientInfo):
    //        {
    //            QString groupName, computerName;
    //            QChar separtor;
    //            in >> groupName >> separtor >> computerName;
    //            emit signalServerRequestClientSummaryInfoPacketReceived(groupName, computerName);
    //        }
    //        break;
    case quint8(MS::ServerResponseSoftwareVersion):
    {
        QString softwareName, version;
        QChar separtor;
        in >> softwareName >> separtor >>version;
        emit signalServerResponseSoftwareVersionPacketReceived(softwareName, softwareName);
    }
    break;

    case quint8(MS::ClientResponseClientSummaryInfo):
    {
        QString workgroupName = "", networkInfo = "", usersInfo = "", osInfo = "", admins = "", clientVersion = "";
        bool usbsdEnabled = false, programesEnabled = false, isJoinedToDomain = false;
        in >> workgroupName >> networkInfo >> usersInfo >> osInfo >> usbsdEnabled >> programesEnabled >> admins >> isJoinedToDomain >> clientVersion;
        emit signalClientResponseClientSummaryInfoPacketReceived(peerName, workgroupName, networkInfo, usersInfo, osInfo, usbsdEnabled, programesEnabled, admins, isJoinedToDomain, clientVersion);
        //qWarning()<<"~~ClientResponseClientSummaryInfo";
    }
    break;
    case quint8(MS::ClientResponseAdminConnectionResult):
    {
        QString message = "";
        bool result = false;
        in >> result >> message;
        emit signalClientResponseAdminConnectionResultPacketReceived(socketID, peerName, result, message);
        qDebug()<<"~~ClientResponseAdminConnectionResult";
    }
    break;
    case quint8(MS::ClientMessage):
    {

        QString message = "";
        quint8 clientMessageType = quint8(MS::MSG_Information);
        in >> message >> clientMessageType;
        emit signalClientMessagePacketReceived(peerName, message, clientMessageType);
    }
    break;

    case quint8(MS::UserResponseRemoteAssistance):
    {

        QString userName = "", computerName = "";
        bool accept = false;
        in >> userName >> computerName >> accept;
        
        emit signalUserResponseRemoteAssistancePacketReceived(userName, computerName, accept);
        
    }
    break;

    case quint8(MS::NewPasswordRetrevedByUser):
    {

//        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerName);

        QString userName = "", computerName = "";
        in >> userName >> computerName ;
        
        emit signalNewPasswordRetrevedByUserPacketReceived(userName, computerName);
        
    }
    break;



///////////////////////////////////////////////
    case quint8(MS::ResponseFileSystemInfo):
    {
        QString parentDirPath = "";
        QByteArray data;
        in >> parentDirPath >> data ;

        emit signalFileSystemInfoReceived(socketID, parentDirPath, data);

        qDebug()<<"~~ResponseFileSystemInfo";
    }
    break;
    case quint8(MS::RequestUploadFile):
    {
        QByteArray fileMD5Sum;
        QString fileName = "";
        quint64 size = 0;
        QString localFileSaveDir = "";
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

    case quint8(MS::ResponseFileDownloadRequest):
    {
        QString remoteFileName = "";
        bool accepted = false;
        in >> remoteFileName >> accepted;

        if(accepted){
            QByteArray fileMD5Sum;
            quint64 size = 0;
            QString localFileSaveDir = "./";
            in >> fileMD5Sum >> size >> localFileSaveDir;
            emit signalFileDownloadRequestAccepted(socketID, remoteFileName, fileMD5Sum, size, localFileSaveDir);
        }else{
            QString message;
            in >> message;
            emit signalFileDownloadRequestDenied(socketID, remoteFileName, message);

        }

        qDebug()<<"~~ResponseFileDownloadRequest";
    }
    break;
    case quint8(MS::ResponseFileUploadRequest):
    {

        QByteArray fileMD5Sum;
        QString message = "";
        bool accepted = false;
        in >> fileMD5Sum >> accepted >> message;
        emit signalFileUploadRequestResponsed(socketID, fileMD5Sum, accepted, message);

        qDebug()<<"~~ResponseFileUploadRequest";
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

        qDebug()<<"~~FileData";
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
        qWarning()<<"ControlCenterPacketsParser! Unknown Packet Type: "<<packetType
//                 <<"    Serial Number: "<<packetSerialNumber
                <<"    From: "<<peerAddress.toString()
               <<":"<<peerPort;
        break;

    }



}























} //namespace HEHUI
