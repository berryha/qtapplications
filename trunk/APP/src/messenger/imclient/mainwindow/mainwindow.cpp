/*
 *
 *
 *
 *
 */



#include <QtGui>
//#include <QtSql>

#include <QPair>

#include "mainwindow.h"
#include "../about/aboutdialog.h"

#include "../settings.h"
#include "../imageresource.h"

#include "../search/addcontactdialog.h"

#include "../../sharedim/constants_global_shared.h"

//#include "HHSharedCore/hdatabaseutility.h"
//#include "HHSharedGUI/hloginbase.h"
#include "HHSharedGUI/hguiinterface.h"
#include "HHSharedGUI/hpluginmanagerwindow.h"
#include "HHSharedCore/hutilities.h"



namespace HEHUI {


MainWindow::MainWindow(QWidget *parent, HEHUI::WindowPosition positon) :
    MainWindowBase(parent), ItemBoxEventHandler()
{
    
    ui.setupUi(this);

    setWindowTitle(QString(APP_NAME) + " Build " + QString(APP_VERSION));
    //setWindowFlags(Qt::SplashScreen | Qt::WindowStaysOnTopHint);
    //setWindowFlags(Qt::Dialog | Qt::WindowStaysOnTopHint);
    setWindowFlags(Qt::Dialog);


    //使窗口居中
    //Center the window
    //moveWindow(positon);

    //resize(QSize(0,0));
    //showMinimized();



    chatWindowManager = 0;
    //    expandListViewManager = 0;
    friendBox = 0;
    m_userInfoTipWindow = 0;




    //初始化UI
    //Init the UI
    initUI();



    //创建托盘图标
    //Create the system tray
    setupSystemTray();

    if(Settings::instance()->getRestoreWindowStateOnStartup()){
        Settings::instance()->restoreState(this);
    }else{
        //使窗口居中
        //Center the window
        moveWindow(positon);
    }

    loadPlugins();



    contactsManager = ContactsManager::instance();

    imUser = IMUser::instance();
    stateBeforeLocking = IM::ONLINESTATE_OFFLINE;



    //检查自动登陆
    //Check Auto Login
    QTimer::singleShot(100, ui.loginPage, SLOT(slotCheckAutoLogin()));
    /*
  bool invisibleLogin = Settings::instance()->getInvisibleLogin();
 bool autoLogin = Settings::instance()->getAutoLogin();
 if(autoLogin){
  QString uid = Settings::instance()->getRecentUser();
  QByteArray pswd = QByteArray::fromBase64(Settings::instance()->getRecentUserPassword());

  User::instance()->setUserID(uid);
  User::instance()->setPassword(pswd);
  User::instance()->setOnlineState(invisibleLogin ? User::INVISIBLE : User::ONLINE);
  ui.loginPage->verifyUser();

 }
*/



    m_packetHandler = 0;
    m_resourcesManager = 0;
    clientPacketsParser = 0;
    networkStarted = false;

    m_udpServer = 0;
    m_rtp = 0;
    m_socketConnectedToServer = INVALID_SOCK_ID;

    checkNetwork();

    search = 0;

    autoShowSystemMessage = false;
    autoShowChatMessageFromContact = false;



    //m_ContactInfoWidget = new ContactInfoWidget(this);

    m_serverHostAddress = QHostAddress::Null;
    m_serverHostPort = 0;
    //    m_loginTimer = 0;
    m_verified = false;

    m_fileManager = 0;


}

MainWindow::~MainWindow() {
    qDebug()<<"----MainWindow::~MainWindow()";

    /*

 if(User::instance()->getOnlineState() != User::OFFLINE && User::instance()->getOnlineState() != User::INVISIBLE){
                emit signalUserOnlineStateChanged(User::OFFLINE);
                qDebug() << "----MainWindow::~MainWindow()~~User::OFFLINE";
 }

 if(Settings::instance()->getRestoreWindowStateOnStartup()){
  Settings::instance()->saveState(this);
 }

 //关闭所有的数据库连接
 //Close all database connections
 DatabaseUtility::closeAllDBConnections();

 systemTray->hide();

*/


    //    delete m_userInfoTipWindow;
    //    m_userInfoTipWindow = 0;

    deleteLater();

}

void MainWindow::closeEvent(QCloseEvent * event) {
    if (Settings::instance()->getHideOnClose()) {
        showMinimized();
        hide();
        event->ignore();
    }else{

        aboutToQuit();

        event->accept();
    }

}


void MainWindow::changeEvent ( QEvent * event ){
    if(event->type() == QEvent::WindowStateChange){
        switch (windowState ()) {
        case Qt::WindowNoState:
            //ui.actionShowMaximized->setEnabled(true);
            //ui.actionShowFullScreen->setEnabled(true);
            ui.actionShowMinimized->setEnabled(true);
            ui.actionShowNormal->setEnabled(false);
            break;
        case Qt::WindowMinimized:
            //ui.actionShowMaximized->setEnabled(true);
            //ui.actionShowFullScreen->setEnabled(true);
            ui.actionShowMinimized->setEnabled(false);
            ui.actionShowNormal->setEnabled(true);
            break;
        case Qt::WindowMaximized:
            //ui.actionShowMaximized->setEnabled(false);
            //ui.actionShowFullScreen->setEnabled(true);
            ui.actionShowMinimized->setEnabled(true);
            ui.actionShowNormal->setEnabled(true);
            break;
        case Qt::WindowFullScreen:
            //ui.actionShowMaximized->setEnabled(false);
            //ui.actionShowFullScreen->setEnabled(false);
            ui.actionShowMinimized->setEnabled(true);
            ui.actionShowNormal->setEnabled(true);
            break;
        default:
            //TODO: Other State
            break;
        }

        event->accept();
        return;
    }

    event->ignore();

}



void MainWindow::initUI(){

    ui.menuView->addSeparator();
    ui.menuView->addMenu(getStyleMenu(Settings::instance()->getStyle(), Settings::instance()->getPalette()));
    QString qmLocale = Settings::instance()->getLanguage();
    QString qmPath = QApplication::applicationDirPath() + QDir::separator () + QString(LANGUAGE_FILE_DIR);
    ui.menuView->addMenu(getLanguageMenu(qmPath, qmLocale));

    pluginsMenu = getPluginsMenu();
    menuBar()->insertMenu(ui.menuHelp->menuAction(), pluginsMenu);


    connect(ui.actionBugReport, SIGNAL(triggered()), this, SLOT(slotBugReport()));

    connect(ui.actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    connect(ui.actionAbout, SIGNAL(triggered()), this, SLOT(slotAbout()));

    connect(ui.actionHelp, SIGNAL(triggered()), this, SLOT(slotHelp()));

    connect(ui.actionSystemConfigure, SIGNAL(triggered()), this, SLOT(slotSystemConfig()));

    connect(ui.actionQuit, SIGNAL(triggered()), this, SLOT(slotQuit()));

    connect(ui.toolButtonSerach, SIGNAL(clicked()), this, SLOT(slotSearch()));
    connect(ui.lockToolButton, SIGNAL(clicked()), this, SLOT(slotLockUI()));

    ui.stackedWidget->setCurrentWidget(ui.loginPage);
    //登陆页验证完成后的信号处理
    //Process the signal from 'ui.loginPage'
    connect(ui.loginPage, SIGNAL(signalUserVerified()), this, SLOT(slotUserVerified()));

    //changeStyle(Settings::instance()->getStyle());


    chatWindowManager = ChatWindowManager::instance();

    //
    //if(!expandListViewManager){

    //        expandListViewManager = new ExpandListViewManager(this);
    //        connect(expandListViewManager, SIGNAL(signalContactItemActivated(const QString &)), chatWindowManager, SLOT(slotNewChatWithContact(const QString &)));
    //        connect(expandListViewManager, SIGNAL(contextMenuEventOnCategoryOccurs(const QString& ,const QPoint, QMenu*)), this, SLOT(slotContextMenuEventOnCategoryOccurs(const QString& ,const QPoint, QMenu*)));
    //        connect(expandListViewManager, SIGNAL(contextMenuEventOnObjectItemOccurs(const QString& ,const QPoint, QMenu*)), this, SLOT(slotContextMenuEventOnObjectItemOccurs(const QString& ,const QPoint, QMenu*)));
    //        connect(expandListViewManager, SIGNAL(signalTooltipEventOnObjectItemOccurs(const QString& ,const QPoint, const QPoint)), this, SLOT(slotTooltipEventOnObjectItemOccurs(const QString&, const QPoint, const QPoint)));

    //}
    //if(!friendsListView){
    //        friendBox = expandListViewManager->createExpandListView(ui.friendsPage);
    //        m_boxCore = new ItemBoxCore();
    friendBox = new ItemBoxWidget(this, ui.friendsPage);
    ui.friendsPageGridLayout->addWidget(friendBox, 0, 0, 1, 1);
    //}


    m_userInfoTipWindow = new UserInfoTipWindow(this);
    connect(m_userInfoTipWindow, SIGNAL(showUserInfoRequested(IMUserBase*)), this, SLOT(showUserInfo(IMUserBase*)));



}

void MainWindow::checkNetwork(){

    m_packetHandler = 0;
    m_resourcesManager = ClientResourcesManager::instance();
    clientPacketsParser = 0;
    networkStarted = false;

    startNetwork();


}

void MainWindow::startNetwork(){

    if(networkStarted){
        qWarning()<<"Network service has already started!";
        return;
    }



    QString errorMessage = "";
    m_udpServer = m_resourcesManager->startUDPServer(QHostAddress::Any, 0, true, &errorMessage);
    if(!m_udpServer){
        QMessageBox::critical(this, tr("Error"), tr("Can not start UDP listening! %1").arg(errorMessage));
    }else{
        qWarning()<<QString("UDP listening on port %1!").arg(m_udpServer->localPort());
    }

    m_rtp = m_resourcesManager->startRTP(QHostAddress::Any, 0, true, &errorMessage);
    if(!errorMessage.isEmpty()){
        QMessageBox::critical(this, tr("Error"), errorMessage);
    }
    //connect(m_rtp, SIGNAL(connected(int, const QString &, quint16)), this, SLOT(peerConnected(int, const QString &, quint16)));
    connect(m_rtp, SIGNAL(disconnected(int)), this, SLOT(peerDisconnected(int)));



    clientPacketsParser = new IMClientPacketsParser(m_resourcesManager, this);
    //    connect(clientPacketsParser, SIGNAL(signalServerDeclarePacketReceived(const QString&, quint16, const QString&, const QString&)), this, SLOT(serverFound(const QString& ,quint16, const QString&, const QString&)), Qt::QueuedConnection);

    connect(clientPacketsParser, SIGNAL(signalServerDeclarePacketReceived(const QString&, quint16, const QString&, const QString&)), ui.loginPage, SIGNAL(signalServerFound(const QString& , quint16, const QString&, const QString&)), Qt::QueuedConnection);
    connect(ui.loginPage, SIGNAL(requestRegistrationServerInfo()), this, SLOT(requestRegistrationServerInfo()), Qt::QueuedConnection);
    connect(ui.loginPage, SIGNAL(registration()), this, SLOT(requestRegistration()), Qt::QueuedConnection);
    connect(clientPacketsParser, SIGNAL(signalRegistrationServerInfoReceived(quint8, bool, const QString &, quint8, const QString &, bool)), ui.loginPage, SIGNAL(signalRegistrationServerInfoReceived(quint8, bool, const QString &, quint8, const QString &, bool)), Qt::QueuedConnection);
    connect(clientPacketsParser, SIGNAL(signalRegistrationResultReceived(quint8, quint32, const QString&)), ui.loginPage, SIGNAL(signalRegistrationResultReceived(quint8, quint32, const QString&)), Qt::QueuedConnection);
    connect(ui.loginPage, SIGNAL(signalRequestLogin(const QHostAddress &, quint16 )), this, SLOT(requestLogin(const QHostAddress &, quint16)));
    connect(ui.loginPage, SIGNAL(signalLookForServer(const QHostAddress &, quint16 )), clientPacketsParser, SLOT(sendClientLookForServerPacket(const QHostAddress &, quint16)));
    connect(this,SIGNAL(signalMyOnlineStateChanged(int, quint8)), clientPacketsParser, SLOT(changeMyOnlineState(int, quint8)));
    
    connect(clientPacketsParser, SIGNAL(signalUpdatePasswordResultReceived(quint8, const QString&)), this, SLOT(slotProcessUpdatePasswordResult(quint8, const QString&)), Qt::QueuedConnection);
    
    connect(clientPacketsParser, SIGNAL(signalLoginServerRedirected(const QString &, quint16, const QString &)), this, SLOT(slotProcessLoginServerRedirected(const QString &, quint16, const QString &)), Qt::QueuedConnection);
    connect(clientPacketsParser, SIGNAL(signalLoginResultReceived(quint8)), this, SLOT(slotProcessLoginResult(quint8)), Qt::QueuedConnection);
    connect(clientPacketsParser, SIGNAL(signalContactStateChangedPacketReceived(const QString &, quint8, const QString &, quint16)), this, SLOT(slotProcessContactStateChanged(const QString &, quint8, const QString &, quint16)));
    connect(clientPacketsParser, SIGNAL(signalContactsOnlineInfoPacketReceived(const QString & )), this, SLOT(slotProcessContactsOnlineInfo(const QString & )));
    connect(clientPacketsParser, SIGNAL(signalUserInfoPacketReceived(const QString &)), this, SLOT(slotProcessUserInfo(const QString &)), Qt::QueuedConnection);
    connect(clientPacketsParser, SIGNAL(signalContactGroupsInfoPacketReceived(const QString &, quint32 )), this, SLOT(slotProcessContactGroupsInfo(const QString &, quint32 )), Qt::QueuedConnection);

    //connect(clientPacketsParser, SIGNAL(signalSearchContactsResultPacketReceived(const QString &)), this, SLOT(slotProcessSearchContactsResult(const QString &)), Qt::QueuedConnection);
    connect(clientPacketsParser, SIGNAL(signalAddContactRequestFromUserPacketReceived(const QString &, const QString &, const QString &, const QString & )), this, SLOT(slotProcessContactRequestFromUser(const QString &, const QString &, const QString &, const QString & )), Qt::QueuedConnection);
    connect(clientPacketsParser, SIGNAL(signalAddContactResultPacketReceived(const QString &, const QString &, const QString &, quint8, const QString & )), this, SLOT(slotProcessAddContactResult(const QString &, const QString &, const QString &, quint8, const QString &)), Qt::QueuedConnection);

    connect(clientPacketsParser, SIGNAL(signalBlacklistInfoPacketReceived(const QString &, quint32 )), this, SLOT(slotProcessBlacklistInfo(const QString &, quint32 )), Qt::QueuedConnection);

    connect(clientPacketsParser, SIGNAL(signalChatMessageReceivedFromContact(const QString &, const QString &, const QString &)), this, SLOT(slotProcessChatMessageReceivedFromContact(const QString &, const QString &, const QString &)), Qt::QueuedConnection);
    connect(clientPacketsParser, SIGNAL(signalChatMessageCachedOnServerReceived(const QStringList & )), this, SLOT(slotProcessChatMessageCachedOnServer(const QStringList & )), Qt::QueuedConnection);
    connect(clientPacketsParser, SIGNAL(signalChatImageReceived(const QString & )), this, SLOT(slotProcessReceivedChatImage(const QString & )), Qt::QueuedConnection);

    connect(clientPacketsParser, SIGNAL(signalInterestGroupChatMessageReceivedFromContact(quint32, const QString &, const QString &, const QString &)), this, SLOT(slotProcessInterestGroupChatMessagesReceivedFromContact(quint32, const QString &, const QString &, const QString &)), Qt::QueuedConnection);
    connect(clientPacketsParser, SIGNAL(signalInterestGroupChatMessagesCachedOnServerReceived(const QStringList & )), this, SLOT(slotProcessInterestGroupChatMessagesCachedOnServer(const QStringList & )), Qt::QueuedConnection);

    
    connect(clientPacketsParser, SIGNAL(signalInterestGroupsListPacketReceived(const QString &, quint32 )), this, SLOT(slotProcessInterestGroupsList(const QString &, quint32 )), Qt::QueuedConnection);
    connect(clientPacketsParser, SIGNAL(signalInterestGroupInfoPacketReceived(const QString &, quint32 )), this, SLOT(slotProcessInterestGroupInfo(const QString &, quint32 )), Qt::QueuedConnection);
    connect(clientPacketsParser, SIGNAL(signalInterestGroupMembersInfoPacketReceived(const QString &, quint32, quint32 )), this, SLOT(slotProcessInterestGroupMembersInfo(const QString &, quint32, quint32 )), Qt::QueuedConnection);
    
    
    //File TX
    connect(clientPacketsParser, SIGNAL(signalContactRequestUploadFile(int, const QString &, const QByteArray &, const QString &, quint64, const QString &)), this, SLOT(processContactRequestUploadFilePacket(int, const QString &, const QByteArray &, const QString &,quint64, const QString &)), Qt::QueuedConnection);
    connect(clientPacketsParser, SIGNAL(signalContactRequestDownloadFile(int, const QString &, const QString &, const QString &, const QString &)), this, SLOT(processContactRequestDownloadFilePacket(int, const QString &, const QString &, const QString &, const QString &)), Qt::QueuedConnection);
    connect(clientPacketsParser, SIGNAL(signalFileDataRequested(int, const QString &, const QByteArray &, int, int )), this, SLOT(processFileDataRequestPacket(int, const QString &, const QByteArray &, int, int )), Qt::QueuedConnection);
    connect(clientPacketsParser, SIGNAL(signalFileDataReceived(int, const QString &, const QByteArray &, int, const QByteArray &, const QByteArray &)), this, SLOT(processFileDataReceivedPacket(int, const QString &, const QByteArray &, int, const QByteArray &, const QByteArray &)), Qt::QueuedConnection);
    connect(clientPacketsParser, SIGNAL(signalFileTXStatusChanged(int, const QString &, const QByteArray &,quint8)), this, SLOT(processFileTXStatusChangedPacket(int, const QString &, const QByteArray &, quint8)), Qt::QueuedConnection);
    connect(clientPacketsParser, SIGNAL(signalFileTXError(int, const QString &, const QByteArray &, quint8 , const QString &)), this, SLOT(processFileTXErrorFromPeer(int, const QString &, const QByteArray &, quint8 , const QString &)), Qt::QueuedConnection);

    
    connect(chatWindowManager, SIGNAL(signalSendChatMessageToCantact(Contact *, const QString &, const QStringList &)), this, SLOT(slotSendChatMessageToContact(Contact *, const QString &, const QStringList &)));
    connect(chatWindowManager, SIGNAL(signalSendChatMessageToInterestGroup(InterestGroup*, const QString &, const QStringList &)), this, SLOT(slotSendChatMessageToInterestGroup(InterestGroup*, const QString &, const QStringList &)));

    
    
    

    //QObject::connect(&mw, SIGNAL(signalUserOnlineStateChanged(UserBase::OnlineState)), networkManager, SLOT(slotChangeUserOnlineState(UserBase::OnlineState)));
    //QObject::connect(&mw, SIGNAL(signalRequestContactInfo(const QString &)), networkManager, SLOT(slotRequestClientInfo(const QString &)));

    //    QObject::connect(networkManager, SIGNAL(signalContactOnlineStateChanged(const QString &, UserBase::OnlineState, const QString &, quint16 , const QString &)), &mw, SLOT(slotChangeContactOnlineState(const QString &, UserBase::OnlineState, const QString &, quint16 , const QString &)));
    //    QObject::connect(networkManager, SIGNAL(signalContactInfoReceivedFromContact(const QString &, UserBase::OnlineState, const QString &, quint16 , const QString &)), &mw, SLOT(slotUpdateContactInfo(const QString &, UserBase::OnlineState, const QString &, quint16 , const QString &)));

    //QObject::connect(&networkManager, SIGNAL(signalContactOnlineStateChanged(const QString &, UserBase::OnlineState, const QString &, quint16 , const QString &)), &mw, SLOT(slotChangeContactOnlineState(const QString &, UserBase::OnlineState, const QString &, quint16 , const QString &)));
    //    QObject::connect(networkManager, SIGNAL(signalChatMessageReceivedFromContact(const QString &, const QString &)), &mw, SLOT(slotProcessChatMessageReceivedFromContact(const QString &, const QString &)));

    //    QObject::connect(networkManager, SIGNAL(signalChatMessageReceivedFromContact(const QString &, const QString &)), chatWindowManager, SLOT(slotNewMessageReceivedFromContact(const QString &, const QString &)));



    //Single Process Thread
    //QtConcurrent::run(clientPacketsParser, &ClientPacketsParser::run);
    //IMPORTANT For Multi-thread
    //    QThreadPool::globalInstance()->setMaxThreadCount(MIN_THREAD_COUNT);
    //    QtConcurrent::run(clientPacketsParser, &ClientPacketsParser::startparseIncomingPackets);
    //    QtConcurrent::run(clientPacketsParser, &ClientPacketsParser::startprocessOutgoingPackets);


    //TODO:
    //clientPacketsParser->sendClientLookForServerPacket(networkManager->localTCPListeningAddress(), networkManager->localTCPListeningPort(), networkManager->hostName(), getServerLastUsed());
    //QTimer::singleShot(60000, this, SLOT(checkHasAnyServerBeenFound()));


    networkStarted = true;


    //    QString section = serviceName() + "/LastCheckUpdate";
    //    QSettings settings(QCoreApplication::applicationDirPath()+"/.settings", QSettings::IniFormat, this);
    //    QDateTime time = settings.value(section, QDateTime()).toDateTime();
    //    if(time.isNull() || (time.addDays(1) < QDateTime::currentDateTime())){
    //        update();
    //        settings.setValue(section, QDateTime::currentDateTime());
    //    }
    //update();


#if defined(Q_OS_WIN32)
    //                SetProcessWorkingSetSize(GetCurrentProcess(), 0xFFFFFFFF, 0xFFFFFFFF);
    //                SetProcessWorkingSetSize(GetCurrentProcess(), -1, -1);

    //    wm->freeMemory();
#endif

}

void MainWindow::stopNetwork(){

    m_serverHostAddress = QHostAddress::Null;
    m_serverHostPort = 0;

    //    if(m_loginTimer){
    //        m_loginTimer->stop();
    //        delete m_loginTimer;
    //        m_loginTimer = 0;
    //    }

    if(clientPacketsParser){
        clientPacketsParser->logout(m_socketConnectedToServer);
        //QTimer::singleShot(1000, clientPacketsParser, SLOT(aboutToQuit()));
        Utilities::msleep(1000);
        delete clientPacketsParser;
        clientPacketsParser = 0;
    }

    if(m_udpServer){
        m_udpServer->close();
    }

    if(m_rtp){
        m_rtp->stopServers();
    }
    m_contactSocketsHash.clear();


    //    ClientResourcesManager::cleanInstance();
    delete m_resourcesManager;
    m_resourcesManager = 0;

    PacketHandlerBase::clean();

    m_verified = false;


}


void MainWindow::slotInitPlugin(AbstractPluginInterface *plugin){
    qDebug("----MainWindow::slotInitPlugin(AbstractPluginInterface *plugin)");

    if(!plugin){
        return;
    }

    GUIInterface *guiPlugin = static_cast<GUIInterface *> (plugin);
    if(guiPlugin){
        guiPlugin->initialize(this, pluginsMenu, 0, systemTray, APP_NAME, APP_VERSION);
        pluginsMenu->addMenu(guiPlugin->menu());

    }

    //    int pluginsCount = guiPluginsList().size();
    //    if(!pluginsCount){
    //            return;
    //    }
    //
    //
    //
    //    for(int i = 0; i< pluginsCount; i++){
    //            GUIInterface *plugin = guiPluginsList().at(i);
    //            plugin->initialize(this, ui.menuPlugins, 0, systemTray, APP_NAME, APP_VERSION);
    //    }

}




//void MainWindow::loadTranslations() {
//	//动态语言切换,在 QWidget::languageChange() 事件中重新翻译UI
//	//Dynamic language switch, retranslate the UI when QWidget::languageChange() event emitted.

//	qDebug() <<"----MainWindow::loadTranslations()~~Locale Language:"<< QLocale::system().name();

//	//全局变量,在此初始化
//	//Global Variables, init here
//	qmLocale = Settings::instance()->getLanguage();
//	qmPath = QApplication::applicationDirPath() + QDir::separator () + QString(LANGUAGE_FILE_DIR);
//	QDir dir;

//	if (dir.exists(qmPath)) {

//		qDebug() << "----MainWindow::loadTranslations()~~Switch Langague to: " << qmLocale;
//		qApp->installTranslator(&appTranslator);
//		qApp->installTranslator(&qtTranslator);

//		appTranslator.load(LANGUAGE_FILE_PREFIX + qmLocale, qmPath);
//		qtTranslator.load(QString("qt_") + qmLocale, qmPath);


//	} else {

//		dir.mkdir(LANGUAGE_FILE_DIR);
//	}

//	//更新菜单
//	//Update the menus
//	setupMenus();

//}

//void MainWindow::setupMenus() {

//    qDebug()<<"----MainWindow::setupMenus()";


//    QStringList translationLanguages = Utilities::availableTranslationLanguages(qmPath);
//    //如果没有找到语言文件或者首选语言是英语,则将"en_US"置为选中
//    //If language files found or the preferred language is English, set the "en_US" menu checked
//    if (translationLanguages.isEmpty()) {
//            ui.actionLanguageDefaultEnglish->setChecked(true);
//            return;
//    }

//    if (!translationLanguages.contains(qmLocale)){
//            qmLocale = "en_US";
//    }

//    if(qmLocale == "en_US"){
//        ui.actionLanguageDefaultEnglish->setChecked(true);
//    }

//    //为每一个语言文件生成动作为菜单
//    //Make action and menu for each language file
//    for (int i = 0; i < translationLanguages.size(); i++){
//        QString translationLanguage = translationLanguages[i];
//            QAction *action = new QAction(tr("&%1 %2") .arg(i + 1).arg(translationLanguage), this);
//            action->setCheckable(true);
//            action->setData(translationLanguage);

//            ui.menuLanguage->addAction(action);
//            languageActionGroup->addAction(action);

//            if (qmLocale == translationLanguage) {
//                    action->setChecked(true);
//                    action->trigger();
//            }

//    }

//        //更新样式菜单
//        //update Style Menu
//        foreach(QAction *action, ui.menuStyle->actions()){
//                if(action->data().toString() == Settings::instance()->getStyle()){
//                        action->setChecked(true);
//                }
//        }
//        ui.actionUseStylesPalette->setChecked(Settings::instance()->getPalette());


//}


/*

void MainWindow::restoreWindowState() {
 restoreState( Settings::instance()->value( "MainWindow/State" ).toByteArray() );
 QPoint p = Settings::instance()->value( "MainWindow/Position" ).toPoint();
 QSize s = Settings::instance()->value( "MainWindow/Size" ).toSize();
 if ( !p.isNull() && !s.isNull() )
 {
  resize( s );
  move( p );
 }

 if ( Settings::instance()->value( "MainWindow/Maximized", false ).toBool() )
  showMaximized();

 if ( Settings::instance()->value( "MainWindow/Hidden", false ).toBool() )
   hide();

}

void MainWindow::saveWindowState() {

 Settings::instance()->setValue( "MainWindow/Position", pos() );
 Settings::instance()->setValue( "MainWindow/Size", size() );
 Settings::instance()->setValue( "MainWindow/State", saveState() );
 Settings::instance()->setValue( "MainWindow/Hidden", isHidden() );

}

*/



void MainWindow::setupSystemTray() {
    
    
    trayMenu = new QMenu(this);
    trayMenu->addMenu(pluginsMenu);
    trayMenu->addSeparator();
    trayMenu->addAction(ui.actionShowMaximized);
    trayMenu->addAction(ui.actionShowMinimized);
    trayMenu->addAction(ui.actionShowFullScreen);
    trayMenu->addAction(ui.actionShowNormal);
    trayMenu->addSeparator();
    trayMenu->addAction(ui.actionQuit);
    
    
    QIcon defTrayIcon = 	ImageResource::createIcon((QString(RESOURCE_PATH)+QString(APP_ICON_PATH)), "", QIcon::Disabled);
    
    TrayIconData data(STIDT_Unknown, "DefaultSystemTrayIconData");
    data.setToolTip(APP_NAME);
    data.setMenu(trayMenu);
    data.setFirstIcon(defTrayIcon);
    
    systemTray = new SystemTrayIconBase(data, this);
    
    
    //        systemTray = new SystemTrayIconBase(defTrayIcon, this);
    //systemTray->setIcon(QIcon(QString(RESOURCE_PATH)+QString(APP_ICON_PATH)));
    //systemTray->setIcon(ImageResource::createIconSet((QString(RESOURCE_PATH)+QString(APP_ICON_PATH)), "", QIcon::Disabled));
    //systemTray->setToolTip(APP_NAME);
    connect(systemTray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(slotIconActivated(QSystemTrayIcon::ActivationReason)));
    
    
    
    
    //systemTray->setContextMenu(trayMenu);
    
    
    
    systemTray->show();





}

QSystemTrayIcon* MainWindow::SystemTrayIcon() {
    if (!systemTray) {
        setupSystemTray();
    }

    return systemTray;
}

void MainWindow::setTrayIconVisible(bool visible) {
    visible = true;
}

void MainWindow::slotIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    
    TrayIconData data = systemTray->currentTrayIconData();
    int dataTypeCode = data.getDataType();
    SystemTrayIconDataType dataType = STIDT_Unknown;
    if(dataTypeCode > 0){
        dataType = SystemTrayIconDataType(dataTypeCode);
    }
    
    switch(dataType){
    case STIDT_Unknown:
    {
        qWarning()<<"Unknown SystemTrayIconDataType: "<<dataTypeCode;
        //if(reason == QSystemTrayIcon::DoubleClick){
        if(reason != QSystemTrayIcon::Context){
            this->showNormal();
            this->activateWindow();
            this->raise();
        }
    }
        break;
    case STIDT_SystemMessage:
    {

    }
        break;
    case STIDT_ServerAnnouncement:
    {

    }
        break;
    case STIDT_FriendshipApplicationResult:
    {
        if(reason == QSystemTrayIcon::DoubleClick){
            systemTray->removeTrayIconData(data.getID());
            systemTray->updateSystemTrayIcon();

            QString contactID = data.getData().toString();
            getNewContactSettings(contactID);
        }

    }
        break;
    case STIDT_FriendshipApplicationFromContact:
    {
        if(reason == QSystemTrayIcon::DoubleClick){
            systemTray->removeTrayIconData(data.getID());
            systemTray->updateSystemTrayIcon();

            QStringList list = data.getData().toString().split(QChar(PACKET_DATA_SEPARTOR));
            if(list.size() == 4){
                showContactRequestFromUser(list.at(0), list.at(1), list.at(2), list.at(3));
            }

        }

    }
        break;
    case STIDT_ContactChatMessage:
    {
        QString contactID = data.getID();
        systemTray->removeTrayIconData(contactID);
        systemTray->updateSystemTrayIcon();

        QHash<QString/*Time*/, QVariant/*Message*/> hashData = data.getData().toHash();
        QStringList times = hashData.keys();
        times.sort();
        foreach(QString time, times){
            //qDebug()<<"contactID:"<<contactID<<" Time:"<<time<<" Msg:"<<hashData.value(time).toString();
            chatWindowManager->slotNewMessageReceivedFromContact(contactID, hashData.value(time).toString(), time);
        }


    }
        break;

    case STIDT_InterestGroupChatMessage:
    {
        quint32 interestGroupID = data.getID().toUInt();
        systemTray->removeTrayIconData(data.getID());
        systemTray->updateSystemTrayIcon();

        //QHash<QString/*Time*/, QStringList/*Contact ID, Message*/ > hashData = data.getData().toHash();
        QHash<QString/*Time*/, QVariant/*Contact ID, Message*/ > hashData = data.getData().toHash();

        QStringList times = hashData.keys();
        times.sort();
        foreach(QString time, times){
            QStringList list = hashData.value(time).toStringList();
            chatWindowManager->slotNewMessageReceivedFromInterestGroup(interestGroupID, list.at(0), list.at(1), time);
        }

    }

    default:
        qWarning()<<"Unknown SystemTrayIconDataType: "<<dataTypeCode;
        //this->showNormal();



    }
    
    
    //     switch (reason) {
    //     case QSystemTrayIcon::Trigger:

    //     case QSystemTrayIcon::DoubleClick:
    //     {

    //     }
    //         break;
    //     case QSystemTrayIcon::MiddleClick:
    //    	 slotAbout();
    //         break;
    //     default:
    //         ;
    //     }


}



//void MainWindow::slotChangeStyle(QAction * action) {
//	//更新样式
//	//Update the app style
//	preferedStyle = action->data().toString();
//	//QApplication::setStyle(preferedStyle);
//	changeStyle(preferedStyle);
//	//slotChangePalette(ui.actionUseStylesPalette->isChecked());
//	if (!action->isChecked()) {
//		action->setChecked(true);
//	}


//	Settings::instance()->setStyle(action->data().toString());
//}

//void MainWindow::changeStyle(const QString &style) {

//	QApplication::setStyle(style);

//	slotChangePalette(Settings::instance()->getPalette());
//	//保存首选样式
//	//Save the preferred style
//	Settings::instance()->setStyle(style);

//}

//void MainWindow::slotChangePalette(bool useStylePalette) {
//	if (useStylePalette) {
//		QApplication::setPalette(QApplication::style()->standardPalette());
//	} else {
//		QApplication::setPalette(originalPalette);
//	}

//	Settings::instance()->setPalette(useStylePalette);
//}

//void MainWindow::slotChangeLanguage(QAction *action) {

//	QString lang = action->data().toString();
//	if (lang.isEmpty()){
//		lang = "en_US";
//	}

//	QString langFile = QString(LANGUAGE_FILE_PREFIX) + lang + QString(".qm");
//	appTranslator.load(langFile, qmPath);
//	qDebug()<<"----MainWindow::slotChangeLanguage(...)~~Loading language file :"<<langFile ;
//	langFile = "qt_" + lang + QString(".qm");
//	qtTranslator.load(langFile, qmPath);
//	qDebug()<<"----MainWindow::slotChangeLanguage(...)~~Loading language file :"<<langFile ;

//	if (!action->isChecked()) {
//		action->setChecked(true);
//	}


//	preferedLanguage = lang;
//	Settings::instance()->setLanguage(lang);

//}



/*
void MainWindow::slotSystemInfo() {
 //如果窗口已打开,
 //If the window has already been opened, activate it
 //if(SystemInfo::isRunning() == true){
 //     foreach (QMdiSubWindow *window, ui.mdiArea->subWindowList()) {
 //    	 SystemInfo *systemInfo = qobject_cast<SystemInfo *>(window->widget());
 //         if (systemInfo)
 //        	 ui.mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
 //             return ;
 //     }

 //	return;
 //}

 //SystemInfo *systemInfo = new SystemInfo(this);
 //ui.mdiArea->addSubWindow(systemInfo, Qt::Dialog);
 //systemInfo->show();

 QMessageBox::information(this,tr("~_~"),tr(" Not accomplished !"));
 qDebug() << "slotSystemInfo";


}




void MainWindow::slotUserManager(){
 //如果窗口已打开,
 //If the window has already been opened, activate it
 //if(UserManagerMainWindow::isRunning() == true){

 //     foreach (QMdiSubWindow *window, ui.mdiArea->subWindowList()) {
 //    	 UserManagerMainWindow *userManager = qobject_cast<UserManagerMainWindow *>(window->widget());
 //         if (userManager)
 //        	 ui.mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
 //             return ;
 //     }

 //	return;
 //}



 User user;
 LoginBase login(&user);
 if (!login.isVerified()) {
  return ;
 }


 //UserManagerMainWindow *userManager = new UserManagerMainWindow(this);
 //ui.mdiArea->addSubWindow(userManager, Qt::Dialog);
 //userManager->show();

 QMessageBox::information(this,tr("~_~"),tr(" Not accomplished !"));
 qDebug() << "slotUserManager";

}

void MainWindow::slotSqlExplorer(){
 //如果窗口已打开,
 //If the window has already been opened, activate it
 //if(SqlExplorer::isRunning() == true){
 //     foreach (QMdiSubWindow *window, ui.mdiArea->subWindowList()) {
 //    	 SqlExplorer *sqlExplorer = qobject_cast<SqlExplorer *>(window->widget());
 //         if (sqlExplorer)
 //        	 ui.mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
 //             return ;
 //     }
//
 //	return;
 //}


 User user;
 LoginBase login(&user);
 if (!login.isVerified()) {
  return ;
 }

 //SqlExplorer *sqlExplorer = new SqlExplorer(this);
 //ui.mdiArea->addSubWindow(sqlExplorer, Qt::Dialog);
 //sqlExplorer->show();
 QMessageBox::information(this,tr("~_~"),tr(" Not accomplished !"));
 qDebug() << "slotSqlExplorer";

}
*/



void MainWindow::slotSystemConfig() {
    QMessageBox::information(this,tr("~_~"),tr(" Not accomplished !"));
    qDebug() << "----MainWindow::slotSystemConfig()";
}

void MainWindow::slotBugReport() {
    QMessageBox::information(this,tr("~_~"),tr(" Not accomplished !"));
    qDebug() << "----MainWindow::MainWindow::slotSystemConfig()()";
}

void MainWindow::slotHelp() {
    QMessageBox::information(this,tr("~_~"),tr(" Not accomplished !"));
    qDebug() << "----MainWindow::slotHelp()";

}

void MainWindow::slotAbout() {
    AboutDialog aboutDlg(this);
    aboutDlg.exec();

}

void MainWindow::slotQuit() {
    qDebug() << "----MainWindow::slotQuit()";

    aboutToQuit();

    QTimer::singleShot(1000, qApp, SLOT(quit()));
    //qApp->quit();

}

void MainWindow::retranslateUi() {

    //重新翻译UI
    //Retranslate UI
    ui.retranslateUi(this);


}


void MainWindow::aboutToQuit(){
    qDebug() << "----MainWindow::aboutToQuit()";


    if(imUser->getOnlineState() != IM::ONLINESTATE_OFFLINE && imUser->getOnlineState() != IM::ONLINESTATE_INVISIBLE){
        imUser->setOnlineState(IM::ONLINESTATE_OFFLINE);
        emit signalMyOnlineStateChanged(m_socketConnectedToServer, quint8(IM::ONLINESTATE_OFFLINE));
        qDebug() << "----MainWindow::doWorkbeforeQuit()~~IMUser::OFFLINE";
    }

    imUser->saveMyInfoToLocalDatabase();

    if(Settings::instance()->getRestoreWindowStateOnStartup()){
        Settings::instance()->saveState(this);
    }



    //关闭所有的数据库连接
    //Close all database connections
    DatabaseUtility::closeAllDBConnections();

    systemTray->hide();

    stopNetwork();

    //TODO:清理


}

void MainWindow::savePreferedStyle(const QString &preferedStyle, bool useStylePalette){
    Settings::instance()->setStyle(preferedStyle);
    Settings::instance()->setPalette(useStylePalette);

}

void MainWindow::savePreferedLanguage(const QString &preferedLanguage){
    Settings::instance()->setLanguage(preferedLanguage);

}

void MainWindow::slotUpdateContactsInfo(){
    qDebug()<<"----MainWindow::slotUpdateContactsInfo()";

    //    if(!expandListViewManager){
    //        expandListViewManager = new ExpandListViewManager(this);
    //        //TODO
    //        connect(expandListViewManager, SIGNAL(signalContactItemActivated(const QString &)), chatWindowManager, SLOT(slotNewChatWithContact(const QString &)));
    //        connect(expandListViewManager, SIGNAL(contextMenuEventOnCategoryOccurs(const QString& ,const QPoint, QMenu*)), this, SLOT(slotContextMenuEventOnCategoryOccurs(const QString& ,const QPoint, QMenu*)));
    //        connect(expandListViewManager, SIGNAL(contextMenuEventOnObjectItemOccurs(const QString& ,const QPoint, QMenu*)), this, SLOT(slotContextMenuEventOnObjectItemOccurs(const QString& ,const QPoint, QMenu*)));
    //        connect(expandListViewManager, SIGNAL(signalTooltipEventOnObjectItemOccurs(const QString& ,const QPoint, const QPoint)), this, SLOT(slotTooltipEventOnObjectItemOccurs(const QString&, const QPoint, const QPoint)));
    //    }
    //    if(!friendsListView){
    //        friendsListView = expandListViewManager->createExpandListView(ui.friendsPage);
    //        ui.friendsPageGridLayout->addWidget(friendsListView, 0, 0, 1, 1);
    //    }

    //contactsManager = new ContactsManager(this);
    //if(!contactsManager){
    //    contactsManager = ContactsManager::instance();
    //}
    contactsManager->slotFetchContactsInfo(friendBox);
    contactsManager->slotFetchStrangersInfo();

    ui.contactsToolBox->setEnabled(true);
    ui.stackedWidget->setCurrentWidget(ui.mainPage);

    setWindowTitle(imUser->getUserID());
    //ui.toolButtonUserFace->setIcon(ImageResource::createIconForContact(imUser->getFace(), true));
    //setWindowIcon(HEHUI::ImageResourceBase::createIconSet(IMUser::instance()->getHeadPortrait()));
    //setWindowIcon(HEHUI::ImageResourceBase::createIconSet(":/resources/images/locked.png"));


}

void MainWindow::slotUpdateBlacklistInfo(){
    ui.listWidgetBlacklist->clear();

    QStringList blacklist = imUser->getBlacklist();
    foreach (QString contactID, blacklist) {
        if(contactID.trimmed().isEmpty()){continue;}
        Contact *contact = 0;
        QString face = "", nickName = contactID;
        if(contactsManager->hasUserInfo(contactID)){
            contact = contactsManager->getUser(contactID);
            face = contact->getFace();
            nickName = contact->getNickName();
        }

        QListWidgetItem *item = new QListWidgetItem(ImageResource::createIconForContact(face), nickName, ui.listWidgetBlacklist);
        item->setData(Qt::UserRole, contactID);
        ui.listWidgetBlacklist->addItem(item);
    }


}

void MainWindow::updateAllInterestGroupsInfoToUI(){
    
    ui.listWidgetGroups->clear();
    
    QList<InterestGroup *> interestGroups = contactsManager->getInterestGroupsList();
    QIcon ico = ImageResource::createIconForInterestGroup();
    foreach (InterestGroup *group, interestGroups) {
        QListWidgetItem *item = new QListWidgetItem(ico, group->getGroupName(), ui.listWidgetGroups);
        item->setData(Qt::UserRole, group->getGroupID());
        ui.listWidgetGroups->addItem(item);
    }
    
    connect(ui.listWidgetGroups, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(interestGroupItemActivated(QListWidgetItem*)));

    
}

void MainWindow::updateInterestGroupInfoToUI(InterestGroup *interestGroup){

    if(!interestGroup){
        return;
    }
    quint32 groupID = interestGroup->getGroupID();
    
    for(int i=0; i<ui.listWidgetGroups->count(); i++){
        QListWidgetItem *item = ui.listWidgetGroups->item(i);
        if(!item){continue;}
        if(item->data(Qt::UserRole).toUInt() == groupID){
            item->setText(interestGroup->getGroupName());
            break;
        }
    }
    
}

void MainWindow::slotUserVerified(){
    qDebug()<<"--MainWindow::slotUserVerified()";

    if(ui.loginPage->getState() == LoginWidget::VERIFYING){
        m_verified = true;
        QString userID = imUser->getUserID();
        Settings::instance()->setCurrentUser(userID);


        slotUpdateContactsInfo();
        slotUpdateBlacklistInfo();
        

        imUser->setOnlineState(imUser->getStateAfterLoggedin());
        //systemTray->resetTrayIcon(ImageResource::createIcon((QString(RESOURCE_PATH)+QString(APP_ICON_PATH)), "", QIcon::Normal));
        systemTray->resetTrayIcon(ImageResource::createMixedIcon((QString(RESOURCE_PATH)+QString(APP_ICON_PATH)), imUser->getOnlineState()));

        systemTray->resetToolTip(userID);
        //systemTray->setIcon(ImageResource::createIcon((QString(RESOURCE_PATH)+QString(APP_ICON_PATH)), "", QIcon::Normal));
        //systemTray->setToolTip(imUser->getUserID());
        
        ui.toolButtonUserFace->setIcon(ImageResource::createIconForContact(imUser->getFace(), IM::ONLINESTATE_ONLINE));
        //ui.toolButtonUserFace->setIcon(ImageResource::createMixedIcon((QString(RESOURCE_PATH)+QString(APP_ICON_PATH)), imUser->getOnlineState()));

        ui.labelUserNickName->setText(imUser->getNickName());
        ui.labelUserID->setText(userID);
        
        //        clientPacketsParser->startHeartbeat();

        //contactsManager = ContactsManager::instance();
        
        contactsManager->loadInterestGroups();
        updateAllInterestGroupsInfoToUI();
        
        
        
    }else{
        //解锁时恢复在线状态
        //Restore user online state after unlocking
        ui.stackedWidget->setCurrentWidget(ui.mainPage);
        imUser->setOnlineState(stateBeforeLocking);
        
        //TODO:是否要放在外面
        if(imUser->getOnlineState() != IM::ONLINESTATE_INVISIBLE){
            //systemTray->resetTrayIcon(ImageResource::createIcon((QString(RESOURCE_PATH)+QString(APP_ICON_PATH)), "", QIcon::Normal));
            systemTray->resetTrayIcon(ImageResource::createMixedIcon((QString(RESOURCE_PATH)+QString(APP_ICON_PATH)), imUser->getOnlineState()));

            emit signalMyOnlineStateChanged(m_socketConnectedToServer, quint8(imUser->getOnlineState()));
        }
    }

    ui.loginPage->switchUI(LoginWidget::NORMAL);
    

}


void MainWindow::slotLockUI(){
    ui.loginPage->lockUI();
    ui.stackedWidget->setCurrentWidget(ui.loginPage);

    stateBeforeLocking = imUser->getOnlineState();
    if(stateBeforeLocking != IM::ONLINESTATE_INVISIBLE && stateBeforeLocking != IM::ONLINESTATE_AWAY){
        imUser->setOnlineState(IM::ONLINESTATE_AWAY);
        emit signalMyOnlineStateChanged(m_socketConnectedToServer, quint8(IM::ONLINESTATE_AWAY));
    }

}


void MainWindow::handleItemActivated(const QString &id){
    chatWindowManager->slotNewChatWithContact(id);
}

void MainWindow::handleContextMenuEventOnCategory(const QString &group_name, const QPoint &global_mouse_pos, QMenu *contextMenu){
    qDebug()<<"--MainWindow::handleContextMenuEventOnCategory(...)";

    m_userInfoTipWindow->hideUserInfoTip();

    if(contextMenu){
        contextMenu->addSeparator();
        QAction actionRenameGroupName(tr("Rename Group"), contextMenu);
        QAction actionDeleteGroup(tr("Delete Group"), contextMenu);
        QAction actionCreateNewGroup(tr("Create New Group"), contextMenu);
        
        
        contextMenu->addAction(&actionRenameGroupName);
        
        if(contactsManager->getContactGroupMembers(group_name).isEmpty()){
            contextMenu->addAction(&actionDeleteGroup);
        }
        
        contextMenu->addAction(&actionCreateNewGroup);
        

        QAction *action = contextMenu->exec(global_mouse_pos);
        if(action == &actionRenameGroupName){
            bool ok = false;
            QString labelText = tr("New Name:\n(Only word-characters up to 16 are acceptable!)");
            QString newGroupName = QInputDialog::getText(this, tr("Rename Group"),
                                                         labelText, QLineEdit::Normal,
                                                         group_name, &ok);
            if (ok && !newGroupName.isEmpty()){
                int pos = 0;
                QRegExpValidator rxValidator(this);
                QRegExp rx("\\b\\w{0,16}\\b");
                rxValidator.setRegExp(rx);
                if(rxValidator.validate(newGroupName, pos) != QValidator::Acceptable){
                    QMessageBox::critical(this, tr("Error"), tr("Invalid Group Name!"));
                    return ;
                }
                //TODO:
                //imUser->updateGroupName(group_name, text);
                imUser->saveMyInfoToLocalDatabase();
                
                if(contactsManager->contactGroups().contains(newGroupName, Qt::CaseInsensitive)){
                    QMessageBox::critical(this, tr("Error"), tr("Group already exists!"));
                    return;
                }

                clientPacketsParser->renameContactGroup(m_socketConnectedToServer, group_name, newGroupName);

                contactsManager->renameGroupToDatabase(group_name, newGroupName);
                contactsManager->renameGroupToUI(friendBox, group_name, newGroupName);
            }


        }else if(action == &actionDeleteGroup){
            //if(!contactsManager->getContactGroupMembers(group_name).isEmpty()){
            //    QMessageBox::critical(this, tr("Error"), tr("The group still contains at least one member!"));
            //    return;
            //}
            //TODO:

            clientPacketsParser->createOrDeleteContactGroup(m_socketConnectedToServer, group_name, false);

            contactsManager->deleteGroupFromDatabase(group_name);
            //TODO?
            //imUser->saveMyInfoToLocalDatabase();

            contactsManager->slotDeleteContactGroupFromUI(friendBox, group_name);
            

        }else if(action == &actionCreateNewGroup){

            bool ok = false;
            QString labelText = tr("Group Name:\n(Only word-characters up to 16 are acceptable!)");
            QString newGroupName = QInputDialog::getText(this, tr("Create New Group"),
                                                         labelText, QLineEdit::Normal,
                                                         "", &ok);
            if (ok && !newGroupName.isEmpty()){
                int pos = 0;
                QRegExpValidator rxValidator(this);
                QRegExp rx("\\b\\w{0,16}\\b");
                rxValidator.setRegExp(rx);
                if(rxValidator.validate(newGroupName, pos) != QValidator::Acceptable){
                    QMessageBox::critical(this, tr("Error"), tr("Invalid Group Name!"));
                    return ;
                }
                //TODO:
                imUser->saveMyInfoToLocalDatabase();
                if(contactsManager->contactGroups().contains(newGroupName, Qt::CaseInsensitive)){
                    QMessageBox::critical(this, tr("Error"), tr("Group already exists!"));
                    return;
                }

                clientPacketsParser->createOrDeleteContactGroup(m_socketConnectedToServer, newGroupName, true);

                int groupID = contactsManager->slotAddNewContactGroupToDatabase(newGroupName);
                contactsManager->slotAddNewContactGroupToUI(friendBox, groupID, newGroupName);
            }
            
        }

    }

}

void MainWindow::handleContextMenuEventOnItem(const QString &contactID, const QPoint &global_mouse_pos, QMenu *contextMenu){
    qDebug()<<"--MainWindow::handleContextMenuEventOnItem(...)";

    m_userInfoTipWindow->hideUserInfoTip();

    if(contactID.trimmed().isEmpty()){
        return;
    }

    QMenu menu;

    QMenu *menuMoveContactToGroup = menu.addMenu(tr("Move To"));
    QStringList groups = imUser->contactGroups();
    QString existingGroupName = imUser->groupNameThatContactBelongsTo(contactID);
    groups.removeAll(existingGroupName);
    if(!groups.isEmpty()){
        foreach (QString group, groups) {
            QAction *action = new QAction(group, menuMoveContactToGroup);
            action->setData(contactID);
            connect(action, SIGNAL(triggered()), this, SLOT(slotMoveContactToGroup()));
            menuMoveContactToGroup->addAction(action);
        }
        menuMoveContactToGroup->addSeparator();
    }
    QAction actionMoveToBlacklist(tr("Blacklist"), menuMoveContactToGroup);
    actionMoveToBlacklist.setData(contactID);
    connect(&actionMoveToBlacklist, SIGNAL(triggered()), this, SLOT(slotMoveContactToBlacklist()));
    menuMoveContactToGroup->addAction(&actionMoveToBlacklist);


    QAction actionDeleteContact(tr("Delete"), &menu);
    //actionMoveToBlacklist.setData(contactID);
    //connect(&actionDeleteContact, SIGNAL(triggered()), this, SLOT(slotDeleteContact()));
    menu.addAction(&actionDeleteContact);

    QAction *executedAction = 0;
    if(contextMenu){
        contextMenu->addMenu(&menu);
        executedAction = contextMenu->exec(global_mouse_pos);
    }else{
        executedAction = menu.exec(global_mouse_pos);
    }

    if(executedAction == &actionDeleteContact){
        //TODO

        slotDeleteContact(contactID, existingGroupName);

        //        Contact *contact = 0;
        //        if(imUser->hasContact(contactID)){
        //            contact = contactsManager->getUser(contactID);
        //        }else{
        //            QMessageBox::critical(this, tr("Error"), tr("Contact '%1' does not exist!").arg(contactID));
        //            return;
        //        }

        //        imUser->addOrDeleteContact(contactID, existingGroupName, false);
        //        imUser->saveMyInfoToLocalDatabase();

        //        int groupID = contactsManager->getGroup(existingGroupName);
        //        if(!groupID){
        //            QMessageBox::critical(this, tr("Error"), tr("Contact group '%1' does not exist!").arg(existingGroupName));
        //            return;
        //        }

        //        contactsManager->slotdeleteContactFromDatabase(contact);
        //        contactsManager->deleteContactFromUI(friendsListView, existingGroupName, contactID);

        //        clientPacketsParser->deleteContact(contactID, existingGroupName, true);



    }


}

void MainWindow::handleTooltipEventOnItem(const QString &contactID, const QPoint &global_item_topLeft_pos, const QPoint &global_mouse_pos){

    qDebug()<<"--MainWindow::handleTooltipEventOnItem()--contactID:"<<contactID;

    //    if(!m_userInfoTipWindow){
    //        m_userInfoTipWindow = new UserInfoTipWindow(this);
    //    }


//    Contact *contact = 0;
//    if(imUser->hasFriendContact(contactID)){
//        contact = contactsManager->getUser(contactID);
//    }else{
//        m_userInfoTipWindow->hideUserInfoTip();
//        return;
//    }
    Contact *contact = contactsManager->getUser(contactID);
    if(!contact){
        m_userInfoTipWindow->hideUserInfoTip();
        return;
    }


    //QString tip = QString("<b><h4>ID:%1</h4></b>").arg(contactID);
    //QToolTip::showText(global_mouse_pos, tip);


    QSize userInfoTipWindowSize = m_userInfoTipWindow->size();
    QPoint p = ui.contactsToolBox->mapToGlobal(QPoint(0,0));
    int x = p.x()-userInfoTipWindowSize.width();
    if(x < 0){
        x = p.x() + ui.contactsToolBox->width();
    }

    //    qDebug()<<"global_item_topLeft_pos:x:"<<global_item_topLeft_pos.x()<<" y:"<<global_item_topLeft_pos.y();
    //    qDebug()<<"global_mouse_pos:x:"<<global_mouse_pos.x()<<" y:"<<global_mouse_pos.y();
    //    qDebug()<<"x:"<<x<<" y:"<<global_item_topLeft_pos.y();

    m_userInfoTipWindow->showUserInfoTip(contact, mapTo(this, QPoint(x, global_item_topLeft_pos.y())) );

    activateWindow();
    raise();


}

void MainWindow::slotMoveContactToGroup(){
    QAction *action = qobject_cast<QAction *>(sender());
    if(!action){return;}
    //QMessageBox::information(0, "slotRenameGroupName", action->data().toString());

    QString newGroupName = action->text();
    QString contactID = action->data().toString();
    Contact *contact = 0;
    if(imUser->hasFriendContact(contactID)){
        contact = contactsManager->getUser(contactID);
    }else{
        QMessageBox::critical(this, tr("Error"), tr("Contact '%1' does not exist!").arg(contactID));
        return;
    }

    QString existingGroupName = imUser->groupNameThatContactBelongsTo(contactID);
    if(existingGroupName == newGroupName){return;}

    imUser->moveContact(contactID, existingGroupName, newGroupName);
    imUser->saveMyInfoToLocalDatabase();

    int groupID = contactsManager->getPersonalContactGroupID(newGroupName);
    if(!groupID){
        QMessageBox::critical(this, tr("Error"), tr("Contact group '%1' does not exist!").arg(newGroupName));
        return;
        //groupID = contactsManager->slotAddNewContactGroupToDatabase(groupName);
        //contactsManager->slotAddNewContactGroupToUI(friendsListView, groupID, groupName);
    }

    contact->setContactGroupID(groupID);
    contactsManager->saveContactInfoToDatabase(contactID);


    clientPacketsParser->moveContactToGroup(m_socketConnectedToServer, contactID, existingGroupName, newGroupName);

    contactsManager->moveContact(contactID, contactsManager->getPersonalContactGroupID(existingGroupName), groupID);

    contactsManager->moveContactToUI(friendBox, existingGroupName, newGroupName, contactID);

    action->disconnect();
    action->deleteLater();
}

void MainWindow::slotMoveContactToBlacklist(){
    QAction *action = qobject_cast<QAction *>(sender());
    if(!action){return;}

    //QString newGroupName = action->text();
    QString contactID = action->data().toString();
    Contact *contact = 0;
    if(imUser->hasFriendContact(contactID)){
        contact = contactsManager->getUser(contactID);
    }else{
        QMessageBox::critical(this, tr("Error"), tr("Contact '%1' does not exist!").arg(contactID));
        return;
    }


    QString existingGroupName = imUser->groupNameThatContactBelongsTo(contactID);

    slotDeleteContact(contactID, existingGroupName);
    imUser->addOrDeleteBlacklistedContact(contactID, true);

    clientPacketsParser->addOrDeleteBlacklistedContact(m_socketConnectedToServer, contactID, true);

    //TODO:
    QListWidgetItem *item = new QListWidgetItem(ImageResource::createIconForContact(contact->getFace()), contact->getNickName(), ui.listWidgetBlacklist);
    item->setData(Qt::UserRole, contactID);
    ui.listWidgetBlacklist->addItem(item);


    imUser->saveMyInfoToLocalDatabase();

    action->disconnect();
    action->deleteLater();
}

void MainWindow::slotDeleteContact(const QString &contactID, const QString &existingGroupName){

    Contact *contact = 0;
    if(imUser->hasFriendContact(contactID)){
        contact = contactsManager->getUser(contactID);
    }else{
        QMessageBox::critical(this, tr("Error"), tr("Contact '%1' does not exist!").arg(contactID));
        return;
    }

    imUser->addOrDeleteContact(contactID, existingGroupName, false);
    //imUser->saveMyInfoToLocalDatabase();

    int groupID = contactsManager->getPersonalContactGroupID(existingGroupName);
    if(!groupID){
        QMessageBox::critical(this, tr("Error"), tr("Contact group '%1' does not exist!").arg(existingGroupName));
        return;
    }

    //contactsManager->slotdeleteContactFromDatabase(contact);
    contact->setContactGroupID(0);
    contactsManager->saveContactInfoToDatabase(contactID);
    clientPacketsParser->deleteContact(m_socketConnectedToServer, contactID, existingGroupName, true);

    contactsManager->addOrDeleteContact(contactID, groupID, false);
    //contactsManager->saveContactGroupsInfoToDatabase();


    contactsManager->deleteContactFromUI(friendBox, existingGroupName, contactID);


}


void MainWindow::slotProcessUpdatePasswordResult(quint8 errorTypeCode, const QString &message){
    
    QMessageBox::critical(this, tr("Error"), tr("Password Update Failed!"));
}

void MainWindow::slotProcessLoginServerRedirected(const QString &serverAddress, quint16 serverPort, const QString &serverName){
    //TODO
    QMessageBox::information(this, tr("Redirected"), tr("Redirected to %1:%2").arg(serverAddress).arg(serverPort));

    requestLogin(QHostAddress(serverAddress), serverPort);

}

void MainWindow::slotProcessLoginResult(quint8 errorTypeCode){
    //TODO:处理登陆结果
    //    QMessageBox::information(this, "A", "slotProcessLoginResult");
    ui.loginPage->slotProcessLoginResult(errorTypeCode);
    //    QMessageBox::information(this, "B", "slotProcessLoginResult");

}

void MainWindow::slotProcessContactStateChanged(const QString &contactID, quint8 onlineStateCode, const QString &contactHostAddress, quint16 contactHostPort){

    Contact *contact = contactsManager->getUser(contactID);
    if(!contact){
        return;
    }
    
    IM::OnlineState state = IM::OnlineState(onlineStateCode);
    contact->setOnlineState(state);
    contact->setLastLoginExternalHostAddress(contactHostAddress);
    contact->setLastLoginExternalHostPort(contactHostPort);

    //TODO:Tip
    contactsManager->updateContactToUI(friendBox, contact->getContactGroupID(), contactID);

    QString nickname = contact->getNickName();
    switch(state){
    case IM::ONLINESTATE_ONLINE:
    {
        systemTray->showMessage(QString(APP_NAME), tr("%1 is online!").arg(nickname), QSystemTrayIcon::Information);
    }
        break;
    case IM::ONLINESTATE_INVISIBLE:
    case IM::ONLINESTATE_OFFLINE:
    {
        systemTray->showMessage(QString(APP_NAME), tr("%1 is offline!").arg(nickname), QSystemTrayIcon::Information);
    }
        break;
    default:
        break;
    }

    update();


}

void MainWindow::slotProcessContactsOnlineInfo(const QString &contactsOnlineInfoString){

    QStringList contactsOnlineInfo = contactsOnlineInfoString.split(QString(UNIT_SEPARTOR));
    foreach (QString infoString, contactsOnlineInfo) {
        QStringList infoList = infoString.split(QString(PACKET_DATA_SEPARTOR));
        QString contactID = infoList.at(0);
        Contact *contact = contactsManager->getUser(contactID);
        if(!contact){
            continue;
        }
        contact->setOnlineState(IM::OnlineState(infoList.at(1).toUInt()));
        contact->setLastLoginExternalHostAddress(infoList.at(2));
        contact->setLastLoginExternalHostPort(infoList.at(3).toUInt());
        contactsManager->updateContactToUI(friendBox, contact->getContactGroupID(), contactID);

    }


}

void MainWindow::slotProcessUserInfo(const QString &userID/*, const QString &userInfo*/){
    qDebug()<<"--MainWindow::slotProcessUserInfo(...)";

    if(userID == imUser->getUserID()){
        //imUser->setPersonalSummaryInfo(userInfo);
        ui.toolButtonUserFace->setIcon(ImageResource::createIconForContact(imUser->getFace(), imUser->getOnlineState()));
        ui.labelUserNickName->setText(imUser->getNickName());
        ui.labelUserID->setText(imUser->getUserID());
        //imUser->saveMyInfoToLocalDatabase();
    }else{
        //TODO
        Contact *contact = contactsManager->getUser(userID);
        if(!contact){return;}
//        contact->setPersonalInfoString(userInfo);
//        contactsManager->saveContactInfoToDatabase(userID);

        quint32 contactGroupID = contact->getContactGroupID();
        if(contactGroupID){
            contactsManager->updateContactToUI(friendBox, contactGroupID, userID);
        }
    }


}

void MainWindow::slotProcessContactGroupsInfo(const QString &contactGroupsInfo, quint32 personalContactGroupsInfoVersionOnServer){
    qDebug()<<"--MainWindow::slotProcessContactGroupsInfo(...)"<<" -contactGroupsInfo:"<<contactGroupsInfo;

    imUser->setContactGroupsInfoString(contactGroupsInfo);
    imUser->setPersonalContactGroupsVersion(personalContactGroupsInfoVersionOnServer);
    
    
    QHash<QString/*Group Name*/, QStringList/*Group Members' ID*/> personalContactGroupsHash = imUser->getPersonalContactGroupsHash();
    QStringList groups = personalContactGroupsHash.keys();
    foreach (QString groupName, groups) {
        int groupID = contactsManager->getPersonalContactGroupID(groupName);
        if(!groupID){
            groupID = contactsManager->slotAddNewContactGroupToDatabase(groupName);
            contactsManager->slotAddNewContactGroupToUI(friendBox, groupID, groupName);
        }
        QStringList members = personalContactGroupsHash.value(groupName);
        foreach (QString contactID, members) {
            if(contactID.trimmed().isEmpty()){continue;}
            Contact *contact = contactsManager->getUser(contactID);
            if(!contact){
                contact =  contactsManager->createNewContact(contactID);
//                contact = new Contact(contactID, 0);
                clientPacketsParser->requestContactInfo(m_socketConnectedToServer, contactID);
//                contactsManager->slotAddNewContactToDatabase(contact);
                contactsManager->addContactToUI(friendBox, groupName, contactID);
            }
            contact->setContactGroupID(groupID);
            contactsManager->saveContactInfoToDatabase(contactID);
        }
        
        
    }
    
    
    
    //    contactsManager->saveContactGroupsInfoToDatabase();
    
    
    imUser->saveMyInfoToLocalDatabase();

    //    slotUpdateContactsInfo();
    
}


//void MainWindow::slotProcessSearchContactsResult(const QString &users){

//}

void MainWindow::slotProcessAddContactResult(const QString &contactID, const QString &userNickName, const QString &userFace, quint8 errorTypeCode, const QString &reasonMessage){

    IM::ErrorType type = IM::ErrorType(errorTypeCode);
    switch(type){
    case IM::ERROR_NoError :
    {
//        Contact *contact = 0;
//        if(imUser->hasContact(contactID)){
//            contact = contactsManager->getUser(contactID);
//        }else{
////            contact = new Contact(contactID, userNickName, this);
////            contact->setFace(userFace);
////            //save new contact to db
////            contactsManager->slotAddNewContactToDatabase(contact);
//            contact =  contactsManager->createNewContact(contactID, userNickName, userFace);
//        }

        Contact *contact =  contactsManager->createNewContact(contactID, userNickName, userFace);
        Q_ASSERT(contact);


        //Save contact to default group
        QString groupName = imUser->getDefaultGroupName();
        int groupID = contactsManager->getPersonalContactGroupID(groupName);
        if(!groupID){
            groupID = contactsManager->slotAddNewContactGroupToDatabase(groupName);
            contactsManager->slotAddNewContactGroupToUI(friendBox, groupID, groupName);
        }

        contact->setContactGroupID(groupID);
        contactsManager->saveContactInfoToDatabase(contactID);
        contactsManager->addContactToUI(friendBox, groupName, contactID);


        imUser->addOrDeleteContact(contactID, "", true);
        imUser->saveMyInfoToLocalDatabase();


        //show trayicon info
        if(autoShowSystemMessage){
            getNewContactSettings(contactID);
        }else{
            //TODO
            TrayIconData data(STIDT_FriendshipApplicationResult, QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz"));
            data.setToolTip(contactID);
            data.settrayIconType(TrayIconData::TRAYICON_Flash);
            data.setFirstIcon(QIcon(":/resources/images/systemmessage.png"));
            data.setData(contactID);
            
            systemTray->appendTrayIconData(data);
            systemTray->updateSystemTrayIcon();
        }
        
    }
        break;
    case IM::ERROR_RequestDenied :
    {
        QString msg = tr("User %1 rejected your request that add (s)he as a contact!)").arg(contactID);
        if(!reasonMessage.trimmed().isEmpty()){
            msg += tr("<p>Message: %1</p>").arg(reasonMessage);
        }
        QMessageBox::critical(this, tr("Request Rejected"), msg);
    }
        break;
    default:
        QMessageBox::critical(this, tr("Error"), tr("Can not add user %1 as a contact! Error code:%2").arg(contactID).arg(errorTypeCode));
        
    }

    
}

void MainWindow::getNewContactSettings(const QString &contactID){
    
    Contact *contact = contactsManager->getUser(contactID);
    if(!contact){
        return;
    }
    
    
    QString existingGroupName = imUser->groupNameThatContactBelongsTo(contactID);

    AddContactDialog dlg(contact, false, this);
    dlg.exec();
    QString groupName = dlg.getGroupname();
    QString remarkName = dlg.getNewName();
    if(!remarkName.trimmed().isEmpty()){
        contact->setRemarkName(remarkName);
        contactsManager->saveContactInfoToDatabase(contactID);
    }
    qDebug()<<"---------existingGroupName:"<<existingGroupName<<"  groupName"<<groupName;
    if(existingGroupName == groupName){
        return;
    }

    
    int groupID = contactsManager->getPersonalContactGroupID(groupName);
    if(!groupID){
        groupID = contactsManager->slotAddNewContactGroupToDatabase(groupName);
        contactsManager->slotAddNewContactGroupToUI(friendBox, groupID, groupName);
    }
    //qDebug()<<"---------groupID:"<<groupID<<"  groupName"<<groupName;

    imUser->moveContact(contactID, existingGroupName, groupName);
    imUser->saveMyInfoToLocalDatabase();
    clientPacketsParser->moveContactToGroup(m_socketConnectedToServer, contactID, existingGroupName, groupName);

    contact->setContactGroupID(groupID);
    contactsManager->saveContactInfoToDatabase(contactID);
    contactsManager->moveContact(contactID, contactsManager->getPersonalContactGroupID(existingGroupName), groupID);

    if(existingGroupName.isEmpty()){
        contactsManager->addContactToUI(friendBox, groupName, contactID);
    }else{
        contactsManager->moveContactToUI(friendBox, existingGroupName, groupName, contactID);
    }

    
}

void MainWindow::slotProcessBlacklistInfo(const QString &blacklistOnServer, quint32 blacklistInfoVersionOnServer){

    imUser->setBlacklistInfoString(blacklistOnServer);
    imUser->setBlacklistInfoVersion(blacklistInfoVersionOnServer);

    slotUpdateBlacklistInfo();

    imUser->saveMyInfoToLocalDatabase();

}

void MainWindow::slotSearch(){
    if(!search){
        search = new Search(this);
        connect(search, SIGNAL(signalSearchContact(const QString &, bool, bool)), this, SLOT(searchContact(const QString &, bool, bool)), Qt::QueuedConnection);
        connect(search, SIGNAL(signalAddContact(const QString&, const QString&)), this, SLOT(addContact(const QString&, const QString&)));

        connect(clientPacketsParser, SIGNAL(signalSearchContactsResultPacketReceived(const QStringList &)), search, SLOT(slotSearchContactsResultPacketReceived(const QStringList &)), Qt::QueuedConnection);


    }

    search->show();

}

void MainWindow::searchContact(const QString &propertiesString, bool matchExactly, bool searchOnlineUsersOnly){
    clientPacketsParser->searchContact(m_socketConnectedToServer, propertiesString, matchExactly, searchOnlineUsersOnly);
}

void MainWindow::addContact(const QString &userID, const QString &verificationMessage){
    clientPacketsParser->addContact(m_socketConnectedToServer, userID, verificationMessage);
}

void MainWindow::slotProcessContactRequestFromUser(const QString &userID, const QString &userNickName, const QString &userFace, const QString &verificationMessage){

    if(autoShowSystemMessage){
        showContactRequestFromUser(userID, userNickName, userFace, verificationMessage);
    }else{

        TrayIconData data(STIDT_FriendshipApplicationFromContact, QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz"));
        data.setToolTip(userID);
        data.settrayIconType(TrayIconData::TRAYICON_Flash);
        data.setFirstIcon(QIcon(":/resources/images/systemmessage.png"));

        QStringList list;
        list << userID << userNickName << userFace << verificationMessage;
        data.setData(list.join(QChar(PACKET_DATA_SEPARTOR)));

        systemTray->appendTrayIconData(data);
        systemTray->updateSystemTrayIcon();
    }




}

void MainWindow::showContactRequestFromUser(const QString &userID, const QString &userNickName, const QString &userFace, const QString &verificationMessage){


    Contact user(userID, userNickName, this);
    user.setFace(userFace);

    AddContactDialog dlg(&user, verificationMessage, this);
    if(dlg.exec() == QDialog::Accepted){
        if(dlg.requestRejected()){
            clientPacketsParser->responseAddContactRequestFromUser(m_socketConnectedToServer, userID, false, dlg.getMessage());
        }else{
            clientPacketsParser->addContact(m_socketConnectedToServer, userID, dlg.getGroupname());
        }
    }


}



void MainWindow::slotProcessChatMessageReceivedFromContact(const QString &contactID, const QString &message, const QString &time){
    qDebug()<<"--MainWindow::slotProcessChatMessageReceivedFromContact(...)--Contact:"<<contactID<<" Message:"<<message<<" Time:"<<time;


    Contact *contact = contactsManager->getUser(contactID);
    if(!contact){return;}

    contactsManager->saveContactChatMessageToDatabase(imUser->getUserID(), contactID, message, time);

    if(chatWindowManager->isVisible() || autoShowChatMessageFromContact){
        chatWindowManager->slotNewMessageReceivedFromContact(contactID, message, time);
    }else{
        //TODO:
        QHash<QString/*Time*/, QVariant/*Message*/> data;
        QString timeString = time;
        if(timeString.isEmpty()){
            timeString = QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss");
        }

        if(systemTray->trayIconDataExists(contactID)){
            data = systemTray->getData(contactID).toHash();
            data.insertMulti(timeString, message);
            systemTray->setData(contactID, QVariant(data));
        }else{
            TrayIconData trayIconData(STIDT_ContactChatMessage, contactID);
            trayIconData.setToolTip(contactID);
            trayIconData.settrayIconType(TrayIconData::TRAYICON_Flash);
            trayIconData.setFirstIcon(ImageResource::createIconForContact(contact->getFace(), IM::ONLINESTATE_ONLINE));

            data.insertMulti(timeString, message);
            trayIconData.setData(QVariant(data));
            systemTray->appendTrayIconData(trayIconData);
        }

        systemTray->updateSystemTrayIcon();
    }


}

void MainWindow::slotProcessChatMessageCachedOnServer(const QStringList &messages){
    qDebug()<<"--MainWindow::slotProcessChatMessageCachedOnServer(...)";

    qWarning()<<"----ChatMessageCachedOnServer! Messages:"<<messages;


    foreach (QString messageString, messages) {
        QStringList info = messageString.split(QString(PACKET_DATA_SEPARTOR));
        if(info.size() == 3){
            slotProcessChatMessageReceivedFromContact(info.at(0), info.at(1), info.at(2));

        }else{
            qWarning()<<"Invalid Chat Message Format!";
        }
    }


}

void MainWindow::slotProcessInterestGroupChatMessagesReceivedFromContact(quint32 interestGroupID, const QString &contactID, const QString &message, const QString &time){

    InterestGroup *group = contactsManager->getInterestGroup(interestGroupID);
    if(!group){
        group = new InterestGroup(interestGroupID, "", this);
        //TODO:
        clientPacketsParser->requestInterestGroupInfo(m_socketConnectedToServer, interestGroupID);
        contactsManager->addNewInterestGroupToDatabase(group);
    }

    Contact *contact = contactsManager->getUser(contactID);
    if(!contact){
        contact =  contactsManager->createNewContact(contactID);
//        contact = new Contact(contactID, this);
        clientPacketsParser->requestContactInfo(m_socketConnectedToServer, contactID);
//        contactsManager->slotAddNewContactToDatabase(contact);
    }

    contactsManager->saveInterestGroupChatMessageToDatabase(contactID, interestGroupID, message, time);


    if(chatWindowManager->isVisible() || autoShowChatMessageFromContact){
        chatWindowManager->slotNewMessageReceivedFromInterestGroup(interestGroupID, contactID, message, time);
    }else{
        //TODO:
        QHash<QString/*Time*/, QVariant/*Contact ID, Message*/ > data;
        //QHash<QString/*Time*/, QStringList/*Contact ID, Message*/ > data;

        QString interestGroupIDString = QString::number(interestGroupID);
        QString timeString = time;
        if(timeString.isEmpty()){
            timeString = QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss");
        }
        if(systemTray->trayIconDataExists(interestGroupIDString)){
            data = systemTray->getData(interestGroupIDString).toHash();
            QStringList list;
            list << contactID << message;
            data.insertMulti(timeString, list);
            systemTray->setData(interestGroupIDString, QVariant(data));
        }else{
            TrayIconData trayIconData(STIDT_InterestGroupChatMessage, interestGroupIDString);
            trayIconData.setToolTip(group->getGroupName());
            trayIconData.settrayIconType(TrayIconData::TRAYICON_Flash);
            trayIconData.setFirstIcon(ImageResource::createIconForInterestGroup());

            QStringList list;
            list << contactID << message;
            data.insertMulti(timeString, list);
            //            data.insertMulti(timeString, qMakePair(contactID, message));
            trayIconData.setData(QVariant(data));
            systemTray->appendTrayIconData(trayIconData);
        }

        systemTray->updateSystemTrayIcon();
    }


}


void MainWindow::slotProcessInterestGroupChatMessagesCachedOnServer(const QStringList &messages){

    foreach (QString messageString, messages) {
        QStringList info = messageString.split(QString(PACKET_DATA_SEPARTOR));
        if(info.size() == 4){
            slotProcessInterestGroupChatMessagesReceivedFromContact(info.at(0).toUInt(), info.at(1), info.at(2), info.at(3));

        }else{
            qWarning()<<"Invalid Chat Message Format!";
        }
    }


}

void MainWindow::slotProcessReceivedChatImage(const QString &fileName){

}

void MainWindow::slotSendChatMessageToContact(Contact *contact, const QString &message, const QStringList &imageList){

    //    QRegExp regExp("<img.*>");
    //    regExp.setCaseSensitivity(Qt::CaseInsensitive);
    //    regExp.setMinimal(true);
    //    int pos = 0;
    //    QStringList imgList;
    //    while ((pos = regExp.indexIn(htmlOfTextEdit, pos)) != -1) {
    //        imgList << regExp.cap(0);
    //        pos += regExp.matchedLength();
    //    }
    
    //    regExp.setPattern("\"(([a-zA-Z]:/\\w)|(/\\w)).+\\.((png)|(gif)|(jpg)|(jpeg))\"");
    //    foreach (QString imgTag, imgList) {

    //    }
    
    
    //    QFileInfo fileInfo(iconPath);
    //    QString fileName = myUserID + "_" + fileInfo.absoluteDir().dirName() + "_" + fileInfo.fileName();
    //    QString filePath = imageCachePath + "/" + fileName;
    
    //    if(!fileInfo.exists()){
    //        QFile::copy(iconPath, filePath);
    //    }
    
    //    emoticon = QString("<img src=\"" + filePath + "\"> ");
    
    
    
    
    //Contact *contact = contactsManager->getUser(contactID);
    if(!contact){return;}
    QString contactID = contact->getUserID();
    if(contact->getOnlineState() == IM::ONLINESTATE_OFFLINE){
        clientPacketsParser->sendChatMessageToServer(m_socketConnectedToServer, contactID, message);
    }else{
        QString contactHostAddress = contact->getLastLoginExternalHostAddress();
        quint16 contactHostPort = contact->getLastLoginExternalHostPort();
        clientPacketsParser->sendChatMessageToContact(m_socketConnectedToServer, contactID, message, contactHostAddress, contactHostPort);
        clientPacketsParser->sendImageFileToContact(m_socketConnectedToServer, contactID, imageList, contactHostAddress, contactHostPort);
    }

    contactsManager->saveContactChatMessageToDatabase(imUser->getUserID(), contactID, message);
    

}

void MainWindow::slotSendChatMessageToInterestGroup(InterestGroup *interestGroup, const QString &message, const QStringList &imageList){

    //TODO:

    if(!interestGroup){
        return;
    }

    clientPacketsParser->sendInterestGroupChatMessageToServer(m_socketConnectedToServer, interestGroup->getGroupID(), message);
    //TODO: Send images to server

    QStringList members = interestGroup->members();
    foreach (QString contactID , members) {
        Contact *contact = contactsManager->getUser(contactID);
        if(!contact){continue;}
        if(contact->getOnlineState() == IM::ONLINESTATE_OFFLINE){
            //            clientPacketsParser->sendChatMessageToServer(contactID, message);
        }else{
            clientPacketsParser->sendInterestGroupChatMessageToContact(m_socketConnectedToServer, contactID, interestGroup->getGroupID(), message, contact->getLastLoginExternalHostAddress(), contact->getLastLoginExternalHostPort());
            clientPacketsParser->sendImageFileToContact(m_socketConnectedToServer, contactID, imageList, contact->getLastLoginExternalHostAddress(), contact->getLastLoginExternalHostPort());
        }
    }

    contactsManager->saveInterestGroupChatMessageToDatabase(imUser->getUserID(), interestGroup->getGroupID(), message);


}

void MainWindow::slotProcessInterestGroupsList(const QString &interestGroupsListFromServer, quint32 interestGroupsInfoVersionOnServer){
    qDebug()<<"--MainWindow::slotProcessInterestGroupsList(..)";
    qDebug()<<"----interestGroupsListFromServer:"<<interestGroupsListFromServer;

    //    if(interestGroupsListFromServer.trimmed().isEmpty()){
    //        return;
    //    }

    QStringList groupsOnServer;
    QStringList infoList = interestGroupsListFromServer.split(",");
    foreach (QString info, infoList) {
        QStringList list = info.split(":");
        if(list.size() != 3){
            qCritical()<<"Invalid Interest Group Info!";
            continue;
        }
        quint32 groupID = list.at(0).toUInt();
        groupsOnServer.append(QString::number(groupID));
        quint32 groupInfoVersion = list.at(1).toUInt();
        quint32 memberListInfoVersion = list.at(2).toUInt();
        qWarning()<<"Server: groupInfoVersion:"<<groupInfoVersion<<" memberListInfoVersion:"<<memberListInfoVersion;

        InterestGroup *group = contactsManager->getInterestGroup(groupID);
        if(group){
            qWarning()<<"Local: groupInfoVersion:"<<group->getGroupInfoVersion()<<" memberListInfoVersion:"<<group->getGroupMemberListInfoVersion();

            if(groupInfoVersion != group->getGroupInfoVersion()){
                clientPacketsParser->requestInterestGroupInfo(m_socketConnectedToServer, groupID);
            }else if(memberListInfoVersion != group->getGroupMemberListInfoVersion()){
                clientPacketsParser->requestInterestGroupMembersInfo(m_socketConnectedToServer, groupID);
            }
        }else{
            group = new InterestGroup(groupID, "", this);
            contactsManager->addNewInterestGroupToDatabase(group);
            //TODO:
            clientPacketsParser->requestInterestGroupInfo(m_socketConnectedToServer, groupID);
            
        }
        
    }
    
    QStringList interestGroupsOnLocal = imUser->getInterestGroups();
    if(!interestGroupsOnLocal.isEmpty()){
        foreach (QString groupID, interestGroupsOnLocal) {
            if(!groupsOnServer.contains(groupID)){
                contactsManager->leaveInterestGroup(groupID.toUInt());
            }
        }

    }
    
    imUser->setInterestGroupInfoVersion(interestGroupsInfoVersionOnServer);
    
    imUser->saveMyInfoToLocalDatabase();

    updateAllInterestGroupsInfoToUI();

}

void MainWindow::slotProcessInterestGroupInfo(const QString &interestGroupInfoFromServer, quint32 groupID){

    InterestGroup *interestGroup = contactsManager->getInterestGroup(groupID);
    if(!interestGroup){
        return;
    }
    
    quint32 oldMembersInfoVersion = interestGroup->getGroupMemberListInfoVersion();
    
    interestGroup->setGroupInfoString(interestGroupInfoFromServer);
    qWarning()<<"interestGroupInfoFromServer:"<<interestGroupInfoFromServer;
    
    if(oldMembersInfoVersion != interestGroup->getGroupMemberListInfoVersion()){
        clientPacketsParser->requestInterestGroupMembersInfo(m_socketConnectedToServer, groupID);
        interestGroup->setGroupMemberListInfoVersion(oldMembersInfoVersion);
    }

    updateInterestGroupInfoToUI(interestGroup);
    contactsManager->saveInterestGroupInfoToDatabase(interestGroup);
    

    
    
}

void MainWindow::slotProcessInterestGroupMembersInfo(const QString &interestGroupMembersInfoFromServer, quint32 interestGroupMembersInfoVersionOnServer, quint32 groupID){
    qDebug()<<"--MainWindow::slotProcessInterestGroupMembersInfo(...)";
    //qDebug()<<"interestGroupMembersInfoFromServer:"<<interestGroupMembersInfoFromServer;
    qDebug()<<"interestGroupMembersInfoVersionOnServer:"<<interestGroupMembersInfoVersionOnServer;
    qDebug()<<"groupID:"<<groupID;

    InterestGroup *interestGroup = contactsManager->getInterestGroup(groupID);
    if(!interestGroup){
        qCritical()<<"No such InterestGroup:"<<groupID;
        return;
    }
    
    QHash <QString/*Member's ID*/, quint32/*Member's Role*/> membersHash;
    QStringList infoList = interestGroupMembersInfoFromServer.split(QString(PACKET_DATA_SEPARTOR));
    foreach (QString info, infoList) {
        QStringList list = info.split(CONTACT_INFO_SEPARATOR);
        QString contactID = list.at(0);
        quint32 contactInfoVersion = list.at(1).toUInt();
        quint32 memberRole = list.at(2).toUInt();
        qWarning()<<"contactID:"<<contactID<<" contactInfoVersion:"<<contactInfoVersion<<" memberRole:"<<memberRole;
        Contact *contact = contactsManager->getUser(contactID);
        if(!contact){
            contact =  contactsManager->createNewContact(contactID);
//            contact = new Contact(contactID, this);
            clientPacketsParser->requestContactInfo(m_socketConnectedToServer, contactID);
//            contactsManager->slotAddNewContactToDatabase(contact);
        }else{
            if(contactInfoVersion > contact->getPersonalDetailInfoVersion()){
                clientPacketsParser->requestContactInfo(m_socketConnectedToServer, contactID);
            }
        }
        
        membersHash.insert(contactID, memberRole);
        
    }
    
    interestGroup->setMembersHash(membersHash);
    
    if(!membersHash.isEmpty()){
        if(contactsManager->saveInterestGroupMembersToDatabase(interestGroup)){
            interestGroup->setGroupMemberListInfoVersion(interestGroupMembersInfoVersionOnServer);
        }
    }

    contactsManager->saveInterestGroupInfoToDatabase(interestGroup);


    
    //TODO
    
    
}

void MainWindow::interestGroupItemActivated(QListWidgetItem * item ){

    chatWindowManager->slotNewChatWithInterestGroup(item->data(Qt::UserRole).toUInt());

    //qWarning()<<"interestGroupItemActivated(...):"<<item->data(Qt::UserRole).toUInt();

}

void MainWindow::showUserInfo(IMUserBase *user){

    Q_ASSERT_X(user, "MainWindow::showUserInfo(...)", "Invalid IMUserBase!");

    ContactInfoWidget *  m_ContactInfoWidget = ContactInfoWidget::getContactInfoWidget(user);
    if(!m_ContactInfoWidget){
        m_ContactInfoWidget = new ContactInfoWidget(user);
    }
    m_ContactInfoWidget->show();
    m_ContactInfoWidget->raise();


}

void MainWindow::requestLogin(const QHostAddress &serverHostAddress, quint16 serverPort){

    Q_ASSERT(m_rtp);


    QString errorMessage;
    if(m_socketConnectedToServer == INVALID_SOCK_ID){
        m_socketConnectedToServer = m_rtp->connectToHost(QHostAddress(serverHostAddress), serverPort, 10000, &errorMessage);
    }
    if(m_socketConnectedToServer == INVALID_SOCK_ID){
        qCritical()<<tr("Can not connect to host! %1").arg(errorMessage);
        return;
    }

    m_serverHostAddress = serverHostAddress;
    m_serverHostPort = serverPort;

    //    peerConnected(m_socketConnectedToServer, serverHostAddress.toString(), serverPort);

    clientPacketsParser->requestLogin(m_socketConnectedToServer);

    QTimer::singleShot(10000, this, SLOT(loginTimeout()));

    //    if(!m_loginTimer){
    //        m_loginTimer = new QTimer(this);
    //        m_loginTimer->setSingleShot(true);
    //        connect(m_loginTimer, SIGNAL(timeout()), this, SLOT(loginTimeout()));
    //    }
    //    m_loginTimer->start(10000);

}

void MainWindow::loginTimeout(){

    if(!m_verified){
        ui.loginPage->slotProcessLoginResult(IM::ERROR_Timeout);
    }

}

void MainWindow::requestRegistrationServerInfo(){
    qDebug()<<"--MainWindow::requestRegistrationServerInfo()";

    int socketID = INVALID_SOCK_ID;

    QHostAddress svrAddress = QHostAddress(imUser->getLoginServerAddress());
    quint16 svrPort = imUser->getLoginServerPort();

    if(svrAddress == m_serverHostAddress && svrPort == m_serverHostPort && m_rtp->isSocketConnected(m_socketConnectedToServer)){
        socketID = m_socketConnectedToServer;
    }else{
        QString errorMessage;
        socketID = m_rtp->connectToHost(svrAddress, svrPort, 10000, &errorMessage);
        if(socketID == INVALID_SOCK_ID){
            QMessageBox::critical(this, tr("Error"), tr("Failed to contact server! <br>%1").arg(errorMessage));
            return;
        }
        m_rtp->closeSocket(m_socketConnectedToServer);
    }

    m_serverHostAddress = svrAddress;
    m_serverHostPort = svrPort;
    m_socketConnectedToServer = socketID;


    bool ok = clientPacketsParser->requestRegistrationServerInfo(m_socketConnectedToServer);
    if(!ok){
        QMessageBox::critical(this, tr("Error"), tr("Failed to send request! <br>%1").arg(m_rtp->lastErrorString()));
    }

}

void MainWindow::requestRegistration(){

    //TODO
    int socketID = INVALID_SOCK_ID;
    QHostAddress svrAddress = QHostAddress::Null;
    quint32 svrPort = 0;

    QStringList serverAddressInfo = imUser->getRegistrationServerAddressInfo().split(":");
    if(serverAddressInfo.size() == 2){
        svrAddress = QHostAddress(serverAddressInfo.at(0));
        svrPort = serverAddressInfo.at(1).toUInt();
    }else{
        svrAddress = QHostAddress(imUser->getLoginServerAddress());
        svrPort = imUser->getLoginServerPort();
    }

    if(QHostAddress(svrAddress) == m_serverHostAddress && svrPort == m_serverHostPort && m_rtp->isSocketConnected(m_socketConnectedToServer)){
        socketID = m_socketConnectedToServer;
    }else{
        QString errorMessage;
        socketID = m_rtp->connectToHost(svrAddress, svrPort, 10000, &errorMessage);
        if(socketID == INVALID_SOCK_ID){
            QMessageBox::critical(this, tr("Error"), tr("Failed to register! <br>%1").arg(errorMessage));
            return;
        }
    }

    bool ok = clientPacketsParser->registration(socketID, imUser->getUserID(), imUser->getPassword());
    if(!ok){
        QMessageBox::critical(this, tr("Error"), tr("Failed to send registration request! <br>%1").arg(m_rtp->lastErrorString()));
    }

}

void MainWindow::peerConnected(int socketID, const QString &peerAddress, quint16 peerPort){
    qWarning()<<QString("Connected! "+peerAddress+":"+QString::number(peerPort));

    if(QHostAddress(peerAddress) == m_serverHostAddress && peerPort == m_serverHostPort){
        //clientPacketsParser->requestLogin(m_socketConnectedToServer);
        m_socketConnectedToServer = socketID;

        //        if(m_loginTimer){
        //            m_loginTimer->stop();
        //            delete m_loginTimer;
        //            m_loginTimer = 0;
        //        }
        emit signalServerOnlineStateChanged(true);
    }else{

    }

}

void MainWindow::signalConnectToPeerTimeout(const QHostAddress &peerAddress, quint16 peerPort){
    qCritical()<<QString("Connecting Timeout! "+peerAddress.toString()+":"+QString::number(peerPort));

}

void MainWindow::peerDisconnected(const QHostAddress &peerAddress, quint16 peerPort, bool normalClose){
    qDebug()<<QString("Disconnected! "+peerAddress.toString()+":"+QString::number(peerPort));

    if(!normalClose){
        qCritical()<<QString("ERROR! Peer %1:%2 Closed Unexpectedly!").arg(peerAddress.toString()).arg(peerPort);
    }

    if(peerAddress == m_serverHostAddress && peerPort == m_serverHostPort){
        //TODO
        m_verified = false;
        emit signalServerOnlineStateChanged(false);
        systemTray->resetTrayIcon(ImageResource::createMixedIcon((QString(RESOURCE_PATH)+QString(APP_ICON_PATH)), IM::ONLINESTATE_OFFLINE));

        if(!normalClose){
            requestLogin(m_serverHostAddress, m_serverHostPort);
        }
    }

}

void MainWindow::peerDisconnected(int socketID){
    qWarning()<<"Peer Disconnected! Socket ID:"<<socketID;

    if(socketID == m_socketConnectedToServer){
        m_socketConnectedToServer = INVALID_SOCK_ID;
        QMessageBox::critical(this, tr("Error"), tr("Disconnected from server!"));
        return;
    }

    m_contactSocketsHash.remove(socketID);

    QList<int> requests = fileTXRequestHash.keys(socketID);
    foreach (int request, requests) {
        fileTXRequestHash.remove(request);
    }

    QList<QByteArray> files = fileTXSocketHash.values(socketID);
    fileTXSocketHash.remove(socketID);
    QList<QByteArray> allFiles = fileTXSocketHash.values();

    foreach (QByteArray fileMD5, files) {
        if(!allFiles.contains(fileMD5)){
            m_fileManager->closeFile(fileMD5);
        }
    }


}

void MainWindow::startFileManager(){

    if(!m_fileManager){
        m_fileManager = ClientResourcesManager::instance()->getFileManager();
        connect(m_fileManager, SIGNAL(dataRead(int , const QByteArray &, int , const QByteArray &, const QByteArray &)), this, SLOT(fileDataRead(int , const QByteArray &, int , const QByteArray &, const QByteArray &)), Qt::QueuedConnection);
        connect(m_fileManager, SIGNAL(error(int , const QByteArray &, quint8, const QString &)), this, SLOT(fileTXError(int , const QByteArray &, quint8, const QString &)), Qt::QueuedConnection);
        connect(m_fileManager, SIGNAL(pieceVerified(const QByteArray &, int , bool , int )), this, SLOT(pieceVerified(const QByteArray &, int , bool , int )), Qt::QueuedConnection);

    }

}

void MainWindow::processContactRequestUploadFilePacket(int socketID, const QString &contactID, const QByteArray &fileMD5Sum, const QString &fileName, quint64 size, const QString &localFileSaveDir){

    Contact *contact = contactsManager->getUser(contactID);
    if(!contact){return;}

    //TODO
    QMessageBox::information(this, "TODO", "processContactRequestUploadFilePacket");


}

void MainWindow::processContactRequestDownloadFilePacket(int socketID, const QString &contactID, const QString &localBaseDir, const QString &fileName, const QString &remoteFileSaveDir){

    startFileManager();

    QString errorString;

    QFileInfo fi(localBaseDir, fileName);
    QString absoluteFilePath = fi.absoluteFilePath();
    if(fi.isDir()){
        QDir dir(absoluteFilePath);

        QStringList filters;
        filters << "*" << "*.*";

        foreach(QString file, dir.entryList(filters, QDir::Dirs | QDir::Files | QDir::System | QDir::Hidden | QDir::NoDotAndDotDot))
        {
            QString newRemoteDir = remoteFileSaveDir + "/" + fileName;
            if(remoteFileSaveDir.endsWith('/')){
                newRemoteDir = remoteFileSaveDir + fileName;
            }
            processContactRequestDownloadFilePacket(socketID, contactID, absoluteFilePath, file, newRemoteDir);

            qApp->processEvents();
        }

        return;
    }

    const FileManager::FileMetaInfo *info = m_fileManager->tryToSendFile(absoluteFilePath, &errorString);
    if(!info){
        clientPacketsParser->denyFileDownloadRequest(socketID, contactID, fileName, false, errorString);
    }

    if(clientPacketsParser->acceptFileDownloadRequest(socketID, contactID, fileName, true, info->md5sum, info->size, remoteFileSaveDir)){
        fileTXSocketHash.insertMulti(socketID, info->md5sum);
    }else{
        m_fileManager->closeFile(info->md5sum);
    }

}

void MainWindow::processFileDataRequestPacket(int socketID, const QString &contactID, const QByteArray &fileMD5, int startPieceIndex, int endPieceIndex){

    Q_ASSERT(m_fileManager);

    if( (startPieceIndex == -1) && (endPieceIndex == -1) ){
        QList<int> completedPieces = m_fileManager->completedPieces(fileMD5);
        foreach (int pieceIndex, completedPieces) {
            fileTXRequestHash.insert(m_fileManager->readPiece(fileMD5, pieceIndex), socketID);
            //QCoreApplication::processEvents();
        }

    }else{
        Q_ASSERT(endPieceIndex >= startPieceIndex);
        for(int i=startPieceIndex; i<=endPieceIndex; i++){
            fileTXRequestHash.insert(m_fileManager->readPiece(fileMD5, i), socketID);
            //QCoreApplication::processEvents();
        }

    }

    //    int id = m_fileManager->readPiece(fileMD5, pieceIndex);
    //    fileTXRequestHash.insert(id, socketID);

}

void MainWindow::processFileDataReceivedPacket(int socketID, const QString &contactID, const QByteArray &fileMD5, int pieceIndex, const QByteArray &data, const QByteArray &sha1){

    Q_ASSERT(m_fileManager);
    m_fileManager->writePiece(fileMD5, pieceIndex, data, sha1);


    //        clientPacketsParser->requestFileData(socketID, fileTXWithAdmin->pos(), FILE_PIECE_LENGTH);


}

void MainWindow::processFileTXStatusChangedPacket(int socketID, const QString &contactID, const QByteArray &fileMD5, quint8 status){

    //IM::FileTXStatus status = IM::FileTXStatus(status);
    switch(status){
    case quint8(IM::File_TX_Preparing):
    {

    }
        break;
    case quint8(IM::File_TX_Receiving):
    {

    }
        break;
    case quint8(IM::File_TX_Sending):
    {

    }
        break;
    case quint8(IM::File_TX_Progress):
    {

    }
        break;
    case quint8(IM::File_TX_Paused):
    {

    }
        break;
    case quint8(IM::File_TX_Aborted):
    {
        QList<int> sockets = fileTXSocketHash.keys(fileMD5);
        if(sockets.contains(socketID) && sockets.size() <= 1){
            m_fileManager->closeFile(fileMD5);
        }
    }
        break;
    case quint8(IM::File_TX_Done):
    {
        QList<int> sockets = fileTXSocketHash.keys(fileMD5);
        if(sockets.contains(socketID) && sockets.size() <= 1){
            m_fileManager->closeFile(fileMD5);
        }

    }
        break;
    default:
        break;
    }

}

void MainWindow::processFileTXErrorFromPeer(int socketID, const QString &contactID, const QByteArray &fileMD5, quint8 errorCode, const QString &errorString){
    qDebug()<<"--MainWindow::processFileTXErrorFromPeer(...) " <<" socketID:"<<socketID;
    qCritical()<<errorString;

}

void MainWindow::fileDataRead(int requestID, const QByteArray &fileMD5, int pieceIndex, const QByteArray &data, const QByteArray &dataSHA1SUM){
    qDebug()<<"--MainWindow::fileDataRead(...) "<<" pieceIndex:"<<pieceIndex<<" size:"<<data.size();

    int socketID = fileTXRequestHash.take(requestID);
    Contact *contact = m_contactSocketsHash.value(socketID);
    if(!contact){return;}

    clientPacketsParser->sendFileData(socketID, contact->getUserID(), fileMD5, pieceIndex, &data, &dataSHA1SUM);

}

void MainWindow::fileTXError(int requestID, const QByteArray &fileMD5, quint8 errorCode, const QString &errorString){
    qCritical()<<errorString;

    if(requestID){
        int socketID = fileTXRequestHash.take(requestID);
        Contact *contact = m_contactSocketsHash.value(socketID);
        if(!contact){return;}

        clientPacketsParser->fileTXError(socketID, contact->getUserID(), fileMD5, errorCode, errorString);
    }else{
        //TODO:
    }


}

void MainWindow::pieceVerified(const QByteArray &fileMD5, int pieceIndex, bool verified, int verificationProgress){
    qDebug()<<"--MainWindow::pieceVerified(...) "<<" pieceIndex:"<<pieceIndex<<" verified:"<<verified<< "verificationProgress:"<<verificationProgress;

    QList<int> sockets = fileTXSocketHash.keys(fileMD5);
    if(sockets.isEmpty()){
        //TODO:
        //m_fileManager->closeFile(fileMD5);
    }

    if(verified){

        if(verificationProgress == 100){
            qWarning()<<"Done!";
            foreach (int socketID, sockets) {
                Contact *contact = m_contactSocketsHash.value(socketID);
                if(!contact){continue;}
                clientPacketsParser->fileTXStatusChanged(socketID, contact->getUserID(), fileMD5, quint8(IM::File_TX_Done));
            }

        }else{
            //TODO:
            //            int uncompletedPieceIndex = m_fileManager->getOneUncompletedPiece(fileMD5);
            //            qDebug()<<"uncompletedPieceIndex:"<<uncompletedPieceIndex;
            //            if(uncompletedPieceIndex < 0){
            //                return;
            //            }
            //            clientPacketsParser->requestFileData(sockets.first(), fileMD5, uncompletedPieceIndex);


            //if((pieceIndex % FILE_PIECES_IN_ONE_REQUEST) == 0){
            //    qDebug()<<"----0----pieceIndex:"<<pieceIndex;
            //    clientPacketsParser->requestFileData(sockets.first(), fileMD5, pieceIndex + 1, pieceIndex + FILE_PIECES_IN_ONE_REQUEST);
            //}

            if((pieceIndex % FILE_PIECES_IN_ONE_REQUEST) == 0){
                //TODO:P2P
                int socketID = sockets.first();
                Contact *contact = m_contactSocketsHash.value(socketID);
                if(!contact){return;}

                if(pieceIndex == 0 ){
                    clientPacketsParser->requestFileData(socketID, contact->getUserID(), fileMD5, 1, 2 * FILE_PIECES_IN_ONE_REQUEST);
                }else{
                    clientPacketsParser->requestFileData(socketID, contact->getUserID(), fileMD5, pieceIndex + FILE_PIECES_IN_ONE_REQUEST + 1, pieceIndex + 2 * FILE_PIECES_IN_ONE_REQUEST);
                }
            }

        }


    }else{
        //TODO:P2P
        int socketID = sockets.first();
        Contact *contact = m_contactSocketsHash.value(socketID);
        if(!contact){return;}
        qCritical()<<"ERROR! Verification Failed! Piece:"<<pieceIndex;
        clientPacketsParser->requestFileData(socketID, contact->getUserID(), fileMD5, pieceIndex, pieceIndex);
    }

}



//void MainWindow::slotChangeContactOnlineState(const QString &contactID, quint8 onlineState, const QString &peerAddress, quint16 peerPort, const QString &greetingInfo){

//	qDebug()<<"----MainWindow::slotChangeContactOnlineState(...)";
//	qDebug()<<"----MainWindow::slotChangeContactOnlineState(...)~~contactID:"<<contactID;
//	qDebug()<<"----MainWindow::slotChangeContactOnlineState(...)~~greetingInfo:"<<greetingInfo;


//	contactsManager->slotChangeContactOnlineState(contactID, onlineState, peerAddress, peerPort, greetingInfo);

//        Contact *contact = contactsManager->getUser(contactID);
//        int groupID = contact->getContactGroupID();
//	ContactGroup *group = contactsManager->getContactGroup(groupID);
//	QString groupName = group->getGroupName();
//        if(onlineState == quint8(IM::OFFLINE) || onlineState == quint8(IM::INVISIBLE)){
//			friendsListView->updateObjectItemIcon(groupName, contactID, ImageResource::createIconForContact("", false));
//			//QMessageBox::information(this, "Offline", QString("IMUser %1 Offline").arg(contactID));
//			qDebug()<<"----MainWindow::slotChangeContactOnlineState(...)~~"<<QString("IMUser %1 Offline").arg(contactID);
//	}else{
//			friendsListView->updateObjectItemIcon(groupName, contactID, ImageResource::createIconForContact("", true));
//			//QMessageBox::information(this, "Online", QString("IMUser %1 Online").arg(contactID));
//			qDebug()<<"----MainWindow::slotChangeContactOnlineState(...)~~"<<QString("IMUser %1 Online").arg(contactID);
//	}
//	//friendsListView->updateObjectItemIcon(groupName, contactID, ":/resources/images/app.png");

//}

//void MainWindow::slotUpdateContactInfo(const QString &contactID, quint8 onlineState, const QString &peerAddress, quint16 peerPort, const QString &info){

//	qDebug()<<"----MainWindow::slotUpdateContactInfo(...)";
//	qDebug()<<"----MainWindow::slotUpdateContactInfo(...)~~contactID:"<<contactID;
//	qDebug()<<"----MainWindow::slotUpdateContactInfo(...)~~info:"<<info;

//	slotChangeContactOnlineState(contactID, onlineState, peerAddress, peerPort, info);

//	//TODO:更新其它信息

//}















} //namespace HEHUI





