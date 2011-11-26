/*
 ****************************************************************************
 * serverservice.h
 *
 * Created on: 2010-7-12
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
 * Last Modified on: 2010-9-3
 * Last Modified by: 贺辉
 ***************************************************************************
 */



#ifndef SERVERSERVICE_H
#define SERVERSERVICE_H

#include <QTimer>

#include "../../sharedms/global_shared.h"
#include "../../sharedms/clientinfo.h"

#include "packetmanager/serverpacketsparser.h"
#include "../resourcesmanagerinstance.h"

#include "HHSharedCore/hdatabaseutility.h"
#include "HHSharedService/hservice.h"



namespace HEHUI {


class ServerService : public Service
{
    Q_OBJECT
public:
    ServerService(int argc, char **argv, const QString &serviceName = "ServerService", const QString &description = "Server service for management system.");
    ~ServerService();



signals:


private slots:
    bool startMainService();
    //void saveClientLog(const QString &computerName, const QString &users, const QString &log, const QString &clientAddress);
    void saveClientLog(const QString &computerName, const QString &users, const QString &clientAddress, quint8 logType, const QString &log, const QString &clientTime);

    void sendServerOnlinePacket();
    void updateOrSaveClientSummaryInfo(const QString &computerName, const QString &workgroupName, const QString &networkInfo, const QString &usersInfo, const QString &osInfo, bool usbsdEnabled, bool programesEnabled, const QString &admins, const QString &clientVersion);
    bool updateOrSaveClientInfoToDatabase(ClientInfo *info);
    void updateOrSaveAllClientsInfoToDatabase();
    void clientDetailedInfoPacketReceived(const QString &computerName, const QString &clientInfo);

    void getRecordsInDatabase();

    void processHeartbeatPacket(const QString &clientAddress, const QString &computerName);
    void processClientOnlineStatusChangedPacket(const QString &clientUDPListeningAddress, quint16 clientUDPListeningPort, const QString &clientName, bool online, bool isAdmin);

    void peerConnected(const QHostAddress &peerAddress, quint16 peerPort);
    void signalConnectToPeerTimeout(const QHostAddress &peerAddress, quint16 peerPort);
    void peerDisconnected(const QHostAddress &peerAddress, quint16 peerPort, bool normalClose);
    void peerDisconnected(int socketID);


private:
    bool openDatabase(bool reopen = false);
    bool isRecordExistInDB(const QString &computerName);

protected:
    void start();
    void stop();
    void pause();
    void resume();
    void processCommand(int code);



private:

    ResourcesManagerInstance *resourcesManager;
    ServerPacketsParser *serverPacketsParser;

    UDPServer *m_udpServer;
    UDTProtocol *m_udtProtocol;
    //UDTSOCKET m_socketConnectedToAdmin;

    bool mainServiceStarted;

    DatabaseUtility *databaseUtility;
    QSqlQuery *query;

    QTimer *sendServerOnlinePacketTimer;

    QHash<QString, ClientInfo *> clientInfoHash;

    int onlineAdminsCount;

    QStringList recordsInDatabase;

    QStringList logs;


};

} //namespace HEHUI

#endif // SERVERSERVICE_H
