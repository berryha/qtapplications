


#include "addcontactdialog.h"

#include "../imageresource.h"
#include "../imuser.h"


namespace HEHUI{

AddContactDialog::AddContactDialog(Contact *user, bool requestToAddNewContact, QWidget *parent) :
    QDialog(parent), m_user(user)
{
    ui.setupUi(this);
    
    m_requestToAddContact = true;
    m_rejectRequest = false;
    
    Q_ASSERT_X(user, "AddContactDialog", "Invalid user object");
    
    ui.toolButtonUserFace->setIcon(ImageResource::createIconForContact(m_user->getFace(), m_user->getOnlineState()));
    ui.labelUserNickName->setText(m_user->getNickName());
    ui.labelUserID->setText(m_user->getUserID());
    
    ui.pushButtonReject->setEnabled(false);
    ui.pushButtonReject->hide();
    
    if(requestToAddNewContact){
        ui.groupBoxUserInfo->setTitle(tr("The following user will be added."));
        ui.groupBoxSettings->hide();
        if(m_user->getFriendshipApply() == Contact::FA_AUTO_ACCEPT){
            ui.groupBoxMessage->hide();
        }else{
            ui.groupBoxMessage->setTitle(tr("Your Request"));
            ui.groupBoxMessage->show();
            ui.lineEditMessage->setFocus();
        }
    
    }else{
        ui.groupBoxUserInfo->setTitle(tr("The following user has been added you as a contact."));
        ui.pushButtonAddAsAContact->setText(tr("OK"));
        ui.groupBoxMessage->hide();
        ui.comboBoxGroup->addItems(IMUser::instance()->contactGroupNames());
        ui.comboBoxGroup->setCurrentIndex(0);
        ui.groupBoxSettings->show();
        ui.lineEditRename->setFocus();
    }


    validator = new QRegExpValidator(this);
    QRegExp rx("\\b\\w{0,16}\\b");
    validator->setRegExp(rx);
    ui.lineEditMessage->setValidator(validator);
    ui.lineEditRename->setValidator(validator);
    ui.comboBoxGroup->setValidator(validator);


    
}

AddContactDialog::AddContactDialog(Contact *user, const QString &requestMessage, QWidget *parent)
    :QDialog(parent), m_user(user)
{
    
    ui.setupUi(this);
    
    m_requestToAddContact = false;
    m_rejectRequest = false;
    
    
    Q_ASSERT_X(user, "AddContactDialog", "Invalid user object");
    
    ui.toolButtonUserFace->setIcon(ImageResource::createIconForContact(m_user->getFace(), m_user->getOnlineState()));
    ui.labelUserNickName->setText(m_user->getNickName());
    ui.labelUserID->setText(m_user->getUserID());
    
    ui.groupBoxUserInfo->setTitle(tr("The following user requested to add you as a contact."));
    ui.groupBoxMessage->setTitle(tr("Request Message"));
    ui.lineEditMessage->setText(requestMessage);
    ui.pushButtonReject->setEnabled(true);
    ui.pushButtonReject->show();
    
    ui.lineEditRename->setFocus();

    validator = new QRegExpValidator(this);
    QRegExp rx("\\b\\w{0,16}\\b");
    validator->setRegExp(rx);
    ui.lineEditMessage->setValidator(validator);
    ui.lineEditRename->setValidator(validator);
    ui.comboBoxGroup->setValidator(validator);
    

}


void AddContactDialog::changeEvent(QEvent *e)
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


QString AddContactDialog::getMessage() const{
    return ui.lineEditMessage->text();
}

QString AddContactDialog::getNewName() const{
    return ui.lineEditRename->text();
}

QString AddContactDialog::getGroupname(){
    QString groupName = ui.comboBoxGroup->currentText();
//    if(groupName.trimmed().isEmpty()){
//        groupName = m_user->getDefaultGroupName();
//    }
    return groupName;
}

bool AddContactDialog::requestRejected(){
    return m_rejectRequest;
}


void AddContactDialog::on_pushButtonAddAsAContact_clicked(){
    m_rejectRequest = false;
    accept();
}

void AddContactDialog::on_pushButtonReject_clicked(){
    m_rejectRequest = true;
    accept();
}

















} //namespace HEHUI

