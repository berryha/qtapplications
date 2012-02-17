/*
 ****************************************************************************
 * server.h
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



#ifndef SERVER_H
#define SERVER_H

#include <QObject>


#include "networkmanager/servernetworkmanager.h"
#include "usersmanager/userinfo.h"

#include "packetmanager/serverpacketsparser.h"

#include "serverlib.h"

#include "HHSharedCore/hdatabaseutility.h"
//#include "HHSharedNetwork/hnetworkmanagerbase.h"



namespace HEHUI {


class SERVERLIB_API Server : public QObject
{
    Q_OBJECT
public:
    Server(QObject *parent = 0);
    ~Server();

    void start();
    void stop();


signals:


private slots:
    bool startMainService();
//    void sendServerOnlinePacket();
    void saveClientLog(const QString &computerName, const QString &users, const QString &log, const QString &clientAddress);


    void peerConnected(const QHostAddress &peerAddress, quint16 peerPort);
    void signalConnectToPeerTimeout(const QHostAddress &peerAddress, quint16 peerPort);
    void peerDisconnected(const QHostAddress &peerAddress, quint16 peerPort, bool normalClose);


private:
    bool openDatabase(bool reopen = false);




private:

    PacketHandlerBase *m_packetHandler;
    ServerNetworkManager *networkManager;
    ServerPacketsParser *serverPacketsParser;

    bool mainServiceStarted;

    DatabaseUtility *databaseUtility;
    QSqlQuery *query;

//    QTimer *sendServerOnlinePacketTimer;

    QHash<QString, UserInfo *> clientInfoHash;

    int onlineAdminsCount;

    QStringList recordsInDatabase;

    QStringList logs;


    RUDPSocket *rudpSocket;

};

} //namespace HEHUI

#endif // SERVER_H
