#ifndef ADUSERINFOWIDGET_H
#define ADUSERINFOWIDGET_H

#include <QWidget>

#include "ui_aduserinfowidget.h"
#include "aduser.h"

#ifdef Q_OS_WIN32
#include "HHSharedWindowsManagement/hadsi.h"
#endif




namespace HEHUI {


//namespace Ui {
//class ADUserInfoWidget;
//}

class ADUserInfoWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit ADUserInfoWidget(ADSI *adsi, ADUser *adUser, QWidget *parent = 0);
    ~ADUserInfoWidget();
    
signals:
    void signalCloseWidget();
    void signalChangesSaved();


private slots:
    void on_pushButtonEdit_clicked();
    void on_pushButtonClose_clicked();

    void saveChanges();

private:
    void initUI();
    void switchToEditMode();
    void switchToViewMode();


private:
    Ui::ADUserInfoWidgetUI ui;

    ADSI *m_adsi;
    ADUser m_adUser;


    QString m_accountName;
    QString m_displayName;
    QString m_description;
    QString m_userWorkstations;
    QString m_telephone;
    QString m_guid;
    QString m_sid;

//    QString m_cn;
//    QString m_distinguishedName;
    QString m_simpleOUString;
    QString m_fullOUString;







};

} //namespace HEHUI

#endif // ADUSERINFOWIDGET_H
