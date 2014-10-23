/*
 ****************************************************************************
 * aduserinfowidget.h
 *
 * Created on: 2012-10-19
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
 * Last Modified on: 2012-10-19
 * Last Modified by: 贺辉
 ***************************************************************************
 */



#ifndef ADUSERINFO_H
#define ADUSERINFO_H

#include <QTimer>


#include "ui_adusermanagerwidget.h"
#include "aduserinfomodel.h"

#ifdef Q_OS_WIN32
#include "HHSharedWindowsManagement/hadsi.h"
#endif



namespace HEHUI {

class ADUserManagerWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit ADUserManagerWidget(QWidget *parent = 0);
    ~ADUserManagerWidget();

protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void changeEvent(QEvent *e);



private slots:
    void on_toolButtonConnect_clicked();
    void on_comboBoxQueryMode_currentIndexChanged( int index );
    void on_toolButtonQueryAD_clicked();

    void on_actionExport_triggered();
    void on_actionPrint_triggered();

    void on_actionUnlockAccount_triggered();
    void on_actionDisableAccount_triggered();
    void on_actionCreateNewAccount_triggered();
    void on_actionProperties_triggered();

    void on_actionResetPassword_triggered();
    void on_actionUserMustChangePassword_triggered();
    void on_actionUserCannotChangePassword_triggered();
    void on_actionPasswordNeverExpires_triggered();


    void slotExportQueryResult();
    void slotPrintQueryResult();

    void slotViewADUserInfo(const QModelIndex &index);
    void slotCreateADUser(ADUser *adUser);
    void showADUserInfoWidget(ADUser *adUser);
    void slotUnlockAccount();
    void slotDisableADUserAccount();

    void slotResetADUserPassword();
    void slotUserMustChangePassword();


    void slotShowCustomContextMenu(const QPoint & pos);
    void updateActions();
    void getSelectedADUser(const QModelIndex &index);

    void activityTimeout();
    bool verifyPrivilege();



private:
    void reset();

private:
    Ui::ADUserManagerWidgetUI ui;

    ADSI *m_adsi;
    bool m_adOpened;
    ADUser *m_selectedADUser;

    QString m_defaultNamingContext;

    ADUserInfoModel *m_userInfoModel;
    ADUserInfoSortFilterProxyModel *m_sortFilterProxyModel;

    QTimer *activityTimer;
    bool m_verified;


};

} //namespace HEHUI

#endif // ADUSERINFO_H
