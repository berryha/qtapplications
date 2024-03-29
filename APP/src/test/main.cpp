
#include <QtGui>
#include <QApplication>
#include <QSplashScreen>



#include "./mainwindow/mainwindow.h"
#include "./shared/app_constants.h"

#include "HHSharedCore/hlogdebug.h"



void showSplashMessage(QSplashScreen *s, const QString &str){
    s->showMessage(str, Qt::AlignRight | Qt::AlignBottom, Qt::darkGreen);
}
 

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(resources);

    QApplication a(argc, argv);


    a.addLibraryPath(QCoreApplication::applicationDirPath());
    a.addLibraryPath(QCoreApplication::applicationDirPath() + QDir::separator () + QString(PLUGINS_MAIN_DIR));
    a.addLibraryPath(QCoreApplication::applicationDirPath() + QDir::separator () + QString(MYLIBS_DIR));
    qDebug()<<"----main(....)~~Library Paths"<<QCoreApplication::libraryPaths ();


    for(int i = 0; i < argc; i++){
        if(QString(argv[i]).toLower() == "-log"){
            qInstallMessageHandler(logDebug);
            qAddPostRoutine(closeDebugLog);
        }
    }





    //创建Splash Screen
    //Create Splash Screen
    QSplashScreen *splash = new QSplashScreen( QPixmap(QString(RESOURCE_PATH)+QString(APP_SPLASH_IMAGE_PATH)) );
    splash->show();

    //设置程序信息
    //Setup the program information
    showSplashMessage(splash, QObject::tr("Setup Application Info"));
    a.setApplicationName(APP_NAME);
    a.setApplicationVersion(APP_VERSION);
    a.setWindowIcon(QIcon(QString(RESOURCE_PATH)+QString(APP_ICON_PATH)));
    a.setOrganizationName(APP_ORG);
    a.setOrganizationDomain(APP_ORG_DOMAIN);
    showSplashMessage(splash, QString(APP_NAME) + " " + QString(APP_VERSION));


    //创建主窗口
    //Create the main window
    HEHUI::MainWindow w;
    showSplashMessage(splash, QObject::tr("Loading Plugins"));
    w.loadPlugins();
    w.show();

//    HEHUI::Screenshot w;
//    w.show();


    //结束Splash Screen
    //Finish the Splash Screen
    splash->finish(&w);
    delete splash;


    return a.exec();

}
