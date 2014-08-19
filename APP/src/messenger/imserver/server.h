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



#ifndef IMSERVER_H
#define IMSERVER_H

#include <QObject>



#include "usersmanager/userinfo.h"

#include "packetmanager/serverpacketsparser.h"

#include "serverlib.h"

#include "../sharedim/resourcesmanagerinstance.h"

#include "HHSharedCore/hdatabaseutility.h"
//#include "HHSharedNetwork/hnetworkmanagerbase.h"



namespace HEHUI {


class SERVERLIB_API IMServer : public QObject
{
    Q_OBJECT
public:
    IMServer(QObject *parent = 0);
    ~IMServer();

    void start();
    void stop();


signals:


private slots:
    bool startMainService();
//    void sendServerOnlinePacket();
    void saveClientLog(const QString &computerName, const QString &users, const QString &log, const QString &clientAddress);

    void saveFileLog(const QString &sender, const QString &receiver, const QString &fileName, const QString &md5Hex, quint64 size);

    void peerConnected(const QHostAddress &peerAddress, quint16 peerPort);
    void signalConnectToPeerTimeout(const QHostAddress &peerAddress, quint16 peerPort);
    void peerDisconnected(const QHostAddress &peerAddress, quint16 peerPort, bool normalClose);
    void peerDisconnected(int socketID);


private:
    bool openDatabase(bool reopen = false);




private:

    PacketHandlerBase *m_packetHandler;
    ResourcesManagerInstance *resourcesManager;
    ServerPacketsParser *serverPacketsParser;

    UDPServer *m_udpServer;
    RTP *m_rtp;


    bool mainServiceStarted;

    DatabaseUtility *databaseUtility;
    QSqlQuery *query;


    QHash<QString, UserInfo *> userInfoHash;

    int onlineAdminsCount;


//    QStringList logs;



};

} //namespace HEHUI

#endif // IMSERVER_H
