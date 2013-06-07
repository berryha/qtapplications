/*
 * contactsmanager.cpp
 *
 *  Created on: 2009-9-12
 *      Author: 贺辉
 */

#include <QDebug>
#include <QSqlError>

#include "contactsmanager.h"
#include "contact.h"
#include "../settings.h"
#include "../imageresource.h"
//#include "../shared/global_app.h"

//#include "../../shared/core/database/databaseutility.h"
//#include "../../shared/core/user.h"
//#include "../../shared/gui/databaseconnecter/databaseconnecter.h"
#include "HHSharedCore/hdatabaseutility.h"
#include "HHSharedCore/huser.h"
#include "HHSharedGUI/hdatabaseconnecter.h"



namespace HEHUI {


//QHash<int, ContactGroup*> ContactsManager::contactGroupHash = QHash<int, ContactGroup*>();
//QHash<QString, Contact*> ContactsManager::contactHash = QHash<QString, Contact*>();

ContactsManager::ContactsManager(QObject *parent)
    :QObject(parent)
{
    qDebug()<<"----ContactsManager::ContactsManager(QObject *parent)~~";

    //        query = 0;
    //userPrivateDataFilePath = Settings::instance()->getUserPrivateDataFilePath(IMUser::instance()->getUserID());
    //openDatabase();

    m_imUser = IMUser::instance();
    //Q_ASSERT_X(m_imUser->getOnlineState() != IM::ONLINESTATE_OFFLINE, "ContactsManager::ContactsManager(QObject *parent)", "User is not online!");


}

ContactsManager::~ContactsManager() {
    
    //    if(query){
    //        query->clear();
    //    }
    //    delete query;
    //    query = 0;
    
    
    
    
}

QStringList ContactsManager::contactGroups() const{
    QStringList groups;
    foreach (ContactGroup *group, contactGroupHash.values()) {
        groups.append(group->getGroupName());
    }

    return groups;

}

bool ContactsManager::hasFriendContact(const QString &contactID){

    //TODO:改进

    if(!hasUserInfo(contactID)){
        return false;
    }

    Contact * contact = getUser(contactID);
    if(!contact){return false;}
    if(contact->getContactGroupID() > 0){
        return true;
    }

    return false;

}

bool ContactsManager::hasUserInfo(const QString &userID){
//    bool has = false;
//    QStringList contacts = contactHash.keys();
//    foreach (QString id, contacts) {
//        if(id == userID){
//            has = true;
//            break;
//        }
//    }

//    return has;

    return contactHash.contains(userID);

}

QString ContactsManager::groupNameThatContactBelongsTo(const QString &contactID) const{

    QString gpName = "";

    if(contactHash.contains(contactID)){
        Contact *contact = contactHash.value(contactID);
        if(contact){
            int gpID = contact->getContactGroupID();
            //qDebug()<<"------gpID:"<<gpID<<"  contactID:"<<contactID;
            ContactGroup *group = contactGroupHash.value(gpID);
            if(group){
                gpName = group->getGroupName();
            }
        }
    }

    return gpName;

}

QString ContactsManager::getContactGroupsInfoString() const{

    QStringList groupsInfo;

    foreach (ContactGroup *group, contactGroupHash.values()) {
        //QList<Contact*> contacts = group->getContactList();
        QString groupInfo = group->getGroupName();
        foreach (Contact *contact, group->getContactList()) {
            groupInfo += (CONTACT_INFO_SEPARATOR + contact->getUserID());
        }
        groupsInfo.append(groupInfo);
    }

    return groupsInfo.join(GROUP_INFO_SEPARATOR);

}

ContactGroup * ContactsManager::getContactGroup(int personalContactGroupID){
    ContactGroup * group = 0;
    if(contactGroupHash.contains(personalContactGroupID)){
        group = contactGroupHash.value(personalContactGroupID);
    }
    
    return group;
    
}

Contact * ContactsManager::getUser(const QString &contactID){
    Contact * contact = 0;
    if(contactHash.contains(contactID)){
        contact = contactHash.value(contactID);
    }else{
        //contact = new Contact(contactID, this);
    }

    return contact;

}

QList<Contact*> ContactsManager::getContactGroupMembers(const QString &contactGroupName){

    int groupID = this->getPersonalContactGroupID(contactGroupName);
    ContactGroup *group = getContactGroup(groupID);
    if(!group){
        return QList<Contact*>();
    }
    
    return group->getContactList();
    
}

QStringList ContactsManager::getContacts() const{

    QStringList contacts;

    foreach (ContactGroup *group, contactGroupHash.values()) {
        //QList<Contact*> contacts = group->getContactList();
        foreach (Contact *contact, group->getContactList()) {
            if(contact->getContactGroupID() > 0){
                contacts << contact->getUserID();
            }
        }
    }

    return contacts;

}

QStringList ContactsManager::getStrangers() const{

    QStringList contacts;

    foreach (ContactGroup *group, contactGroupHash.values()) {
        //QList<Contact*> contacts = group->getContactList();
        foreach (Contact *contact, group->getContactList()) {
            if(contact->getContactGroupID() == 0){
                contacts << contact->getUserID();
            }

        }
    }

    return contacts;

}


QStringList ContactsManager::getUsers() const{

    QStringList users;

    foreach (ContactGroup *group, contactGroupHash.values()) {
        foreach (Contact *contact, group->getContactList()) {
            users << contact->getUserID();
        }
    }

    return users;

}

bool ContactsManager::loadInterestGroups(){

    QString groupQueryString = QString("select * from interestgroups");
    
    QSqlQueryModel *model = new QSqlQueryModel(this);
    
    model->setQuery(queryDatabase(groupQueryString, true));
    if(model->lastError().type() != QSqlError::NoError){
        qCritical()<<"Query Error!"<<model->lastError().text();
        delete model;
        return false;
    }

    for (int j=0; j<model->rowCount(); j++) {
        quint32 groupID = QVariant(model->record(j).value("GroupID")).toUInt();
        InterestGroup *interestGroup = new InterestGroup(groupID, this);
        interestGroup->setGroupTypeID(QVariant(model->record(j).value("TypeID")).toUInt());
        interestGroup->setParentGroupID(QVariant(model->record(j).value("ParentGroup")).toUInt());
        interestGroup->setCreatorID(QVariant(model->record(j).value("Creator")).toString());
        
        interestGroup->setGroupName(QVariant(model->record(j).value("GroupName")).toString());
        interestGroup->setCreationTime(QVariant(model->record(j).value("CreationTime")).toDateTime());
        interestGroup->setGroupInfoVersion(QVariant(model->record(j).value("GroupInfoVersion")).toUInt());
        
        interestGroup->setMemberListInfoVersion(QVariant(model->record(j).value("MemberListVersion")).toUInt());
        interestGroup->setDescription(QVariant(model->record(j).value("Description")).toString());
        interestGroup->setAnnouncement(QVariant(model->record(j).value("Announcement")).toString());
        interestGroup->setRemark(QVariant(model->record(j).value("Remark")).toString());
        
        interestGroup->clearUpdatedProperties();
        
        interestGroupsHash.insert(groupID, interestGroup);

        qApp->processEvents();
    }
    
    
    model->deleteLater();

    return true;
    
}

InterestGroup * ContactsManager::getInterestGroup(quint32 groupID){

    InterestGroup * group = 0;
    if(interestGroupsHash.contains(groupID)){
        group = interestGroupsHash.value(groupID);
    }
    
    return group;
}

bool ContactsManager::addNewInterestGroupToDatabase(InterestGroup *interestGroup){
    
    quint32 groupID = interestGroup->getGroupID();

    if(interestGroupsHash.contains(groupID)){
        return false;
    }

    if(!localUserDataDB.isValid()){
        if(!openDatabase()){
            return false;
        }
    }
    QSqlQuery query(localUserDataDB);

    QString queryString = QString("Insert  Into [interestgroups] ([GroupID]) Values('%1')")
            .arg(groupID);
    
    if(!query.exec(queryString)){
        qCritical()<<"Can not insert new interest group data to database! Error:"<<query.lastError().text();
        return false;
    }
    
    interestGroupsHash.insert(groupID, interestGroup);
    
    return true;
    
}

bool ContactsManager::leaveInterestGroup(quint32 groupID){
    

    if(!interestGroupsHash.contains(groupID)){
        return false;
    }

    if(!localUserDataDB.isValid()){
        if(!openDatabase()){
            return false;
        }
    }
    QSqlQuery query(localUserDataDB);
    
    
    QString queryString = QString("delete  from [interestgroups] where ID='%1' ").arg(groupID) ;
    
    if(!query.exec(queryString)){
        qCritical()<<"Can not delete interest group data from database! Error:"<<query.lastError().text();
        return false;
    }
    
    interestGroupsHash.remove(groupID);
    
    
    queryString = QString("update  [contacts_detailed_info] set InterestGroupID=0 where InterestGroupID=%1 ").arg(groupID) ;
    if(!query.exec(queryString)){
        qCritical()<<"Can not update contacts info to database! Error:"<<query.lastError().text();
        return false;
    }
    
    
    return true;
    
}


bool ContactsManager::saveInterestGroupInfoToDatabase(InterestGroup *interestGroup){

    if(!interestGroup){
        return false;
    }
    
    
    if(!localUserDataDB.isValid()){
        if(!openDatabase()){
            return false;
        }
    }
    QSqlQuery query(localUserDataDB);

    QString updateSQLStatement = interestGroup->getUpdateSQLStatement();
    if(updateSQLStatement.trimmed().isEmpty()){
        return false;
    }
    
    QString statement = QString("update interestgroups set %1 where GroupID='%2' ").arg(updateSQLStatement).arg(interestGroup->getGroupID());

    if(!query.exec(statement)){
        QSqlError error = query.lastError();
        QString msg = QString("Can not save interest group info to database! Group ID:%1, %2 Error Type:%3 Error NO.:%4").arg(interestGroup->getGroupID()).arg(error.text()).arg(error.type()).arg(error.number());
        qCritical()<<msg;
        return false;
    }
    
    interestGroup->clearUpdatedProperties();
    
    return true;
    
    
}


QList<InterestGroup *> ContactsManager::getInterestGroupsList(){

    return interestGroupsHash.values();
}

bool ContactsManager::saveInterestGroupMemberToDatabase(quint32 groupID, const QString &userID, quint32 memberRole){

    if(!localUserDataDB.isValid()){
        if(!openDatabase()){
            return false;
        }
    }
    QSqlQuery query(localUserDataDB);

    QString statement = QString("insert into interestgroupmembers(MemberID, GroupID, MemberRole) values('%1', %2, %3)  ").arg(userID).arg(groupID).arg(memberRole);
    
    if(!query.exec(statement)){
        QSqlError error = query.lastError();
        QString msg = QString("Can not save interest group member info to database! Group ID:%1, User ID:%2! %3 Error Type:%4 Error NO.:%5").arg(groupID).arg(userID).arg(error.text()).arg(error.type()).arg(error.number());
        qCritical()<<msg;
        return false;
    }
    
    return true;

}


bool ContactsManager::saveInterestGroupMembersToDatabase(InterestGroup *interestGroup){

    if(!interestGroup){
        qCritical()<<"ERROR! Invalid InterestGroup!";
        return false;
    }
    
    //TODO:
    QHash<QString,quint32> membersHash = interestGroup->getMembersHash();
    if(membersHash.isEmpty()){
        qCritical()<<"ERROR! Empty membersHash!";
        return false;
    }
    quint32 groupID = interestGroup->getGroupID();
    
    
    if(!localUserDataDB.isValid()){
        if(!openDatabase()){
            return false;
        }
    }
    QSqlQuery query(localUserDataDB);

    QString statement = QString("delete from interestgroupmembers where GroupID=%1 ").arg(groupID);
    
    if(!query.exec(statement)){
        QSqlError error = query.lastError();
        QString msg = QString("Can not delete interest group members from database! Group ID:%1, %2 Error Type:%3 Error NO.:%4").arg(groupID).arg(error.text()).arg(error.type()).arg(error.number());
        qCritical()<<msg;
        return false;
    }
    
    QStringList members = membersHash.keys();
    
    foreach (QString userID, members) {
        
        if(!saveInterestGroupMemberToDatabase(groupID, userID, membersHash.value(userID))){
            return false;
        }
        
    }
    
    
    
    return true;
    
}

bool ContactsManager::saveContactChatMessageToDatabase(const QString &senderID, const QString &receiverID, const QString &message, const QString &time){

    if(message.isEmpty()){
        qWarning()<<"Empty Contact Chat Message!";
        return false;
    }

    if(!localUserDataDB.isValid()){
        if(!openDatabase()){
            return false;
        }
    }

    QString t = time;
    if(t.trimmed().isEmpty()){
        t = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    }

    QSqlQuery query(localUserDataDB);

    QString statement = QString("insert into contactchatmessages(SenderID, ReceiverID, Message, Time) values('%1', '%2', '%3', '%4') ").arg(senderID).arg(receiverID).arg(message).arg(t);

    if(!query.exec(statement)){
        QSqlError error = query.lastError();
        QString msg = QString("Can not save contact chat message to database! Sender ID:%1, Receiver ID:%2,  %3 Error Type:%4 Error NO.:%5").arg(senderID).arg(receiverID).arg(error.text()).arg(error.type()).arg(error.number());
        qCritical()<<msg;
        return false;
    }

    return true;


}

bool ContactsManager::saveInterestGroupChatMessageToDatabase(const QString &senderID, quint32 interestGroupID, const QString &message, const QString &time){

    if(message.isEmpty()){
        qWarning()<<"Empty InterestGroupID Chat Message!";
        return false;
    }

    if(!localUserDataDB.isValid()){
        if(!openDatabase()){
            return false;
        }
    }

    QString t = time;
    if(t.trimmed().isEmpty()){
        t = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    }

    QSqlQuery query(localUserDataDB);

    QString statement = QString("insert into interestgroupchatmessages(SenderID, GroupID, Message, Time) values('%1', '%2', '%3', '%4') ").arg(senderID).arg(interestGroupID).arg(message).arg(t);

    if(!query.exec(statement)){
        QSqlError error = query.lastError();
        QString msg = QString("Can not save interestgroup chat message to database! Sender ID:%1, GroupID ID:%2,  %3 Error Type:%4 Error NO.:%5").arg(senderID).arg(interestGroupID).arg(error.text()).arg(error.type()).arg(error.number());
        qCritical()<<msg;
        return false;
    }

    return true;


}

Contact * ContactsManager::createNewContact(const QString &contactID, const QString &nickname, const QString &face){

    Contact *contact = 0;
    if(contactHash.contains(contactID)){
        contact = contactHash.value(contactID);
        contact->setNickName(nickname);
        contact->setFace(face);
    }else{
        contact = new Contact(contactID, nickname, this);
        contact->setFace(face);
        if(!slotAddNewContactToDatabase(contact)){
            //delete contact;
            //contact = 0;
        }

    }

    return contact;

}

void ContactsManager::slotFetchContactsInfo(ItemBoxWidget *expandListView){
    qDebug()<<"ContactsManager::slotFetchContactsInfo(...)";

    QString groupQueryString = QString("select * from contactgroups");
    
    QSqlQueryModel *model = new QSqlQueryModel(this);
    QSqlQueryModel *contactsModel = new QSqlQueryModel(this);
    
    model->setQuery(queryDatabase(groupQueryString, true));
    if(model->lastError().type() != QSqlError::NoError){
        qCritical()<<"Query Error!"<<model->lastError().text();
        delete model;
        delete contactsModel;
        return;
    }

    while (model->canFetchMore()) {
        model->fetchMore();
    }

    
    for (int i=0; i<model->rowCount(); i++) {
        int groupID = QVariant(model->record(i).value("GroupID")).toInt();
        if(contactGroupHash.contains(groupID)){continue;}
        QString groupName = QVariant(model->record(i).value("GroupName")).toString();
        QList<Contact*> list;
        QString contactsQueryString = QString("select * from contacts_detailed_info where ContactGroupID=%1") .arg(groupID);
        
        contactsModel->setQuery(queryDatabase(contactsQueryString, true));
        if(contactsModel->lastError().type() != QSqlError::NoError){
            qCritical()<<"Query Error!"<<contactsModel->lastError().text();
            delete model;
            delete contactsModel;
            return;
        }

        while (contactsModel->canFetchMore()) {
            contactsModel->fetchMore();
        }
        
        for (int j=0; j<contactsModel->rowCount(); j++) {
            QString contactUID = QVariant(contactsModel->record(j).value("UserID")).toString();
            Contact *contact = new Contact(contactUID, "", this);
            contact->setTrueName(QVariant(contactsModel->record(j).value("TrueName")).toString());
            contact->setNickName(QVariant(contactsModel->record(j).value("NickName")).toString());
            contact->setGender(IMUserBase::Gender(QVariant(contactsModel->record(j).value("Gender")).toUInt()));
            contact->setAge(IMUserBase::Gender(QVariant(contactsModel->record(j).value("Age")).toUInt()));
            contact->setFace(QVariant(contactsModel->record(j).value("Face")).toString());
            
            //            contact->setInterestGroupID(QVariant(contactsModel->record(j).value("InterestGroupID")).toUInt());
            //            contact->setSystemGroupID(QVariant(contactsModel->record(j).value("SystemGroupID")).toUInt());
            contact->setPersonalDetailInfoVersion(QVariant(contactsModel->record(j).value("PersonalInfoVersion")).toUInt());
            
            contact->setHomeAddress(QVariant(contactsModel->record(j).value("HomeAddress")).toString());
            contact->setHomePhoneNumber(QVariant(contactsModel->record(j).value("HomePhoneNumber")).toString());
            contact->setHomeZipCode(QVariant(contactsModel->record(j).value("HomeZipCode")).toString());
            contact->setPersonalHomepage(QVariant(contactsModel->record(j).value("PersonalHomepage")).toString());
            contact->setPersonalEmailAddress(QVariant(contactsModel->record(j).value("PersonalEmailAddress")).toString());
            
            contact->setLastLoginTime(QVariant(contactsModel->record(j).value("LastLoginTime")).toDateTime());
            contact->setLastLoginHostAddress(QVariant(contactsModel->record(j).value("LastLoginHostAddress")).toString());
            contact->setLastLoginHostPort(QVariant(contactsModel->record(j).value("LastLoginHostPort")).toUInt());
            
            //contact->setQuestionForSecurity(QVariant(contactsModel->record(j).value("QuestionForSecurity")).toString());
            //contact->setAnswerForSecurity(QVariant(contactsModel->record(j).value("AnswerForSecurity")).toString());
            contact->setCompanyName(QVariant(contactsModel->record(j).value("CompanyName")).toString());
            contact->setJobTitle(QVariant(contactsModel->record(j).value("JobTitle")).toString());
            contact->setBusinessAddress(QVariant(contactsModel->record(j).value("BusinessAddress")).toString());
            contact->setBusinessPhoneNumber(QVariant(contactsModel->record(j).value("BusinessPhoneNumber")).toString());
            contact->setBusinessZipCode(QVariant(contactsModel->record(j).value("BusinessZipCode")).toString());
            contact->setBusinessFaxNumber(QVariant(contactsModel->record(j).value("BusinessFaxNumber")).toString());
            contact->setBusinessHomepage(QVariant(contactsModel->record(j).value("BusinessHomepage")).toString());
            contact->setBusinessEmailAddress(QVariant(contactsModel->record(j).value("BusinessEmailAddress")).toString());
            contact->setRegistrationTime(QVariant(contactsModel->record(j).value("RegistrationTime")).toDateTime());
            contact->setLoginTimes(QVariant(contactsModel->record(j).value("LoginTimes")).toInt());
            contact->setRemarkName(QVariant(contactsModel->record(j).value("RemarkName")).toString());

            contact->setContactGroupID(groupID);
            contactHash.insert(contactUID, contact);
            list.append(contact);
            qApp->processEvents();

        }
        
        ContactGroup *group = new ContactGroup(groupID, groupName, list, this);
        contactGroupHash.insert(groupID, group);

        slotLoadContacts(expandListView, groupID, groupName, list);

    }
    
    
    contactsModel->deleteLater();
    model->deleteLater();


}

void ContactsManager::slotFetchStrangersInfo(){
    qDebug()<<"ContactsManager::slotFetchStrangersInfo(...)";

    QSqlQueryModel *contactsModel = new QSqlQueryModel(this);
    QString contactsQueryString = QString("select * from contacts_detailed_info where ContactGroupID=0");

    contactsModel->setQuery(queryDatabase(contactsQueryString, true));
    if(contactsModel->lastError().type() != QSqlError::NoError){
        qCritical()<<"Query Error!"<<contactsModel->lastError().text();
        delete contactsModel;
        return;
    }

    for (int j=0; j<contactsModel->rowCount(); j++) {
        QString contactUID = QVariant(contactsModel->record(j).value("UserID")).toString();
        Contact *contact = new Contact(contactUID, "", this);
        contact->setTrueName(QVariant(contactsModel->record(j).value("TrueName")).toString());
        contact->setNickName(QVariant(contactsModel->record(j).value("NickName")).toString());
        contact->setGender(IMUserBase::Gender(QVariant(contactsModel->record(j).value("Gender")).toUInt()));
        contact->setAge(IMUserBase::Gender(QVariant(contactsModel->record(j).value("Age")).toUInt()));
        contact->setFace(QVariant(contactsModel->record(j).value("Face")).toString());

        //        contact->setInterestGroupID(QVariant(contactsModel->record(j).value("InterestGroupID")).toUInt());
        //        contact->setSystemGroupID(QVariant(contactsModel->record(j).value("SystemGroupID")).toUInt());
        contact->setPersonalDetailInfoVersion(QVariant(contactsModel->record(j).value("PersonalInfoVersion")).toUInt());

        contact->setHomeAddress(QVariant(contactsModel->record(j).value("HomeAddress")).toString());
        contact->setHomePhoneNumber(QVariant(contactsModel->record(j).value("HomePhoneNumber")).toString());
        contact->setHomeZipCode(QVariant(contactsModel->record(j).value("HomeZipCode")).toString());
        contact->setPersonalHomepage(QVariant(contactsModel->record(j).value("PersonalHomepage")).toString());
        contact->setPersonalEmailAddress(QVariant(contactsModel->record(j).value("PersonalEmailAddress")).toString());

        contact->setLastLoginTime(QVariant(contactsModel->record(j).value("LastLoginTime")).toDateTime());
        contact->setLastLoginHostAddress(QVariant(contactsModel->record(j).value("LastLoginHostAddress")).toString());
        contact->setLastLoginHostPort(QVariant(contactsModel->record(j).value("LastLoginHostPort")).toUInt());

        //contact->setQuestionForSecurity(QVariant(contactsModel->record(j).value("QuestionForSecurity")).toString());
        //contact->setAnswerForSecurity(QVariant(contactsModel->record(j).value("AnswerForSecurity")).toString());
        contact->setCompanyName(QVariant(contactsModel->record(j).value("CompanyName")).toString());
        contact->setJobTitle(QVariant(contactsModel->record(j).value("JobTitle")).toString());
        contact->setBusinessAddress(QVariant(contactsModel->record(j).value("BusinessAddress")).toString());
        contact->setBusinessPhoneNumber(QVariant(contactsModel->record(j).value("BusinessPhoneNumber")).toString());
        contact->setBusinessZipCode(QVariant(contactsModel->record(j).value("BusinessZipCode")).toString());
        contact->setBusinessFaxNumber(QVariant(contactsModel->record(j).value("BusinessFaxNumber")).toString());
        contact->setBusinessHomepage(QVariant(contactsModel->record(j).value("BusinessHomepage")).toString());
        contact->setBusinessEmailAddress(QVariant(contactsModel->record(j).value("BusinessEmailAddress")).toString());
        contact->setRegistrationTime(QVariant(contactsModel->record(j).value("RegistrationTime")).toDateTime());
        contact->setLoginTimes(QVariant(contactsModel->record(j).value("LoginTimes")).toInt());
        contact->setRemarkName(QVariant(contactsModel->record(j).value("RemarkName")).toString());


        contact->setContactGroupID(0);
        contactHash.insert(contactUID, contact);
        qApp->processEvents();
    }


    contactsModel->deleteLater();


}


void ContactsManager::slotAddNewContactGroupToUI(ItemBoxWidget *expandListView, int personalContactGroupID, const QString &groupName){
    qDebug()<<"--ContactsManager::slotAddNewContactGroupToUI(...)";

    //    Category *category = new Category();
    //    category->setID(QString::number(groupID));
    //    category->setName(groupName);
    //    expandListView->load(category);

    Category category;
    category.setID(QString::number(personalContactGroupID));
    category.setName(groupName);
    expandListView->addCategory(category);

}

void ContactsManager::slotDeleteContactGroupFromUI(ItemBoxWidget *expandListView, const QString &groupName){

    expandListView->removeCategory(groupName);
}

void ContactsManager::addContactToUI(ItemBoxWidget *expandListView, const QString &groupName, const QString &contactID){
    qDebug()<<"--ContactsManager::addContactToUI(...)  groupName:"<<groupName<<" contactID:"<<contactID;

    Contact *contact = 0;
    if(!contactHash.contains(contactID)){
        return;
    }
    contact = contactHash.value(contactID);
    Q_ASSERT(contact);

    ObjectItem objectItem;
    objectItem.setID(contactID);

//    QString name = contact->getRemarkName();
//    if(name.isEmpty()){
//        name = contact->getNickName();
//    }
//    if(name.isEmpty()){
//        name = contact->getUserID();
//    }
    objectItem.setName(contact->displayName());

    objectItem.setIconName(ImageResource::getIconFilePathForContact(contact->getFace(), false));

    expandListView->addItem(groupName, objectItem);


}

void ContactsManager::deleteContactFromUI(ItemBoxWidget *expandListView, const QString &groupName, const QString &contactID){

    expandListView->removeItem(groupName, contactID);

}

void ContactsManager::moveContactToUI(ItemBoxWidget *expandListView, const QString &old_groupName, const QString &new_groupName, const QString &contactID){
    qDebug()<<"moveContactToUI(...)";

    if(!contactHash.contains(contactID)){
        return;
    }
    expandListView->moveItem(old_groupName, new_groupName, contactID);

    //expandListView->update();
    //expandListView->updateGeometry();

}

void ContactsManager::updateContactToUI(ItemBoxWidget *expandListView, const QString &groupName, const QString &contactID){
    qDebug()<<"--ContactsManager::updateContactToUI(...)  groupName:"<<groupName<<" contactID:"<<contactID;

    Contact *contact = 0;
    if(!contactHash.contains(contactID)){
        return;
    }
    contact = contactHash.value(contactID);
    Q_ASSERT(contact);

    expandListView->updateObjectItemName(groupName, contactID, contact->displayName());
    //expandListView->updateObjectItemIcon(groupName, contactID, contact->getFace());
    expandListView->updateObjectItemIcon(groupName, contactID, ImageResource::getIconFilePathForContact(contact->getFace(), contact->getOnlineState()));


}

void ContactsManager::updateContactToUI(ItemBoxWidget *expandListView, int personalContactGroupID, const QString &contactID){

    updateContactToUI(expandListView, getPersonalContactGroupName(personalContactGroupID), contactID);

}


void ContactsManager::renameGroupToUI(ItemBoxWidget *expandListView, const QString &old_groupName, const QString &new_groupName){

    //    ContactGroup *contactGroup = 0;
    //    int groupID = getGroup(group_name);
    //    if(contactGroupHash.contains(groupID)){
    //        contactGroup = contactGroupHash.value(groupID);
    //    }else{
    //        return;
    //    }
    //    contactGroup->setGroupName(new_groupName);

    expandListView->updateCategoryName(old_groupName, new_groupName);


}


void ContactsManager::slotLoadContacts(ItemBoxWidget *expandListView, int groupID, const QString groupName, QList<Contact*> contactList){
        qDebug()<<"--ContactsManager::slotLoadContacts(...)";

    //	Category *category = new Category();
    //	category->setID(QString::number(groupID));
    //	category->setName(groupName);

    Category category;
    category.setID(QString::number(groupID));
    category.setName(groupName);

    foreach(Contact *contact, contactList)
    {
        //ObjectItem *objectItem = new ObjectItem();
        ObjectItem objectItem;
        objectItem.setID(contact->getUserID());

//        QString name = contact->getRemarkName();
//        if(name.isEmpty()){
//            name = contact->getNickName();
//        }
//        if(name.isEmpty()){
//            name = contact->getUserID();
//        }
        objectItem.setName(contact->displayName());

        objectItem.setIconName(ImageResource::getIconFilePathForContact(contact->getFace(), false));
        //objectItem.setIconMode(QIcon::Disabled);
        //objectItem.setCategoryID(groupName);

        category.addItem(objectItem);

        qApp->processEvents();
    }

    expandListView->addCategory(category);

}

bool ContactsManager::addOrDeleteContact(const QString &contactID, quint32 groupID, bool add ){

    Contact *contact = contactHash.value(contactID);
    if(!contact){
        return false;
    }

    ContactGroup *group = contactGroupHash.value(groupID);
    if(group){
        //QList<Contact*> contacts = group->getContactList();
        if(add){
            //contacts.append(contact);
            group->addContact(contact);
        }else{
            //contacts.removeAll(contact);
            group->deleteContact(contact);
        }
        //group->setContactList(contacts);
    }

    return true;

}

bool ContactsManager::moveContact(const QString &contactID, quint32 oldGroupID, quint32 newGroupID){

    Contact *contact = contactHash.value(contactID);
    if(!contact){
        return false;
    }

//    QList<Contact*> contacts;

    ContactGroup *oldGroup = contactGroupHash.value(oldGroupID);
    if(oldGroup){
//        contacts = oldGroup->getContactList();
//        contacts.removeAll(contact);
//        oldGroup->setContactList(contacts);
//        contacts.clear();
        oldGroup->deleteContact(contact);
    }

    ContactGroup *newGroup = contactGroupHash.value(newGroupID);
    if(newGroup){
//        contacts = newGroup->getContactList();
//        contacts.append(contact);
//        newGroup->setContactList(contacts);
        newGroup->addContact(contact);
    }

    return true;

}


void ContactsManager::slotChangeContactOnlineState(const QString &contactID, quint8 onlineStateCode, const QString &peerAddress, quint16 peerPort, const QString &greetingInfo){

    qDebug()<<"----ContactsManager::slotChangeContactOnlineState(...)";
    qDebug()<<"----ContactsManager::slotChangeContactOnlineState(...)~~contactID:"<<contactID;
    qDebug()<<"----ContactsManager::slotChangeContactOnlineState(...)~~greetingInfo:"<<greetingInfo;

    Contact *contact = contactHash.value(contactID);
    contact->setOnlineState(IM::OnlineState(onlineStateCode));

}



//添加新的联系人
//Add new contact
bool ContactsManager::slotAddNewContactToDatabase(Contact *contact){
    qDebug()<<"----ContactsManager::slotAddNewContactToDatabase(...)  Contact ID:"<<contact->getUserID();
    
    if(contactHash.contains(contact->getUserID())){
        qCritical()<<"Error! Contact already exists!";
        return false;
    }
    qDebug()<<"-----------------------------0";

    if(!localUserDataDB.isValid()){
        if(!openDatabase()){
            return false;
        }
    }
    QSqlQuery query(localUserDataDB);
    
    QString contactUID = contact->getUserID();
    int contactsGroupID = contact->getContactGroupID();
    QString contactNickName = contact->getNickName();
    QString face = contact->getFace();
    
    QString queryString = QString("Insert  Into [contacts_detailed_info] ([UserID],[NickName],[Face],[ContactGroupID]) Values('%1', '%2', '%3', '%4')")
            .arg(contactUID).arg(contactNickName).arg(face).arg(contactsGroupID) ;
    //QSqlQuery query = queryDatabase(queryString, true);
    
    if(!query.exec(queryString)){
        qCritical()<<"Can not insert new contact data to database! Error:"<<query.lastError().text();
        return false;
    }

    contactHash.insert(contact->getUserID(), contact);
    int groupID = contact->getContactGroupID();
    ContactGroup *group = contactGroupHash.value(groupID);
    if(group){
        QList<Contact*> contacts = group->getContactList();
        contacts.append(contact);
        group->setContactList(contacts);
    }

    return true;
    
}

//删除联系人
//Delete contact
bool ContactsManager::slotdeleteContactFromDatabase(Contact *contact){
    qDebug()<<"----ContactsManager::slotdeleteContactFromDatabase(...)";

    if(!contactHash.contains(contact->getUserID())){
        return false;
    }

    if(!localUserDataDB.isValid()){
        if(!openDatabase()){
            return false;
        }
    }
    QSqlQuery query(localUserDataDB);

    QString contactUID = contact->getUserID();

    QString queryString = QString("Delete  From [contacts_detailed_info] Where [UserID] ='%1' ").arg(contactUID);
    if(!query.exec(queryString)){
        qCritical()<<"Can not delete contact data from database! Error:"<<query.lastError().text();
        return false;
    }

    contactHash.remove(contact->getUserID());
    int groupID = contact->getContactGroupID();
    ContactGroup *group = contactGroupHash.value(groupID);
    if(group){
        QList<Contact*> contacts = group->getContactList();
        contacts.removeAll(contact);
        group->setContactList(contacts);
    }

    return true;

}

int ContactsManager::getPersonalContactGroupID(const QString &groupName){

    if(!localUserDataDB.isValid()){
        if(!openDatabase()){
            return 0;
        }
    }
    QSqlQuery query(localUserDataDB);
    
    QString queryString = QString("Select [GroupID]  From [contactgroups] where [GroupName] = '%1' ").arg(groupName);

    //QSqlQuery query = queryDatabase(queryString, true);
    if(!query.exec(queryString)){
        qCritical()<<QString("Can not get contact group ID! Group Name:'%1', Error:%2").arg(groupName).arg(query.lastError().text());

        return 0;
    }
    
    query.first();
    if(!query.isValid()){
        return 0;
    }
    
    return query.value(0).toInt();
    
    
    
}

QString ContactsManager::getPersonalContactGroupName(int personalContactGroupID) {

    if(!localUserDataDB.isValid()){
        if(!openDatabase()){
            return "";
        }
    }
    QSqlQuery query(localUserDataDB);
    
    QString queryString = QString("Select [GroupName]  From [contactgroups] where [GroupID] = %1 ").arg(personalContactGroupID);

    //QSqlQuery query = queryDatabase(queryString, true);
    if(!query.exec(queryString)){
        qCritical()<<QString("Can not get contact group name! Group ID:'%1', Error:%2").arg(personalContactGroupID).arg(query.lastError().text());

        return "";
    }
    
    query.first();
    if(!query.isValid()){
        return "";
    }
    
    
    return query.value(0).toString();
    
}

int ContactsManager::slotAddNewContactGroupToDatabase(const QString &groupName){
    qDebug()<<"--slotAddNewContactGroup(...)";

    if(!localUserDataDB.isValid()){
        if(!openDatabase()){
            return false;
        }
    }
    QSqlQuery query(localUserDataDB);
    
    QString queryString = QString("Insert  Into [contactgroups] ([GroupName]) Values('%1')").arg(groupName);

    //QSqlQuery query = queryDatabase(queryString, true);
    if(!query.exec(queryString)){
        qCritical()<<QString("Can not add new contact group! Group Name:'%1', Error:%2").arg(groupName).arg(query.lastError().text());

        return 0;
    }
    
    
    queryString = QString("Select [GroupID]  From [contactgroups] where [GroupName] = '%1' ").arg(groupName);
    //query = queryDatabase(queryString, true);
    if(!query.exec(queryString)){
        qCritical()<<QString("Can not query contact group info! Group Name:'%1', Error:%2").arg(groupName).arg(query.lastError().text());

        return 0;
    }
    
    query.first();
    if(!query.isValid()){
        return 0;
    }
    
    int groupID = query.value(0).toInt();
    ContactGroup *group = new ContactGroup(groupID, groupName, QList<Contact*>());
    contactGroupHash.insert(groupID, group);


    return groupID;
    
} 

bool ContactsManager::renameGroupToDatabase(const QString &old_groupName, const QString &new_groupName){

    if(!localUserDataDB.isValid()){
        if(!openDatabase()){
            return false;
        }
    }
    QSqlQuery query(localUserDataDB);

    QString queryString = QString("Update [contactgroups] Set [GroupName] = '%1' where [GroupName] = '%2' ").arg(new_groupName).arg(old_groupName);

    if(!query.exec(queryString)){
        qCritical()<<QString("Can not rename contact group! Group Name:'%1', Error:%2").arg(old_groupName).arg(query.lastError().text());

        return false;
    }

    queryString = QString("Select [GroupID]  From [contactgroups] where [GroupName] = '%1' ").arg(new_groupName);
    if(!query.exec(queryString)){
        qCritical()<<QString("Can not get contact group ID! Group Name:'%1', Error:%2").arg(new_groupName).arg(query.lastError().text());
        return false;
    }
    query.first();
    if(!query.isValid()){
        return false;
    }
    int groupID = query.value(0).toInt();

    ContactGroup *contactGroup = 0;
    if(contactGroupHash.contains(groupID)){
        contactGroup = contactGroupHash.value(groupID);

    }else{
        //contactGroup = new ContactGroup()
        //contactGroupHash.insert(groupID, contactGroup);
        //TODO:
        return false;
    }

    contactGroup->setGroupName(new_groupName);

    return true;


}

bool ContactsManager::deleteGroupFromDatabase(const QString &groupName){
    qDebug()<<"--ContactsManager::deleteGroupFromDatabase(..) groupName:"<<groupName;

    int groupID = getPersonalContactGroupID(groupName);
    ContactGroup *group = contactGroupHash.take(groupID);
    if(group){
        delete group;
        qDebug()<<"contactGroupHash.keys():"<<contactGroupHash.keys();
    }


    if(!localUserDataDB.isValid()){
        if(!openDatabase()){
            return false;
        }
    }
    QSqlQuery query(localUserDataDB);


    QString queryString = QString("Delete From [contactgroups] where [GroupName] = '%1' ").arg(groupName);

    if(!query.exec(queryString)){
        qCritical()<<QString("Can not delete contact group! Group Name:'%1', Error:%2").arg(groupName).arg(query.lastError().text());

        return false;
    }


    return true;


}

bool ContactsManager::getMyInfoFormLocalDatabase(){
    qWarning()<<"--getMyInfoFormLocalDatabase()";


    if(!localUserDataDB.isValid()){
        if(!openDatabase()){
            return false;
        }
    }
    QSqlQuery query(localUserDataDB);

    //IMUser *m_imUser = IMUser::instance();
    QSqlRecord record;
    QString statement = QString("select * from my_detailed_info where UserID='%2' ").arg(m_imUser->getUserID());

    
    if(!query.exec(statement)){
        QSqlError error = query.lastError();
        QString msg = QString("Can not query user info from database! %1 Error Type:%2 Error NO.:%3").arg(error.text()).arg(error.type()).arg(error.number());
        qCritical()<<msg;

        //TODO:数据库重启，重新连接
        //MySQL数据库重启，重新连接
        if(error.number() == 2006){
            query.clear();
            openDatabase(true);
        }

        return false;
    }
    
    
    //    query = queryDatabase(statement, false);
    //    if((query.lastError().type() != QSqlError::NoError)){
    //        qCritical()<<QString("Can not query data from local database! Error: %1").arg(query.lastError().text());
    //        return false;
    //    }
    query.first();
    if(!query.isValid()){
        qCritical()<<QString("Can not query user info! Invalid record! User ID:%1").arg(m_imUser->getUserID());
        
        statement = QString("Insert Into my_detailed_info(UserID) Values('%1')").arg(m_imUser->getUserID());
        if(!query.exec(statement)){
            qCritical()<<"Can not insert data! "<<query.lastError().text();
        }
        
        return false;
    }
    

    record = query.record();
    

    m_imUser->setTrueName(QVariant(query.value(record.indexOf("TrueName"))).toString());
    m_imUser->setNickName(QVariant(query.value(record.indexOf("NickName"))).toString());
    m_imUser->setGender(IMUserBase::Gender(QVariant(query.value(record.indexOf("Gender"))).toUInt()));
    m_imUser->setFace(QVariant(query.value(record.indexOf("Face"))).toString());
    m_imUser->setInterestGroupInfoVersion(QVariant(query.value(record.indexOf("InterestGroupsInfoVersion"))).toUInt());
    m_imUser->setPersonalContactGroupsVersion(QVariant(query.value(record.indexOf("PersonalContactGroupsInfoVersion"))).toUInt());
    m_imUser->setBlacklistInfoVersion(QVariant(query.value(record.indexOf("BlacklistInfoVersion"))).toUInt());
    m_imUser->setBlacklistInfoString(QVariant(query.value(record.indexOf("Blacklist"))).toString());
    m_imUser->setPersonalDetailInfoVersion(QVariant(query.value(record.indexOf("PersonalInfoVersion"))).toUInt());
    
    m_imUser->setHomeAddress(QVariant(query.value(record.indexOf("HomeAddress"))).toString());
    m_imUser->setHomePhoneNumber(QVariant(query.value(record.indexOf("HomePhoneNumber"))).toString());
    m_imUser->setHomeZipCode(QVariant(query.value(record.indexOf("HomeZipCode"))).toString());
    m_imUser->setPersonalHomepage(QVariant(query.value(record.indexOf("PersonalHomepage"))).toString());
    m_imUser->setPersonalEmailAddress(QVariant(query.value(record.indexOf("PersonalEmailAddress"))).toString());

    m_imUser->setLastLoginTime(QVariant(query.value(record.indexOf("LastLoginTime"))).toDateTime());
    m_imUser->setLastLoginHostAddress(QVariant(query.value(record.indexOf("LastLoginHostAddress"))).toString());
    m_imUser->setLastLoginHostPort(QVariant(query.value(record.indexOf("LastLoginHostPort"))).toUInt());

    //info->setQuestionForSecurity(QVariant(query.value(record.indexOf("QuestionForSecurity"))).toString());
    //info->setAnswerForSecurity(QVariant(query.value(record.indexOf("AnswerForSecurity"))).toString());
    
    m_imUser->setCompanyName(QVariant(query.value(record.indexOf("CompanyName"))).toString());
    m_imUser->setJobTitle(QVariant(query.value(record.indexOf("JobTitle"))).toString());

    m_imUser->setBusinessAddress(QVariant(query.value(record.indexOf("BusinessAddress"))).toString());
    m_imUser->setBusinessPhoneNumber(QVariant(query.value(record.indexOf("BusinessPhoneNumber"))).toString());
    m_imUser->setBusinessZipCode(QVariant(query.value(record.indexOf("BusinessZipCode"))).toString());
    m_imUser->setBusinessFaxNumber(QVariant(query.value(record.indexOf("BusinessFaxNumber"))).toString());
    m_imUser->setBusinessHomepage(QVariant(query.value(record.indexOf("BusinessHomepage"))).toString());
    m_imUser->setBusinessEmailAddress(QVariant(query.value(record.indexOf("BusinessEmailAddress"))).toString());

    m_imUser->setRegistrationTime(QVariant(query.value(record.indexOf("RegistrationTime"))).toDateTime());
    m_imUser->setLoginTimes(QVariant(query.value(record.indexOf("LoginTimes"))).toInt());

    m_imUser->clearUpdatedProperties();

    return true;
    


}

bool ContactsManager::saveMyInfoToDatabase(){

    qDebug()<<"--ContactsManager::saveMyInfoToDatabase()";
    
    //IMUser *info = IMUser::instance();
    IMUser *info = m_imUser;
    QString updateSQLStatement = info->getUpdateSQLStatement();
    if(updateSQLStatement.trimmed().isEmpty()){
        return false;
    }
    
    if(!localUserDataDB.isValid()){
        if(!openDatabase()){
            return false;
        }
    }
    QSqlQuery query(localUserDataDB);
    QString statement = QString("update my_detailed_info set %1 where UserID='%2' ").arg(updateSQLStatement).arg(info->getUserID());
    if(!query.exec(statement)){
        QSqlError error = query.lastError();
        QString msg = QString("Can not query user info from database! %1 Error Type:%2 Error NO.:%3").arg(error.text()).arg(error.type()).arg(error.number());
        qCritical()<<msg;
        return false;
    }
    
    info->clearUpdatedProperties();
    
    return true;
    
}

bool ContactsManager::setContactSummaryInfo(const QString &contactID, const QString &contactSummaryInfo){

    if(contactHash.contains(contactID)){
        contactHash.value(contactID)->setPersonalSummaryInfo(contactSummaryInfo);
        return true;
    }
    
    return false;
    
}

bool ContactsManager::getContactInfoFormLocalDatabase(const QString &contactID){
    qWarning()<<"--getContactInfoFormLocalDatabase()";
    
    
    if(!localUserDataDB.isValid()){
        if(!openDatabase()){
            return false;
        }
    }
    QSqlQuery query(localUserDataDB);
    
    Contact *contact = 0;
    if(contactHash.contains(contactID)){
        contact = contactHash.value(contactID);
    }else{
        contact = new Contact(contactID, this);
        contactHash.insert(contactID, contact);
    }
    
    QSqlRecord record;
    QString statement = QString("select * from contacts_detailed_info where UserID='%2' ").arg(contactID);

    if(!query.exec(statement)){
        QSqlError error = query.lastError();
        QString msg = QString("Can not query contact's' info from database! %1 Error Type:%2 Error NO.:%3").arg(error.text()).arg(error.type()).arg(error.number());
        qCritical()<<msg;
        return false;
    }
    
    
    //    query = queryDatabase(statement, false);
    //    if((query.lastError().type() != QSqlError::NoError)){
    //        qCritical()<<QString("Can not query data from local database! Error: %1").arg(query.lastError().text());
    //        return false;
    //    }
    query.first();
    if(!query.isValid()){
        qCritical()<<QString("Can not query contact's' info! Invalid record! Contact ID:%1").arg(contactID);
        
        return false;
    }
    

    record = query.record();
    

    contact->setTrueName(QVariant(query.value(record.indexOf("TrueName"))).toString());
    contact->setNickName(QVariant(query.value(record.indexOf("NickName"))).toString());
    contact->setGender(IMUserBase::Gender(QVariant(query.value(record.indexOf("Gender"))).toUInt()));
    contact->setAge(QVariant(query.value(record.indexOf("Age"))).toUInt());
    contact->setFace(QVariant(query.value(record.indexOf("Face"))).toString());
    
    //    contact->setInterestGroupID(QVariant(query.value(record.indexOf("InterestGroupID"))).toUInt());
    //    contact->setSystemGroupID(QVariant(query.value(record.indexOf("SystemGroupID"))).toUInt());
    contact->setContactGroupID(QVariant(query.value(record.indexOf("ContactGroupID"))).toUInt());
    contact->setPersonalDetailInfoVersion(QVariant(query.value(record.indexOf("PersonalInfoVersion"))).toUInt());
    
    //    info->setSystemGroupInfoVersion(QVariant(query.value(record.indexOf("SystemGroupsInfoVersion"))).toUInt());
    //    info->setInterestGroupInfoVersion(QVariant(query.value(record.indexOf("PersonalInterestGroupsInfoVersion"))).toUInt());
    //    info->setPersonalContactGroupsVersion(QVariant(query.value(record.indexOf("PersonalContactGroupsInfoVersion"))).toUInt());
    
    contact->setHomeAddress(QVariant(query.value(record.indexOf("HomeAddress"))).toString());
    contact->setHomePhoneNumber(QVariant(query.value(record.indexOf("HomePhoneNumber"))).toString());
    contact->setHomeZipCode(QVariant(query.value(record.indexOf("HomeZipCode"))).toString());
    contact->setPersonalHomepage(QVariant(query.value(record.indexOf("PersonalHomepage"))).toString());
    contact->setPersonalEmailAddress(QVariant(query.value(record.indexOf("PersonalEmailAddress"))).toString());

    contact->setLastLoginTime(QVariant(query.value(record.indexOf("LastLoginTime"))).toDateTime());
    contact->setLastLoginHostAddress(QVariant(query.value(record.indexOf("LastLoginHostAddress"))).toString());
    contact->setLastLoginHostPort(QVariant(query.value(record.indexOf("LastLoginHostPort"))).toUInt());

    //info->setQuestionForSecurity(QVariant(query.value(record.indexOf("QuestionForSecurity"))).toString());
    //info->setAnswerForSecurity(QVariant(query.value(record.indexOf("AnswerForSecurity"))).toString());
    
    contact->setCompanyName(QVariant(query.value(record.indexOf("CompanyName"))).toString());
    contact->setJobTitle(QVariant(query.value(record.indexOf("JobTitle"))).toString());

    contact->setBusinessAddress(QVariant(query.value(record.indexOf("BusinessAddress"))).toString());
    contact->setBusinessPhoneNumber(QVariant(query.value(record.indexOf("BusinessPhoneNumber"))).toString());
    contact->setBusinessZipCode(QVariant(query.value(record.indexOf("BusinessZipCode"))).toString());
    contact->setBusinessFaxNumber(QVariant(query.value(record.indexOf("BusinessFaxNumber"))).toString());
    contact->setBusinessHomepage(QVariant(query.value(record.indexOf("BusinessHomepage"))).toString());
    contact->setBusinessEmailAddress(QVariant(query.value(record.indexOf("BusinessEmailAddress"))).toString());

    contact->setRegistrationTime(QVariant(query.value(record.indexOf("RegistrationTime"))).toDateTime());
    contact->setLoginTimes(QVariant(query.value(record.indexOf("LoginTimes"))).toInt());

    contact->setRemarkName(QVariant(query.value(record.indexOf("RemarkName"))).toString());


    contact->clearUpdatedProperties();

    return true;
    


}

bool ContactsManager::saveContactInfoToDatabase(const QString &contactID){

    qDebug()<<"--ContactsManager::saveContactInfoToDatabase(...)";
    

    if(!localUserDataDB.isValid()){
        if(!openDatabase()){
            return false;
        }
    }
    QSqlQuery query(localUserDataDB);

    Contact *contact = 0;
    if(contactHash.contains(contactID)){
        contact = contactHash.value(contactID);
    }else{
        qCritical()<<"Error! Contact '"<<contactID<<"' does not exist!";
        return false;
    }
    QString updateSQLStatement = contact->getUpdateSQLStatement();
    if(updateSQLStatement.trimmed().isEmpty()){
        qCritical()<<"Error! Empty Update SQL Statement";
        return false;
    }
    
    QString statement = QString("update contacts_detailed_info set %1 where UserID='%2' ").arg(updateSQLStatement).arg(contactID);   
    if(!query.exec(statement)){
        QSqlError error = query.lastError();
        QString msg = QString("Can not save contact info to database! Contact ID:%1, %2 Error Type:%3 Error NO.:%4").arg(contactID).arg(error.text()).arg(error.type()).arg(error.number());
        qCritical()<<msg;
        return false;
    }
    
    contact->clearUpdatedProperties();
    
    return true;
    
}


//bool ContactsManager::saveContactGroupsInfoToDatabase(){
    
//    foreach (Contact *contact, contactHash.values()) {
//        contact->setContactGroupID(0);
//    }
    
    
//    QHash<QString/*Group Name*/, QStringList/*Group Members' ID*/> personalContactGroupsHash = m_imUser->getPersonalContactGroupsHash();

    
//    QStringList groups = personalContactGroupsHash.keys();
//    foreach (QString groupName, groups) {
//        int groupID = getPersonalContactGroupID(groupName);
//        if(!groupID){
//            groupID = slotAddNewContactGroupToDatabase(groupName);
//        }
//        QStringList members = personalContactGroupsHash.value(groupName);
//        foreach (QString contactID, members) {
//            if(contactID.trimmed().isEmpty()){continue;}
//            Contact *contact = 0;
//            if(contactHash.contains(contactID)){
//                contact = contactHash.value(contactID);
//            }else{
//                contact = new Contact(contactID, 0);
//                contactHash.insert(contactID, contact);
//                slotAddNewContactToDatabase(contact);
//            }
//            contact->setContactGroupID(groupID);
//            saveContactInfoToDatabase(contactID);
            
//        }
        
        
//    }
    
//    //TODO:更新联系人信息
    
//    return true;
    
    
//}


bool ContactsManager::openDatabase(bool reopen){
    qWarning()<<"--ContactsManager::openDatabase(...)";

    userPrivateDataFilePath = Settings::instance()->getUserPrivateDataFilePath(m_imUser->getUserID());

    //Check Local Database
    bool needInitUserDB = false;
    if(!QFile(userPrivateDataFilePath).exists()){
        needInitUserDB = true;
    }



    if(reopen){
        //        if(query){
        //            query->clear();
        //            delete query;
        //            query = 0;
        //        }
        DatabaseUtility::closeDBConnection(LOCAL_USERDATA_DB_CONNECTION_NAME);
    }


    //QSqlDatabase db = QSqlDatabase::database(LOCAL_USERDATA_DB_CONNECTION_NAME);
    localUserDataDB = QSqlDatabase::database(LOCAL_USERDATA_DB_CONNECTION_NAME);
    if(!localUserDataDB.isValid()){
        QSqlError err;
        DatabaseUtility databaseUtility;
        userPrivateDataFilePath = Settings::instance()->getUserPrivateDataFilePath(m_imUser->getUserID());
        err = databaseUtility.openDatabase(LOCAL_USERDATA_DB_CONNECTION_NAME,
                                           LOCAL_USERDATA_DB_DRIVER,
                                           "",
                                           0,
                                           "",
                                           "",
                                           userPrivateDataFilePath,
                                           HEHUI::SQLITE);
        if (err.type() != QSqlError::NoError) {
            qCritical() << QString("An error occurred when opening the database on '%1'! %2").arg(REMOTE_SITOY_COMPUTERS_DB_SERVER_HOST).arg(err.text());
            return false;
        }

    }

    localUserDataDB = QSqlDatabase::database(LOCAL_USERDATA_DB_CONNECTION_NAME);
    if(!localUserDataDB.isOpen()){
        qCritical()<<QString("Database is not open! %1").arg(localUserDataDB.lastError().text());
        return false;
    }

    //    if(!query){
    //        query = new QSqlQuery(localUserDataDB);
    //    }

    if(needInitUserDB){
        if(!initLocalDatabase()){
            return false;
        }
    }

    return true;



}

bool ContactsManager::initLocalDatabase(QString *errorMessage){
    qWarning()<<"--ContactsManager::initLocalDatabase(...)";

    if(!localUserDataDB.isValid() || !localUserDataDB.isOpen()){
        if(errorMessage){
            *errorMessage = tr("Database Invalid Or Not Open!");
        }
        return false;
    }

    QSqlQuery query;
    QSqlError error = DatabaseUtility::excuteSQLScriptFromFile(localUserDataDB, ":/text/resources/text/userdata.sql", "UTF-8", &query, true);
    if(error.type() != QSqlError::NoError){
        QString msg = error.text();
        if(errorMessage){
            *errorMessage = msg;
        }
        return false;
    }


    QString statement = QString("insert into my_detailed_info(UserID) values('%1')").arg(m_imUser->getUserID());
    if(!query.exec(statement)){
        QSqlError error = query.lastError();
        QString msg = QString("Can not initialize user database! %1 Error Type:%2 Error NO.:%3").arg(error.text()).arg(error.type()).arg(error.number());
        qCritical()<<msg;
        if(errorMessage){
            *errorMessage = msg;
        }
        return false;
    }

    return true;

}






/*

void ContactsManager::slotUpdateContactsInfo2(){



 QSqlQuery query;
 QSqlRecord record;


 //Local
 QString queryString = QString("select ContactsListVersion from contacts where UID = '%1'") .arg(User::instance()->getUserID());

 int localContactsListVersion = -1;

 query = queryDatabase(queryString, true);
 if(query.lastError().type() == QSqlError::NoError && query.isValid()){
  query.first();
  localContactsListVersion = query.value(0).toInt();
  qDebug()<<"Local Contacts List Version:"<<localContactsListVersion;
 }else{
  //QMessageBox::critical(this, QObject::tr("Fatal Error"), tr("Can not query data from local database! <br> %1").arg(model.lastError().text()));
  qCritical()<<QString("Can not query data from local database!");
  qCritical()<<QString("Error: %1").arg(query.lastError().text());
  //statusBar()->showMessage(tr("Error! Can not query data from local database!"));
 }


 if(User::instance()->getRemoteServerContactsListVersion() != localContactsListVersion){
  qDebug()<<"Remote Server Contacts List Version:"<<User::instance()->getRemoteServerContactsListVersion();

  if(User::instance()->getRemoteServerContactsListVersion() > localContactsListVersion){
   queryString = QString("update contacts set ContactsList = '%1'  where UID = '%2'") .arg(User::instance()->getContactsList(), User::instance()->getUserID());
   query = queryDatabase(queryString, true);
   qDebug()<<"Update Local Contacts List Version";
  }else{
   queryString = QString("select ContactsList from contacts where UID = '%1'") .arg(User::instance()->getUserID());
   QString contactsList;
   query = queryDatabase(queryString, true);
   if(query.lastError().type() == QSqlError::NoError && query.isValid()){
    query.first();
    contactsList = query.value(0).toString();
   }else{
    //QMessageBox::critical(this, QObject::tr("Fatal Error"), tr("Can not query data from database server! <br> %1").arg(model.lastError().text()));
    qCritical()<<QString("Can not query data from database server!");
    qCritical()<<QString("Error: %1").arg(query.lastError().text());
    //statusBar()->showMessage(tr("Error! Can not query data from database server!"));
   }
   queryString = QString("update contacts set ContactsList = '%1' , ContactsListVersion = %2 where UID = '%3'") .arg(contactsList).arg(localContactsListVersion).arg(User::instance()->getUserID());
   query = queryDatabase(queryString, false);
   if(query.lastError().type() != QSqlError::NoError){qDebug()<<"Query Error:"<<query.lastError().text();}
   qDebug()<<"Update Remote Server Contacts List Version";
   User::instance()->setContactsList(contactsList);
   User::instance()->setRemoteServerContactsListVersion(localContactsListVersion);
   //qDebug()<<"contactsList~~~~"<<contactsList;
  }


 }


 qDebug()<<"-------------contactsList:"<<User::instance()->getContactsList();
 //queryString = QString("select UID, Role, Dept, TrueName, NickName, HeadPortrait, LastLoginIP from contacts where UID in (%1) ") .arg(User::instance()->getContactsList());
 queryString = QString("select UID, Role, Dept, TrueName, NickName, Icon, LastLoginIP from contacts where UID in (%1) ") .arg(User::instance()->getContactsList());
query.clear();
 query = queryDatabase(queryString, true);
 query.first();
 qDebug()<<"-------------queryString:"<<queryString;
 qDebug()<<"-------------query.size:"<<query.size();
 record = query.record();

 while (query.next()){
  QString contactUID = QVariant(query.value(record.indexOf("UID"))).toString();
  int contactsRoleID = QVariant(query.value(record.indexOf("Role"))).toInt();
  QString contactDept = QVariant(query.value(record.indexOf("Dept"))).toString();
  QString contactTrueName = QVariant(query.value(record.indexOf("TrueName"))).toString();
  QString contactNickName = QVariant(query.value(record.indexOf("NickName"))).toString();
  QString contactHeadPortrait = QVariant(query.value(record.indexOf("HeadPortrait"))).toString();
  QString contactLastLoginIP = QVariant(query.value(record.indexOf("LastLoginIP"))).toString();

  ContactBase *contact = new ContactBase();
  contact->setUID(contactUID);
  contact->setRoleID(contactsRoleID);
  contact->setDept(contactDept);
  contact->setHeadPortrait(contactHeadPortrait);
  contact->setTrueName(contactTrueName);
  contact->setNickName(contactNickName);
  contact->setLastLoginIP(contactLastLoginIP);

  contactHash.insert(contactUID, contact);

 }

 record.clearValues();
 queryString = QString("select * from group");
 query = queryDatabase(queryString, true);
 record = query.record();
 while (query.next()){
  int contactGroupID = QVariant(query.value(record.indexOf("ID"))).toInt();
  QString contactGroupName = QVariant(query.value(record.indexOf("Name"))).toString();
  int contactGroupParentID = QVariant(query.value(record.indexOf("ParentCategory"))).toInt();
  QString contactGroupRemark = QVariant(query.value(record.indexOf("Remark"))).toString();

  ContactGroup *group = new ContactGroup();
  group->setGroupID(contactGroupID);
  group->setGroupName(contactGroupName);
  group->setParentGroupID(contactGroupParentID);

  contactGroupHash.insert(contactGroupID, group);

 }



 qDebug()<<"ContactsManager::slotUpdateContactsInfo()~~~~";

}

*/


/*
 void ContactsManager::slotLoadContacts(ItemBoxWidget *widgetBox, bool loadFromFile, const QString &string){

 QString file; // = QApplication::applicationDirPath()+QDir::separator()+"contacts.xml";

  if(string.isEmpty() || !QFile(string).exists()){
   file = ":/text/resources/text/default_contacts.xml";
  }


  if(loadFromFile){
   widgetBox->setFileName(file);
   widgetBox->load();
  }else{
   widgetBox->loadContents(string);
  }


  qDebug()<<"~~~~~~~~~~~~~ Load Contacts ~~~~~~~~~~~~~~";


}
*/


QSqlQuery ContactsManager::queryDatabase(const QString & queryString, bool localConfigDatabase) {

    QSqlQuery query;
    DatabaseUtility du;


    //                QString userPrivateDataFilePath = Settings::instance()->getUserPrivateDataFilePath(IMUser::instance()->getUserID());

    if(localConfigDatabase){
        query = du.queryDatabase(queryString,
                                 LOCAL_USERDATA_DB_CONNECTION_NAME,
                                 LOCAL_USERDATA_DB_DRIVER,
                                 "",
                                 0,
                                 "",
                                 "",
                                 userPrivateDataFilePath,
                                 HEHUI::SQLITE);
    }else{
        query = du.queryDatabase(queryString,
                                 REMOTE_SITOY_COMPUTERS_DB_CONNECTION_NAME,
                                 REMOTE_SITOY_COMPUTERS_DB_DRIVER,
                                 REMOTE_SITOY_COMPUTERS_DB_SERVER_HOST,
                                 REMOTE_SITOY_COMPUTERS_DB_SERVER_PORT,
                                 REMOTE_SITOY_COMPUTERS_DB_USER_NAME,
                                 REMOTE_SITOY_COMPUTERS_DB_USER_PASSWORD,
                                 REMOTE_SITOY_COMPUTERS_DB_NAME,
                                 HEHUI::MYSQL);
    }

    return query;


}


QSqlQuery ContactsManager::queryDatabase(const QString & queryString, const QString &connectionName, const QString &driver,
                                         const QString &host, int port, const QString &user, const QString &passwd,
                                         const QString &databaseName, HEHUI::DatabaseType databaseType) {


    QSqlQuery query;
    DatabaseUtility du(this);

    query = du.queryDatabase(queryString, connectionName, driver, host, port, user, passwd, databaseName, databaseType);

    return query;

}





} //namespace HEHUI
