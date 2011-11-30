/*
 ****************************************************************************
 * controlcenterplugin.cpp
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




#include "controlcenterplugin.h"


//#include "../../shared/core/user.h"
//#include "../../shared/gui/login/loginbase.h"
#include "HHSharedCore/huser.h"
#include "HHSharedGUI/hloginbase.h"


namespace HEHUI {


ControlCenterPlugin::ControlCenterPlugin() {

    controlCenterWidgetList = QList<ControlCenter *> ();

}

ControlCenterPlugin::~ControlCenterPlugin() {
    //unload();
}


/*
bool SystemInfoPlugin::init( QWidget * parentWidget, QMenu *menu, QToolBar *toolBar, QSystemTrayIcon *systemTrayIcon, const QString& pName, const QString& pVersion  ){
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


QWidget * SystemInfoPlugin::parentWidgetOfPlugin(){

    return parentWidget;
}

*/

bool ControlCenterPlugin::isSingle(){

    return true;

}

QString ControlCenterPlugin::name () const{
    return QString(tr("Control Center"));
}

QString ControlCenterPlugin::version() const{
    return "2010.05.10";
}

QString ControlCenterPlugin::description() const{
    return QString(tr("Control Center Plugin"));
}

QIcon ControlCenterPlugin::icon () const{
    return QIcon(":/icon/resources/images/controlcenter.png");

}

QString ControlCenterPlugin::whatsThis () const{
    return QString(tr("Control Center"));
}

QString ControlCenterPlugin::toolTip () const{
    return QString(tr("Control Center"));
}

bool ControlCenterPlugin::unload(){
    qDebug("----ControlCenterPlugin::unload()");

    emit signalPluginToBeUnloaded();

    if(controlCenterWidgetList.isEmpty()){
        return true;
    }

    foreach(ControlCenter *controlCenter, controlCenterWidgetList){
        if(!controlCenter){break;}
        if(controlCenter->close()){
            controlCenterWidgetList.removeAll(controlCenter);
            delete controlCenter;
            controlCenter = 0;
        }
    }

    return controlCenterWidgetList.isEmpty();

}

void ControlCenterPlugin::slotMainActionForMenuTriggered(){
    if(isSingle() && ControlCenter::isRunning()){
        //TODO: Activate the widget
        return;
    }

    QWidget *parentWidget = qobject_cast<QWidget *> (parent());

    HEHUI::User user;
    HEHUI::LoginBase login(&user, name(), parentWidget);
    if (!login.isVerified()) {
        return ;
    }

    ControlCenter *controlCenter = new ControlCenter(user.getUserID(), parentWidget);
    connect(controlCenter, SIGNAL(destroyed(QObject *)), SLOT(slotControlCenterWidgetDestoryed(QObject *)));

    if(parentWidget){
        if(QMdiArea *mdiArea = qobject_cast<QMdiArea *>(parentWidget)){
            QMdiSubWindow *subWindow = new QMdiSubWindow;
            subWindow->setWidget(controlCenter);
            subWindow->setAttribute(Qt::WA_DeleteOnClose);
            mdiArea->addSubWindow(subWindow);
            connect(this, SIGNAL(signalPluginToBeUnloaded()), subWindow, SLOT(close()));

            //mdiArea->addSubWindow(sqlExplorer, Qt::Dialog);

        }
    }

    controlCenter->show();
    controlCenterWidgetList.append(controlCenter);
}

void ControlCenterPlugin::slotControlCenterWidgetDestoryed(QObject * obj){
    qDebug("----ControlCenterPlugin::slotControlCenterWidgetDestoryed(QObject * obj)");

    ControlCenter *controlCenter = static_cast<ControlCenter *> (sender());
    if(controlCenter){
        controlCenterWidgetList.removeAll(controlCenter);
    }

}


Q_EXPORT_PLUGIN2(controlCenterPlugin, ControlCenterPlugin)


} //namespace HEHUI
