#ifndef ADUSERINFOWIDGET_H
#define ADUSERINFOWIDGET_H

#include <QWidget>

#include "ui_aduserinfowidget.h"

#ifdef Q_OS_WIN32
#include "HHSharedWindowsManagement/hadsi.h"
#endif




namespace HEHUI {


//namespace Ui {
//class ADUserInfoWidget;
//}

class ADUserInfoWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit ADUserInfoWidget(ADSI *adsi, QWidget *parent = 0);
    ~ADUserInfoWidget();
    
private:
    Ui::ADUserInfoWidgetUI ui;



};

} //namespace HEHUI

#endif // ADUSERINFOWIDGET_H
