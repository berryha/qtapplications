

/*
 ****************************************************************************
 * usermanagermainwindow.h
 *
 * Created on: 2009-5-1
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
 * Last Modified on: 2010-08-18
 * Last Modified by: 贺辉
 ***************************************************************************
 */



#ifndef USERMANAGERMAINWINDOW_H
#define USERMANAGERMAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QLabel>
#include <QProgressBar>
#include <QSqlQueryModel>
#include <QTimer>


//#include "../ui/ui_usermanager.h"
#include "ui_usermanager.h"


//#include "../../shared/core/user.h"
//#include "../../shared/core/mysharedlib_global.h"
//#include "../../shared/gui/progress/progressdlg.h"
//#include "../../shared/windowsmanagement/windowsmanagement.h"


#include "HHSharedCore/huser.h"
#include "HHSharedCore/hmysharedlib_global.h"
#include "HHSharedGUI/hprogressdlg.h"
#ifdef Q_OS_WIN32
#include "HHSharedWindowsManagement/hwindowsmanagement.h"
#endif





namespace HEHUI {


class UserManagerMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    UserManagerMainWindow(bool isYDAdmin = false, QWidget *parent = 0);
    ~UserManagerMainWindow();

    static bool isRunning(){return running;}

    void retranslateUi();

protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void languageChange();
    void closeEvent(QCloseEvent *e);

public slots:
    void slotQueryUser(const QString &userID, const QString &userName, const QString &userDept);


private slots:
    void slotCheckIsInitializingNeeded();

    void slotResetStatusBar(bool show);

    void slotQueryUserButtonClicked();
    void slotAddUserButtonClicked();

    void slotShowUserInfo(const QModelIndex &index);
    void slotExportQueryResult();
    void slotPrintQueryResult();
    void slotModifyUserInfo();

    void slotAddingUserJobDone(bool result);
    void slotInitializingUserJobDone(bool result);

    void activityTimeout();
    bool verifyPrivilege();


private:
    QString UserID() const;
    QString UserName() const;
    QString UserDept() const;
    QString UserPassword() const;
    QString hasExtMail() const;
    QString hasIntMail() const;

    //	void showQueryResult(const QString &queryString);
    void querySitoyUsersInfo(const QString &queryString);
    void modifyUsersInfo(const QString &userID, const QString &userName, const QString &depatment, const QStringList &emailAccountList);

    void initStatusBar();



private:
    Ui::UserManagerMainWindowUI ui;

    static bool running;

    bool isAddingUser;

    QString emails;


    //    HEHUI::User *user;

    QString databaseConnectionName;
    QSqlQueryModel *model;




    QWidget *m_progressWidget;
    QLayout* hlayout;
    QLabel *label;
    QProgressBar *progressBar;

    ProgressDlg *progressDlg;

    bool isYDAdmin;


#ifdef Q_OS_WIN32
    WindowsManagement *wm;
#endif

    QTimer *activityTimer;
    bool m_verified;


};

} //namespace HEHUI

#endif // USERMANAGERMAINWINDOW_H
