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

AbstractUDTSocket::AbstractUDTSocket(QObject *parent)
    :QObject(parent)
{

    // use this function to initialize the UDT library
    UDT::startup();

    epollID = 0;
    serv = UDT::INVALID_SOCK;

    m_listening = false;
    m_serverAddress = QHostAddress::Null;
    m_serverPort = 0;




}

AbstractUDTSocket::~AbstractUDTSocket() {

    if(m_listening){
        close();
    }

    UDT::epoll_release(epollID);

    // use this function to release the UDT library
    UDT::cleanup();

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
    hints.ai_socktype = SOCK_STREAM;
    //hints.ai_socktype = SOCK_DGRAM;
    //hints.ai_addr.s_addr = htonl(INADDR_ANY);
    hints.ai_addr.s_addr = inet_addr(localAddress.toString().toLocal8Bit().data());




    if (0 != getaddrinfo(NULL, QString::number(port).toStdString().c_str(), &hints, &res))
    {
       cout << "illegal port number or port is busy.\n" << endl;
       return 0;
    }

    serv = UDT::socket(res->ai_family, res->ai_socktype, res->ai_protocol);


    // UDT Options
    //UDT::setsockopt(serv, 0, UDT_CC, new CCCFactory<CUDPBlast>, sizeof(CCCFactory<CUDPBlast>));
    //UDT::setsockopt(serv, 0, UDT_MSS, new int(9000), sizeof(int));
    //UDT::setsockopt(serv, 0, UDT_RCVBUF, new int(10000000), sizeof(int));
    //UDT::setsockopt(serv, 0, UDP_RCVBUF, new int(10000000), sizeof(int));

    //non-blocking receiving
    //UDT::setsockopt(serv, 0, UDT_RCVSYN, new bool(false), sizeof(bool));


    if (UDT::ERROR == UDT::bind(serv, res->ai_addr, res->ai_addrlen))
    {
       cout << "bind: " << UDT::getlasterror().getErrorMessage() << endl;
       return 0;
    }

    freeaddrinfo(res);


    cout << "server is ready at port: " << port << endl;

    if (UDT::ERROR == UDT::listen(serv, 10))
    {
       cout << "listen: " << UDT::getlasterror().getErrorMessage() << endl;
       return 0;
    }

    epollID = UDT::epoll_create();
    UDT::epoll_add_usock(epollID, serv);

    m_serverAddress = localAddress;
    m_serverPort = port;


    QtConcurrent::run(this, &AbstractUDTSocket::waitForNewConnection, 0, new bool(0));


    m_listening = true;

    return true;


}

void AbstractUDTSocket::close(){

    UDT::close(serv);

    m_listening = false;
    m_serverAddress = QHostAddress::Null;
    m_serverPort = 0;

}

bool AbstractUDTSocket::connectToHost(const QHostAddress &address, quint16 port, bool sync){

    Q_ASSERT_X(epollID, "epollID", "ERROR! EPOLL Not Initialized!");
    if(!epollID){
        qCritical()<<"ERROR! EPOLL Not Initialized!";
        return;
    }

    // use this function to initialize the UDT library
    //UDT::startup();

    struct addrinfo hints, *local, *peer;

    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    //hints.ai_socktype = SOCK_DGRAM;


    if (0 != getaddrinfo(NULL, QString::number(port).toStdString().c_str(), &hints, &local))
    {
        cout << "incorrect network address.\n" << endl;
        return 0;
    }

    UDTSOCKET client = UDT::socket(local->ai_family, local->ai_socktype, local->ai_protocol);

    freeaddrinfo(local);

    if (0 != getaddrinfo(targetAddress.toString().toStdString().c_str(), QString::number(port).toStdString().c_str(), &hints, &peer))
    {
        //cout << "incorrect server/peer address. " << argv[1] << ":" << argv[2] << endl;
        qWarning()<< "incorrect server/peer address. " << targetAddress.toString() << ":" << port;
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

    if (UDT::ERROR == UDT::bind(client, serv))
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


    UDT::close(client);


    // use this function to release the UDT library
    //UDT::cleanup();

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

        if (UDT::INVALID_SOCK == (peer = UDT::accept(serv, (sockaddr*)&clientaddr, &addrlen)))
        {
            qWarning()<<"ERROR! Invalid UDTSOCKET! "<< UDT::getlasterror().getErrorMessage();
           //cout << "accept: " << UDT::getlasterror().getErrorMessage() << endl;
           continue;
        }

        char clienthost[NI_MAXHOST];
        char clientservice[NI_MAXSERV];
        getnameinfo((sockaddr *)&clientaddr, addrlen, clienthost, sizeof(clienthost), clientservice, sizeof(clientservice), NI_NUMERICHOST|NI_NUMERICSERV);
        cout << "New Connection: " << clienthost << ":" << clientservice << endl;


        if(UDT::epoll_add_usock(epollID, peer, NULL) < 0){
            qWarning()<<"ERROR! epoll_add_usock Failed! "<< UDT::getlasterror().getErrorMessage();
            fprintf(stderr, "epoll_add_usock error\n");
            continue;
        }

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

bool AbstractUDTSocket::sendUDTData(const QHostAddress &targetAddress, quint16 port, const QByteArray &byteArray, bool stream){

    // use this function to initialize the UDT library
    //UDT::startup();

    struct addrinfo hints, *local, *peer;

    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    //hints.ai_socktype = SOCK_DGRAM;


    if (0 != getaddrinfo(NULL, QString::number(port).toStdString().c_str(), &hints, &local))
    {
        cout << "incorrect network address.\n" << endl;
        return 0;
    }

    UDTSOCKET client = UDT::socket(local->ai_family, local->ai_socktype, local->ai_protocol);

    freeaddrinfo(local);

    if (0 != getaddrinfo(targetAddress.toString().toStdString().c_str(), QString::number(port).toStdString().c_str(), &hints, &peer))
    {
        //cout << "incorrect server/peer address. " << argv[1] << ":" << argv[2] << endl;
        qWarning()<< "incorrect server/peer address. " << targetAddress.toString() << ":" << port;
        return 0;
    }

    //non-blocking sending
    //UDT::setsockopt(client, 0, UDT_SNDSYN, new bool(false), sizeof(bool));
    //UDT::setsockopt(client, 0, UDT_SNDBUF, new int(10240000), sizeof(int));
    UDT::setsockopt(client, 0, UDT_SNDBUF, new int(1024000000), sizeof(int));



    // connect to the server, implict bind
    if (UDT::ERROR == UDT::connect(client, peer->ai_addr, peer->ai_addrlen))
    {
        cout << "connect: " << UDT::getlasterror().getErrorMessage() << endl;
        return 0;
    }

    freeaddrinfo(peer);



    int size = byteArray.size();
    //char* data = new char[size];
    const char* data = byteArray.constData();

    //		   #ifndef WIN32
    //		      pthread_create(new pthread_t, NULL, monitor, &client);
    //		   #else
    //		      CreateThread(NULL, 0, monitor, &client, 0, NULL);
    //		   #endif

        int ssize = 0;
        int ss;
        while (ssize < size)
        {
            if (UDT::ERROR == (ss = UDT::send(client, data + ssize, size - ssize, 0)))
            {
                cout << "send:" << UDT::getlasterror().getErrorMessage() << endl;
                break;
            }

            ssize += ss;
            //msleep(1000);
            QCoreApplication::processEvents();
        }

        if (ssize < size){
            qCritical()<<"Data sent failed!"<<"   Data Size:"<<size<<"   Sent size:"<<ssize;
            return false;
        }

    UDT::close(client);


    // use this function to release the UDT library
    //UDT::cleanup();

    return true;

}

void AbstractUDTSocket::waitForIO(int msecTimeout){

    set<UDTSOCKET> readfds, writefds;
    int count = 0;

    while(true){
        count = UDT::epoll_wait(epollID, &readfds, &writefds, 1000);
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


    char* data;
    int size = 2048;
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

    dataReceived(QByteArray(data), true);

    delete [] data;



}

void AbstractUDTSocket::writeDataToSocket(UDTSOCKET socket){


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






