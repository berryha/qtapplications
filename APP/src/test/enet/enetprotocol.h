#ifndef ENETPROTOCOLTEST_H
#define ENETPROTOCOLTEST_H

#include "HHSharedENET/henetprotocolbase.h"
#include "HHSharedNetwork/hpacket.h"

namespace HEHUI {


class ENETProtocolTest : public ENETProtocolBase
{
    Q_OBJECT
public:
    explicit ENETProtocolTest(QObject *parent = 0);

signals:
//    void packetReceived(Packet *packet);
    void dataReceived(const QString &peerAddress, quint16 peerPort, const QByteArray &data);

public slots:


private:
    void processReceivedData(quint32 peerID, const QByteArray &data);

    void convertDataToPacket(quint32 peerID, const QByteArray &data);


private:




};

} //namespace HEHUI

#endif // ENETPROTOCOLTEST_H
