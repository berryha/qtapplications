/*
 ****************************************************************************
 * imuser.h
 *
 * Created On: 2010-7-3
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
 * Last Modified On: 2010-7-3
 * Last Modified By: 贺辉
 ***************************************************************************
 */

#ifndef IMUSER_H_
#define IMUSER_H_

#include "../sharedim/imuserbase.h"


namespace HEHUI {

class  IMUser: public IMUserBase {
public:
    static IMUser * instance();
    virtual ~IMUser();

    bool loadMyInfoFromLocalDatabase();
    bool saveMyInfoToLocalDatabase();

    bool setContactInfoString(const QString &contactID, const QString &contactInfoString, bool summaryInfo);
    bool saveContactInfoToLocalDatabase(const QString &contactID);

    IM::OnlineState getStateAfterLoggedin() const;
    void setStateAfterLoggedin(IM::OnlineState state);


    void setLoginServerAddress(const QString &serverAddress);
    QString getLoginServerAddress() const;
    void setLoginServerPort(quint16 serverPort);
    quint16 getLoginServerPort() const;
    void setRegistrationServerAddressInfo(const QString &regServerAddress);
    QString getRegistrationServerAddressInfo() const;


private:
    IMUser(const QString & userID = "", QObject *parent = 0);


public slots:
    //        bool addGroup(const QString &groupName, const QStringList &members = QStringList());
    //        bool removeGroup(const QString &groupName);
    //        bool updateGroupName(const QString &oldGroupName, const QString &newGroupName);
    //        bool addContactToGroup(const QString &contactID, const QString &groupName);


    bool isFriendContact(const QString &contactID);


private slots:

    QString databaseColumnName(IM::PropertyIDOfUser propertyID) const;

private:
    static IMUser *imUserInstance;

    IM::OnlineState stateAfterLoggedin;

    QString m_loginServerAddress;
    quint16 m_loginServerPort;
    QString m_regServerAddress;


};

}

#endif /* IMUSER_H_ */
