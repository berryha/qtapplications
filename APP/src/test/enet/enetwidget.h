#ifndef ENETWidget_H
#define ENETWidget_H

#include <QWidget>

#include "ui_enetwidget.h"

#include "enetprotocol.h"


namespace HEHUI {

class ENETWidget : public QWidget
{
    Q_OBJECT

public:
    ENETWidget(QWidget *parent = 0);
    ~ENETWidget();


private slots:

    void on_toolButtonListen_clicked(){
        listen();
    }
    void on_toolButtonConnect_clicked(){
        connectToPeer();
    }
    void on_toolButtonSend_clicked(){
        send();
    }
    void on_toolButtonClean_clicked(){
        clean();
    }

    void listen();
    void connectToPeer();
    bool startRUDPServer(quint16 port);
    void send();
    void clean();




    void connected(quint32 m_peerID, const QString &address, quint16 peerPort);
    void signalConnectToPeerTimeout(const QHostAddress &peerAddress, quint16 peerPort);
    void disconnected(quint32 m_peerID, const QString &address, quint16 peerPort);

    void dataReceived(const QString &peerAddress, quint16 peerPort, const QByteArray &data);



private:
    Ui::ENETUIClass ui;

    ENETProtocolTest *enetProtocol;
    quint32 m_peerID;

    bool isListening;
    bool isConnected;

    QHostAddress m_peerAddress;
    quint16 m_peerPort;

    quint16 localPort;

    QDateTime startTime, endTime;
    quint64 totalDataSize;
    quint16 m_receivedDataCount;







};
} //namespace HEHUI

#endif // ENETWidget_H
