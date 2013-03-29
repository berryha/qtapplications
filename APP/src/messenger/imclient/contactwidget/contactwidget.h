#ifndef CONTACTWIDGET_H
#define CONTACTWIDGET_H

#include <QWidget>
#include "ui_contactwidget.h"

class ContactWidget : public QWidget
{
    Q_OBJECT

public:
    ContactWidget(QWidget *parent = 0);
    ~ContactWidget();

private:
    Ui::ContactWidgetClass ui;
};

#endif // CONTACTWIDGET_H
