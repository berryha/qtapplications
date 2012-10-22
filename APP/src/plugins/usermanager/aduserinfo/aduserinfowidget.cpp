#include "aduserinfowidget.h"
#include "ui_aduserinfowidget.h"

ADUserInfoWidget::ADUserInfoWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ADUserInfoWidget)
{
    ui->setupUi(this);
}

ADUserInfoWidget::~ADUserInfoWidget()
{
    delete ui;
}
