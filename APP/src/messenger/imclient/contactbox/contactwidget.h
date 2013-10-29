#ifndef CONTACTWIDGET_H
#define CONTACTWIDGET_H

#include <QWidget>
#include "ui_contactwidget.h"

#include "../contactsmanager/contact.h"


namespace HEHUI {



class ContactWidget : public QWidget
{
    Q_OBJECT

public:
    ContactWidget(Contact *contact, QWidget *parent = 0);
    ~ContactWidget();

    void updateContactToUI();


    void setExpanded(bool expand);


private:
    Ui::ContactWidgetClass ui;


    Contact *m_contact;


};

} //namespace HEHUI

#endif // CONTACTWIDGET_H
