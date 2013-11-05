#ifndef CHATWINDOWMANAGER_H
#define CHATWINDOWMANAGER_H

#include <QMainWindow>
#include <QList>
#include <QPair>
#include <QCloseEvent>


#include "ui_chatwindowmanager.h"


#include "../contactsmanager/contact.h"
//#include "chatmessagewindow.h"
#include "groupchatwindow.h"
#include "contactchatwidget.h"

//#include "../../shared/core/singleton.h"
#include "HHSharedCore/hsingleton.h"



namespace HEHUI {

class ChatWindowManager : public QMainWindow, public Singleton<ChatWindowManager>
{
    Q_OBJECT
    friend class Singleton<ChatWindowManager>;

public:
    enum ChatWindowDisplayStyle{TabbedChatWindow = 0, MDIChatWindow, SeparatedChatWindow};
    ChatWindowManager(QWidget *parent = 0);
    ~ChatWindowManager();

protected:
    void closeEvent(QCloseEvent *);


public:
    void setChatWindowDisplayStyle(ChatWindowDisplayStyle style);
    ChatWindowDisplayStyle getChatWindowDisplayStyle();
    void switchChatWindowDisplayStyle(ChatWindowDisplayStyle style);

    bool closeContactChatWindow(Contact *contact);
    bool closeInterestGroupChatWindow(InterestGroup *group);

    void interestGroupMemberJoinedOrQuitted(quint32 groupID, const QString &memberID, bool join);



signals:
    void signalSendChatMessageToCantact(Contact *contact, const QString &message, const QStringList &imageList);
    void signalSendChatMessageToInterestGroup(InterestGroup *interestGroup, const QString &message, const QStringList &imageList);
    void signalRequestDownloadImage(const QString &contactID, const QString &imageName);


public slots:
    void slotNewChatWithContact(const QString &contactID);
    void slotNewMessageReceivedFromContact(const QString &contactID, const QString &message, const QString &time);

    void slotNewChatWithInterestGroup(quint32 interestGroupID);
    void slotNewMessageReceivedFromInterestGroup(quint32 interestGroupID, const QString &contactID, const QString &message, const QString &time);


    void processImageDownloadResult(const QString &contactID, const QString &imageName, bool downloaded);

private slots:
    void initTabWidget();
    void slotTabPageChanged();
    void slotNewTab();
    void slotcloseTab();


    void handleChatWindowClosed();
    void showContextMenu(const QPoint &pos);
    void handleContactChatHistoryMenuAction();
    void handleGroupChatHistoryMenuAction();


    void switchToSubWindowView();
    void switchToTabbedView();
    void switchToSeparatedView();


private:
    //	bool isChatWindowOpened(Contact *contact);

    ContactChatWidget * createContactChatWindow(Contact *contact);
    QMdiSubWindow* findChatWithContactMdiSubWindow(Contact *contact);
    ContactChatWidget * findContactChatTabWidget(Contact *contact);



    GroupChatWindow* createInterestGroupChatWindow(InterestGroup *group);
    QMdiSubWindow* findChatWithInterestGroupMdiSubWindow(InterestGroup *group);
    GroupChatWindow * findInterestGroupChatTabWidget(InterestGroup *group);

    QMenu * chatHistoryMenu();

private:
    Ui::ChatWindowManagerClass ui;


    ChatWindowDisplayStyle m_chatWindowDisplayStyle;

    QHash<QString/*Contact ID*/, ContactChatWidget *> m_contactChatWidgetHash;
    QHash<quint32/*Group ID*/, GroupChatWindow *> m_groupChatWidgetHash;

    QStringList m_contactChatHistoryList;
    QList<quint32> m_groupChatHistoryList;



};

} //namespace HEHUI


#endif // CHATWINDOWMANAGER_H
