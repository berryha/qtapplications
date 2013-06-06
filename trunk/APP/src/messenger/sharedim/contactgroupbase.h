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

    void setGroupID(quint32 groupID){this->m_groupID = groupID;}
    quint32 getGroupID() const {return m_groupID;}

    QString getGroupName() const {return m_groupName;}
    void setGroupName(const QString &groupName){this->m_groupName = groupName;}

    void setCreatorName(const Qstring &creator){this->m_creatorName = creator;}
    QString getCreatorName() const {return m_creatorName;}



private:
    quint32 m_groupID;
    QString m_groupName;

    QString m_creatorName;


};

} //namespace HEHUI

#endif // CONTACTGROUPBASE_H
