#ifndef SYSTEMMANAGEMENTWIDGET_H
#define SYSTEMMANAGEMENTWIDGET_H

#include <QWidget>
#include <QCloseEvent>
#include <QMenu>
#include <QSqlQueryModel>

#include "ui_systemmanagementwidget.h"
#include "../../sharedms/rtp.h"

#include "networkmanager/controlcenterpacketsparser.h"




namespace HEHUI {


class SystemManagementWidget : public QWidget
{
    Q_OBJECT

public:
    SystemManagementWidget(RTP *rtp, ControlCenterPacketsParser *parser, const QString &adminName, const QString &computerName, const QString &users, const QString &peerIPAddress, const QString &peerMACAddress, bool usbsdEnabled = false, bool programesEnabled = false, const QString &admins = "", bool isJoinedToDomain = false, QWidget *parent = 0);
    ~SystemManagementWidget();


    QHostAddress peerIPAddress() const{return m_peerIPAddress;}

    
    
signals:
    void requestRemoteAssistance();

public slots:
    void setRTP(RTP *rtp);
//    void setUDTProtocol(UDTProtocol *udtProtocol);
//    void setTCPServer(TCPServer *tcpServer);
    void setControlCenterPacketsParser(ControlCenterPacketsParser *parser);

protected:
    void closeEvent(QCloseEvent *event);
//    void dragEnterEvent(QDragEnterEvent *event);
//    void dragMoveEvent(QDragMoveEvent *event);
//    void dropEvent(QDropEvent *event);


private slots:
    void on_toolButtonVerify_clicked();
    void on_pushButtonUSBSD_clicked();
    void on_pushButtonPrograms_clicked();
    void on_pushButtonShowAdmin_clicked();
    void on_pushButtonRemoteAssistance_clicked();

    void on_actionAddAdmin_triggered();
    void on_actionDeleteAdmin_triggered();

    void getNewComputerName();
    void on_pushButtonRenameComputer_clicked();
    void on_pushButtonDomain_clicked();

    void on_pushButtonMMC_clicked();
    void on_pushButtonCMD_clicked();
    void on_pushButtonRegedit_clicked();
    void on_pushButtonOtherEXE_clicked();

    void on_toolButtonQuerySystemInfo_clicked();
    void on_toolButtonRequestSystemInfo_clicked();
    void on_toolButtonRescanSystemInfo_clicked();
    void on_toolButtonSaveAs_clicked();

    void on_toolButtonRunRemoteApplication_clicked();
    void on_toolButtonSendCommand_clicked();

    void processClientOnlineStatusChangedPacket(int socketID, const QString &computerName, bool online);
    void processClientResponseAdminConnectionResultPacket(int socketID, const QString &computerName, bool result, const QString &message);

    void requestConnectionToClientTimeout();

    void clientMessageReceived(const QString &computerName, const QString &message, quint8 clientMessageType);

    void clientResponseClientSummaryInfoPacketReceived(const QString &computerName, const QString &workgroupName, const QString &networkInfo, const QString &usersInfo, const QString &osInfo, bool usbsdEnabled, bool programesEnabled, const QString &admins, bool isJoinedToDomain, const QString &clientVersion);


    void clientDetailedInfoPacketReceived(const QString &computerName, const QString &clientInfo);
    void requestClientInfoTimeout();

    void clientResponseRemoteConsoleStatusPacketReceived(const QString &computerName, bool running, const QString &extraMessage, quint8 messageType);
    void remoteConsoleCMDResultFromClientPacketReceived(const QString &computerName, const QString &result);
    void requestRemoteConsoleTimeout();

    void userResponseRemoteAssistancePacketReceived(const QString &userName, const QString &computerName, bool accept);


    void peerDisconnected(int socketID);
    void peerDisconnected(bool normalClose);


private:
    bool verifyPrivilege();
    bool temporarilyAllowed();

    void runProgrameAsAdmin(const QString &exeFilePath, const QString &parameters = "", bool show = true);

    void resetSystemInfo();

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
    bool m_isJoinedToDomain;

    bool localComputer;

    QString m_winDirPath;
    //QString m_joinInfo;

    static QMap<QString/*Short Name*/, QString/*Department*/>departments;

    QMenu *administratorsManagementMenu;
    QStringList m_administrators;


    QStringList adminProcesses;

    bool clientResponseAdminConnectionResultPacketReceived;
    bool remoteConsoleRunning;


    QSqlQueryModel *queryModel;

    RTP *m_rtp;
    int m_peerSocket;

    bool m_aboutToCloseSocket;

    //UDPServer *m_udpServer;
//    UDTProtocol *m_udtProtocol;

//    TCPServer *m_tcpServer;
//    bool m_usingTCP;

//    UDTProtocolForFileTransmission *m_udtProtocolForFileTransmission;
//    UDTSOCKET m_peerFileTransmissionSocket;

//    FileManager *m_fileManager;
//    QList<int/*File TX Request ID*/> fileTXRequestList;
//    QList<QByteArray/*File MD5*/> filesList;

    FileManagement *m_fileManagementWidget;




};

} //namespace HEHUI

#endif // SYSTEMMANAGEMENTWIDGET_H
