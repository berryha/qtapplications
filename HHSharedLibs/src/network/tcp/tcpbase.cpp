
#include <QtConcurrentRun>


#include "tcpbase.h"

//#include "../packethandler/packet.h"


namespace HEHUI{

TCPBase::TCPBase(QObject *parent) :
    QObject(parent)
{

    m_tcpServer = new TcpServer(this);
    connect(m_tcpServer, SIGNAL(newIncomingTCPConnection(TcpSocket *)), this, SLOT(newIncomingConnection(TcpSocket *)));

    m_lastSocketID = 0;

    m_proxy = QNetworkProxy::NoProxy;


}

TCPBase::~TCPBase(){

    if(m_tcpServer->isListening()){
        m_tcpServer->close();
    }
    m_tcpServer->deleteLater();

    QMutexLocker locker(&mutex);
    foreach (TcpSocket *socket, m_socketsHash.values()) {
        if(!socket){continue;}
        socket->close();
        socket->deleteLater();
    }
    m_socketsHash.clear();

}

bool TCPBase::listen ( const QHostAddress & address, quint16 port ){

    return m_tcpServer->listen(address, port);

}

bool TCPBase::isListening () const{
    return m_tcpServer->isListening();
}

void TCPBase::closeServer (){
    if(m_tcpServer->isListening()){
        m_tcpServer->close();
    }
}

QString	TCPBase::serverErrorString () const{
    return m_tcpServer->errorString();
}

void TCPBase::serverAddressInfo (QHostAddress *address, quint16 *port ){

    if(address){
        *address = m_tcpServer->serverAddress();
    }
    if(port){
        *port = m_tcpServer->serverPort();
    }
}

QAbstractSocket::SocketError TCPBase::serverError () const{
    return m_tcpServer->serverError();
}

bool TCPBase::waitForNewConnection ( int msec, bool * timedOut ){
    return m_tcpServer->waitForNewConnection(msec, timedOut);
}

QNetworkProxy TCPBase::proxy () const{
    return m_proxy;
}

void TCPBase::setProxy ( const QNetworkProxy & networkProxy ){
    this->m_proxy = networkProxy;
}

int TCPBase::connectToHost ( const QString & hostName, quint16 port, int waitMsecs){
    TcpSocket *socket = new TcpSocket(this);
    socket->setProxy(m_proxy);
    connect(socket, SIGNAL(connected()), this, SLOT(peerConnected()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SIGNAL(socketError(QAbstractSocket::SocketError)));

    int socketID = -1;
    {
        QMutexLocker locker(&mutex);
        socketID = (++m_lastSocketID);
        m_socketsHash.insert(socketID, socket);
    }

    socket->connectToHost(hostName, port);
    if(waitMsecs){
        socket->waitForConnected(waitMsecs);
    }

    return socketID;

}

int TCPBase::connectToHost ( const QHostAddress & address, quint16 port, int waitMsecs){
    TcpSocket *socket = new TcpSocket(this);
    socket->setProxy(m_proxy);
    connect(socket, SIGNAL(connected()), this, SLOT(peerConnected()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SIGNAL(socketError(QAbstractSocket::SocketError)));

    int socketID = -1;
    {
        QMutexLocker locker(&mutex);
        socketID = (++m_lastSocketID);
        m_socketsHash.insert(socketID, socket);
    }

    socket->connectToHost(address, port);
    if(waitMsecs){
        socket->waitForConnected(waitMsecs);
    }

    return socketID;

}

void TCPBase::disconnectFromHost (int socketID, int waitMsecs){

    TcpSocket *socket = 0;
    {
        QMutexLocker locker(&mutex);
        socket = m_socketsHash.value(socketID);
    }
    if(!socket){
        return ;
    }

    socket->disconnectFromHost();
    if(waitMsecs){
        socket->waitForDisconnected(waitMsecs);
    }

}

void TCPBase::closeSocket(int socketID){

    QMutexLocker locker(&mutex);
    TcpSocket *socket = m_socketsHash.take(socketID);
    if(!socket){
        return;
    }

    socket->close();
    delete socket;
    socket = 0;

}

bool TCPBase::isConnected(int socketID){

    QMutexLocker locker(&mutex);
    TcpSocket *socket = m_socketsHash.value(socketID);
    if(!socket){
        return false;
    }

    return (socket->state() == QAbstractSocket::ConnectedState);

}

bool TCPBase::isConnected(const QHostAddress & address, quint16 port){

    QMutexLocker locker(&mutex);
    foreach (TcpSocket *socket, m_socketsHash.values()) {
        if(socket->peerAddress() == address && socket->peerPort() == port){
            return (socket->state() == QAbstractSocket::ConnectedState);
        }
    }

    return false;

}

QAbstractSocket::SocketState TCPBase::socketState(int socketID){

    QMutexLocker locker(&mutex);
    if(!m_socketsHash.contains(socketID)){
        return QAbstractSocket::UnconnectedState;
    }

    return m_socketsHash.value(socketID)->state();

}

QAbstractSocket::SocketState TCPBase::socketState(const QHostAddress & address, quint16 port){

    QMutexLocker locker(&mutex);
    foreach (TcpSocket *socket, m_socketsHash.values()) {
        if(socket->peerAddress() == address && socket->peerPort() == port){
            return socket->state();
        }
    }

    return QAbstractSocket::UnconnectedState;

}

void TCPBase::socketPeerAddressInfo(int socketID, QHostAddress *peerAddress, quint16 *peerPort ){

    TcpSocket *socket = 0;
    {
        QMutexLocker locker(&mutex);
        socket = m_socketsHash.value(socketID);
    }
    if(!socket){return;}

    if(peerAddress){
        *peerAddress = socket->peerAddress();
    }
    if(peerPort){
        *peerPort = socket->peerPort();
    }

}

void TCPBase::socketLocalAddressInfo(int socketID, QHostAddress *localAddress, quint16 *localPort ){

    TcpSocket *socket = 0;
    {
        QMutexLocker locker(&mutex);
        socket = m_socketsHash.value(socketID);
    }
    if(!socket){return;}

    if(localAddress){
        *localAddress = socket->localAddress();
    }
    if(localPort){
        *localPort = socket->localPort();
    }

}

QString	TCPBase::socketErrorString (int socketID){
    TcpSocket *socket = 0;
    {
        QMutexLocker locker(&mutex);
        socket = m_socketsHash.value(socketID);
    }
    if(!socket){
        QString err = tr("No such socket '%1'!").arg(socketID);
        return err;
    }

    return socket->errorString();

}

bool TCPBase::sendData(int socketID, const QByteArray *byteArray){

    TcpSocket *socket = 0;
    {
        QMutexLocker locker(&mutex);
        socket = m_socketsHash.value(socketID);
    }
    if(!socket){return false;}

    return socket->sendTCPDatagram(byteArray);

//    QByteArray block;
//    QDataStream out(&block, QIODevice::WriteOnly);
//    out.setVersion(QDataStream::Qt_4_8);
//    out << quint16(0) << *byteArray;

//    out.device()->seek(0);
//    out << quint16(block.size() - sizeof(quint16));
//    return (socket->write(block) == block.size());

}


void TCPBase::newIncomingConnection(TcpSocket *socket){

    {
        QMutexLocker locker(&mutex);
        int socketID = (++m_lastSocketID);
        m_socketsHash.insert(socketID, socket);
    }
    setupNewSocket(socket);

}

void TCPBase::peerConnected (){
    TcpSocket *socket = qobject_cast<TcpSocket *>(sender());
    setupNewSocket(socket);
}

void TCPBase::peerDisconnected (){
    TcpSocket *socket = qobject_cast<TcpSocket *>(sender());
    if(!socket){
        return;
    }

    int socketID = -1;
    {
        QMutexLocker locker(&mutex);
        socketID = m_socketsHash.key(socket);
    }
    qDebug()<<QString("Peer Disconnected! %1:%2").arg(socket->peerAddress().toString()).arg(socket->peerPort());
    emit disconnected(socketID, socket->peerAddress().toString(), socket->peerPort());

}

void TCPBase::setupNewSocket(TcpSocket *socket){

    if(!socket){
        return;
    }
    connect(socket, SIGNAL(disconnected()), this, SLOT(peerDisconnected()));
    connect(socket, SIGNAL(dataReceived(const QByteArray &)), this, SLOT(dataReceived(const QByteArray &)));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SIGNAL(socketError(QAbstractSocket::SocketError)));

    int socketID = -1;
    {
        QMutexLocker locker(&mutex);
        socketID = m_socketsHash.key(socket);
    }
    qDebug()<<QString("Peer Connected! %1:%2").arg(socket->peerAddress().toString()).arg(socket->peerPort());
    emit connected(socketID, socket->peerAddress().toString(), socket->peerPort());

}

void TCPBase::dataReceived(const QByteArray &data){
    qDebug()<<"--TCPBase::dataReceived(...) "<<" data.size():"<<data.size();

    TcpSocket *socket = qobject_cast<TcpSocket *>(sender());
    if(!socket){
        return;
    }
    qDebug()<<QString("Data Received From %1:%2. Size:%3").arg(socket->peerAddress().toString()).arg(socket->peerPort()).arg(data.size());


    int socketID = -1;
    {
        QMutexLocker locker(&mutex);
        socketID = m_socketsHash.key(socket);
    }
    Q_ASSERT(socketID > 0);

    processData(socketID, data);

}

void TCPBase::processData(int socketID, const QByteArray &data){


//    QHostAddress address;
//    quint16 port = 0;
//    socketPeerAddressInfo(socketID, &address, &port);

//    QDataStream in(&data, QIODevice::ReadOnly);
//    in.setVersion(QDataStream::Qt_4_8);
//    QVariant v;
//    in >> v;
//    if (v.canConvert<Packet>()){
//        Packet *packet = PacketHandlerBase::getPacket(socketID);
//        *packet = v.value<Packet>();
//        packet->setTransmissionProtocol(TP_TCP);
//        //packet->setSocketID(socketID);


//        packet->setPeerHostAddress(address);
//        packet->setPeerHostPort(port);
////        packet->setLocalHostAddress(m_udpSocket->localAddress());
////        packet->setLocalHostPort(m_udpSocket->localPort());

////        m_packetHandlerBase->appendIncomingPacket(packet);

////        emit packetReceived(packet);
//    }


}








} //namespace HEHUI
