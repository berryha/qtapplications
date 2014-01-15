#include "enetprotocolbase.h"

#include <QCoreApplication>
#include <QDebug>
#include <QtConcurrent>

//#ifdef Q_OS_WIN32
//#define WIN32_LEAN_AND_MEAN
//#include<winsock2.h>
//#include<windows.h>
////#else

//#endif




namespace HEHUI {


ENETProtocolBase::ENETProtocolBase(QObject *parent) :
    QObject(parent)
{

    m_listening = false;
    m_threadCount = 0;

    localServer = 0;

    if(enet_initialize())
    {
        qCritical()<<"ERROR! Failed to initialize ENET!";
    }

}

ENETProtocolBase::~ENETProtocolBase(){
    close();
}


bool ENETProtocolBase::isListening() const {
    return m_listening;
}

bool ENETProtocolBase::getPeerAddressInfo(quint32 peerID, QString *address, quint16 *port){
    if(!peersHash.contains(peerID)){
        qCritical()<<"ERROR! Failed to get peer host address! No such peer!";
        return false;
    }

    ENetPeer *peer = peersHash.value(peerID);
    ENetAddress remote = peer->address;
    char ip[256];
    if( enet_address_get_host_ip(&remote, ip, 256) < 0 ){
        qCritical()<<"ERROR! Failed to get peer host address!";
        return false;
    }

    if(address){
        *address = QString(ip);
    }
    if(port){
        *port = remote.port;
    }

    return true;

}

bool ENETProtocolBase::getLocalListeningAddressInfo(QString *address, quint16 *port){

    if(!localServer){
        return false;
    }

    ENetAddress local = localServer->address;
    char ip[256];
    if( enet_address_get_host_ip(&local, ip, 256) < 0 ){
        qCritical()<<"ERROR! Failed to get local host address!";
        return false;
    }

    if(address){
        *address = QString(ip);
    }
    if(port){
        *port = local.port;
    }

    return true;

}

bool ENETProtocolBase::listen(quint16 port, const QHostAddress &localAddress, unsigned int maximumNumberOfPeers){

    if(localServer){
        qCritical()<<"ERROR! ENET Server already listening!";
        return false;
    }

    if(enet_initialize())
    {
        qCritical()<<"ERROR! Failed to initialize ENET!";
        return false;
    }

    if(port == 0){
        //TODO
    }

    ENetAddress localListeningAddress;
    enet_address_set_host(&localListeningAddress, localAddress.toString().toStdString().c_str());
    localListeningAddress.port = port;

    return listen(&localListeningAddress, maximumNumberOfPeers);

//    localServer = enet_host_create(&localListeningAddress, maximumNumberOfPeers, 0, 0);
//    if(localServer == 0)
//    {
//        qCritical()<<"ERROR! Failed to create ENET server!";
//         enet_deinitialize();
//        return false;
//    }

//    return true;
}


void ENETProtocolBase::close(){

    m_listening = false;
    while (m_threadCount) {
        //wait for other threads!
        QCoreApplication::processEvents();
        //qDebug()<<"Waiting for threads to quit ...";
        msleep(10);
    }

    if(localServer){
        enet_host_destroy(localServer);
        localServer = 0;
    }

    enet_deinitialize();

    peersHash.clear();

}

void ENETProtocolBase::startWaitingForIOInOneThread(unsigned int msecWaitForIOTimeout){
    QtConcurrent::run(this, &ENETProtocolBase::waitForIO, msecWaitForIOTimeout);
}

bool ENETProtocolBase::connectToHost(const QHostAddress &address, quint16 port, quint32 *peerID, unsigned int msecTimeout){

    if(!localServer){
        qCritical()<<"ERROR! ENET Server is not running!";
        return false;
    }

    ENetAddress peerAddress;
    enet_address_set_host(&peerAddress, address.toString().toStdString().c_str());
    peerAddress.port = port;

    ENetPeer *peer = enet_host_connect(localServer, &peerAddress, 2, 0);
    if(peer == NULL)
    {
        qCritical()<<"ERROR! Failed to connect to peer!";
        return false;
    }

    //TODO

    unsigned int timeBase = timeGetTime();
    while (peer->state != ENET_PEER_STATE_CONNECTED) {
        msleep(10);
        if(timeGetTime() - timeBase > msecTimeout){
            qCritical()<<"ERROR! Connecting to peer timeout!";
            enet_peer_reset (peer);
            return false;
        }
    }


//    ENetEvent event;
//    if (enet_host_service(localServer, &event, msecTimeout) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
//    {
//        return true;
//    }else{
//        qCritical()<<"ERROR! Failed to connect to peer!";
//        enet_peer_reset (peer);
//        return false;
//    }

    if(peerID){
        *peerID = peer->connectID;
    }

    return true;


}

bool ENETProtocolBase::sendData(ENetPeer *peer, const QByteArray *byteArray){

    ENetPacket *packet = enet_packet_create(byteArray->constData(), byteArray->size(), ENET_PACKET_FLAG_RELIABLE);
    int result = enet_peer_send(peer, 0, packet);
    if(result < 0){
        qCritical()<<"ERROR! Failed to send data!";
        return false;
    }

    enet_host_flush (localServer);

    return true;
}

bool ENETProtocolBase::sendData(quint32 peerID, const QByteArray *byteArray){
    ENetPeer *peer = peersHash.value(peerID);
    if(!peer){
        return false;
    }

    return sendData(peer, byteArray);
}

void ENETProtocolBase::disconnectNow(ENetPeer *peer){
    //No ENET_EVENT_DISCONNECT event will be generated.
      enet_peer_disconnect_now(peer, 0);

      peersHash.remove(peer->connectID);
}

void ENETProtocolBase::disconnectNow(quint32 peerID){
    ENetPeer *peer = peersHash.value(peerID);
    if(!peer){
        return;
    }
    disconnectNow(peer);
}

void ENETProtocolBase::disconnect(ENetPeer *peer){
    //An ENET_EVENT_DISCONNECT event will be generated by enet_host_service() once the disconnection is complete.
    enet_peer_disconnect(peer, 0);
}

void ENETProtocolBase::disconnect(quint32 peerID){
    ENetPeer *peer = peersHash.value(peerID);
    if(!peer){
        return;
    }
    disconnect(peer);
}

void ENETProtocolBase::disconnectLater(ENetPeer *peer){
    //An ENET_EVENT_DISCONNECT event will be generated by enet_host_service() once the disconnection is complete.
    //All queued outgoing packets are sent.
    enet_peer_disconnect_later(peer, 0);
}

void ENETProtocolBase::disconnectLater(quint32 peerID){
    ENetPeer *peer = peersHash.value(peerID);
    if(!peer){
        return;
    }
    disconnectLater(peer);
}

bool ENETProtocolBase::listen(ENetAddress *localListeningAddress, unsigned int maximumNumberOfPeers){

    if(localServer){
        qCritical()<<"ERROR! ENET Server already listening!";
        return false;
    }

    localServer = enet_host_create(localListeningAddress, maximumNumberOfPeers, 0, 0, 0);
    if(localServer == 0)
    {
        qCritical()<<"ERROR! Failed to create ENET server!";
         enet_deinitialize();
        return false;
    }

    return true;
}

void ENETProtocolBase::waitForIO(int msecTimeout){

    m_threadCount++;

    ENetEvent event;
    while(m_listening)
    {

        int eventsCount = enet_host_service(localServer, &event, msecTimeout);
        if(eventsCount < 0 ){
            qCritical()<<"ERROR! Error getting events!";
            continue;
        }

//        if(eventsCount == 0){
//            msleep(10);
//        }

        if(event.type==ENET_EVENT_TYPE_CONNECT)
         {
          static unsigned int num=0;
          ENetAddress remote = event.peer->address;
          char ip[256];
          enet_address_get_host_ip(&remote,ip,256);
          qDebug()<<"IP: "<<ip<<" NO.:"<<num;
          event.peer->data=(void*)num++;

          qDebug()<<"------CONNECT-----connectID:"<<event.peer->connectID;

          ENetPeer *peer = event.peer;
          peersHash.insert(peer->connectID, peer);

          emit connected(peer->connectID, QString(ip), remote.port);
        }
         else if(event.type==ENET_EVENT_TYPE_RECEIVE)
         {
          qDebug() <<"Data received from " <<event.peer->data <<", Sent from " <<event.channelID <<" Channel" ;
          qDebug() <<"Size:" <<event.packet->dataLength ;
          qDebug() <<"Data:" <<(char*)event.packet->data ;

          QByteArray byteArray;
          byteArray.resize(event.packet->dataLength);
          byteArray.append( (char*)event.packet->data );

          processReceivedData(event.peer->connectID, &byteArray);

          enet_packet_destroy(event.packet);
         }
         else if(event.type==ENET_EVENT_TYPE_DISCONNECT) //失去连接
         {
            ENetAddress remote = event.peer->address;
            char ip[256];
            enet_address_get_host_ip(&remote,ip,256);

            peersHash.remove(event.peer->connectID);

            qDebug()<<"------DISCONNECT-----connectID:"<<event.peer->connectID;

            emit disconnected(event.peer->connectID, QString(ip), remote.port);

          qDebug() <<"NO. " <<event.peer->data <<"Peer closed!" ;
        }

    }

    m_threadCount--;

}

inline void ENETProtocolBase::msleep(int msec){

#ifdef Q_OS_WIN32
    Sleep(msec);
#else
    usleep(msec*1000);
#endif

}


} //namespace HEHUI
