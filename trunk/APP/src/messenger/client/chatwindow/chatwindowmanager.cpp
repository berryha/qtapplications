#include <QMdiSubWindow>
#include <QDebug>

#include "chatwindowmanager.h"
#include "../imageresource.h"


#include "../contactsmanager/contactsmanager.h"

namespace HEHUI {

ChatWindowManager::ChatWindowManager(QWidget *parent)
    : QMainWindow(parent)
{
	ui.setupUi(this);


}

ChatWindowManager::~ChatWindowManager()
{

}

void ChatWindowManager::closeEvent(QCloseEvent *e) {

    ui.mdiArea->closeAllSubWindows();
    showMinimized();
    hide();

    e->ignore();


/*
    if (Settings::instance()->getHideOnClose()) {
    	showMinimized();
        hide();
        e->ignore();
    }else{

    ui.mdiArea->closeAllSubWindows();
 	//Clear the pairList
    	pairList.clear();

	e->accept();
    }
*/


}

void ChatWindowManager::slotNewChatWithContact(const QString &contactID){
    qDebug()<<"----ChatWindowManager::slotNewChatWithContact(const QString &id)~~~";

    Contact *contact = ContactsManager::instance()->getUser(contactID);
    if(!contact){
        qCritical()<<"XXXXChatWindowManager::slotNewMessageReceivedFromContact(...)~~Error:No such contact:"<<contactID;
        return;
    }

    QMdiSubWindow * subWindow = findChatWithContactWindow(contact);


    if(!subWindow){
        ContactChatWindow *contactChatWindow = createContactChatWindow(contact);
        //                connect(contactChatWindow, SIGNAL(sendMsgButtonClicked(const QString &, const QString &, const QStringList &)), this, SIGNAL(signalSendChatMessageToCantact(const QString &, const QString &, const QStringList &)));
        //                connect(this, SIGNAL(signalChatImageReceived(const QString&)), contactChatWindow, SLOT(updateImage(const QString&)));

        contactChatWindow->show();
        //messageWindow->setWindowIcon(HEHUI::ImageResourceBase::createIconSet(contact->getHeadPortrait()));

    }else{
        ui.mdiArea->setActiveSubWindow(subWindow);
    }

    showNormal();
    activateWindow();
    raise();
    qDebug()<<"=================================";

}

void ChatWindowManager::slotNewMessageReceivedFromContact(const QString &contactID, const QString &message, const QString &time){
    //TODO:
    qWarning()<< "----ChatWindowManager::slotNewMessageReceivedFromContact(....)~~From:"<<contactID<<" msg:"<<message<<" Time:"<<time;


    Contact *contact = ContactsManager::instance()->getUser(contactID);
    if(!contact){
        qCritical()<<"Error:No such contact:"<<contactID;
        return;
    }

    QMdiSubWindow * subWindow = findChatWithContactWindow(contact);
    ContactChatWindow *contactChatWindow;

    if(!subWindow){
        contactChatWindow = createContactChatWindow(contact);
    }else{
        contactChatWindow = qobject_cast<ContactChatWindow *>(subWindow->widget());
    }

    if(!contactChatWindow){
        qCritical()<<"ERROR! Invalid ContactChatWindow!";
        return;
    }
    contactChatWindow->show();

    update();
    showNormal();
    //show();
    raise();

    if(time.trimmed().isEmpty()){
        contactChatWindow->appendMessageReceivedFromContact(message);
    }else{
        contactChatWindow->appendMessageReceivedFromContact(message, 0, time);
    }

    QApplication::alert(subWindow);

}

void ChatWindowManager::slotNewChatWithInterestGroup(quint32 interestGroupID){

    InterestGroup *group = ContactsManager::instance()->getInterestGroup(interestGroupID);
    if(!group){
        qCritical()<<"Error:No such interest group:"<<interestGroupID;
        return;
    }

    QMdiSubWindow * subWindow = findChatWithInterestGroupWindow(group);
    GroupChatWindow *groupChatWindow;
    if(!subWindow){
        groupChatWindow = createGroupChatWindow(group);

        //            connect(groupChatWindow, SIGNAL(sendMsgButtonClicked(InterestGroup *, const QString &, const QStringList &)), this, SIGNAL(signalSendChatMessageToInterestGroup(InterestGroup *, const QString &, const QStringList &)));
        //            connect(this, SIGNAL(signalChatImageReceived(const QString&)), groupChatWindow, SLOT(updateImage(const QString&)));


        //messageWindow->setWindowIcon(HEHUI::ImageResourceBase::createIconSet(contact->getHeadPortrait()));

    }else{
        groupChatWindow = qobject_cast<GroupChatWindow *>(subWindow->widget());
        //ui.mdiArea->setActiveSubWindow(subWindow);
    }

    groupChatWindow->show();

    update();
    showNormal();
    //show();
    raise();

}

void ChatWindowManager::slotNewMessageReceivedFromInterestGroup(quint32 interestGroupID, const QString &contactID, const QString &message, const QString &time){

    InterestGroup *group = ContactsManager::instance()->getInterestGroup(interestGroupID);
    if(!group){
        qCritical()<<"Error:No such interest group:"<<interestGroupID;
        return;
    }
    if(!group->hasMember(contactID)){
        qCritical()<<QString("Error:No such member '%1' in group '%2'!").arg(contactID).arg(interestGroupID);
        return;
    }
    Contact *contact = ContactsManager::instance()->getUser(contactID);
    if(!contact){
        qCritical()<<"Error:No such contact:"<<contactID;
        return;
    }

    QMdiSubWindow * subWindow = findChatWithInterestGroupWindow(group);
    GroupChatWindow *groupChatWindow;
    if(!subWindow){
        groupChatWindow = createGroupChatWindow(group);

        //            connect(groupChatWindow, SIGNAL(sendMsgButtonClicked(InterestGroup *, const QString &, const QStringList &)), this, SIGNAL(signalSendChatMessageToInterestGroup(InterestGroup *, const QString &, const QStringList &)));
        //            connect(this, SIGNAL(signalChatImageReceived(const QString&)), groupChatWindow, SLOT(updateImage(const QString&)));


        //messageWindow->setWindowIcon(HEHUI::ImageResourceBase::createIconSet(contact->getHeadPortrait()));

    }else{
        groupChatWindow = qobject_cast<GroupChatWindow *>(subWindow->widget());
        //ui.mdiArea->setActiveSubWindow(subWindow);
    }
    groupChatWindow->show();

    update();
    showNormal();
    //show();
    raise();


    if(time.trimmed().isEmpty()){
        groupChatWindow->appendMessageReceivedFromContact(message, contact);
    }else{
        groupChatWindow->appendMessageReceivedFromContact(message, contact, time);
    }

    QApplication::alert(subWindow);

}

void ChatWindowManager::chatWindowClosed(){

    if(!ui.mdiArea->subWindowList().size()){
        showMinimized();
        hide();
    }
    
}

ContactChatWindow * ChatWindowManager::createContactChatWindow(Contact *contact){
        ContactChatWindow *contactChatWindow = new ContactChatWindow(contact);
        connect(contactChatWindow, SIGNAL(sendMsgButtonClicked(Contact *, const QString &, const QStringList &)), this, SIGNAL(signalSendChatMessageToCantact(Contact *, const QString &, const QStringList &)));
        connect(this, SIGNAL(signalChatImageReceived(const QString&)), contactChatWindow, SLOT(updateImage(const QString&)));

        QMdiSubWindow *subWindow = ui.mdiArea->addSubWindow(contactChatWindow);
        //subWindow->setWindowIcon(ImageResource::createMixedIcon((QString(RESOURCE_PATH) + "/" +contact->getFace()), contact->getOnlineState()));
        subWindow->setWindowIcon(ImageResource::createIconForContact(contact->getFace(), contact->getOnlineState()));


        connect(subWindow, SIGNAL(destroyed()), this, SLOT(chatWindowClosed()));

        return contactChatWindow;

}

QMdiSubWindow* ChatWindowManager::findChatWithContactWindow(Contact *contact){

	foreach (QMdiSubWindow *window, ui.mdiArea->subWindowList()) {
                ContactChatWindow *messageWindow = qobject_cast<ContactChatWindow *>(window->widget());
                if(!messageWindow){continue;}
                if(messageWindow->contact() == contact){
			return window;
		}
	}

	return 0;

}

GroupChatWindow* ChatWindowManager::createGroupChatWindow(InterestGroup *group){

    //InterestGroup *group = ContactsManager::instance()->getInterestGroup(interestGroupID);
    if(!group){
            qCritical()<<"Error:No such interest group!";
            return 0;
    }

    GroupChatWindow *groupChatWindow = new GroupChatWindow(group);
    connect(groupChatWindow, SIGNAL(sendMsgButtonClicked(InterestGroup *, const QString &, const QStringList &)), this, SIGNAL(signalSendChatMessageToInterestGroup(InterestGroup *, const QString &, const QStringList &)));
    connect(this, SIGNAL(signalChatImageReceived(const QString&)), groupChatWindow, SLOT(updateImage(const QString&)));


    QMdiSubWindow *subWindow = ui.mdiArea->addSubWindow(groupChatWindow);
    //subWindow->setWindowIcon(ImageResource::createMixedIcon((QString(RESOURCE_PATH) + "/" +contact->getFace()), contact->getOnlineState()));
    subWindow->setWindowIcon(ImageResource::createIconForInterestGroup());

    connect(subWindow, SIGNAL(destroyed()), this, SLOT(chatWindowClosed()));

    return groupChatWindow;
}

QMdiSubWindow* ChatWindowManager::findChatWithInterestGroupWindow(InterestGroup *group){

    foreach (QMdiSubWindow *window, ui.mdiArea->subWindowList()) {
            GroupChatWindow *groupChatWindow = qobject_cast<GroupChatWindow *>(window->widget());
            if(!groupChatWindow){continue;}
            if(groupChatWindow->interestGroup() == group){
                    return window;
            }
    }

    return 0;

}



//bool ChatWindowManager::isChatWindowOpened(Contact *contact){
//	return false;
//}

























} //namespace HEHUI
