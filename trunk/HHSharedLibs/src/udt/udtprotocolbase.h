/*
 * abstractudtsocket.h
 *
 *  Created on: 2010-12-15
 *      Author: hehui
 */

#ifndef UDTPROCOTOLBASESOCKET_H_
#define UDTPROCOTOLBASESOCKET_H_

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

#ifndef MIN_THREAD_COUNT
#define MIN_THREAD_COUNT 10
#endif

#ifndef MAX_DATA_BLOCK_SIZE
#define MAX_DATA_BLOCK_SIZE 1024000
#endif



#include "mysharedlib_global.h"
#include "./udt/src/udt.h"
#include "./udt/src/ccc.h"

#include <iostream>
using namespace std;



namespace HEHUI {


class MYSHAREDLIB_API UDTProtocolBase :public QObject{
    Q_OBJECT
public:


    struct SocketOptions{
        SocketOptions(){
#ifdef WIN32
            UDT_MSS = 1052;
#else
            UDT_MSS = 1500;
#endif

            UDT_SNDSYN = false; //UDT Default:true
            UDT_RCVSYN = false; //UDT Default:true
            UDT_CC = 0;
            UDT_FC = 25600;

            UDT_SNDBUF = 10240000; //10M
            UDT_RCVBUF = 10240000;
            UDP_SNDBUF = 1024000; //1M
            UDP_RCVBUF = 1024000;

            UDT_LINGER.l_linger = 180;
            UDT_RENDEZVOUS = false;

            UDT_SNDTIMEO = 1000; //UDT Default:-1
            UDT_RCVTIMEO = 1000; //UDT Default:-1

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

    UDTProtocolBase(bool stream = true, const SocketOptions *options = 0, QObject *parent = 0);
    virtual ~UDTProtocolBase();


    static const UDTSOCKET INVALID_UDT_SOCK;


    //    void setSocketOption(UDT::SOCKOPT optname, const char* optval, int optlen);
    void setSocketOptions(const SocketOptions *options);
    SocketOptions getSocketOptions() const;

    bool isStreamMode(){return m_stream;}
    UDTSOCKET getServerSocket(){return serverSocket;}

    bool getAddressInfoFromSocket(UDTSOCKET socket, QString *address, quint16 *port, bool getPeerInfo = true);

signals:
    void connected(const QHostAddress &address, quint16 port);
    void connected(UDTSOCKET socket);
    void disconnected(const QHostAddress &address, quint16 port);
    void disconnected(UDTSOCKET socket);


public slots:
    //Start the server to listen,  implement the virtual function startWaitingForIO()
    UDTSOCKET listen(quint16 port = 0, const QHostAddress &localAddress= QHostAddress::Any);

    //Call this function after server is listening
    virtual void startWaitingForIO(int msecWaitForIOTimeout = 1000);



    //Connect to peer
    UDTSOCKET connectToHost(const QHostAddress &address, quint16 port, bool sync = false);
    //Close peer socket
    void closeSocket(UDTSOCKET socket);

    //Send data
    bool sendUDTStreamData(UDTSOCKET socket, const QByteArray *byteArray);
    bool sendUDTMessageData(UDTSOCKET socket, const QByteArray *byteArray, int ttl = -1, bool inorder = true);

    //Close the server
    void closeUDTProtocol();

private slots:

    void readDataFromSocket(UDTSOCKET socket);
    void writeDataToSocket(UDTSOCKET socket);

//    QByteArray processStreamDataBeforeSent(const QByteArray *data);
    void processStreamDataAfterReceived(UDTSOCKET socket, QByteArray *byteArray);

    virtual void streamDataReceived(UDTSOCKET socket, const QByteArray &data) = 0;
    virtual void messageDataReceived(UDTSOCKET socket, const QByteArray &data) = 0;

    //@Unused
    void waitForNewConnection(int msecSleep = 0);
    void waitForIO(int msecTimeout = 0);



private:
    struct CachedDataInfo;
    void recycleCachedDataInfo(CachedDataInfo* info);
    CachedDataInfo * getCachedDataInfo();

    UDT::ERRORINFO getLastErrorInfo(){return UDT::getlasterror();}

protected:
    //@Unused
    void waitForReading(int msecTimeout = -1);
    void waitForWriting(int msecTimeout = -1);
    UDTSOCKET acceptNewConnection();


private:
    int epollID;
    UDTSOCKET serverSocket;

    bool m_listening;
    bool m_stream;
    QString m_serverAddress;
    quint16 m_serverPort;


    SocketOptions m_socketOptions;


    ///////////////
//    class CachedDataInfo : public QObject{
//        CachedDataInfo(int blockSize = 0, QByteArray *data = 0, QObject *parent = 0)
//            :QObject(parent)
//        {
//            this->m_blockSize = blockSize;
//            if(!data){
//                data = new QByteArray();
//            }
//        }
//        ~CachedDataInfo(){
//            m_blockSize = 0;
//            m_data->clear();
//            delete m_data;
//        }
//        int blockSize(){return m_blockSize;}
//        void setBlockSize(int blockSize){m_blockSize = blockSize;}
//        QByteArray *data(){return m_data;}
//    private:
//        int m_blockSize;
//        QByteArray *m_data;
//    };
    //////////////



    struct CachedDataInfo{
        CachedDataInfo(){
            blockSize = 0;
            data = new QByteArray();
            data->resize(0);
        }
        qint32 blockSize;
        QByteArray *data;
    };
    QHash<UDTSOCKET, CachedDataInfo*> m_cachedDataInfo;
    QHash<UDTSOCKET, QByteArray*> m_cachedDataInfoHash;


    QList<CachedDataInfo*> m_unusedCachedDataInfo;



};


}

#endif /* ABSTRACTUDTSOCKET_H_ */
