

#include <QDesktopWidget>
#include <QMessageBox>
#include <QDebug>

#include "updatepasswordwidget.h"


bool UpdatePasswordWidget::m_newPasswordRetreved = false;

UpdatePasswordWidget::UpdatePasswordWidget(const QString &adminAddress, quint16 adminPort, const QString &adminName, const QString &oldPassword, const QString &newPassword, QWidget *parent)
    : QWidget(parent), m_oldPassword(oldPassword), m_newPassword(newPassword)
{
	ui.setupUi(this);
        
        setWindowFlags(Qt::WindowStaysOnTopHint);
        
        // Get the size of screen
        QDesktopWidget* desktop = QApplication::desktop();
        QRect rect = desktop->availableGeometry(this);
        int desktopWidth = rect.width();
        int desktopHeight = rect.height();
        int windowWidth = frameGeometry().width();
        int windowHeight = frameGeometry().height();
        //move the window
        move((desktopWidth - windowWidth) / 2, (desktopHeight - windowHeight) / 2);
//        move((desktopWidth - windowWidth), (desktopHeight - windowHeight));
        
        
        m_adminAddress = "";
        m_adminPort = 0;
        
        m_oldPassword = "";
        m_newPassword = "";
        
        
        informNewPassword(adminAddress, adminPort, adminName, oldPassword, newPassword);
        
        
}

UpdatePasswordWidget::~UpdatePasswordWidget()
{
    //qWarning()<<"UpdatePasswordWidget::~UpdatePasswordWidget()";
    

}

void UpdatePasswordWidget::init(){

    ui.lineEditOldPassword->clear();
    
    ui.labelNewPassword->setText("");

    ui.groupBoxNewPassword->hide();
    
    ui.pushButtonClose->setEnabled(false);
    ui.pushButtonClose->hide();
    
    ui.groupBoxOldPassword->show();
    ui.pushButtonRetrieve->setEnabled(true);
    ui.pushButtonRetrieve->show();
    
}

void UpdatePasswordWidget::closeEvent(QCloseEvent *event){
    
    
    showMinimized();
    
    init();
    
    
    event->accept();
    
}

//bool UpdatePasswordWidget::running(){
//    return m_running;
//}

void UpdatePasswordWidget::informNewPassword(const QString &adminAddress, quint16 adminPort, const QString &adminName, const QString &oldPassword, const QString &newPassword){

    qWarning()<<"m_newPasswordRetreved:"<<m_newPasswordRetreved<<" newPassword:"<<newPassword<<" m_newPassword:"<<m_newPassword;
    
    if(m_newPasswordRetreved && (newPassword == m_newPassword)){
        return;
    }
    qWarning()<<"1";
    m_adminAddress = adminAddress;
    m_adminPort = adminPort;
    m_oldPassword = oldPassword;
    m_newPassword = newPassword;
    
    ui.labeladminInfo->setText(tr("IT. %1").arg(adminName));
    
    ui.groupBoxNewPassword->hide();
    
    ui.pushButtonClose->setEnabled(false);
    ui.pushButtonClose->hide();
    
    ui.groupBoxOldPassword->show();
    ui.pushButtonRetrieve->setEnabled(true);
    ui.pushButtonRetrieve->show();
    
    this->showNormal();
    raise();
    
}

void UpdatePasswordWidget::on_pushButtonRetrieve_clicked(){

    QString text = ui.lineEditOldPassword->text().trimmed().toLower();
    ui.lineEditOldPassword->clear();
    if(text != m_oldPassword.toLower()){
        QMessageBox::critical(this, tr("Error"), tr("Incorrect Password!"));
        ui.lineEditOldPassword->setFocus();
        return;
    }
    

    ui.groupBoxOldPassword->hide();
    ui.pushButtonRetrieve->setEnabled(false);
    ui.pushButtonRetrieve->hide();
        
    ui.labelNewPassword->setText(m_newPassword);
    ui.groupBoxNewPassword->show();
    ui.pushButtonClose->setEnabled(true);
    ui.pushButtonClose->show();
    
    m_newPasswordRetreved = true;
    
    emit newPasswordRetreved();
    
     
}

//void UpdatePasswordWidget::on_pushButtonClose_clicked(){

//    init();
//    showMinimized();
//    hide();
//}



