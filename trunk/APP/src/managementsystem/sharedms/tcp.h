#ifndef TCP_H
#define TCP_H

#include "HHSharedNetwork/htcpbase.h"
#include "HHSharedNetwork/hpacket.h"

namespace HEHUI {

class TCP : public TCPBase
{
    Q_OBJECT
public:
    explicit TCP(QObject *parent = 0);


signals:
    void packetReceived(Packet *packet);



private:
    void processData(int socketID, QByteArray *data);



    
};

}

#endif // TCP_H
