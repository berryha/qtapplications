


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

    setWindowTitle(interestGroup->getGroupName() + "(" + QString::number(interestGroup->getGroupID()) + ")");

    ui.textBrowserAnnounce->setHtml(interestGroup->getAnnouncement());

    QStringList members = interestGroup->members();
    foreach (const QString &memberID, members) {
        Contact *contact = ContactsManager::instance()->getUser(memberID);
        QListWidgetItem *item = new QListWidgetItem(ImageResource::createIconForContact(contact->getFace(), contact->getOnlineState()), contact->getNickName(), ui.listWidgetMembers);
        item->setData(Qt::UserRole, contact->getUserID());
        ui.listWidgetMembers->addItem(item);

    }

    connect(ui.listWidgetMembers, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(memberItemActivated(QListWidgetItem*)));


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

void GroupChatWindow::appendMessageReceivedFromContact(const QString &message, Contact *contact, const QString &datetime){
    ui.chatMessageWindow->appendChatMessage(message, contact, datetime);
}

void GroupChatWindow::memberItemActivated(QListWidgetItem *memberItem){
    //TODO:

    QString memberID = memberItem->data(Qt::UserRole).toString();

    QMessageBox::information(this, "memberID", memberID);
}




} //namespace HEHUI
