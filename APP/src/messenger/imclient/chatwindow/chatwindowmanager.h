#ifndef CHATWINDOWMANAGER_H
#define CHATWINDOWMANAGER_H

#include <QMainWindow>
#include <QList>
#include <QPair>
#include <QCloseEvent>


#include "../ui/ui_chatwindowmanager.h"
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
    enum ChatWindowDisplayStyle{TabbedChatWindow = 0, MDIChatWindow, SeparateChatWindow};
    ChatWindowManager(QWidget *parent = 0);
    ~ChatWindowManager();

protected:
    void closeEvent(QCloseEvent *);


public:
    void setChatWindowDisplayStyle(ChatWindowDisplayStyle style);
    ChatWindowDisplayStyle getChatWindowDisplayStyle();
    void switchChatWindowDisplayStyle(ChatWindowDisplayStyle style);


signals:
    void signalSendChatMessageToCantact(Contact *contact, const QString &message, const QStringList &imageList);
    void signalSendChatMessageToInterestGroup(InterestGroup *interestGroup, const QString &message, const QStringList &imageList);

    void signalChatImageReceived(const QString &imageName);

public slots:
    void slotNewChatWithContact(const QString &contactID);
    void slotNewMessageReceivedFromContact(const QString &contactID, const QString &message, const QString &time);

    void slotNewChatWithInterestGroup(quint32 interestGroupID);
    void slotNewMessageReceivedFromInterestGroup(quint32 interestGroupID, const QString &contactID, const QString &message, const QString &time);


private slots:
    void initTabWidget();
    void slotTabPageChanged();
    void slotNewTab();
    void slotcloseTab();


    void handleChatWindowClosed();
    void showContextMenu(const QPoint &pos);

    void switchToSubWindowView();
    void switchToTabbedView();


private:
    //	bool isChatWindowOpened(Contact *contact);

    ContactChatWidget * createContactChatWindow(Contact *contact);
    QMdiSubWindow* findChatWithContactMdiSubWindow(Contact *contact);
    ContactChatWidget * findContactChatTabWidget(Contact *contact);


    GroupChatWindow* createGroupChatWindow(InterestGroup *group);
    QMdiSubWindow* findChatWithInterestGroupMdiSubWindow(InterestGroup *group);
    GroupChatWindow * findInterestGroupChatTabWidget(InterestGroup *group);


private:
    Ui::ChatWindowManagerClass ui;


    ChatWindowDisplayStyle m_chatWindowDisplayStyle;

    QHash<QString/*Contact ID*/, ContactChatWidget *> m_contactChatWidgetHash;
    QHash<quint32/*Group ID*/, GroupChatWindow *> m_groupChatWidgetHash;



    //QList<QPair<Contact*, QMdiSubWindow*> > pairList;





};

} //namespace HEHUI


#endif // CHATWINDOWMANAGER_H
