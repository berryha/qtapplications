


/*
 ****************************************************************************
 * settingsdialog.h
 *
 * Created on: 2009-8-20
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
 * Last Modified on: 2010-8-20
 * Last Modified by: 贺辉
 ***************************************************************************
 */




#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H


#include <QDialog>
#include "ui_settingsdialog.h"

#ifdef Q_OS_WIN32
#include "HHSharedWindowsManagement/hwindowsmanagement.h"
#endif


namespace HEHUI {

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    SettingsDialog(const QString &userID, WindowsManagement *wm, int defaultLocationIndex, QWidget *parent = 0);
    ~SettingsDialog();
    
    



private slots:
    void on_comboBoxLocation_currentIndexChanged ( int index );
    void on_pushButtonOK_clicked();
    


private:
    Ui::SettingsDialogClass ui;

    QString m_userID;

#ifdef Q_OS_WIN32
    WindowsManagement *wm;
#endif



};

} //namespace HEHUI

#endif // SETTINGSDIALOG_H
