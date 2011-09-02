#ifndef SEARCH_H
#define SEARCH_H

#include <QtGui/QDialog>
#include "ui_search.h"

#include "userinfomodel.h"
#include "../contactsmanager/contact.h"



namespace HEHUI{

class Search : public QDialog
{
    Q_OBJECT

public:
//    static Search * instance();
    ~Search();
    
//private:
    Search(QWidget *parent = 0);
    
signals:
    void signalSearchContact(const QString &propertiesString, bool matchExactly, bool searchOnlineUsersOnly);
    void signalAddContact(const QString &userID, const QString &verificationMessage);
    
    
public slots:
    void slotSearchContactsResultPacketReceived(const QStringList &users);
    
private slots:
    
    void reset();
    void resetUsersInfo();
    void slotUserSelected(const QModelIndex &index);
    void slotAddUserAsAContact(const QModelIndex &index);
    
    void on_tabWidget_currentChanged( int index );
    void on_pushButtonCondition_clicked();
    void on_pushButtonSearch_clicked();
    
    void on_radioButtonUsersMatchWildcard_clicked();
    void on_radioButtonUsersMatchExactly_clicked();
    
    void on_radioButtonGroupsMatchWildcard_clicked();
    void on_radioButtonGroupsMatchExactly_clicked();
    
    void on_pushButtonUserDetails_clicked();
    void on_pushButtonAddAsContact_clicked();
    
    

private:
    Ui::SearchClass ui;
    
//    static Search * m_searchInstance;
    
    QHash<QString/*User's ID*/, Contact *> usersHash;
    UserInfoModel *userInfoModel;
    
    
    
    
    
    
    
    
    
    
    
    
    
};

} //namespace HEHUI

#endif // SEARCH_H
