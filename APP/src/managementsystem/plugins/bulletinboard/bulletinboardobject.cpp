
#include <QThreadPool>
#include <QtConcurrentRun>
#include <QMessageBox>

#include "bulletinboardobject.h"


namespace HEHUI {


BulletinBoardObject::BulletinBoardObject(QObject *parent) :
    QObject(parent)
{
    
    
    resourcesManager = ResourcesManagerInstance::instance();
    bulletinBoardPacketsParser = 0;
    
    m_networkReady = false;

    m_udtProtocol = 0;
    m_socketConnectedToLocalServer = UDTProtocol::INVALID_UDT_SOCK;
//    m_socketConnectedToAdmin = UDTProtocol::INVALID_UDT_SOCK;

    startNetwork();

    
    remoteAssistance = 0;
    bulletinBoardWidget = 0;
    updatePasswordWidget = 0;
    
    localUDTListeningPort = UDT_LISTENING_PORT + 20;

    
    
}

BulletinBoardObject::~BulletinBoardObject(){

    if(bulletinBoardPacketsParser){
        //bulletinBoardPacketsParser->sendUserOfflinePacket();

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
    
    //NetworkManager::freeInstance();
    resourcesManager->cleanInstance();
    resourcesManager = 0;

    PacketHandlerBase::clean();

    
}


void BulletinBoardObject::startNetwork(){

    if(!m_udtProtocol){
        QString errorMessage = "";
        m_udtProtocol = resourcesManager->startUDTProtocol(QHostAddress::Any, localUDTListeningPort, true, &errorMessage);
        if(!m_udtProtocol){
            QString error = tr("Can not start UDT listening on port %1! %2").arg(localUDTListeningPort).arg(errorMessage);
            qCritical()<< error;
            return;
        }
        localUDTListeningPort = m_udtProtocol->getUDTListeningPort();
        connect(m_udtProtocol, SIGNAL(disconnected(int)), this, SLOT(peerDisconnected(int)));
        m_udtProtocol->startWaitingForIO(1);
    }

    connectToLocalServer();





    if(!bulletinBoardPacketsParser){
        bulletinBoardPacketsParser = new BulletinBoardPacketsParser(m_udtProtocol, this);

        connect(bulletinBoardPacketsParser, SIGNAL(signalAdminRequestRemoteAssistancePacketReceived(const QString&, quint16, const QString&)), this, SLOT(adminRequestRemoteAssistancePacketReceived(const QString&, quint16, const QString&)), Qt::QueuedConnection);
        connect(bulletinBoardPacketsParser, SIGNAL(signalAdminInformUserNewPasswordPacketReceived(const QString&, quint16, const QString&, const QString&, const QString&)), this, SLOT(AdminInformUserNewPasswordPacketReceived(const QString&, quint16, const QString&, const QString&, const QString&)), Qt::QueuedConnection);
        connect(bulletinBoardPacketsParser, SIGNAL(signalAnnouncementPacketReceived(const QString&, quint32, const QString&)), this, SLOT(serverAnnouncementPacketReceived(const QString&, quint32, const QString&)), Qt::QueuedConnection);

    }


    m_networkReady = true;


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
        connect(updatePasswordWidget, SIGNAL(newPasswordRetreved()), this, SLOT(sendNewPasswordRetrevedByUserPacket()));
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

void BulletinBoardObject::newPasswordRetreved(){

    bulletinBoardPacketsParser->sendNewPasswordRetrevedByUserPacket(m_socketConnectedToLocalServer);
}

void BulletinBoardObject::peerConnected(const QHostAddress &peerAddress, quint16 peerPort){
    qWarning()<<QString("Connected! "+peerAddress.toString()+":"+QString::number(peerPort));

}

void BulletinBoardObject::signalConnectToPeerTimeout(const QHostAddress &peerAddress, quint16 peerPort){
    qCritical()<<QString("Connecting Timeout! "+peerAddress.toString()+":"+QString::number(peerPort));

}

void BulletinBoardObject::peerDisconnected(const QHostAddress &peerAddress, quint16 peerPort, bool normalClose){
    qDebug()<<QString("Disconnected! "+peerAddress.toString()+":"+QString::number(peerPort));

    if(!normalClose){
        qCritical()<<QString("ERROR! Peer %1:%2 Closed Unexpectedly!").arg(peerAddress.toString()).arg(peerPort);
    }


}

void BulletinBoardObject::peerDisconnected(int socketID){
    qDebug()<<"--BulletinBoardObject::peerDisconnected(...) "<<"socketID:"<<socketID;

    m_udtProtocol->closeSocket(socketID);
    m_socketConnectedToLocalServer = UDTProtocol::INVALID_UDT_SOCK;

    QTimer::singleShot(60000, this, SLOT(connectToLocalServer()));


}

void BulletinBoardObject::connectToLocalServer(){

    if(m_socketConnectedToLocalServer == UDTProtocol::INVALID_UDT_SOCK){
        m_socketConnectedToLocalServer = m_udtProtocol->connectToHost(QHostAddress::LocalHost, UDT_LISTENING_PORT);
    }
    if(!m_udtProtocol->isSocketConnected(m_socketConnectedToLocalServer)){
        QTimer::singleShot(60000, this, SLOT(connectToLocalServer()));
    }else{
        qDebug()<<"m_socketConnectedToLocalServer:"<<m_socketConnectedToLocalServer;

        bool ok = bulletinBoardPacketsParser->sendUserOnlinePacket(m_socketConnectedToLocalServer);
        if(!ok){
            qCritical()<<m_udtProtocol->getLastErrorMessage();
        }

    }

}










} //namespace HEHUI
