
/*
 ****************************************************************************
 * settingsdialog.cpp
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
 * Last Modified on: 2010-08-20
 * Last Modified by: 贺辉
 ***************************************************************************
 */


#include <QMessageBox>
#include <QDebug>

#include "settingsdialog.h"



namespace HEHUI {


SettingsDialog::SettingsDialog(const QString &userID, WindowsManagement *wm, int defaultLocationIndex, QWidget *parent)
    : QDialog(parent), m_userID(userID), wm(wm)
{

    Q_ASSERT_X(wm != NULL, "SettingsDialog::SettingsDialog(...)", "'wm' is NULL!");
    
    ui.setupUi(this);
    setWindowTitle(userID);
    
    ui.stackedWidget->setCurrentWidget(ui.pageUserSettings);
    
    ui.lineEditCurPCName->setText(wm->getComputerName());
    
    ui.comboBoxLocation->setCurrentIndex(defaultLocationIndex);
        



}

SettingsDialog::~SettingsDialog()
{
    
}

void SettingsDialog::on_comboBoxLocation_currentIndexChanged ( int index ){
    //qWarning()<<"--------"<<index;

    QString m_computerName = "";
    switch(index){
    case 0:
        wm->setLocation(WindowsManagement::No1_Branch_Factory);
        m_computerName = QString(m_userID + "-BF1").left(15);
        break;
    case 1:
        wm->setLocation(WindowsManagement::No2_Branch_Factory);
        m_computerName = QString(m_userID + "-BF2").left(15);
        break;
    case 2:
        wm->setLocation(WindowsManagement::No3_Branch_Factory);
        m_computerName = QString(m_userID + "-BF3").left(15);
        break;
    case 3:
        wm->setLocation(WindowsManagement::No4_Branch_Factory);
        m_computerName = QString(m_userID + "-BF4").left(15);
        break;
    case 4:
        wm->setLocation(WindowsManagement::LEATHER_PRODUCTS_FACTORY_YD);
        m_computerName = QString(m_userID + "-YD").left(15);
        break;
    default:
        wm->setLocation(WindowsManagement::No1_Branch_Factory);
        m_computerName = m_userID.left(15);
        break;

    }

    m_computerName = m_computerName.replace("_", "-");

    ui.lineEditNewPCName->setText(m_computerName);

}

void SettingsDialog::on_pushButtonOK_clicked(){

//    if(wm){
//        QString computerName =ui.lineEditNewPCName->text().trimmed().toLower();
//        if(((m_userID.toLower() == "sh01") && (computerName.startsWith("sh01", Qt::CaseInsensitive))) || (m_userID.toLower() == computerName)){
//            QMessageBox::critical(this, tr("Error"), tr("Invalid computer name!"));
//            ui.lineEditNewPCName->setFocus();
//            return;
//        }
    
//        wm->setNewComputerNameToBeUsed(computerName);
//    }

    accept();


}








} //namespace HEHUI
