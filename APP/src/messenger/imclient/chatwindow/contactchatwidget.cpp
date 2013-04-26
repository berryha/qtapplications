#include "contactchatwidget.h"


ContactChatWidget::ContactChatWidget(Contact *contact, QWidget *parent)
    : QWidget(parent), m_contact(contact)
{
	ui.setupUi(this);

        Q_ASSERT_X(contact != 0, "ContactChatWindow", "NULL Contact!");

        ui.chatMessageWindow->setContact(contact);

        connect(ui.chatMessageWindow, SIGNAL(sendMsgButtonClicked(Contact*, const QString&, const QStringList&)), this, SIGNAL(sendMsgButtonClicked(Contact*, const QString&, const QStringList&)));

        QString displayName = contact->getNickName();
        QString contactID = contact->getUserID();
        if (displayName.isEmpty()) {
            displayName = contactID;
        }else if(contactID != displayName){
            displayName = contact->getNickName() + "("  + contact->getUserID() + ")";
        }
        setWindowTitle(displayName);

}

ContactChatWidget::~ContactChatWidget()
{

}

void ContactChatWidget::appendMessageReceivedFromContact(const QString &message, Contact *contact, const QString &datetime ){
    ui.chatMessageWindow->appendMessageReceivedFromContact(message, contact, datetime);
}

void ContactChatWidget::updateImage(const QString &imageName){
    ui.chatMessageWindow->updateImage(imageName);
}
