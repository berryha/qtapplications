/*
 * contactgroup.cpp
 *
 *  Created on: 2009-11-2
 *      Author: 贺辉
 */

#include "contactgroup.h"

namespace HEHUI {

ContactGroup::ContactGroup(int id, const QString &name, QList<Contact*> contactList, QObject *parent)
 :GroupBase(id, parent), groupID(id),groupName(name),contactList(contactList)
{
	// TODO Auto-generated constructor stub

}

ContactGroup::~ContactGroup() {
	// TODO Auto-generated destructor stub
}



}
