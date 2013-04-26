#ifndef CONTACTCHATWINDOW_H
#define CONTACTCHATWINDOW_H

#include <QWidget>
#include "ui_contactchatwidget.h"


class ContactChatWidget : public QWidget
{
    Q_OBJECT

public:
    ContactChatWidget(Contact *contact, QWidget *parent = 0);
    ~ContactChatWidget();

    Contact * contact(){return m_contact;}

signals:
    void sendMsgButtonClicked(Contact *contact, const QString &message, const QStringList &imageList);

public slots:
    void appendMessageReceivedFromContact(const QString &message, Contact *contact = 0, const QString &datetime = QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss"));

    void updateImage(const QString &imageName);


private:
    Ui::ContactChatWidgetClass ui;

    Contact *m_contact;



};

#endif // CONTACTCHATWINDOW_H
