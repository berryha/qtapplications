#ifndef ENETPROTOCOLBASE_H
#define ENETPROTOCOLBASE_H



#include <QObject>
#include <QHostAddress>

#include "enet/include/enet/enet.h"


#if defined(ENET_LIBRARY_EXPORT)
#  define ENET_LIB_API  Q_DECL_EXPORT
#else
#  define ENET_LIB_API Q_DECL_IMPORT
#endif


namespace HEHUI {


class ENET_LIB_API ENETProtocolBase : public QObject
{
    Q_OBJECT
public:
    explicit ENETProtocolBase(QObject *parent = 0);
    ~ENETProtocolBase();

    bool isListening() const;
    bool getPeerAddressInfo(quint32 peerID, QString *address, quint16 *port);
    bool getLocalListeningAddressInfo(QString *address, quint16 *port);

signals:
    void connected(quint32 peerID, const QString &address, quint16 port);
    void disconnected(quint32 peerID, const QString &address, quint16 port);

//    void connected(const QHostAddress &address, quint16 port);
//    void disconnected(const QHostAddress &address, quint16 port);



public slots:
    //Start the server to listen,  implement the virtual function startWaitingForIO()
    bool listen(quint16 port, const QHostAddress &localAddress= QHostAddress::Any, unsigned int maximumNumberOfPeers = 65535);
    //Close the server
    void close();

    //Call this function after server is listening
    void startWaitingForIOInOneThread(unsigned int msecWaitForIOTimeout = 20);

    //Connect to peer
    bool connectToHost(const QHostAddress &address, quint16 port, quint32 *peerID, unsigned int msecTimeout = 5000);

    //Send data
    bool sendData(ENetPeer *peer, const QByteArray *byteArray);
    bool sendData(quint32 peerID, const QByteArray *byteArray);

    //Close peer socket
    void disconnectNow(ENetPeer *peer);
    void disconnectNow(quint32 peerID);

    void disconnect(ENetPeer *peer);
    void disconnect(quint32 peerID);

    void disconnectLater(ENetPeer *peer);
    void disconnectLater(quint32 peerID);


protected:
    bool listen(ENetAddress *localListeningAddress = 0, unsigned int maximumNumberOfPeers = 0xFFF);
    void waitForIO(int msecTimeout = 20);

private slots:


private:
    void msleep(int msec);

    virtual void processReceivedData(quint32 peerID, QByteArray *data) = 0;


private:

    bool m_listening;
    int m_threadCount;

    ENetHost *localServer;

    QHash<quint32 /*Peer ID*/, ENetPeer *> peersHash;



};

} //namespace HEHUI

#endif // ENETPROTOCOLBASE_H
