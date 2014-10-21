/*
 ****************************************************************************
 * usermanagerplugin.cpp
 *
 * Created on: 2009-8-24
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
 * Last Modified on: 2010-05-19
 * Last Modified by: 贺辉
 ***************************************************************************
 */




#include "usermanagerplugin.h"

//#include "../../shared/core/user.h"
//#include "../../shared/gui/login/loginbase.h"
#include "HHSharedCore/huser.h"
#include "HHSharedGUI/hloginbase.h"


namespace HEHUI {


UserManagerPlugin::UserManagerPlugin() {
    widgetList = QList<QWidget *> ();
}

UserManagerPlugin::~UserManagerPlugin() {
    //unload();
}


/*
bool UserManagerPlugin::init( QWidget * parentWidget, QMenu *menu, QToolBar *toolBar, QSystemTrayIcon *systemTrayIcon, const QString& pName, const QString& pVersion  ){
 setParent(parentWidget);
 this->parentWidget = parentWidget;
 this->systemTrayIcon = systemTrayIcon;
 QAction *action = new QAction(icon(), name(), parentWidget);
 action->setToolTip(toolTip());
 action->setStatusTip(toolTip());
 action->setWhatsThis(whatsThis());
 connect(action, SIGNAL(triggered()), this, SLOT(slotRun()));

 if(menu){
  menu->addAction(action);
 }

 if(toolBar){
  toolBar->addAction(action);
 }

 if(systemTrayIcon){
  //TODO:
  //systemTrayMenu->addAction(action);
 }

 return true;

}


QWidget * UserManagerPlugin::parentWidgetOfPlugin(){

 return parentWidget;
}
*/


bool UserManagerPlugin::isSingle(){
    return true;
}

QString UserManagerPlugin::name () const{
    return QString(tr("User Manager"));
}

QString UserManagerPlugin::version() const{
    return "2010.05.10";
}

QString UserManagerPlugin::description() const{
    return QString(tr("User Manager For Sitoy"));
}

QIcon UserManagerPlugin::icon () const{
    return QIcon(":/resources/images/usermanager.png");
}

QString UserManagerPlugin::whatsThis () const{
    return QString(tr("User Manager"));
}

QString UserManagerPlugin::toolTip () const{
    return QString(tr("User Manager"));
}

bool UserManagerPlugin::unload(){
    qDebug("----UserManagerPlugin::unload()");

    //emit signalPluginToBeUnloaded();

    foreach(QWidget *wgt, widgetList){
        if(!wgt){continue;}
        if(wgt->close()){
            //widgetList.removeAll(wgt);
            delete wgt;
            wgt = 0;
        }
    }

    return widgetList.isEmpty();
}

void UserManagerPlugin::slotMainActionForMenuTriggered(){
    if(isSingle() && UserManagerMainWindow::isRunning()){
        //TODO: Activate the widget
        qDebug()<<"----UserManagerPlugin::slotRun():UserManager Is Running!";
        return;
    }

    QWidget *parentWidget = qobject_cast<QWidget *> (parent());

    HEHUI::User user;
    HEHUI::LoginBase login(&user, name(), parentWidget);
    if (!login.isVerified()) {
        return ;
    }

    bool isYDAdmin = false;
    if(user.getBusinessAddress() == "DG"){
        isYDAdmin = false;
    }else if(user.getBusinessAddress() == "YD"){
        isYDAdmin = true;
    }

    UserManagerMainWindow *wgt = new UserManagerMainWindow(isYDAdmin, parentWidget);
    //connect(userManager, SIGNAL(destroyed(QObject *)), this, SLOT(slotUserManagerWidgetDestoryed(QObject *)));

    QMdiArea *mdiArea = 0;
    if(parentWidget){
        mdiArea = qobject_cast<QMdiArea *>(parentWidget);
    }
    if(mdiArea){
        QMdiSubWindow *subWindow = new QMdiSubWindow;
        subWindow->setWidget(wgt);
        subWindow->setAttribute(Qt::WA_DeleteOnClose);
        mdiArea->addSubWindow(subWindow);
        //connect(this, SIGNAL(signalPluginToBeUnloaded()), subWindow, SLOT(close()));

        connect(subWindow, SIGNAL(destroyed(QObject *)), this, SLOT(slotWidgetDestoryed(QObject *)));
        widgetList.append(subWindow);
    }else{
        connect(wgt, SIGNAL(destroyed(QObject *)), this, SLOT(slotWidgetDestoryed(QObject *)));
        widgetList.append(wgt);
    }

//    if(parentWidget){
//        if(QMdiArea *mdiArea = qobject_cast<QMdiArea *>(parentWidget)){
//            QMdiSubWindow *subWindow = new QMdiSubWindow;
//            subWindow->setWidget(userManager);
//            subWindow->setAttribute(Qt::WA_DeleteOnClose);
//            mdiArea->addSubWindow(subWindow);
//            //connect(this, SIGNAL(signalPluginToBeUnloaded()), subWindow, SLOT(close()));

//            connect(subWindow, SIGNAL(destroyed(QObject *)), this, SLOT(slotUserManagerWidgetDestoryed(QObject *)));
//            widgetList.append(subWindow);
//        }else{
//            widgetList.append(userManager);
//        }
//    }else{
//        widgetList.append(userManager);
//    }

    wgt->show();

}

void UserManagerPlugin::slotWidgetDestoryed(QObject *obj){
    qDebug("--UserManagerPlugin::slotWidgetDestoryed(QObject *obj)");

    QWidget *wgt = static_cast<QWidget *> (sender());
    if(wgt){
        widgetList.removeAll(wgt);
    }

//    UserManagerMainWindow *um = static_cast<UserManagerMainWindow *> (sender());
//    if(um){
//        widgetList.removeAll(um);
//    }



}





//Q_EXPORT_PLUGIN2(userManagerPlugin, UserManagerPlugin)

} //namespace HEHUI
