

#include <QHoverEvent>



#include "contactwidget.h"

#include "../imageresource.h"




namespace HEHUI{


ContactWidget::ContactWidget(Contact *contact, QWidget *parent)
    : QWidget(parent), m_contact(contact)
{
	ui.setupUi(this);


    Q_ASSERT(m_contact);

    updateContactToUI();

    setExpanded(false);




    //setStyleSheet(":hover { background: yellow; }");

    //ui.labelDisplayName->setStyleSheet("{color: palette(link);}");

//setMouseTracking(true);

//    ui.toolButtonFace->installEventFilter(this);
    installEventFilter(this);

    setAutoFillBackground(true);

}

ContactWidget::~ContactWidget()
{

}

void ContactWidget::updateContactToUI(){

    ui.pushButtonFace->setIcon(ImageResource::createIconForContact(m_contact->getFace(), m_contact->getOnlineState()));
    ui.labelDisplayName->setText(m_contact->displayName());
}

void ContactWidget::setExpanded(bool expand){

    if(expand){
        ui.frameTools->show();
        ui.pushButtonFace->setIconSize(QSize(36, 36));
        setBackgroundRole(QPalette::Highlight);

    }else{
        ui.frameTools->hide();
        ui.pushButtonFace->setIconSize(QSize(22, 22));
        setBackgroundRole(QPalette::Base);
    }

    updateGeometry();

}

bool ContactWidget::isExpanded(){
    return ui.frameTools->isVisible();
}

bool ContactWidget::isMouseUnderFace(){
    QPoint mousePoint = mapFromGlobal(QCursor::pos());
    return ui.pushButtonFace->rect().contains(mousePoint);
}


bool ContactWidget::eventFilter(QObject *obj, QEvent *event)
{
    qDebug()<<"ContactWidget::eventFilter:"<<event->type();


    switch (event->type()) {
    case QEvent::Enter:
    {
        //ui.labelDisplayName->setText(ui.labelDisplayName->text()+"(Enter)");
        qDebug()<<"----QEvent::Enter"<<" "<<this;

        if(!isExpanded()){
            setBackgroundRole(QPalette::Midlight);
        }

        return true;
    }
        break;
    case QEvent::Leave:
    {
        //ui.labelDisplayName->setText(ui.labelDisplayName->text()+"(Leave)");
        if(!isExpanded()){
            setBackgroundRole(QPalette::Base);
        }

        qDebug()<<"----QEvent::Leave"<<" "<<this;

        return true;
    }
        break;

//    case QEvent::MouseButtonPress:
//    {
//        setBackgroundRole(QPalette::Highlight);
//        qDebug()<<"----QEvent::MouseButtonPress"<<" "<<this;
//        return false;
//    }
//        break;

    default:
        break;
    }

    return QObject::eventFilter(obj, event);

}

//bool ContactWidget::event(QEvent *event){

//    switch(event->type()){

//    case QEvent::Enter:
//    {
//        QHoverEvent *e = static_cast<QHoverEvent *>(event);
//        qDebug()<<"----QEvent::HoverEnter";
//    }
//        break;

//    default:
//        break;

//    }

//    qDebug()<<"-----1-----:"<<event->type();

//    return QWidget::event(event);

//}




} //namespace HEHUI
