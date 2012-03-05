#ifndef GROUPCHATWINDOW_H
#define GROUPCHATWINDOW_H

#include <QtGui/QWidget>
#include "ui_groupchatwindow.h"

namespace HEHUI {

class GroupChatWindow : public QWidget
{
    Q_OBJECT

public:
    GroupChatWindow(InterestGroup *interestGroup, QWidget *parent = 0);
    ~GroupChatWindow();

    InterestGroup * interestGroup();

signals:
    void sendMsgButtonClicked(InterestGroup *interestGroup, const QString &message, const QStringList &imageList);

public slots:
    void appendMessageReceivedFromContact(const QString &message, Contact *contact = 0, const QString &datetime = QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss.zzz"));

    void updateImage(const QString &imageName);

private slots:
    void memberItemActivated(QListWidgetItem *memberItem);

private:
    Ui::GroupChatWindowClass ui;

    InterestGroup *m_interestGroup;


};

} //namespace HEHUI

#endif // GROUPCHATWINDOW_H
