/*
 * abstractudtsocket.cpp
 *
 *  Created on: 2010-12-15
 *      Author: hehui
 */


#include "abstractudtsocket.h"

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

AbstractUDTSocket::AbstractUDTSocket(bool stream, const SocketOptions *options, QObject *parent)
    :QObject(parent), m_stream(stream)
{

    // use this function to initialize the UDT library
    UDT::startup();

    epollID = 0;
    serverSocket = UDT::INVALID_SOCK;

    m_listening = false;
    m_serverAddress = QHostAddress::Null;
    m_serverPort = 0;

    if(options){
        m_socketOptions = *options;
    }



}

AbstractUDTSocket::~AbstractUDTSocket() {

    if(m_listening){
        close();
    }

    //UDT::epoll_release(epollID);

    // use this function to release the UDT library
    UDT::cleanup();

}

//void AbstractUDTSocket::setSocketOption(UDT::SOCKOPT optname, const char* optval, int optlen){

//}

void AbstractUDTSocket::setSocketOptions(const SocketOptions *options){

    this->m_socketOptions = *options;
}

AbstractUDTSocket::SocketOptions AbstractUDTSocket::getSocketOptions() const{

    return m_socketOptions;

}

bool AbstractUDTSocket::listen(quint16 port, const QHostAddress &localAddress){

    if(m_listening){
        qCritical()<<"Server is already listenning!";
        return false;
    }

    addrinfo hints;
    addrinfo* res;
    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = m_stream?SOCK_STREAM:SOCK_DGRAM;
    //hints.ai_socktype = SOCK_DGRAM;

    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    //sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = inet_addr(localAddress.toString().toLocal8Bit().data());
    sin.sin_port = htons(port);

    hints.ai_addr = (sockaddr *)&sin;


    if (0 != getaddrinfo(NULL, QString::number(port).toStdString().c_str(), &hints, &res))
    {
       cout << "illegal port number or port is busy.\n" << endl;
       return 0;
    }

    serverSocket = UDT::socket(res->ai_family, res->ai_socktype, res->ai_protocol);


    // UDT Options
    UDT::setsockopt(serverSocket, 0, UDT_MSS, &(m_socketOptions.UDT_MSS), sizeof(int));
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


    if (UDT::ERROR == UDT::bind(serverSocket, res->ai_addr, res->ai_addrlen))
    {
       cout << "bind: " << UDT::getlasterror().getErrorMessage() << endl;
       return 0;
    }

    freeaddrinfo(res);


    cout << "server is ready at port: " << port << endl;

    if (UDT::ERROR == UDT::listen(serverSocket, 10))
    {
       cout << "listen: " << UDT::getlasterror().getErrorMessage() << endl;
       return 0;
    }

    epollID = UDT::epoll_create();
    UDT::epoll_add_usock(epollID, serverSocket);

    m_serverAddress = localAddress;
    m_serverPort = port;


    QtConcurrent::run(this, &AbstractUDTSocket::waitForNewConnection, 0, new bool(0));


    m_listening = true;

    return true;


}

void AbstractUDTSocket::close(){


    m_listening = false;
    m_serverAddress = QHostAddress::Null;
    m_serverPort = 0;


    foreach (UDTSOCKET socket, socketsHash.values()) {
        UDT::epoll_remove_usock(epollID, socket);
    }
    UDT::epoll_release(epollID);
    epollID = 0;

    socketsHash.clear();

    UDT::close(serverSocket);

    foreach (CachedDataInfo *info, m_cachedDataInfo) {
        recycleCachedDataInfo(info);
    }
    m_cachedDataInfo.clear();

    foreach (CachedDataInfo *info, m_unusedCachedDataInfo) {
        delete info->data;
        delete info;
    }
    m_unusedCachedDataInfo.clear();


}

bool AbstractUDTSocket::connectToHost(const QHostAddress &address, quint16 port, bool sync){

    if(address.isNull() || address == QHostAddress::Any){
        qCritical()<<"ERROR! Invalid Peer Address!";
        return false;
    }
    if(!port){
        qCritical()<<"ERROR! Invalid Peer Port!";
        return false;
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


    if (0 != getaddrinfo(NULL, QString::number(port).toStdString().c_str(), &hints, &local))
    {
        cout << "incorrect network address.\n" << endl;
        return 0;
    }

    UDTSOCKET client = UDT::socket(local->ai_family, local->ai_socktype, local->ai_protocol);

    freeaddrinfo(local);

    if (0 != getaddrinfo(address.toString().toStdString().c_str(), QString::number(port).toStdString().c_str(), &hints, &peer))
    {
        //cout << "incorrect server/peer address. " << argv[1] << ":" << argv[2] << endl;
        qWarning()<< "incorrect server/peer address. " << address.toString() << ":" << port;
        return 0;
    }

    //non-blocking sending
    //UDT::setsockopt(client, 0, UDT_SNDSYN, new bool(false), sizeof(bool));
    //UDT::setsockopt(client, 0, UDT_SNDBUF, new int(10240000), sizeof(int));
    UDT::setsockopt(client, 0, UDT_SNDBUF, new int(1024000000), sizeof(int));
    if(!sync){
        //non-blocking sending
        UDT::setsockopt(client, 0, UDT_SNDSYN, &sync, sizeof(bool));
        //non-blocking receiving
        UDT::setsockopt(client, 0, UDT_RCVSYN, &sync, sizeof(bool));
    }

    if (UDT::ERROR == UDT::bind(client, serverSocket))
    {
        qCritical()<<"ERROR! Failed to bind! "<<UDT::getlasterror().getErrorMessage();
       //cout << "bind: " << UDT::getlasterror().getErrorMessage() << endl;
       return 0;
    }


    if (UDT::ERROR == UDT::connect(client, peer->ai_addr, peer->ai_addrlen))
    {
        qCritical()<<"ERROR! Failed to connect! "<<UDT::getlasterror().getErrorMessage();
        //cout << "connect: " << UDT::getlasterror().getErrorMessage() << endl;
        return 0;
    }

    freeaddrinfo(peer);


    //UDT::close(client);
    // use this function to release the UDT library
    //UDT::cleanup();

    return true;

}

void AbstractUDTSocket::disconnectFromHost(const QHostAddress &address, quint16 port){


    if(address.isNull() || address == QHostAddress::Any){
        qCritical()<<"ERROR! Invalid Peer Address!";
        return;
    }
    if(!port){
        qCritical()<<"ERROR! Invalid Peer Port!";
        return;
    }

    Q_ASSERT_X(epollID, "epollID", "ERROR! EPOLL Not Initialized!");
    if(!epollID){
        qCritical()<<"ERROR! EPOLL Not Initialized!";
        return;
    }

    UDTSOCKET socket = socketsHash.take(address.toString()+":"+QString::number(port));
    if(UDT::INVALID_SOCK == socket){
        qCritical()<<"ERROR! Invalid UDTSOCKET!";
        return;
    }

    UDT::close(socket);



}

void AbstractUDTSocket::waitForNewConnection(int msec, bool * timedOut){

    Q_ASSERT_X(epollID, "epollID", "ERROR! EPOLL Not Initialized!");
    if(!epollID){
        qCritical()<<"ERROR! EPOLL Not Initialized!";
        return;
    }

    sockaddr_storage clientaddr;
    int addrlen = sizeof(clientaddr);
    UDTSOCKET peer;

    while(true){

        if (UDT::INVALID_SOCK == (peer = UDT::accept(serverSocket, (sockaddr*)&clientaddr, &addrlen)))
        {
            qWarning()<<"ERROR! Invalid UDTSOCKET! "<< UDT::getlasterror().getErrorMessage();
           //cout << "accept: " << UDT::getlasterror().getErrorMessage() << endl;
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
        socketsHash.insert(QString(peerAddress)+":"+QString(peerPort), peer);

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

bool AbstractUDTSocket::sendUDTStreamData(const QHostAddress &targetAddress, quint16 port, const QByteArray *byteArray){



    if(targetAddress.isNull() || targetAddress == QHostAddress::Any){
        qCritical()<<"ERROR! Invalid Peer Address!";
        return false;
    }
    if(!port){
        qCritical()<<"ERROR! Invalid Peer Port!";
        return false;
    }

    Q_ASSERT_X(epollID, "epollID", "ERROR! EPOLL Not Initialized!");
    if(!epollID){
        qCritical()<<"ERROR! EPOLL Not Initialized!";
        return false;
    }

    UDTSOCKET socket = socketsHash.value(targetAddress.toString()+":"+QString::number(port));
    if(UDT::INVALID_SOCK == socket){
        qCritical()<<"ERROR! Invalid UDTSOCKET!";
        return false;
    }

    QByteArray block = processStreamDataBeforeSent(*byteArray);
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

bool AbstractUDTSocket::sendUDTMessageData(const QHostAddress &targetAddress, quint16 port, const QByteArray *byteArray, int ttl, bool inorder){

    if(targetAddress.isNull() || targetAddress == QHostAddress::Any){
        qCritical()<<"ERROR! Invalid Peer Address!";
        return false;
    }
    if(!port){
        qCritical()<<"ERROR! Invalid Peer Port!";
        return false;
    }

    Q_ASSERT_X(epollID, "epollID", "ERROR! EPOLL Not Initialized!");
    if(!epollID){
        qCritical()<<"ERROR! EPOLL Not Initialized!";
        return false;
    }

    UDTSOCKET socket = socketsHash.value(targetAddress.toString()+":"+QString::number(port));
    if(UDT::INVALID_SOCK == socket){
        qCritical()<<"ERROR! Invalid UDTSOCKET!";
        return false;
    }

    int size = byteArray->size();
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

void AbstractUDTSocket::waitForIO(int msecTimeout){

    set<UDTSOCKET> readfds, writefds;
    int count = 0;

    while(true){
        count = UDT::epoll_wait(epollID, &readfds, &writefds, msecTimeout);
        printf("epoll returned %d sockets ready to IO| %d in set\n", count, readfds.size());
        for( std::set<UDTSOCKET>::const_iterator it = readfds.begin(); it != readfds.end(); ++it){
           //TODO:Process
            QtConcurrent::run(this, &AbstractUDTSocket::readDataFromSocket, *it);
        }
        readfds.clear();

        for( std::set<UDTSOCKET>::const_iterator it = writefds.begin(); it != writefds.end(); ++it){
           //TODO:Process
            QtConcurrent::run(this, &AbstractUDTSocket::writeDataToSocket, *it);
        }
        writefds.clear();

    }





}

void AbstractUDTSocket::readDataFromSocket(UDTSOCKET socket){

//    char peerHostAddress[NI_MAXHOST];
//    char peerPort[NI_MAXSERV];
//    getnameinfo((sockaddr *)&socket, addrlen, peerHostAddress, sizeof(clienthost), peerPort, sizeof(clientservice), NI_NUMERICHOST|NI_NUMERICSERV);
//    cout << "new connection: " << peerHostAddress << ":" << peerPort << endl;


//    struct addrinfo hints, *local;
//    memset(&hints, 0, sizeof(struct addrinfo));

//    //hints.ai_flags = AI_PASSIVE;
//    //hints.ai_family = AF_INET;
//    hints.ai_socktype = SOCK_STREAM;
//    //hints.ai_socktype = SOCK_DGRAM;
//    if (0 != getaddrinfo(NULL, QString::number(port).toStdString().c_str(), &hints, &local))
//    {
//        cout << "Not SOCK_STREAM" << endl;
//        return ;
//    }
//    freeaddrinfo(local);



    //TODO:Size
    char* data;
    int size = 2048;
    data = new char[size];

    int rs = 0;


    if(m_stream){

        while (true)
        {
           int rsize = 0;
           while (rsize < size)
           {
              if (UDT::ERROR == (rs = UDT::recv(socket, data + rsize, size - rsize, 0)))
              {
                 cout << "recv:" << UDT::getlasterror().getErrorMessage() << endl;
                 break;
              }

              rsize += rs;
           }

           if (rsize < size)
              break;

        }

        processStreamDataAfterReceived(socket, QByteArray(data));

    }else{
        if (UDT::ERROR == (rs = UDT::recvmsg(socket, data, size)))
        {
           cout << "recvmsg:" << UDT::getlasterror().getErrorMessage() << endl;
        }

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



    delete [] data;



}

void AbstractUDTSocket::writeDataToSocket(UDTSOCKET socket){


}

QByteArray AbstractUDTSocket::processStreamDataBeforeSent(const QByteArray &data){


    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_7);
    out << qint32(0) << data;

    out.device()->seek(0);
    out << qint32(block.size() - sizeof(qint32));

    return block;

}

void AbstractUDTSocket::processStreamDataAfterReceived(UDTSOCKET socket, const QByteArray &data){


    QByteArray byteArray(data);

//    QString key = address+":"+QString::number(port);
//    CachedDataInfo *info = m_cachedDataInfo.value(key);

    CachedDataInfo *info = m_cachedDataInfo.value(socket);


    int blockSize = info?info->blockSize:0;
    QByteArray *cachedData = 0;

    if(0 != blockSize){
        cachedData = info->data;
        Q_ASSERT(cachedData);

        byteArray.prepend(*cachedData);
        cachedData->clear();

        if(-1 == blockSize){
            blockSize = 0;
        }
    }

    QDataStream in(&byteArray, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_7);
    QIODevice *dev = in.device();
    //dev->seek(offset);

    QByteArray temp;
    qint64 readSize = 0;
    forever{

        if(0 == blockSize){
            blockSize = sizeof(qint32);
            temp.clear();
            temp.resize(blockSize);
            readSize = dev->read(temp.data(), blockSize);
            if(readSize == blockSize){
                blockSize = temp.toInt();
                //continue;
            }else{
                if(!info){
                    info = getCachedDataInfo();
                    m_cachedDataInfo.insert(socket, info);
                }
                info->blockSize = -1;
                *(info->data) = temp;
                break;
            }
        }


        temp.clear();
        temp.resize(blockSize);
        readSize = dev->read(temp.data(), blockSize);
        if(readSize == blockSize){
            streamDataReceived(socket, temp);
            blockSize = 0;
            continue;
        }else{
            if(!info){
                info = getCachedDataInfo();
                m_cachedDataInfo.insert(socket, info);
            }
            info->blockSize = blockSize;
            *(info->data) = temp;
            break;
        }

    }


}

void AbstractUDTSocket::recycleCachedDataInfo(CachedDataInfo* info){
    if(!info){
        return;
    }
    info->blockSize = 0;
    info->data->clear();
}

AbstractUDTSocket::CachedDataInfo * AbstractUDTSocket::getCachedDataInfo(){

    if(m_unusedCachedDataInfo.isEmpty()){
        return new CachedDataInfo();
    }
    return m_unusedCachedDataInfo.takeFirst();

}

void AbstractUDTSocket::getAddressInfoFromSocket(UDTSOCKET socket, QString *address, quint16 *port){


    sockaddr clientaddr;
    int addrlen = sizeof(clientaddr);
    //UDT::getsockname(socket, &clientaddr, &addrlen);
    UDT::getpeername(socket, &clientaddr, &addrlen);
    char clienthost[NI_MAXHOST];
    char clientservice[NI_MAXSERV];
    getnameinfo(&clientaddr, addrlen, clienthost, sizeof(clienthost), clientservice, sizeof(clientservice), NI_NUMERICHOST|NI_NUMERICSERV);
    cout << "Data Received From: " << clienthost << ":" << clientservice << endl;

    QString str = socketsHash.key(socket);
    qDebug()<<"str:"<<str;

    if(address){
        *address = QString::fromLocal8Bit(clienthost);
    }
    if(port){
        *port = QString::fromLocal8Bit(clientservice).toUInt();
    }

}

















}


#ifndef Q_OS_WIN32
void* recvdata(void* usocket)
#else
DWORD WINAPI recvdata(LPVOID usocket)
#endif
{
    qWarning()<<"recvdata(void* usocket)";

   UDTSOCKET recver = *(UDTSOCKET*)usocket;
   delete (UDTSOCKET*)usocket;

   char* data;
   int size = 100000;
   data = new char[size];

   while (true)
   {
      int rsize = 0;
      int rs = 0;
      while (rsize < size)
      {
         if (UDT::ERROR == (rs = UDT::recv(recver, data + rsize, size - rsize, 0)))
         {
            cout << "recv:" << UDT::getlasterror().getErrorMessage() << endl;
            break;
         }

         rsize += rs;
      }

      if (rsize < size)
         break;


   }

   delete [] data;

   UDT::close(recver);


}






