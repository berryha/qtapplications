#ifndef CONTACTCHATWINDOW_H
#define CONTACTCHATWINDOW_H

#include <QWidget>
#include "ui_contactchatwidget.h"


class ContactChatWidget : public QWidget
{
    Q_OBJECT

public:
    ContactChatWidget(Contact *contact = 0, QWidget *parent = 0);
    ~ContactChatWidget();

    void setContact(Contact *contact);
    Contact * contact();
    QString displayName() const;

protected:
    void closeEvent(QCloseEvent * event);

signals:
    void sendMsgButtonClicked(Contact *contact, const QString &message, const QStringList &imageList);
    void toBeDstroyed();

public slots:
    void appendMessageReceivedFromContact(const QString &message, Contact *contact = 0, const QString &datetime = QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss"));

    void updateImage(const QString &imageName);


private:
    Ui::ContactChatWidgetClass ui;

    Contact *m_contact;

    QString m_displayName;



};

#endif // CONTACTCHATWINDOW_H
