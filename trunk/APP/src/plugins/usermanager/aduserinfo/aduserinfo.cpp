#include "aduserinfo.h"

ADUserInfo::ADUserInfo(QWidget *parent) :
    QWidget(parent)
{
    ui.setupUi(this);



}

void ADUserInfo::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui.retranslateUi(this);
        break;
    default:
        break;
    }
}
