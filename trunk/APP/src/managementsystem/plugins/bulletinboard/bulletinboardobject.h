#ifndef BULLETINBOARDOBJECT_H
#define BULLETINBOARDOBJECT_H

#include <QObject>

//#include "systemsummaryinfo.h"
#include "remoteassistance/remoteassistance.h"
#include "bulletinboard/bulletinboardwidget.h"
#include "updatepassword/updatepasswordwidget.h"


#include "networkmanager/bulletinboardpacketsparser.h"
#include "../../sharedms/networkmanager.h"



namespace HEHUI {


class BulletinBoardObject : public QObject
{
    Q_OBJECT
public:
    explicit BulletinBoardObject(QObject *parent = 0);
    ~BulletinBoardObject();
    
    
signals:

    
public slots:
    
private slots:
    void networkReady();

    void adminRequestRemoteAssistancePacketReceived(const QString &adminAddress, quint16 adminPort, const QString &adminName);
    void AdminInformUserNewPasswordPacketReceived(const QString &adminAddress, quint16 adminPort, const QString &adminName, const QString &oldPassword, const QString &newPassword );
    void serverAnnouncementPacketReceived(const QString &adminName, quint32 announcementID, const QString &announcement);
    
    void newPasswordRetreved(const QString &adminAddress, quint16 adminPort);
    
    void peerConnected(const QHostAddress &peerAddress, quint16 peerPort);
    void signalConnectToPeerTimeout(const QHostAddress &peerAddress, quint16 peerPort);
    void peerDisconnected(const QHostAddress &peerAddress, quint16 peerPort, bool normalClose);

    
private:

    bool m_networkReady;

    PacketHandlerBase *m_packetHandler;
    NetworkManagerInstance *networkManager;
    BulletinBoardPacketsParser *bulletinBoardPacketsParser;
    
    RemoteAssistance *remoteAssistance;
    BulletinBoardWidget *bulletinBoardWidget;
    UpdatePasswordWidget *updatePasswordWidget;
    
    quint16 localUDPListeningPort;

    RUDPSocket *rudpSocket;
    
};

} //namespace HEHUI 

#endif // BULLETINBOARDOBJECT_H
