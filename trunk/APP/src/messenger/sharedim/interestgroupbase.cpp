/*
 * groupbase.cpp
 *
 *  Created on: 2011-2-28
 *      Author: hehui
 */


#include <QVariant>
#include <QDebug>

#include "interestgroupbase.h"

namespace HEHUI {

InterestGroupBase::InterestGroupBase(quint32 groupID, const QString &groupName, QObject *parent)
    :QObject(groupID, groupName, parent)
{
	
    groupTypeID = 0;
    parentGroupID = 0;
    creationTime = QDateTime();

    description = "";
    announcement = "";
    remark = "";
        
}

InterestGroupBase::~InterestGroupBase() {
	// TODO Auto-generated destructor stub
}

bool InterestGroupBase::isNull(){
    return ((groupID < 1) || (groupName.isEmpty()) || (creatorID.isEmpty()));
}

void InterestGroupBase::setGroupInfoString(const QString &infoString){
    if(infoString.trimmed().isEmpty()){
        return;
    }
    
    QStringList infoList = infoString.split(QString(PACKET_DATA_SEPARTOR));

    setGroupID(infoList.at(0).toUInt());
    setGroupTypeID(infoList.at(1).toUInt());
    setParentGroupID(infoList.at(2).toUInt());
    setCreatorID(infoList.at(3));
    setGroupName(infoList.at(4));
    setCreationTime(QDateTime::fromString(infoList.at(5).toUInt(), "yyyy.MM.dd hh:mm:ss"));
    setGroupInfoVersion(infoList.at(6).toUInt());
    setGroupMemberListInfoVersion(infoList.at(7).toUInt());
    setDescription(infoList.at(8).toUInt());
    setAnnouncement(infoList.at(9).toUInt());
    setRemark(infoList.at(10).toUInt());

    
}

QString InterestGroupBase::getGroupInfoString(){
    QStringList infoList;
    infoList << QString::number(getGroupID())
             << QString::number(groupTypeID) << QString::number(parentGroupID)
             << getCreatorID() << getGroupName() << creationTime.toString("yyyy.MM.dd hh:mm:ss")
             <<QString::number(getGroupInfoVersion()) <<QString::number(getGroupMemberListInfoVersion())
             << getDescription() << getAnnouncement() <<getRemark()
                ;
    
    return infoList.join(QString(PACKET_DATA_SEPARTOR));    
    
}

void InterestGroupBase::addMember(const QString &memberuserID, quint32 memberRole){

    membersHash.insert(memberuserID, memberRole);
}

void InterestGroupBase::deleteMember(const QString &memberuserID){

    membersHash.remove(memberuserID);
}

bool InterestGroupBase::hasMember(const QString &memberUserID){
    QList<QString> members = membersHash.keys();
    return members.contains(memberUserID);
}

QStringList InterestGroupBase::members() const{
    return membersHash.keys();
}

quint32 InterestGroupBase::memberRole(const QString &memberUserID){
    return membersHash.value(memberUserID);
}










}
