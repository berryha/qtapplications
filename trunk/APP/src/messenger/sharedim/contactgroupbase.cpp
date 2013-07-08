#include "contactgroupbase.h"



namespace HEHUI {

QString ContactGroupBase::Group_Blacklist_Name = QString(tr("Blacklist"));
QString ContactGroupBase::Group_Friends_Name = QString(tr("Friends"));
QString ContactGroupBase::Group_Strangers_Name = QString(tr("Strangers"));

ContactGroupBase::ContactGroupBase(quint32 groupID, const QString &groupName, QObject *parent)
    :IMGroupBase(groupID, groupName, parent)
{


}

void ContactGroupBase::setGroupInfoFromString(const QString &infoString, const QString &fieldSepartor){
    if(infoString.trimmed().isEmpty()){
        return;
    }

    QStringList infoList = infoString.split(fieldSepartor);

    setGroupID(infoList.at(0).toUInt());
    setGroupName(infoList.at(1));
    setGroupInfoVersion(infoList.at(2).toUInt());
    setGroupMemberListInfoVersion(infoList.at(3).toUInt());

}

QString ContactGroupBase::getGroupInfoAsString(const QString &fieldSepartor){
    QStringList infoList;
    infoList << QString::number(getGroupID())
             << getGroupName()
             << QString::number(getGroupInfoVersion())
             <<QString::number(getGroupMemberListInfoVersion())
                ;

    return infoList.join(fieldSepartor);

}

void ContactGroupBase::setMembersFromString(const QString &infoString, const QString &fieldSepartor){
    membersList = infoString.split(fieldSepartor);
}

QString ContactGroupBase::getMembersAsString(const QString &fieldSepartor){
    return membersList.join(fieldSepartor);
}

void ContactGroupBase::addMember(const QString &memberuserID){

    membersList.append(memberuserID);
}

void ContactGroupBase::deleteMember(const QString &memberuserID){

    membersList.removeAll(memberuserID);
}

bool ContactGroupBase::hasMember(const QString &memberUserID){

    return membersList.contains(memberUserID, Qt::CaseInsensitive);
}

QStringList ContactGroupBase::members() const{
    return membersList;
}










} //namespace HEHUI
