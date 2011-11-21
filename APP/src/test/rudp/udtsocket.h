#ifndef UDTSOCKET_H
#define UDTSOCKET_H

#include <QObject>
#include "HHSharedUDT/hudtprotocolbase.h"

#include "clientpacketsparser.h"
#include "HHSharedNetwork/hnetworkmanagerbase.h"



namespace HEHUI {


class UDTProtocol : public UDTProtocolBase
{
    Q_OBJECT
public:
    explicit UDTProtocol(QObject *parent = 0);
    ~UDTProtocol();


signals:
    void dataReceived(const QString &address, quint16 peerPort, const QByteArray &data);


private slots:
//    void dataReceived(const QString &address, quint16 port, char *data);

    void streamDataReceived(UDTSOCKET socket, const QByteArray &data);
    void messageDataReceived(UDTSOCKET socket, const QByteArray &data);



private:

    NetworkManagerBase *networkManager;
    PacketHandlerBase *m_packetHandlerBase;
    ClientPacketsParser *clientPacketsParser;


//    struct CachedDataInfo{
//        CachedDataInfo(){
//            blockSize = 0;
//            data = 0;
//        }
//        int blockSize;
//        QByteArray *data;

//    };
//    QHash<QString/*IP:Port*/, CachedDataInfo*> m_cachedDataInfo;






};

} //namespace HEHUI

#endif // UDTSOCKET_H
