#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "sharedmslib.h"

#include "HHSharedNetwork/htcpbase.h"
#include "HHSharedNetwork/hpacket.h"

namespace HEHUI {

class SHAREDMSLIB_API TCPServer : public TCPBase
{
    Q_OBJECT
public:
    explicit TCPServer(QObject *parent = 0);

    quint16 getTCPServerListeningPort();

signals:
    void packetReceived(Packet *packet);



private:
    void processData(int socketID, QByteArray *data);



    
};

}

#endif // TCPSERVER_H
