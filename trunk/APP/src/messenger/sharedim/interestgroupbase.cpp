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

InterestGroupBase::InterestGroupBase(quint32 groupID, QObject *parent)
    :QObject(parent), groupID(groupID)
{
	
    groupTypeID = 0;
//    groupID = 0;
    parentGroupID = 0;
    groupName = "";
    creatorID = "";
    creationTime = QDateTime();
    groupInfoVersion = 0;
    memberListInfoVersion = 0;
    
    description = "";
    announcement = "";
    remark = "";

//    lastUpdateTime = QDateTime();
    
    updatedPropertiesMutex =new QMutex();
    
    

}

InterestGroupBase::~InterestGroupBase() {
	// TODO Auto-generated destructor stub
}

bool InterestGroupBase::isNull(){
    return ((groupID < 1) || (groupName.isEmpty()) || (creatorID.isEmpty()));
}

bool InterestGroupBase::hasMember(const QString &memberUserID){
    QList<QString> members = membersHash.keys();
    if(members.contains(memberUserID)){
        return true;
    }
    
    return false;
}

quint32 InterestGroupBase::memberRole(const QString &memberUserID){
    return membersHash.value(memberUserID);
}

QString InterestGroupBase::getUpdateSQLStatement() const{
    QMutexLocker locker(updatedPropertiesMutex);
    
    QStringList sqlstatements;
    QList<IM::PropertyIDOfGroup> keys = updatedProperties.keys();
    foreach (IM::PropertyIDOfGroup propertyID, keys) {
        QString propertyName = databaseColumnName(propertyID);
        if(propertyName.isEmpty()){
            qCritical()<<QString("Unknown Property ID '%1'!").arg(propertyID);
            continue;
        }
        sqlstatements.append(QString(propertyName + "=" + updatedProperties.value(propertyID)));
    }
    
    return sqlstatements.join(" , ");
    
}

void InterestGroupBase::addUpdatedProperty(IM::PropertyIDOfGroup propertyID, const QString &value){

    QMutexLocker locker(updatedPropertiesMutex);
    updatedProperties.insert(propertyID, value);
    
}

void InterestGroupBase::clearUpdatedProperties(){
    QMutexLocker locker(updatedPropertiesMutex);
    
    updatedProperties.clear();
    
}

QString InterestGroupBase::databaseColumnName(IM::PropertyIDOfGroup propertyID) const{
    
    QString columnName = "";
    switch(propertyID){
    case IM::PIG_GroupID:
        columnName = "ID";
        break;
    case IM::PIG_GroupTypeID:
        columnName = "TypeID";
        break;
    case IM::PIG_ParentGroupID:
        columnName = "ParentGroup";
        break;
    case IM::PIG_CreatorID:
        columnName = "Creator";
        break;
    case IM::PIG_GroupName:
        columnName = "GroupName";
        break;
    case IM::PIG_CreationTime:
        columnName = "CreationTime";
        break;
    case IM::PIG_GroupInfoVersion:
        columnName = "GroupInfoVersion";
        break;
        
    case IM::PIG_MemberListInfoVersion:
        columnName = "MemberListVersion";
        break;  
    case IM::PIG_Description:
        columnName = "Description";
        break;         
    case IM::PIG_Announcement:
        columnName = "Announcement";
        break;  
    case IM::PIG_Remark :
        columnName = "Remark";
        break; 
//    case IM::PIG_LastUpdateTime :
//        columnName = "LastUpdateTime";
//        break;
      
        
    default:
        columnName = "";




    }

    return columnName;



}

void InterestGroupBase::setGroupInfoString(const QString &infoString){
    if(infoString.trimmed().isEmpty()){
        return;
    }
    
    QStringList infoList = infoString.split(QString(PACKET_DATA_SEPARTOR));
    
    this->groupTypeID = infoList.at(0).toUInt();
    addUpdatedProperty(IM::PIG_GroupTypeID, QString::number(groupTypeID));
    
    this->parentGroupID = infoList.at(1).toUInt();
    addUpdatedProperty(IM::PIG_ParentGroupID, QString::number(parentGroupID));
    
    this->creatorID = infoList.at(2);
    addUpdatedProperty(IM::PIG_CreatorID, "'"+creatorID+"'");
    
    this->groupName = infoList.at(3);
    addUpdatedProperty(IM::PIG_GroupName, "'"+groupName+"'");
    
    this->creationTime = QVariant(infoList.at(4)).toDateTime();
    addUpdatedProperty(IM::PIG_CreationTime, "'"+infoList.at(5)+"'");
    
    this->groupInfoVersion = infoList.at(5).toUInt();
    addUpdatedProperty(IM::PIG_GroupInfoVersion, QString::number(groupInfoVersion));
    
    this->memberListInfoVersion = infoList.at(6).toUInt();
    addUpdatedProperty(IM::PIG_MemberListInfoVersion, QString::number(memberListInfoVersion));
    
    this->description = infoList.at(7);
    addUpdatedProperty(IM::PIG_Description, "'"+description+"'");
    
    this->announcement = infoList.at(8);
    addUpdatedProperty(IM::PIG_Announcement, "'"+announcement+"'");
    
    this->remark = infoList.at(9);
    addUpdatedProperty(IM::PIG_Remark, "'"+remark+"'");
    

    
    
    
}

QString InterestGroupBase::getGroupInfoString(){
    QStringList infoList;
    infoList << QString::number(groupTypeID)
             << QString::number(parentGroupID) << creatorID << groupName
             << creationTime.toString("yyyy.MM.dd hh:mm:ss")
             <<QString::number(groupInfoVersion) <<QString::number(memberListInfoVersion)
             << description << announcement <<remark
                ;
    
    return infoList.join(QString(PACKET_DATA_SEPARTOR));
    
    
}

void InterestGroupBase::addMember(const QString &memberuserID, quint32 memberRole){

    membersHash.insert(memberuserID, memberRole);
}

void InterestGroupBase::deleteMember(const QString &memberuserID){

    membersHash.remove(memberuserID);
}

QStringList InterestGroupBase::members() const{
    return membersHash.keys();
}











}
