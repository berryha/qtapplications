
//#include <QWidget>
#include <QDesktopWidget>
#include <QProcess>
#include <QDebug>

#include "remoteassistance.h"


namespace HEHUI {


RemoteAssistance::RemoteAssistance(const QString &adminAddress, quint16 adminPort, const QString &adminName, QWidget *parent)
    : QWidget(parent)
{


    ui.setupUi(this);

    setWindowFlags(Qt::WindowStaysOnTopHint);

    // Get the size of screen
    QDesktopWidget* desktop = QApplication::desktop();
    QRect rect = desktop->availableGeometry(this);
//    int desktopWidth = rect.width();
//    int desktopHeight = rect.height();
//    int windowWidth = frameGeometry().width();
//    int windowHeight = frameGeometry().height();
//    //move the window
//    move((desktopWidth - windowWidth) / 2, (desktopHeight - windowHeight) / 2);

    move(rect.center());

    requestRemoteAssistance(adminAddress, adminPort, adminName);

    //        updateTimeInfo();

    //        timer = new QTimer(this);
    //        timer->setInterval(1000);
    //        connect(timer, SIGNAL(timeout()), this, SLOT(updateTimeInfo()));
    //        timer->start();

}

RemoteAssistance::~RemoteAssistance()
{
    //qWarning()<<"RemoteAssistance::~RemoteAssistance()";
}

void RemoteAssistance::closeEvent(QCloseEvent *event){

    showMinimized();
    
    if(timer->isActive()){
        emit signalUserResponseRemoteAssistance(m_adminAddress, m_adminPort, false);
        
        timer->stop();
    }
    
    
    event->accept();
    
    
    
    //deleteLater();

}

void RemoteAssistance::requestRemoteAssistance(const QString &adminAddress, quint16 adminPort, const QString &adminName){
    
    m_adminAddress = adminAddress;
    m_adminPort = adminPort;
    
    ui.labelMessage->setText(tr("Administrator %1@%2 request to control your computer remotely!").arg(adminName).arg(adminAddress));
    updateTimeInfo();
    
    if(!timer){
        timer = new QTimer(this);
        timer->setInterval(1000);
        connect(timer, SIGNAL(timeout()), this, SLOT(updateTimeInfo()));
    }

    timer->start();
    
    showNormal();
    raise();
    
    
}

void RemoteAssistance::updateTimeInfo(){
    static int sec = 30;
    sec--;
    if(sec ==0){
        ui.labelTimeInfo->setText(tr("Timeout! Automatically Accept!"));
        
        startVNC();
        return;
    }
    
    ui.labelTimeInfo->setText(tr("Automatically accept after <font color=red>%1</font> seconds!").arg(QString::number(sec)));
}

void RemoteAssistance::startVNC(){
    emit signalUserResponseRemoteAssistance(m_adminAddress, m_adminPort, true);

    timer->stop();

    ui.pushButtonAccept->setEnabled(false);
    ui.pushButtonReject->setEnabled(false);
    
    QProcess process;
    process.startDetached(QString("tvnserver.exe -controlapp -connect %1").arg(m_adminAddress));

    
    this->close();
    
}

void RemoteAssistance::on_pushButtonAccept_clicked(){
    
    startVNC();
}

void RemoteAssistance::on_pushButtonReject_clicked(){
    emit signalUserResponseRemoteAssistance(m_adminAddress, m_adminPort, false);
    
    this->close();
    
}




















} //namespace HEHUI

