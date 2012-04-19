
#include <QtGui>
#include <QtGui/QApplication>
//#include <QtGui/QDesktopWidget>
//#include <QtGui/QLabel>
//#include <QtCore/QDir>
//#include <QtCore/QSettings>


#include "mainwindow.h"

#include "HHSharedCore/hlogdebug.h"




int main(int argc, char **argv)
{
    Q_INIT_RESOURCE(resources);

    QApplication a(argc, argv);

    for(int i = 0; i < argc; i++){
        if(QString(argv[i]).toLower() == "-log"){
            qInstallMsgHandler(logDebug);
            qAddPostRoutine(closeDebugLog);
        }
    }


    HEHUI::MainWindow mw;
    mw.show();


    return a.exec();

}

