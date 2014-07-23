#ifndef ENETPROTOCOLBASE_H
#define ENETPROTOCOLBASE_H



#include <QObject>
#include <QHostAddress>


#if defined(ENET_LIBRARY_EXPORT)
#  define ENET_LIB_API  Q_DECL_EXPORT
#else
#  define ENET_LIB_API Q_DECL_IMPORT
#endif


namespace HEHUI {

class ENETProtocolBasePrivate;

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

public slots:
    //Start the server to listen,  implement the virtual function startWaitingForIO()
    bool listen(quint16 port, const QHostAddress &localAddress= QHostAddress::Any, unsigned int maximumNumberOfPeers = 0xFFF);
    //Close the server
    void close();

    //Call this function after server is listening
    void startWaitingForIOInAnotherThread(unsigned int msecWaitForIOTimeout = 20);

    //Connect to peer
    bool connectToHost(const QHostAddress &address, quint16 port, quint32 *peerID, unsigned int msecTimeout = 5000, quint32 channels = 0);

    //Send data
    bool sendData(quint32 peerID, const QByteArray *byteArray, bool reliable = true, quint8 channel = 0);

    //enet_host_flush
    void flush();

    //Close peer socket
    void disconnectNow(quint32 peerID);
    void disconnect(quint32 peerID);
    void disconnectLater(quint32 peerID);


protected:
    void waitForIO(int msecTimeout = 20);


private slots:
    virtual void processReceivedData(quint32 peerID, const QByteArray &data) = 0;


private:
    ENETProtocolBasePrivate *m_basePrivate;

};

} //namespace HEHUI

#endif // ENETPROTOCOLBASE_H
