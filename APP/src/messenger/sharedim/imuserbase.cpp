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
    
    personalInfoVersion = 0;
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

void IMUserBase::addUpdatedProperty(IM::PropertyIDOfUser propertyID, const QString &value){

    QMutexLocker locker(updatedPropertiesMutex);
    updatedProperties.insert(propertyID, value);
    
}

QString IMUserBase::getUpdateSQLStatement() const{
    QMutexLocker locker(updatedPropertiesMutex);
    
    QStringList sqlstatements;
    QList<IM::PropertyIDOfUser> keys = updatedProperties.keys();
    foreach (IM::PropertyIDOfUser propertyID, keys) {
        QString propertyName = databaseColumnName(propertyID);
        if(propertyName.isEmpty()){
            qCritical()<<QString("Unknown Property ID '%1'!").arg(propertyID);
            continue;
        }
        sqlstatements.append(QString(propertyName + "=" + updatedProperties.value(propertyID)));
    }
    
    return sqlstatements.join(" , ");
    
}

void IMUserBase::clearUpdatedProperties(){
    QMutexLocker locker(updatedPropertiesMutex);
    
    updatedProperties.clear();
    
    
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
    
    addUpdatedProperty(IM::PI_PersonalContactGroupsInfoVersion, QString::number(personalContactGroupsInfoVersion));

    return personalContactGroupsInfoVersion;
}

QString IMUserBase::getDefaultGroupName() const{
    return defaultGroupName;
}


void IMUserBase::setPersonalSummaryInfo(const QString &personalSummaryInfo){
    qDebug()<<"--IMUserBase::setPersonalSummaryInfo(...)"<<" personalSummaryInfo:"<<personalSummaryInfo;
    
    QStringList infoList = personalSummaryInfo.split(QString(PACKET_DATA_SEPARTOR));
    if(infoList.at(0) != this->getUserID()){
        return;
    }
    
    QString nickNameString = infoList.at(1);
    setNickName(nickNameString);
    addUpdatedProperty(IM::PI_NickName, "'"+nickNameString+"'");
    
    QString genderString = infoList.at(2);
    setGender(Gender(genderString.toUInt()));
    addUpdatedProperty(IM::PI_Gender, genderString);
    
    QString ageString = infoList.at(3);
    setAge(ageString.toUInt());
    addUpdatedProperty(IM::PI_Age, ageString);
    
    QString faceString = infoList.at(4);
    setFace(faceString);
    addUpdatedProperty(IM::PI_Face, "'"+faceString+"'");
    
    QString friendshipApplyString = infoList.at(5);
    setFriendshipApply(FriendshipApply(friendshipApplyString.toUInt()));
    addUpdatedProperty(IM::PI_FriendshipApply, friendshipApplyString);
    
    QString personalInfoVersionString = infoList.at(6);
    setPersonalInfoVersion(personalInfoVersionString.toUInt());
    addUpdatedProperty(IM::PI_PersonalInfoVersion, personalInfoVersionString);



}

QString IMUserBase::getPersonalSummaryInfo() const{
    QStringList infoList;
    infoList << this->getUserID() << this->getNickName()
             << QString::number(this->getGender()) << QString::number(getAge())
             << this->getFace() << QString::number(this->getFriendshipApply())
             << QString::number(getPersonalInfoVersion())
                ;

    return infoList.join(QString(PACKET_DATA_SEPARTOR));

}

void IMUserBase::setContactGroupsInfoString(const QString &contactGroupsInfo){
    
    if(contactGroupsInfo.trimmed().isEmpty()){
        //TODO:
        personalContactGroupsHash.clear();
        return;
    }
    
    QStringList groupsInfoList = contactGroupsInfo.split(QString(GROUP_INFO_SEPARATOR));
    for(int i=0; i<groupsInfoList.size(); i++) {
        QStringList infoList = groupsInfoList.at(i).split(QString(CONTACT_INFO_SEPARATOR));
        if(infoList.isEmpty()){continue;}
        QString groupName = infoList.takeFirst();
        personalContactGroupsHash.insert(groupName, infoList);
    }
    

}

QString IMUserBase::getContactGroupsInfoString() const{
    
    QStringList groupsInfo;
    QStringList groups = personalContactGroupsHash.keys();
    foreach (QString groupName, groups) {
        QStringList members = personalContactGroupsHash.value(groupName);
        groupsInfo.append(groupName + CONTACT_INFO_SEPARATOR + members.join(CONTACT_INFO_SEPARATOR));
    }

    return groupsInfo.join(GROUP_INFO_SEPARATOR);

}

QStringList IMUserBase::getContacts() const{
    QStringList contacts;
    QStringList groups = personalContactGroupsHash.keys();
    foreach (QString groupName, groups) {
        contacts << personalContactGroupsHash.value(groupName);
    }

    return contacts;

}

bool IMUserBase::addOrDeleteContact(const QString &contactID, const QString &groupName, bool add){
    
    if(contactID.trimmed().isEmpty()){
        return false;
    }

    QString gpName = groupName;
    if(groupName.trimmed().isEmpty()){
        gpName = defaultGroupName;
    }

    QStringList members = personalContactGroupsHash.value(gpName);
    if(add){
        if(members.contains(contactID)){return false;}
        members.append(contactID);
        personalContactGroupsHash[gpName] = members;

    }else{
        QStringList groups;
        if(groupName.trimmed().isEmpty()){
            groups = personalContactGroupsHash.keys();
        }else{
            groups.append(gpName);
        }

        foreach (QString group, groups) {
            QStringList groupMembers = personalContactGroupsHash.value(group);
            //if(groupMembers.contains(contactID)){
            groupMembers.removeAll(contactID);
            //}
            personalContactGroupsHash[group] = groupMembers;
        }

    }

    updatePersonalContactGroupsInfoVersion();
    addUpdatedProperty(IM::PI_PersonalContactGroupsInfoString, "'"+getContactGroupsInfoString()+"'");
    
    return true;

}

bool IMUserBase::moveContact(const QString &contactID, const QString &oldGroupName, const QString &newGroupName){
    
    QString oldGPName = oldGroupName;
    if(oldGPName.trimmed().isEmpty()){
        oldGPName = defaultGroupName;
    }
    
    if(oldGPName == newGroupName){
        return false;
    }
    
    QStringList members = personalContactGroupsHash.value(oldGPName);
    if(members.contains(contactID)){
        members.removeAll(contactID);
        personalContactGroupsHash[oldGPName] = members;
    }

    members = personalContactGroupsHash.value(newGroupName);
    members.append(contactID);
    personalContactGroupsHash[newGroupName] = members;
    
    updatePersonalContactGroupsInfoVersion();
    addUpdatedProperty(IM::PI_PersonalContactGroupsInfoString, "'"+getContactGroupsInfoString()+"'");

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

    addUpdatedProperty(IM::PI_InterestGroupsInfoString, "'"+interestGroups.join(",")+"'");

    return true;
}

quint32 IMUserBase::updateInterestGroupInfoVersion(){

    this->interestGroupInfoVersion++;
    addUpdatedProperty(IM::PI_InterestGroupsInfoVersion, QString::number(interestGroupInfoVersion));

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
    addUpdatedProperty(IM::PI_Blacklist, "'"+getBlacklistInfoString()+"'");
    
    return true;
}

quint32 IMUserBase::updateBlacklistInfoVersion(){
    this->blacklistInfoVersion++;
    addUpdatedProperty(IM::PI_BlacklistInfoVersion, QString::number(blacklistInfoVersion));

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
    addUpdatedProperty(IM::PI_Blacklist, "'"+getBlacklistInfoString()+"'");

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
