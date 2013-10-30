

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


//    ui.toolButtonFace->installEventFilter(this);
//    installEventFilter(this);

    setExpanded(false);




    //setStyleSheet(":hover { background: yellow; }");

    //ui.labelDisplayName->setStyleSheet("{color: palette(link);}");



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
    }else{
        ui.frameTools->hide();
        ui.pushButtonFace->setIconSize(QSize(22, 22));
    }

    updateGeometry();


}

bool ContactWidget::isMouseUnderFace(){
    QPoint mousePoint = mapFromGlobal(QCursor::pos());
    return ui.pushButtonFace->rect().contains(mousePoint);
}


//bool ContactWidget::eventFilter(QObject *obj, QEvent *event)
//{
//    qDebug()<<"--------0:"<<event->type();


//    switch (event->type()) {
//    case QEvent::Enter:
//    {
//        qDebug()<<"----QEvent::Enter";
//        setBackgroundRole(QPalette::LinkVisited	);
//        repaint();
//    }
//        break;
//    case QEvent::Leave:
//    {
//        qDebug()<<"----QEvent::Leave";

//    }
//        break;
//    default:
//        break;
//    }

//    return QObject::eventFilter(obj, event);


////    if (event->type() == QEvent::HoverEnter) {

//////        if(obj == ui.toolButtonFace){
////            qDebug()<<"------------!!!------!!!---------------";
//////        }

////        return true;
////    } else {
////        // standard event processing
////        return QObject::eventFilter(obj, event);
////    }

//}





} //namespace HEHUI
