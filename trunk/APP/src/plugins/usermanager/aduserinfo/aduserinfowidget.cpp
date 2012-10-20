/*
 ****************************************************************************
 * aduserinfowidget.cpp
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



#ifndef DOMAIN_NAME
#define DOMAIN_NAME "sitoy.group"
#endif
#ifndef DOMAIN_DEFAULTNAMINGCONTEXT
#define DOMAIN_DEFAULTNAMINGCONTEXT "DC=test,DC=local"
#endif

#ifndef DOMAIN_DC_IP
#define DOMAIN_DC_IP "200.200.200.106"
#endif
#ifndef DOMAIN_ADMIN_NAME
#define DOMAIN_ADMIN_NAME "dgadmin"
#endif
#ifndef DOMAIN_ADMIN_PASSWORD
#define DOMAIN_ADMIN_PASSWORD "dmsto&*("
#endif

#ifndef ADSI_LIB
#define ADSI_LIB "adsi.dll"
#endif


#include <QMessageBox>
#include <QDebug>

#include "aduserinfowidget.h"


namespace HEHUI {

ADUserInfoWidget::ADUserInfoWidget(QWidget *parent) :
    QWidget(parent)
{
    ui.setupUi(this);

    ui.lineEditServerIP->setText(DOMAIN_DC_IP);

    m_adsi = new ADSI(this);
    m_adOpened = false;

    //m_defaultNamingContext = "";

    m_userInfoModel = new ADUserInfoModel(this);
    m_sortFilterProxyModel = new ADUserInfoSortFilterProxyModel(this);
    m_sortFilterProxyModel->setSourceModel(m_userInfoModel);
    ui.tableViewADUsers->setModel(m_sortFilterProxyModel);

    ui.comboBoxQueryMode->setCurrentIndex(0);
    ui.stackedWidget->setCurrentWidget(ui.pageSimpleQuery);

    ui.lineEditFilter->setText("(&(objectcategory=person)(objectclass=user)(sAMAccountName=*)(displayName=*))");
    ui.lineEditDataToRetrieve->setText("sAMAccountName,displayName");


}

void ADUserInfoWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui.retranslateUi(this);
        break;
    default:
        break;
    }
}

void ADUserInfoWidget::on_toolButtonConnect_clicked(){
    qDebug()<<"--ADUserInfo::on_ui_toolButtonConnect_clicked()";
    if(m_adOpened){
        m_adsi->AD_Close();
        m_adsi->unloadLibrary();
        reset();
        ui.groupBoxADUsersList->setEnabled(false);
        ui.comboBoxOU->clear();
        m_adOpened = false;
        return;
    }

    QString serverIP = ui.lineEditServerIP->text().trimmed();
    QString adminName = ui.lineEditADAdminName->text().trimmed();
    if(adminName.isEmpty()){adminName = DOMAIN_ADMIN_NAME;}
    QString password = ui.lineEditPassword->text().trimmed();
    if(password.isEmpty()){password = DOMAIN_ADMIN_PASSWORD;}

    if (!m_adsi->loadLibrary(ADSI_LIB)){
        QMessageBox::critical(this, tr("Error"), m_adsi->lastErrorString());
        m_adsi->unloadLibrary();
        return;
    }

    m_adOpened = m_adsi->AD_Open(adminName, password, serverIP, 0);
    if(!m_adOpened){
        QMessageBox::critical(this, tr("Error"), m_adsi->lastErrorString());
        return;
    }

    //m_defaultNamingContext = m_adsi->AD_DefaultNamingContext();

    ui.comboBoxOU->addItem("");

    QString ous = m_adsi->AD_GetAllOUs("", ";", "\\");
    QStringList ouList = ous.split(";");
    ouList.sort();
    ui.comboBoxOU->addItems(ouList);

    ui.groupBoxADUsersList->setEnabled(true);


}

void ADUserInfoWidget::on_comboBoxQueryMode_currentIndexChanged( int index ){
    if(index == 0){
        ui.stackedWidget->setCurrentWidget(ui.pageSimpleQuery);
    }else{
        ui.stackedWidget->setCurrentWidget(ui.pageCustomQuery);
    }
}

void ADUserInfoWidget::on_toolButtonQueryAD_clicked(){

    QString itemSeparator = "\\", attributeSeparator = "|";

    QString filter, dataToRetrieve;
    if(ui.comboBoxQueryMode->currentIndex() == 0){
        QString displayName = ui.lineEditDisplayName->text();
        filter = QString("(&(objectcategory=person)(objectclass=user)(sAMAccountName=%1*)%2)").arg(ui.lineEditAccountName->text()).arg(displayName.trimmed().isEmpty()?"":QString("(displayName=%1*)").arg(displayName));
        dataToRetrieve = "sAMAccountName,displayName,userWorkstations,telephoneNumber,description,objectGUID,objectSid";
    }else{
        filter = ui.lineEditFilter->text();
        dataToRetrieve = ui.lineEditDataToRetrieve->text().trimmed();
    }

    QString ouString = ui.comboBoxOU->currentText();
    if(ouString.contains("\\")){
        QStringList ousList = ouString.split("\\");
        ouString = "";
        while (!ousList.isEmpty()) {
            ouString = ouString + "OU=" + ousList.takeLast() + ",";
        }
        ouString += DOMAIN_DEFAULTNAMINGCONTEXT;
    }else if(!ouString.isEmpty()){
        ouString = ouString + "," + DOMAIN_DEFAULTNAMINGCONTEXT;
    }

    QString resultString = m_adsi->AD_GetObjectsInOU(ouString, filter, dataToRetrieve, itemSeparator, attributeSeparator);
    if(resultString.isEmpty()){
        QString error = m_adsi->AD_GetLastErrorString();
        if(!error.isEmpty()){
            QMessageBox::critical(this, tr("Error"), tr("Failed to query!\r\n%1").arg(error) );
            return;
        }
    }


//QMessageBox::information(this, "resultString", resultString);

    QStringList itemStrings = resultString.split(itemSeparator);
    QList<QStringList> items;
    foreach (QString itemString, itemStrings) {
        QStringList attributes = itemString.split(attributeSeparator);
        items.append(attributes);
    }

    m_userInfoModel->setADUserItems(dataToRetrieve.split(","), items);

    ui.tableViewADUsers->horizontalHeader ()->resizeSections(QHeaderView::ResizeToContents);
    //ui.tableViewADUsers->resizeColumnToContents(0);
    //ui.tableViewADUsers->setColumnHidden(3, true);


}


void ADUserInfoWidget::reset(){

}











} //namespace HEHUI
