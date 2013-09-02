


#include "addcontactdialog.h"

#include "../imageresource.h"


namespace HEHUI{

AddContactDialog::AddContactDialog(Contact *user, bool requestToAddNewContact, QWidget *parent) :
    QDialog(parent), m_contact(user)
{
    ui.setupUi(this);

    m_imUser = IMUser::instance();
    
    m_requestToAddContact = true;
    m_rejectRequest = false;
    
    Q_ASSERT_X(user, "AddContactDialog", "Invalid user object");
    
    ui.toolButtonUserFace->setIcon(ImageResource::createIconForContact(m_contact->getFace(), m_contact->getOnlineState()));
    ui.labelUserNickName->setText(m_contact->getNickName());
    ui.labelUserID->setText(m_contact->getUserID());
    
    ui.pushButtonReject->setEnabled(false);
    ui.pushButtonReject->hide();
    
    if(requestToAddNewContact){
        ui.groupBoxUserInfo->setTitle(tr("The following user will be added."));
        ui.groupBoxSettings->hide();
        ui.pushButtonReject->setText(tr("&Cancel"));
        if(m_contact->getFriendshipApply() == Contact::FA_AUTO_ACCEPT){
            ui.groupBoxMessage->hide();
        }else{
            ui.groupBoxMessage->setTitle(tr("Your Request"));
            ui.groupBoxMessage->show();
            ui.lineEditMessage->setFocus();
        }
    
    }else{
        ui.groupBoxUserInfo->setTitle(tr("The following user has been added you as a contact."));
        ui.pushButtonReject->hide();
        ui.pushButtonAddAsAContact->setText(tr("&OK"));
        ui.groupBoxMessage->hide();

        initContactGroupCombox();

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
    :QDialog(parent), m_contact(user)
{
    
    ui.setupUi(this);

    m_imUser = IMUser::instance();
    
    m_requestToAddContact = false;
    m_rejectRequest = false;
    
    
    Q_ASSERT_X(user, "AddContactDialog", "Invalid user object");
    
    ui.toolButtonUserFace->setIcon(ImageResource::createIconForContact(m_contact->getFace(), m_contact->getOnlineState()));
    ui.labelUserNickName->setText(m_contact->getNickName());
    ui.labelUserID->setText(m_contact->getUserID());
    
    ui.groupBoxUserInfo->setTitle(tr("The following user requested to add you as a contact."));
    ui.groupBoxMessage->setTitle(tr("Request Message"));
    ui.lineEditMessage->setText(requestMessage);
    ui.pushButtonReject->setEnabled(true);
    ui.pushButtonReject->show();
    
    ui.lineEditRename->setFocus();

    initContactGroupCombox();
    ui.pushButtonAddAsAContact->setText(tr("&Accept"));

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

void AddContactDialog::initContactGroupCombox(){

    //ui.comboBoxGroup->addItems(m_imUser->contactGroupNames());
    QList<ContactGroupBase *> groups = m_imUser->getContactGroups();
    groups.removeAll(m_imUser->getContactGroup(ContactGroupBase::Group_Friends_ID));
    foreach (ContactGroupBase *group, groups) {
        ui.comboBoxGroup->addItem(group->getGroupName(), QVariant(group->getGroupID()));
    }
    ui.comboBoxGroup->insertItem(0, ContactGroupBase::Group_Friends_Name, QVariant(ContactGroupBase::Group_Friends_ID));
    ui.comboBoxGroup->setCurrentIndex(0);

}

















} //namespace HEHUI

