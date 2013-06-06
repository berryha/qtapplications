/*
 * contactgroup.h
 *
 *  Created on: 2009-11-2
 *      Author: 贺辉
 */

#ifndef CONTACTGROUP_SERVER_H_
#define CONTACTGROUP_SERVER_H_

#include <QObject>
#include "userinfo.h"

#include "../../sharedim/contactgroupbase.h"

namespace HEHUI {

class ContactGroup :public ContactGroupBase{
    Q_OBJECT
public:
    ContactGroup(quint32 groupID, const QString &groupName, const QString &creatorName, QList<UserInfo*> contactList, QObject *parent = 0);
    virtual ~ContactGroup();


    QList<UserInfo*> getContactList() const
    {
        return contactList;
    }

    void setContactList(QList<UserInfo*> contactList)
    {
        this->contactList = contactList;
    }


//    int getParentGroupID() const
//    {
//        return parentGroupID;
//    }

//    void setParentGroupID(int parentGroupID)
//    {
//        this->parentGroupID = parentGroupID;
//    }


    void addContact(UserInfo *contact){
        contactList.append(contact);
    }
    void deleteContact(UserInfo *contact){
        contactList.removeAll(contact);
    }

    void setCreatorInfo(UserInfo *creator){this->m_creator = creator;}
    UserInfo * getCreatorInfo(){return m_creator;}


private:
//    int parentGroupID;

    QList<UserInfo*> contactList;

    UserInfo *m_creator;

};

} //namespace HEHUI

#endif /* CONTACTGROUP_SERVER_H_ */
