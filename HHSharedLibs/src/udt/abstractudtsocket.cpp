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

    m_listening = false;
    m_serverAddress = QHostAddress::Null;
    m_serverPort = 0;

    serv = UDT::INVALID_SOCK;



}

AbstractUDTSocket::~AbstractUDTSocket() {

    if(m_listening){
        close();
    }

    // use this function to release the UDT library
    UDT::cleanup();

}

bool AbstractUDTSocket::listen(const QHostAddress &localAddress, quint16 port){

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

void AbstractUDTSocket::waitForNewConnection(int msec, bool * timedOut){

    sockaddr_storage clientaddr;
    int addrlen = sizeof(clientaddr);

    UDTSOCKET recver;

    while (true)
    {
       if (UDT::INVALID_SOCK == (recver = UDT::accept(serv, (sockaddr*)&clientaddr, &addrlen)))
       {
          cout << "accept: " << UDT::getlasterror().getErrorMessage() << endl;
          continue;
       }

       char clienthost[NI_MAXHOST];
       char clientservice[NI_MAXSERV];
       getnameinfo((sockaddr *)&clientaddr, addrlen, clienthost, sizeof(clienthost), clientservice, sizeof(clientservice), NI_NUMERICHOST|NI_NUMERICSERV);
       cout << "new connection: " << clienthost << ":" << clientservice << endl;

       #ifndef Q_OS_WIN32
          pthread_t rcvthread;
          pthread_create(&rcvthread, NULL, recvdata, new UDTSOCKET(recver));
          pthread_detach(rcvthread);
       #else
          CreateThread(NULL, 0, recvdata, new UDTSOCKET(recver), 0, NULL);
          //CreateThread(NULL, 0, recvdata, &recver, 0, NULL);
       #endif
    }



}

bool AbstractUDTSocket::sendUDTData(const QHostAddress &targetAddress, quint16 port, const QByteArray &byteArray){

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


