/*
 ****************************************************************************
 * applyforregistrationwidget.h
 *
 * Created on: 2010-5-29
 *     Author: 贺辉
 *    License: LGPL
 *    Comment:
 *
 *
 *    =============================  Usage  =============================
 *|
 *|
 *    ===================================================================
 *
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 ****************************************************************************
 */

/*
 ***************************************************************************
 * Last Modified on: 2010-5-29
 * Last Modified by: 贺辉
 ***************************************************************************
 */





#ifndef APPLYFORREGISTRATIONWIDGET_H
#define APPLYFORREGISTRATIONWIDGET_H

#include <QWidget>
#include "ui_applyforregistrationwidget.h"

#include "../../sharedim/constants_global_shared.h"


namespace HEHUI{

class ApplyForRegistrationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ApplyForRegistrationWidget(QWidget *parent = 0);
    ~ApplyForRegistrationWidget();
    
    
    QString userID();
    QString email();
    QString password();

protected:
    void changeEvent(QEvent *e);
    
signals:
    void requestRegistration();
    void registration(const QString &userID, const QString &password, const QString &email);
        
public slots:
    void slotProcessRegistrationModeInfo(bool canRegister, bool extraMessage, IM::RegistrationMode regMode, bool requireActivation);
    void slotProcessRegistrationResult(quint8 errorTypeCode, const QString &message);

private slots:
    void on_pushButtonRegister_clicked();
    bool isUserIDValid();
    bool isPasswordValid();
    
    void requestRegistrationTimeout();
    void registrationTimeout();

    

private:
    Ui::ApplyForRegistrationWidgetUI ui;

//    QRegExpValidator *validator;
    
    bool m_registrationModeInfoResponseReceived;
    bool m_registrationResultReceived;

    IM::RegistrationMode m_registrationMode;
    bool m_requireActivation;


};
} //namespace HEHUI

#endif // APPLYFORREGISTRATIONWIDGET_H
