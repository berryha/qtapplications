/*
 * contactgroup.cpp
 *
 *  Created on: 2009-11-2
 *      Author: 贺辉
 */

#include "contactgroup.h"

namespace HEHUI {

ContactGroup::ContactGroup(int groupID, const QString &groupName, const QString &creatorName, QList<UserInfo*> contactList, QObject *parent)
    :ContactGroupBase(groupID, groupName), contactList(contactList)
{
    setCreator(creatorName);

}

ContactGroup::~ContactGroup() {
    // TODO Auto-generated destructor stub
}



}
