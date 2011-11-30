#ifndef UDTPROTOCOLTEST_H
#define UDTPROTOCOLTEST_H

#include "HHSharedUDT/hudtprotocolbase.h"
#include "HHSharedNetwork/hpacket.h"

namespace HEHUI {


class UDTProtocolTest : public UDTProtocolBase
{
    Q_OBJECT
public:
    explicit UDTProtocolTest(bool stream = false, const SocketOptions *options = 0, QObject *parent = 0);

signals:
    void packetReceived(Packet *packet);
    void dataReceived(const QString &peerAddress, quint16 peerPort, const QByteArray &data);

public slots:
    bool sendData(UDTSOCKET socket, const QByteArray *byteArray);


private slots:
    void streamDataReceived(UDTSOCKET socket, QByteArray *data) ;
    void messageDataReceived(UDTSOCKET socket, QByteArray *data) ;

    void convertDataToPacket(UDTSOCKET socket, QByteArray *data);


private:




};

} //namespace HEHUI

#endif // UDTPROTOCOL_H
