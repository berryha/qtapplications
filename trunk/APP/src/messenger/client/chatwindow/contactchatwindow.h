#ifndef CONTACTCHATWINDOW_H
#define CONTACTCHATWINDOW_H

#include <QtGui/QMainWindow>
#include "ui_contactchatwindow.h"


class ContactChatWindow : public QMainWindow
{
    Q_OBJECT

public:
    ContactChatWindow(Contact *contact, QWidget *parent = 0);
    ~ContactChatWindow();

    Contact * contact(){return m_contact;}

signals:
    void sendMsgButtonClicked(Contact *contact, const QString &message, const QStringList &imageList);

public slots:
    void appendMessageReceivedFromContact(const QString &message, Contact *contact = 0, const QString &datetime = QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss"));

    void updateImage(const QString &imageName);


private:
    Ui::ContactChatWindowClass ui;

    Contact *m_contact;



};

#endif // CONTACTCHATWINDOW_H
