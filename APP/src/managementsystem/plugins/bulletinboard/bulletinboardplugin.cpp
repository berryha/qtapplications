/*
 ****************************************************************************
 * bulletinboardplugin.cpp
 *
 * Created on: 2010-07-13
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
 * Last Modified on: 2010-07-13
 * Last Modified by: 贺辉
 ***************************************************************************
 */




#include "bulletinboardplugin.h"


//#include "../../shared/core/user.h"
//#include "../../shared/gui/login/loginbase.h"
#include "HHSharedCore/huser.h"
#include "HHSharedGUI/hloginbase.h"


namespace HEHUI {

BulletinBoardPlugin::BulletinBoardPlugin() {
    

    systemSummaryInfoWidgetList = QList<SystemSummaryInfo *> ();

    actionMain = 0;
    //QTimer::singleShot(2000, this, SLOT(slotRun()));


    bulletinBoardObject = new BulletinBoardObject(this);

}

BulletinBoardPlugin::~BulletinBoardPlugin() {
    
    //unload();
    
    
    //    delete bulletinBoardPacketsParser;
    //    bulletinBoardPacketsParser = 0;
    
    //    NetworkManager::freeInstance();
    //    networkManager = 0;
    
    //qWarning()<<"BulletinBoardPlugin::~BulletinBoardPlugin()";

}

bool BulletinBoardPlugin::isSingle(){

    return true;

}

QString BulletinBoardPlugin::name () const{
    return QString(tr("Bulletin Board"));
}

QString BulletinBoardPlugin::version() const{
    return "2010.05.10";
}

QString BulletinBoardPlugin::description() const{
    return QString(tr("Bulletin Board Plugin"));
}

QIcon BulletinBoardPlugin::icon () const{
    return QIcon(":/icon/resources/images/bulletinboard.png");

}

QString BulletinBoardPlugin::whatsThis () const{
    return QString(tr("Bulletin Board"));
}

QString BulletinBoardPlugin::toolTip () const{
    return QString(tr("Bulletin Board"));
}


void BulletinBoardPlugin::retranslateUi(){


    if(actionMain){
        actionMain->setText(tr("System Summary Info"));
    }
}


QAction * BulletinBoardPlugin::mainActionForMenu(){

    if(!actionMain){
        actionMain = new QAction(this);
        actionMain->setIcon(QIcon(":/icon/resources/images/systemsummaryinfo.png"));
        actionMain->setText(tr("System Summary Info"));
        actionMain->setToolTip(tr("System Summary Info"));
        actionMain->setStatusTip(tr("System Summary Info"));
        actionMain->setWhatsThis(tr("System Summary Info"));
        connect(actionMain, SIGNAL(triggered()), this, SLOT(slotMainActionForMenuTriggered()));
    }

    return actionMain;

}

bool BulletinBoardPlugin::unload(){
    qDebug()<<"----BulletinBoardPlugin::unload()";
    
    //    if(bulletinBoardPacketsParser){
    //        bulletinBoardPacketsParser->sendUserOfflinePacket();
    //        bulletinBoardPacketsParser->aboutToQuit();
    //    }
    
    //    if(remoteAssistance){
    //        remoteAssistance->close();
    //        delete remoteAssistance;
    //    }
    //    remoteAssistance = 0;
    
    //    if(bulletinBoardWidget){
    //        bulletinBoardWidget->close();
    //        delete bulletinBoardWidget;
    //    }
    //    remoteAssistance = 0;
    
    //    if(updatePasswordWidget){
    //        updatePasswordWidget->close();
    //        delete updatePasswordWidget;
    //    }
    //    remoteAssistance = 0;
    
    emit signalPluginToBeUnloaded();

    delete bulletinBoardObject;
    bulletinBoardObject = 0;


    if(systemSummaryInfoWidgetList.isEmpty()){
        return true;
    }

    foreach(SystemSummaryInfo *systemSummaryInfo, systemSummaryInfoWidgetList){
        if(!systemSummaryInfo){continue;}
        if(systemSummaryInfo->close()){
            systemSummaryInfoWidgetList.removeAll(systemSummaryInfo);
            delete systemSummaryInfo;
            systemSummaryInfo = 0;
        }
    }

    return systemSummaryInfoWidgetList.isEmpty();

}

void BulletinBoardPlugin::slotMainActionForMenuTriggered(){
    if(isSingle() && SystemSummaryInfo::isRunning()){
        //TODO: Activate the widget
        return;
    }

    QWidget *parentWidget = qobject_cast<QWidget *> (parent());


    SystemSummaryInfo *systemSummaryInfo = new SystemSummaryInfo(parentWidget);
    connect(systemSummaryInfo, SIGNAL(destroyed(QObject *)), SLOT(sloSystemSummaryInfoWidgetDestoryed(QObject *)));

    if(parentWidget){
        if(QMdiArea *mdiArea = qobject_cast<QMdiArea *>(parentWidget)){
            QMdiSubWindow *subWindow = new QMdiSubWindow;
            subWindow->setWidget(systemSummaryInfo);
            subWindow->setAttribute(Qt::WA_DeleteOnClose);
            mdiArea->addSubWindow(subWindow);
            connect(this, SIGNAL(signalPluginToBeUnloaded()), subWindow, SLOT(close()));

            //mdiArea->addSubWindow(sqlExplorer, Qt::Dialog);
        }
    }

    systemSummaryInfo->show();
    systemSummaryInfoWidgetList.append(systemSummaryInfo);
}

void BulletinBoardPlugin::sloSystemSummaryInfoWidgetDestoryed(QObject * obj){
    qDebug("----ControlCenterPlugin::sloSystemSummaryInfoWidgetDestoryed(QObject * obj)");

    SystemSummaryInfo *controlCenter = static_cast<SystemSummaryInfo *> (sender());
    if(controlCenter){
        systemSummaryInfoWidgetList.removeAll(controlCenter);
    }

}





//void BulletinBoardPlugin::networkReady(){

//    disconnect(networkManager, SIGNAL(signalNetworkReady()), this, SLOT(networkReady()));

//    bool result = false;
//    result = networkManager->startUDPServer();
//    if(result == false){
//        QString msg = tr("Can not start UDP listening!");
//        //QMessageBox::critical(this, tr("Error"), msg);
//        qCritical()<<msg;
//        return;
//    }else{
//        if(!bulletinBoardPacketsParser){
//            bulletinBoardPacketsParser = new BulletinBoardPacketsParser();        
//        }

//        //connect(bulletinBoardPacketsParser, SIGNAL(signalAdminRequestRemoteAssistancePacketReceived(const QString&, quint16, const QString&)), this, SLOT(adminRequestRemoteAssistancePacketReceived(const QString&, quint16, const QString&)), Qt::QueuedConnection);
//        //connect(bulletinBoardPacketsParser, SIGNAL(signalAdminInformUserNewPasswordPacketReceived(const QString&, quint16, const QString&, const QString&, const QString&)), this, SLOT(AdminInformUserNewPasswordPacketReceived(const QString&, quint16, const QString&, const QString&, const QString&)), Qt::QueuedConnection);
//        connect(bulletinBoardPacketsParser, SIGNAL(signalAnnouncementPacketReceived(const QString&, const QString&)), this, SLOT(serverAnnouncementPacketReceived(const QString&, const QString&)), Qt::QueuedConnection);

//        connect(bulletinBoardPacketsParser, SIGNAL(signalT(const QString&)), this, SLOT(test(const QString&)));



//        //IMPORTANT For Multi-thread
//        QThreadPool::globalInstance()->setMaxThreadCount(MIN_THREAD_COUNT);
//        QtConcurrent::run(bulletinBoardPacketsParser, &BulletinBoardPacketsParser::run);


//        bulletinBoardPacketsParser->sendUserOnlinePacket();

//        m_networkReady = true;

//        bulletinBoardPacketsParser->startHeartbeat();

//        qWarning()<<QString("UDP listening on port %1!").arg(networkManager->localUDPListeningPort());


//    }



//}

//void BulletinBoardPlugin::adminRequestRemoteAssistancePacketReceived(const QString &adminAddress, quint16 adminPort, const QString &adminName ){

//    if(!remoteAssistance){
//        remoteAssistance = new RemoteAssistance(adminAddress, adminPort, adminName);
//        connect(remoteAssistance, SIGNAL(signalUserResponseRemoteAssistance(const QString &, quint16, bool)), bulletinBoardPacketsParser, SLOT(sendUserResponseRemoteAssistancePacket(const QString &, quint16, bool )));
//    }else {
//        remoteAssistance->requestRemoteAssistance(adminAddress, adminPort, adminName);
//    }

//    remoteAssistance->show();




//}

//void BulletinBoardPlugin::AdminInformUserNewPasswordPacketReceived(const QString &adminAddress, quint16 adminPort, const QString &adminName, const QString &oldPassword, const QString &newPassword ){

//    qWarning()<<"BulletinBoardPlugin::AdminInformUserNewPasswordPacketReceived(...)";

//    if(!updatePasswordWidget){
//        updatePasswordWidget = new UpdatePasswordWidget(adminAddress, adminPort, adminName, oldPassword, newPassword);
//        connect(updatePasswordWidget, SIGNAL(newPasswordRetreved(const QString&,quint16)), bulletinBoardPacketsParser, SLOT(sendNewPasswordRetrevedByUserPacket(const QString&,quint16)));
//    }else {
//        updatePasswordWidget->informNewPassword(adminAddress, adminPort, adminName, oldPassword, newPassword);
//    }

//    updatePasswordWidget->show();

//}

//void BulletinBoardPlugin::serverAnnouncementPacketReceived(const QString &adminName, const QString &announcement){

//    qWarning()<<"BulletinBoardPlugin::serverAnnouncementPacketReceived(...)";

//    if(!bulletinBoardWidget){
//        bulletinBoardWidget = new BulletinBoardWidget(adminName, announcement);
//    }else{
//        bulletinBoardWidget->showServerAnnouncement(adminName, announcement);
//    }

//    bulletinBoardWidget->show();

//}

//void BulletinBoardPlugin::test(const QString &msg){
//    qWarning()<<"BulletinBoardPlugin::test(...)"<<" msg:"<<msg;
//}



































//Q_EXPORT_PLUGIN2(bulletinBoardPlugin, BulletinBoardPlugin)


} //namespace HEHUI



