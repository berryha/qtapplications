#ifndef UDTPROTOCOL_H
#define UDTPROTOCOL_H

#include "HHSharedNetwork/hpackethandlerbase.h"
#include "HHSharedUDT/hudtprotocolbase.h"


namespace HEHUI {


class UDTProtocol : public UDTProtocolBase
{
    Q_OBJECT
public:
    explicit UDTProtocol(PacketHandlerBase *packetHandlerBase, QObject *parent = 0);
    
signals:
    
public slots:

private slots:
    void streamDataReceived(UDTSOCKET socket, QByteArray *data) ;
    void messageDataReceived(UDTSOCKET socket, QByteArray *data) ;

    void convertDataToPacket(UDTSOCKET socket, QByteArray *data);

private:

    PacketHandlerBase *m_packetHandlerBase;



    
};

} //namespace HEHUI

#endif // UDTPROTOCOL_H
