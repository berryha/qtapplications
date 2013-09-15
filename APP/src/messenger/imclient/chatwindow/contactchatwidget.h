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

    bool isDownloadingImage(const QString &imageName);
    void processImageDownloadResult(const QString &imageName, bool downloaded);


protected:
    void closeEvent(QCloseEvent * event);

signals:
    void sendMsgButtonClicked(Contact *contact, const QString &message, const QStringList &imageList);
    void toBeDstroyed();
    void signalRequestDownloadImage(const QString &contactID, const QString &imageName);


public slots:
    void appendMessageReceivedFromContact(const QString &message, Contact *contact = 0, const QString &datetime = QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss"));



private:
    Ui::ContactChatWidgetClass ui;

    Contact *m_contact;

    QString m_displayName;



};

#endif // CONTACTCHATWINDOW_H
