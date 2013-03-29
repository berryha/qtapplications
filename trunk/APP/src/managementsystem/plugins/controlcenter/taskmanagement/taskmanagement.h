#ifndef TASKMANAGEMENT_H
#define TASKMANAGEMENT_H

#include <QWidget>
#include "ui_taskmanagement.h"

class TaskManagement : public QWidget
{
    Q_OBJECT

public:
    TaskManagement(QWidget *parent = 0);
    ~TaskManagement();

private:
    Ui::TaskManagementClass ui;
};

#endif // TASKMANAGEMENT_H
