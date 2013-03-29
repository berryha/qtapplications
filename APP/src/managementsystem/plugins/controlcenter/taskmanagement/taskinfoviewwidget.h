#ifndef TASKINFOVIEWWIDGET_H
#define TASKINFOVIEWWIDGET_H

#include <QWidget>
#include "ui_taskinfoviewwidget.h"

class TaskInfoViewWidget : public QWidget
{
    Q_OBJECT

public:
    TaskInfoViewWidget(QWidget *parent = 0);
    ~TaskInfoViewWidget();

private:
    Ui::TaskInfoViewWidgetClass ui;
};

#endif // TASKINFOVIEWWIDGET_H
