#ifndef CONTROLCENTER_H
#define CONTROLCENTER_H

#include <QWidget>
#include <QMainWindow>
#include <QLabel>
#include <QProgressBar>
#include <QSqlQueryModel>
#include <QSortFilterProxyModel>
#include <QProcess>

#include "ui_controlcenter.h"
#include "networkmanager/controlcenterpacketsparser.h"
#include "networkmanager/resourcesmanagerinstance.h"

#include"clientinfomodel/clientinfomodel.h"

#include "../../sharedms/clientinfo.h"
#include "../../sharedms/rtp.h"



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
    void filter();
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

    void startNetwork();

    void serverFound(const QString &serverAddress, quint16 serverUDTListeningPort, quint16 serverTCPListeningPort, const QString &serverName, const QString &version, int serverInstanceID);

    void updateOrSaveClientInfo(const QString &computerName, const QString &workgroupName, const QString &networkInfo, const QString &usersInfo, const QString &osInfo, bool usbsdEnabled, bool programesEnabled, const QString &admins, bool isJoinedToDomain,const QString &clientVersion);
    
    void processClientOnlineStatusChangedPacket(int socketID, const QString &clientName, bool online);

    void peerConnected(const QHostAddress &peerAddress, quint16 peerPort);
    void signalConnectToPeerTimeout(const QHostAddress &peerAddress, quint16 peerPort);
    void peerDisconnected(const QHostAddress &peerAddress, quint16 peerPort, bool normalClose);
    void peerDisconnected(int socketID);

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


    //QWidget *m_progressWidget;
    //QLayout* hlayout;
    //QLabel *label;
    //QProgressBar *progressBar;


    ResourcesManagerInstance *resourcesManager;
    ControlCenterPacketsParser *controlCenterPacketsParser;


    QString m_adminName;

    QString m_administrators;

    SystemManagementWidget *localSystemManagementWidget;

    QString localComputerName;

    QProcess *vncProcess;
    
    QMenu *searchClientsMenu;
    QMenu *updatePasswordMenu;

    QHash<QString/*Computer Name*/, ClientInfo *> clientInfoHash;
    ClientInfoModel *clientInfoModel;
    //QSortFilterProxyModel *proxyModel;
    ClientInfoSortFilterProxyModel *proxyModel;
    

    bool m_networkReady;

    UDPServer *m_udpServer;
    quint16 m_localUDPListeningPort;

    RTP *m_rtp;
    quint16 m_localRTPListeningPort;
    UDTSOCKET m_socketConnectedToServer;


//    QHash<int/*Socket ID*/, QHostAddress/*IP*/> clientSocketsHash;

    
};

} //namespace HEHUI

#endif // CONTROLCENTER_H
