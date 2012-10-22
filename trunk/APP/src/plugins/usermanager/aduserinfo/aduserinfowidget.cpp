#include "aduserinfowidget.h"
#include "ui_aduserinfowidget.h"



namespace HEHUI {

ADUserInfoWidget::ADUserInfoWidget(ADSI *adsi, bool createNewUser, QWidget *parent) :
    QWidget(parent), m_adsi(adsi), m_createNewUser(createNewUser)
{
    ui.setupUi(this);



}

ADUserInfoWidget::~ADUserInfoWidget()
{


}











} //namespace HEHUI
