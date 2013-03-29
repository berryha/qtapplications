/*
 ****************************************************************************
 * mainwindow.h
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
  * Last Modified on: 2010-05-20
  * Last Modified by: 贺辉
  ***************************************************************************
*/





#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QTranslator>

//#include "../ui/ui_mainwindow.h"
#include "ui_mainwindow.h"
#include "../shared/app_constants.h"
//#include "../../shared/gui/widgetbase/mainwindowbase.h"
#include "HHSharedGUI/hmainwindowbase.h"


namespace HEHUI {


class MainWindow: public MainWindowBase  {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0, HEHUI::WindowPosition positon = HEHUI::Center)	;
    ~MainWindow();

    QSystemTrayIcon *SystemTrayIcon();

    void retranslateUi();

    void setTrayIconVisible(bool visible);

private:
    //void createActions();

    void setupSystemTray();

    void initUI();

    bool hasActiveMDIChild();


public slots:
    void slotIconActivated(QSystemTrayIcon::ActivationReason reason);

    void slotSystemConfig();
    void slotBugReport();
    void slotHelp();
    void slotAbout();

    void slotQuit();

    void savePreferedStyle(const QString &preferedStyle, bool useStylePalette);
    void savePreferedLanguage(const QString &preferedLanguage);


private slots:
    void slotInitPlugin(AbstractPluginInterface *plugin);
    void slotUpdateWindowActions();

protected:
    void closeEvent ( QCloseEvent * event );
    void changeEvent ( QEvent * event ) ;


private:
    Ui::MainWindowUIClass ui;


    QPalette originalPalette;
    QString preferedStyle;
    QString preferedLanguage;

    QSystemTrayIcon *systemTray;
    QMenu *trayMenu;

    QMenu *pluginsMenu;


};

} //namespace HEHUI

#endif // MAINWINDOW_H
