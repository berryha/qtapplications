/*
 ****************************************************************************
 * serverservice.cpp
 *
 * Created on: 2010-7-12
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
 * Last Modified on: 2010-9-3
 * Last Modified by: 贺辉
 ***************************************************************************
 */




#include <QtCore>
#include "serverservice.h"

#include "../app_constants.h"
#include "../settings.h"





namespace HEHUI {


ServerService::ServerService(int argc, char **argv, const QString &serviceName, const QString &description )
    :Service(argc, argv, serviceName, description)
{


    setStartupType(QtServiceController::AutoStartup);
    //    setServiceFlags(CanBeSuspended);

    //    networkManager = NetworkManager::instance();
    //    serverPacketsParser = 0;
    //    databaseUtility = new DatabaseUtility(this);
    //    mainServiceStarted = false;

    resourcesManager = 0;
    serverPacketsParser = 0;

    m_udpServer = 0;
    m_rtp = 0;
    m_udtProtocol = 0;

    databaseUtility = 0;
    query = 0;
    mainServiceStarted = false;

    clientInfoHash.clear();

    onlineAdminsCount = 0;



}

ServerService::~ServerService(){
    qDebug()<<"ServerService::~ServerService()";

    if(resourcesManager){
        //resourcesManager->closeAllServers();
    }

    if(serverPacketsParser){
        delete serverPacketsParser;
        serverPacketsParser = 0;
    }

//    ResourcesManagerInstance::cleanInstance();
    delete resourcesManager;
    resourcesManager = 0;

//    delete m_udpServer;
//    delete m_udtProtocol;

    PacketHandlerBase::clean();


    QList<ClientInfo*> clientInfoList = clientInfoHash.values();
    clientInfoHash.clear();
    foreach(ClientInfo *info, clientInfoList){
        delete info;
        info = 0;
    }

    delete databaseUtility;
    databaseUtility = 0;
    delete query;
    query = 0;

    mainServiceStarted = false;


}

bool ServerService::startMainService(){
    qDebug()<<"----ServerService::startMainService()";

    if(mainServiceStarted){
        qWarning()<<"Main service has already started!";
        return true;
    }


    if(openDatabase()){
        getRecordsInDatabase();
    }


    QString errorMessage = "";
//    m_udpServer = resourcesManager->startIPMCServer(QHostAddress(IP_MULTICAST_GROUP_ADDRESS), quint16(IP_MULTICAST_GROUP_PORT), &errorMessage);
//    if(!m_udpServer){
//        logMessage(QString("Can not start IP Multicast listening on address '%1', port %2! %3").arg(IP_MULTICAST_GROUP_ADDRESS).arg(IP_MULTICAST_GROUP_PORT).arg(errorMessage), QtServiceBase::Error);
//        m_udpServer = resourcesManager->startUDPServer(QHostAddress::Any, quint16(IP_MULTICAST_GROUP_PORT), true, &errorMessage);
//    }else{
//        qWarning()<<QString("IP Multicast listening on address '%1', port %2!").arg(IP_MULTICAST_GROUP_ADDRESS).arg(IP_MULTICAST_GROUP_PORT);
//    }

    m_udpServer = resourcesManager->startUDPServer(QHostAddress::Any, quint16(IP_MULTICAST_GROUP_PORT), true, &errorMessage);
    if(!m_udpServer){
        logMessage(QString("Can not start UDP listening on port %1! %2").arg(IP_MULTICAST_GROUP_PORT).arg(errorMessage), QtServiceBase::Error);
    }else{
        qWarning()<<QString("UDP listening on port %1!").arg(IP_MULTICAST_GROUP_PORT);
    }


    m_rtp = resourcesManager->startRTP(QHostAddress::Any, UDT_LISTENING_PORT, true, &errorMessage);
    connect(m_rtp, SIGNAL(disconnected(int)), this, SLOT(peerDisconnected(int)), Qt::QueuedConnection);

    m_udtProtocol = m_rtp->getUDTProtocol();

//    if(!m_udtProtocol){
//        QString error = tr("Can not start UDT listening on port %1! %2").arg(UDT_LISTENING_PORT).arg(errorMessage);
//        logMessage(error, QtServiceBase::Error);
//        return false;
//    }else{
//        qWarning()<<QString("UDT listening on port %1!").arg(UDT_LISTENING_PORT);
//    }
//    connect(m_udtProtocol, SIGNAL(disconnected(int)), this, SLOT(peerDisconnected(int)));
    m_udtProtocol->startWaitingForIOInOneThread(30);
    //m_udtProtocol->startWaitingForIOInSeparateThread(10, 500);



    serverPacketsParser = new ServerPacketsParser(resourcesManager, this);
    //connect(m_udpServer, SIGNAL(signalNewUDPPacketReceived(Packet*)), clientPacketsParser, SLOT(parseIncomingPacketData(Packet*)));
    //connect(m_udtProtocol, SIGNAL(packetReceived(Packet*)), clientPacketsParser, SLOT(parseIncomingPacketData(Packet*)));

    connect(serverPacketsParser, SIGNAL(signalClientLogReceived(const QString&, const QString&, const QString&, quint8, const QString&, const QString&)), this, SLOT(saveClientLog(const QString&, const QString&, const QString&, quint8, const QString&, const QString&)), Qt::QueuedConnection);
    connect(serverPacketsParser, SIGNAL(signalClientResponseClientSummaryInfoPacketReceived(const QString&, const QString&, const QString&, const QString&, const QString&, bool, bool, const QString&, bool, const QString&)), this, SLOT(updateOrSaveClientSummaryInfo(const QString&, const QString&, const QString&, const QString&, const QString&, bool, bool, const QString&, bool, const QString&)), Qt::QueuedConnection);
    connect(serverPacketsParser, SIGNAL(signalClientResponseClientDetailedInfoPacketReceived(const QString &, const QString &)), this, SLOT(clientDetailedInfoPacketReceived(const QString &, const QString &)));

//    connect(serverPacketsParser, SIGNAL(signalHeartbeatPacketReceived(const QString &, const QString&)), this, SLOT(processHeartbeatPacket(const QString &, const QString&)), Qt::QueuedConnection);
    connect(serverPacketsParser, SIGNAL(signalClientOnlineStatusChanged(int, const QString&, bool)), this, SLOT(processClientOnlineStatusChangedPacket(int, const QString&, bool)), Qt::QueuedConnection);
    connect(serverPacketsParser, SIGNAL(signalAdminOnlineStatusChanged(int, const QString&, const QString&, bool)), this, SLOT(processAdminOnlineStatusChangedPacket(int, const QString&, const QString&, bool)), Qt::QueuedConnection);

    //Single Process Thread
    //QtConcurrent::run(serverPacketsParser, &ServerPacketsParser::run);

    //IMPORTANT For Multi-thread
    //QThreadPool::globalInstance()->setMaxThreadCount(MIN_THREAD_COUNT);
    //QtConcurrent::run(serverPacketsParser, &ServerPacketsParser::startparseIncomingPackets);
    //QtConcurrent::run(serverPacketsParser, &ServerPacketsParser::startprocessOutgoingPackets);


    //sendServerOnlinePacket();

    //serverPacketsParser->sendServerDeclarePacket(QHostAddress(IP_MULTICAST_GROUP_ADDRESS), quint16(IP_MULTICAST_GROUP_PORT), networkManager->localTCPListeningAddress(), networkManager->localTCPListeningPort(), networkManager->hostName());
    //serverPacketsParser->sendServerDeclarePacket(QHostAddress::Broadcast, quint16(IP_MULTICAST_GROUP_PORT), networkManager->localTCPListeningAddress(), networkManager->localTCPListeningPort(), networkManager->hostName());
    sendServerOnlinePacket();

    sendServerOnlinePacketTimer = new QTimer(this);
    sendServerOnlinePacketTimer->setSingleShot(false);
    sendServerOnlinePacketTimer->setInterval(15*60000);
    connect(sendServerOnlinePacketTimer, SIGNAL(timeout()), this, SLOT(sendServerOnlinePacket()));
    sendServerOnlinePacketTimer->start();

    mainServiceStarted = true;

    return true;
}

void ServerService::saveClientLog(const QString &computerName, const QString &users, const QString &clientAddress, quint8 logType, const QString &log, const QString &clientTime){
    //qWarning()<<"ServerService::saveClientLog(...) IP:"<<clientAddress<<" log:"<<log;

    if(!query){
        if(!openDatabase()){
            return;
        }
    }else{
        query->clear();
    }

    query->prepare("INSERT INTO logs (ComputerName, Users, IPAddress, Type, Content, ClientTime) "
                   "VALUES (:ComputerName, :Users, :IPAddress, :Type, :Content, :ClientTime)");

    query->bindValue(":ComputerName", computerName);
    query->bindValue(":Users", users);
    query->bindValue(":IPAddress", clientAddress);
    query->bindValue(":Type", logType);
    query->bindValue(":Content", log);
    query->bindValue(":ClientTime", clientTime);

    if(!query->exec()){
        QSqlError error = query->lastError();
        QString msg = QString("Can not write log to database! %1 Error Type:%2 Error NO.:%3").arg(error.text()).arg(error.type()).arg(error.number());
        logMessage(msg, QtServiceBase::Error);
        qCritical()<< msg;

        //MySQL数据库重启，重新连接
        if(error.number() == 2006){
            query->clear();
            openDatabase(true);
        }

    }

    query->clear();

}

void ServerService::sendServerOnlinePacket(){
    qDebug()<<"----ServerService::sendServerOnlinePacket()";

    serverPacketsParser->sendServerDeclarePacket(QHostAddress::Broadcast, quint16(IP_MULTICAST_GROUP_PORT));
    serverPacketsParser->sendServerDeclarePacket(QHostAddress(IP_MULTICAST_GROUP_ADDRESS), quint16(IP_MULTICAST_GROUP_PORT));

    //serverPacketsParser->sendServerOnlinePacket();
    
    updateOrSaveAllClientsInfoToDatabase();

}

void ServerService::updateOrSaveClientSummaryInfo(const QString &computerName, const QString &workgroupName, const QString &networkInfo, const QString &usersInfo, const QString &osInfo, bool usbsdEnabled, bool programesEnabled, const QString &admins, bool isJoinedToDomain, const QString &clientVersion){
    //    qWarning()<<"ServerService::updateOrSaveClientSummaryInfo(...)";

    if(computerName.trimmed().isEmpty()){
        qCritical()<<"Invalid Computer Name!";
        return;
    }
    //qWarning()<<"Computer Name:"<<computerName;
    //qWarning()<<"isRecordExistInDB:"<<isRecordExistInDB(computerName);


    QString statement = "";
    ClientInfo *info = 0;
    if(clientInfoHash.contains(computerName)){
        info = clientInfoHash.value(computerName);
        //qWarning()<<QString("Client Info of '%1' Exists In:  Memory:YES  DB:%2").arg(computerName).arg(isRecordExistInDB(computerName)?"YES":"NO")<<"\n";
    }else{
        info = new ClientInfo(computerName, this);
        clientInfoHash.insert(computerName, info);
        //qWarning()<<QString("Client Info of '%1' Exists In:  Memory:NO  DB:%2").arg(computerName).arg(isRecordExistInDB(computerName)?"YES":"NO")<<"\n";
    }
    //qWarning();
    //qWarning()<<QString("Total Online Clients:%1").arg(clientSocketsHash.size());


    if(isRecordExistInDB(computerName)){
        qDebug()<<"Client Info Exists!";
        //info = clientInfoHash.value(computerName);
        statement = "UPDATE summaryinfo SET LastOnlineTime = NULL ";
        if(workgroupName != info->getWorkgroup()){
            statement += QString(", Workgroup = '%1' ").arg(workgroupName);
            //info->setWorkgroup(workgroupName);
        }
        if(networkInfo != info->getNetwork()){
            statement += QString(", Network = '%1' ").arg(networkInfo);
            //info->setNetwork(networkInfo);
        }
        if(usersInfo != info->getUsers()){
            QString tempUsersInfo = usersInfo;
            tempUsersInfo.replace("\\", "\\\\");
            statement += QString(", Users = '%1' ").arg(tempUsersInfo);
            //info->setUsers(usersInfo);
        }
        if(osInfo != info->getOs()){
            statement += QString(", OS = '%1' ").arg(osInfo);
            //info->setOs(osInfo);
        }
        //if(usbsdEnabled != info->getUsbSDEnabled()){
        statement += QString(", USBSD = %1 ").arg(QVariant(usbsdEnabled).toUInt());
        //info->setUsbSDEnabled(usbsdEnabled);
        //}
        //if(programesEnabled != info->getProgramsEnabled()){
        statement += QString(", Programes = %1 ").arg(QVariant(programesEnabled).toUInt());
        //info->setProgramsEnabled(programesEnabled);
        //}
        statement += QString(", JoinedToDomain = %1 ").arg(QVariant(isJoinedToDomain).toUInt());

        if(admins != info->getAdministrators()){
            QString tempAdmins = admins;
            tempAdmins.replace("\\", "\\\\");
            statement += QString(", Administrators = '%1' ").arg(tempAdmins);
            //info->setAdministrators(admins);
        }
        if(clientVersion != info->getClientVersion()){
            statement += QString(", ClientVersion = '%1' ").arg(clientVersion);
            //info->setClientVersion(clientVersion);
        }

        statement += QString("WHERE ComputerName = '%1'").arg(computerName);
        //qWarning()<<"Update Client Info For Computer "<<computerName << " "<<networkInfo;
        //qWarning()<<statement;

    }else{
        qDebug()<<"Client Summary Info Not Exists!";

        statement = "START TRANSACTION;";
        statement += QString("INSERT INTO summaryinfo (ComputerName, Workgroup, Network, Users, OS, USBSD, Programes, JoinedToDomain, Administrators, ClientVersion) "
                             "VALUES ('%1', '%2', '%3', '%4', '%5', %6, %7, %8, '%9', '%10'); ")
                .arg(computerName).arg(workgroupName).arg(networkInfo).arg(usersInfo).arg(osInfo)
                .arg(QVariant(usbsdEnabled).toUInt()).arg(QVariant(programesEnabled).toUInt())
                .arg(QVariant(isJoinedToDomain).toUInt()).arg(admins).arg(clientVersion);

        statement += QString("INSERT INTO detailedinfo (ComputerName) "
                             "VALUES ('%1'); ").arg(computerName);

        statement += "COMMIT;";
    }


    info->setSummaryInfoSavedTODatabase(false);
    info->setUpdateSummaryInfoStatement(statement);

    if(onlineAdminsCount > 0){
        if(updateOrSaveClientInfoToDatabase(info)){
            info->setWorkgroup(workgroupName);
            info->setNetwork(networkInfo);
            info->setUsers(usersInfo);
            info->setOs(osInfo);
            info->setUsbSDEnabled(usbsdEnabled);
            info->setProgramsEnabled(programesEnabled);
            info->setAdministrators(admins);
            info->setIsJoinedToDomain(isJoinedToDomain);
            info->setClientVersion(clientVersion);

            info->setLastHeartbeatTime(QDateTime::currentDateTime());

        }
    } 

    qWarning();
    qWarning()<<QString(" '%1' Exists In:  Memory:%2  DB:%3 Version:%4").arg(computerName).arg(clientInfoHash.contains(computerName)?"YES":"NO").arg(isRecordExistInDB(computerName)?"YES":"NO").arg(clientVersion);


}

bool ServerService::updateOrSaveClientInfoToDatabase(ClientInfo *info){
    //    qWarning()<<"ServerService::updateOrSaveClientInfoToDatabase(ClientInfo *info)";

    if(!info){
        return false;
    }

    QString computerName = info->getComputerName();

    QString summaryStatement = "";
    QString detailedStatement = "";
    QString updateInstalledSoftwaresInfoStatement = "";

    //    if(isRecordExistInDB(info->getComputerName())){
    if(!info->getSummaryInfoSavedTODatabase()){
        summaryStatement = info->getUpdateSummaryInfoStatement();
    }
    if(!info->getDetailedInfoSavedTODatabase()){
        detailedStatement = info->getUpdateDetailedInfoStatement();
        //qWarning()<<"detailedStatement:"<<detailedStatement;
    }
    if(!info->isInstalledSoftwaresInfoSavedTODatabase()){
        updateInstalledSoftwaresInfoStatement = info->getUpdateInstalledSoftwaresInfoStatement();
    }


    //    }
    //    else{
    //        summaryStatement = "START TRANSACTION;";
    //        //statement += QString("delete from clientinfo where ComputerName = '%1'; ").arg(computerName);
    //        summaryStatement += QString("INSERT INTO summaryinfo (ComputerName, Workgroup, Network, Users, OS, USBSD, Programes, Administrators, ClientVersion) "
    //                            "VALUES ('%1', '%2', '%3', '%4', '%5', %6, %7, '%8', '%9'); ")
    //                .arg(computerName).arg(info->getJoinInformation()).arg(info->getNetwork()).arg(info->getUsers()).arg(info->getOs())
    //                .arg(QVariant(info->getUsbSDEnabled()).toUInt()).arg(QVariant(info->getProgramsEnabled()).toUInt())
    //                .arg(info->getAdministrators()).arg(info->getClientVersion());

    //        summaryStatement += QString("INSERT INTO detailedinfo (ComputerName) "
    //                            "VALUES ('%1'); ").arg(computerName);

    //        summaryStatement += "COMMIT;";

    //    }


    if(!query){
        if(!openDatabase()){
            return false;
        }
    }else{
        query->clear();
    }

    if(!summaryStatement.trimmed().isEmpty()){
        if(!query->exec(summaryStatement)){
            QSqlError error = query->lastError();
            QString msg = QString("Can not write client summary info to database! %1 Error Type:%2 Error NO.:%3").arg(error.text()).arg(error.type()).arg(error.number());
            logMessage(msg, QtServiceBase::Error);
            qCritical()<<msg;
            qCritical()<<"summaryStatement:";
            qCritical()<<summaryStatement;
            qCritical()<<"";
            //MySQL数据库重启，重新连接
            if(error.number() == 2006){
                query->clear();
                openDatabase(true);
            }else{
                getRecordsInDatabase();
            }

            return false;
        }
//        qWarning()<<" Summary Statement For "<<computerName<<": "<<summaryStatement;

        info->setSummaryInfoSavedTODatabase(true);
        info->setUpdateSummaryInfoStatement("");
        qWarning()<<"Client summary info from "<<computerName<<" has been saved!";

        query->clear();
    }

    if(!detailedStatement.trimmed().isEmpty()){
        if(!query->exec(detailedStatement)){
            QSqlError error = query->lastError();
            QString msg = QString("Can not write client detailed info to database! %1 Error Type:%2 Error NO.:%3").arg(error.text()).arg(error.type()).arg(error.number());
            logMessage(msg, QtServiceBase::Error);
            qCritical()<<msg;

            //MySQL数据库重启，重新连接
            if(error.number() == 2006){
                query->clear();
                openDatabase(true);
            }else{
                getRecordsInDatabase();
            }

            return false;
        }
//        qWarning()<<" Detailed Statement For "<<computerName<<": "<<detailedStatement;

        info->setDetailedInfoSavedTODatabase(true);
        info->setUpdateDetailedInfoStatement("");
        qWarning()<<"Client detailed info from "<<computerName<<" has been saved!";

        query->clear();

    }

    if(!updateInstalledSoftwaresInfoStatement.trimmed().isEmpty()){
        query->exec(updateInstalledSoftwaresInfoStatement);
        QSqlError error = query->lastError();
        if(error.type() != QSqlError::NoError){
            //QSqlError error = query->lastError();
            QString msg = QString("Can not write client installed softwares info to database! %1 Error Type:%2 Error NO.:%3").arg(error.text()).arg(error.type()).arg(error.number());
            logMessage(msg, QtServiceBase::Error);
            qCritical()<<msg;

            //MySQL数据库重启，重新连接
            if(error.number() == 2006){
                query->clear();
                openDatabase(true);
            }else{
                getRecordsInDatabase();
            }

            return false;
        }
//        qWarning()<<" Installed Softwares Statement For "<<computerName<<": "<<updateInstalledSoftwaresInfoStatement;

        info->setInstalledSoftwaresInfoSavedTODatabase(true);
        info->setUpdateInstalledSoftwaresInfoStatement("");
        qWarning()<<"Client installed softwares info from "<<computerName<<" has been saved!";

        query->clear();

    }



    if(!isRecordExistInDB(computerName)){
        recordsInDatabase.append(computerName.toLower());
    }

    return true;

}

void ServerService::updateOrSaveAllClientsInfoToDatabase(){

    getRecordsInDatabase();

    foreach(ClientInfo *info, clientInfoHash.values()){
        updateOrSaveClientInfoToDatabase(info);
    }



}

void ServerService::clientDetailedInfoPacketReceived(const QString &computerName, const QString &clientInfo){

    qWarning()<<"Client detailed info has been received From "<< computerName;
    //    qWarning()<<"-----clientDetailedInfoPacketReceived";

    if(computerName.trimmed().isEmpty()){
        qCritical()<<"Invalid Computer Name!";
        return;
    }

    QDir::setCurrent(QDir::tempPath());
    QString clientInfoFilePath = QString("./%1.ini").arg(computerName);

    QFile file(clientInfoFilePath);
    if(!file.open(QIODevice::WriteOnly | QFile::Truncate | QIODevice::Text)){
        qCritical()<<QString(tr("Can not write client detailed info file '")+clientInfoFilePath+tr("'!"));
        return;
    }
    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << clientInfo;
    file.flush();
    file.close();

    QSettings systemInfo(clientInfoFilePath, QSettings::IniFormat, this);
    systemInfo.setIniCodec("UTF-8");

    if(!QFile(clientInfoFilePath).exists()){
        qCritical()<<QString(tr("Client Info File '")+clientInfoFilePath+tr("' Missing!"));
        return;
    }


    systemInfo.beginGroup("DevicesInfo");

    QString cpu = systemInfo.value("CPU").toString();
    QString memory = systemInfo.value("Memory").toString();
    QString motherboardName = systemInfo.value("MotherboardName").toString();
    QString dmiUUID = systemInfo.value("DMIUUID").toString();
    QString chipset = systemInfo.value("Chipset").toString();
    QString video = systemInfo.value("Video").toString();
    QString monitor = systemInfo.value("Monitor").toString();
    QString audio = systemInfo.value("Audio").toString();
    audio.replace("'", "\\'");

    QStringList storages;
    QString storagesInfo = "";
    for (int i = 1; i < 6; i++) {
        //QString diskKey,diskInfo;
        QString diskKey = QString("Disk" + QString::number(i));
        QString diskInfo = systemInfo.value(diskKey).toString();
        if (diskInfo.isEmpty()) {
            break;
        }
        storages.append(diskInfo);
    }
    storagesInfo = storages.join(" | ");
    //qWarning()<<"storagesInfo:"<<storagesInfo;


    QString adapter1Name = systemInfo.value("Adapter1Name").toString();
    QString adapter1HDAddress = systemInfo.value("Adapter1HDAddress").toString();
    QString adapter1IPAddress = systemInfo.value("Adapter1IPAddress").toString();
    QString nic1Info = adapter1Name + "|" + adapter1HDAddress + "|" + adapter1IPAddress;

    QString adapter2Name = systemInfo.value("Adapter2Name").toString();
    QString adapter2HDAddress = systemInfo.value("Adapter2HDAddress").toString();
    QString adapter2IPAddress = systemInfo.value("Adapter2IPAddress").toString();
    QString nic2Info = adapter2Name + "|" + adapter2HDAddress + "|" + adapter2IPAddress;

    systemInfo.endGroup();



    systemInfo.beginGroup("OSInfo");
    QString installationDate = systemInfo.value("InstallationDate").toString();
    QString windowsDir = systemInfo.value("WindowsDir").toString();
    QString osKey = systemInfo.value("Key").toString();

    systemInfo.endGroup();



    systemInfo.beginGroup("InstalledSoftwareInfo");
    QStringList softwares;
    for(int k = 1; k < 400; k++){
        QString info = systemInfo.value(QString::number(k)).toString();
        if(info.isEmpty()){break;}
        softwares << info;
    }
    systemInfo.endGroup();





    QString statement = "";
    ClientInfo *info = 0;
    if(clientInfoHash.contains(computerName)){
        info = clientInfoHash.value(computerName);
    }else{
        info = new ClientInfo(computerName, this);
        clientInfoHash.insert(computerName, info);
    }

    //    if(isRecordExistInDB(computerName)){
    statement = "UPDATE detailedinfo SET UpdateTime = NULL ";
    if(installationDate != info->getInstallationDate()){
        statement += QString(", InstallationDate = '%1' ").arg(installationDate);
    }
    if(windowsDir != info->getWindowsDir()){
        statement += QString(", WindowsDir = '%1' ").arg(windowsDir);
    }
    if(osKey != info->getOsKey()){
        statement += QString(", OSKey = '%1' ").arg(osKey);
    }
    if(cpu != info->getCpu()){
        statement += QString(", CPU = '%1' ").arg(cpu);
    }
    if(memory != info->getMemory()){
        statement += QString(", Memory = '%1' ").arg(memory);
    }
    if(motherboardName != info->getMotherboardName()){
        statement += QString(", MotherboardName = '%1' ").arg(motherboardName);
    }
    if(dmiUUID != info->getDmiUUID()){
        statement += QString(", DMIUUID = '%1' ").arg(dmiUUID);
    }
    if(chipset != info->getChipset()){
        statement += QString(", Chipset = '%1' ").arg(chipset);
    }
    if(video != info->getVideo()){
        statement += QString(", Video = '%1' ").arg(video);
    }
    if(monitor != info->getMonitor()){
        statement += QString(", Monitor = '%1' ").arg(monitor);
    }
    if(audio != info->getAudio()){
        statement += QString(", Audio = '%1' ").arg(audio);
    }
    if(storagesInfo != info->getStorage()){
        statement += QString(", Storage = '%1' ").arg(storagesInfo);
    }
    if(nic1Info != info->getNic1Info()){
        statement += QString(", NIC1 = '%1' ").arg(nic1Info);
    }
    if(nic2Info != info->getNic2Info()){
        statement += QString(", NIC2 = '%1' ").arg(nic2Info);
    }



    statement += QString("WHERE ComputerName = '%1'").arg(computerName);
    //qWarning()<<"Update Client Info For Computer "<<computerName << " "<<networkInfo;
    //qWarning()<<statement;

    //    }else{
    //        qDebug()<<"Client Detailed Info Not Exists!";

    //        //statement = "START TRANSACTION;";
    //        statement += QString("INSERT INTO detailedinfo(ComputerName, InstallationDate, WindowsDir, OSKey, CPU, DMIUUID, MotherboardName, Chipset, Memory, Storage, Video, Audio, NIC1, NIC2, Monitor, UpdateTime, Remark) "
    //                             "VALUES ('%1', '%2', '%3', '%4', '%5', '%6', '%7', '%8', '%9', '%10', '%11', '%12', '%13', '%14', '%15', NULL, NULL); ")
    //                .arg(computerName).arg(installationDate).arg(windowsDir).arg(osKey).arg(cpu)
    //                .arg(dmiUUID).arg(motherboardName).arg(chipset).arg(memory).arg(storagesInfo).arg(video).arg(audio).arg(nic1Info).arg(nic2Info).arg(monitor);

    //        //statement += QString("INSERT INTO detailedinfo (ComputerName) "
    //        //                    "VALUES ('%1'); ").arg(computerName);

    //        //statement += "COMMIT;";


    //    }


    info->setDetailedInfoSavedTODatabase(false);
    info->setUpdateDetailedInfoStatement(statement);




//    QStringList installedSoftwaresInfo = info->getInstalledSoftwaresInfo();
//    bool changed = false;
//    if(softwares.size() == installedSoftwaresInfo.size()){
//        foreach (QString info, softwares) {
//            if(!installedSoftwaresInfo.contains(info)){
//                changed = true;
//                break;
//            }
//        }
//    }else{
//        changed = true;
//    }

    if(softwares.size() != info->getinstalledSoftwaresCount()){
        QString updateInstalledSoftwaresInfoStatement = QString("START TRANSACTION; delete from installedsoftware where ComputerName = '%1'; ").arg(computerName);
        foreach (QString info, softwares) {
            QStringList values = info.split(" | ");
            if(values.size() != 5){continue;}
            updateInstalledSoftwaresInfoStatement += QString(" insert into installedsoftware(ComputerName, SoftwareName, SoftwareVersion, Size, InstallationDate, Publisher) values('%1', '%2', '%3', '%4', '%5', '%6'); ").arg(computerName).arg(values.at(0)).arg(values.at(1)).arg(values.at(2)).arg(values.at(3)).arg(values.at(4));
        }
        updateInstalledSoftwaresInfoStatement += "COMMIT;";

        info->setInstalledSoftwaresInfoSavedTODatabase(false);
        info->setUpdateInstalledSoftwaresInfoStatement(updateInstalledSoftwaresInfoStatement);
        //qWarning()<<"------0------updateInstalledSoftwaresInfoStatement:"<<updateInstalledSoftwaresInfoStatement;

    }




    if(onlineAdminsCount > 0){
        if(updateOrSaveClientInfoToDatabase(info)){
            info->setInstallationDate(installationDate);
            info->setWindowsDir(windowsDir);
            info->setOsKey(osKey);

            info->setCpu(cpu);
            info->setMemory(memory);
            info->setMotherboardName(motherboardName);
            info->setDmiUUID(dmiUUID);
            info->setChipset(chipset);
            info->setVideo(video);
            info->setMonitor(monitor);
            info->setAudio(audio);
            info->setStorage(storagesInfo);

            info->setNic1Info(nic1Info);
            info->setNic2Info(nic2Info);

//            info->setInstalledSoftwaresInfo(softwares);
            info->setInstalledSoftwaresCount(softwares.size());
        }
    }







}


void ServerService::getRecordsInDatabase(){

    if(!query){
        if(!openDatabase()){
            return;
        }
    }else{
        query->clear();
    }

    recordsInDatabase.clear();
    if(query->exec("select ComputerName from summaryinfo")){
        while(query->next()){
            recordsInDatabase.append(query->value(0).toString().toLower());
        }
        qWarning()<<"Records In Database:"<<recordsInDatabase.size();
    }else{
        qCritical()<<QString("Error! Can not query records from database! %1").arg(query->lastError().text());
    }
    query->clear();

}

//void ServerService::processHeartbeatPacket(const QString &clientAddress, const QString &computerName){

//    ClientInfo *info = 0;
//    if(clientInfoHash.contains(computerName)){
//        info = clientInfoHash.value(computerName);
//        info->setLastHeartbeatTime(QDateTime::currentDateTime());
//        info->setOnline(true);
//        //qWarning()<<"Heartbeat Packet From:"<<computerName;
//    }else{
//        //serverPacketsParser->sendServerRequestClientSummaryInfoPacket("", computerName, "", clientAddress);
//        qWarning()<<QString("Unknown Heartbeat Packet From Computer '%1'! IP:%2").arg(computerName).arg(clientAddress);
//    }

//}

void ServerService::processClientOnlineStatusChangedPacket(int socketID, const QString &clientName, bool online){
    qDebug()<<"ServerService::processClientOnlineStatusChangedPacket(...)"<<" socketID:"<<socketID<<" clientName:"<<clientName<<" online:"<<online;

    QString ip = "";
    quint16 port = 0;

    if(online){

        if(!m_rtp->getAddressInfoFromSocket(socketID, &ip, &port)){
            qCritical()<<m_rtp->lastErrorString();
            return;
        }

        if(clientSocketsHash.values().contains(clientName)){
            int preSocketID = clientSocketsHash.key(clientName);
            qDebug()<<"---------preSocketID:"<<preSocketID<<" socketID:"<<socketID;
            if(preSocketID != socketID){
                m_rtp->closeSocket(preSocketID);
                //peerDisconnected(preSocketID);
                clientSocketsHash.remove(preSocketID);
                adminSocketsHash.remove(preSocketID);
            }

//            clientSocketsHash.remove(preSocketID);
//            m_udtProtocol->closeSocket(preSocketID);
        }
        clientSocketsHash.insert(socketID, clientName);

    }else{
        clientSocketsHash.remove(socketID);
        m_rtp->closeSocket(socketID);
    }

    ClientInfo *info = 0;
    if(clientInfoHash.contains(clientName)){
        info = clientInfoHash.value(clientName);
        if(!online){
            ip = info->getClientUDTListeningAddress();
            port = info->getClientUDTListeningPort();
        }
    }else{
        info =  new ClientInfo(clientName, this);
        clientInfoHash.insert(clientName, info);

//        if(online){
//            serverPacketsParser->sendServerRequestClientSummaryInfoPacket(socketID, "", clientName, "");
//        }
        //qWarning()<<QString("Unknown Client '%1' %2 ! %3").arg(clientName).arg(online?"Online":"Offline").arg(QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss"));
        //qWarning()<<QString("Unknown Client '%1' From %2:%3 %4 ! %5").arg(clientName).arg(ip).arg(port).arg(online?"Online":"Offline").arg(QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss"));
    }

    info->setOnline(online);
    if(online){
        info->setClientUDTListeningAddress(ip);
        info->setClientUDTListeningPort(port);
    }

    qWarning();
    qWarning()<<QString("Client '%1' From %2:%3 %4 via %5! Time:%6 Socket: %7").arg(clientName).arg(ip).arg(port).arg(online?"Online":"Offline").arg(m_rtp->isUDTSocket(socketID)?"UDT":"TCP").arg(QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss")).arg(socketID);
    qWarning()<<QString("Total Online Clients:%1").arg(clientSocketsHash.size());


}

void ServerService::processAdminOnlineStatusChangedPacket(int socketID, const QString &clientName, const QString &adminName, bool online){

    QString ip = "";
    quint16 port = 0;

    if(!m_rtp->getAddressInfoFromSocket(socketID, &ip, &port)){
        qCritical()<<m_rtp->lastErrorString();

        return;
    }

    qWarning()<<QString(" Admin %1@%2 %3! %4").arg(adminName).arg(clientName).arg(online?"Online":"Offline").arg(QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss"));

    if(online){
        adminSocketsHash.insert(socketID, adminName);
        updateOrSaveAllClientsInfoToDatabase();
    }else{
        adminSocketsHash.remove(socketID);
    }

}

void ServerService::peerConnected(const QHostAddress &peerAddress, quint16 peerPort){
    qDebug()<<QString("Connected! "+peerAddress.toString()+":"+QString::number(peerPort));

}

void ServerService::signalConnectToPeerTimeout(const QHostAddress &peerAddress, quint16 peerPort){
    qCritical()<<QString("Connecting Timeout! "+peerAddress.toString()+":"+QString::number(peerPort));

}

void ServerService::peerDisconnected(const QHostAddress &peerAddress, quint16 peerPort, bool normalClose){
    qDebug()<<QString("Disconnected! "+peerAddress.toString()+":"+QString::number(peerPort));

    if(!normalClose){
        qCritical()<<QString("ERROR! Peer %1:%2 Closed Unexpectedly!").arg(peerAddress.toString()).arg(peerPort);
    }


}

void ServerService::peerDisconnected(int socketID){
    qDebug()<<"ServerService::peerDisconnected(...)"<<" socketID:"<<socketID;
    qDebug()<<"----------3----------"<<" socketID:"<<socketID<<" Time:"<<QDateTime::currentDateTime().toString("mm:ss:zzz")<<" ThreadID:"<<QThread::currentThreadId();

//    m_rtp->closeSocket(socketID);

    if(clientSocketsHash.contains(socketID)){
        QString address = "Unknown Address";
        ClientInfo *info = clientInfoHash.value(clientSocketsHash.value(socketID));
        if(info){
            address = info->getClientUDTListeningAddress() + ":" + QString::number(info->getClientUDTListeningPort());
        }
        qCritical()<<QString("ERROR! Client '%1' From %2 Closed Connection Unexpectedly! %3").arg(clientSocketsHash.value(socketID)).arg(address).arg(QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss"));
        clientSocketsHash.remove(socketID);
    }

    if(adminSocketsHash.contains(socketID)){
        qCritical()<<QString("ERROR! Admin '%1' Closed Connection Unexpectedly! %2").arg(adminSocketsHash.value(socketID)).arg(QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss"));
        adminSocketsHash.remove(socketID);
    }


}



bool ServerService::openDatabase(bool reopen){

    if(reopen){
        if(query){
            query->clear();
            delete query;
            query = 0;
        }
        databaseUtility->closeDBConnection(SERVERSERVICE_DB_CONNECTION_NAME);
    }

    //    Settings settings(APP_NAME, APP_VERSION, SERVICE_NAME);
    //    qWarning()<<settings.getDBType();
    //    qWarning()<<settings.getDBDriver();
    //    qWarning()<<settings.getDBServerHost();
    //    qWarning()<<settings.getDBServerPort();
    //    qWarning()<<settings.getDBServerUserName();
    //    qWarning()<<settings.getDBServerUserPassword();
    //    qWarning()<<settings.getDBName();



    QSqlDatabase db = QSqlDatabase::database(SERVERSERVICE_DB_CONNECTION_NAME);
    if(!db.isValid()){
        QSqlError err;
        //        err = databaseUtility->openDatabase(MYSQL_DB_CONNECTION_NAME,
        //                                            REMOTE_SITOY_COMPUTERS_DB_DRIVER,
        //                                            REMOTE_SITOY_COMPUTERS_DB_SERVER_HOST,
        //                                            REMOTE_SITOY_COMPUTERS_DB_SERVER_PORT,
        //                                            REMOTE_SITOY_COMPUTERS_DB_USER_NAME,
        //                                            REMOTE_SITOY_COMPUTERS_DB_USER_PASSWORD,
        //                                            REMOTE_SITOY_COMPUTERS_DB_NAME,
        //                                            HEHUI::MYSQL);
        HEHUI::Settings settings(APP_NAME, APP_VERSION, SERVICE_NAME, "./");
        err = databaseUtility->openDatabase(SERVERSERVICE_DB_CONNECTION_NAME,
                                            settings.getDBDriver(),
                                            settings.getDBServerHost(),
                                            settings.getDBServerPort(),
                                            settings.getDBServerUserName(),
                                            settings.getDBServerUserPassword(),
                                            settings.getDBName(),
                                            settings.getDBType());
        if (err.type() != QSqlError::NoError) {
            logMessage(QString("An error occurred when opening the database on '%1'! %2").arg(REMOTE_SITOY_COMPUTERS_DB_SERVER_HOST).arg(err.text()), QtServiceBase::Error);
            qCritical() << QString("XX An error occurred when opening the database: %1").arg(err.text());
            return false;
        }

    }

    db = QSqlDatabase::database(SERVERSERVICE_DB_CONNECTION_NAME);
    if(!db.isOpen()){
        qCritical()<<QString("Database is not open! %1").arg(db.lastError().text());
        return false;
    }

    if(!query){
        query = new QSqlQuery(db);

        //        recordsInDatabase.clear();
        //        if(query->exec("select ComputerName from clientinfo")){
        //            while(query->next()){
        //                recordsInDatabase.append(query->value(0).toString());
        //            }
        //            qWarning()<<"Records In Database:"<<recordsInDatabase.size();
        //        }
        //        query->clear();

    }


    return true;



}

bool ServerService::isRecordExistInDB(const QString &computerName){

    //    if(!query){
    //        if(!openDatabase()){
    //            return false;
    //        }
    //    }else{
    //        query->clear();
    //    }

    //    QString queryString = QString("select ComputerName from clientinfo where ComputerName = '%1'").arg(computerName);

    //    if(!query->exec(queryString)){
    //        qWarning()<<query->lastError().text();
    //        return false;
    //    }
    //    query->first();
    //    QString value = query->value(0).toString();
    //    qWarning()<<"value:"<<value;
    //    if(value == computerName){
    //        return true;
    //    }

    //    return false;

    return recordsInDatabase.contains(computerName, Qt::CaseInsensitive);

}


void ServerService::start()
{

    //return;

    qDebug()<<"----ServerService::start()";

    resourcesManager = ResourcesManagerInstance::instance();
    serverPacketsParser = 0;

    databaseUtility = new DatabaseUtility(this);

    mainServiceStarted = false;

    startMainService();

}

void ServerService::stop()
{

    if(serverPacketsParser){
        serverPacketsParser->sendServerOfflinePacket();
    }

    updateOrSaveAllClientsInfoToDatabase();

    databaseUtility->closeAllDBConnections();

    if(m_udpServer){
        m_udpServer->close();
    }
    if(m_rtp){
        m_rtp->stopServers();
    }

}

void ServerService::pause()
{

}

void ServerService::resume()
{

}

void ServerService::processCommand(int code)
{

    qDebug()<<"----ServerService::processCommand(int code)";
    qDebug()<<"code:"<<code;


    switch(code){
    case 0:
        serverPacketsParser->sendUpdateClientSoftwarePacket();
        break;
    case 1:
        serverPacketsParser->sendServerRequestClientSummaryInfoPacket("", "", "", "255.255.255.255");
        break;
    case 2:
        updateOrSaveAllClientsInfoToDatabase();
        break;
    case 3:
        serverPacketsParser->sendRequestClientDetailedInfoPacket();
    case 4:
        serverPacketsParser->sendRequestClientDetailedInfoPacket("255.255.255.255", IP_MULTICAST_GROUP_PORT, "", false);
        break;
    default:
        qWarning()<<QString("Unknown Command Code '%1'!").arg(code);
        break;

    }


}





























} //namespace HEHUI

