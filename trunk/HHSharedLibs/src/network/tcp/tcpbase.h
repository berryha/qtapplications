#ifndef TCPBASE_H
#define TCPBASE_H

#include <QObject>
#include <QHostAddress>
#include <QNetworkProxy>
#include <QMutex>

#include "tcpsocket.h"
#include "tcpserver.h"

#include "../networklib.h"
#include "../global_network.h"

namespace HEHUI {


class NETWORK_LIB_API TCPBase : public QObject
{
    Q_OBJECT
public:
    explicit TCPBase(QObject *parent = 0);
    ~TCPBase();

    /////////////////////////////
    bool listen ( const QHostAddress & address = QHostAddress::Any, quint16 port = 0 );
    bool isListening () const;
    void closeServer ();
    QString	serverErrorString () const;
    void serverAddressInfo(QHostAddress *address, quint16 *port );
    QAbstractSocket::SocketError serverError () const;
    bool waitForNewConnection ( int msec = 0, bool * timedOut = 0 );

    QNetworkProxy proxy () const;
    void setProxy ( const QNetworkProxy & networkProxy );

    //////////////////////////////
    int connectToHost ( const QString & hostName, quint16 port, int waitMsecs = 0);
    int connectToHost ( const QHostAddress & address, quint16 port, int waitMsecs = 0);
    void disconnectFromHost (int socketID, int waitMsecs = 0);
    void closeSocket(int socketID);
    bool isConnected(int socketID);
    bool isConnected(const QHostAddress & address, quint16 port);
    QAbstractSocket::SocketState socketState(int socketID);
    QAbstractSocket::SocketState socketState(const QHostAddress & address, quint16 port);
    void socketPeerAddressInfo(int socketID, QHostAddress *peerAddress, quint16 *peerPort );
    void socketLocalAddressInfo(int socketID, QHostAddress *localAddress, quint16 *localPort );
    QString	socketErrorString (int socketID);

    bool sendData(int socketID, const QByteArray *byteArray);



signals:
    void connected (int socketID, const QString &peerAddress, quint16 peerPort);
    void disconnected (int socketID, const QString &peerAddress, quint16 peerPort);
    void socketError(QAbstractSocket::SocketError socketError);

protected:
    
private slots:
    void newIncomingConnection(TcpSocket *socket);
    void peerConnected ();
    void peerDisconnected ();

    void setupNewSocket(TcpSocket *socket);
    void dataReceived(const QByteArray &data);

private:
    virtual void processData(int socketID, const QByteArray &data) = 0;


private:
    TcpServer *m_tcpServer;
    int m_lastSocketID;

    QHash<int/*Socket ID*/, TcpSocket*> m_socketsHash;

    QNetworkProxy m_proxy;

    QMutex mutex;

    
};

} //namespace HEHUI

#endif // TCPBASE_H
