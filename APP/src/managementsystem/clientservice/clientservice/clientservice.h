
#ifndef CLIENTSERVICE_H
#define CLIENTSERVICE_H



#include <QObject>
#include <QSettings>
#include <QTimer>

#include "../../sharedms/global_shared.h"
#include "packetmanager/clientpacketsparser.h"
#include "../clientresourcesmanager.h"
#include "process.h"
#include "systeminfo.h"




#include "HHSharedService/hservice.h"
#include "HHSharedCore/hdatabaseutility.h"

//#include "HHSharedUDT/hudtprotocolforfiletransmission.h"

#ifdef Q_OS_WIN32
    #include "HHSharedWindowsManagement/hwindowsmanagement.h"
    #ifndef SITOY_USERS_DB_CONNECTION_NAME
    #define SITOY_USERS_DB_CONNECTION_NAME "200.200.200.2/mis/users"
    #endif
#endif



namespace HEHUI {


class ClientService : public Service
{
    Q_OBJECT
public:
    ClientService(int argc, char **argv, const QString &serviceName = "ClientService", const QString &description = "Client service for management system.");
    ~ClientService();

    bool setDeskWallpaper(const QString &wallpaperPath);

signals:


private slots:
    bool startMainService();

    void serverLookedUp(const QHostInfo &host);

    void serverFound(const QString &serverAddress, quint16 serverUDTListeningPort, quint16 serverTCPListeningPort, const QString &serverName, const QString &version, int serverInstanceID);

    void processServerRequestClientInfoPacket(const QString &groupName, const QString &computerName, const QString &userName/*, const QString &address*/);

    void processClientDetailedInfoRequestedPacket(const QString &computerName, bool rescan, int socketID);
    void scanFinished(bool ok, const QString &message);

    void processSetupUSBSDPacket(const QString &computerName, const QString &users, bool enable, bool temporarilyAllowed, const QString &adminName, const QString &adminAddress, quint16 adminPort);
    void processSetupProgramesPacket(const QString &computerName, const QString &users, bool enable, bool temporarilyAllowed, const QString &adminName, const QString &adminAddress, quint16 adminPort);
    void processShowAdminPacket(const QString &computerName, const QString &users, bool show);
    void processModifyAdminGroupUserPacket(const QString &computerName, const QString &userName, bool addToAdminGroup, const QString &adminName, const QString &adminAddress, quint16 adminPort);
    void processRenameComputerPacketReceived(const QString &newComputerName, const QString &adminName, const QString &adminAddress, quint16 adminPort);
    void processJoinOrUnjoinDomainPacketReceived(const QString &adminName, bool join, const QString &domainOrWorkgroupName, const QString &adminAddress, quint16 adminPort);

    void processAdminRequestConnectionToClientPacket(int adminSocketID, const QString &computerName, const QString &users);
    void processAdminSearchClientPacket(const QString &adminAddress, quint16 adminPort, const QString &computerName, const QString &userName, const QString &workgroup, const QString &macAddress, const QString &ipAddress, const QString &osVersion, const QString &adminName);
    
    
    void processServerAnnouncementPacket(const QString &workgroupName, const QString &computerName, quint32 announcementID, const QString &announcement, const QString &adminName, bool mustRead);    
    void processAdminRequestRemoteAssistancePacket(const QString &computerName, const QString &adminName, const QString &adminAddress, quint16 adminPort);
    void processAdminRequestUpdateMSUserPasswordPacket(const QString &workgroupName, const QString &adminName, const QString &adminAddress, quint16 adminPort);
    void processAdminRequestInformUserNewPasswordPacket(const QString &workgroupName, const QString &adminName, const QString &adminAddress, quint16 adminPort);

    void processAdminRequestRemoteConsolePacket(const QString &computerName, const QString &applicationPath, const QString &adminID, bool startProcess, const QString &adminAddress, quint16 adminPort);
    void processRemoteConsoleCMDFromServerPacket(const QString &computerName, const QString &command, const QString &adminAddress, quint16 adminPort);

    void consoleProcessStateChanged(bool running, const QString &message);
    void consoleProcessOutputRead(const QString &output);

    void processLocalUserOnlineStatusChanged(int socketID, const QString &userName, bool online);

    QStringList usersOnLocalComputer();
    void uploadClientSummaryInfo(int socketID);
    void uploadClientSummaryInfo(const QString &adminAddress, quint16 adminPort);

//    void uploadClientDetailedInfoToServer();

    void update();
    bool updateAdministratorPassword(const QString &newPassword);
    void setWinAdminPassword(const QString &userPassword);
    QString getWinAdminPassword() const;

    bool checkUsersAccount();

    bool enableUSBSD(bool temporary);
    void disableUSBSD();
    bool isUSBSDEnabled();
    void checkUSBSD();

    bool enableProgrames(bool temporary);
    void disableProgrames();
    bool isProgramesEnabled();
    void checkProgrames();


    QStringList administrators();
    void modifyAdminGroupUser(const QString &userName, bool addToAdminGroup);

    bool setupStartupWithSafeMode(bool startup);

    QString getServerLastUsed() const;
    void setServerLastUsed(const QString &serverAddress);

    void checkHasAnyServerBeenFound();

    void peerConnected(const QHostAddress &peerAddress, quint16 peerPort);
    void signalConnectToPeerTimeout(const QHostAddress &peerAddress, quint16 peerPort);
    void peerDisconnected(const QHostAddress &peerAddress, quint16 peerPort, bool normalClose);
    void peerDisconnected(int socketID);

///////////////////
    void fileSystemInfoRequested(int socketID, const QString &parentDirPath);
    //File TX
    void startFileManager();
    void processAdminRequestUploadFilePacket(int socketID, const QByteArray &fileMD5Sum, const QString &fileName, quint64 size, const QString &localFileSaveDir);
    void processAdminRequestDownloadFilePacket(int socketID, const QString &localBaseDir, const QString &fileName, const QString &remoteFileSaveDir);
    void processFileDataRequestPacket(int socketID, const QByteArray &fileMD5, int startPieceIndex, int endPieceIndex);
    void processFileDataReceivedPacket(int socketID, const QByteArray &fileMD5, int pieceIndex, const QByteArray &data, const QByteArray &sha1);
    void processFileTXStatusChangedPacket(int socketID, const QByteArray &fileMD5, quint8 status);
    void processFileTXErrorFromPeer(int socketID, const QByteArray &fileMD5, quint8 errorCode, const QString &errorString);

    void fileDataRead(int requestID, const QByteArray &fileMD5, int pieceIndex, const QByteArray &data, const QByteArray &dataSHA1SUM);
    void fileTXError(int requestID, const QByteArray &fileMD5, quint8 errorCode, const QString &errorString);
    void pieceVerified(const QByteArray &fileMD5, int pieceIndex, bool verified, int verificationProgress);


private:
    bool getLocalFilesInfo(const QString &parentDirPath, QByteArray *result, QString *errorMessage);


protected:
    void start();
    void stop();
    void pause();
    void resume();
    void processCommand(int code);



private:

    ClientResourcesManager *resourcesManager;
    ClientPacketsParser *clientPacketsParser;

    UDPServer *m_udpServer;
    RTP *m_rtp;

//    UDTProtocol *m_udtProtocol;
    int m_socketConnectedToServer;
    int m_socketConnectedToAdmin;
    int peerSocketThatRequiresDetailedInfo;


//    UDTProtocolForFileTransmission *m_udtProtocolForFileTransmission;

//    QHash<int /*UDT Socket ID*/, QString /*User Name*/> m_localUserSocketsHash;

    bool mainServiceStarted;

    DatabaseUtility *databaseUtility;

    QSettings *settings;


    QString m_localComputerName;
    bool m_isJoinedToDomain;
    QString m_joinInfo;


#if defined(Q_OS_WIN32)
    WindowsManagement *wm;


#endif


    Process *process;

    SystemInfo *systemInfo;

    QString m_adminAddress;
    quint16 m_adminPort;
    
    QHash<QString/*User Name*/, QString/*Password*/> userPasswordsHash;


    QTimer *lookForServerTimer;

    QHostAddress m_serverAddress;
    quint16 m_serverUDTListeningPort;

    QString m_serverName;
    int m_serverInstanceID;

    FileManager *m_fileManager;
    QHash<int/*File TX Request ID*/, int/*Socket ID*/> fileTXRequestHash;
    QMultiHash<int/*Socket ID*/, QByteArray/*File MD5*/> fileTXSocketHash;

    QStringList logs;


};

} //namespace HEHUI

#endif // CLIENTSERVICE_H
