#ifndef UPDATEPASSWORDWIDGET_H
#define UPDATEPASSWORDWIDGET_H

#include <QWidget>
#include <QCloseEvent>


#include "ui_updatepasswordwidget.h"



class UpdatePasswordWidget : public QWidget
{
    Q_OBJECT

public:
    UpdatePasswordWidget(const QString &adminAddress, quint16 adminPort, const QString &adminName, const QString &oldPassword, const QString &newPassword, QWidget *parent = 0);
    ~UpdatePasswordWidget();
    
    void informNewPassword(const QString &adminAddress, quint16 adminPort, const QString &adminName, const QString &oldPassword, const QString &newPassword);
    
private:
    void init();    
    
signals:
    void newPasswordRetreved();
    
private slots:
    void on_pushButtonRetrieve_clicked();
//    void on_pushButtonClose_clicked();
    
protected:
    void closeEvent(QCloseEvent *event);
    
    
private:
    Ui::UpdatePasswordWidgetClass ui;
    
    QString m_adminAddress;
    quint16 m_adminPort;
    
    QString m_oldPassword;
    QString m_newPassword;
    
    static bool m_newPasswordRetreved;
    
//    static bool m_running;
    

    
    
};

#endif // UPDATEPASSWORDWIDGET_H
