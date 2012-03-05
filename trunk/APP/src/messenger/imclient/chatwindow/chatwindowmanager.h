#ifndef CHATWINDOWMANAGER_H
#define CHATWINDOWMANAGER_H

#include <QtGui/QMainWindow>
#include <QList>
#include <QPair>
#include <QCloseEvent>


#include "../ui/ui_chatwindowmanager.h"
#include "../contactsmanager/contact.h"
//#include "chatmessagewindow.h"
#include "groupchatwindow.h"
#include "contactchatwindow.h"

//#include "../../shared/core/singleton.h"
#include "HHSharedCore/hsingleton.h"



namespace HEHUI {

class ChatWindowManager : public QMainWindow, public Singleton<ChatWindowManager>
{
    Q_OBJECT
    friend class Singleton<ChatWindowManager>;

public:
    ChatWindowManager(QWidget *parent = 0);
    ~ChatWindowManager();

protected:
	void closeEvent(QCloseEvent *);

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
        void chatWindowClosed();
        
private:
//	bool isChatWindowOpened(Contact *contact);

        ContactChatWindow *createContactChatWindow(Contact *contact);
        QMdiSubWindow* findChatWithContactWindow(Contact *contact);

        GroupChatWindow* createGroupChatWindow(InterestGroup *group);
        QMdiSubWindow* findChatWithInterestGroupWindow(InterestGroup *group);



private:
    Ui::ChatWindowManagerClass ui;

    //QList<QPair<Contact*, QMdiSubWindow*> > pairList;





};
} //namespace HEHUI
#endif // CHATWINDOWMANAGER_H
