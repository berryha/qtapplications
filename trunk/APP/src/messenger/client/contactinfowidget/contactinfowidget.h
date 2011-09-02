#ifndef CONTACTINFOWIDGET_H
#define CONTACTINFOWIDGET_H

#include <QtGui/QWidget>
#include "ui_contactinfowidget.h"

#include "../../sharedim/imuserbase.h"


#include "HHSharedGUI/hwidgetbase.h"

namespace HEHUI {

class ContactInfoWidget : public WidgetBase
{
    Q_OBJECT

public:
    ContactInfoWidget(IMUserBase *user, QWidget *parent = 0, Qt::WindowFlags fl = Qt::Dialog);
    ~ContactInfoWidget();


    QSystemTrayIcon *SystemTrayIcon(){return 0;}

protected:
    void languageChange();
    void closeEvent(QCloseEvent *);


private slots:

    void on_toolButtonUserFace_clicked();
    void on_pushButtonGeneralInformation_clicked();
    void on_pushButtonMoreInformation_clicked();

    void on_pushButtonSettings_clicked();
    void on_pushButtonUpdate_clicked();
    void on_pushButtonOK_clicked();
    void on_pushButtonCancel_clicked();
    void on_pushButtonApply_clicked();


private:
    void initUI();


private:
    Ui::ContactInfoWidgetClass ui;



    IMUserBase *m_user;









};

} //namespace HEHUI

#endif // CONTACTINFOWIDGET_H
