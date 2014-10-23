

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

#include <QMainWindow>
#include <QLabel>
#include <QProgressBar>
#include <QSqlQueryModel>
#include <QTimer>


#include "ui_usermanagermainwindow.h"


#include "HHSharedCore/huser.h"
#include "HHSharedCore/hmysharedlib_global.h"
#include "HHSharedGUI/hprogressdlg.h"
#ifdef Q_OS_WIN32
#include "aduserinfo/adusermanagerwidget.h"
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

    void on_actionExport_triggered(){slotExportQueryResult();}
    void on_actionPrint_triggered(){slotPrintQueryResult();}
    void on_actionEdit_triggered(){slotModifyUserInfo();}
    void on_actionCreateEmailAccount_triggered(){slotCreateEmailAccounts();}
    void on_actionAutoLogon_triggered();//{setAutoLogon(true);}
    void slotExportQueryResult();
    void slotPrintQueryResult();
    void slotModifyUserInfo();
    void slotCreateEmailAccounts();
    void slotShowCustomContextMenu(const QPoint & pos);
    void updateActions();

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

    void setAutoLogon(bool autoLogon = false);



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

    bool m_isJoinedToDomain;
    QString m_joinInfo;
#ifdef Q_OS_WIN32
    ADUserManagerWidget *m_adUserManagerWidget;

    WindowsManagement *wm;
    QString m_msUpdateExeFilename;
#endif


    QTimer *activityTimer;
    bool m_verified;


};

} //namespace HEHUI

#endif // USERMANAGERMAINWINDOW_H
