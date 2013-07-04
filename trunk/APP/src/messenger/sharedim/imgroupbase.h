/*
 * groupbase.h
 *
 *  Created on: 2011-2-28
 *      Author: hehui
 */

#ifndef IIMGROUPBASE_H_
#define IIMGROUPBASE_H_


#include <QObject>
#include <QDateTime>
#include <QStringList>
#include <QHash>
#include <QMutex>


#include "constants_global_shared.h"

#include "sharedimlib.h"



namespace HEHUI {

class SHAREDIMLIB_API IMGroupBase : public QObject {
    Q_OBJECT
public:
    IMGroupBase(quint32 groupID = 0, const QString &groupName = "", QObject *parent = 0);
    virtual ~IMGroupBase();
    
    bool isNull();

    QString getUpdateSQLStatement() const;
    void addUpdatedProperty(IM::PropertyIDOfGroup propertyID, const QString &value);
    void clearUpdatedProperties();
    
    virtual QString databaseColumnName(IM::PropertyIDOfGroup propertyID) const;
    



    


    QString getCreatorID() const
    {
        return creatorID;
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

    quint32 getGroupMemberListInfoVersion() const
    {
        return memberListInfoVersion;
    }


    void setCreatorID(const QString &creatorID)
    {
        this->creatorID = creatorID;
        addUpdatedProperty(IM::PIG_CreatorID, "'"+creatorID+"'");
        
    }


    void setGroupID(quint32 groupID)
    {
        this->groupID = groupID;
        addUpdatedProperty(IM::PIG_GroupID, QString::number(groupID));
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

    void setGroupMemberListInfoVersion(quint32 memberListInfoVersion)
    {
        this->memberListInfoVersion = memberListInfoVersion;
        addUpdatedProperty(IM::PIG_MemberListInfoVersion, QString::number(memberListInfoVersion));
    }




protected:

private:
    
    quint32 groupID;
    QString creatorID;
    QString groupName;
    quint32 groupInfoVersion;
    quint32 memberListInfoVersion;

    QHash<IM::PropertyIDOfGroup/*Property ID*/, QString/*SQL Update Statement*/> updatedProperties;
    QMutex *updatedPropertiesMutex;


};

}

#endif /* IIMGROUPBASE_H_ */
