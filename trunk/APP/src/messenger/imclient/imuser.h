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

class  IMUser: public HEHUI::IMUserBase {
public:
        static IMUser * instance();
	virtual ~IMUser();
        
        bool loadMyInfoFromLocalDatabase();
        bool saveMyInfoToLocalDatabase();
        
        bool setContactSummaryInfo(const QString &contactID, const QString &contactSummaryInfo);
        bool saveContactInfoToLocalDatabase(const QString &contactID);
        
        IM::OnlineState getStateAfterLoggedin() const;
        void setStateAfterLoggedin(IM::OnlineState state);

        QStringList contactGroups() const;
        

private:
	IMUser(const QString & userID = "", QObject *parent = 0);


public slots:
//        bool addGroup(const QString &groupName, const QStringList &members = QStringList());
//        bool removeGroup(const QString &groupName);
//        bool updateGroupName(const QString &oldGroupName, const QString &newGroupName);
//        bool addContactToGroup(const QString &contactID, const QString &groupName);

        QString getContactGroupsInfoString() const;
        QStringList getContacts() const;

        bool hasContact(const QString &contactID);
        QString groupNameThatContactBelongsTo(const QString &contactID) const;


private slots:

        QString databaseColumnName(IM::PropertyIDOfUser propertyID) const;

private:
	static IMUser *imUserInstance;
        
        IM::OnlineState stateAfterLoggedin;
        





};

}

#endif /* IMUSER_H_ */
