

#include <QMessageBox>

#include "aduserinfowidget.h"
#include "ui_aduserinfowidget.h"


//#ifdef Q_OS_WIN32
//#include "HHSharedWindowsManagement/hwindowsmanagement.h"
//#endif



namespace HEHUI {


ADUserInfoWidget::ADUserInfoWidget(ADSI *adsi, ADUser *adUser, QWidget *parent) :
    QWidget(parent), m_adsi(adsi)
{
    ui.setupUi(this);

    ui.groupBoxUseLocalComputer->setVisible(false);

    m_accountName = "";
    m_displayName = "";
    m_description = "";
    m_userWorkstations = "";
    m_telephone = "";
    m_guid = "";
    m_sid = "";
//    m_distinguishedName = "";
    m_simpleOUString = "";
    m_fullOUString = "";

    m_adUser = *adUser;

    initUI();




}

ADUserInfoWidget::~ADUserInfoWidget()
{


}

void ADUserInfoWidget::on_pushButtonEdit_clicked(){


    if(ui.lineEditDisplayName->isReadOnly()){
        switchToEditMode();
        return;
    }

    saveChanges();


}

void ADUserInfoWidget::on_pushButtonClose_clicked(){

    if(!ui.lineEditDisplayName->isReadOnly()){

        QString accountName = ui.lineEditSAMAccount->text().trimmed();
        QString displayName = ui.lineEditDisplayName->text();
        QString description = ui.lineEditDescription->text();
        QString userWorkstations = ui.lineEditUserWorkstations->text().trimmed();
        QString telephone = ui.lineEditTelephone->text();
        QString ouString = ui.comboBoxOU->currentText();

        if(m_accountName != accountName || m_displayName != displayName
                || m_description != description || m_userWorkstations != userWorkstations
                || m_telephone != telephone || m_simpleOUString != ouString){

            int rep = QMessageBox::question(this, tr("Question"), tr("Do you want to save changes before quit?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
            if(rep == QMessageBox::Yes){
                saveChanges();
            }

        }

    }

    this->close();
    emit signalCloseWidget();

}

void ADUserInfoWidget::saveChanges(){

    int pos = 0;
    QRegExpValidator rxValidator(this);
    QRegExp rx;


    QString accountName = ui.lineEditSAMAccount->text().trimmed();
    rx.setPattern("^\\w+$");
    rxValidator.setRegExp(rx);
    if(rxValidator.validate(accountName, pos) != QValidator::Acceptable){
        QMessageBox::critical(this, tr("Error"), tr("Invalid Account Name!"));
        ui.lineEditSAMAccount->setFocus();
        return ;
    }

    QString displayName = ui.lineEditDisplayName->text();
    if(displayName.contains(";") || displayName.contains("|")){
        QMessageBox::critical(this, tr("Error"), tr("Invalid Display Name!"));
        ui.lineEditDisplayName->setFocus();
        return ;
    }

    QString description = ui.lineEditDescription->text();
    if(description.contains(";") || description.contains("|")){
        QMessageBox::critical(this, tr("Error"), tr("Invalid Description!"));
        ui.lineEditDescription->setFocus();
        return ;
    }

    QString userWorkstations = ui.lineEditUserWorkstations->text().trimmed();
    rx.setPattern("^(\\w+,*)+$");
    rxValidator.setRegExp(rx);
    if(rxValidator.validate(userWorkstations, pos) != QValidator::Acceptable){
        QMessageBox::critical(this, tr("Error"), tr("Invalid Workstations!"));
        ui.lineEditUserWorkstations->setFocus();
        return ;
    }

    QString telephone = ui.lineEditTelephone->text();
    if(telephone.contains(";") || telephone.contains("|")){
        QMessageBox::critical(this, tr("Error"), tr("Invalid Telephone Number!"));
        ui.lineEditTelephone->setFocus();
        return ;
    }

    QString ouString = ui.comboBoxOU->currentText();
    if( (!m_simpleOUString.isEmpty()) && (ouString.isEmpty()) ){
        QMessageBox::critical(this, tr("Error"), tr("Invalid OU!"));
        ui.comboBoxOU->setFocus();
        return ;
    }


    m_fullOUString = ouString;
    if(m_fullOUString.contains("\\")){
        QStringList ousList = m_fullOUString.split("\\");
        m_fullOUString = "";
        while (!ousList.isEmpty()) {
            m_fullOUString = m_fullOUString + "OU=" + ousList.takeLast() + ",";
        }
        m_fullOUString += ADUser::getADDefaultNamingContext();
    }else if(!m_fullOUString.isEmpty()){
        m_fullOUString = "OU=" + m_fullOUString + "," + ADUser::getADDefaultNamingContext();
    }

    bool ok = false;

    if(m_accountName != accountName){
        ok = m_adsi->AD_CreateUser(m_fullOUString, accountName, "");
        if(!ok){
            QMessageBox::critical(this, tr("Error"), tr("Failed to create new account! \r\n %1").arg(m_adsi->AD_GetLastErrorString()));
            return;
        }

        m_accountName = accountName;
        m_guid = m_adUser.getAttribute("objectGUID");
        m_sid = m_adUser.getAttribute("objectSid");
        ui.lineEditGUID->setText(m_guid);
        ui.lineEditSID->setText(m_sid);

    }

    switchToViewMode();

    if(m_displayName != displayName){
        ok = m_adsi->AD_ModifyAttribute(accountName, "displayName", displayName, 0);
        if(!ok){
            m_displayName = m_adsi->AD_GetObjectAttribute(accountName, "displayName");
            ui.lineEditDisplayName->setText(m_displayName);
            QMessageBox::critical(this, tr("Error"), tr("Failed to update display name! \r\n %1").arg(m_adsi->AD_GetLastErrorString()));
        }else{
            m_displayName = displayName;
        }
    }

    if(m_description != description){
        ok = m_adsi->AD_ModifyAttribute(accountName, "description", description, 0);
        if(!ok){
            m_description = m_adsi->AD_GetObjectAttribute(accountName, "description");
            ui.lineEditDescription->setText(m_description);
            QMessageBox::critical(this, tr("Error"), tr("Failed to update description! \r\n %1").arg(m_adsi->AD_GetLastErrorString()));
        }else{
            m_description = description;
        }
    }

    if(m_userWorkstations != userWorkstations){
        ok = m_adsi->AD_ModifyAttribute(accountName, "userWorkstations", userWorkstations);
        if(!ok){
            m_userWorkstations = m_adsi->AD_GetObjectAttribute(accountName, "userWorkstations");
            ui.lineEditUserWorkstations->setText(m_userWorkstations);
            QMessageBox::critical(this, tr("Error"), tr("Failed to update user workstations! \r\n %1").arg(m_adsi->AD_GetLastErrorString()));
        }else{
            m_userWorkstations = userWorkstations;
        }
    }

    if(m_telephone != telephone){
        ok = m_adsi->AD_ModifyAttribute(accountName, "telephoneNumber", telephone, 0);
        if(!ok){
            m_telephone = m_adsi->AD_GetObjectAttribute(accountName, "telephoneNumber");
            ui.lineEditTelephone->setText(m_telephone);
            QMessageBox::critical(this, tr("Error"), tr("Failed to update telephone number! \r\n %1").arg(m_adsi->AD_GetLastErrorString()));
        }else{
            m_telephone = telephone;
        }
    }

    if( (m_simpleOUString != ouString) && (!ouString.isEmpty()) ){
        ok = m_adsi->AD_MoveObject(m_fullOUString, accountName);
        if(!ok){
            ui.comboBoxOU->setCurrentIndex(ui.comboBoxOU->findText(m_simpleOUString));
            QMessageBox::critical(this, tr("Error"), tr("Failed to move user! \r\n %1").arg(m_adsi->AD_GetLastErrorString()));
        }else{
            m_simpleOUString = ouString;
        }
    }

    emit signalChangesSaved();

}


void ADUserInfoWidget::initUI(){

    m_accountName = m_adUser.getAttribute("sAMAccountName");

    if(m_accountName.isEmpty()){
        ui.lineEditSAMAccount->setReadOnly(false);
        switchToEditMode();
        ui.pushButtonEdit->setEnabled(false);
        ui.pushButtonEdit->setVisible(false);
        return;
    }

    m_guid = m_adUser.getAttribute("objectGUID");
    m_sid = m_adUser.getAttribute("objectSid");
    if(m_guid.isEmpty()){
        switchToEditMode();
        ui.lineEditDisplayName->setFocus();

    }else{
        switchToViewMode();
        ui.lineEditGUID->setText(m_guid);
        ui.lineEditSID->setText(m_sid);
        ui.pushButtonClose->setFocus();
    }

    ui.lineEditSAMAccount->setText(m_accountName);

    m_displayName = m_adUser.getAttribute("displayName");
    ui.lineEditDisplayName->setText(m_displayName);

    m_userWorkstations = m_adUser.getAttribute("userWorkstations").toUpper();
    ui.lineEditUserWorkstations->setText(m_userWorkstations);

    m_description = m_adUser.getAttribute("description");
    ui.lineEditDescription->setText(m_description);

    m_telephone = m_adUser.getAttribute("telephoneNumber");
    ui.lineEditTelephone->setText(m_telephone);

    ui.comboBoxOU->addItem("");
    ui.comboBoxOU->addItems(ADUser::getOUList());

    QString m_cn = m_adsi->AD_GetObjectAttribute(m_accountName, "cn");
    QString m_distinguishedName = m_adsi->AD_GetObjectAttribute(m_accountName, "distinguishedName");
    if(m_distinguishedName.contains("OU=")){
        int idx =  m_distinguishedName.indexOf("OU=");
        QString temp = m_distinguishedName.replace(0, idx, "");
        //QString temp = m_distinguishedName.remove("CN=" + m_cn + "," ) ;
        temp = temp.remove("," + ADUser::getADDefaultNamingContext());
        temp = temp.remove("OU=");
        QStringList templist = temp.split(",");
        temp = "";
        while (!templist.isEmpty()) {
            temp = temp + templist.takeLast() + "\\";
        }
        temp = temp.left(temp.size() - 1);
        m_simpleOUString = temp;

        ui.comboBoxOU->setCurrentIndex(ui.comboBoxOU->findText(temp) );
    }
    if(m_distinguishedName.contains(",CN=Users")){
        ui.pushButtonEdit->setEnabled(false);
        ui.pushButtonEdit->setVisible(false);
    }


//#ifdef Q_OS_WIN32
//    WindowsManagement wm(this);
//    QString computername = wm.getComputerName().toUpper();
//    QString username = wm.getUserNameOfCurrentThread();

//    if(!m_userWorkstations.contains(computername, Qt::CaseInsensitive)){

//    }

//    QStringList localGroups;
//    wm.getLocalGroupsTheUserBelongs(&localGroups, accountName);
//    QMessageBox::information(this, "localGroups", localGroups.join(","));

//#endif



}

void ADUserInfoWidget::switchToEditMode(){

    ui.lineEditDisplayName->setReadOnly(false);
    ui.lineEditDescription->setReadOnly(false);
    ui.lineEditUserWorkstations->setReadOnly(false);
    ui.lineEditTelephone->setReadOnly(false);
    ui.comboBoxOU->setEnabled(true);

    ui.pushButtonEdit->setText(tr("&Save"));


}

void ADUserInfoWidget::switchToViewMode(){

    ui.lineEditSAMAccount->setReadOnly(true);
    ui.lineEditDisplayName->setReadOnly(true);
    ui.lineEditDescription->setReadOnly(true);
    ui.lineEditUserWorkstations->setReadOnly(true);
    ui.lineEditTelephone->setReadOnly(true);
    ui.comboBoxOU->setEnabled(false);

    ui.pushButtonEdit->setText(tr("&Edit"));


}











} //namespace HEHUI
