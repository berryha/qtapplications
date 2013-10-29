#include "contactwidget.h"

#include "../imageresource.h"

namespace HEHUI{


ContactWidget::ContactWidget(Contact *contact, QWidget *parent)
    : QWidget(parent), m_contact(contact)
{
	ui.setupUi(this);

    Q_ASSERT(m_contact);

    updateContactToUI();


}

ContactWidget::~ContactWidget()
{

}

void ContactWidget::updateContactToUI(){

    ui.toolButtonFace->setIcon(ImageResource::createIconForContact(m_contact->getFace(), m_contact->getOnlineState()));
    ui.labelDisplayName->setText(m_contact->displayName());


}





} //namespace HEHUI