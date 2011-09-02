
#include <QThreadPool>
#include <QtConcurrentRun>

#include "bulletinboardobject.h"


namespace HEHUI {


BulletinBoardObject::BulletinBoardObject(QObject *parent) :
    QObject(parent)
{
    
    
    m_packetHandler = 0;
    networkManager = NetworkManagerInstance::instance();
    bulletinBoardPacketsParser = 0;
    
    m_networkReady = false;

    if(networkManager->isNetworkReady()){
        qWarning()<<"Network Ready!";
        networkReady();

    }else{
        qWarning()<<"Can not find valid IP address! Service startup failed!";

        connect(networkManager, SIGNAL(signalNetworkReady()), this, SLOT(networkReady()));
        networkManager->startWaitingNetworkReady();
    }
    
    remoteAssistance = 0;
    bulletinBoardWidget = 0;
    updatePasswordWidget = 0;
    
    localUDPListeningPort = 0;
    
    
}

BulletinBoardObject::~BulletinBoardObject(){

    if(bulletinBoardPacketsParser){
        bulletinBoardPacketsParser->sendUserOfflinePacket();
        bulletinBoardPacketsParser->aboutToQuit();
    }
    
    if(remoteAssistance){
        remoteAssistance->close();
        delete remoteAssistance;
    }
    remoteAssistance = 0;
    
    if(bulletinBoardWidget){
        bulletinBoardWidget->close();
        delete bulletinBoardWidget;
    }
    remoteAssistance = 0;
    
    if(updatePasswordWidget){
        updatePasswordWidget->close();
        delete updatePasswordWidget;
    }
    remoteAssistance = 0;
    
    
    
    delete bulletinBoardPacketsParser;
    bulletinBoardPacketsParser = 0;
    
    networkManager->closeUDPServer(localUDPListeningPort);
    //    NetworkManager::freeInstance();
    networkManager = 0;

    m_packetHandler->clean();
    delete m_packetHandler;
    m_packetHandler = 0;
    
}


void BulletinBoardObject::networkReady(){

    disconnect(networkManager, SIGNAL(signalNetworkReady()), this, SLOT(networkReady()));

    if(m_networkReady){
        return;
    }

    m_packetHandler = new PacketHandlerBase(this);
    networkManager->setPacketHandler(m_packetHandler);

    int port = 0;
    port = networkManager->startUDPServer(QHostAddress::Any, (IP_MULTICAST_GROUP_PORT+10));
    if(port == 0){
        QString msg = tr("Can not start UDP listening!");
        //QMessageBox::critical(this, tr("Error"), msg);
        qCritical()<<msg;
        return;
    }else{
        if(!bulletinBoardPacketsParser){
            bulletinBoardPacketsParser = new BulletinBoardPacketsParser(networkManager, this);
            bulletinBoardPacketsParser->setLocalUDPListeningAddress(QHostAddress::Any);
            bulletinBoardPacketsParser->setLocalUDPListeningPort(port);
            localUDPListeningPort = port;
            
        }
        
        connect(bulletinBoardPacketsParser, SIGNAL(signalAdminRequestRemoteAssistancePacketReceived(const QString&, quint16, const QString&)), this, SLOT(adminRequestRemoteAssistancePacketReceived(const QString&, quint16, const QString&)), Qt::QueuedConnection);
        connect(bulletinBoardPacketsParser, SIGNAL(signalAdminInformUserNewPasswordPacketReceived(const QString&, quint16, const QString&, const QString&, const QString&)), this, SLOT(AdminInformUserNewPasswordPacketReceived(const QString&, quint16, const QString&, const QString&, const QString&)), Qt::QueuedConnection);
        connect(bulletinBoardPacketsParser, SIGNAL(signalAnnouncementPacketReceived(const QString&, quint32, const QString&)), this, SLOT(serverAnnouncementPacketReceived(const QString&, quint32, const QString&)), Qt::QueuedConnection);
        
        
        
        
        //IMPORTANT For Multi-thread
        QThreadPool::globalInstance()->setMaxThreadCount(MIN_THREAD_COUNT);
        QtConcurrent::run(bulletinBoardPacketsParser, &BulletinBoardPacketsParser::run);
        
        
        bulletinBoardPacketsParser->sendUserOnlinePacket();
        
        m_networkReady = true;
        
        bulletinBoardPacketsParser->startHeartbeat();

        qWarning()<<QString("UDP listening on port %1! (BulletinBoardObject)").arg(port);


    }



}

void BulletinBoardObject::adminRequestRemoteAssistancePacketReceived(const QString &adminAddress, quint16 adminPort, const QString &adminName ){

    if(!remoteAssistance){
        remoteAssistance = new RemoteAssistance(adminAddress, adminPort, adminName);
        connect(remoteAssistance, SIGNAL(signalUserResponseRemoteAssistance(const QString &, quint16, bool)), bulletinBoardPacketsParser, SLOT(sendUserResponseRemoteAssistancePacket(const QString &, quint16, bool )));
    }else {
        remoteAssistance->requestRemoteAssistance(adminAddress, adminPort, adminName);
    }
    
    remoteAssistance->show();
    remoteAssistance->raise();
    
    
    
    
}

void BulletinBoardObject::AdminInformUserNewPasswordPacketReceived(const QString &adminAddress, quint16 adminPort, const QString &adminName, const QString &oldPassword, const QString &newPassword ){

    qWarning()<<"BulletinBoardPlugin::AdminInformUserNewPasswordPacketReceived(...)";
    
    if(!updatePasswordWidget){
        updatePasswordWidget = new UpdatePasswordWidget(adminAddress, adminPort, adminName, oldPassword, newPassword);
        connect(updatePasswordWidget, SIGNAL(newPasswordRetreved(const QString&,quint16)), this, SLOT(sendNewPasswordRetrevedByUserPacket(const QString&, quint16)));
    }else {
        updatePasswordWidget->informNewPassword(adminAddress, adminPort, adminName, oldPassword, newPassword);
    }
    
    //updatePasswordWidget->show();
    
}

void BulletinBoardObject::serverAnnouncementPacketReceived(const QString &adminName, quint32 announcementID, const QString &announcement){

    qWarning()<<"BulletinBoardPlugin::serverAnnouncementPacketReceived(...)";
    
    if(!bulletinBoardWidget){
        bulletinBoardWidget = new BulletinBoardWidget(adminName, announcementID, announcement);
    }else{
        bulletinBoardWidget->showServerAnnouncement(adminName, announcementID, announcement);
    }
    
    bulletinBoardWidget->show();
    bulletinBoardWidget->raise();

}

void BulletinBoardObject::newPasswordRetreved(const QString &adminAddress, quint16 adminPort){

    bulletinBoardPacketsParser->sendNewPasswordRetrevedByUserPacket(adminAddress, adminPort);
}












} //namespace HEHUI
