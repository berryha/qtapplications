#include "mainwindow.h"



namespace HEHUI
{


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    ui.setupUi(this);

    setWindowFlags(Qt::Window
                   | Qt::WindowMinimizeButtonHint
                   | Qt::WindowStaysOnTopHint);

    m_bulletinBoardObject = new BulletinBoardObject(this);

}

MainWindow::~MainWindow(){
    delete m_bulletinBoardObject;
    m_bulletinBoardObject = 0;
}







} //namespace HEHUI
