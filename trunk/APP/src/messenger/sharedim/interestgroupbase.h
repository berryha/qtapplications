/*
 * groupbase.h
 *
 *  Created on: 2011-2-28
 *      Author: hehui
 */

#ifndef INTERESTGROUPBASE_H_
#define INTERESTGROUPBASE_H_


#include <QObject>
#include <QDateTime>
#include <QStringList>
#include <QHash>
#include <QMutex>


#include "constants_global_shared.h"
#include "sharedimlib.h"
#include "imgroupbase.h"



namespace HEHUI {

class SHAREDIMLIB_API InterestGroupBase : public IMGroupBase {
    Q_OBJECT
public:
    InterestGroupBase(quint32 groupID = 0, const QString &groupName = "", QObject *parent = 0);
    virtual ~InterestGroupBase();
    
    void setGroupInfoString(const QString &infoString);
    QString getGroupInfoString();
    
    void addMember(const QString &memberuserID, quint32 memberRole);   
    void deleteMember(const QString &memberuserID);
    bool hasMember(const QString &memberUserID);
    QStringList members() const;
    quint32 memberRole(const QString &memberUserID);

    
    
    
    

    QString getAnnouncement() const
    {
        return announcement;
    }

    QDateTime getCreationTime() const
    {
        return creationTime;
    }

    QString getDescription() const
    {
        return description;
    }

    quint32 getGroupTypeID() const
    {
        return groupTypeID;
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

    void setDescription(QString description)
    {
        this->description = description;
        addUpdatedProperty(IM::PIG_Description, "'"+description+"'");
        
    }


    void setGroupTypeID(quint32 groupTypeID)
    {
        this->groupTypeID = groupTypeID;
        addUpdatedProperty(IM::PIG_GroupTypeID, QString::number(groupTypeID));
        
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
    
    quint32 groupTypeID;
    quint32 parentGroupID;
    QDateTime creationTime;
    
    QString description;
    QString announcement;
    QString remark;

//    QDateTime lastUpdateTime;


    QHash <QString/*Member's ID*/, quint32/*Member's Role*/> membersHash;


};

}

#endif /* INTERESTGROUPBASE_H_ */
