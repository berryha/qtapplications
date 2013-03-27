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





#include "modifyuserinfodialog.h"



ModifyUserInfoDialog::ModifyUserInfoDialog(const QString &userID, const QString &userName, const QString &depatment, const QString &emailAccountString, QWidget *parent)
    : QDialog(parent), m_userID(userID)
{

    ui.setupUi(this);
    setWindowTitle(userID);

    ui.lineEditChineseName->setText(userName);
    ui.comboBoxUserDept->setCurrentIndex(ui.comboBoxUserDept->findText(depatment));
    ui.checkBoxExternalEmail->setChecked(emailAccountString.contains("sitoy.com", Qt::CaseInsensitive));
    ui.checkBoxInternalEmail->setChecked(emailAccountString.contains("sitoydg.com", Qt::CaseInsensitive));



}

ModifyUserInfoDialog::~ModifyUserInfoDialog()
{

}

void ModifyUserInfoDialog::changeEvent(QEvent *e)
{

    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui.retranslateUi(this);
        break;
    default:
        break;
    }


}

QString ModifyUserInfoDialog::newChineseName() const{
    return ui.lineEditChineseName->text();
}

QString ModifyUserInfoDialog::newDepartment() const{
    return ui.comboBoxUserDept->currentText();
}

QStringList ModifyUserInfoDialog::newEmailAccountStringList() const{

    QString emailAccount = ui.lineEditEmailAccount->text();
    QStringList emailAccountList;
    emailAccountList << "" << "";
    if(ui.checkBoxExternalEmail->isChecked()){
       emailAccountList[0] = (emailAccount + "@sitoy.com");
    }

    if(ui.checkBoxInternalEmail->isChecked()){
       emailAccountList[1] = (emailAccount + "@sitoydg.com");
    }

    return emailAccountList;

}

void ModifyUserInfoDialog::on_comboBoxUserDept_currentIndexChanged (const QString & text){

    QString emailAccount = m_userID;
    if(emailAccount.isEmpty()){
        if(text.toLower() == "sales" && m_userID.left(2).toLower() == "sa"){
            emailAccount.replace(0, 2, "sales_");
        }

    }

    ui.lineEditEmailAccount->setText(emailAccount);

}

void ModifyUserInfoDialog::on_pushButtonOK_clicked(){

    accept();


}




