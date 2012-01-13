#ifndef SYSTEMMANAGEMENTWIDGET_H
#define SYSTEMMANAGEMENTWIDGET_H

#include <QtGui/QWidget>
#include <QCloseEvent>
#include <QMenu>
#include <QSqlQueryModel>

#include "ui_systemmanagementwidget.h"

#include "networkmanager/controlcenterpacketsparser.h"




namespace HEHUI {


class SystemManagementWidget : public QWidget
{
    Q_OBJECT

public:
    SystemManagementWidget(UDTProtocol *udtProtocol, ControlCenterPacketsParser *parser, const QString &adminName, const QString &computerName, const QString &users, const QString &peerIPAddress, const QString &peerMACAddress, bool usbsdEnabled = false, bool programesEnabled = false, const QString &admins = "", QWidget *parent = 0);
    ~SystemManagementWidget();


    QHostAddress peerIPAddress() const{return m_peerIPAddress;}

    
    
signals:
    void requestRemoteAssistance();

public slots:
    void setUDTProtocol(UDTProtocol *udtProtocol);

    void setControlCenterPacketsParser(ControlCenterPacketsParser *parser);

    void peerDisconnected(bool normalClose);


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


    void processClientResponseAdminConnectionResultPacket(int socketID, const QString &computerName, bool result, const QString &message);

    void requestConnectionToClientTimeout();

    void clientMessageReceived(const QString &computerName, const QString &message);

    void clientResponseClientSummaryInfoPacketReceived(const QString &computerName, const QString &workgroupName, const QString &networkInfo, const QString &usersInfo, const QString &osInfo, bool usbsdEnabled, bool programesEnabled, const QString &admins, const QString &clientVersion);


    void clientDetailedInfoPacketReceived(const QString &computerName, const QString &clientInfo);
    void requestClientInfoTimeout();

    void clientResponseRemoteConsoleStatusPacketReceived(const QString &computerName, bool running, const QString &extraMessage);
    void remoteConsoleCMDResultFromClientPacketReceived(const QString &computerName, const QString &result);
    void requestRemoteConsoleTimeout();

    void userResponseRemoteAssistancePacketReceived(const QString &userName, const QString &computerName, bool accept);

///////////////////
    void requestFileSystemInfo(const QString &parentDirPath);
    void fileSystemInfoReceived(int socketID, const QString &parentDirPath, const QByteArray &fileSystemInfoData);
    void requestUploadFilesToRemote(const QStringList &localFiles, const QString &remoteDir);
    void requestDownloadFileFromRemote(const QStringList &remoteFiles, const QString &localDir);

    //File TX
    void startFileManager();
    void processPeerRequestUploadFilePacket(int socketID, const QByteArray &fileMD5Sum, const QString &fileName, quint64 size, const QString &remoteFileSaveDir);
    void processPeerRequestDownloadFilePacket(int socketID, const QString &filePath);

    void fileDownloadRequestAccepted(int socketID, const QString &remoteFilePath, const QByteArray &fileMD5Sum, quint64 size);
    void fileDownloadRequestDenied(int socketID, const QString &remoteFilePath, const QString &message);
    void fileUploadRequestResponsed(int socketID, const QByteArray &fileMD5Sum, bool accepted, const QString &message);

    void processFileDataRequestPacket(int socketID, const QByteArray &fileMD5, int startPieceIndex, int endPieceIndex);
    void processFileDataReceivedPacket(int socketID, const QByteArray &fileMD5, int pieceIndex, const QByteArray &data, const QByteArray &sha1);
    void processFileTXStatusChangedPacket(int socketID, const QByteArray &fileMD5, quint8 status);
    void processFileTXErrorFromPeer(int socketID, const QByteArray &fileMD5, quint8 errorCode, const QString &errorMessage);

    void fileDataRead(int requestID, const QByteArray &fileMD5, int pieceIndex, const QByteArray &data, const QByteArray &dataSHA1SUM);
    void fileTXError(int requestID, const QByteArray &fileMD5, quint8 errorCode, const QString &errorString);
    void pieceVerified(const QByteArray &fileMD5, int pieceIndex, bool verified, int verificationProgress);

    void filesDropped(const QStringList &localFiles);
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
    QStringList m_administrators;
    QMenu *administratorsManagementMenu;


    bool localComputer;

    QStringList adminProcesses;

    bool clientResponseAdminConnectionResultPacketReceived;

    bool remoteConsoleRunning;


    QString m_winDirPath;

    QSqlQueryModel *queryModel;

    //UDPServer *m_udpServer;
    UDTProtocol *m_udtProtocol;
    UDTSOCKET m_peerSocket;

    UDTProtocolForFileTransmission *m_udtProtocolForFileTransmission;
    UDTSOCKET m_peerFileTransmissionSocket;

    FileManager *m_fileManager;
    QList<int/*File TX Request ID*/> fileTXRequestList;
    QList<QByteArray/*File MD5*/> filesList;

    FileManagement *m_fileManagementWidget;


};

} //namespace HEHUI

#endif // SYSTEMMANAGEMENTWIDGET_H
