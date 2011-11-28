#ifndef UDTPROTOCOL_H
#define UDTPROTOCOL_H

#include "HHSharedUDT/hudtprotocolbase.h"
#include "HHSharedNetwork/hpacket.h"

namespace HEHUI {


class UDTProtocol : public UDTProtocolBase
{
    Q_OBJECT
public:
    explicit UDTProtocol(bool stream = false, const SocketOptions *options = 0, QObject *parent = 0);
    
signals:
    void packetReceived(Packet *packet);
    
public slots:

private slots:
    void streamDataReceived(UDTSOCKET socket, QByteArray *data) ;
    void messageDataReceived(UDTSOCKET socket, QByteArray *data) ;

    void convertDataToPacket(UDTSOCKET socket, QByteArray *data);


private:



    
};

} //namespace HEHUI

#endif // UDTPROTOCOL_H
