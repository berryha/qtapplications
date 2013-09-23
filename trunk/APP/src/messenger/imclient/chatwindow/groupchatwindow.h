#ifndef GROUPCHATWINDOW_H
#define GROUPCHATWINDOW_H

#include <QWidget>
#include "ui_groupchatwindow.h"

namespace HEHUI {

class GroupChatWindow : public QWidget
{
    Q_OBJECT

public:
    GroupChatWindow(InterestGroup *interestGroup, QWidget *parent = 0);
    ~GroupChatWindow();

    InterestGroup * interestGroup();

    bool isDownloadingImage(const QString &imageName);
    void processImageDownloadResult(const QString &imageName, bool downloaded);

    void memberJoinedOrQuitted(const QString &memberID, bool join);

protected:
    void closeEvent(QCloseEvent * event);

signals:
    void sendMsgButtonClicked(InterestGroup *interestGroup, const QString &message, const QStringList &imageList);
    void toBeDstroyed();
    void signalRequestDownloadImage(const QString &contactID, const QString &imageName);


public slots:
    void appendMessageReceivedFromContact(const QString &message, Contact *contact = 0, const QString &datetime = QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss.zzz"));


private slots:
    void memberItemActivated(QListWidgetItem *memberItem);

private:
    QListWidgetItem * memberItem(const QString &memberID);


private:
    Ui::GroupChatWindowClass ui;

    InterestGroup *m_interestGroup;


};

} //namespace HEHUI

#endif // GROUPCHATWINDOW_H
