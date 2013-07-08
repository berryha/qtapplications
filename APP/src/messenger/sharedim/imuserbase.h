/*
 ****************************************************************************
 * imuserbase.h
 *
 * Created on: 2008-10-14
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
 * Last Modified on: 2010-07-03
 * Last Modified by: 贺辉
 ***************************************************************************
 */




#ifndef IMUSERBASE_H_
#define IMUSERBASE_H_

#include <QtCore>
#include <QObject>
#include <QStringList>


#include "constants_global_shared.h"
#include "sharedimlib.h"



#include "HHSharedCore/huser.h"
#include "HHSharedCore/hglobal_core.h"



namespace HEHUI {

class SHAREDIMLIB_API IMUserBase : public User//, public Singleton<IMUserBase>
{
    Q_OBJECT
    //friend class Singleton<IMUserBase>;

public:
    enum FriendshipApply{FA_AUTO_ACCEPT = 0, FA_REQUIRE_AUTHENTICATION};
    enum FriendshipApplyResult{FAR_UNKNOWN = 0, FAR_ACCEPTED = 1, FAR_DENIED};

    enum ShortTalk{ST_AUTO_ACCEPT = 0, ST_PROMPT, ST_DENY};
    enum AgeSection{AS_16_22 = 0, AS_23_30, AS_31_40, AS_40_};

    enum AccountState{AS_Invalid = 0, AS_Normal, AS_Banned, AS_Limitted};

    IMUserBase(QObject *parent = 0);
    IMUserBase(const QString & userID, QObject *parent = 0);
    virtual ~IMUserBase();

    QByteArray encryptedPassword() const;
    
    void addUpdatedPersonalInfoProperty(IM::PropertyIDOfUser propertyID, const QString &value, bool summaryInfo);
    QString getUpdateSQLStatement(bool summaryInfo) const;
    void clearUpdatedProperties();
    
    virtual QString databaseColumnName(IM::PropertyIDOfUser propertyID) const = 0;

    quint32 updatePersonalContactGroupsInfoVersion();
    
    QString getDefaultGroupName() const;





    FriendshipApply getFriendshipApply() const
    {
        return friendshipApply;
    }

    IM::OnlineState getOnlineState() const
    {
        return onlineState;
    }

    QHash<QString, QStringList> getPersonalContactGroupsHash() const
    {
        return personalContactGroupsHash;
    }

    quint32 getBlacklistInfoVersion() const{
        return blacklistInfoVersion;
    }

    QStringList getBlacklist() const{
        return blacklist;
    }


    quint32 getPersonalContactGroupsVersion() const
    {
        return personalContactGroupsInfoVersion;
    }
    

    quint32 getInterestGroupInfoVersion() const
    {
        return interestGroupInfoVersion;
    }
    
    QStringList getInterestGroups() const
    {
        return interestGroups;
    }
    void setInterestGroups(const QStringList &groups){
        this->interestGroups = groups;
    }

    quint32 getPersonalSummaryInfoVersion() const
    {
        return personalSummaryInfoVersion;
    }
    quint32 getPersonalDetailInfoVersion() const
    {
        return personalDetailInfoVersion;
    }

    //    QHash<quint32,QString> getPersonalInterestGroupsHash() const
    //    {
    //        return personalInterestGroupsHash;
    //    }

    QByteArray getSessionEncryptionKey() const
    {
        return sessionEncryptionKey;
    }

    ShortTalk getShortTalk() const
    {
        return shortTalk;
    }

    //    QHash<quint32,QString> getSystemGroups() const
    //    {
    //        return systemGroupsHash;
    //    }

    int getUserRole() const
    {
        return userRole;
    }

    void setFriendshipApply(FriendshipApply friendshipApply)
    {
        this->friendshipApply = friendshipApply;
    }

    void setOnlineState(IM::OnlineState onlineState)
    {
        this->onlineState = onlineState;
    }

    void setPersonalContactGroupsHash(QHash<QString, QStringList> personalContactGroups)
    {
        this->personalContactGroupsHash = personalContactGroups;
    }

    void setBlacklistInfoVersion(quint32 blacklistInfoVersion){
        this->blacklistInfoVersion = blacklistInfoVersion;
        addUpdatedPersonalInfoProperty(IM::PI_BlacklistInfoVersion, QString::number(blacklistInfoVersion), true);
    }

    void setBlacklist( const QStringList &blacklist){
        this->blacklist = blacklist;
    }

    void setPersonalContactGroupsVersion(quint32 personalContactGroupsVersion)
    {
        this->personalContactGroupsInfoVersion = personalContactGroupsVersion;
        addUpdatedPersonalInfoProperty(IM::PI_PersonalContactGroupsInfoVersion, QString::number(personalContactGroupsInfoVersion), true);

    }

    void setInterestGroupInfoVersion(quint32 interestGroupInfoVersion)
    {
        this->interestGroupInfoVersion = interestGroupInfoVersion;
        addUpdatedPersonalInfoProperty(IM::PI_InterestGroupsInfoVersion, QString::number(interestGroupInfoVersion), true);

    }

    
//    void setInterestGroupsStringFromDatabase(const QString &interestGroupsStringFromDatabase)
//    {
//        this->interestGroups = interestGroupsStringFromDatabase.split(",");
//    }

    void setPersonalSummaryInfoVersion(quint32 personalSummaryInfoVersion)
    {
        this->personalSummaryInfoVersion = personalSummaryInfoVersion;
    }
    void setPersonalDetailInfoVersion(quint32 personalDetailInfoVersion)
    {
        this->personalDetailInfoVersion = personalDetailInfoVersion;
    }

    //    void setPersonalInterestGroupsHash(QHash<quint32,QString> personalInterestGroups)
    //    {
    //        this->personalInterestGroupsHash = personalInterestGroups;
    //    }

    void setSessionEncryptionKey(const QByteArray &sessionEncryptionKey)
    {
        this->sessionEncryptionKey = sessionEncryptionKey;
    }

    void setShortTalk(ShortTalk shortTalk)
    {
        this->shortTalk = shortTalk;
    }

    void setUserRole(int userRole)
    {
        this->userRole = userRole;
    }

    void setAccountState(AccountState state){
        this->accountState = state;
    }
    AccountState getAccountState() const{
        return accountState;
    }







private:
    void init();
    


public slots:
    void setPersonalInfoString(const QString &personalInfoString, bool summaryInfo);
    QString getPersonalInfoString(bool requestSummaryInfo) const;

    void setContactGroupsInfoString(const QString &contactGroupsInfo);
    virtual QString getContactGroupsInfoString() const;
    virtual QStringList getContacts() const;
    bool addOrDeleteContact(const QString &contactID, const QString &groupName, bool add = true);
    bool moveContact(const QString &contactID, const QString &oldGroupName, const QString &newGroupName);
    

    bool joinOrLeaveInterestGroup(const QString &interestGroupID, bool join = true);
    quint32 updateInterestGroupInfoVersion();

    
    bool addOrDeleteBlacklistedContact(const QString &contactID,  bool addToBlacklist = true);
    quint32 updateBlacklistInfoVersion();
    QString getBlacklistInfoString();
    void setBlacklistInfoString(const QString &blacklistInfoString);

    //    bool hasContact(const QString &contactID);
    //    QString groupNameThatContactBelongsTo(const QString &contactID) const;
    


private:


    //    QHash<quint32/*Group ID*/, QString/*Group Name*/> systemGroupsHash;
    //    QHash<quint32/*Group ID*/, QString/*Group Name*/> personalInterestGroupsHash;
    QHash<QString/*Group Name*/, QStringList/*Group Members' ID*/> personalContactGroupsHash;

    //    QHash<QString/*Contact's ID*/, QString/*Contact's Nick Name*/> personalContacts;


    quint32 blacklistInfoVersion;
    QStringList blacklist;

    QStringList interestGroups;
    quint32 interestGroupInfoVersion;
    
    quint32 personalSummaryInfoVersion;
    quint32 personalDetailInfoVersion;
    quint32 personalContactGroupsInfoVersion;


    IM::OnlineState onlineState;
    FriendshipApply friendshipApply;
    ShortTalk shortTalk;
    int userRole;
    AccountState accountState;

    QByteArray sessionEncryptionKey;
    
    QHash<IM::PropertyIDOfUser/*Property ID*/, QString/*SQL Update Statement*/> updatedSummaryInfoProperties;
    QHash<IM::PropertyIDOfUser/*Property ID*/, QString/*SQL Update Statement*/> updatedDetailInfoProperties;
    QMutex *updatedPropertiesMutex;

    
    QString defaultGroupName;


};

} //namespace HEHUI

#endif /* IMUSERBASE_H_ */
