/*
 ****************************************************************************
 * filename
 *
 * Created on: 2010-12-30
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
 * Last Modified on: 2010-12-30
 * Last Modified by: 贺辉
 ***************************************************************************
 */





#ifndef MODIFYUSERINFODIALOG_H
#define MODIFYUSERINFODIALOG_H

#include <QDialog>
#include "ui_modifyuserinfodialog.h"


class ModifyUserInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ModifyUserInfoDialog(const QString &userID, const QString &userName, const QString &depatment, const QString &emailAccountString, QWidget *parent = 0);
    ~ModifyUserInfoDialog();

    QString newChineseName() const;
    QString newDepartment() const;
    QStringList newEmailAccountStringList() const;


protected:
    void changeEvent(QEvent *e);

private slots:
    void on_pushButtonOK_clicked();
    void on_comboBoxUserDept_currentIndexChanged (const QString & text);


private:
    Ui::ModifyUserInfoDialog ui;

    QString m_userID;


};

#endif // MODIFYUSERINFODIALOG_H
