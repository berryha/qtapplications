/*
 ****************************************************************************
 * imuserbase.cpp
 *
 * Created on: 2008-10-14
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
  * Last Modified on: 2010-07-03
  * Last Modified by: 贺辉
  ***************************************************************************
*/




#include "imuserbase.h"
#include "HHSharedCore/hcryptography.h"


namespace HEHUI {

IMUserBase::IMUserBase(QObject *parent)
    :User("", "","", parent)
{

    init();

}

IMUserBase::IMUserBase(const QString &userID, QObject *parent)
    :User(userID, "", "", parent)
{

    init();

}

IMUserBase::~IMUserBase() {



}

void IMUserBase::init(){

    userRole = 0;

    //    systemGroupsHash = QHash<quint32, QString>();
    //    personalInterestGroupsHash = QHash<quint32, QString> ();
    //    personalContacts = QHash<quint32, QString> ();
    
    //personalContactGroupsHash = QHash<QString, QStringList> ();
    personalContactGroupsHash.clear();

    blacklistInfoVersion = 0;
    blacklist.clear();

    interestGroups.clear();
    
    personalDetailInfoVersion = 0;
    personalContactGroupsInfoVersion = 0;
    interestGroupInfoVersion = 0;
    

    onlineState = IM::ONLINESTATE_OFFLINE;
    friendshipApply = FA_AUTO_ACCEPT;
    shortTalk = ST_PROMPT;

    sessionEncryptionKey = encryptedPassword();
    
    updatedPropertiesMutex =new QMutex();
    
    defaultGroupName = tr("Friends");

    
    

}

QByteArray IMUserBase::encryptedPassword() const{

    return Cryptography::MD5(Cryptography::SHA1(getPassword().toUtf8()));

}

void IMUserBase::addUpdatedPersonalInfoProperty(IM::PropertyIDOfUser propertyID, const QString &value, bool summaryInfo){

    QMutexLocker locker(updatedPropertiesMutex);
    if(summaryInfo){
        updatedSummaryInfoProperties.insert(propertyID, value);
    }else{
        updatedDetailInfoProperties.insert(propertyID, value);
    }
    
}

QString IMUserBase::getUpdateSQLStatement(bool summaryInfo) const{
    QMutexLocker locker(updatedPropertiesMutex);
    
    QStringList sqlstatements;
    if(summaryInfo){
        QList<IM::PropertyIDOfUser> keys = updatedSummaryInfoProperties.keys();
        foreach (IM::PropertyIDOfUser propertyID, keys) {
            QString propertyName = databaseColumnName(propertyID);
            if(propertyName.isEmpty()){
                qCritical()<<QString("Unknown Property ID '%1'!").arg(propertyID);
                continue;
            }
            sqlstatements.append(QString(propertyName + "=" + updatedSummaryInfoProperties.value(propertyID)));
        }
    }else{
        QList<IM::PropertyIDOfUser> keys = updatedDetailInfoProperties.keys();
        foreach (IM::PropertyIDOfUser propertyID, keys) {
            QString propertyName = databaseColumnName(propertyID);
            if(propertyName.isEmpty()){
                qCritical()<<QString("Unknown Property ID '%1'!").arg(propertyID);
                continue;
            }
            sqlstatements.append(QString(propertyName + "=" + updatedDetailInfoProperties.value(propertyID)));
        }

    }

    return sqlstatements.join(" , ");
    
}

void IMUserBase::clearUpdatedProperties(){
    QMutexLocker locker(updatedPropertiesMutex);
    
    updatedSummaryInfoProperties.clear();
    updatedDetailInfoProperties.clear();
    
    
}

//QString IMUserBase::databaseColumnName(IM::PropertyID propertyID) const{
//    QString columnName = "";
//    switch(propertyID){
//    case IM::PI_UserID:
//        columnName = "UserID";
//        break;
//    case IM::PI_Password:
//        columnName = "UserPassword";
//        break;
//    case IM::PI_TrueName:
//        columnName = "TrueName";
//        break;
//    case IM::PI_NickName:
//        columnName = "NickName";
//        break;
//    case IM::PI_Gender:
//        columnName = "Gender";
//        break;
//    case IM::PI_Age:
//        columnName = "Age";
//        break;
//    case IM::PI_Face:
//        columnName = "Face";
//        break;

//    case IM::PI_PersonalContactGroupsInfoString:
//        columnName = "PersonalContactGroupsInfo";
//        break;
//    case IM::PI_PersonalContactGroupsInfoVersion:
//        columnName = "PersonalContactGroupsInfoVersion";
//        break;
//    case IM::PI_PersonalInterestGroupsInfoString:
//        columnName = "PersonalInterestGroupsInfo";
//        break;
//    case IM::PI_PersonalInterestGroupsInfoVersion:
//        columnName = "PersonalInterestGroupsInfoVersion";
//        break;
//    case IM::PI_SystemGroupsInfoString:
//        columnName = "SystemGroupsInfo";
//        break;
//    case IM::PI_SystemGroupsInfoVersion:
//        columnName = "SystemGroupsInfoVersion";
//        break;
//    case IM::PI_PersonalInfoVersion:
//        columnName = "PersonalInfoVersion";
//        break;


//    case IM::PI_HomeAddress:
//        columnName = "HomeAddress";
//        break;
//    case IM::PI_HomePhoneNumber:
//        columnName = "HomePhoneNumber";
//        break;
//    case IM::PI_HomeZipCode:
//        columnName = "HomeZipCode";
//        break;
//    case IM::PI_PersonalHomepage:
//        columnName = "PersonalHomepage";
//        break;
//    case IM::PI_PersonalEmailAddress:
//        columnName = "PersonalEmailAddress";
//        break;
//    case IM::PI_LastLoginTime:
//        columnName = "LastLoginTime";
//        break;
//    case IM::PI_LastLoginHostAddress:
//        columnName = "LastLoginHostAddress";
//        break;
//    case IM::PI_LastLoginHostPort:
//        columnName = "LastLoginHostPort";
//        break;
//    case IM::PI_QuestionForSecurity:
//        columnName = "QuestionForSecurity";
//        break;
//    case IM::PI_AnswerForSecurity:
//        columnName = "AnswerForSecurity";
//        break;

//    case IM::PI_CompanyName:
//        columnName = "CompanyName";
//        break;
//    case IM::PI_JobTitle:
//        columnName = "JobTitle";
//        break;
//    case IM::PI_BusinessAddress:
//        columnName = "BusinessAddress";
//        break;
//    case IM::PI_BusinessPhoneNumber:
//        columnName = "BusinessPhoneNumber";
//        break;
//    case IM::PI_BusinessZipCode:
//        columnName = "BusinessZipCode";
//        break;
//    case IM::PI_BusinessFaxNumber:
//        columnName = "BusinessFaxNumber";
//        break;
//    case IM::PI_BusinessHomepage:
//        columnName = "BusinessHomepage";
//        break;
//    case IM::PI_BusinessEmailAddress:
//        columnName = "BusinessEmailAddress";
//        break;
//    case IM::PI_RegistrationTime:
//        columnName = "RegistrationTime";
//        break;
//    case IM::PI_LoginTimes:
//        columnName = "LoginTimes";
//        break;
//    case IM::PI_FriendshipApply:
//        columnName = "FriendshipApply";
//        break;
//    case IM::PI_ShortTalk:
//        columnName = "ShortTalk";
//        break;

//    case IM::PI_Role:
//        columnName = "Role";
//        break;

//    default:
//        columnName = "";




//    }

//    return columnName;



//}



quint32 IMUserBase::updatePersonalContactGroupsInfoVersion(){
    this->personalContactGroupsInfoVersion++;
    
    addUpdatedPersonalInfoProperty(IM::PI_PersonalContactGroupsInfoVersion, QString::number(personalContactGroupsInfoVersion), true);

    return personalContactGroupsInfoVersion;
}

QString IMUserBase::getDefaultGroupName() const{
    return defaultGroupName;
}


void IMUserBase::setPersonalInfoString(const QString &personalInfoString, bool summaryInfo){
    qDebug()<<"--IMUserBase::setPersonalSummaryInfo(...)"<<" personalSummaryInfo:"<<personalInfoString;
    
    QStringList infoList = personalInfoString.split(QString(PACKET_DATA_SEPARTOR));
    if(infoList.at(0) != this->getUserID()){
        return;
    }

    if(summaryInfo){

        QString trueNameString = infoList.at(1);
        setTrueName(trueNameString);
        addUpdatedPersonalInfoProperty(IM::PI_TrueName, "'"+trueNameString+"'", summaryInfo);

        QString nickNameString = infoList.at(2);
        setNickName(nickNameString);
        addUpdatedPersonalInfoProperty(IM::PI_NickName, "'"+nickNameString+"'", summaryInfo);

        QString genderString = infoList.at(3);
        setGender(Gender(genderString.toUInt()));
        addUpdatedPersonalInfoProperty(IM::PI_Gender, genderString, summaryInfo);

        QString ageString = infoList.at(4);
        setAge(ageString.toUInt());
        addUpdatedPersonalInfoProperty(IM::PI_Age, ageString, summaryInfo);

        QString faceString = infoList.at(5);
        setFace(faceString);
        addUpdatedPersonalInfoProperty(IM::PI_Face, "'"+faceString+"'", summaryInfo);

        QString personalContactGroupsVersionString = infoList.at(6);
        setPersonalContactGroupsVersion(personalContactGroupsVersionString.toUInt());
        addUpdatedPersonalInfoProperty(IM::PI_PersonalContactGroupsInfoVersion, personalContactGroupsVersionString, summaryInfo);

        QString interestGroupInfoString = infoList.at(7);
        setInterestGroupInfoVersion(interestGroupInfoString.toUInt());
        addUpdatedPersonalInfoProperty(IM::PI_InterestGroupsInfoVersion, interestGroupInfoString, summaryInfo);

        QString blacklistInfoString = infoList.at(8);
        setBlacklistInfoVersion(blacklistInfoString.toUInt());
        addUpdatedPersonalInfoProperty(IM::PI_BlacklistInfoVersion, blacklistInfoString, summaryInfo);

        QString personalSummaryInfoVersionString = infoList.at(9);
        setPersonalSummaryInfoVersion(personalSummaryInfoVersionString.toUInt());
        addUpdatedPersonalInfoProperty(IM::PI_PersonalSummaryInfoVersion, personalSummaryInfoVersionString, summaryInfo);

        QString personalDetailInfoVersionString = infoList.at(10);
        setPersonalDetailInfoVersion(personalDetailInfoVersionString.toUInt());
        addUpdatedPersonalInfoProperty(IM::PI_PersonalDetailInfoVersion, personalDetailInfoVersionString, summaryInfo);

        QString friendshipApplyString = infoList.at(11);
        setFriendshipApply(FriendshipApply(friendshipApplyString.toUInt()));
        addUpdatedPersonalInfoProperty(IM::PI_FriendshipApply, friendshipApplyString, summaryInfo);

        QString shortTalkString = infoList.at(12);
        setShortTalk(ShortTalk(shortTalkString.toUInt()));
        addUpdatedPersonalInfoProperty(IM::PI_ShortTalk, shortTalkString, summaryInfo);

        QString userRoleString = infoList.at(13);
        setUserRole(userRoleString.toUInt());
        addUpdatedPersonalInfoProperty(IM::PI_Role, userRoleString, summaryInfo);

        QString descriptionString = infoList.at(14);
        setDescription(descriptionString);
        addUpdatedPersonalInfoProperty(IM::PI_Description, descriptionString, summaryInfo);

        QString accountStateString = infoList.at(15);
        setAccountState(AccountState(accountStateString.toUInt()));
        addUpdatedPersonalInfoProperty(IM::PI_AccountState, accountStateString, summaryInfo);

    }else{

        QString homeAddressString = infoList.at(1);
        setHomeAddress(homeAddressString);
        addUpdatedPersonalInfoProperty(IM::PI_HomeAddress, "'"+homeAddressString+"'", summaryInfo);

        QString homePhoneNumberString = infoList.at(2);
        setHomePhoneNumber(homePhoneNumberString);
        addUpdatedPersonalInfoProperty(IM::PI_HomePhoneNumber, "'"+homePhoneNumberString+"'", summaryInfo);

        QString homeZipCodeString = infoList.at(3);
        setHomeZipCode(homeZipCodeString);
        addUpdatedPersonalInfoProperty(IM::PI_HomeZipCode, "'"+homeZipCodeString+"'", summaryInfo);

        QString personalHomepageString = infoList.at(4);
        setPersonalHomepage(personalHomepageString);
        addUpdatedPersonalInfoProperty(IM::PI_PersonalHomepage, "'"+personalHomepageString+"'", summaryInfo);

        QString personalEmailAddressString = infoList.at(5);
        setPersonalEmailAddress(personalEmailAddressString);
        addUpdatedPersonalInfoProperty(IM::PI_PersonalEmailAddress, "'"+personalEmailAddressString+"'", summaryInfo);

        QString companyNameString = infoList.at(6);
        setPersonalEmailAddress(companyNameString);
        addUpdatedPersonalInfoProperty(IM::PI_CompanyName, "'"+companyNameString+"'", summaryInfo);

        QString jobTitleString = infoList.at(7);
        setJobTitle(jobTitleString);
        addUpdatedPersonalInfoProperty(IM::PI_JobTitle, "'"+jobTitleString+"'", summaryInfo);

        QString businessAddressString = infoList.at(8);
        setBusinessAddress(businessAddressString);
        addUpdatedPersonalInfoProperty(IM::PI_BusinessAddress, "'"+businessAddressString+"'", summaryInfo);

        QString businessPhoneNumberString = infoList.at(9);
        setBusinessPhoneNumber(businessPhoneNumberString);
        addUpdatedPersonalInfoProperty(IM::PI_BusinessPhoneNumber, "'"+businessPhoneNumberString+"'", summaryInfo);

        QString businessZipCodeString = infoList.at(10);
        setBusinessZipCode(businessZipCodeString);
        addUpdatedPersonalInfoProperty(IM::PI_BusinessZipCode, "'"+businessZipCodeString+"'", summaryInfo);

        QString businessFaxNumberString = infoList.at(11);
        setBusinessFaxNumber(businessFaxNumberString);
        addUpdatedPersonalInfoProperty(IM::PI_BusinessFaxNumber, "'"+businessFaxNumberString+"'", summaryInfo);

        QString businessHomepageString = infoList.at(12);
        setBusinessHomepage(businessHomepageString);
        addUpdatedPersonalInfoProperty(IM::PI_BusinessHomepage, "'"+businessHomepageString+"'", summaryInfo);

        QString businessEmailAddressString = infoList.at(13);
        setBusinessEmailAddress(businessEmailAddressString);
        addUpdatedPersonalInfoProperty(IM::PI_BusinessEmailAddress, "'"+businessEmailAddressString+"'", summaryInfo);

    }


}

QString IMUserBase::getPersonalInfoString(bool requestSummaryInfo) const{
    QStringList infoList;
    if(requestSummaryInfo){
        infoList << this->getUserID()
                 << this->getTrueName() << this->getNickName()
                 << QString::number(this->getGender()) << QString::number(getAge()) << this->getFace()
                 << QString::number(this->getPersonalContactGroupsVersion()) << QString::number(this->getInterestGroupInfoVersion())
                 << QString::number(this->getBlacklistInfoVersion()) << QString::number(this->getPersonalSummaryInfoVersion()) << QString::number(this->getPersonalDetailInfoVersion())
                 << QString::number(this->getFriendshipApply()) << QString::number(this->getShortTalk()) << QString::number(this->getUserRole())
                 << getdescription() << QString::number(getAccountState())
                    ;
    }else{
        infoList << this->getUserID()
                 << this->getHomeAddress() << this->getHomePhoneNumber() << this->getHomeZipCode()
                 << this->getPersonalHomepage() << this->getPersonalEmailAddress()
                 << this->getCompanyName() << this->getJobTitle() << this->getBusinessAddress()
                 << this->getBusinessPhoneNumber() << this->getBusinessZipCode() << this->getBusinessFaxNumber()
                 << this->getBusinessHomepage() << this->getBusinessEmailAddress()
                 //<< this->getRegistrationTime().toString("yyyy-MM-dd HH:mm:ss")
                    ;
    }

    return infoList.join(QString(PACKET_DATA_SEPARTOR));

}

void IMUserBase::setContactGroupsInfoString(const QString &contactGroupsInfo, const QString &rowSepartor, const QString &fieldSepartor){
    
    foreach (ContactGroupBase *group, personalContactGroupsHash) {
        personalContactGroupsHash.remove(group->getGroupID());
        delete group;
        group = 0;
    }

    if(contactGroupsInfo.trimmed().isEmpty()){
        return;
    }
    
    QStringList groupsInfoList = contactGroupsInfo.split(rowSepartor);
    for(int i=0; i<groupsInfoList.size(); i++) {
        ContactGroupBase * contactGroup = new ContactGroupBase();
        if(!contactGroup->setGroupInfoFromString(groupsInfoList.at(i), fieldSepartor)){
            delete contactGroup;
            contactGroup = 0;
            continue;
        }
        personalContactGroupsHash.insert(contactGroup->getGroupID(), contactGroup);
    }
    

}

QString IMUserBase::getContactGroupsInfoString(const QString &rowSepartor, const QString &fieldSepartor) const{
    
    if(personalContactGroupsHash.isEmpty()){
        return "";
    }

    QStringList groupsInfo;
    foreach (ContactGroupBase *group, personalContactGroupsHash) {
        groupsInfo.append(group->getGroupInfoAsString(fieldSepartor));
    }
    return groupsInfo.join(rowSepartor);


//    QStringList groups = personalContactGroupsHash.keys();
//    foreach (QString groupName, groups) {
//        QStringList members = personalContactGroupsHash.value(groupName);
//        groupsInfo.append(groupName + CONTACT_INFO_SEPARATOR + members.join(CONTACT_INFO_SEPARATOR));
//    }

//    return groupsInfo.join(GROUP_INFO_SEPARATOR);

}

QStringList IMUserBase::getContacts() const{
    QStringList contacts;
    foreach (ContactGroupBase *contactGroup, personalContactGroupsHash.values()) {
        contacts << contactGroup->getMembers();
    }

    return contacts;

}

bool IMUserBase::addOrDeleteContact(const QString &contactID, quint32 groupID, bool add){
    
    if(contactID.trimmed().isEmpty()){
        return false;
    }

    ContactGroupBase *contactGroup = personalContactGroupsHash.value(groupID);
    if(add){
        contactGroup->addMember(contactID);
    }else{
        contactGroup->deleteMember(contactID);
    }


    updatePersonalContactGroupsInfoVersion();
    //    addUpdatedProperty(IM::PI_PersonalContactGroupsInfoString, "'"+getContactGroupsInfoString()+"'");
    
    return true;

}

bool IMUserBase::moveContact(const QString &contactID, quint32 oldGroupID, quint32 newGroupID){
    
    ContactGroupBase *oldContactGroup = personalContactGroupsHash.value(oldGroupID);
    ContactGroupBase *newContactGroup = personalContactGroupsHash.value(newGroupID);
    Q_ASSERT(oldContactGroup);
    Q_ASSERT(newContactGroup);

    oldContactGroup->deleteMember(contactID);
    newContactGroup->addMember(contactID);
    
    updatePersonalContactGroupsInfoVersion();
//    addUpdatedPersonalInfoProperty(IM::PI_PersonalContactGroupsInfoString, "'"+getContactGroupsInfoString()+"'");

    return true;

}

//bool IMUserBase::addContactToBlacklist(const QString &contactID){
//    if(blacklist.contains(contactID)){return false;}
//    blacklist.append(contactID);

//    updateBlacklistInfoVersion();
//    addUpdatedProperty(IM::PI_Blacklist, "'"+getBlacklistInfoString()+"'");

//}

//bool IMUserBase::deleteContactFromBlacklist(const QString &contactID){
//    blacklist.removeAll(contactID);

//    updateBlacklistInfoVersion();
//    addUpdatedProperty(IM::PI_Blacklist, "'"+getBlacklistInfoString()+"'");
//}

bool IMUserBase::joinOrLeaveInterestGroup(const QString &interestGroupID, bool join){

    if(join){
        if(interestGroups.contains(interestGroupID)){
            return false;
        }else{
            interestGroups.append(interestGroupID);
        }
    }else{
        interestGroups.removeAll(interestGroupID);
    }

    updateInterestGroupInfoVersion();

//    addUpdatedPersonalInfoProperty(IM::PI_InterestGroupsInfoString, "'"+interestGroups.join(",")+"'");

    return true;
}

quint32 IMUserBase::updateInterestGroupInfoVersion(){

    this->interestGroupInfoVersion++;
    addUpdatedPersonalInfoProperty(IM::PI_InterestGroupsInfoVersion, QString::number(interestGroupInfoVersion), true);

    return interestGroupInfoVersion;

}

bool IMUserBase::addOrDeleteBlacklistedContact(const QString &contactID,  bool addToBlacklist){

    if(contactID.trimmed().isEmpty()){return false;}

    if(addToBlacklist){
        if(blacklist.contains(contactID)){return false;}
        blacklist.append(contactID);
    }else{
        blacklist.removeAll(contactID);
    }

    updateBlacklistInfoVersion();
//    addUpdatedPersonalInfoProperty(IM::PI_Blacklist, "'"+getBlacklistInfoString()+"'");
    
    return true;
}

quint32 IMUserBase::updateBlacklistInfoVersion(){
    this->blacklistInfoVersion++;
    addUpdatedPersonalInfoProperty(IM::PI_BlacklistInfoVersion, QString::number(blacklistInfoVersion), true);

    return blacklistInfoVersion;

}

QString IMUserBase::getBlacklistInfoString(){
    return blacklist.join(CONTACT_INFO_SEPARATOR);
}

void IMUserBase::setBlacklistInfoString(const QString &blacklistInfoString){
    if(blacklistInfoString.trimmed().isEmpty()){
        blacklist.clear();
    }else{
        this->blacklist = blacklistInfoString.split(CONTACT_INFO_SEPARATOR);
    }
//    addUpdatedPersonalInfoProperty(IM::PI_Blacklist, "'"+getBlacklistInfoString()+"'");

}


//bool IMUserBase::hasContact(const QString &contactID){

//    bool has = false;
//    QStringList groups = personalContactGroupsHash.keys();
//    foreach (QString groupName, groups) {
//        QStringList members = personalContactGroupsHash.value(groupName);
//        if(members.contains(contactID)){
//            has = true;
//            break;
//        }
//    }

//    return has;
//}

//QString IMUserBase::groupNameThatContactBelongsTo(const QString &contactID) const{

//    QString gpName = "";
//    QStringList groups = personalContactGroupsHash.keys();
//    foreach (QString groupName, groups) {
//        QStringList members = personalContactGroupsHash.value(groupName);
//        if(members.contains(contactID)){
//            gpName = groupName;
//            break;
//        }
//    }

//    return gpName;

//}













} //namespace HEHUI
