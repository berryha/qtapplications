#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QtGui/QWidget>
#include <QSqlError>
#include <QDialog>
#include <QMessageBox>
#include <QLocalServer>

#include "ui_loginwidget.h"
#include "../imuser.h"
//#include "../packetmanager/clientpacketsparser.h"


//#include "../../shared/core/singleton.h"
//#include "../../shared/core/utilities.h"
//#include "../../shared/gui/databaseconnecter/databaseconnecterdialog.h"
#include "HHSharedCore/hsingleton.h"
#include "HHSharedCore/hutilities.h"
//#include "HHSharedGUI/hdatabaseconnecter.h"


namespace HEHUI {


class LoginWidget: public QWidget {
    Q_OBJECT

public:
    enum State {
        NORMAL, VERIFYING, LOCKED
    };

    LoginWidget(IMUser *u = 0, QWidget *parent = 0);
    ~LoginWidget();

    //	void setUser(IMUser *u) {user = u;};
    //bool isInfoAccepted(){return infoAccepted;};

    State getState() const;

    void switchUI(State state = NORMAL);
    void lockUI(const QString &key = "");

protected:
    void keyPressEvent(QKeyEvent *);
    void languageChange();

private:
    void setupNetworkConfig();
    void loadServers();

    inline QString userName() const;
    inline QString passWord() const;

    inline QString serverAddress();
    inline quint16 serverPort();

    void showNormalUI();
    void showVerifyingUI();
    void showLockedUI();

    void unLockUI(const QString &key);

    bool checkServerAddress();
    bool canLogin();
    void verifyUser();



signals:
    void registration(const QString &serverHostAddress, quint16 serverHostPort, const QString &userID, const QString &password, const QString &email);
    void signalRegistrationResultReceived(quint8 errorTypeCode, const QString &message);

    void signalRequestLogin(const QHostAddress &serverHostAddress, quint16 serverHostPort);
    void signalLookForServer(const QHostAddress &targetAddress, quint16 targetPort);
    void signalUserVerified();
    void signalServerFound(const QString &serverAddress, quint16 serverRTPListeningPort, const QString &serverName, const QString &version);


public slots:

    //        void serverFound(const QString &serverAddress, quint16 serverUDPListeningPort, quint16 serverTCPListeningPort, const QString &serverName, const QString &version);
    void slotProcessLoginResult(quint8 errorTypeCode);

    void slotCheckAutoLogin();

private slots:
    void on_toolButtonApplyForRegistration_clicked();
    void on_invisibleCheckBox_clicked();
    void on_autoLoginCheckBox_clicked();
    void on_toolButtonConfig_clicked(bool checked);
    void on_loginToolButton_clicked();
    void on_cancelToolButton_clicked();

    void on_comboBoxNetworkType_currentIndexChanged(int index);
    void on_comboBoxProtocol_currentIndexChanged(const QString & text);
    void on_comboBoxServerIP_currentIndexChanged(int index);

    void on_toolButtonServersManager_clicked();


    void on_unlockToolButton_clicked();

    void slotServersUpdated();
    void slotServerSelected(const QString &serverInfoString);

    void loginTimeout();

    void slotRegistration(const QString &userID, const QString &password, const QString &email);


private:
    Ui::LoginWidgetClass ui;

    IMUser *user;

    State currentState;
    //bool infoAccepted;

    QString key;

    QMovie *movie;

    bool invisibleLogin;
    bool autoLogin;

    QLocalServer *localServer;

    //ClientPacketsParser *clientPacketsParser;



};

} //namespace HEHUI

#endif // LOGINWIDGET_H
