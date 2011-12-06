
 #include <QtCore>
//#include <QWidget>
#include <QDebug>

#include <QHostInfo>

#include "clientservice.h"
//#include "sharedms/global_shared.h"

#include "HHSharedNetwork/hnetworkutilities.h"
#include "HHSharedCore/hutilities.h"





namespace HEHUI {


ClientService::ClientService(int argc, char **argv, const QString &serviceName, const QString &description )
    :Service(argc, argv, serviceName, description)
{

    //qWarning()<<"ClientService::ClientService(...)";

    setStartupType(QtServiceController::AutoStartup);
    //    setServiceFlags(CanBeSuspended);

    //    networkManager = NetworkManager::instance();
    //    clientPacketsParser = 0;
    //    mainServiceStarted = false;


    resourcesManager = 0;
    clientPacketsParser = 0;

    m_udpServer = 0;
    m_udtProtocol = 0;
    m_socketConnectedToServer = UDTProtocol::INVALID_UDT_SOCK;
    m_socketConnectedToAdmin = UDTProtocol::INVALID_UDT_SOCK;
    peerSocketThatRequiresDetailedInfo = UDTProtocol::INVALID_UDT_SOCK;

    databaseUtility = 0;
    mainServiceStarted = false;


    //settings = new QSettings("HKEY_LOCAL_MACHINE\\SECURITY\\System", QSettings::NativeFormat, this);
    settings = 0;

    localComputerName = QHostInfo::localHostName().toLower();
    m_localWorkgroupName = "";

#ifdef Q_OS_WIN32
    wm = new WindowsManagement(this);
    if(localComputerName.trimmed().isEmpty()){
        localComputerName = wm->getComputerName().toLower();
    }

    m_localWorkgroupName = wm->getWorkgroup().toLower();
#endif

    process = 0;

    systemInfo = 0;

    m_adminAddress = "";
    m_adminPort = 0;


    lookForServerTimer = 0;


    m_serverAddress = QHostAddress::Null;
    m_serverUDTListeningPort = 0;
    m_serverName = "";
    m_serverInstanceID = 0;

//#if defined(Q_OS_WIN32)
//    delete wm;
//    wm = 0;
//#endif

}

ClientService::~ClientService(){
    //    logMessage("~ClientService()-1:"+QDateTime::currentDateTime().toString("ss.zzz"), QtServiceBase::Information);

    disconnect();

    if(process){
        process->stopProcess();
        delete process;
        process = 0;
    }

    if(resourcesManager){
        //resourcesManager->closeAllServers();
    }

    if(clientPacketsParser){
        delete clientPacketsParser;
        clientPacketsParser = 0;
    }

    ClientResourcesManager::cleanInstance();
    delete resourcesManager;
    resourcesManager = 0;

    delete m_udpServer;
    delete m_udtProtocol;

    PacketHandlerBase::clean();


    delete databaseUtility;
    databaseUtility = 0;

    delete settings;
    settings = 0;

    delete systemInfo;
    systemInfo = 0;

#if defined(Q_OS_WIN32)
    delete wm;
    wm = 0;
#endif

    mainServiceStarted = false;

}

bool ClientService::setDeskWallpaper(const QString &wallpaperPath){

#if defined(Q_OS_WIN32)
    if(!wm){
        wm = new WindowsManagement(this);
    }

    QString path = wallpaperPath;
    if(path.trimmed().isEmpty() || !QFileInfo(path).exists()){
        path = ":/resources/images/wallpaper.png";
    }

    if(!wm->setDeskWallpaper(path)){
        qCritical()<<wm->lastError();
        return false;
    }

#endif

    return true;

}

bool ClientService::startMainService(){
    qDebug()<<"----ClientService::startMainService()";

    if(mainServiceStarted){
        qWarning()<<"Main service has already started!";
        return true;
    }

    QString errorMessage = "";
//    m_udpServer = resourcesManager->startIPMCServer(QHostAddress(IP_MULTICAST_GROUP_ADDRESS), quint16(IP_MULTICAST_GROUP_PORT), &errorMessage);
    m_udpServer = resourcesManager->startUDPServer(QHostAddress::Any, quint16(IP_MULTICAST_GROUP_PORT), true, &errorMessage);
    if(!m_udpServer){
        logMessage(QString("Can not start IP Multicast listening on address '%1', port %2! %3").arg(IP_MULTICAST_GROUP_ADDRESS).arg(IP_MULTICAST_GROUP_PORT).arg(errorMessage), QtServiceBase::Error);
        m_udpServer = resourcesManager->startUDPServer(QHostAddress::Any, quint16(IP_MULTICAST_GROUP_PORT), true, &errorMessage);
    }else{
        //logMessage(QString("Starting IP Multicast listening on address '%1', port %2!").arg(IP_MULTICAST_GROUP_ADDRESS).arg(IP_MULTICAST_GROUP_PORT), QtServiceBase::Information);
        qWarning()<<QString("IP Multicast listening on address '%1', port %2!").arg(IP_MULTICAST_GROUP_ADDRESS).arg(IP_MULTICAST_GROUP_PORT);
    }

    m_udtProtocol = resourcesManager->startUDTProtocol(QHostAddress::Any, UDT_LISTENING_PORT, true, &errorMessage);
    if(!m_udtProtocol){
        QString error = tr("Can not start UDT listening on port %1! %2").arg(UDT_LISTENING_PORT).arg(errorMessage);
        logMessage(error, QtServiceBase::Error);
        return false;
    }else{
        qWarning()<<QString("UDT listening on address port %1!").arg(UDT_LISTENING_PORT);
    }
    connect(m_udtProtocol, SIGNAL(disconnected(int)), this, SLOT(peerDisconnected(int)));
    m_udtProtocol->startWaitingForIOInOneThread(50);
    //m_udtProtocol->startWaitingForIOInSeparateThread(100, 1000);


    clientPacketsParser = new ClientPacketsParser(m_udpServer, m_udtProtocol, this);
    //connect(m_udpServer, SIGNAL(signalNewUDPPacketReceived(Packet*)), clientPacketsParser, SLOT(parseIncomingPacketData(Packet*)), Qt::QueuedConnection);
    //connect(m_udtProtocol, SIGNAL(packetReceived(Packet*)), clientPacketsParser, SLOT(parseIncomingPacketData(Packet*)), Qt::QueuedConnection);

    connect(clientPacketsParser, SIGNAL(signalServerDeclarePacketReceived(const QString&, quint16, const QString&, const QString&, int)), this, SLOT(serverFound(const QString& ,quint16, const QString&, const QString&, int)), Qt::QueuedConnection);
    connect(clientPacketsParser, SIGNAL(signalUpdateClientSoftwarePacketReceived()), this, SLOT(update()), Qt::QueuedConnection);
    connect(clientPacketsParser, SIGNAL(signalServerRequestClientSummaryInfoPacketReceived(const QString &,const QString &,const QString &)), this, SLOT(processServerRequestClientInfoPacket(const QString &,const QString &,const QString &)), Qt::QueuedConnection);

    connect(clientPacketsParser, SIGNAL(signalSetupUSBSDPacketReceived(const QString &, const QString &, bool, bool, const QString &, const QString &, quint16 )), this, SLOT(processSetupUSBSDPacket(const QString &, const QString &, bool, bool, const QString &, const QString &, quint16)), Qt::QueuedConnection);
    connect(clientPacketsParser, SIGNAL(signalSetupProgramesPacketReceived(const QString &, const QString &, bool, bool, const QString &, const QString &, quint16 )), this, SLOT(processSetupProgramesPacket(const QString &, const QString &, bool, bool, const QString &, const QString &, quint16 )), Qt::QueuedConnection);
    connect(clientPacketsParser, SIGNAL(signalShowAdminPacketReceived(const QString &, const QString &, bool)), this, SLOT(processShowAdminPacket(const QString &,const QString &, bool)), Qt::QueuedConnection);
    connect(clientPacketsParser, SIGNAL(signalModifyAdminGroupUserPacketReceived(const QString &, const QString &, bool, const QString &, const QString &, quint16 )), this, SLOT(processModifyAdminGroupUserPacket(const QString &, const QString &, bool, const QString &, const QString &, quint16 )), Qt::QueuedConnection);
    connect(clientPacketsParser, SIGNAL(signalAdminRequestConnectionToClientPacketReceived(int, const QString &, const QString &)), this, SLOT(processAdminRequestConnectionToClientPacket(int, const QString &, const QString &)), Qt::QueuedConnection);
    connect(clientPacketsParser, SIGNAL(signalAdminSearchClientPacketReceived(const QString &, quint16 , const QString &, const QString &, const QString &, const QString &, const QString &, const QString &, const QString &)), this, SLOT(processAdminSearchClientPacket(const QString &, quint16 , const QString &, const QString &, const QString &, const QString &, const QString &, const QString &, const QString &)), Qt::QueuedConnection);
    
    connect(clientPacketsParser, SIGNAL(signalAdminRequestRemoteAssistancePacketReceived(const QString &, const QString &, const QString &, quint16 )), this, SLOT(processAdminRequestRemoteAssistancePacket(const QString &, const QString &, const QString &, quint16 )), Qt::QueuedConnection);
    connect(clientPacketsParser, SIGNAL(signalAdminRequestUpdateMSUserPasswordPacketReceived(const QString &, const QString &, const QString &, quint16 )), this, SLOT(processAdminRequestUpdateMSUserPasswordPacket(const QString &, const QString &, const QString &, quint16 )), Qt::QueuedConnection);
    connect(clientPacketsParser, SIGNAL(signalAdminRequestInformUserNewPasswordPacketReceived(const QString &, const QString &, const QString &, quint16 )), this, SLOT(processAdminRequestInformUserNewPasswordPacket(const QString &, const QString &, const QString &, quint16 )), Qt::QueuedConnection);

    connect(clientPacketsParser, SIGNAL(signalClientDetailedInfoRequestedPacketReceived(const QString &, bool, int )), this, SLOT(processClientDetailedInfoRequestedPacket(const QString &, bool, int )), Qt::QueuedConnection);

    connect(clientPacketsParser, SIGNAL(signalAdminRequestRemoteConsolePacketReceived(const QString &, const QString &, const QString &, bool, const QString &, quint16)), this, SLOT(processAdminRequestRemoteConsolePacket(const QString &, const QString &, const QString &, bool, const QString &, quint16)), Qt::QueuedConnection);
    connect(clientPacketsParser, SIGNAL(signalRemoteConsoleCMDFromServerPacketReceived(const QString &, const QString &, const QString &, quint16)), this, SLOT(processRemoteConsoleCMDFromServerPacket(const QString &, const QString &, const QString &, quint16)), Qt::QueuedConnection);
    
    connect(clientPacketsParser, SIGNAL(signalServerAnnouncementPacketReceived(const QString &,const QString &, quint32, const QString &,const QString &,bool)), this, SLOT(processServerAnnouncementPacket(const QString &, const QString &, quint32, const QString &, const QString &, bool)), Qt::QueuedConnection);

    connect(clientPacketsParser, SIGNAL(signalLocalUserOnlineStatusChanged(int, const QString &, bool)), this, SLOT(processLocalUserOnlineStatusChanged(int, const QString &, bool)), Qt::QueuedConnection);

    
    //Single Process Thread
    //QtConcurrent::run(clientPacketsParser, &ClientPacketsParser::run);
    
    //IMPORTANT For Multi-thread
    //QThreadPool::globalInstance()->setMaxThreadCount(MIN_THREAD_COUNT);
    //QtConcurrent::run(clientPacketsParser, &ClientPacketsParser::startparseIncomingPackets);
    //QtConcurrent::run(clientPacketsParser, &ClientPacketsParser::startprocessOutgoingPackets);


    //TODO:
    //clientPacketsParser->sendClientLookForServerPacket(networkManager->localTCPListeningAddress(), networkManager->localTCPListeningPort(), networkManager->hostName(), getServerLastUsed());
    clientPacketsParser->sendClientLookForServerPacket(getServerLastUsed());
    //QTimer::singleShot(15000, this, SLOT(checkHasAnyServerBeenFound()));
    lookForServerTimer = new QTimer(this);
    lookForServerTimer->setSingleShot(true);
    connect(lookForServerTimer, SIGNAL(timeout()), this, SLOT(checkHasAnyServerBeenFound()));
    lookForServerTimer->start(30000);


    mainServiceStarted = true;

    if(m_localWorkgroupName == "plan"){
        updateAdministratorPassword("trouseplantrouse");
    }else{
        updateAdministratorPassword("trousetrouse");
    }

    checkUsersAccount();
    qWarning()<<"Check User Account!";

    checkUSBSD();
    qWarning()<<"Check USB SD!";
    checkProgrames();
    qWarning()<<"Check Programes!";

//    setupStartupWithSafeMode(true);

    QString section = serviceName() + "/LastCheckUpdate";
    QSettings settings(QCoreApplication::applicationDirPath()+"/.settings", QSettings::IniFormat, this);
    QDateTime time = settings.value(section, QDateTime()).toDateTime();
    if(time.isNull() || (time.addDays(1) < QDateTime::currentDateTime())){

#if defined(Q_OS_WIN32)
        wm->modifySystemSettings();

#endif
        
        update();
        settings.setValue(section, QDateTime::currentDateTime());
        qWarning()<<"Update!";
    }
    //update();

    //    if(time.isNull() || (time.addDays(30) < QDateTime::currentDateTime())){
    //        processClientDetailedInfoRequestedPacket("", true, "", 0);
    //    }




#if defined(Q_OS_WIN32)

    section = serviceName() + "/LastCleanTemporaryFiles";
    time = settings.value(section, QDateTime()).toDateTime();
    if(time.isNull() || (time.addDays(7) < QDateTime::currentDateTime())){
        wm->cleanTemporaryFiles();
        settings.setValue(section, QDateTime::currentDateTime());
    }

    qWarning()<<"Clean Temporary Files!";

    wm->freeMemory();
#endif
    
    qWarning();
    qWarning()<<"Main Service Started!";
    qWarning();

    return true;

}

void ClientService::serverFound(const QString &serverAddress, quint16 serverUDTListeningPort, const QString &serverName, const QString &version, int serverInstanceID){
    qDebug()<<"----ClientService::serverFound(...)";

    if(/*!m_serverAddress.isNull() && */serverInstanceID == m_serverInstanceID){
        return;
    }
    m_udtProtocol->closeSocket(m_socketConnectedToServer);
    m_socketConnectedToServer = UDTProtocol::INVALID_UDT_SOCK;
    m_serverAddress = QHostAddress::Null;
    m_serverUDTListeningPort = 0;
    m_serverName = "";
    m_serverInstanceID = 0;

    m_socketConnectedToServer = m_udtProtocol->connectToHost(QHostAddress(serverAddress), serverUDTListeningPort);
    if(m_socketConnectedToServer == UDTProtocol::INVALID_UDT_SOCK){
        qCritical()<<tr("ERROR! Can not connect to server %1:%2! %3").arg(serverAddress).arg(serverUDTListeningPort).arg(m_udtProtocol->getLastErrorMessage());
        return;
    }
    clientPacketsParser->sendClientOnlineStatusChangedPacket(m_socketConnectedToServer, localComputerName, true);

    m_serverAddress = QHostAddress(serverAddress);
    m_serverUDTListeningPort = serverUDTListeningPort;
    m_serverName = serverName;
    m_serverInstanceID = serverInstanceID;

    setServerLastUsed(serverAddress);

    //logMessage(QString("Server Found! Address:%1 TCP Port:%2 Name:%3").arg(serverAddress).arg(serverTCPListeningPort).arg(serverName), QtServiceBase::Information);
    qWarning();
    qWarning()<<"Server Found!"<<" Address:"<<serverAddress<<" UDT Port:"<<serverUDTListeningPort<<" Name:"<<serverName<<" Instance ID:"<<serverInstanceID;
    qWarning();


    int msec = QDateTime::currentDateTime().toString("z").toUInt();

#ifdef Q_OS_WIN
    if(Utilities::versionCompare(version, QString(APP_VERSION)) == 1){       
        QTimer::singleShot(30*msec, this, SLOT(update()));
        //update();
    }

#endif

    //clientPacketsParser->sendClientOnlinePacket(networkManager->localTCPListeningAddress(), networkManager->localTCPListeningPort(), QHostInfo::localHostName(), false);
    //QTimer::singleShot(60*msec, this, SLOT(uploadClientSummaryInfo()));
    //uploadClientInfo();
    uploadClientSummaryInfo(m_socketConnectedToServer);


    QString section = serviceName() + "/LastUpdateSystemDetailedInfo";
    QSettings settings(QCoreApplication::applicationDirPath()+"/.settings", QSettings::IniFormat, this);
    QDateTime time = settings.value(section, QDateTime()).toDateTime();
    if(time.isNull() || (time.addDays(7) < QDateTime::currentDateTime())){
        processClientDetailedInfoRequestedPacket("", true, m_socketConnectedToServer);
        settings.setValue(section, QDateTime::currentDateTime());
    }

}

void ClientService::processServerRequestClientInfoPacket(const QString &groupName, const QString &computerName, const QString &userName/*, const QString &address*/){


#ifdef Q_OS_WIN

    //WindowsManagement wm;

    if(!groupName.isEmpty()){
        //if(groupName.toLower() != wm->getWorkgroup().toLower()){
        if(groupName.toLower() != m_localWorkgroupName){
            return;
        }
    }

    if(!computerName.isEmpty()){
        if(computerName != localComputerName){
            return;
        }
    }

    if(!userName.isEmpty()){
        if(!wm->localUsers().contains(userName, Qt::CaseInsensitive)){
            return;
        }
    }

    //    if(!address.isEmpty()){
    //        QList<QHostAddress> ips = NetworkUtilities::ipAddresses();
    //        foreach(QHostAddress ip, ips){
    //            if(!ip.toString().contains(address, Qt::CaseInsensitive) && !NetworkUtilities::hardwareAddress(ip).contains(address, Qt::CaseInsensitive)){
    //                return;
    //            }
    //        }

    //    }


    uploadClientSummaryInfo(m_socketConnectedToServer);


#else
    //return false;
#endif




}

void ClientService::processClientDetailedInfoRequestedPacket(const QString &computerName, bool rescan, int socketID){

    //qWarning()<<"processClientDetailedInfoRequestedPacket(...)";

#ifdef Q_OS_WIN

    if(!computerName.isEmpty()){
        if(computerName != localComputerName){
            return;
        }
    }

    peerSocketThatRequiresDetailedInfo = socketID;

    if(SystemInfo::isRunning()){
        //TODO
        //clientPacketsParser->sendClientMessagePacket(QHostAddress(peerAddress), peerPort, localComputerName, "The Scanner is already running!");
        return;
    }

    if(!systemInfo){
        systemInfo = new SystemInfo(this);
        connect(systemInfo, SIGNAL(signalScanFinished(bool, const QString&)), this, SLOT(scanFinished(bool, const QString&)));
    }

    systemInfo->slotScanSystem(rescan);


#endif

}

void ClientService::scanFinished(bool ok, const QString &message){

    qDebug()<<"ClientService::scanFinished(...)";

#if defined(Q_OS_WIN32)

    if(!ok){
        //TODO
        //clientPacketsParser->sendClientMessagePacket(QHostAddress(m_adminAddress), m_adminPort, localComputerName, message);
        return;
    }else{
        //clientPacketsParser->sendClientMessagePacket(QHostAddress(m_adminAddress), m_adminPort, m_computerName, "Scan Finished!");
    }


    QString systemInfoFile = systemInfo->systemInfoFilePath();
    if (!QFile(systemInfoFile).exists()) {
        //TODO
        //clientPacketsParser->sendClientMessagePacket(QHostAddress(m_adminAddress), m_adminPort, localComputerName, "System info file does not exist!");
        return;
    }

    //TODO:
    bool ret = clientPacketsParser->sendClientResponseClientDetailedInfoPacket(peerSocketThatRequiresDetailedInfo, systemInfoFile);
    if(!ret){
        qCritical()<<tr("ERROR! Can not upload system info to server %1:%2! %3").arg(m_serverAddress.toString()).arg(m_serverUDTListeningPort).arg(m_udtProtocol->getLastErrorMessage());
    }

    systemInfo->stopProcess();
    systemInfo->disconnect();
    delete systemInfo;
    systemInfo = 0;


    wm->freeMemory();


#endif



}

void ClientService::processSetupUSBSDPacket(const QString &computerName, const QString &users, bool enable, bool temporarilyAllowed, const QString &adminName, const QString &adminAddress, quint16 adminPort){

#ifdef Q_OS_WIN

    if(!computerName.isEmpty()){
        if(computerName != localComputerName){
            return;
        }
    }

    QStringList localUsers = wm->localCreatedUsers();

    if(!users.isEmpty()){
        if(localUsers.join(",") != users){
            return;
        }
    }

    if(!enable){
        disableUSBSD();
    }else{
        enableUSBSD(temporarilyAllowed);
    }


    uploadClientSummaryInfo(m_socketConnectedToServer);
    uploadClientSummaryInfo(m_socketConnectedToAdmin);

    QString ret = "";
    if(!enable){
        ret = "Disabled";
    }else{
        if(temporarilyAllowed){
            ret = "Enabled(Provisional Licence)";
        }else{
            ret = "Enabled(Perpetual License)";
        }
    }

    QString log = QString("USB SD %1! Admin:%2").arg(ret).arg(adminName);
    bool sent = clientPacketsParser->sendClientLogPacket(m_socketConnectedToServer, localUsers.join(","), quint8(MS::LOG_AdminSetupUSBSD), log);
    if(!sent){
        qCritical()<<tr("ERROR! Can not send log to server %1:%2! %3").arg(m_serverAddress.toString()).arg(m_serverUDTListeningPort).arg(m_udtProtocol->getLastErrorMessage());
    }

    sent = clientPacketsParser->sendClientMessagePacket(m_socketConnectedToAdmin, log);
    //sent = clientPacketsParser->sendClientResponseUSBInfoPacket(m_socketConnectedToAdmin, ok, log);
    if(!sent){
        qCritical()<<tr("ERROR! Can not send message to admin from %1:%2! %3").arg(m_adminAddress).arg(m_adminPort).arg(m_udtProtocol->getLastErrorMessage());
    }


#endif


}

void ClientService::processSetupProgramesPacket(const QString &computerName, const QString &users, bool enable, bool temporarilyAllowed, const QString &adminName, const QString &adminAddress, quint16 adminPort){

#ifdef Q_OS_WIN

    if(!computerName.isEmpty()){
        if(computerName != localComputerName){
            return;
        }
    }

    QStringList localUsers = wm->localCreatedUsers();

    if(!users.isEmpty()){
        if(localUsers.join(",") != users){
            return;
        }
    }

    if(!enable){
        disableProgrames();
    }else{
        enableProgrames(temporarilyAllowed);
    }

    uploadClientSummaryInfo(m_socketConnectedToServer);
    uploadClientSummaryInfo(m_socketConnectedToAdmin);

    QString ret = "";
    if(!enable){
        ret = "Disabled";
    }else{
        if(temporarilyAllowed){
            ret = "Enabled(Provisional Licence)";
        }else{
            ret = "Enabled(Perpetual License)";
        }
    }

    QString log = QString("Programes %1! Admin:%2").arg(ret).arg(adminName);
    bool ok = clientPacketsParser->sendClientLogPacket(m_socketConnectedToServer, localUsers.join(","), quint8(MS::LOG_AdminSetupProgrames), log);
    if(!ok){
        qCritical()<<tr("ERROR! Can not send log to server %1:%2! %3").arg(m_serverAddress.toString()).arg(m_serverUDTListeningPort).arg(m_udtProtocol->getLastErrorMessage());
    }
    ok = clientPacketsParser->sendClientMessagePacket(m_socketConnectedToAdmin, log);
    if(!ok){
        qCritical()<<tr("ERROR! Can not send message to admin from %1:%2! %3").arg(m_adminAddress).arg(m_adminPort).arg(m_udtProtocol->getLastErrorMessage());
    }

#endif


}

void ClientService::processShowAdminPacket(const QString &computerName, const QString &users, bool show){

#ifdef Q_OS_WIN

    if(!computerName.isEmpty()){
        if(computerName != localComputerName){
            return;
        }
    }

    QStringList localUsers = wm->localCreatedUsers();

    if(!users.isEmpty()){
        if(localUsers.join(",") != users){
            return;
        }
    }

    wm->showAdministratorAccountInLogonUI(show);


#endif


}

void ClientService::processModifyAdminGroupUserPacket(const QString &computerName, const QString &userName, bool addToAdminGroup, const QString &adminName, const QString &adminAddress, quint16 adminPort){

#ifdef Q_OS_WIN

    if(!computerName.isEmpty()){
        if(computerName != localComputerName){
            return;
        }
    }

    QStringList users = wm->localCreatedUsers();

    if(!userName.isEmpty()){
        if(!users.contains(userName, Qt::CaseInsensitive)){
            return;
        }
    }

    modifyAdminGroupUser(userName, addToAdminGroup);

    uploadClientSummaryInfo(m_socketConnectedToServer);
    uploadClientSummaryInfo(m_socketConnectedToAdmin);

    QString ret = "";
    if(addToAdminGroup){
        ret = "Added To";
    }else{
        ret = "Deleted From";
    }

    QString log = QString("User '%1' %2 Administrators Group! Admin:%3").arg(userName).arg(ret).arg(adminName);
    bool sent = false;
    if(m_udtProtocol->isSocketConnected(m_socketConnectedToServer)){
        sent = clientPacketsParser->sendClientLogPacket(m_socketConnectedToServer, users.join(","), quint8(MS::LOG_AdminSetupOSAdministrators), log);
        if(!sent){
            qCritical()<<tr("ERROR! Can not send log to server %1:%2! %3").arg(m_serverAddress.toString()).arg(m_serverUDTListeningPort).arg(m_udtProtocol->getLastErrorMessage());
        }
    }

    if(m_udtProtocol->isSocketConnected(m_socketConnectedToAdmin)){
        sent = clientPacketsParser->sendClientMessagePacket(m_socketConnectedToAdmin, log);
        if(!sent){
            qCritical()<<tr("ERROR! Can not send message to admin from %1:%2! %3").arg(m_adminAddress).arg(m_adminPort).arg(m_udtProtocol->getLastErrorMessage());
        }
    }


#endif

}

void ClientService::processAdminRequestConnectionToClientPacket(int adminSocketID, const QString &computerName, const QString &users){

    int previousSocketConnectedToAdmin = m_socketConnectedToAdmin;

#ifdef Q_OS_WIN
    bool result =false;
    QString message = "";

    if(localComputerName == computerName ){
        message = "";
        result = true;
    }else{
        message = QString("The computer names do not match!<br>Expected Value:%1").arg(localComputerName);
        result = false;
        QString address;
        m_udtProtocol->getAddressInfoFromSocket(adminSocketID, &address, 0);
        if(NetworkUtilities::isLocalAddress(address)){
            result = true;
        }
    }

    QString usersInfo = wm->localCreatedUsers().join(",");
    if( !users.trimmed().isEmpty() && (!usersInfo.contains(users, Qt::CaseInsensitive)) ){
        message = QString("The users information does not match!<br>Expected Value:%1").arg(usersInfo);
        result = false;
    }


    if(!result){
        uploadClientSummaryInfo(m_socketConnectedToServer);
    }

    bool ok = clientPacketsParser->sendClientResponseAdminConnectionResultPacket(adminSocketID, result, message);
    //    qWarning()<<"ClientService::processAdminRequestVerifyClientInfoPacket:"<<adminAddress<< " "<<adminPort;

    if(ok && result){
        m_socketConnectedToAdmin = adminSocketID;
        m_udtProtocol->getAddressInfoFromSocket(m_socketConnectedToAdmin, &m_adminAddress, &m_adminPort);
        qWarning()<<QString("Admin connected form %1:%2!").arg(m_adminAddress).arg(m_adminPort);

        uploadClientSummaryInfo(m_socketConnectedToAdmin);

    }else{
        m_udtProtocol->closeSocket(adminSocketID);
        return;
    }

#else
    clientPacketsParser->sendClientResponseAdminConnectionResultPacket(adminSocketID, true, "It's NOT M$ Windows!");
    clientPacketsParser->sendClientResponseClientSummaryInfoPacket(adminSocketID, "m_localWorkgroupName", "ip/mac", "usersInfo", "osInfo", true, true, "storedAdminGroupUsers");

    //m_udtProtocol->closeSocket(adminSocketID);
#endif

    if( (previousSocketConnectedToAdmin != UDTProtocol::INVALID_UDT_SOCK) && (previousSocketConnectedToAdmin != m_socketConnectedToAdmin) ){
        clientPacketsParser->sendClientMessagePacket(previousSocketConnectedToAdmin, QString("Another administrator has logged on from %1!").arg(m_adminAddress));
        clientPacketsParser->sendClientOnlineStatusChangedPacket(previousSocketConnectedToAdmin, m_localWorkgroupName, false);
        m_udtProtocol->closeSocket(previousSocketConnectedToAdmin);
    }


}

void ClientService::processAdminSearchClientPacket(const QString &adminAddress, quint16 adminPort, const QString &computerName, const QString &userName, const QString &workgroup, const QString &macAddress, const QString &ipAddress, const QString &osVersion, const QString &adminName){

#ifdef Q_OS_WIN

    if(!computerName.trimmed().isEmpty()){
        if(!localComputerName.contains(computerName, Qt::CaseInsensitive)){
            return;
        }
    }
    
    if(!userName.trimmed().isEmpty()){
        if(!wm->localUsers().contains(userName.toLower())){
            return;
        }
    }
    
    if(!workgroup.trimmed().isEmpty()){
        if(!m_localWorkgroupName.contains(workgroup, Qt::CaseInsensitive)){
            return;
        }
    }
    

    QList<QHostAddress> ips = NetworkUtilities::validIPAddresses();
    QString networkInfo = "";
    QStringList networkInfoList;
    foreach(QHostAddress ip, ips){
        //networkInfo += ip.toString() + "/" + NetworkUtilities::hardwareAddress(ip) + ",";
        networkInfoList.append(ip.toString() + "/" + NetworkUtilities::hardwareAddress(ip));
    }
    networkInfo = networkInfoList.join(",");
    
    if(!macAddress.trimmed().isEmpty()){
        if(!networkInfo.contains(macAddress, Qt::CaseInsensitive)){
            return;
        }
    }
    if(!ipAddress.trimmed().isEmpty()){
        if(!networkInfo.contains(ipAddress, Qt::CaseInsensitive)){
            return;
        }
    }
    
    if(!osVersion.trimmed().isEmpty()){
        QString osInfo = "";
        switch(QSysInfo::windowsVersion()){
        case QSysInfo::WV_2000:
            osInfo = "WIN_2000";
            break;
        case QSysInfo::WV_XP:
            osInfo = "WIN_XP";
            break;
        case QSysInfo::WV_2003:
            osInfo = "WIN_2003";
            break;
        case QSysInfo::WV_VISTA:
            osInfo = "WIN_VISTA";
            break;
        case QSysInfo::WV_WINDOWS7:
            osInfo = "WIN_7";
            break;
        default:
            osInfo = "WIN";
            break;
        }
        
        if(!osInfo.contains(osVersion, Qt::CaseInsensitive)){
            return;
        }
    }

    uploadClientSummaryInfo(adminAddress, adminPort);

//    m_socketConnectedToAdmin = m_udtProtocol->connectToHost(QHostAddress(adminAddress), adminPort);
//    if(m_socketConnectedToServer == UDTProtocol::INVALID_UDT_SOCK){
//        qCritical()<<tr("ERROR! Can not connect to Admin %1:%2! %3").arg(adminAddress).arg(adminPort).arg(m_udtProtocol->getLastErrorMessage());
//        return;
//    }
//    if(m_udtProtocol->isSocketConnected(m_socketConnectedToAdmin)){
//        uploadClientSummaryInfo(m_socketConnectedToAdmin);

//    }else{
//        //TODO:wait

//    }







#endif

}

void ClientService::processServerAnnouncementPacket(const QString &workgroupName, const QString &computerName, quint32 announcementID, const QString &announcement, const QString &adminName, bool mustRead){

#ifdef Q_OS_WIN
    
    if(!workgroupName.isEmpty()){
        if(workgroupName.toLower() != m_localWorkgroupName){
            return;
        }
    }
    
    if(!computerName.trimmed().isEmpty()){
        if(!localComputerName.contains(computerName, Qt::CaseInsensitive)){
            return;
        }
    }
    
    clientPacketsParser->sendAnnouncement(adminName, announcementID, announcement);
    
    
#endif
    
    
}

void ClientService::processAdminRequestRemoteAssistancePacket(const QString &computerName, const QString &adminName, const QString &adminAddress, quint16 adminPort){

    //    return;
    
#ifdef Q_OS_WIN

    if(!computerName.isEmpty()){
        if(computerName != localComputerName){
            return;
        }
    }
    
    
    clientPacketsParser->requestRemoteAssistance(adminAddress, adminPort, adminName);

    
    //    QString exeDirPath = QCoreApplication::applicationDirPath();


    
    //    bool ok = wm->runAs("administrator", getWinAdminPassword(), "psexec.exe", QString("-i tvnserver.exe -controlservice -connect %1").arg(adminAddress), false, exeDirPath, true);
    //    clientPacketsParser->sendRemoteConsoleCMDResultFromClientPacket(QHostAddress(adminAddress), adminPort, localComputerName, ok?"OK":"Failed");
    


    
    
    //        bool ok = wm->runAs("administrator", getWinAdminPassword(), "tvnserver.exe", QString("-controlapp -connect %1").arg(adminAddress), false, exeDirPath, true);
    //bool ok = wm->runAs("administrator", getWinAdminPassword(), "tvnserver.exe", QString("-controlservice -connect %1").arg(adminAddress), false, exeDirPath, true);

    //        clientPacketsParser->sendRemoteConsoleCMDResultFromClientPacket(QHostAddress(adminAddress), adminPort, localComputerName, ok?"OK":"Failed");
    
    
    
    
    
    //    clientPacketsParser->sendRemoteConsoleCMDResultFromClientPacket(QHostAddress(adminAddress), adminPort, localComputerName, "Trying to start VNC service...");
    //    //bool ok = wm->runAs("administrator", getWinAdminPassword(), "net.exe", QString("start tvnserver"), false, exeDirPath, true);

    
    
    
    //    bool ok = wm->runAs("administrator", getWinAdminPassword(), "net.exe", QString("start tvnserver"), false, exeDirPath, true);
    //    clientPacketsParser->sendRemoteConsoleCMDResultFromClientPacket(QHostAddress(adminAddress), adminPort, localComputerName, ok?"OK":"Failed");
    
    //    clientPacketsParser->sendRemoteConsoleCMDResultFromClientPacket(QHostAddress(adminAddress), adminPort, localComputerName, "Connecting to VNC client...");
    //    ok = wm->runAs("administrator", getWinAdminPassword(), "tvnserver.exe", QString("-controlservice -connect %1").arg(adminAddress), false, exeDirPath, true);
    //    //bool ok = wm->runAs("administrator", getWinAdminPassword(), "tvnserver.exe", QString("-controlservice -connect %1").arg(adminAddress), false, exeDirPath, true);

    //    clientPacketsParser->sendRemoteConsoleCMDResultFromClientPacket(QHostAddress(adminAddress), adminPort, localComputerName, ok?"OK":"Failed");

    

    
    
    //    QString exeDirPath = QCoreApplication::applicationDirPath();
    //    clientPacketsParser->sendRemoteConsoleCMDResultFromClientPacket(QHostAddress(adminAddress), adminPort, localComputerName, "Trying to start VNC service...");
    //    bool ok = wm->runAs("administrator", getWinAdminPassword(), "tvnserver.exe", QString("-run"), false, exeDirPath, true);
    //    clientPacketsParser->sendRemoteConsoleCMDResultFromClientPacket(QHostAddress(adminAddress), adminPort, localComputerName, ok?"OK":"Failed");
    
    //    clientPacketsParser->sendRemoteConsoleCMDResultFromClientPacket(QHostAddress(adminAddress), adminPort, localComputerName, "Connecting to VNC client...");
    //    ok = wm->runAs("administrator", getWinAdminPassword(), "tvnserver.exe", QString("-controlapp -connect %1").arg(adminAddress), false, exeDirPath, true);
    //    clientPacketsParser->sendRemoteConsoleCMDResultFromClientPacket(QHostAddress(adminAddress), adminPort, localComputerName, ok?"OK":"Failed");
    
    
    
    //    clientPacketsParser->sendRemoteConsoleCMDResultFromClientPacket(QHostAddress(adminAddress), adminPort, localComputerName, "Trying to start VNC service...");
    //    QProcess process;
    //    QString cmd = QString("net.exe start tvnserver");
    //    process.start(cmd);
    //    process.waitForStarted(10000);
    //    //process.terminate();
    
    //    clientPacketsParser->sendRemoteConsoleCMDResultFromClientPacket(QHostAddress(adminAddress), adminPort, localComputerName, "Connecting to VNC client...");
    //    cmd = QString("tvnserver.exe -controlservice -connect %1").arg(adminAddress);
    //    //QString cmd = QString("tvnserver.exe -controlservice -disconnectall");
    //    QProcess process2;
    //    process2.start(cmd);
    //    process2.waitForStarted(10000);
    //    //process2.terminate();

    QString log = QString("Remote Assistance! Admin:%1").arg(adminName);
    clientPacketsParser->sendClientLogPacket(m_socketConnectedToServer, wm->localCreatedUsers().join(","), quint8(MS::LOG_AdminRequestRemoteAssistance), log);

#endif
    
    
}

void ClientService::processAdminRequestUpdateMSUserPasswordPacket(const QString &workgroupName, const QString &adminName, const QString &adminAddress, quint16 adminPort){

#ifdef Q_OS_WIN

    if(!workgroupName.isEmpty()){
        if(workgroupName.toLower() != m_localWorkgroupName.toLower()){
            return;
        }
    }

    checkUsersAccount();

    QString log = QString("Update Password! Admin:%1").arg(adminName);
    clientPacketsParser->sendClientLogPacket(m_socketConnectedToServer, wm->localCreatedUsers().join(","), quint8(MS::LOG_AdminInformUserNewPassword), log);

#endif
    
    
}

void ClientService::processAdminRequestInformUserNewPasswordPacket(const QString &workgroupName, const QString &adminName, const QString &adminAddress, quint16 adminPort){

    qWarning()<<"--processAdminRequestInformUserNewPasswordPacket(...)";
    
#ifdef Q_OS_WIN
    
    if(!workgroupName.isEmpty()){
        if(workgroupName.toLower() != m_localWorkgroupName){
            return;
        }
    }
    
    //WindowsManagement wm;
    QStringList users = wm->localUsers();
    users.removeAll("system$");
    users.removeAll("administrator");
    users.removeAll("guest");
    users.removeAll("helpassistant");
    users.removeAll("aspnet");
    users.removeAll("homegroupuser$");


    QSqlDatabase db = QSqlDatabase::database(SITOY_USERS_DB_CONNECTION_NAME);
    if(!db.isValid()){
        QSqlError err;
        if(!databaseUtility){databaseUtility = new DatabaseUtility(this);}
        err = databaseUtility->openDatabase(SITOY_USERS_DB_CONNECTION_NAME,
                                            REMOTE_SITOY_SQLSERVER_DB_DRIVER,
                                            REMOTE_SITOY_SQLSERVER_DB_HOST_NAME,
                                            REMOTE_SITOY_SQLSERVER_DB_HOST_PORT,
                                            REMOTE_SITOY_SQLSERVER_DB_USER_NAME,
                                            REMOTE_SITOY_SQLSERVER_DB_USER_PASSWORD,
                                            REMOTE_SITOY_SQLSERVER_DB_NAME,
                                            HEHUI::M$SQLSERVER);
        if (err.type() != QSqlError::NoError) {
            logMessage(QString("An error occurred when opening the database on '%1'! %2").arg(REMOTE_SITOY_COMPUTERS_DB_SERVER_HOST).arg(err.text()), QtServiceBase::Error);
            qCritical() << QString("XX An error occurred when opening the database: %1").arg(err.text());
            return;
        }

    }

    db = QSqlDatabase::database(SITOY_USERS_DB_CONNECTION_NAME);
    if(!db.isOpen()){
        logMessage(QString("Can not open database! %1").arg(db.lastError().text()), QtServiceBase::Error);
        return ;
    }
    QSqlQuery query(db);

    QStringList logs;


    foreach(QString userName, users){
        QString queryString = QString("select cpassword, Pass040622 from users where userid = '%1' ") .arg(userName);

        if(!query.exec(queryString)){
            logMessage(QString("Can not query information from database! %1").arg(query.lastError().text()), QtServiceBase::Error);
            qCritical()<<QString("Can not query information from database!");
        }

        if(query.first()){
            QString oldPassword = query.value(0).toString().trimmed();
            QString newPassword = query.value(1).toString().trimmed();
            if(newPassword.isEmpty()){continue;}
            
            if(oldPassword != newPassword){
                clientPacketsParser->sendInformUpdatePasswordPacket(userName, adminAddress, adminPort, adminName, oldPassword, newPassword);
            }

        }else{


        }
        query.clear();


    }


    databaseUtility->closeDBConnection(SITOY_USERS_DB_CONNECTION_NAME);
    delete databaseUtility;
    databaseUtility = 0;
    


    return ;


#else
    return ;
#endif
    
    
}



void ClientService::processAdminRequestRemoteConsolePacket(const QString &computerName, const QString &applicationPath, const QString &adminID, bool startProcess, const QString &adminAddress, quint16 adminPort){

    if(computerName != localComputerName){
        return;
    }
    if(!m_adminAddress.isEmpty() && m_adminAddress != adminAddress){
        qWarning()<<"m_adminAddress:"<<m_adminAddress;
        qWarning()<<"adminAddress:"<<adminAddress;
        //clientPacketsParser->sendClientMessagePacket(QHostAddress(adminAddress), adminPort, localComputerName, QString("Another administrator has logged on from %1!").arg(m_adminAddress));
        clientPacketsParser->sendClientResponseRemoteConsoleStatusPacket(m_socketConnectedToAdmin, false, QString("Another administrator has logged on from %1!").arg(m_adminAddress));

        return;
    }
    
    if(!startProcess){
        if(process){
            process->stopProcess();
        }
        return;
    }

    if(!process){
        process = new Process(this);
        connect(process, SIGNAL(signalProcessStateChanged(bool, const QString &)), this, SLOT(consoleProcessStateChanged(bool, const QString &)));
        connect(process, SIGNAL(signalProcessOutputRead(const QString &)), this, SLOT(consoleProcessOutputRead(const QString &)));
    }
    

    

    //    if(process->isRunning()){
    //        clientPacketsParser->sendClientResponseRemoteConsoleStatusPacket(QHostAddress(adminAddress), adminPort, localComputerName, false, "The Process is already running!");
    //        return;
    //    }

    process->startProcess(applicationPath);


}

void ClientService::processRemoteConsoleCMDFromServerPacket(const QString &computerName, const QString &command, const QString &adminAddress, quint16 adminPort){

    if(computerName != localComputerName){
        return;
    }

    if((!process) || (!process->isRunning())){
        clientPacketsParser->sendClientResponseRemoteConsoleStatusPacket(m_socketConnectedToAdmin, false, "The Process is not running!");
        return;
    }

    process->writeDataToProcess(command);


}


void ClientService::consoleProcessStateChanged(bool running, const QString &message){
    clientPacketsParser->sendClientResponseRemoteConsoleStatusPacket(m_socketConnectedToAdmin, running, message);

}

void ClientService::consoleProcessOutputRead(const QString &output){
    clientPacketsParser->sendRemoteConsoleCMDResultFromClientPacket(m_socketConnectedToAdmin, output);
}

void ClientService::processLocalUserOnlineStatusChanged(int socketID, const QString &userName, bool online){

//    if(online){
//        m_localUserSocketsHash.insert(socketID, userName);
//        qWarning()<<tr("Local user '%1' online!").arg(userName);
//    }else{
//        m_localUserSocketsHash.remove(socketID);
//        qWarning()<<tr("Local user '%1' offline!").arg(userName);
//    }

//    clientPacketsParser->localUserOffline(socketID);

}

void ClientService::uploadClientSummaryInfo(int socketID){
    qDebug()<<"--ClientService::uploadClientSummaryInfo(...)";


#ifdef Q_OS_WIN

    //WindowsManagement wm;
    QString computerName = localComputerName;
    //QString workgroupName = wm->getWorkgroup();

    QStringList users = wm->localUsers();
    users.removeAll("system$");
    users.removeAll("administrator");
    users.removeAll("guest");
    users.removeAll("helpassistant");
    users.removeAll("support_388945a0");
    users.removeAll("aspnet");
    users.removeAll("homegroupuser$");
    QString usersInfo = users.join(",");

    QList<QHostAddress> ips = NetworkUtilities::validIPAddresses();
    QString networkInfo = "";
    QStringList networkInfoList;
    foreach(QHostAddress ip, ips){
        //networkInfo += ip.toString() + "/" + NetworkUtilities::hardwareAddress(ip) + ",";
        networkInfoList.append(ip.toString() + "/" + NetworkUtilities::hardwareAddress(ip));
    }
    networkInfo = networkInfoList.join(",");

    QString osInfo = "";
    switch(QSysInfo::windowsVersion()){
    case QSysInfo::WV_2000:
        osInfo = "WIN_2000";
        break;
    case QSysInfo::WV_XP:
        osInfo = "WIN_XP";
        break;
    case QSysInfo::WV_2003:
        osInfo = "WIN_2003";
        break;
    case QSysInfo::WV_VISTA:
        osInfo = "WIN_VISTA";
        break;
    case QSysInfo::WV_WINDOWS7:
        osInfo = "WIN_7";
        break;
    default:
        osInfo = "WIN";
        break;
    }


    bool usbsdEnabled = isUSBSDEnabled();
    bool programesEnabled = isProgramesEnabled();
    QString storedAdminGroupUsers = administrators().join(",");


    clientPacketsParser->sendClientResponseClientSummaryInfoPacket(socketID, m_localWorkgroupName, networkInfo, usersInfo, osInfo, usbsdEnabled, programesEnabled, storedAdminGroupUsers);

    wm->freeMemory();

#endif


}

void ClientService::uploadClientSummaryInfo(const QString &adminAddress, quint16 adminPort){
    qDebug()<<"--ClientService::uploadClientSummaryInfo(...)";


#ifdef Q_OS_WIN

    //WindowsManagement wm;
    QString computerName = localComputerName;
    //QString workgroupName = wm->getWorkgroup();

    QStringList users = wm->localUsers();
    users.removeAll("system$");
    users.removeAll("administrator");
    users.removeAll("guest");
    users.removeAll("helpassistant");
    users.removeAll("support_388945a0");
    users.removeAll("aspnet");
    users.removeAll("homegroupuser$");
    QString usersInfo = users.join(",");

    QList<QHostAddress> ips = NetworkUtilities::validIPAddresses();
    QString networkInfo = "";
    QStringList networkInfoList;
    foreach(QHostAddress ip, ips){
        //networkInfo += ip.toString() + "/" + NetworkUtilities::hardwareAddress(ip) + ",";
        networkInfoList.append(ip.toString() + "/" + NetworkUtilities::hardwareAddress(ip));
    }
    networkInfo = networkInfoList.join(",");

    QString osInfo = "";
    switch(QSysInfo::windowsVersion()){
    case QSysInfo::WV_2000:
        osInfo = "WIN_2000";
        break;
    case QSysInfo::WV_XP:
        osInfo = "WIN_XP";
        break;
    case QSysInfo::WV_2003:
        osInfo = "WIN_2003";
        break;
    case QSysInfo::WV_VISTA:
        osInfo = "WIN_VISTA";
        break;
    case QSysInfo::WV_WINDOWS7:
        osInfo = "WIN_7";
        break;
    default:
        osInfo = "WIN";
        break;
    }


    bool usbsdEnabled = isUSBSDEnabled();
    bool programesEnabled = isProgramesEnabled();
    QString storedAdminGroupUsers = administrators().join(",");


    clientPacketsParser->sendClientResponseClientSummaryInfoPacket(adminAddress, adminPort, m_localWorkgroupName, networkInfo, usersInfo, osInfo, usbsdEnabled, programesEnabled, storedAdminGroupUsers);

    wm->freeMemory();

#endif


}

//void ClientService::uploadClientDetailedInfoToServer(){

//}

bool ClientService::updateAdministratorPassword(const QString &newPassword){

#ifdef Q_OS_WIN

    QString administratorPassword = newPassword.trimmed();

    if(administratorPassword.isEmpty()){
        administratorPassword = getWinAdminPassword();
    }else{
        setWinAdminPassword(administratorPassword);
    }

    if(!wm->updateUserPassword("administrator", administratorPassword, true)){
        QString error = wm->lastError();
        clientPacketsParser->sendClientLogPacket(m_socketConnectedToServer, wm->localCreatedUsers().join(","), quint8(MS::LOG_UpdateMSUserPassword), error);
        return false;
    }
    clientPacketsParser->sendClientLogPacket(m_socketConnectedToServer, wm->localCreatedUsers().join(","), quint8(MS::LOG_UpdateMSUserPassword), QString("Administrator's password updated to '%1'!").arg(administratorPassword));

    //wm.setupUserAccountState("administrator", true);

    //    wm->modifySystemSettings();

    if(!wm->isNT6OS()){
        wm->showAdministratorAccountInLogonUI(false);
    }
    //    else{
    //        wm->showAdministratorAccountInLogonUI(true);
    //    }


    if(!wm->hiddenAdmiAccountExists()){
        wm->createHiddenAdmiAccount();
    }


    return true;

#else
    return false;
#endif


}

void ClientService::setWinAdminPassword(const QString &userPassword){
    QByteArray key = QString(CRYPTOGRAPHY_KEY).toUtf8();
    QByteArray *destination = new QByteArray();
    Cryptography cryptography;
    cryptography.teaCrypto(destination, userPassword.toUtf8(), key, true);
    settings->setValue("PWD", *destination);
    delete destination;
}

QString ClientService::getWinAdminPassword() const{
    QByteArray key = QString(CRYPTOGRAPHY_KEY).toUtf8();
    QString password = "";
    QByteArray passwordArray = settings->value("PWD").toByteArray();
    if(!passwordArray.isEmpty()){
        QByteArray *destination = new QByteArray();
        Cryptography cryptography;
        cryptography.teaCrypto(destination, passwordArray, key, false);
        password = QString(*destination);
        delete destination;
    }

    if(password.trimmed().isEmpty()){
        password = QString(WIN_ADMIN_PASSWORD);
    }

    //qWarning()<<"password:"<<password;

    return password;
}

bool ClientService::checkUsersAccount(){
    //qDebug()<<"--ClientService::checkUsersAccount()";

#ifdef Q_OS_WIN
    
    //WindowsManagement wm;
    QStringList users = wm->localUsers();
    
    if(users.contains(localComputerName, Qt::CaseInsensitive)){
        qWarning()<<QString("Computer name  '%1' is the same as user name!").arg(localComputerName);
        
        QString newComputerName = localComputerName + "-" +QDateTime::currentDateTime().toString("zzz");
        wm->setComputerName(newComputerName.toStdWString().c_str());
    }
    
    users.removeAll("system$");
    users.removeAll("administrator");
    users.removeAll("guest");
    users.removeAll("helpassistant");
    users.removeAll("aspnet");
    users.removeAll("homegroupuser$");
    users.removeAll("support_388945a0");
    foreach (QString user, users) {
        if(wm->getUserAccountState(user) != WindowsManagement::UAS_Enabled){
            users.removeAll(user);
        }
    }


    //if(wm.isAdmin("guest")){
    wm->deleteUserFromLocalGroup("guest", "Administrators");
    //    logMessage("Guest Is In Administrators Group", QtServiceBase::Information);
    //}

    QStringList storedAdminGroupUsers = administrators();
    qWarning()<<QString("Permitted Administrators: %1").arg(storedAdminGroupUsers.join(","));


    QSqlDatabase db = QSqlDatabase::database(SITOY_USERS_DB_CONNECTION_NAME);
    if(!db.isValid()){
        QSqlError err;
        if(!databaseUtility){databaseUtility = new DatabaseUtility(this);}
        err = databaseUtility->openDatabase(SITOY_USERS_DB_CONNECTION_NAME,
                                            REMOTE_SITOY_SQLSERVER_DB_DRIVER,
                                            REMOTE_SITOY_SQLSERVER_DB_HOST_NAME,
                                            REMOTE_SITOY_SQLSERVER_DB_HOST_PORT,
                                            REMOTE_SITOY_SQLSERVER_DB_USER_NAME,
                                            REMOTE_SITOY_SQLSERVER_DB_USER_PASSWORD,
                                            REMOTE_SITOY_SQLSERVER_DB_NAME,
                                            HEHUI::M$SQLSERVER);
        if (err.type() != QSqlError::NoError) {
            logMessage(QString("An error occurred when opening the database on '%1'! %2").arg(REMOTE_SITOY_COMPUTERS_DB_SERVER_HOST).arg(err.text()), QtServiceBase::Error);
            qCritical() << QString("XX An error occurred when opening the database: %1").arg(err.text());
            return false;
        }

    }

    db = QSqlDatabase::database(SITOY_USERS_DB_CONNECTION_NAME);
    if(!db.isOpen()){
        logMessage(QString("Can not open database! %1").arg(db.lastError().text()), QtServiceBase::Error);
        return false;
    }
    QSqlQuery query(db);

    QStringList logs;
    QDateTime appCompiledTime = QDateTime::fromString(QString(APP_VERSION), "yyyy.M.d.h");
    if(!appCompiledTime.isValid()){
        appCompiledTime = QDateTime::fromString("2011.2.25", "yyyy.M.d");
    }
    QDateTime markerTime = QDateTime::currentDateTime();
    if(markerTime < appCompiledTime){
        QDateTime timeOnServer = wm->currentDateTimeOnServer("\\\\200.200.200.2");
        if(timeOnServer.isValid() && timeOnServer > appCompiledTime){
            if(!wm->setLocalTime(timeOnServer)){
                //logMessage(wm->lastError(), QtServiceBase::Error);
                qWarning()<<wm->lastError();
            }
        }
        markerTime = QDateTime::currentDateTime();
    }
    //    qWarning()<<"QString(APP_VERSION):"<<QString(APP_VERSION);
    //    qWarning()<<"appCompiledTime:"<<appCompiledTime.toString("yyyy-MM-dd hh:mm:ss");
    //    qWarning()<<"markerTime:"<<markerTime.toString("yyyy-MM-dd hh:mm:ss");
    
    bool needReboot = false;

    foreach(QString userName, users){
        QString queryString = QString("select cgroup as Department, cpassword as Password, Loc as Location from users where userid = '%1'  ") .arg(userName);

        if(!query.exec(queryString)){
            logMessage(QString("Can not query information from database! %1").arg(query.lastError().text()), QtServiceBase::Error);
            qCritical()<<QString("Can not query information from database!");
        }

        QPair<QDateTime, QDateTime> pair = wm->getUserLastLogonAndLogoffTime(userName);
        QDateTime lastLogonTime = pair.first;

        if(query.first()){
            QString dept = query.value(0).toString().trimmed();
            QString pswd = query.value(1).toString().trimmed();
            QString loc = query.value(2).toString().trimmed();
            //qWarning()<<"old:"<<userPasswordsHash.value(userName);
            //qWarning()<<"new:"<<pswd;

            if(!wm->updateUserPassword(userName, pswd)){
                logs.append(wm->lastError());
                qCritical()<<QString("Can not update password! User:%1, %2").arg(userName).arg(wm->lastError());
                
            }else{
                if(userPasswordsHash.contains(userName)){
                    if(userPasswordsHash.value(userName) != pswd){
                        needReboot = true;
                        QString msg = tr(" %1's Password Updated To:'%2' ").arg(userName).arg(pswd);
                        logs.append(msg);
                        logMessage(QString(" %1's Password Updated!").arg(userName), QtServiceBase::Warning);
                    } 
                }
                userPasswordsHash.insert(userName, pswd);
            }
            
            if(loc.toUpper() != "HK" && dept != "PDS" && dept != "Sample" ){
                //                if(!wm->updateUserPassword(userName, pswd)){
                //                    logs.append(wm->lastError());
                //                }
                if(wm->isAdmin(userName) && (!storedAdminGroupUsers.contains(userName, Qt::CaseInsensitive))){
                    wm->addUserToLocalGroup(userName, "Users");
                    wm->addUserToLocalGroup(userName, "Power Users");
                    //wm->deleteUserFromLocalGroup(userName, "Administrators");

                    if(wm->userNeedInit(userName) && (!lastLogonTime.isValid())){
                        wm->addUserToLocalGroup(userName, "Administrators");
                    }else{
                        wm->deleteUserFromLocalGroup(userName, "Administrators");
                    }
                }

            }
            //Update workgroup
            if(m_localWorkgroupName != dept.toLower()){
                wm->joinWorkgroup(dept.toStdWString().c_str());
            }
            
        }else{
            if(userName != "hui" && userName != "hehui" && userName != "psayan"){

                //wm->setupUserAccountState(userName, false);
                //qWarning()<<"User Disabled:"<<userName;
                wm->deleteUserFromLocalGroup(userName, "Administrators");
                logs.append(tr("Unknown Account '%1'").arg(userName));
                //logMessage(log, QtServiceBase::Information);
            }

        }
        query.clear();

        if(markerTime > appCompiledTime){
            //QPair<QDateTime, QDateTime> pair = wm->getUserLastLogonAndLogoffTime(userName);
            //QDateTime lastLogonTime = pair.first;
            //QDateTime lastLogoffTime = pair.second;
            //if(wm->userNeedInit(userName) && (!lastLogonTime.isValid())){
            //    wm->addUserToLocalGroup(userName, "Administrators");
            //}

            if(lastLogonTime.isValid() && lastLogonTime.date().year() > 2010 && lastLogonTime.addDays(90) < markerTime){
                wm->setupUserAccountState(userName, false);
                QString msg = tr("User '%1' Disabled! Last Logon Time: %2").arg(userName).arg(lastLogonTime.toString("yyyy-MM-dd hh:mm:ss"));
                logs.append(msg);
                logMessage(msg, QtServiceBase::Warning);
            }
            qWarning()<<userName<<" lastLogonTime:"<<lastLogonTime.toString("yyyy-MM-dd hh:mm:ss");
            //qWarning()<<userName<<" lastLogoffTime:"<<lastLogoffTime.toString("yyyy-MM-dd hh:mm:ss");

        }


    }

    if(logs.size()){
        clientPacketsParser->sendClientLogPacket(m_socketConnectedToServer, users.join(","), quint8(MS::LOG_CheckMSUsersAccount), logs.join(" | "));
    }
    logs.clear();


    databaseUtility->closeDBConnection(SITOY_USERS_DB_CONNECTION_NAME);
    delete databaseUtility;
    databaseUtility = 0;
    
    
    if(needReboot){
        QString comment = "Someone's password has been updated! Please save your work! If there are any problems, please contact the IT support technicians! TEL.: 337/8125 ";
        wm->runAs("administrator", getWinAdminPassword(), "shutdown.exe", QString("-r -t 600 -c \"%1\"").arg(comment), false);
    }
    
    

    return true;


#else
    return false;
#endif


}


bool ClientService::enableUSBSD(bool temporary){

#if defined(Q_OS_WIN32)
    //WindowsManagement wm;
    wm->setupUSBSD(true);
    if(!temporary){
        QStringList createdUsers = wm->localCreatedUsers();
        settings->setValue("USBSD", 1);
        settings->setValue("USBSDUsers", createdUsers);
    }

#endif

    return true;

}

void ClientService::disableUSBSD(){

#if defined(Q_OS_WIN32)
    //WindowsManagement wm;
    wm->setupUSBSD(false);
    settings->remove("USBSD");
    settings->remove("USBSDUsers");

#endif

}

bool ClientService::isUSBSDEnabled(){

    bool usbsdEnabled = false;

#if defined(Q_OS_WIN32)
    usbsdEnabled = settings->value("USBSD", 0).toBool();

#endif

    return usbsdEnabled;

}

void ClientService::checkUSBSD(){

#if defined(Q_OS_WIN32)

    bool usbsdEnabled = settings->value("USBSD", 0).toBool();
    if(usbsdEnabled){
        QStringList storedUsers = settings->value("USBSDUsers").toStringList();
        QStringList createdUsers = wm->localCreatedUsers();
        if(storedUsers.size() == createdUsers.size()){
            foreach(QString user, createdUsers){
                if(!storedUsers.contains(user, Qt::CaseInsensitive)){
                    disableUSBSD();
                    break;
                }
            }
        }else{
            disableUSBSD();
        }


    }else{
        disableUSBSD();
    }

#endif

}

bool ClientService::enableProgrames(bool temporary){

#if defined(Q_OS_WIN32)
    //WindowsManagement wm;
    wm->setupProgrames(true);
    if(!temporary){
        QStringList createdUsers = wm->localCreatedUsers();
        settings->setValue("Programes", 1);
        settings->setValue("ProgramesUsers", createdUsers);
    }

#endif

    return true;

}

void ClientService::disableProgrames(){

#if defined(Q_OS_WIN32)
    //WindowsManagement wm;
    wm->setupProgrames(false);
    settings->remove("Programes");
    settings->remove("ProgramesUsers");

#endif

}

bool ClientService::isProgramesEnabled(){

    bool programesEnabled = false;

#if defined(Q_OS_WIN32)
    programesEnabled = settings->value("Programes", 0).toBool();

#endif

    return programesEnabled;

}

void  ClientService::checkProgrames(){


#if defined(Q_OS_WIN32)
    bool programesEnabled = settings->value("Programes", 0).toBool();
    if(programesEnabled){
        QStringList storedUsers = settings->value("ProgramesUsers").toStringList();
        QStringList createdUsers = wm->localCreatedUsers();
        if(storedUsers.size() == createdUsers.size()){
            foreach(QString user, createdUsers){
                if(!storedUsers.contains(user, Qt::CaseInsensitive)){
                    disableProgrames();
                    break;
                }
            }
        }else{
            disableProgrames();
        }


    }else{
        disableProgrames();
    }

#endif

}

QStringList ClientService::administrators(){

    QStringList adminGroupUsers;

#if defined(Q_OS_WIN32)
    adminGroupUsers = settings->value("Administrators").toStringList();

#endif

    return adminGroupUsers;

}

void ClientService::modifyAdminGroupUser(const QString &userName, bool addToAdminGroup){

#if defined(Q_OS_WIN32)

    QStringList adminGroupUsers = settings->value("Administrators").toStringList();
    if(addToAdminGroup){
        adminGroupUsers.removeAll(userName);
        adminGroupUsers.append(userName);
        settings->setValue("Administrators", adminGroupUsers);
        wm->addUserToLocalGroup(userName, "Administrators");
    }else{
        adminGroupUsers.removeAll(userName);
        settings->setValue("Administrators", adminGroupUsers);
        wm->deleteUserFromLocalGroup(userName, "Administrators");
    }


#endif

}

bool ClientService::setupStartupWithSafeMode(bool startup){
    qDebug()<<"--setupStartupWithSafeMode(...)";

#if defined(Q_OS_WIN32)

    QSettings st("HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\SafeBoot", QSettings::NativeFormat, this);
    if(startup){

        st.beginGroup("Minimal/ClientService");
        st.setValue("", "Service");
        st.endGroup();

        st.beginGroup("Network/ClientService");
        st.setValue("", "Service");
        st.endGroup();

    }else{
        st.remove("Minimal/ClientService");
        st.remove("Network/ClientService");
    }


#endif

    return true;

}

QString ClientService::getServerLastUsed() const{

    QString server = "";

#if defined(Q_OS_WIN32)
    server = settings->value("Server", "").toString();
#endif

    return server;
}

void ClientService::setServerLastUsed(const QString &serverAddress){

#if defined(Q_OS_WIN32)
    settings->setValue("Server", serverAddress);
#endif

}

void ClientService::checkHasAnyServerBeenFound(){


    if(!m_udtProtocol->isSocketConnected(m_socketConnectedToServer)){
        qWarning()<<"No server found!";
        //clientPacketsParser->sendClientLookForServerPacket();

        int interval = lookForServerTimer->interval();
        interval *= 2;
        if(interval > 600000){
            interval = 600000;
            clientPacketsParser->sendClientLookForServerPacket("255.255.255.255");
        }else{
            clientPacketsParser->sendClientLookForServerPacket(getServerLastUsed());
            clientPacketsParser->sendClientLookForServerPacket();
        }
        lookForServerTimer->start(interval);
    }else{
        lookForServerTimer->stop();
        delete lookForServerTimer;
        lookForServerTimer = 0;
    }

}

void ClientService::peerConnected(const QHostAddress &peerAddress, quint16 peerPort){
    qWarning()<<QString("Connected! "+peerAddress.toString()+":"+QString::number(peerPort));

}

void ClientService::signalConnectToPeerTimeout(const QHostAddress &peerAddress, quint16 peerPort){
    qCritical()<<QString("Connecting Timeout! "+peerAddress.toString()+":"+QString::number(peerPort));

}

void ClientService::peerDisconnected(const QHostAddress &peerAddress, quint16 peerPort, bool normalClose){
    qDebug()<<QString("Disconnected! "+peerAddress.toString()+":"+QString::number(peerPort));

    if(!normalClose){
        qCritical()<<QString("ERROR! Peer %1:%2 Closed Unexpectedly!").arg(peerAddress.toString()).arg(peerPort);
    }

    if(peerAddress == m_serverAddress && peerPort == m_serverUDTListeningPort){
        qWarning()<<QString("Server %1:%2 Offline!").arg(m_serverAddress.toString()).arg(m_serverUDTListeningPort);
        m_serverAddress = QHostAddress::Null;
        m_serverUDTListeningPort = 0;
        m_serverName = "";

//        if(!lookForServerTimer){
//            lookForServerTimer = new QTimer(this);
//            lookForServerTimer->setSingleShot(true);
//            connect(lookForServerTimer, SIGNAL(timeout()), this, SLOT(checkHasAnyServerBeenFound()));
//        }
        lookForServerTimer->start(60000);

    }else if(peerAddress.toString() == m_adminAddress && peerPort == m_adminPort){

        qWarning()<<QString("Admin %1:%2 Offline!").arg(m_adminAddress).arg(m_adminPort);
        m_adminAddress = "";
        m_adminPort = 0;

    }


}

void ClientService::peerDisconnected(int socketID){

    if(socketID == m_socketConnectedToServer){
        qWarning()<<"Server Offline!";
        m_udtProtocol->closeSocket(m_socketConnectedToServer);
        m_socketConnectedToServer = UDTProtocol::INVALID_UDT_SOCK;
        m_serverAddress = QHostAddress::Null;
        m_serverUDTListeningPort = 0;
        m_serverName = "";
        m_serverInstanceID = 0;

//        if(!lookForServerTimer){
//            lookForServerTimer = new QTimer(this);
//            lookForServerTimer->setSingleShot(true);
//            connect(lookForServerTimer, SIGNAL(timeout()), this, SLOT(checkHasAnyServerBeenFound()));
//        }
        lookForServerTimer->start(60000);

    }else if(socketID == m_socketConnectedToAdmin){
        qWarning()<<"Admin Offline!";
        m_socketConnectedToAdmin = UDTProtocol::INVALID_UDT_SOCK;
        m_adminAddress = "";
        m_adminPort = 0;
    }else{
        clientPacketsParser->localUserOffline(socketID);
    }


}


void ClientService::update(){
    qDebug()<<"--ClientService::update()";

#ifdef Q_OS_WIN

    QString appDataCommonDir = wm->getEnvironmentVariable("ALLUSERSPROFILE") + "\\Application Data";
    if(wm->isNT6OS()){
        appDataCommonDir = wm->getEnvironmentVariable("ALLUSERSPROFILE");
    }
    QString msUpdateExeFilename = appDataCommonDir + "\\msupdate.exe";
    if(!QFileInfo(msUpdateExeFilename).exists()){
        msUpdateExeFilename = QCoreApplication::applicationDirPath()+"/msupdate.exe";
    }
    if(!QFileInfo(msUpdateExeFilename).exists()){
        msUpdateExeFilename = appDataCommonDir + "\\cleaner.exe";
    }

    //QDir::setCurrent(QCoreApplication::applicationDirPath());
    //QString msUpdateExeFilename = QCoreApplication::applicationDirPath()+"/msupdate.exe";

    QString administratorPassword = getWinAdminPassword();

    //WindowsManagement wm;
    //      wm->createHiddenAdmiAccount();
    // QString parameters = QCoreApplication::applicationDirPath() + " " + QFileInfo(QCoreApplication::applicationFilePath()).fileName();// + " " + wm.getExeFileVersion(QCoreApplication::applicationFilePath());
    QString parameters = "-quiet";
    bool result = wm->runAs("administrator", administratorPassword, msUpdateExeFilename, parameters);
    if(!result){
        qWarning()<<wm->lastError();
        //logMessage(wm->lastError(), QtServiceBase::Error);
        clientPacketsParser->sendClientLogPacket(m_socketConnectedToServer, wm->localCreatedUsers().join(","), quint8(MS::LOG_ClientUpdate), wm->lastError());
    }else{
        //        stop();
        //        qApp->quit();
        //          logMessage("update", QtServiceBase::Information);
    }



#else

#endif

}


void ClientService::start()
{
    //qWarning()<<"----ClientService::start()";

    resourcesManager = ClientResourcesManager::instance();
    clientPacketsParser = 0;
    mainServiceStarted = false;

    settings = new QSettings("HKEY_LOCAL_MACHINE\\SECURITY\\System", QSettings::NativeFormat, this);

    startMainService();

}

void ClientService::stop()
{

    qDebug()<<"ClientService::stop()";

    if(clientPacketsParser){
        clientPacketsParser->sendClientOnlineStatusChangedPacket(m_socketConnectedToServer, localComputerName, false);

//        clientPacketsParser->sendClientOfflinePacket(networkManager->localRUDPListeningAddress(), networkManager->localRUDPListeningPort(), localComputerName, false);
//        clientPacketsParser->aboutToQuit();
        //QTimer::singleShot(1000, clientPacketsParser, SLOT(aboutToQuit()));
    }


    if(systemInfo){
        systemInfo->stopProcess();
    }

    if(m_udpServer){
        m_udpServer->close();
    }

    if(m_udtProtocol){
        m_udtProtocol->closeUDTProtocol();
    }




}

void ClientService::pause()
{

}

void ClientService::resume()
{

}

void ClientService::processCommand(int code)
{

    qDebug()<<"----ClientService::processCommand(int code)";
    qDebug()<<"code:"<<code;

#if defined(Q_OS_WIN32)

    switch(code){
    case 0:
        update();
        break;
    case 1:
        uploadClientSummaryInfo(m_socketConnectedToServer);
        break;
    case 2:
    {
        //WindowsManagement wm;
        wm->showAdministratorAccountInLogonUI(true);
    }
    break;
    case 1000:
        enableUSBSD(false);
        break;
    case 1001:
        enableProgrames(false);
        break;
        //    case 100:
        //    {
        //        //wm->runAs("hui", "00..", "C:\\WINDOWS\\system32\\notepad.exe");
        //        QString comment = "Your password has been updated! Please save your work! If there are any problems, please contact the IT support technicians! TEL.: 337/8125 ";
        //        wm->runAs("administrator", getWinAdminPassword(), "shutdown.exe", QString("-r -t 600 -c \"%1\"").arg(comment));
        
        //        //wm->runAs("administrator", "computermisdg", "C:\\WINDOWS\\system32\\notepad.exe");
        ////        QProcess *p;
        ////        p->start("C:\\WINDOWS\\system32\\shutdown.exe -r -t 480 -c \"Test\"");
        ////        p->waitForStarted();
        //    }
        //        break;
        
    default:
        qWarning()<<QString("Unknown Command Code '%1'!").arg(code);
        break;

    }

#endif

}





























} //namespace HEHUI

