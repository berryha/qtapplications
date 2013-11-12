


#include <QMessageBox>

#include "groupchatwindow.h"
#include "contactsmanager/contactsmanager.h"
#include "imageresource.h"


namespace HEHUI {


GroupChatWindow::GroupChatWindow(InterestGroup *interestGroup, QWidget *parent)
    : QWidget(parent), m_interestGroup(interestGroup)
{
    ui.setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    Q_ASSERT_X(interestGroup != 0, "GroupChatWindow", "Invalid InterestGroup!");

    ui.chatMessageWindow->setInterestGroup(interestGroup);

    connect(ui.chatMessageWindow, SIGNAL(sendMsgButtonClicked(InterestGroup*, const QString&, const QStringList&)), this, SIGNAL(sendMsgButtonClicked(InterestGroup*, const QString&, const QStringList&)));
    connect(ui.chatMessageWindow, SIGNAL(signalRequestDownloadImage(const QString&, const QString&)), this, SIGNAL(signalRequestDownloadImage(const QString &, const QString &)));
    connect(ui.chatMessageWindow, SIGNAL(signalCloseWindow()), this, SIGNAL(signalCloseWindow()));


    setWindowTitle(interestGroup->getGroupName() + "(" + QString::number(interestGroup->getGroupID()) + ")");

    ui.textBrowserAnnounce->setHtml(interestGroup->getAnnouncement());

    IMUser *myself = IMUser::instance();
    QString myID = myself->getUserID();

    //Add members
    ContactsManager *contactsManager =  ContactsManager::instance();
    QStringList members = interestGroup->members();
    members.removeAll(myID);
    foreach (const QString &memberID, members) {
        Contact *contact = contactsManager->getUser(memberID);
        QListWidgetItem *item = new QListWidgetItem(ImageResource::createIconForContact(contact->getFace(), contact->getOnlineState()), contact->getNickName(), ui.listWidgetMembers);
        item->setData(Qt::UserRole, contact->getUserID());
        ui.listWidgetMembers->addItem(item);
    }

    //Add myself
    QListWidgetItem *myItem = new QListWidgetItem(ImageResource::createIconForContact(myself->getFace(), myself->getOnlineState()), myself->getNickName(), ui.listWidgetMembers);
    myItem->setData(Qt::UserRole, myID);
    ui.listWidgetMembers->addItem(myItem);


    connect(ui.listWidgetMembers, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(memberItemActivated(QListWidgetItem*)));

    ui.chatMessageWindow->setEnabled(interestGroup->getState());

}

GroupChatWindow::~GroupChatWindow()
{
    emit toBeDstroyed();
}

void GroupChatWindow::closeEvent(QCloseEvent * event){

    if(ui.chatMessageWindow->close()){
        event->accept();
    }else{
        event->ignore();
    }
}

InterestGroup * GroupChatWindow::interestGroup(){
    return m_interestGroup;
}

bool GroupChatWindow::isDownloadingImage(const QString &imageName){
    return ui.chatMessageWindow->isDownloadingImage(imageName);
}

void GroupChatWindow::processImageDownloadResult(const QString &imageName, bool downloaded){
    ui.chatMessageWindow->processImageDownloadResult(imageName, downloaded);
}

void GroupChatWindow::contactOnlineStateChanged(Contact *contact){
    //TODO

    QListWidgetItem * item = memberItem(contact->getUserID());
    if(!item){return;}

    item->setIcon(ImageResource::createIconForContact(contact->getFace(), contact->getOnlineState()));
}

void GroupChatWindow::memberJoinedOrQuitted(const QString &memberID, bool join){

    QListWidgetItem * item = memberItem(memberID);

    if(join){
        if(item){return;}

        Contact *contact = ContactsManager::instance()->getUser(memberID);
        QListWidgetItem *item = new QListWidgetItem(ImageResource::createIconForContact(contact->getFace(), contact->getOnlineState()), contact->getNickName(), ui.listWidgetMembers);
        item->setData(Qt::UserRole, contact->getUserID());
        ui.listWidgetMembers->addItem(item);
    }else{
        if(!item){return;}

        item = ui.listWidgetMembers->takeItem(ui.listWidgetMembers->row(item));
        delete item;
    }

}

void GroupChatWindow::appendMessageReceivedFromContact(const QString &message, Contact *contact, const QString &datetime){
    ui.chatMessageWindow->appendChatMessage(message, contact, datetime);
}

void GroupChatWindow::memberItemActivated(QListWidgetItem *memberItem){
    //TODO:

    QString memberID = memberItem->data(Qt::UserRole).toString();

    QMessageBox::information(this, "memberID", memberID);
}

QListWidgetItem * GroupChatWindow::memberItem(const QString &memberID){

    int count = ui.listWidgetMembers->count();
    for(int i=0; i<count; i++){
        QListWidgetItem *item = ui.listWidgetMembers->item(i);
        if(!item){continue;}
        if(memberID == item->data(Qt::UserRole).toString()){
            return item;
        }
    }

}




} //namespace HEHUI
