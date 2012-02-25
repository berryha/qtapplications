

#include <QCoreApplication>
#include <QString>
#include <QDir>
#include <QSettings>
#include <QDebug>



#include "./systeminfo.h"


bool SystemInfo::running = false;

SystemInfo::SystemInfo(QObject *parent) :
    QObject(parent)
{



    running = true;

    process = 0;

    QDir::setCurrent(QCoreApplication::applicationDirPath ());
    //QString everestDirPath = QCoreApplication::applicationDirPath () + QString("/everest");
    everestDirPath = QString("./aida64business");

    m_systemInfoFilePath = everestDirPath + QString("/systeminfo.ini");




}

SystemInfo::~SystemInfo() {
    qDebug()<<"SystemInfo::~SystemInfo()";

    running = false;

    if(process){
        stopProcess();
    }


}



void SystemInfo::slotScanSystem(bool rescan) {
    qDebug()<<"SystemInfo::slotScanSystem()";

    if (!rescan && QFileInfo(systemInfoFilePath()).exists()) {
        emit signalScanFinished(true, "");
        return;
    }

    QString exeFilePath = everestDirPath + QString("/sysinfo.exe");

    if (!QFile(exeFilePath).exists()) {
        emit signalScanFinished(false, QString(tr("File '")+exeFilePath+tr("' Missing!")));
        running = false;
        return;
    }


    //QProcess *process = new QProcess(this);
    if(!process){
        process = new QProcess(this);
        process->setProcessChannelMode(QProcess::MergedChannels);
        connect(process, SIGNAL(finished( int , QProcess::ExitStatus)), this, SLOT(slotScannerExit( int , QProcess::ExitStatus )));
    }


    process->start(exeFilePath);



}

void SystemInfo::slotScannerExit( int exitCode, QProcess::ExitStatus exitStatus){
    if((exitCode != 0) || (exitStatus == QProcess::CrashExit)){
        emit signalScanFinished(false, QString("Scanner exited with error!"));
    }else{
        emit signalScanFinished(true, "");
    }

    running = false;


}

void SystemInfo::stopProcess(){

    if(process && (process->state() != QProcess::NotRunning)){
        process->kill();
        emit signalScanFinished(false, QString("Scanner process was killed!"));
    }

    delete process;
    process = 0;


}





