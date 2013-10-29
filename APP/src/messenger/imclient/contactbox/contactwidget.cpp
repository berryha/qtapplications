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

}

ContactWidget::~ContactWidget()
{

}

void ContactWidget::updateContactToUI(){

    ui.toolButtonFace->setIcon(ImageResource::createIconForContact(m_contact->getFace(), m_contact->getOnlineState()));
    ui.labelDisplayName->setText(m_contact->displayName());
}

void ContactWidget::setExpanded(bool expand){

    if(expand){
//        ui.frameTools->show();
//        ui.toolButtonFace->setIconSize(QSize(42, 42));
        ui.frameExpanded->show();
        ui.frameLite->hide();


    }else{
//        ui.frameTools->hide();
//        ui.toolButtonFace->setIconSize(QSize(22, 22));
        ui.frameExpanded->hide();
        ui.frameLite->show();


    }


}





} //namespace HEHUI
