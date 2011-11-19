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
#include "./udt/src/ccc.h"

#include <iostream>
using namespace std;



namespace HEHUI {


class MYSHAREDLIB_API AbstractUDTSocket :public QObject{
    Q_OBJECT
public:

    struct SocketOptions{
        SocketOptions(){
#ifdef WIN32
            UDT_MSS = 1052;
#else
            UDT_MSS = 1500;
#endif

            UDT_SNDSYN = true;
            UDT_RCVSYN = true;
            UDT_CC = 0;
            UDT_FC = 25600;

            UDT_SNDBUF = 10240000; //10M
            UDT_RCVBUF = 10240000;
            UDP_SNDBUF = 1024000; //1M
            UDP_RCVBUF = 1024000;

            UDT_LINGER.l_linger = 180;
            UDT_RENDEZVOUS = false;

            UDT_SNDTIMEO = -1;
            UDT_RCVTIMEO = -1;

            UDT_REUSEADDR = true;
            UDT_MAXBW = -1;

            UDT_STATE = 0;
            UDT_EVENT = 0;

        }

        int UDT_MSS;
        bool UDT_SNDSYN;
        bool UDT_RCVSYN;
        CCC* UDT_CC;
        int UDT_FC;

        int UDT_SNDBUF;
        int UDT_RCVBUF;
        int UDP_SNDBUF;
        int UDP_RCVBUF;

        linger UDT_LINGER;
        bool UDT_RENDEZVOUS;

        int UDT_SNDTIMEO;
        int UDT_RCVTIMEO;

        bool UDT_REUSEADDR;
        int64_t UDT_MAXBW;

        int32_t UDT_STATE; //Read only!
        int32_t UDT_EVENT; //Read only!

    };

    AbstractUDTSocket(bool stream = true, const SocketOptions *options = 0, QObject *parent = 0);
    virtual ~AbstractUDTSocket();


//    void setSocketOption(UDT::SOCKOPT optname, const char* optval, int optlen);
    void setSocketOptions(const SocketOptions *options);
    SocketOptions getSocketOptions() const;

    bool isStreamMode(){return m_stream;}

signals:
    void connected(const QHostAddress &address, quint16 port);
    void disconnected(const QHostAddress &address, quint16 port);
    void disconnected(int udtSocketID);


public slots:
    bool listen(quint16 port = 0, const QHostAddress &localAddress= QHostAddress::Any);
    void close();

    bool connectToHost(const QHostAddress &address, quint16 port, bool sync = false);
    void disconnectFromHost(const QHostAddress &address, quint16 port);

    bool sendUDTStreamData(const QHostAddress &targetAddress, quint16 port, const QByteArray *byteArray);
    bool sendUDTMessageData(const QHostAddress &targetAddress, quint16 port, const QByteArray *byteArray, int ttl = -1, bool inorder = true);

private slots:
    void waitForNewConnection(int msec = 0, bool * timedOut = 0);
    void waitForIO(int msecTimeout = 0);

    void readDataFromSocket(UDTSOCKET socket);
    void writeDataToSocket(UDTSOCKET socket);

    QByteArray processStreamDataBeforeSent(const QByteArray &data);
    void processStreamDataAfterReceived(UDTSOCKET socket, const QByteArray &data);

    virtual void streamDataReceived(int udtSocketID, const QByteArray &data) = 0;
    virtual void messageDataReceived(int udtSocketID, const QByteArray &data) = 0;

private:
    struct CachedDataInfo;
    void recycleCachedDataInfo(CachedDataInfo* info);
    CachedDataInfo * getCachedDataInfo();

protected:
    void getAddressInfoFromSocket(UDTSOCKET socket, QString *address, quint16 *port, bool getPeerInfo = true);


private:
    int epollID;
    UDTSOCKET serverSocket;

    bool m_listening;
    bool m_stream;
    QString m_serverAddress;
    quint16 m_serverPort;

    QHash<QString/*IP:Port*/, UDTSOCKET> socketsHash;


    SocketOptions m_socketOptions;


    struct CachedDataInfo{
        CachedDataInfo(){
            blockSize = 0;
            data = 0;
        }
        int blockSize;
        QByteArray *data;

    };
    QHash<UDTSOCKET, CachedDataInfo*> m_cachedDataInfo;

    QList<CachedDataInfo*> m_unusedCachedDataInfo;




};

}

#endif /* ABSTRACTUDTSOCKET_H_ */
