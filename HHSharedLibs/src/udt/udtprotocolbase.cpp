/*
 * abstractudtsocket.cpp
 *
 *  Created on: 2010-12-15
 *      Author: hehui
 */


#include "udtprotocolbase.h"

#include <QCoreApplication>
#include <QtConcurrentRun>
#include <QDebug>

//#ifndef Q_OS_WIN32
//   #include <unistd.h>
//   #include <cstdlib>
//   #include <cstring>
//   #include <netdb.h>
//#else
//   #include <windows.h>
//   #include <winsock2.h>
//   #include <ws2tcpip.h>
//    //#ifdef LEGACY_WIN32
//     //   #include <wspiapi.h>
//    //#endif
//#endif



namespace HEHUI {


const UDTSOCKET UDTProtocolBase::INVALID_UDT_SOCK = UDT::INVALID_SOCK;

UDTProtocolBase::UDTProtocolBase(bool stream, const SocketOptions *options, QObject *parent)
    :QObject(parent), m_stream(stream)
{

    // use this function to initialize the UDT library
    UDT::startup();


    epollID = 0;
    serverSocket = UDT::INVALID_SOCK;

    m_listening = false;
    m_serverAddress = "";
    m_serverPort = 0;

    if(options){
        m_socketOptions = *options;
    }

    //IMPORTANT For Multi-thread
    if(QThreadPool::globalInstance()->maxThreadCount() < MIN_THREAD_COUNT){
        QThreadPool::globalInstance()->setMaxThreadCount(MIN_THREAD_COUNT);
    }



}

UDTProtocolBase::~UDTProtocolBase() {

    //if(m_listening){
    closeUDTProtocol();
    //}

    //UDT::epoll_release(epollID);

    // use this function to release the UDT library
    //UDT::cleanup();

}

//void AbstractUDTSocket::setSocketOption(UDT::SOCKOPT optname, const char* optval, int optlen){

//}

void UDTProtocolBase::setSocketOptions(const SocketOptions *options){

    this->m_socketOptions = *options;
}

UDTProtocolBase::SocketOptions UDTProtocolBase::getSocketOptions() const{

    return m_socketOptions;

}

UDTSOCKET UDTProtocolBase::listen(quint16 port, const QHostAddress &localAddress){
    qDebug()<<"--UDTProtocolBase::listen(...) "<<localAddress.toString()<<":"<<port;

    if(m_listening){
        qCritical()<<"Server is already listenning!";
        return UDT::INVALID_SOCK;
    }

    addrinfo hints;
    addrinfo* localAddressInfo;
    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = m_stream?SOCK_STREAM:SOCK_DGRAM;

    if (0 != getaddrinfo(localAddress.toString().toStdString().c_str(), QString::number(port).toStdString().c_str(), &hints, &localAddressInfo))
        //if (0 != getaddrinfo(NULL, QString::number(port).toStdString().c_str(), &hints, &res))
    {
        cout << "illegal port number or port is busy.\n" << endl;
        freeaddrinfo(localAddressInfo);
        return UDT::INVALID_SOCK;
    }

    serverSocket = UDT::socket(localAddressInfo->ai_family, localAddressInfo->ai_socktype, localAddressInfo->ai_protocol);

    // UDT Options
    //TODO: setup UDT_MSS
    //UDT::setsockopt(serverSocket, 0, UDT_MSS, &(m_socketOptions.UDT_MSS), sizeof(int));
    UDT::setsockopt(serverSocket, 0, UDT_SNDSYN, &(m_socketOptions.UDT_SNDSYN), sizeof(bool));
    UDT::setsockopt(serverSocket, 0, UDT_RCVSYN, &(m_socketOptions.UDT_RCVSYN), sizeof(bool));
    if(m_socketOptions.UDT_CC){
        //UDT::setsockopt(serv, 0, UDT_CC, new CCCFactory<CUDPBlast>, sizeof(CCCFactory<CUDPBlast>));
        //UDT::setsockopt(serv, 0, UDT_CC, &(m_socketOptions.UDT_CC), sizeof(CCC));
    }
    UDT::setsockopt(serverSocket, 0, UDT_FC, &(m_socketOptions.UDT_FC), sizeof(int));
    UDT::setsockopt(serverSocket, 0, UDT_SNDBUF, &(m_socketOptions.UDT_SNDBUF), sizeof(int));
    UDT::setsockopt(serverSocket, 0, UDT_RCVBUF, &(m_socketOptions.UDT_RCVBUF), sizeof(int));
    UDT::setsockopt(serverSocket, 0, UDP_SNDBUF, &(m_socketOptions.UDP_SNDBUF), sizeof(int));
    UDT::setsockopt(serverSocket, 0, UDP_RCVBUF, &(m_socketOptions.UDP_RCVBUF), sizeof(int));
    UDT::setsockopt(serverSocket, 0, UDT_LINGER, &(m_socketOptions.UDT_LINGER), sizeof(linger));
    UDT::setsockopt(serverSocket, 0, UDT_RENDEZVOUS, &(m_socketOptions.UDT_RENDEZVOUS), sizeof(bool));
    UDT::setsockopt(serverSocket, 0, UDT_SNDTIMEO, &(m_socketOptions.UDT_SNDTIMEO), sizeof(int));
    UDT::setsockopt(serverSocket, 0, UDT_RCVTIMEO, &(m_socketOptions.UDT_RCVTIMEO), sizeof(int));
    UDT::setsockopt(serverSocket, 0, UDT_REUSEADDR, &(m_socketOptions.UDT_REUSEADDR), sizeof(bool));
    UDT::setsockopt(serverSocket, 0, UDT_MAXBW, &(m_socketOptions.UDT_MAXBW), sizeof(int64_t));


    qDebug()<<"m_socketOptions.UDT_MSS:"<<m_socketOptions.UDT_MSS;
    qDebug()<<"m_socketOptions.UDT_REUSEADDR:"<<m_socketOptions.UDT_REUSEADDR;
    qDebug()<<"m_socketOptions.UDT_SNDSYN:"<<m_socketOptions.UDT_SNDSYN;
    qDebug()<<"m_socketOptions.UDT_RCVSYN:"<<m_socketOptions.UDT_RCVSYN;
    qDebug()<<"m_socketOptions.UDT_FC:"<<m_socketOptions.UDT_FC;
    qDebug()<<"m_socketOptions.UDT_SNDBUF:"<<m_socketOptions.UDT_SNDBUF;
    qDebug()<<"m_socketOptions.UDT_RCVBUF:"<<m_socketOptions.UDT_RCVBUF;
    qDebug()<<"m_socketOptions.UDP_SNDBUF:"<<m_socketOptions.UDP_SNDBUF;
    qDebug()<<"m_socketOptions.UDP_RCVBUF:"<<m_socketOptions.UDP_RCVBUF;
    qDebug()<<"m_socketOptions.UDT_LINGER:"<<m_socketOptions.UDT_LINGER.l_linger;
    qDebug()<<"m_socketOptions.UDT_RENDEZVOUS:"<<m_socketOptions.UDT_RENDEZVOUS;
    qDebug()<<"m_socketOptions.UDT_SNDTIMEO:"<<m_socketOptions.UDT_SNDTIMEO;
    qDebug()<<"m_socketOptions.UDT_RCVTIMEO:"<<m_socketOptions.UDT_RCVTIMEO;
    qDebug()<<"m_socketOptions.UDT_REUSEADDR:"<<m_socketOptions.UDT_REUSEADDR;
    qDebug()<<"m_socketOptions.UDT_MAXBW:"<<m_socketOptions.UDT_MAXBW;



    if (UDT::ERROR == UDT::bind(serverSocket, localAddressInfo->ai_addr, localAddressInfo->ai_addrlen))
    {
        qCritical()<<"ERROR! Unable to listen! Failed to bind! "<<UDT::getlasterror().getErrorMessage();
        //cout << "bind: " << UDT::getlasterror().getErrorMessage() << endl;
        freeaddrinfo(localAddressInfo);

        //TODO:Close the socket
        //UDT::close(serverSocket);
        serverSocket = UDT::INVALID_SOCK;
        return UDT::INVALID_SOCK;
    }
    freeaddrinfo(localAddressInfo);


    if (UDT::ERROR == UDT::listen(serverSocket, 10))
    {
        cout << "listen: " << UDT::getlasterror().getErrorMessage() << endl;

        //TODO:Close the socket
        //UDT::close(serverSocket);
        serverSocket = UDT::INVALID_SOCK;
        return UDT::INVALID_SOCK;
    }

    epollID = UDT::epoll_create();
    //UDT::epoll_add_usock(epollID, serverSocket);


    //    QtConcurrent::run(this, &UDTProtocolBase::waitForNewConnection, 0);
    //    QtConcurrent::run(this, &UDTProtocolBase::waitForIO, msecWaitForIOTimeout);
    //    QtConcurrent::run(this, &UDTProtocolBase::waitForReading, msecWaitForIOTimeout);
    //    QtConcurrent::run(this, &UDTProtocolBase::waitForWriting, msecWaitForIOTimeout);


    m_listening = true;

    getAddressInfoFromSocket(serverSocket, &m_serverAddress, &m_serverPort, false);
    qDebug()<<QString("Server is ready on %1:%2").arg(m_serverAddress).arg(m_serverPort);


    //Call startWaitingForIO() to wait for IO


    return serverSocket;


}

//
void UDTProtocolBase::startWaitingForIO(int msecWaitForIOTimeout){
    qDebug()<<"--UDTProtocolBase::startWaitingForIO(...) "<<"msecWaitForIOTimeout:"<<msecWaitForIOTimeout;

//    QtConcurrent::run(this, &UDTProtocolBase::waitForNewConnection, msecWaitForIOTimeout);
//    QtConcurrent::run(this, &UDTProtocolBase::waitForIO, msecWaitForIOTimeout);

    QtConcurrent::run(this, &UDTProtocolBase::waitForReading, msecWaitForIOTimeout);
    QtConcurrent::run(this, &UDTProtocolBase::waitForWriting, msecWaitForIOTimeout);


}

void UDTProtocolBase::closeUDTProtocol(){
    qDebug()<<"--UDTProtocolBase::closeUDTProtocol()";


    m_listening = false;
    m_serverAddress = "";
    m_serverPort = 0;

    UDT::epoll_release(epollID);
    epollID = 0;

    qDebug()<<"-------------2";
    //UDT::close(serverSocket);

    // use this function to release the UDT library
    UDT::cleanup();
    qDebug()<<"-------------3";

    foreach (QByteArray *info, m_cachedDataInfoHash) {
        recycleCachedData(info);
    }
    m_cachedDataInfoHash.clear();

    foreach (QByteArray *info, m_unusedCached) {
        delete info;
    }
    m_unusedCached.clear();

    qDebug()<<"-------------4";




}

UDTSOCKET UDTProtocolBase::connectToHost(const QHostAddress &address, quint16 port, bool sync){
    qDebug()<<"--UDTProtocolBase::connectToHost(...)" <<address.toString()<<":"<<port<<" sync:"<<sync;

    if(address.isNull() || address == QHostAddress::Any){
        qCritical()<<"ERROR! Invalid Peer Address!";
        return UDT::INVALID_SOCK;
    }
    if(!port){
        qCritical()<<"ERROR! Invalid Peer Port!";
        return UDT::INVALID_SOCK;
    }

    if(!epollID){
        listen();
        //qCritical()<<"ERROR! EPOLL Not Initialized!";
        //return false;
    }
    Q_ASSERT_X(epollID, "epollID", "ERROR! EPOLL Not Initialized!");


    // use this function to initialize the UDT library
    //UDT::startup();

    struct addrinfo hints, *local, *peer;
    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = m_stream?SOCK_STREAM:SOCK_DGRAM;
    //hints.ai_socktype = SOCK_DGRAM;

    if (0 != getaddrinfo(address.toString().toStdString().c_str(), QString::number(port).toStdString().c_str(), &hints, &peer))
    {
        //cout << "incorrect server/peer address. " << argv[1] << ":" << argv[2] << endl;
        qWarning()<< "incorrect server/peer address. " << address.toString() << ":" << port;
        freeaddrinfo(peer);
        return UDT::INVALID_SOCK;
    }


    if (0 != getaddrinfo(m_serverAddress.toStdString().c_str(), QString::number(m_serverPort).toStdString().c_str(), &hints, &local))
    {
        cout << "incorrect network address.\n" << endl;
        freeaddrinfo(local);
        return UDT::INVALID_SOCK;
    }


    UDTSOCKET client = UDT::socket(local->ai_family, local->ai_socktype, local->ai_protocol);


    //non-blocking sending
    //UDT::setsockopt(client, 0, UDT_SNDSYN, new bool(false), sizeof(bool));
    //UDT::setsockopt(client, 0, UDT_SNDBUF, new int(10240000), sizeof(int));
    //UDT::setsockopt(client, 0, UDT_SNDBUF, new int(1024000000), sizeof(int));
//    if(!sync){
//        //non-blocking sending
//        UDT::setsockopt(client, 0, UDT_SNDSYN, &sync, sizeof(bool));
//        //non-blocking receiving
//        UDT::setsockopt(client, 0, UDT_RCVSYN, &sync, sizeof(bool));
//    }

    // for rendezvous connection, enable the code below
    //UDT::setsockopt(client, 0, UDT_RENDEZVOUS, new bool(true), sizeof(bool));
    //UDT::setsockopt(client, 0, UDT_REUSEADDR, new bool(true), sizeof(bool));


    if (UDT::ERROR == UDT::bind(client, local->ai_addr, local->ai_addrlen))
    {
        qCritical()<<"ERROR! Unable to connect to peer! Failed to bind! "<<QString(UDT::getlasterror().getErrorMessage()).toLocal8Bit();
        //cout << "bind: " << UDT::getlasterror().getErrorMessage() << endl;
        freeaddrinfo(local);

        //TODO:Close the socket
        UDT::close(client);
        return UDT::INVALID_SOCK;
    }
    freeaddrinfo(local);


    UDT::epoll_add_usock(epollID, client);


    if (UDT::ERROR == UDT::connect(client, peer->ai_addr, peer->ai_addrlen))
    {
        qCritical()<<"ERROR! Failed to connect! "<<UDT::getlasterror().getErrorMessage();
        //cout << "connect: " << UDT::getlasterror().getErrorMessage() << endl;
        freeaddrinfo(peer);

        //TODO:Close the socket
        UDT::close(client);
        return UDT::INVALID_SOCK;
    }
    freeaddrinfo(peer);


    //UDT::close(client);
    // use this function to release the UDT library
    //UDT::cleanup();

    return client;

}


void UDTProtocolBase::closeSocket(UDTSOCKET socket){
    qDebug()<<"--UDTProtocolBase::closeSocket(...) "<<"socket:"<<socket;

    UDT::close(socket);

    QByteArray *data = m_cachedDataInfoHash.take(socket);
    recycleCachedData(data);

}

void UDTProtocolBase::waitForNewConnection(int msecSleep){
    qDebug()<<"--UDTProtocolBase::waitForNewConnection(...)";

    Q_ASSERT_X(epollID, "epollID", "ERROR! EPOLL Not Initialized!");
    if(!epollID){
        qCritical()<<"ERROR! EPOLL Not Initialized!";
        return;
    }

    sockaddr_storage clientaddr;
    int addrlen = sizeof(clientaddr);
    UDTSOCKET peer;

    while(m_listening){

        if (UDT::INVALID_SOCK == (peer = UDT::accept(serverSocket, (sockaddr*)&clientaddr, &addrlen)))
        {
            //qWarning()<<"ERROR! Invalid UDTSOCKET! "<< UDT::getlasterror().getErrorMessage();
            //cout << "accept: " << UDT::getlasterror().getErrorMessage() << endl;

            int errorCode = UDT::getlasterror().getErrorCode();
            if(errorCode == UDT::ERRORINFO::EINVSOCK || errorCode == UDT::ERRORINFO::ENOLISTEN){
                return;
            }
            continue;
        }

        char peerAddress[NI_MAXHOST];
        char peerPort[NI_MAXSERV];
        getnameinfo((sockaddr *)&clientaddr, addrlen, peerAddress, sizeof(peerAddress), peerPort, sizeof(peerPort), NI_NUMERICHOST|NI_NUMERICSERV);
        cout << "New Connection: " << peerAddress << ":" << peerPort << " UDTSOCKET:"<< peer << endl;


        if(UDT::epoll_add_usock(epollID, peer, NULL) < 0){
            qWarning()<<"ERROR! epoll_add_usock Failed! "<< UDT::getlasterror().getErrorMessage();
            //fprintf(stderr, "epoll_add_usock error\n");
            UDT::close(peer);
            continue;
        }

        QCoreApplication::processEvents();



    }

    //    sockaddr_storage clientaddr;
    //    int addrlen = sizeof(clientaddr);
    //    //UDTSOCKET recver;

    //    while (true)
    //    {
    //       if (UDT::INVALID_SOCK == (peer = UDT::accept(serv, (sockaddr*)&clientaddr, &addrlen)))
    //       {
    //          cout << "accept: " << UDT::getlasterror().getErrorMessage() << endl;
    //          continue;
    //       }

    //       char clienthost[NI_MAXHOST];
    //       char clientservice[NI_MAXSERV];
    //       getnameinfo((sockaddr *)&clientaddr, addrlen, clienthost, sizeof(clienthost), clientservice, sizeof(clientservice), NI_NUMERICHOST|NI_NUMERICSERV);
    //       cout << "new connection: " << clienthost << ":" << clientservice << endl;

    //       #ifndef Q_OS_WIN32
    //          pthread_t rcvthread;
    //          pthread_create(&rcvthread, NULL, recvdata, new UDTSOCKET(recver));
    //          pthread_detach(rcvthread);
    //       #else
    //          CreateThread(NULL, 0, recvdata, new UDTSOCKET(peer), 0, NULL);
    //          //CreateThread(NULL, 0, recvdata, &recver, 0, NULL);
    //       #endif
    //    }



}

bool UDTProtocolBase::sendUDTStreamData(UDTSOCKET socket, const QByteArray *byteArray){
    qDebug()<<"--UDTProtocolBase::sendUDTStreamData(...) " <<"socket:"<<socket;


    Q_ASSERT_X(epollID, "epollID", "ERROR! EPOLL Not Initialized!");
    if(!epollID){
        qCritical()<<"ERROR! EPOLL Not Initialized!";
        return false;
    }

    if(byteArray->size() > MAX_DATA_BLOCK_SIZE){
        qCritical()<<QString("ERROR! Data is too large! Max allowed size:%1, current data size:%2").arg(MAX_DATA_BLOCK_SIZE).arg(byteArray->size());
        return false;
    }


    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_7);
    out << *byteArray ;

    int size = block.size();
    const char* data = block.constData();

    //    int size = byteArray.size();
    //    const char* data = byteArray.constData();

    int ssize = 0;
    int ss;
    while (ssize < size)
    {
        if (UDT::ERROR == (ss = UDT::send(socket, data + ssize, size - ssize, 0)))
        {
            cout << "send:" << UDT::getlasterror().getErrorMessage() << endl;
            break;
        }

        ssize += ss;
        //QCoreApplication::processEvents();
    }

    if (ssize < size){
        qCritical()<<"Data sent failed!"<<"   Data Size:"<<size<<"   Sent size:"<<ssize;
        return false;
    }


    return true;

}

bool UDTProtocolBase::sendUDTMessageData(UDTSOCKET socket, const QByteArray *byteArray, int ttl, bool inorder){
    qDebug()<<"--UDTProtocolBase::sendUDTMessageData(...) " <<" socket:"<<socket;


    Q_ASSERT_X(epollID, "epollID", "ERROR! EPOLL Not Initialized!");
    if(!epollID){
        qCritical()<<"ERROR! EPOLL Not Initialized!";
        return false;
    }


    int size = byteArray->size();
    if(size > MAX_DATA_BLOCK_SIZE){
        qCritical()<<QString("ERROR! Data is too large! Max allowed size:%1, current data size:%2").arg(MAX_DATA_BLOCK_SIZE).arg(size);
        return false;
    }
    const char* data = byteArray->constData();

    int ss = UDT::sendmsg(socket, data, size, ttl, inorder);

    if (UDT::ERROR == ss || 0 == ss)
    {
        qCritical()<<"ERROR! Failed to send data! "<<UDT::getlasterror().getErrorMessage();
        //cout << "sendmsg:" << UDT::getlasterror().getErrorMessage() << endl;
        return false;
    }

    return true;


}

void UDTProtocolBase::waitForIO(int msecTimeout){
    qDebug()<<"--UDTProtocolBase::waitForIO(...)";

    set<UDTSOCKET> readfds, writefds;
    int count = 0;

    while(m_listening){
        count = UDT::epoll_wait(epollID, &readfds, &writefds, msecTimeout);
        if(count > 0){
            printf("epoll returned %d sockets ready to IO | %d in read set, %d in write set\n", count, readfds.size(), writefds.size());

            for( std::set<UDTSOCKET>::const_iterator it = readfds.begin(); it != readfds.end(); ++it){
                //TODO:Process
                //QtConcurrent::run(this, &UDTProtocolBase::readDataFromSocket, *it);
                readDataFromSocket(*it);
            }
            //readfds.clear();

            for( std::set<UDTSOCKET>::const_iterator it = writefds.begin(); it != writefds.end(); ++it){
                //TODO:Process
                //QtConcurrent::run(this, &UDTProtocolBase::writeDataToSocket, *it);
                writeDataToSocket(*it);
            }
            //writefds.clear();
        }


    }





}

void UDTProtocolBase::waitForReading(int msecTimeout){
    qDebug()<<"--UDTProtocolBase::waitForReading(...)";

    set<UDTSOCKET> readfds;
    int count = 0;

    while(m_listening){

        acceptNewConnection();

        count = UDT::epoll_wait(epollID, &readfds, NULL, msecTimeout);
        if(count > 0){
            printf("epoll returned %d sockets ready to IO | %d in read set\n", count, readfds.size());

            for( std::set<UDTSOCKET>::const_iterator it = readfds.begin(); it != readfds.end(); ++it){
                //TODO:Process
                qDebug()<<"--0--QThread::currentThreadId():"<<QThread::currentThreadId();

                //QtConcurrent::run(this, &UDTProtocolBase::readDataFromSocket, *it);
                readDataFromSocket(*it);
            }
            //readfds.clear();

        }

        //acceptNewConnection();
    }

}

void UDTProtocolBase::waitForWriting(int msecTimeout){
    qDebug()<<"--UDTProtocolBase::waitForWriting(...)";

    set<UDTSOCKET> writefds;
    int count = 0;

    while(m_listening){

        acceptNewConnection();

        count = UDT::epoll_wait(epollID, NULL, &writefds, msecTimeout);
        if(count > 0){
            //printf("epoll returned %d sockets ready to IO | %d in write set\n", count, writefds.size());

            for( std::set<UDTSOCKET>::const_iterator it = writefds.begin(); it != writefds.end(); ++it){
                //TODO:Process
                //QtConcurrent::run(this, &UDTProtocolBase::writeDataToSocket, *it);
                writeDataToSocket(*it);
            }
            //writefds.clear();
        }

        //acceptNewConnection();
    }

}

UDTSOCKET UDTProtocolBase::acceptNewConnection(){
    //qDebug()<<"--UDTProtocolBase::acceptNewConnection()";

    sockaddr_storage clientaddr;
    int addrlen = sizeof(clientaddr);
    UDTSOCKET peer;

    if (UDT::INVALID_SOCK == (peer = UDT::accept(serverSocket, (sockaddr*)&clientaddr, &addrlen)))
    {
        //qDebug()<<"ERROR! Invalid UDTSOCKET! "<< UDT::getlasterror().getErrorMessage();
        //cout << "accept: " << UDT::getlasterror().getErrorMessage() << endl;

//        int errorCode = UDT::getlasterror().getErrorCode();
//        QString errorMessage = QString(UDT::getlasterror().getErrorMessage());
//        switch(errorCode){
//        case UDT::ERRORINFO::EINVSOCK:
//            //serverSocket is an invalid UDT socket.
//            qCritical()<<"ERROR! "<<errorMessage;
//            break;
//        case UDT::ERRORINFO::ENOLISTEN:
//            //serverSocket is not in the listening state.
//            qCritical()<<"ERROR! "<<errorMessage;
//            break;
//        case UDT::ERRORINFO::ERDVNOSERV:
//            //serverSocket is set up to support rendezvous connection.
//            qDebug()<<errorMessage;
//            break;
//        case UDT::ERRORINFO::EASYNCRCV:
//            //serverSocket is non-blocking (UDT_RCVSYN = false) but there is no connection available.
//            qDebug()<<errorMessage;
//            break;
//        default:
//            qDebug()<<errorMessage;
//            break;
//        }

        return UDT::INVALID_SOCK;

    }

    char peerAddress[NI_MAXHOST];
    char peerPort[NI_MAXSERV];
    getnameinfo((sockaddr *)&clientaddr, addrlen, peerAddress, sizeof(peerAddress), peerPort, sizeof(peerPort), NI_NUMERICHOST|NI_NUMERICSERV);
    cout << "New Connection: " << peerAddress << ":" << peerPort << " UDTSOCKET:"<< peer << endl;


    if(UDT::epoll_add_usock(epollID, peer, NULL) < 0){
        qWarning()<<"ERROR! epoll_add_usock Failed! "<< UDT::getlasterror().getErrorMessage();
        //fprintf(stderr, "epoll_add_usock error\n");
        UDT::close(peer);
        return UDT::INVALID_SOCK;
    }

    return peer;

}

void UDTProtocolBase::readDataFromSocket(UDTSOCKET socket){
    //qDebug()<<"--UDTProtocolBase::readDataFromSocket(..) "<<"socket:"<<socket;


    int size = MAX_DATA_BLOCK_SIZE + 4;

    QByteArray byteArray;
    byteArray.resize(size);
    //TODO:Size
    char* data = byteArray.data();
    //data = new char[size];


    int receivedSize = 0;

    if(m_stream){
        //while (true)
        //{
        int totalReceivedSize = 0;
        while (totalReceivedSize < size)
        {
            if (UDT::ERROR == (receivedSize = UDT::recv(socket, data + totalReceivedSize, size - totalReceivedSize, 0)))
            {
                qCritical()<<"ERROR! Failed to receive data! "<< UDT::getlasterror().getErrorMessage();
                //cout << "recv:" << UDT::getlasterror().getErrorMessage() << endl;
                break;
            }

            totalReceivedSize += receivedSize;
        }

        if (0 == totalReceivedSize){
            return;
        }

        byteArray.resize(totalReceivedSize);
        processStreamDataAfterReceived(socket, &byteArray);

    }else{
        if (UDT::ERROR == (receivedSize = UDT::recvmsg(socket, data, size)))
        {
            cout << "recvmsg:" << UDT::getlasterror().getErrorMessage() << endl;
        }

        byteArray.resize(receivedSize);

        messageDataReceived(socket, QByteArray(data));

    }




    //    sockaddr* addr = NULL;
    //    int size = 0;
    //    addr = (sockaddr*)new sockaddr_in;
    //    size = sizeof(sockaddr_in);
    //    UDT::getsockname(socket, addr, &size);
    //    char sharedport[NI_MAXSERV];
    //    getnameinfo(addr, size, NULL, 0, sharedport, sizeof(sharedport), NI_NUMERICSERV);


    //    sockaddr_storage clientaddr;
    //    int addrlen = sizeof(clientaddr);
    //    UDT::getsockname(socket, (sockaddr*)&clientaddr, &addrlen);
    //    char clienthost[NI_MAXHOST];
    //    char clientservice[NI_MAXSERV];
    //    getnameinfo((sockaddr*)&clientaddr, addrlen, clienthost, sizeof(clienthost), clientservice, sizeof(clientservice), NI_NUMERICHOST|NI_NUMERICSERV);
    //    cout << "Data Received From: " << clienthost << ":" << clientservice << endl;



    //    sockaddr clientaddr;
    //    int addrlen = sizeof(clientaddr);
    //    //UDT::getsockname(socket, &clientaddr, &addrlen);
    //    UDT::getpeername(socket, &clientaddr, &addrlen);
    //    char clienthost[NI_MAXHOST];
    //    char clientservice[NI_MAXSERV];
    //    getnameinfo(&clientaddr, addrlen, clienthost, sizeof(clienthost), clientservice, sizeof(clientservice), NI_NUMERICHOST|NI_NUMERICSERV);
    //    cout << "Data Received From: " << clienthost << ":" << clientservice << endl;

    //    QString str = socketsHash.key(socket);
    //    qDebug()<<"str:"<<str;



    //    delete [] data;



}

void UDTProtocolBase::writeDataToSocket(UDTSOCKET socket){
    //qDebug()<<"--UDTProtocolBase::writeDataToSocket() "<<"socket:"<<socket;

    return;
    //UDT::epoll_remove_usock(epollID, socket);

    UDTSTATUS status = UDT::getsockstate(socket);
    qDebug()<<"socket:"<<socket<<" status:"<<status;

    switch(status){
    case INIT: //1
    {
        UDT::epoll_add_usock(epollID, socket);
        qDebug()<<"INIT";
    }
        break;
    case OPENED: //2
    {
        UDT::epoll_add_usock(epollID, socket);
        qDebug()<<"OPENED";
    }
        break;
    case LISTENING: //3
    {
        UDT::epoll_add_usock(epollID, socket);
        qDebug()<<"LISTENING";

    }
        break;
    case CONNECTING: //4
    {
        UDT::epoll_add_usock(epollID, socket);
        qDebug()<<"CONNECTING";
    }
        break;
    case CONNECTED: //5
    {
        UDT::epoll_add_usock(epollID, socket);
        qDebug()<<"CONNECTED";

    }
        break;
    case BROKEN: //6
    {
        UDT::close(socket);

        //UDT::epoll_remove_usock(epollID, socket);
        qDebug()<<"BROKEN";
    }
        break;
    case CLOSING: //7
    {
        UDT::epoll_add_usock(epollID, socket);
        qDebug()<<"CLOSING";
    }
        break;
    case CLOSED: //8
    {
        UDT::close(socket);

        //UDT::epoll_remove_usock(epollID, socket);
        qDebug()<<"CLOSED";
    }
        break;
    case NONEXIST: //9
    {
        UDT::close(socket);

        //UDT::epoll_remove_usock(epollID, socket);
        qDebug()<<"NONEXIST";
    }
        break;
    default:
        break;


    }


}

void UDTProtocolBase::processStreamDataAfterReceived(UDTSOCKET socket, QByteArray *byteArray){
    qDebug()<<"--UDTProtocolBase::processStreamDataAfterReceived(...) "<<"socket:"<<socket<<" size:"<<byteArray->size();


    QByteArray *cachedData = m_cachedDataInfoHash.value(socket);
    if(cachedData){
        byteArray->prepend(*cachedData);
        cachedData->clear();
    }


    QDataStream in(byteArray, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_7);
    QIODevice *dev = in.device();

    QByteArray temp;
    qint64 offset = 0;

    forever{
        in >> temp;
        if(temp.isEmpty()){
            if(!cachedData){
                cachedData = new QByteArray();
                m_cachedDataInfoHash.insert(socket, cachedData);
            }

            dev->seek(offset);
            cachedData->append(dev->readAll());
            break;

        }else{
            streamDataReceived(socket, temp);
            offset  = dev->pos();
        }

    }


}

void UDTProtocolBase::recycleCachedData(QByteArray* data){
    if(!data){
        return;
    }
    data->clear();

}

QByteArray * UDTProtocolBase::getCachedData(){

    if(m_unusedCached.isEmpty()){
        return new QByteArray();
    }
    return m_unusedCached.takeFirst();

}

bool UDTProtocolBase::getAddressInfoFromSocket(UDTSOCKET socket, QString *address, quint16 *port, bool getPeerInfo){


    sockaddr clientaddr;
    int addrlen = sizeof(clientaddr);
    int error;
    if(getPeerInfo){
        error = UDT::getpeername(socket, &clientaddr, &addrlen);
    }else{
        error = UDT::getsockname(socket, &clientaddr, &addrlen);
    }
    if(UDT::ERROR == error){
        qCritical()<<QString("ERROR! Failed to get %1 address info! %2").arg(getPeerInfo?"peer":"local").arg(UDT::getlasterror().getErrorMessage());
        return false;
    }

    char clienthost[NI_MAXHOST];
    char clientservice[NI_MAXSERV];
    getnameinfo(&clientaddr, addrlen, clienthost, sizeof(clienthost), clientservice, sizeof(clientservice), NI_NUMERICHOST|NI_NUMERICSERV);
    //cout << "Address Info: " << clienthost << ":" << clientservice << endl;

    if(address){
        *address = QString::fromLocal8Bit(clienthost);
    }
    if(port){
        *port = QString::fromLocal8Bit(clientservice).toUInt();
    }

    return true;

}
















}



