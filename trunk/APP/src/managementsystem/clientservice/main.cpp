

//#include <QtGui/QApplication>
//#include <QtGui/QDesktopWidget>
//#include <QtGui/QLabel>
#include <QtCore/QDir>
#include <QtCore/QSettings>

#include "app_constants.h"
#include "clientservice/clientservice.h"

#include "HHSharedCore/hlogdebug.h"




int main(int argc, char **argv)
{
    Q_INIT_RESOURCE(resources);

//#if !defined(Q_WS_WIN)
//    // QtService stores service settings in SystemScope, which normally require root privileges.
//    // To allow testing this example as non-root, we change the directory of the SystemScope settings file.
//    QSettings::setPath(QSettings::NativeFormat, QSettings::SystemScope, QDir::tempPath());
//    qWarning("(Example uses dummy settings file: %s/QtSoftware.conf)", QDir::tempPath().toLatin1().constData());
//#endif


    //HEHUI::ClientService service(argc, argv, "ClientService", "Client Service For Computer Management System");
    HEHUI::ClientService service(argc, argv, SERVICE_NAME, APP_NAME);

    cout<<qPrintable(QString(APP_NAME) + " Build " + QString(APP_VERSION))<<endl<<endl;

    //reset the message handler
    qInstallMsgHandler(0);

    for(int i = 0; i < argc; i++){
        if(QString(argv[i]).toLower() == "-log"){
            qInstallMsgHandler(logDebug);
            qAddPostRoutine(closeDebugLog);
        }else if(QString(argv[i]).toLower() == "-setwallpaper"){
            QString imagePath = "";
            if(i < (argc - 1) ){
                imagePath = QString::fromLocal8Bit(argv[i + 1]);
                //qDebug()<<imagePath;
            }
            service.setDeskWallpaper(imagePath);
        }
    }



    return service.exec();

}

