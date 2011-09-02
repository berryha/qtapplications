/*
 ****************************************************************************
 * server.cpp
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
#include "server.h"

#include "app_constants_server.h"
#include "settings.h"
#include "../sharedim/constants_global_shared.h"


namespace HEHUI {


Server::Server(QObject *parent)
    :QObject(parent)
{


    m_packetHandler = 0;
    networkManager = 0;
    serverPacketsParser = 0;
    databaseUtility = 0;
    query = 0;
    mainServiceStarted = false;

    clientInfoHash.clear();

    onlineAdminsCount = 0;

}

Server::~Server(){
    qDebug()<<"Server::~Server()";

    delete serverPacketsParser;
    serverPacketsParser = 0;

    //TODO:释放资源
    //    NetworkManager::freeInstance();
    networkManager = 0;

    m_packetHandler->clean();
    delete m_packetHandler;
    m_packetHandler = 0;


    QList<UserInfo*> clientInfoList = clientInfoHash.values();
    clientInfoHash.clear();
    foreach(UserInfo *info, clientInfoList){
        delete info;
        info = 0;
    }

    delete databaseUtility;
    databaseUtility = 0;
    delete query;
    query = 0;

    
    delete sendServerOnlinePacketTimer;
    
    mainServiceStarted = false;


}


bool Server::startMainService(){

    if(mainServiceStarted){
        qWarning()<<"Main service has already started!";
        return true;
    }

    m_packetHandler = new PacketHandlerBase(this);
    networkManager->setPacketHandler(m_packetHandler);

    //TODO
    if(openDatabase()){

    }

    bool result = false;
    result = networkManager->startIPMCServer();
    if(result == false){
        qCritical() << QString("Can not start IP Multicast listening on address '%1', port %2!").arg(IM_SERVER_IPMC_ADDRESS).arg(IM_SERVER_UDP_LISTENING_PORT);
        return false;
    }else{
        qWarning() << QString("Starting IP Multicast listening on address '%1', port %2!").arg(IM_SERVER_IPMC_ADDRESS).arg(IM_SERVER_UDP_LISTENING_PORT);
    }

    int udpPort = 0;
    udpPort = networkManager->startUDPServer();
    if(udpPort == 0){
        QString msg = tr("Can not start UDP listening!");
        //QMessageBox::critical(this, tr("Error"), msg);
        qCritical()<<msg;
        return false;
    }else{
        qWarning() << QString("Starting UDP listening on port %1!").arg(QString::number(udpPort));
    }

    result = networkManager->startTCPServer();
    if(result == false){
        qCritical() << QString("Can not start TCP listening on address '%1', port %2!").arg(networkManager->localTCPListeningAddress().toString()).arg(IM_SERVER_TCP_LISTENING_PORT);
    }else{
        qWarning() << QString("Starting TCP listening on address '%1', port %2!").arg(networkManager->localTCPListeningAddress().toString()).arg(IM_SERVER_TCP_LISTENING_PORT);
    }

    serverPacketsParser = new ServerPacketsParser(networkManager, this);
    serverPacketsParser->setLocalUDPListeningAddress(QHostAddress::Any);
    serverPacketsParser->setLocalUDPListeningPort(udpPort);
    
    //    connect(serverPacketsParser, SIGNAL(signalClientLogReceived(const QString&, const QString&, const QString&, const QString&)), this, SLOT(saveClientLog(const QString&, const QString&, const QString&, const QString&)), Qt::QueuedConnection);
    //    connect(serverPacketsParser, SIGNAL(signalHeartbeatPacketReceived(const QString &, const QString&)), this, SLOT(processHeartbeatPacket(const QString &, const QString&)), Qt::QueuedConnection);
    //    connect(serverPacketsParser, SIGNAL(signalClientOnlineStatusChanged(const QString&, quint16, const QString&, bool, bool)), this, SLOT(processClientOnlineStatusChangedPacket(const QString&, quint16, const QString&, bool, bool)), Qt::QueuedConnection);

    //Single Process Thread
    //QtConcurrent::run(serverPacketsParser, &ServerPacketsParser::run);
    //IMPORTANT For Multi-thread
    QThreadPool::globalInstance()->setMaxThreadCount(MIN_THREAD_COUNT);
    QtConcurrent::run(serverPacketsParser, &ServerPacketsParser::startparseIncomingPackets);
    QtConcurrent::run(serverPacketsParser, &ServerPacketsParser::startparseIncomingPackets);
    QtConcurrent::run(serverPacketsParser, &ServerPacketsParser::startprocessOutgoingPackets);
    serverPacketsParser->startCheckIMUsersOnlineStateTimer();


    serverPacketsParser->sendServerDeclarePacket(QHostAddress(IM_SERVER_IPMC_ADDRESS), quint16(IM_SERVER_UDP_LISTENING_PORT), networkManager->localTCPListeningAddress(), quint16(IM_SERVER_UDP_LISTENING_PORT), networkManager->localTCPListeningPort(), networkManager->hostName());
    //serverPacketsParser->sendServerDeclarePacket(QHostAddress::Broadcast, quint16(IP_MULTICAST_GROUP_PORT), networkManager->localTCPListeningAddress(), networkManager->localTCPListeningPort(), networkManager->hostName());

    sendServerOnlinePacketTimer = new QTimer(this);
    sendServerOnlinePacketTimer->setSingleShot(false);
    sendServerOnlinePacketTimer->setInterval(30*60000);
    connect(sendServerOnlinePacketTimer, SIGNAL(timeout()), this, SLOT(sendServerOnlinePacket()));
    sendServerOnlinePacketTimer->start();


    mainServiceStarted = true;

    return true;
}

void Server::sendServerOnlinePacket(){

    serverPacketsParser->sendServerOnlinePacket(networkManager->localTCPListeningAddress(), networkManager->localTCPListeningPort(), networkManager->hostName());

}

void Server::saveClientLog(const QString &computerName, const QString &users, const QString &log, const QString &clientAddress){
    //    qWarning()<<"Server::saveClientLog(...)";

    if(!query){
        if(!openDatabase()){
            return;
        }
    }else{
        query->clear();
    }

    query->prepare("INSERT INTO logs (ComputerName, Users, IPAddress, Content) "
                   "VALUES (:ComputerName, :Users, :IPAddress, :Content)");

    query->bindValue(":ComputerName", computerName);
    query->bindValue(":Users", users);
    query->bindValue(":IPAddress", clientAddress);
    query->bindValue(":Content", log);


    if(!query->exec()){
        QSqlError error = query->lastError();
        QString msg = QString("Can not write log to database! %1 Error Type:%2 Error NO.:%3").arg(error.text()).arg(error.type()).arg(error.number());
        qCritical()<< msg;

        //MySQL数据库重启，重新连接
        if(error.number() == 2006){
            query->clear();
            openDatabase(true);
        }

    }

    query->clear();

}











bool Server::openDatabase(bool reopen){

    if(reopen){       
        if(query){
            query->clear();
            delete query;
            query = 0;
        }
        databaseUtility->closeDBConnection(Server_DB_CONNECTION_NAME);
    }

    //    Settings settings(APP_NAME, APP_VERSION, SERVICE_NAME);
    //    qWarning()<<settings.getDBType();
    //    qWarning()<<settings.getDBDriver();
    //    qWarning()<<settings.getDBServerHost();
    //    qWarning()<<settings.getDBServerPort();
    //    qWarning()<<settings.getDBServerUserName();
    //    qWarning()<<settings.getDBServerUserPassword();
    //    qWarning()<<settings.getDBName();



    QSqlDatabase db = QSqlDatabase::database(Server_DB_CONNECTION_NAME);
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
        Settings settings(APP_NAME, APP_VERSION, SETTINGS_FILE_NAME, "./");
        err = databaseUtility->openDatabase(Server_DB_CONNECTION_NAME,
                                            settings.getDBDriver(),
                                            settings.getDBServerHost(),
                                            settings.getDBServerPort(),
                                            settings.getDBServerUserName(),
                                            settings.getDBServerUserPassword(),
                                            settings.getDBName(),
                                            settings.getDBType());
        if (err.type() != QSqlError::NoError) {
            qCritical() << QString("An error occurred when opening the database on '%1'! %2").arg(REMOTE_SITOY_COMPUTERS_DB_SERVER_HOST).arg(err.text());
            return false;
        }

    }

    db = QSqlDatabase::database(Server_DB_CONNECTION_NAME);
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



void Server::start()
{

    qDebug()<<"----Server::start()";


    m_packetHandler = 0;
    networkManager = ServerNetworkManager::instance();
    serverPacketsParser = 0;

    databaseUtility = new DatabaseUtility(this);

    mainServiceStarted = false;


    if(networkManager->isNetworkReady()){
        qDebug()<<"Network Ready!";
        startMainService();

    }else{
        //logMessage(QString("Can not find valid IP address! Service startup failed!"), QtServiceBase::Error);
        qWarning()<<"Can not find valid IP address! Service startup failed!";

        connect(networkManager, SIGNAL(signalNetworkReady()), this, SLOT(startMainService()));
        networkManager->startWaitingNetworkReady();

    }




}

void Server::stop()
{
    if(sendServerOnlinePacketTimer){
        sendServerOnlinePacketTimer->stop();
    }
    
    
    if(serverPacketsParser){
        serverPacketsParser->sendServerOfflinePacket(networkManager->localTCPListeningAddress(), networkManager->localTCPListeningPort(), networkManager->hostName());
        serverPacketsParser->aboutToQuit();
    }


    DatabaseUtility::closeAllDBConnections();
    


}

































} //namespace HEHUI


