#include "contactchatwidget.h"


ContactChatWidget::ContactChatWidget(Contact *contact, QWidget *parent)
    : QWidget(parent), m_contact(contact)
{
	ui.setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);


    connect(ui.chatMessageWindow, SIGNAL(sendMsgButtonClicked(Contact*, const QString&, const QStringList&)), this, SIGNAL(sendMsgButtonClicked(Contact*, const QString&, const QStringList&)));
    connect(ui.chatMessageWindow, SIGNAL(signalRequestDownloadImage(const QString &, const QString &)), this, SIGNAL(signalRequestDownloadImage(const QString &, const QString &)));
    connect(ui.chatMessageWindow, SIGNAL(signalShowMessageHistory(bool)), this, SLOT(showMessageHistory(bool)));
    connect(ui.chatMessageWindow, SIGNAL(signalCloseWindow()), this, SIGNAL(signalCloseWindow()));


    m_displayName = tr("Unknown");

    if(contact){
        setContact(contact);
    }

    m_messageHistoryView = 0;


    m_preferedSize = QSize();

    QTimer::singleShot(1, this, SLOT(setPreferedSize()));



}

void ContactChatWidget::setContact(Contact *contact){

    Q_ASSERT_X(contact != 0, "ContactChatWidget::setContact", "NULL Contact!");

    this->m_contact = contact;

    ui.chatMessageWindow->setContact(contact);

    m_displayName = contact->displayName();
    QString contactID = contact->getUserID();
    if(contactID != m_displayName){
        m_displayName = m_displayName + "("  + contactID + ")";
    }
    setWindowTitle(m_displayName);

}

ContactChatWidget::~ContactChatWidget()
{
    emit toBeDstroyed();
}

Contact * ContactChatWidget::contact(){
    return m_contact;
}

QString ContactChatWidget::displayName() const{
    return m_displayName;
}

bool ContactChatWidget::isDownloadingImage(const QString &imageName){
    return ui.chatMessageWindow->isDownloadingImage(imageName);
}

void ContactChatWidget::processImageDownloadResult(const QString &imageName, bool downloaded){
    return ui.chatMessageWindow->processImageDownloadResult(imageName, downloaded);
}

void ContactChatWidget::contactOnlineStateChanged(){
    //TODO

}

//QSize ContactChatWidget::sizeHint(){
//    return QSize(640, 480);
//}

void ContactChatWidget::closeEvent(QCloseEvent * event){

    if(ui.chatMessageWindow->close()){
        event->accept();
    }else{
        event->ignore();
    }
}



void ContactChatWidget::appendMessageReceivedFromContact(const QString &message, Contact *contact, const QString &datetime ){
    ui.chatMessageWindow->appendChatMessage(message, contact, datetime);
}

void ContactChatWidget::processContactHistoryMessage(const QStringList &messages, bool canFetchMore){
    if(m_messageHistoryView){
        m_messageHistoryView->appendHistoryChatMessages(messages, canFetchMore);
    }
}

void ContactChatWidget::showMessageHistory(bool show){

    ui.chatMessageWindow->resize(0,0);

    if(show){
        setMinimumSize(QSize(0, 0));

        if(!m_messageHistoryView){
            m_messageHistoryView = new MessageHistoryView(this);
            m_messageHistoryView->setMinimumWidth(m_messageHistoryView->width());
            connect(m_messageHistoryView, SIGNAL(signalRequestHistoryMessage(const QString &, const QString &, const QString &, bool)), this, SLOT(requestContactHistoryMessage(const QString &, const QString &, const QString &, bool)));

            ui.tabWidget->addTab(m_messageHistoryView, tr("Message History"));
        }
        ui.tabWidget->setCurrentWidget(m_messageHistoryView);

    }else{
        setMinimumSize(m_preferedSize);

        ui.tabWidget->removeTab(ui.tabWidget->indexOf(m_messageHistoryView));
        delete m_messageHistoryView;
        m_messageHistoryView = 0;

        resize(m_preferedSize);


    }


}

void ContactChatWidget::requestContactHistoryMessage(const QString &startTime, const QString &endTime, const QString &content, bool requestBackword){
    Q_ASSERT(m_contact);
    emit signalRequestContactHistoryMessage(startTime, endTime, content, requestBackword, m_contact->getUserID());
}

void ContactChatWidget::setPreferedSize(){
    if(!m_preferedSize.isValid()){
        m_preferedSize = size();
        setMinimumSize(m_preferedSize);
        ui.chatMessageWindow->setMinimumWidth(ui.chatMessageWindow->width());
    }

}

