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

#include "ui_aduserinfo.h"
#include "aduserinfomodel.h"

#ifdef Q_OS_WIN32
#include "HHSharedWindowsManagement/hadsi.h"
#endif



namespace HEHUI {

class ADUserInfoWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit ADUserInfoWidget(QWidget *parent = 0);
    
protected:
    void changeEvent(QEvent *e);


private slots:
    void on_toolButtonConnect_clicked();
    void on_comboBoxQueryMode_currentIndexChanged( int index );
    void on_toolButtonQueryAD_clicked();



private:
    void reset();

private:
    Ui::ADUserInfo ui;

    ADSI *m_adsi;
    bool m_adOpened;

    //QString m_defaultNamingContext;

    ADUserInfoModel *m_userInfoModel;
    ADUserInfoSortFilterProxyModel *m_sortFilterProxyModel;




};

} //namespace HEHUI

#endif // ADUSERINFO_H
