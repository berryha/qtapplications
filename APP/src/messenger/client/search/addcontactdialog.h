#ifndef ADDCONTACTDIALOG_H
#define ADDCONTACTDIALOG_H

#include "ui_addcontactdialog.h"

#include  "../contactsmanager/contact.h"



namespace HEHUI{

class AddContactDialog : public QDialog
{
    Q_OBJECT

public:
    
    //enum WindowType{WT_Request_To_Add_Contact, WT_Request_From_User, WT_Response_From_User};
    
    explicit AddContactDialog(Contact *user, bool requestToAddNewContact = true, QWidget *parent = 0);
    explicit AddContactDialog(Contact *user, const QString &requestMessage, QWidget *parent = 0);
    
    QString getMessage() const;
    QString getNewName() const;
    QString getGroupname();
    
    bool requestRejected();

protected:
    void changeEvent(QEvent *e);
    
private slots:
    void on_pushButtonAddAsAContact_clicked();
    void on_pushButtonReject_clicked();
    

private:
    Ui::AddContactDialog ui;
    
    Contact *m_user;
    bool m_requestToAddContact;
    
    bool m_rejectRequest;
    
    QRegExpValidator *validator;
    
    
};

} //namespace HEHUI

#endif // ADDCONTACTDIALOG_H
