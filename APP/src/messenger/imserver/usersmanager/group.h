/*
 * Group.h
 *
 *  Created on: 2011-2-28
 *      Author: hehui
 */

#ifndef GROUP_H_
#define GROUP_H_


#include <QObject>
#include <QDateTime>
#include <QStringList>
#include <QHash>

#include "../../sharedim/groupbase.h"


namespace HEHUI {

class Group : public GroupBase {
    Q_OBJECT
public:
        Group(quint32 groupID = 0, QObject *parent = 0);
	virtual ~Group();


        QString databaseColumnName(IM::PropertyIDOfGroup propertyID) const;


protected:

private:
    










};

}

#endif /* GROUP_H_ */
