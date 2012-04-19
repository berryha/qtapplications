#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"

#include "bulletinboardobject.h"

namespace HEHUI{


class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    
private:
    Ui::MainWindow ui;

    BulletinBoardObject *m_bulletinBoardObject;

};

}//namespace HEHUI

#endif // MAINWINDOW_H
