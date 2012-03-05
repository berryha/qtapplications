/*
 * contactgroup.h
 *
 *  Created on: 2009-11-2
 *      Author: 贺辉
 */

#ifndef CONTACTGROUP_H_
#define CONTACTGROUP_H_

#include <QObject>
#include "contact.h"

#include "../../sharedim/groupbase.h"

namespace HEHUI {

class ContactGroup :public GroupBase{
	Q_OBJECT
public:
        ContactGroup(int id, const QString &name, QList<Contact*> contactList, QObject *parent = 0);
	virtual ~ContactGroup();


    QList<Contact*> getContactList() const
    {
        return contactList;
    }

    void setContactList(QList<Contact*> contactList)
    {
        this->contactList = contactList;
    }

    int getGroupID() const
    {
        return groupID;
    }

    QString getGroupName() const
    {
        return groupName;
    }

    int getParentGroupID() const
    {
        return parentGroupID;
    }

    void setGroupID(int groupID)
    {
        this->groupID = groupID;
    }

    void setGroupName(QString groupName)
    {
        this->groupName = groupName;
    }

    void setParentGroupID(int parentGroupID)
    {
        this->parentGroupID = parentGroupID;
    }


    void addContact(Contact *contact){
        contactList.append(contact);
    }
    void deleteContact(Contact *contact){
        contactList.removeAll(contact);
    }


private:
	int groupID;
	QString groupName;
	int parentGroupID;

        QList<Contact*> contactList;

};

} //namespace HEHUI

#endif /* CONTACTGROUP_H_ */
