/*
 ****************************************************************************
 * sqlexplorerplugin.cpp
 *
 * Created on: 2009-8-22
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






#include <QWidget>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QDebug>

#include "sqlexplorerplugin.h"


//#include "../../shared/core/user.h"
//#include "../../shared/gui/login/loginbase.h"
#include "HHSharedCore/huser.h"
#include "HHSharedGUI/hloginbase.h"

namespace HEHUI {

SqlExplorerPlugin::SqlExplorerPlugin() {
    sqlExplorerWidgetList = QList<SqlExplorer *> ();

}

SqlExplorerPlugin::~SqlExplorerPlugin() {

    unload();

}




bool SqlExplorerPlugin::isSingle(){

        return true;

}

QString SqlExplorerPlugin::name () const{
	return QString(tr("SQL Explorer"));
}

QString SqlExplorerPlugin::version() const{
    return "2010.05.10";

}

QString SqlExplorerPlugin::description() const{
    return QString(tr("SQL Explorer Plugin"));
}

QIcon SqlExplorerPlugin::icon () const{
	return QIcon(":/resources/images/console.png");

}

QString SqlExplorerPlugin::whatsThis () const{
	return QString(tr("SQL Explorer"));
}

QString SqlExplorerPlugin::toolTip () const{
	return QString(tr("SQL Explorer"));
}

bool SqlExplorerPlugin::unload(){
    qDebug("----SqlExplorerPlugin::unload()");

    emit signalPluginToBeUnloaded();

    if(sqlExplorerWidgetList.isEmpty()){
        return true;
    }

    foreach(SqlExplorer *sqlExplorer, sqlExplorerWidgetList){
        if(!sqlExplorer){break;}
        if(sqlExplorer->close()){
            sqlExplorerWidgetList.removeAll(sqlExplorer);
            delete sqlExplorer;
            sqlExplorer = 0;
        }
    }

    return sqlExplorerWidgetList.isEmpty();

}

bool SqlExplorerPlugin::verifyUser(){
    qWarning()<<"--------SqlExplorerPlugin::verifyUser()";
        return true;
}

void SqlExplorerPlugin::slotMainActionForMenuTriggered(){

	if(isSingle() && SqlExplorer::isRunning()){
		//TODO: Activate the widget
		return;
            }else{
                QString msg = tr("<p>Make sure you are familiar with database systems, SQL and the operating mechanism of this plugin!</p>");
                msg += tr("<p>It is also highly recommended that you close all other plugins first! </p>");
                msg += tr("<font color = 'red'><b>It may cause unexpected behavior!</b></font>");
                QMessageBox msgBox;
                msgBox.setWindowTitle(tr("Warning: Dangerous Operations"));
                msgBox.setIcon(QMessageBox::Warning);
                msgBox.setText(msg);

                msgBox.addButton(tr("&Continue"), QMessageBox::AcceptRole);
                QPushButton *abortButton = msgBox.addButton(tr("&Abort"), QMessageBox::RejectRole);
                msgBox.setDefaultButton(abortButton);

                msgBox.exec();
                if(msgBox.clickedButton() == abortButton){
                    return;
                }

            }

        QWidget *parentWidget = qobject_cast<QWidget *> (parent());

        HEHUI::User user;
        HEHUI::LoginBase login(&user, name(), parentWidget);
        if (!login.isVerified() ) {
		return ;
	}

	SqlExplorer *sqlExplorer = new SqlExplorer(parentWidget);
        connect(sqlExplorer, SIGNAL(destroyed(QObject *)), this, SLOT(slotSqlExplorerWidgetDestoryed(QObject *)));

	if(parentWidget){
		if(QMdiArea *mdiArea = qobject_cast<QMdiArea *>(parentWidget)){
                    QMdiSubWindow *subWindow = new QMdiSubWindow;
                         subWindow->setWidget(sqlExplorer);
                         subWindow->setAttribute(Qt::WA_DeleteOnClose);
                         mdiArea->addSubWindow(subWindow);
                         connect(this, SIGNAL(signalPluginToBeUnloaded()), subWindow, SLOT(close()));

                        //mdiArea->addSubWindow(sqlExplorer, Qt::Dialog);
		}
	}

	sqlExplorer->show();
        sqlExplorerWidgetList.append(sqlExplorer);
}

void SqlExplorerPlugin::slotSqlExplorerWidgetDestoryed(QObject * obj){
    qDebug("----slotSqlExplorerWidgetClosed(QObject * obj )");

    SqlExplorer *sqlExplorer = static_cast<SqlExplorer *> (sender());
    if(sqlExplorer){
        sqlExplorerWidgetList.removeAll(sqlExplorer);
    }


}



//Q_EXPORT_PLUGIN2(sqlExplorerPlugin, SqlExplorerPlugin)


} //namespace HEHUI {
