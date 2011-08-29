#ifndef USERINFOTIPWINDOW_H
#define USERINFOTIPWINDOW_H

#include <QtGui/QWidget>
#include "ui_userinfotipwindow.h"


#include "contactsmanager/contact.h"


namespace HEHUI {


class UserInfoTipWindow : public QWidget
{
    Q_OBJECT

public:
    UserInfoTipWindow(QWidget *parent = 0, Qt::WindowFlags fl = Qt::Popup | Qt::FramelessWindowHint | Qt::SplashScreen);
    ~UserInfoTipWindow();



protected:
//    bool event(QEvent *event);


public slots:
    void showUserInfoTip(Contact *contact, const QPoint &global_pos);
    void hideUserInfoTip();


private slots:
    void on_toolButtonEmail_clicked(){
        sendEmail();
    }

private:
    void sendEmail();

private:
    Ui::UserInfoTipWindowClass ui;

    Contact *m_contact;

    QTimer *timer;






};

} //namespace HEHUI

#endif // USERINFOTIPWINDOW_H
