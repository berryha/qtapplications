/*
 * groupbase.h
 *
 *  Created on: 2011-2-28
 *      Author: hehui
 */

#ifndef GROUPBASE_H_
#define GROUPBASE_H_


#include <QObject>
#include <QDateTime>
#include <QStringList>
#include <QHash>
#include <QMutex>


#include "constants_global_shared.h"

#include "sharedimlib.h"



namespace HEHUI {

class SHAREDIMLIB_API GroupBase : public QObject {
    Q_OBJECT
public:
    GroupBase(quint32 groupID = 0, QObject *parent = 0);
    virtual ~GroupBase();
    
    bool isNull();
    bool hasMember(const QString &memberUserID);
    quint32 memberRole(const QString &memberUserID);
    
    QString getUpdateSQLStatement() const;
    void addUpdatedProperty(IM::PropertyIDOfGroup propertyID, const QString &value);
    void clearUpdatedProperties();
    
    virtual QString databaseColumnName(IM::PropertyIDOfGroup propertyID) const;
    
    void setGroupInfoString(const QString &infoString);
    QString getGroupInfoString();
    
    void addMember(const QString &memberuserID, quint32 memberRole);   
    void deleteMember(const QString &memberuserID);

    QStringList members() const;
    
 
    
    
    
    
    

    QString getAnnouncement() const
    {
        return announcement;
    }

    QDateTime getCreationTime() const
    {
        return creationTime;
    }

    QString getCreatorID() const
    {
        return creatorID;
    }

    QString getDescription() const
    {
        return description;
    }

    quint32 getGroupID() const
    {
        return groupID;
    }

    quint32 getGroupInfoVersion() const
    {
        return groupInfoVersion;
    }

    QString getGroupName() const
    {
        if(groupName.trimmed().isEmpty()){
            return QString::number(groupID);
        }
        return groupName;
    }

    quint32 getGroupTypeID() const
    {
        return groupTypeID;
    }

    quint32 getMemberListInfoVersion() const
    {
        return memberListInfoVersion;
    }

    QHash<QString,quint32> getMembersHash() const
    {
        return membersHash;
    }

    quint32 getParentGroupID() const
    {
        return parentGroupID;
    }

    QString getRemark() const
    {
        return remark;
    }

//    QDateTime getLastUpdateTime() const{
//        return lastUpdateTime;
//    }

    void setAnnouncement(QString announcement)
    {
        this->announcement = announcement;
        addUpdatedProperty(IM::PIG_Announcement, "'"+announcement+"'");
        
    }

    void setCreationTime(QDateTime creationTime)
    {
        this->creationTime = creationTime;
        addUpdatedProperty(IM::PIG_CreationTime, "'"+creationTime.toString("yyyy.MM.dd hh:mm:ss")+"'");
        
    }

    void setCreatorID(const QString &creatorID)
    {
        this->creatorID = creatorID;
        addUpdatedProperty(IM::PIG_CreatorID, "'"+creatorID+"'");
        
    }

    void setDescription(QString description)
    {
        this->description = description;
        addUpdatedProperty(IM::PIG_Description, "'"+description+"'");
        
    }

    void setGroupID(quint32 groupID)
    {
        this->groupID = groupID;
    }

    void setGroupInfoVersion(quint32 groupInfoVersion)
    {
        this->groupInfoVersion = groupInfoVersion;
        addUpdatedProperty(IM::PIG_GroupInfoVersion, QString::number(groupInfoVersion));
        
    }

    void setGroupName(QString groupName)
    {
        this->groupName = groupName;
        addUpdatedProperty(IM::PIG_GroupName, "'"+groupName+"'");
        
    }

    void setGroupTypeID(quint32 groupTypeID)
    {
        this->groupTypeID = groupTypeID;
        addUpdatedProperty(IM::PIG_GroupTypeID, QString::number(groupTypeID));
        
    }

    void setMemberListInfoVersion(quint32 memberListInfoVersion)
    {
        this->memberListInfoVersion = memberListInfoVersion;
        addUpdatedProperty(IM::PIG_MemberListInfoVersion, QString::number(memberListInfoVersion));
    }

    void setMembersHash(QHash<QString,quint32> membersHash)
    {
        this->membersHash = membersHash;
    }

    void setParentGroupID(quint32 parentGroupID)
    {
        this->parentGroupID = parentGroupID;
        addUpdatedProperty(IM::PIG_ParentGroupID, QString::number(parentGroupID));
        
    }

    void setRemark(QString remark)
    {
        this->remark = remark;
        addUpdatedProperty(IM::PIG_Remark, "'"+remark+"'");
        
    }

//    void setLastUpdateTime(QDateTime time){
//        this->lastUpdateTime = time;
//        addUpdatedProperty(IM::PIG_LastUpdateTime, "'"+lastUpdateTime.toString("yyyy.MM.dd hh:mm:ss")+"'");
//    }



protected:

private:
    
    quint32 groupID;
    quint32 groupTypeID;
    quint32 parentGroupID;
    QString groupName;
    QString creatorID;
    QDateTime creationTime;
    quint32 groupInfoVersion;
    quint32 memberListInfoVersion;
    
    QString description;
    QString announcement;
    QString remark;

//    QDateTime lastUpdateTime;


    QHash <QString/*Member's ID*/, quint32/*Member's Role*/> membersHash;


    QHash<IM::PropertyIDOfGroup/*Property ID*/, QString/*SQL Update Statement*/> updatedProperties;
    QMutex *updatedPropertiesMutex;









};

}

#endif /* GROUPBASE_H_ */
