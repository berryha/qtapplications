#ifndef CONTROLCENTER_H
#define CONTROLCENTER_H

#include <QtGui/QWidget>
#include <QtGui/QMainWindow>
#include <QLabel>
#include <QProgressBar>
#include <QSqlQueryModel>
#include <QSortFilterProxyModel>
#include <QProcess>

#include "ui_controlcenter.h"
#include "networkmanager/controlcenterpacketsparser.h"
#include "networkmanager/networkmanagerinstance.h"

#include"clientinfomodel/clientinfomodel.h"

#include "../../sharedms/clientinfo.h"


#include "systemmanagement/systemmanagementwidget.h"



#include "HHSharedCore/huser.h"
#include "HHSharedCore/hmysharedlib_global.h"
#include "HHSharedGUI/hprogressdlg.h"




namespace HEHUI {


class ControlCenter : public QMainWindow
{
    Q_OBJECT

public:
    ControlCenter(const QString &adminName, QWidget *parent = 0);
    ~ControlCenter();



    static bool isRunning(){return running;}

    void retranslateUi();

protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void languageChange();
    void closeEvent(QCloseEvent *e);

public slots:
    void slotQueryClient(const QString &computerName, const QString &userName, const QString &workgroup, const QString &macAddress, const QString &ipAddress, const QString &osVersion, const QString &usbsd, const QString &programes);


private slots:
    void slotInitTabWidget();
    void slotTabPageChanged();
    void slotNewTab();
    void slotcloseTab();

    void slotRemoteManagement();

    void slotQueryDatabase();
    void slotSearchNetwork();
    //    void slotUpdateButtonClicked();

    void slotShowClientInfo(const QModelIndex &index);
    void slotExportQueryResult();
    void slotPrintQueryResult();

    void slotRemoteDesktop();
    void slotVNC();
    void slotUpdateUserLogonPassword();
    void slotInformUserNewLogonPassword();
    void slotSendAnnouncement();
    


    void slotShowCustomContextMenu(const QPoint & pos);

    void networkReady();

    void serverFound(const QString &serverAddress, quint16 serverTCPListeningPort, const QString &serverName, const QString &version);

    void updateOrSaveClientInfo(const QString &computerName, const QString &workgroupName, const QString &networkInfo, const QString &usersInfo, const QString &osInfo, bool usbsdEnabled, bool programesEnabled, const QString &admins, const QString &clientVersion);
    

private:
    void updateActions();




private:
    QString computerName() const;
    QString userName() const;

    QString workgroup() const;
    QString usbsdEnabled();

    QString macAddress() const;
    QString ipAddress() const;

    QString osVersion() const;
    QString programesEnabled() const;



    void querySitoyClientInfo(const QString &queryString);


private:
    Ui::ControlCenterClass ui;

    static bool running;


    QString databaseConnectionName;
    QSqlQueryModel *queryModel;


    QWidget *m_progressWidget;
    QLayout* hlayout;
    QLabel *label;
    QProgressBar *progressBar;


    PacketHandlerBase *m_packetHandler;
    NetworkManagerInstance *networkManager;
    ControlCenterPacketsParser *controlCenterPacketsParser;

    bool m_networkReady;

    QString m_adminName;

    QString m_administrators;

    SystemManagementWidget *localSystemManagementWidget;

    QString localComputerName;

    QProcess *vncProcess;
    
    QMenu *searchClientsMenu;
    QMenu *updatePasswordMenu;

    QHash<QString, ClientInfo *> clientInfoHash;
    ClientInfoModel *clientInfoModel;
    QSortFilterProxyModel *proxyModel;
    
    quint16 localUDPListeningPort;
    

    
};

} //namespace HEHUI

#endif // CONTROLCENTER_H
