#ifndef SYSTEMMANAGEMENTWIDGET_H
#define SYSTEMMANAGEMENTWIDGET_H

#include <QtGui/QWidget>
#include <QCloseEvent>
#include <QMenu>


#include "ui_systemmanagementwidget.h"

#include "networkmanager/controlcenterpacketsparser.h"




namespace HEHUI {


class SystemManagementWidget : public QWidget
{
    Q_OBJECT

public:
    SystemManagementWidget(const QString &adminName, ControlCenterPacketsParser *parser, const QString &computerName, const QString &users, const QString &peerIPAddress, const QString &peerMACAddress, bool usbsdEnabled = false, bool programesEnabled = false, const QString &admins = "", QWidget *parent = 0);
    ~SystemManagementWidget();


    
    
signals:
    void requestRemoteAssistance();

public slots:
    void setControlCenterPacketsParser(ControlCenterPacketsParser *parser);


protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void on_toolButtonVerify_clicked();
    void on_pushButtonUSBSD_clicked();
    void on_pushButtonPrograms_clicked();
    void on_pushButtonShowAdmin_clicked();
    void on_pushButtonRemoteAssistance_clicked();

    void on_actionAddAdmin_triggered();
    void on_actionDeleteAdmin_triggered();

    void on_pushButtonMMC_clicked();
    void on_pushButtonCMD_clicked();
    void on_pushButtonRegedit_clicked();
    void on_pushButtonOtherEXE_clicked();

    void on_toolButtonRequestSystemInfo_clicked();
    void on_toolButtonRescanSystemInfo_clicked();
    void on_toolButtonSaveAs_clicked();

    void on_toolButtonRunRemoteApplication_clicked();
    void on_toolButtonSendCommand_clicked();


    void processClientResponseAdminConnectionResultPacket(const QString &computerName, bool result, const QString &message);

    void requestConnectionToClientTimeout();

    void clientMessageReceived(const QString &computerName, const QString &message);

    void clientResponseClientSummaryInfoPacketReceived(const QString &computerName, const QString &workgroupName, const QString &networkInfo, const QString &usersInfo, const QString &osInfo, bool usbsdEnabled, bool programesEnabled, const QString &admins, const QString &clientVersion);


    void clientDetailedInfoPacketReceived(const QString &computerName, const QString &clientInfo);
    void requestClientInfoTimeout();

    void clientResponseRemoteConsoleStatusPacketReceived(const QString &computerName, bool running, const QString &extraMessage);
    void remoteConsoleCMDResultFromClientPacketReceived(const QString &computerName, const QString &result);
    void requestRemoteConsoleTimeout();

    void userResponseRemoteAssistancePacketReceived(const QString &userName, const QString &computerName, bool accept);


private:
    bool verifyPrivilege();
    bool temporarilyAllowed();

    void runProgrameAsAdmin(const QString &exeFilePath, const QString &parameters = "", bool show = true);


private:
    Ui::SystemManagementWidgetClass ui;

    QString m_adminName;
    ControlCenterPacketsParser *controlCenterPacketsParser;
    QString m_computerName;
    QString m_users;
    QHostAddress m_peerIPAddress;
    QString m_peerMACAddress;
    bool m_usbsdEnabled;
    bool m_programesEnabled;
    QStringList m_administrators;
    QMenu *administratorsManagementMenu;


    bool localComputer;

    QStringList adminProcesses;

    bool clientResponseAdminConnectionResultPacketReceived;

    bool remoteConsoleRunning;

    //    QTimer *requestConnectionToClientTimer;

    //    ClientInfo *clientInfo;

    QString m_winDirPath;




};

} //namespace HEHUI

#endif // SYSTEMMANAGEMENTWIDGET_H