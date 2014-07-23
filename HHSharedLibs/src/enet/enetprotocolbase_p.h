#ifndef ENETPROTOCOLBASE_P_H
#define ENETPROTOCOLBASE_P_H



#include <QObject>
#include <QHostAddress>

#include "enet/include/enet/enet.h"


namespace HEHUI {

class ENETProtocolBasePrivate;

class ENETProtocolBasePrivate : public QObject
{
    Q_OBJECT
public:
    explicit ENETProtocolBasePrivate(QObject *parent = 0);
    ~ENETProtocolBasePrivate();

    bool isListening() const;
    bool getPeerAddressInfo(quint32 peerID, QString *address, quint16 *port);
    bool getLocalListeningAddressInfo(QString *address, quint16 *port);


    void setPeerPrivateData(quint32 peerID, void *data);
    void * getPeerPrivateData(quint32 peerID);


signals:
    void connected(quint32 peerID, const QString &address, quint16 port);
    void disconnected(quint32 peerID, const QString &address, quint16 port);
    void dataReceived(quint32 peerID, const QByteArray &data);

public slots:
    //Start the server to listen,  implement the virtual function startWaitingForIO()
    bool listen(quint16 port, const QHostAddress &localAddress= QHostAddress::Any, unsigned int maximumNumberOfPeers = ENET_PROTOCOL_MAXIMUM_PEER_ID);
    //Close the server
    void close();

    //Call this function after server is listening
    void startWaitingForIOInAnotherThread(unsigned int msecWaitForIOTimeout = 10);
    void waitForIO(int msecTimeout = 20);

    //Connect to peer
    bool connectToHost(const QHostAddress &address, quint16 port, quint32 *peerID, unsigned int msecTimeout = 5000, quint32 channels = 0);

    //Send data
    bool sendData(ENetPeer *peer, const QByteArray *byteArray, bool reliable = true, quint8 channel = 0);
    bool sendData(quint32 peerID, const QByteArray *byteArray, bool reliable = true, quint8 channel = 0);

    //enet_host_flush
    void flush();

    //Close peer socket
    void disconnectNow(quint32 peerID);

    void disconnect(quint32 peerID);

    void disconnectLater(quint32 peerID);


protected:
    bool listen(ENetAddress *localListeningAddress = 0, unsigned int maximumNumberOfPeers = ENET_PROTOCOL_MAXIMUM_PEER_ID);


private slots:


private:
    void msleep(int msec);

    void setPeerPrivateData(ENetPeer *peer, void *data);

    //Close peer socket
    void disconnectNow(ENetPeer *peer);
    void disconnect(ENetPeer *peer);
    void disconnectLater(ENetPeer *peer);

//    virtual void processReceivedData(quint32 peerID, QByteArray *data) = 0;


private:

    bool m_listening;
    int m_threadCount;

    ENetHost *localServer;

    QHash<quint32 /*Peer ID*/, ENetPeer *> peersHash;



};

} //namespace HEHUI

#endif // ENETPROTOCOLBASE_P_H
