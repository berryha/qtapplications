#ifndef ADUSERINFOWIDGET_H
#define ADUSERINFOWIDGET_H

#include <QWidget>

namespace Ui {
class ADUserInfoWidget;
}

class ADUserInfoWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit ADUserInfoWidget(QWidget *parent = 0);
    ~ADUserInfoWidget();
    
private:
    Ui::ADUserInfoWidget *ui;
};

#endif // ADUSERINFOWIDGET_H
