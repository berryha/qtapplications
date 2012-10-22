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
    
protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void changeEvent(QEvent *e);


private slots:
    void on_toolButtonConnect_clicked();
    void on_comboBoxQueryMode_currentIndexChanged( int index );
    void on_toolButtonQueryAD_clicked();

    void on_actionExport_triggered(){slotExportQueryResult();}
    void on_actionPrint_triggered(){slotPrintQueryResult();}
    void on_actionEdit_triggered(){slotModifyADUserInfo();}
    void on_actionResetPassword_triggered(){slotResetADUserPassword();}
    void on_actionDisableAccount_triggered(){slotDisableADUserAccount();}


    void slotExportQueryResult();
    void slotPrintQueryResult();

    void slotViewADUserInfo();
    void slotModifyADUserInfo();

    void slotResetADUserPassword();
    void slotDisableADUserAccount();


    void slotShowCustomContextMenu(const QPoint & pos);
    void updateActions();
    void getSelectedADUser(const QModelIndex &index);



private:
    void reset();

private:
    Ui::ADUserManagerWidgetClass ui;

    ADSI *m_adsi;
    bool m_adOpened;
    ADUser *m_selectedADUser;

    //QString m_defaultNamingContext;

    ADUserInfoModel *m_userInfoModel;
    ADUserInfoSortFilterProxyModel *m_sortFilterProxyModel;




};

} //namespace HEHUI

#endif // ADUSERINFO_H
