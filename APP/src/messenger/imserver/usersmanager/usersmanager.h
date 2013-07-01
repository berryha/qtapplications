/*
 ****************************************************************************
 * usersmanager.h
 *
 * Created on: 2009-9-12
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
 * Last Modified on: 2010-05-31
 * Last Modified by: 贺辉
 ***************************************************************************
 */

#ifndef USERSMANAGER_H_
#define USERSMANAGER_H_

#include <QObject>
#include <QtSql>
#include <QHash>
#include <QHostAddress>


#include "userinfo.h"
#include "interestgroup.h"


#include "HHSharedCore/hsingleton.h"
#include "HHSharedCore/hdatabaseutility.h"

namespace HEHUI {

class UsersManager//: public QObject//, public Singleton<UsersManager>
{
//    Q_OBJECT
    //friend class Singleton<UsersManager>;

public:

    UsersManager(QObject *parent = 0);
    virtual ~UsersManager();


    UserInfo* getUserInfo(const QString &imUserID);
    UserInfo* getOnlineUserInfo(const QString &imUserID);
    UserInfo* getOfflineUserInfo(const QString &imUserID);
    QList<UserInfo*> onlineUsers();
    void userOnline(UserInfo* userInfo);
    void userOffline(UserInfo* userInfo);


    bool canUserLogin(const QString &userID, const QString &clientVersion, IM::ErrorType *errorType);
    void getUserLoginServer(const QString &userID, QHostAddress *serverAddress, quint16 *serverPort);
    UserInfo* logUserIn(const QString &userID, const QByteArray &encryptedPassword, IM::OnlineState loginState, IM::ErrorType *errorType);
    
    bool registerNewUser(const QString &userID, const QString &password, IM::ErrorType *errorType, quint32 *sysID, QString *message = 0);
    void updateUserPassword(const QString &userID, const QString &newPassword, IM::ErrorType *errorType, QString *message = 0);

    QStringList searchContact(const QString &propertiesString, bool matchExactly, bool searchOnlineUsersOnly);


    bool saveCachedChatMessageFromIMUser(const QString &senderID, const QString &receiverID, const QString &message);
    QStringList cachedChatMessagesForIMUser(UserInfo* userInfo);

    bool saveUserLastLoginInfo(UserInfo* userInfo, const QString &userHostAddress, quint16 userHostPort);
    bool saveUserLastLogoutInfo(UserInfo* userInfo);
    bool getUserLastLoginInfo(UserInfo *userInfo);
    

    bool saveUserInfoToDatabase(UserInfo *info);

    bool saveFriendshipApplyRequest(const QString &applicantID, const QString &contactID, const QString &message, quint8 resultCode = quint8(UserInfo::FAR_UNKNOWN),  bool senderRead = false, bool receiverRead = false);
    bool deleteFriendshipApplyRequest(const QString &applicantID, const QString &contactID);
    bool getFriendshipApplyRequest(const QString &receiverID, QList<QStringList> *sentApplicationList, QList<QStringList> *receivedApplicationList);


    QString getInterestGroupsListForUser(UserInfo* userInfo);
    QString getInterestGroupInfoStringForUser(UserInfo* userInfo, quint32 groupID);
    QString getInterestGroupMembersInfoStringForUser(UserInfo* userInfo, InterestGroup *group);
    
    
    
    
/////////////////////////// Group Manager ////////////////////////////////////////    
    InterestGroup* getInterestGroup(quint32 groupID);
    bool createNewInterestGroup(quint32 groupTypeID, quint32 parentGroupID, quint32 creatorSystemID, const QString &groupName );
 
    QStringList searchInterestGroup(const QString &propertiesString, bool matchExactly, bool searchOnlineUsersOnly);
    bool saveInterestGroupToDatabase(InterestGroup *groupInfo);

    bool saveMembershipApplyRequest(quint32 applicantID, quint32 groupID, const QString &message = "");
    bool deleteMembershipApplyRequest(quint32 applicantID, quint32 groupID);
    bool getMembershipApplyRequest(quint32 groupID, QList<QStringList> *receivedApplicationList);

    bool saveCachedInterestGroupChatMessageFromIMUser(const QString &senderID, quint32 interestGroupID, const QString &message);
    QStringList cachedInterestGroupChatMessagesForIMUser(UserInfo* userInfo);

    
/////////////////////////// ---Group Manager--- ////////////////////////////////////////    
    

private:

    UserInfo * queryUserInfo(const QString &imUserID);
    bool queryUserInfo(UserInfo *info);

    bool getUserInterestGroupsFromDatabase(UserInfo* info);


    
    
    
/////////////////////////// Group Manager ////////////////////////////////////////    
  
    InterestGroup * queryInterestGroup(quint32 groupID);
    bool queryInterestGroup(InterestGroup *info);
/////////////////////////// ---Group Manager--- ////////////////////////////////////////    
     
     
    bool openDatabase(bool reopen = false);
    

    QSqlQuery queryDatabase(const QString & queryString, bool localConfigDatabase);

    QSqlQuery queryDatabase(const QString & queryString,
                            const QString &connectionName, const QString &driver,
                            const QString &host, int port, const QString &user,
                            const QString &passwd, const QString &databaseName,
                            HEHUI::DatabaseType databaseType);


signals:
//    void signalIMUserVerifyed(const QString &imUserID);
//    void signalIMUserForcedToInterrupt(const QString &imUserID, const QString &ip, uint port);

public slots:

//    void slotFetchIMUsersInfo();
//    void slotAddNewIMUser(UserInfo *imUser);


private slots:
    
    void slotCheckOnlineUsers();






private:


    static QMutex *onlineUserMutex;
    static QHash<QString/*User ID*/, UserInfo*> *onlineUserInfoHash;
    static QMutex *offlineUserMutex;
    static QHash<QString/*User ID*/, UserInfo*> *offlineUserInfoHash;

    //ServerNetworkManager *serverNetworkManager;
    
//    QSqlQuery *query;
    
    QSqlDatabase db;
    
    
    static QMutex *groupMutex;
    static QHash<quint32/*Group ID*/, InterestGroup*> *groupHash;


};

} //namespace HEHUI

#endif /* USERSMANAGER_H_ */
