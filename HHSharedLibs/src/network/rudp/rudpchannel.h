#ifndef RUDPCHANNEL_H
#define RUDPCHANNEL_H

#include <QThread>
#include <QTimer>
#include <QHostAddress>
#include <QUdpSocket>
#include <QMutex>

#include "../mysharedlib_global.h"

#include "rudppacket.h"
#include "../packethandler/packethandlerbase.h"



namespace HEHUI {

class MYSHAREDLIB_API RUDPChannel : public QThread
{
    Q_OBJECT
public:
    enum ChannelState {UnconnectedState, ConnectingState, DisconnectingState, ConnectedState, ListeningState};

    explicit RUDPChannel(QUdpSocket *udpSocket, PacketHandlerBase *packetHandlerBase, QObject *parent = 0);
    RUDPChannel(QUdpSocket *udpSocket, PacketHandlerBase *packetHandlerBase, const QHostAddress &peerAddress, quint16 peerPort, QObject *parent = 0);

    ~RUDPChannel();

    void run();



    void setPeerHostAddress(const QHostAddress &hostAddress){m_peerAddress = hostAddress;}
    QHostAddress getPeerHostAddress() const{return m_peerAddress;}
    void setPeerHostPort(quint16 port){m_peerPort = port;}
    quint16 getPeerHostPort()const{return m_peerPort;}

    bool canSendData(qint64 size);

signals:
    void peerConnected(const QHostAddress &peerAddress, quint16 peerPort);
    void signalConnectToPeerTimeout(const QHostAddress &peerAddress, quint16 peerPort);
    void peerDisconnected(const QHostAddress &peerAddress, quint16 peerPort);
    //void channelClosed();

//    void dataReceived(const QHostAddress &peerAddress, quint16 peerPort, const QByteArray &data);


public slots:
    void connectToPeer(int msecTimeout = RUDP_CONNECTION_TIMEOUT);
    void connectToPeer(const QString &peerAddressString, quint16 peerPort, int msecTimeout = RUDP_CONNECTION_TIMEOUT);
    void connectToPeer(const QHostAddress &peerAddress, quint16 peerPort, int msecTimeout = RUDP_CONNECTION_TIMEOUT);
    bool waitForConnected(int msecTimeout = RUDP_CONNECTION_TIMEOUT);

    void disconnectFromPeer();
    void closeChannel();

    void datagramReceived(QByteArray &block);

    bool sendData( QByteArray &data);
    quint64 sendDatagram(QByteArray *data, bool isReliableDataPacket = true);

private:
    quint16 beginDataTransmission();
    quint64 sendDatagram(QByteArray *data, quint64 offset, bool fragment);
    void endDataTransmission(quint16 fragmentDataID);

    void sendUnreliableDataPacket(QByteArray *data);

    bool isUnreliablePacket(quint8 packetType);



private slots:
    void connectToPeerTimeout();

    void sendHandshakePacket(uint handshakeID);
    void sendPacketDroppedInfo(quint16 packetID);
    void sendKeepAlivePacket();


    void sendToBeSentPackets();
//    bool sendToBeSentPacketDirectly(RUDPPacket *packet);
//    bool sendReTxPacket(RUDPPacket *packet);


    bool tryingToSendPacket(RUDPPacket *packet);


//    void startCheckACKTimer();
//    void checkACKTimerTimeout();

    void startKeepAliveTimer();
    void keepAliveTimerTimeout();

    void startCheckPeerAliveTimer();
    void checkPeerAliveTimerTimeout();

//    void transmissionTimeout();




    void sendACK2(quint16 packetSN);
    bool retransmitLostPacket();
    bool sendPacket(RUDPPacket *packet);

    void startSendACKTimer();
    void sendACKTimerTimeout();

    void startSendNACKTimer();
    void sendNACKTimerTimeout();
    void packLostPacket(QList<quint16> *packets, QDataStream *out);
    void addLostPacketsInReceiverSide(quint16 start, quint16 end);
    void removeLostPacketInReceiverSide(quint16 packetSN);

    void startRetransmissionTimer();
    void retransmissionTimerTimeout();



private:
    void init();
    void reset();
    quint16 createSerialNumber();
    quint16 createSerialNumberForControlPacket();


    void addWaitingForACKPacket(RUDPPacket *packet);
    void removeWaitingForACKPackets(quint16 start, quint16 end);
    void updateFirstWaitingForACKPacketIDInSendWindow(quint16 peerFirstReceivedPacketIDInQueue);

    void processPacket(RUDPPacket *packet);
    void getLostPacketsFromNACK(QList<quint16> *lostPackets, QDataStream *in);

    void cacheData(QByteArray *data);

//    RUDPPacket * takedWaitingForACKPacket(quint16 packetID);

    RUDPPacket * takeToBeSentPacket(quint16 packetID);
    void addToBeSentPacket(RUDPPacket *packet);

    RUDPPacket * getUnusedPacket();
    void recylePacket(RUDPPacket *packet);
    void cleanAllUnusedPackets();


    static void setGlobalSendBufferSize(qint64 size);
    static qint64 getGlobalFreeSendBufferSize();
    static void updateGlobalFreeSendBufferSize(qint64 size, bool reduce = true) ;


private:

    QHostAddress m_peerAddress;
    quint16 m_peerPort;

    quint16 m_MSS;

    int SYN; //10 msecs
    int RTT; //RTT
    int RTTVar; //variance of RTT samples


    //------发送端------
    QTimer *sendPacketTimer; //发送包定时器
    int sendPacketInterval; //包发送周期
    quint16 LSSN; //Last Sent Data packet
    QHash<quint16/*Packet SN*/, RUDPPacket*> m_ToBeSentPacketsHash; //To Be Sent Packets
    QHash<quint16/*Packet SN*/, RUDPPacket*> sentPackets; //Already Sent Packets
    QList<quint16/*Packet SN*/> lostPacketsInSenderSide; //Sender's Loss List
    QList<quint16/*Packet SN*/> waitingForACKPackets; //waiting for ACK

    //-----------------


    //------接收端------
    quint64 maxReceiveBufferSize, m_freeReceiveBufferSize;
    quint16 m_receiveWindowSize;

    quint16 LRSN;

    QTimer *sendACKTimer; //send an ACK
    int sendACKTimerInterval; //10 msecs

    int m_packetArrivalSpeed;
    int m_linkCapacity;

    QTimer *sendNACKTimer; //send a NACK
    int sendNACKTimerInterval; // 4 * RTT+ RTTVar + SYN

    QTimer *retransmissionTimer; //retransmission timer
    int retransmissionTimerInterval;
    int EXPCOUNT; //the number of continuous timeouts
    // EXPCOUNT * (4 * RTT + RTTVar + SYN)
    QDateTime timeEXPCOUNTReset;


//    QHash<quint16/*Packet SN*/, RUDPPacket*> receivedPackets; //接收历史



    struct LostPacketInfo{
        quint16 packetSN;
        int feedbacktimes;
        QDateTime latestFeedbackTime;
    };
    QHash<quint16/*Packet SN*/, LostPacketInfo*> lostPacketsInReceiverSide; //Receiver's Loss List
    QList<quint16/*Packet SN*/> lostPacketsSNInReceiverSide; //Receiver's Loss List

    struct ACKPacketInfo{
        quint16 firstReceivedPacketIDInReceiveWindow;
        QDateTime sentTime;
    };
    QHash<quint16/*Packet SN*/, ACKPacketInfo*> ackPacketsHistory; //ACK History Window
    QList<quint16/*Packet SN*/> ackPacketsSNHistory;
    quint16 largestACK2SN;

//    struct ReceivedDataPacketInfo{
//        quint16 packetSN;
//        QDateTime arrivalTime;
//    };
//    QList<ReceivedDataPacketInfo> receivedDataPacketsHistory; //Received Data Packets History Window, 17 items!
    QList<QDateTime/*Arrival Time*/> receivedDataPacketsHistory; //Received Data Packets History Window, 17 items!

    QList<qint64> probingPacketsInterval; //the time interval between each probing packet pair, 16 items!

    //-----------------



    uint m_handshakeID;

    quint16 m_firstReceivedPacketIDInReceiveWindow;
    QHash<quint16/*Packet SN*/, RUDPPacket*> m_cachedReceivedPacketsHash;

    quint16 m_sendWindowSize;

    static qint64 m_freeSendBufferSize;
    static QMutex *m_freeSendBufferSizeMutex;

    quint16 m_firstWaitingForACKPacketIDInSendWindow;


    quint16 m_receivedFragmentDataPacketID;
    QHash<quint16/*Packet SN*/, RUDPPacket*> m_receivedFragmentDataPackets;

    QList<RUDPPacket *> m_unusedPackets;
    QMutex unusedPacketsMutex;


    QTimer *m_keepAliveTimer;
    int m_keepAliveTimerInterval;
    QDateTime m_peerLastLiveTime;

    QTimer *m_checkPeerAliveTimer;
    int m_checkPeerAliveTimes;

    ChannelState m_ChannelState;

    quint16 m_packetSerialNumber;

    QUdpSocket *m_udpSocket;
    PacketHandlerBase *m_packetHandlerBase;


////    QTimer *m_connectToPeerTimer;
////    int m_msecConnectToPeerTimeout;

//    uint m_handshakeID;
//    //bool m_connected;

//    quint16 m_firstReceivedPacketIDInReceiveWindow;
//    QHash<quint16/*Packet SN*/, RUDPPacket*> m_cachedReceivedPacketsHash;

//    quint16 m_sendWindowSize;
////    quint64 m_sendBufferSize;
//    quint64 m_globalFreeSendBufferSize;
//    QMutex m_globalFreeSendBufferSizeMutex;
////    quint32 m_maxCachedToBeSentPacketsCount;

//    quint16 m_firstWaitingForACKPacketIDInSendWindow;
//    QHash<quint16/*Packet SN*/, RUDPPacket*> m_waitingForACKPacketsHash; //waiting for ACK
//    QList<quint16/*Packet SN*/> m_ackedPackets;


//    QHash<quint16/*Packet SN*/, RUDPPacket*> m_ToBeSentPacketsHash; //waiting for Tx
//    //QList<quint16/*Packet SN*/>  m_ToBeSentPacketQueue;



//    quint16 m_receivedFragmentDataPacketID;
//    QHash<quint16/*Packet SN*/, RUDPPacket*> m_receivedFragmentDataPackets;



//    QList<quint16/*Packet SN*/> m_lastReceivedPackets;





//    QList<RUDPPacket *> m_unusedPackets;
//    QMutex unusedPacketsMutex;



//    QTimer *m_keepAliveTimer;
//    int m_keepAliveTimerInterval;
//    QDateTime m_peerLastLiveTime;
//    QDateTime m_lastPacketSentTime;

//    QTimer *m_checkPeerAliveTimer;
//    int m_checkPeerAliveTimes;

//    ChannelState m_ChannelState;

//    quint16 m_packetSerialNumber;

//    QUdpSocket *m_udpSocket;


    quint32 m_fragmentCount;










};

} //namespace HEHUI

#endif // RUDPCHANNEL_H
