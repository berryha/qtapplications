#ifndef ADUSERINFO_H
#define ADUSERINFO_H

#include "ui_aduserinfo.h"

class ADUserInfo : public QWidget
{
    Q_OBJECT
    
public:
    explicit ADUserInfo(QWidget *parent = 0);
    
protected:
    void changeEvent(QEvent *e);
    
private:
    Ui::ADUserInfo ui;




};

#endif // ADUSERINFO_H
