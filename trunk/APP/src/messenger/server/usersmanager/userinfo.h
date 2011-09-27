/*
 ****************************************************************************
 * userinfo.h
 *
 * Created on: 2010-5-27
 *     Author: 贺辉
 *    License: LGPL
 *    Comment:
 *
 *
 *    =============================  Usage  =============================
 *|
 *|
 *    ===================================================================
 *
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 ****************************************************************************
 */

/*
 ***************************************************************************
 * Last Modified on: 2010-5-31
 * Last Modified by: 贺辉
 ***************************************************************************
 */





#ifndef USERINFO_H
#define USERINFO_H

#include <QObject>
//#include <QTimer>
#include <QMutex>

#include "../../sharedim/imuserbase.h"


namespace HEHUI {


class UserInfo : public IMUserBase
{
    Q_OBJECT
public:
    UserInfo(const QString &userID, QObject *parent = 0);
    UserInfo(const QString &ipAddress, quint16 port, QObject *parent = 0);
    ~UserInfo();


    bool isOnLine();
//    void updateRemainingOnlineStatusCheckingTimes(bool heartbeatPacketReceived);

    bool isBanned();
    void setBanned(bool banned);
    void authenticationFailed();


    void setOnline();
    void setOffline();
    
    QString databaseColumnName(IM::PropertyIDOfUser propertyID) const;
    
//    QString getUpdateSQLStatement() const;
//    void clearUpdatedProperties();



public slots:
    bool addGroup(const QString &groupName, const QStringList &members = QStringList());
    bool createOrDeleteGroup(const QString &groupName, bool create);
    bool updateGroupName(const QString &oldGroupName, const QString &newGroupName);
    bool addContactToGroup(const QString &contactID, const QString &groupName);

    bool hasContact(const QString &contactID);
    QString groupNameThatContactBelongsTo(const QString &contactID) const;


private:
        void init();


private:

//    quint16 remainingOnlineStatusCheckingTimes;

    quint16 remainingAuthenticationRequestTimes;
    bool bannedFromAuthenticationRequest;
    QDateTime timeToLiftTheBan;
//    QDateTime timeLastHeartbeatPacketReceived;


    //QHash<IM::PropertyID/*Property ID*/, QString/*Property Value*/> updatedProperties;
    
//    QHash<IM::PropertyID/*Property ID*/, QString/*SQL Update Statement*/> updatedProperties;
//    QMutex *updatedPropertiesMutex;








};

} //namespace HEHUI

#endif // USERINFO_H
