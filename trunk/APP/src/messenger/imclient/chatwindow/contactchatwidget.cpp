#include "contactchatwidget.h"


ContactChatWidget::ContactChatWidget(Contact *contact, QWidget *parent)
    : QWidget(parent), m_contact(contact)
{
	ui.setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose);

    connect(ui.chatMessageWindow, SIGNAL(sendMsgButtonClicked(Contact*, const QString&, const QStringList&)), this, SIGNAL(sendMsgButtonClicked(Contact*, const QString&, const QStringList&)));


//        Q_ASSERT_X(contact != 0, "ContactChatWindow", "NULL Contact!");

//        ui.chatMessageWindow->setContact(contact);


//        QString displayName = contact->getNickName();
//        QString contactID = contact->getUserID();
//        if (displayName.isEmpty()) {
//            displayName = contactID;
//        }else if(contactID != displayName){
//            displayName = contact->getNickName() + "("  + contact->getUserID() + ")";
//        }
//        setWindowTitle(displayName);


    m_displayName = tr("Unknown");

    if(contact){
        setContact(contact);
    }

}

void ContactChatWidget::setContact(Contact *contact){

    Q_ASSERT_X(contact != 0, "ContactChatWidget::setContact", "NULL Contact!");

    this->m_contact = contact;

    ui.chatMessageWindow->setContact(contact);

    m_displayName = contact->getNickName();
    QString contactID = contact->getUserID();
    if (m_displayName.isEmpty()) {
        m_displayName = contactID;
    }else if(contactID != m_displayName){
        m_displayName = contact->getNickName() + "("  + contact->getUserID() + ")";
    }
    setWindowTitle(m_displayName);

}

Contact * ContactChatWidget::contact(){
    return m_contact;
}

QString ContactChatWidget::displayName() const{
    return m_displayName;
}

ContactChatWidget::~ContactChatWidget()
{
    emit toBeDstroyed();
}

void ContactChatWidget::appendMessageReceivedFromContact(const QString &message, Contact *contact, const QString &datetime ){
    ui.chatMessageWindow->appendMessageReceivedFromContact(message, contact, datetime);
}

void ContactChatWidget::updateImage(const QString &imageName){
    ui.chatMessageWindow->updateImage(imageName);
}
