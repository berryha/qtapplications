#ifndef CONTACTGROUPBASE_H
#define CONTACTGROUPBASE_H


#include <QString>

#include "sharedimlib.h"


namespace HEHUI {



class SHAREDIMLIB_API ContactGroupBase
{
public:
    enum DefaultGroupID{Group_Blacklist_ID = 0, Group_Friends_ID = 1, Group_Strangers_ID = 2};
    static QString Group_Blacklist_Name, Group_Friends_Name, Group_Strangers_Name;

    ContactGroupBase(quint32 groupID, const QString &name = "");


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







    void setGroupID(quint32 groupID){this->m_groupID = groupID;}
    quint32 getGroupID() const {return m_groupID;}

    QString getGroupName() const {return m_groupName;}
    void setGroupName(const QString &groupName){this->m_groupName = groupName;}

    void setCreatorName(const Qstring &creator){this->m_creatorName = creator;}
    QString getCreatorName() const {return m_creatorName;}

    quint32 getGroupInfoVersion() const
    {
        return groupInfoVersion;
    }
    void setGroupInfoVersion(quint32 groupInfoVersion)
    {
        this->groupInfoVersion = groupInfoVersion;
        addUpdatedProperty(IM::PIG_GroupInfoVersion, QString::number(groupInfoVersion));

    }

    quint32 getMemberListInfoVersion() const{return memberListInfoVersion;}
    void setMemberListInfoVersion(quint32 memberListInfoVersion){this->memberListInfoVersion = memberListInfoVersion;}


private:
    quint32 m_groupID;
    QString m_groupName;

    QString m_creatorName;
    quint32 groupInfoVersion;
    quint32 memberListInfoVersion;

    QList <QString/*Member's ID*/> membersHash;

    QHash<IM::PropertyIDOfGroup/*Property ID*/, QString/*SQL Update Statement*/> updatedProperties;
    QMutex *updatedPropertiesMutex;

};

} //namespace HEHUI

#endif // CONTACTGROUPBASE_H
