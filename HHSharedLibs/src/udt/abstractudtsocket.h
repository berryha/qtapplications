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


signals:


public slots:
        bool listen(const QHostAddress &localAddress= QHostAddress::Any, quint16 port = 0);
        void close();

        bool sendUDTData(const QHostAddress &targetAddress, quint16 port, const QByteArray &byteArray);

private slots:
        void waitForNewConnection(int msec = 0, bool * timedOut = 0);





private:
        UDTSOCKET serv;

        bool m_listening;
        QHostAddress m_serverAddress;
        quint16 m_serverPort;






};

}

#endif /* ABSTRACTUDTSOCKET_H_ */
