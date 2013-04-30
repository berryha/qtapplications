/*
 * contactsmanager.h
 *
 *  Created on: 2009-9-12
 *      Author: 贺辉
 */

#ifndef CONTACTSMANAGER_H_
#define CONTACTSMANAGER_H_

#include <QObject>
#include <QtSql>
#include <QHash>

#include "contact.h"
#include "contactgroup.h"
#include "imuser.h"

#include "interestgroup.h"

//#include "../../../shared/core/global_core.h"
//#include "../../../shared/core/singleton.h"
//#include "../../shared/gui/widgetbase/expandlistview/expandlistview.h"
#include "HHSharedCore/hglobal_core.h"
#include "HHSharedCore/hsingleton.h"
#include "HHSharedGUI/ItemBox"


namespace HEHUI {

class ContactsManager : public QObject, public Singleton<ContactsManager>{
    Q_OBJECT
    friend class Singleton<ContactsManager>;

public:
    typedef ItemBoxWidget::Item ObjectItem;
    typedef ItemBoxWidget:: ItemList ObjectItemList;
    typedef ItemBoxWidget::Category Category;
    typedef ItemBoxWidget::CategoryList CategoryList;

    ContactsManager(QObject *parent = 0);
    virtual ~ContactsManager();



    QStringList contactGroups() const;
    QString groupNameThatContactBelongsTo(const QString &contactID) const;


    QString getContactGroupsInfoString() const;
    ContactGroup * getContactGroup(int personalContactGroupID);

    bool hasFriendContact(const QString &contactID);
    bool hasUserInfo(const QString &userID);
    Contact * getUser(const QString &contactID);

    QList<Contact*> getContactGroupMembers(const QString &contactGroupName);
    QStringList getContacts() const;
    QStringList getStrangers() const;
    QStringList getUsers() const;




    bool loadInterestGroups();
    InterestGroup * getInterestGroup(quint32 groupID);
    bool addNewInterestGroupToDatabase(InterestGroup *interestGroup);
    bool leaveInterestGroup(quint32 groupID);
    bool saveInterestGroupInfoToDatabase(InterestGroup *interestGroup);
    QList<InterestGroup *> getInterestGroupsList();
    bool saveInterestGroupMemberToDatabase(quint32 groupID, const QString &userID, quint32 memberRole);

    bool saveInterestGroupMembersToDatabase(InterestGroup *interestGroup);

    bool saveContactChatMessageToDatabase(const QString &senderID, const QString &receiverID, const QString &message, const QString &time = "");
    bool saveInterestGroupChatMessageToDatabase(const QString &senderID, quint32 interestGroupID, const QString &message, const QString &time = "");



    Contact * createNewContact(const QString &contactID, const QString &nickname = "", const QString &face = "");

public slots:
    //void slotUpdateContactsInfo2();
    void slotFetchContactsInfo(ItemBoxWidget *expandListView);
    void slotFetchStrangersInfo();


    void slotAddNewContactGroupToUI(ItemBoxWidget *expandListView, int personalContactGroupID, const QString &groupName);
    void slotDeleteContactGroupFromUI(ItemBoxWidget *expandListView, const QString &groupName);

    void addContactToUI(ItemBoxWidget *expandListView, const QString &groupName, const QString &contactID);
    void deleteContactFromUI(ItemBoxWidget *expandListView, const QString &groupName, const QString &contactID);
    void moveContactToUI(ItemBoxWidget *expandListView, const QString &old_groupName, const QString &new_groupName, const QString &contactID);
    void updateContactToUI(ItemBoxWidget *expandListView, const QString &groupName, const QString &contactID);
    void updateContactToUI(ItemBoxWidget *expandListView, int personalContactGroupID, const QString &contactID);


    void renameGroupToUI(ItemBoxWidget *expandListView, const QString &old_groupName, const QString &new_groupName);


    bool addOrDeleteContact(const QString &contactID, int groupID, bool add = true);
    bool moveContact(const QString &contactID, int oldGroupID, int newGroupID);


    //联系人在线状态改变
    void slotChangeContactOnlineState(const QString &contactID, quint8 onlineStateCode, const QString &peerAddress, quint16 peerPort, const QString &greetingInfo);




    int getPersonalContactGroupID(const QString &groupName);
    QString getPersonalContactGroupName(int personalContactGroupID) ;
    int slotAddNewContactGroupToDatabase(const QString &groupName);
    bool renameGroupToDatabase(const QString &old_groupName, const QString &new_groupName);
    bool deleteGroupFromDatabase(const QString &groupName);


    bool getMyInfoFormLocalDatabase();
    bool saveMyInfoToDatabase();

    bool setContactSummaryInfo(const QString &contactID, const QString &contactSummaryInfo);
    bool getContactInfoFormLocalDatabase(const QString &contactID);
    bool saveContactInfoToDatabase(const QString &contactID);

//    bool saveContactGroupsInfoToDatabase();


    bool openDatabase(bool reopen = false);
    bool initLocalDatabase(QString *errorMessage = 0);



private slots:
    //void slotSetupContacts(bool loadFromFile, const QString &string);
    //void slotLoadContacts(ItemBoxWidget *widgetBox, bool loadFromFile = true, const QString &string = "");
    void slotLoadContacts(ItemBoxWidget *expandListView, int groupID, const QString groupName, QList<Contact*> contactList);
    //void slotLoadContacts(ItemBoxWidget *expandListView,ContactGroup *contactGroup);

    bool slotAddNewContactToDatabase(Contact *contact);
    bool slotdeleteContactFromDatabase(Contact *contact);


private:
    QSqlQuery queryDatabase(const QString & queryString, bool localConfigDatabase) ;

    QSqlQuery queryDatabase(const QString & queryString, const QString &connectionName, const QString &driver,
                            const QString &host, int port, const QString &user, const QString &passwd,
                            const QString &databaseName, HEHUI::DatabaseType databaseType) ;

private:


    //QSqlQuery *query;
    QString userPrivateDataFilePath;
    QSqlDatabase localUserDataDB;

    QHash<int/*Grup ID*/, ContactGroup*> contactGroupHash;
    QHash<QString/*Contact ID*/, Contact*> contactHash;

    QHash<int/*Grup ID*/, InterestGroup*> interestGroupsHash;

    IMUser *m_imUser;



};

} //namespace HEHUI

#endif /* CONTACTSMANAGER_H_ */
