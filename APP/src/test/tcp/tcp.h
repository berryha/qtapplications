#ifndef TCP_H
#define TCP_H

#include "HHSharedNetwork/htcpbase.h"


namespace HEHUI {

class TCP : public TCPBase
{
    Q_OBJECT
public:
    explicit TCP(QObject *parent = 0);
    
signals:
    void dataReceived(const QString &peerAddress, quint16 peerPort, const QByteArray &data);

public slots:


private:
    void processData(int socketID, const QByteArray &data);



    
};

}

#endif // TCP_H
