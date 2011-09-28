
#ifndef CLIENTSERVICE_H
#define CLIENTSERVICE_H



#include <QObject>
#include <QSettings>

#include "../../sharedms/global_shared.h"
#include "../../sharedms/networkmanager.h"
#include "packetmanager/clientpacketsparser.h"
#include "process.h"
#include "systeminfo.h"



//#include "../../../shared/service/service/service.h"
//#include "../../../shared/network/networkmanagerbase.h"

#include "HHSharedService/hservice.h"
#include "HHSharedCore/hdatabaseutility.h"
//#include "HHSharedNetwork/hnetworkmanagerbase.h"

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



signals:


private slots:
    bool startMainService();

    void serverFound(const QString &serverAddress, quint16 serverRUDPListeningPort, const QString &serverName, const QString &version);
    void processServerRequestClientInfoPacket(const QString &groupName, const QString &computerName, const QString &userName/*, const QString &address*/);

    void processClientDetailedInfoRequestedPacket(const QString &computerName, bool rescan, const QString &peerAddress, quint16 peerPort);
    void scanFinished(bool ok, const QString &message);

    void processSetupUSBSDPacket(const QString &computerName, const QString &users, bool enable, bool temporarilyAllowed, const QString &adminName, const QString &adminAddress, quint16 adminPort);
    void processSetupProgramesPacket(const QString &computerName, const QString &users, bool enable, bool temporarilyAllowed, const QString &adminName, const QString &adminAddress, quint16 adminPort);
    void processShowAdminPacket(const QString &computerName, const QString &users, bool show);
    void processModifyAdminGroupUserPacket(const QString &computerName, const QString &userName, bool addToAdminGroup, const QString &adminName, const QString &adminAddress, quint16 adminPort);
    void processAdminRequestConnectionToClientPacket(const QString &adminAddress, quint16 adminPort, const QString &computerName, const QString &users);
    void processAdminSearchClientPacket(const QString &adminAddress, quint16 adminPort, const QString &computerName, const QString &userName, const QString &workgroup, const QString &macAddress, const QString &ipAddress, const QString &osVersion, const QString &adminName);
    
    
    void processServerAnnouncementPacket(const QString &workgroupName, const QString &computerName, quint32 announcementID, const QString &announcement, const QString &adminName, bool mustRead);    
    void processAdminRequestRemoteAssistancePacket(const QString &computerName, const QString &adminName, const QString &adminAddress, quint16 adminPort);
    void processAdminRequestUpdateMSUserPasswordPacket(const QString &workgroupName, const QString &adminName, const QString &adminAddress, quint16 adminPort);
    void processAdminRequestInformUserNewPasswordPacket(const QString &workgroupName, const QString &adminName, const QString &adminAddress, quint16 adminPort);

    void processAdminRequestRemoteConsolePacket(const QString &computerName, const QString &applicationPath, const QString &adminID, bool startProcess, const QString &adminAddress, quint16 adminPort);
    void processRemoteConsoleCMDFromServerPacket(const QString &computerName, const QString &command, const QString &adminAddress, quint16 adminPort);

    void consoleProcessStateChanged(bool running, const QString &message);
    void consoleProcessOutputRead(const QString &output);



    void uploadClientSummaryInfo(const QString &targetAddress = "", quint16 targetPort = 0);
    void uploadClientDetailedInfoToServer();

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




protected:
    void start();
    void stop();
    void pause();
    void resume();
    void processCommand(int code);



private:

    PacketHandlerBase *m_packetHandler;
    ClientNetworkManager *networkManager;
    ClientPacketsParser *clientPacketsParser;

    bool mainServiceStarted;

    DatabaseUtility *databaseUtility;

    QSettings *settings;


#if defined(Q_OS_WIN32)
    WindowsManagement *wm;

#endif

    QString localComputerName;
    QString m_localWorkgroupName;


    Process *process;

    SystemInfo *systemInfo;

    QString m_adminAddress;
    quint16 m_adminPort;
    
    QHash<QString/*User Name*/, QString/*Password*/> userPasswordsHash;

    QString peerAddressThatRequiresDetailedInfo;
    quint16 peerPortThatRequiresDetailedInfo;

    RUDPSocket *rudpSocket;

    QTimer *lookForServerTimer;


};

} //namespace HEHUI

#endif // CLIENTSERVICE_H
