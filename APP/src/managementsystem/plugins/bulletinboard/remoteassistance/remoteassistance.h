#ifndef REMOTEASSISTANCE_H
#define REMOTEASSISTANCE_H

#include <QWidget>
#include <QTimer>
#include <QCloseEvent>

#include "ui_remoteassistance.h"
//#include "../networkmanager/bulletinboardpacketsparser.h"



namespace HEHUI {


class RemoteAssistance : public QWidget
{
    Q_OBJECT

public:
    RemoteAssistance(const QString &adminAddress, quint16 adminPort, const QString &adminName, QWidget *parent = 0);
    ~RemoteAssistance();

    
    void requestRemoteAssistance(const QString &adminAddress, quint16 adminPort, const QString &adminName);
    

signals:
    void signalUserResponseRemoteAssistance(const QString &adminAddress, quint16 adminPort, bool accept);
    
private slots:
    void updateTimeInfo();
    void startVNC();
    
    void on_pushButtonAccept_clicked();
    void on_pushButtonReject_clicked();
    
protected:
    void closeEvent(QCloseEvent *event);
    
private:
    Ui::RemoteAssistanceClass ui;
    
    QString m_adminAddress;
    quint16 m_adminPort;
    
    QTimer *timer;
    
    
};

} //namespace HEHUI

#endif // REMOTEASSISTANCE_H
