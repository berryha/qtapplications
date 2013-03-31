
#include <QtGui>
#include <QApplication>
#include <QSplashScreen>
#include <iostream>

#include "./mainwindow/mainwindow.h"

//#include "login/login.h"
//#include "./networkmanager/clientresourcesmanager.h"

#include "HHSharedCore/hlogdebug.h"


using namespace std;
using namespace HEHUI;

void showSplashMessage(QSplashScreen *s, const QString &str){
    s->showMessage(str, Qt::AlignRight | Qt::AlignBottom, Qt::darkGreen);
}


int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(resources);

    QApplication a(argc, argv);

    cout<<qPrintable(QString(APP_NAME) + " Build " + QString(APP_VERSION))<<endl<<endl;


    for(int i = 0; i < argc; i++){
        if(QString(argv[i]).toLower() == "-log"){
            qInstallMessageHandler(logDebug);
            qAddPostRoutine(closeDebugLog);
        }
    }


    QDate date = QDate::currentDate();
    if(date.year() != 2012 || date.month() > 6){
        QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("Application has expired!"));
        qDebug()<<"Application has expired!";
        return 0;
    }

    a.addLibraryPath(QCoreApplication::applicationDirPath());
    a.addLibraryPath(QCoreApplication::applicationDirPath() + QDir::separator () + QString(PLUGINS_MAIN_DIR));
    a.addLibraryPath(QCoreApplication::applicationDirPath() + QDir::separator () + QString(MYLIBS_DIR));
    qDebug()<<"----main(int argc, char *argv[])~~Library Paths"<<a.libraryPaths ();


    //创建Splash Screen
    //Create Splash Screen
    //QSplashScreen *splash = new QSplashScreen( QPixmap(QString(RESOURCE_PATH)+QString(APP_SPLASH_IMAGE_PATH)) );
    //splash->show();

    //设置程序信息
    //Setup the program information
    //showSplashMessage(splash, QObject::tr("Init Application Info"));
    a.setApplicationName(APP_NAME);
    a.setApplicationVersion(APP_VERSION);
    a.setWindowIcon(QIcon(QString(RESOURCE_PATH)+QString(APP_ICON_PATH)));
    a.setOrganizationName(APP_ORG);
    a.setOrganizationDomain(APP_ORG_DOMAIN);
    //showSplashMessage(splash, QString(APP_NAME) + " " + QString(APP_VERSION));

    //Login
    //	User *user = User::instance();
    //	Login login(user, 0);
    //	if (!login.isVerified()) {
    //		return 0;
    //	}


    //创建主窗口
    //Create the main window
    MainWindow mw;

    //ClientNetworkManager networkManager(&mw, ClientNetworkManager::LAN, ClientNetworkManager::P2P);
    //ClientNetworkManager *networkManager = ClientNetworkManager::instance();
    //networkManager->slotStartUDPCastListening(UDP_MULTICAST_GROUP_IP, UDP_CAST_GROUP_PORT);

    //ChatWindowManager *chatWindowManager = ChatWindowManager::instance();

    //QObject::connect(&mw, SIGNAL(signalUserOnlineStateChanged(UserBase::OnlineState)), networkManager, SLOT(slotChangeUserOnlineState(UserBase::OnlineState)));
    //QObject::connect(&mw, SIGNAL(signalRequestContactInfo(const QString &)), networkManager, SLOT(slotRequestClientInfo(const QString &)));

//    QObject::connect(networkManager, SIGNAL(signalContactOnlineStateChanged(const QString &, UserBase::OnlineState, const QString &, quint16 , const QString &)), &mw, SLOT(slotChangeContactOnlineState(const QString &, UserBase::OnlineState, const QString &, quint16 , const QString &)));
//    QObject::connect(networkManager, SIGNAL(signalContactInfoReceivedFromContact(const QString &, UserBase::OnlineState, const QString &, quint16 , const QString &)), &mw, SLOT(slotUpdateContactInfo(const QString &, UserBase::OnlineState, const QString &, quint16 , const QString &)));

    //QObject::connect(&networkManager, SIGNAL(signalContactOnlineStateChanged(const QString &, UserBase::OnlineState, const QString &, quint16 , const QString &)), &mw, SLOT(slotChangeContactOnlineState(const QString &, UserBase::OnlineState, const QString &, quint16 , const QString &)));
//    QObject::connect(networkManager, SIGNAL(signalChatMessageReceivedFromContact(const QString &, const QString &)), &mw, SLOT(slotProcessChatMessageReceivedFromContact(const QString &, const QString &)));

//    QObject::connect(networkManager, SIGNAL(signalChatMessageReceivedFromContact(const QString &, const QString &)), chatWindowManager, SLOT(slotNewMessageReceivedFromContact(const QString &, const QString &)));



    mw.show();

    //结束Splash Screen
    //Finish the Splash Screen
    //splash->finish(&w);
    //delete splash;

    a.setQuitOnLastWindowClosed(false);

    return a.exec();

}






