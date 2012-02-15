/*
 ****************************************************************************
 * mainwindow.cpp
 *
 * Created on: 2009-4-27
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
  * Last Modified on: 2010-05-17
  * Last Modified by: 贺辉
  ***************************************************************************
*/




#include <QtGui>

#include "mainwindow.h"
#include "../about/aboutdialog.h"
#include "../shared/settings.h"

//#include "../../shared/core/database/databaseutility.h"
//#include "../../shared/gui/login/loginbase.h"
//#include "../../shared/gui/plugin/guiinterface.h"
//#include "../../shared/gui/plugin/pluginmanager/pluginmanagerwindow.h"

#include "HHSharedCore/hdatabaseutility.h"
#include "HHSharedGUI/hloginbase.h"
#include "HHSharedGUI/hguiinterface.h"
#include "HHSharedGUI/hpluginmanagerwindow.h"
#include "HHSharedCore/hutilities.h"



namespace HEHUI {


MainWindow::MainWindow(QWidget *parent, HEHUI::WindowPosition positon) :
    MainWindowBase(parent) {
    ui.setupUi(this);

    setWindowTitle(QString(APP_NAME) + " Build " + QString(APP_VERSION));
    //setWindowFlags(Qt::SplashScreen | Qt::WindowStaysOnTopHint);
    //setWindowFlags(Qt::WindowStaysOnTopHint);

    //使窗口居中
    //Center the window
    //moveWindow(positon);

    //resize(QSize(0,0));
    //showMinimized();
    //showMaximized();

    //应用样式
    //Apply the app style
    //	originalPalette = QApplication::palette();
    //	changeStyle(Settings::instance()->getStyle());


    //初始化UI
    //Init the UI
    initUI();

    //创建托盘图标
    //Create the system tray
    setupSystemTray();

    if(Settings::instance()->getRestoreWindowStateOnStartup()){
        Settings::instance()->restoreState(this);
    }else{
        //使窗口居中
        //Center the window
        moveWindow(positon);
        showMaximized();
    }


    //        qmLocale = Settings::instance()->getLanguage();
    //        qmPath = QApplication::applicationDirPath() + QDir::separator () + QString(LANGUAGE_FILE_DIR);
    //更新菜单
    //Update the menus
    //setupMenus();



}

MainWindow::~MainWindow() {
    qDebug()<<"--MainWindow::~MainWindow()";

    if(Settings::instance()->getRestoreWindowStateOnStartup()){
        Settings::instance()->saveState(this);
    }

    //Close all database connections
    DatabaseUtility::closeAllDBConnections();
}

void MainWindow::closeEvent(QCloseEvent * event) {
    //Close all subwindows
    ui.mdiArea->closeAllSubWindows();
    if(ui.mdiArea->activeSubWindow()){
        event->ignore();
        return;
    }

    unloadPlugins();

    event->accept();
}


void MainWindow::changeEvent ( QEvent * event ){
    if(event->type() == QEvent::WindowStateChange){
        switch (windowState ()) {
        case Qt::WindowNoState:
            ui.actionShowMaximized->setEnabled(true);
            ui.actionShowFullScreen->setEnabled(true);
            ui.actionShowMinimized->setEnabled(true);
            ui.actionShowNormal->setEnabled(false);
            break;
        case Qt::WindowMinimized:
            ui.actionShowMaximized->setEnabled(true);
            ui.actionShowFullScreen->setEnabled(true);
            ui.actionShowMinimized->setEnabled(false);
            ui.actionShowNormal->setEnabled(true);
            break;
        case Qt::WindowMaximized:
            ui.actionShowMaximized->setEnabled(false);
            ui.actionShowFullScreen->setEnabled(true);
            ui.actionShowMinimized->setEnabled(true);
            ui.actionShowNormal->setEnabled(true);
            break;
        case Qt::WindowFullScreen:
            ui.actionShowMaximized->setEnabled(false);
            ui.actionShowFullScreen->setEnabled(false);
            ui.actionShowMinimized->setEnabled(true);
            ui.actionShowNormal->setEnabled(true);
            break;
        default:
            //TODO: Other State
            break;
        }

        event->accept();
    }

    event->ignore();

}

void MainWindow::initUI(){

    ui.menuView->addSeparator();
    ui.menuView->addMenu(getStyleMenu(Settings::instance()->getStyle(), Settings::instance()->getPalette()));
    QString qmLocale = Settings::instance()->getLanguage();
    QString qmPath = QApplication::applicationDirPath() + QDir::separator () + QString(LANGUAGE_FILE_DIR);
    ui.menuView->addMenu(getLanguageMenu(qmPath, qmLocale));

    pluginsMenu = getPluginsMenu();
    menuBar()->insertMenu(ui.menuHelp->menuAction(), pluginsMenu);
    ui.toolBarPlugins->addAction(getPluginsManagementAction());


    connect(ui.actionBugReport, SIGNAL(triggered()), this, SLOT(slotBugReport()));

    connect(ui.actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    connect(ui.actionAbout, SIGNAL(triggered()), this, SLOT(slotAbout()));

    connect(ui.actionHelp, SIGNAL(triggered()), this, SLOT(slotHelp()));


    connect(ui.actionSystemConfigure, SIGNAL(triggered()), this, SLOT(slotSystemConfig()));

    connect(ui.actionQuit, SIGNAL(triggered()), this, SLOT(slotQuit()));


    //connect(ui.menuSubWindows, SIGNAL(aboutToShow()), this, SLOT(slotUpdateWindowActions()));
    connect(ui.mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow *)), this, SLOT(slotUpdateWindowActions()));





}

void MainWindow::slotInitPlugin(AbstractPluginInterface *plugin){
    qDebug("----MainWindow::slotInitPlugin(AbstractPluginInterface *plugin)");

    if(!plugin){
        return;
    }

    GUIInterface *guiPlugin = static_cast<GUIInterface *> (plugin);
    if(guiPlugin){
        guiPlugin->initialize(ui.mdiArea, pluginsMenu, ui.toolBarPlugins, systemTray, APP_NAME, APP_VERSION);
        pluginsMenu->addMenu(guiPlugin->menu());
        ui.toolBarPlugins->addAction(guiPlugin->menu()->menuAction());
    }

    //	int pluginsCount = guiPluginsList().size();
    //	if(!pluginsCount){
    //		return;
    //	}
    //
    //        QToolBar *pluginsToolBar = new QToolBar(tr("Plugins"), this);
    //        insertToolBar(ui.toolBarHelp, pluginsToolBar);
    //
    //	for(int i = 0; i< pluginsCount; i++){
    //                GUIInterface *plugin = guiPluginsList().at(i);
    //                if(plugin){
    //                    plugin->initialize(ui.mdiArea, ui.menuPlugins, pluginsToolBar, systemTray, APP_NAME, APP_VERSION);
    //                    ui.menuPlugins->addMenu(plugin->menu());
    //                    pluginsToolBar->addAction(plugin->menu()->menuAction());
    //                }
    //}

}


//void MainWindow::setupMenus() {
//    qDebug()<<"----MainWindow::setupMenus()";


//    QStringList translationLanguages = Utilities::availableTranslationLanguages(qmPath);
//    //如果没有找到语言文件或者首选语言是英语,则将"en_US"置为选中
//    //If language files found or the preferred language is English, set the "en_US" menu checked
//    if (translationLanguages.isEmpty()) {
//            ui.actionLanguageDefaultEnglish->setChecked(true);
//            return;
//    }

//    if (!translationLanguages.contains(qmLocale)){
//            qmLocale = "en_US";
//    }

//    if(qmLocale == "en_US"){
//        ui.actionLanguageDefaultEnglish->setChecked(true);
//    }

//    //为每一个语言文件生成动作为菜单
//    //Make action and menu for each language file
//    for (int i = 0; i < translationLanguages.size(); i++){
//        QString translationLanguage = translationLanguages[i];
//            QAction *action = new QAction(tr("&%1 %2") .arg(i + 1).arg(translationLanguage), this);
//            action->setCheckable(true);
//            action->setData(translationLanguage);

//            ui.menuLanguage->addAction(action);
//            languageActionGroup->addAction(action);

//            if (qmLocale == translationLanguage) {
//                    action->setChecked(true);
//                    action->trigger();
//            }

//    }

//	//更新样式菜单
//	//update Style Menu
//	foreach(QAction *action, ui.menuStyle->actions()){
//		if(action->data().toString() == Settings::instance()->getStyle()){
//			action->setChecked(true);
//		}
//	}
//	ui.actionUseStylesPalette->setChecked(Settings::instance()->getPalette());

//}

/*

void MainWindow::restoreWindowState() {
    restoreState( Settings::instance()->value( "MainWindow/State" ).toByteArray() );
    QPoint p = Settings::instance()->value( "MainWindow/Position" ).toPoint();
    QSize s = Settings::instance()->value( "MainWindow/Size" ).toSize();
    if ( !p.isNull() && !s.isNull() )
    {
        resize( s );
        move( p );
    }

    if ( Settings::instance()->value( "MainWindow/Maximized", false ).toBool() )
        showMaximized();

    if ( Settings::instance()->value( "MainWindow/Hidden", false ).toBool() )
            hide();

}

void MainWindow::saveWindowState() {

    Settings::instance()->setValue( "MainWindow/Position", pos() );
    Settings::instance()->setValue( "MainWindow/Size", size() );
    Settings::instance()->setValue( "MainWindow/State", saveState() );
    Settings::instance()->setValue( "MainWindow/Hidden", isHidden() );

}
*/


void MainWindow::setupSystemTray() {
    systemTray = new QSystemTrayIcon(this);
    systemTray->setIcon(QIcon(QString(RESOURCE_PATH)+QString(APP_ICON_PATH)));
    systemTray->setToolTip(APP_NAME);
    connect(systemTray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(slotIconActivated(QSystemTrayIcon::ActivationReason)));

    trayMenu = new QMenu(this);
    trayMenu->addMenu(pluginsMenu);
    trayMenu->addSeparator();
    trayMenu->addAction(ui.actionShowMaximized);
    trayMenu->addAction(ui.actionShowMinimized);
    trayMenu->addAction(ui.actionShowFullScreen);
    trayMenu->addAction(ui.actionShowNormal);
    trayMenu->addSeparator();
    trayMenu->addAction(ui.actionQuit);

    systemTray->setContextMenu(trayMenu);
    systemTray->show();

}

QSystemTrayIcon* MainWindow::SystemTrayIcon() {
    if (!systemTray) {
        setupSystemTray();
    }

    return systemTray;
}

void MainWindow::setTrayIconVisible(bool visible) {
    visible = true;
}

void MainWindow::slotIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:

    case QSystemTrayIcon::DoubleClick:
        //systemInfo();
        break;
    case QSystemTrayIcon::MiddleClick:
        slotAbout();
        break;
    default:
        ;
    }
}






/*
void MainWindow::slotSystemInfo() {
    //如果窗口已打开,
    //If the window has already been opened, activate it
    //if(SystemInfo::isRunning() == true){
    //     foreach (QMdiSubWindow *window, ui.mdiArea->subWindowList()) {
    //    	 SystemInfo *systemInfo = qobject_cast<SystemInfo *>(window->widget());
    //         if (systemInfo)
    //        	 ui.mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
    //             return ;
    //     }

    //	return;
    //}

    //SystemInfo *systemInfo = new SystemInfo(this);
    //ui.mdiArea->addSubWindow(systemInfo, Qt::Dialog);
    //systemInfo->show();

    QMessageBox::information(this,tr("~_~"),tr(" Not accomplished !"));
    qDebug() << "slotSystemInfo";


}




void MainWindow::slotUserManager(){
    //如果窗口已打开,
    //If the window has already been opened, activate it
    //if(UserManagerMainWindow::isRunning() == true){

    //     foreach (QMdiSubWindow *window, ui.mdiArea->subWindowList()) {
    //    	 UserManagerMainWindow *userManager = qobject_cast<UserManagerMainWindow *>(window->widget());
    //         if (userManager)
    //        	 ui.mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
    //             return ;
    //     }

    //	return;
    //}



    User user;
    LoginBase login(&user);
    if (!login.isVerified()) {
        return ;
    }


    //UserManagerMainWindow *userManager = new UserManagerMainWindow(this);
    //ui.mdiArea->addSubWindow(userManager, Qt::Dialog);
    //userManager->show();

    QMessageBox::information(this,tr("~_~"),tr(" Not accomplished !"));
    qDebug() << "slotUserManager";

}

void MainWindow::slotSqlExplorer(){
    //如果窗口已打开,
    //If the window has already been opened, activate it
    //if(SqlExplorer::isRunning() == true){
    //     foreach (QMdiSubWindow *window, ui.mdiArea->subWindowList()) {
    //    	 SqlExplorer *sqlExplorer = qobject_cast<SqlExplorer *>(window->widget());
    //         if (sqlExplorer)
    //        	 ui.mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
    //             return ;
    //     }
//
    //	return;
    //}


    User user;
    LoginBase login(&user);
    if (!login.isVerified()) {
        return ;
    }

    //SqlExplorer *sqlExplorer = new SqlExplorer(this);
    //ui.mdiArea->addSubWindow(sqlExplorer, Qt::Dialog);
    //sqlExplorer->show();
    QMessageBox::information(this,tr("~_~"),tr(" Not accomplished !"));
    qDebug() << "slotSqlExplorer";

}
*/



void MainWindow::slotSystemConfig() {
    QMessageBox::information(this,tr("~_~"),tr(" Not accomplished !"));
    qDebug() << "----MainWindow::slotSystemConfig()";
}



void MainWindow::slotBugReport() {
    QMessageBox::information(this,tr("~_~"),tr(" Not accomplished !"));
    qDebug() << "----MainWindow::slotBugReport()";
}

void MainWindow::slotHelp() {
    QMessageBox::information(this,tr("~_~"),tr(" Not accomplished !"));
    qDebug() << "----MainWindow::slotHelp()";
}

void MainWindow::slotAbout() {
    AboutDialog aboutDlg(this);
    aboutDlg.exec();

}

void MainWindow::slotQuit() {
    //Closes all database connections
    //dbo->closeAllDBConnections();

    //writeSettings();
    //Settings::instance()->saveState(this);

    systemTray->hide();

    qApp->quit();

}


void MainWindow::savePreferedStyle(const QString &preferedStyle, bool useStylePalette){
    Settings::instance()->setStyle(preferedStyle);
    Settings::instance()->setPalette(useStylePalette);

}

void MainWindow::savePreferedLanguage(const QString &preferedLanguage){
    Settings::instance()->setLanguage(preferedLanguage);

}

void MainWindow::retranslateUi() {

    //重新翻译UI
    //Retranslate UI
    ui.retranslateUi(this);

}

bool MainWindow::hasActiveMDIChild(){
    if (ui.mdiArea->activeSubWindow()){
        return true;
    }
    return false;
}

void MainWindow::slotUpdateWindowActions(){
    bool hasSubWindow = (ui.mdiArea->subWindowList().size() > 0);
    bool hasSubWindows = (ui.mdiArea->subWindowList().size() > 1);

    ui.actionCloseSubWindow->setEnabled(hasSubWindow);
    ui.actionCloseAllSubWindows->setEnabled(hasSubWindow);

    ui.actionTitleSubWindows->setEnabled(hasSubWindow);
    ui.actionCascadeSubWindows->setEnabled(hasSubWindow);

    ui.actionNextSubWindow->setEnabled(hasSubWindows);
    ui.actionPreviousSubWindow->setEnabled(hasSubWindows);

    /*

    ui.menuSubWindows->clear();

    QList<QMdiSubWindow *> windows = ui.mdiArea->subWindowList();

    //ui.menuSubWindows->setVisible(!windows.isEmpty());

    for (int i = 0; i < windows.size(); ++i) {
        QWidget *child = qobject_cast<QWidget *>(windows.at(i)->widget());

       QString text;
       if (i < 9) {
          text = tr("&%1 %2").arg(i + 1).arg(child->windowTitle());
       } else {
             text = tr("%1 %2").arg(i + 1).arg(child->windowTitle());
       }
       QAction *action  = ui.menuSubWindows->addAction(text);
       action->setCheckable(true);

       //action ->setChecked(child == qobject_cast<QWidget *>(ui.mdiArea->activeSubWindow()));

       connect(action, SIGNAL(triggered()), ui.mdiArea, SLOT(setActiveSubWindow(child)));

    }
*/

}

















} //namespace HEHUI
