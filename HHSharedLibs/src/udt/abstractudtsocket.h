/*
 * abstractudtsocket.h
 *
 *  Created on: 2010-12-15
 *      Author: hehui
 */

#ifndef ABSTRACTUDTSOCKET_H_
#define ABSTRACTUDTSOCKET_H_

#include <QObject>
#include <QHostAddress>

#ifdef Q_OS_WIN32
    //#include <windows.h>
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #ifdef LEGACY_WIN32
        #include <wspiapi.h>
    #endif
#else
    #include <unistd.h>
    #include <cstdlib>
    #include <cstring>
    #include <netdb.h>
#endif

#include "mysharedlib_global.h"
#include "./udt/src/udt.h"

#include <iostream>
using namespace std;

#ifndef Q_OS_WIN32
    void* recvdata(void*);
#else
    DWORD WINAPI recvdata(LPVOID);
#endif



namespace HEHUI {


class MYSHAREDLIB_API AbstractUDTSocket :public QObject{
    Q_OBJECT
public:
    AbstractUDTSocket(QObject *parent = 0);
    virtual ~AbstractUDTSocket();

    virtual void dataReceived(const QHostAddress &address, quint16 port, char *data, bool stream) = 0;


signals:
    void connected(const QHostAddress &address, quint16 port);
    void dataReceived(const QByteArray &data, bool stream);



public slots:
    bool listen(quint16 port = 0, const QHostAddress &localAddress= QHostAddress::Any);
    void close();

    bool connectToHost(const QHostAddress &address, quint16 port, bool sync = false);

    bool sendUDTData(const QHostAddress &targetAddress, quint16 port, const QByteArray &byteArray, bool stream = true);

private slots:
    void waitForNewConnection(int msec = 0, bool * timedOut = 0);
    void waitForIO(int msecTimeout = 0);

    void readDataFromSocket(UDTSOCKET socket);
    void writeDataToSocket(UDTSOCKET socket);





private:
    int epollID;
    UDTSOCKET serv;

    bool m_listening;
    QHostAddress m_serverAddress;
    quint16 m_serverPort;








};

}

#endif /* ABSTRACTUDTSOCKET_H_ */
