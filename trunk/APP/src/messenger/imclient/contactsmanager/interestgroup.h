/*
 * groupfromserver.h
 *
 *  Created on: 2011-2-28
 *      Author: hehui
 */

#ifndef INTERESTGROUP_H_
#define INTERESTGROUP_H_


#include <QObject>
#include <QDateTime>
#include <QStringList>
#include <QMap>

#include "../../sharedim/interestgroupbase.h"


namespace HEHUI {

class InterestGroup : public InterestGroupBase {
    Q_OBJECT
public:
    InterestGroup(quint32 groupID = 0, const QString &groupName = "", QObject *parent = 0);
    virtual ~InterestGroup();


    QString databaseColumnName(IM::PropertyIDOfGroup propertyID) const;

    void appandUnreadMessage(const QString &time, const QString &message);
    QMap<QString/*Time String*/, QString/*Message*/> takeUnreadMessages();


protected:

private:
    

    QMap<QString/*Time String*/, QString/*Message*/> unreadMessages;









};

}

#endif /* INTERESTGROUP_H_ */
