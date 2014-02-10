/*
 * process.cpp
 *
 *  Created on: 2010-11-17
 *      Author: HeHui
 */



#include <QTextCodec>

#include <QDebug>

#include "process.h"




namespace HEHUI {

Process::Process(QObject *parent)
    :QObject(parent)
{
    process = new QProcess(this);
    process->setReadChannelMode(QProcess::MergedChannels);
    connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(readProcessOutput()));
    connect(process, SIGNAL(readyReadStandardError()), this, SLOT(readProcessError()));
    connect(process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(processErrorOccured(QProcess::ProcessError)));

    connect(process, SIGNAL(started()), this, SLOT(processStarted()));
    connect(process, SIGNAL(finished(int)), this, SLOT(stopProcess()));

    m_running = false;

    m_exeFilePath = "";

}

Process::~Process() {
    // TODO Auto-generated destructor stub
}

bool Process::isRunning(){
    return m_running;

}


void Process::startProcess(const QString &exeFilePath){

    if(process->state() == QProcess::NotRunning){
        m_exeFilePath = exeFilePath;
        process->start(exeFilePath);
    }else{
        m_running = true;
        emit signalProcessStateChanged(true, QString("The Process '%1' is already running!").arg(m_exeFilePath));
        //emit signalProcessOutputRead("Process is already running!");

    }

    //m_running = process->waitForStarted();


}

void Process::stopProcess(){

    if(process && (process->state() != QProcess::NotRunning)){
        process->kill();
        //emit signalProcessStateChanged(false, "Process Killed.");
    }

    //    delete process;
    //    process = 0;

    m_running = false;
    m_exeFilePath = "";

    emit signalProcessStateChanged(m_running, QString("Process exited with code %1").arg(process->exitCode()));



}

void Process::readProcessOutput(){

    QString output = QString::fromLocal8Bit(process->readAllStandardOutput());
    emit signalProcessOutputRead(output);


}

void Process::readProcessError(){

    QString output = QString::fromLocal8Bit(process->readAllStandardError());
    emit signalProcessOutputRead(output);


}

void Process::writeDataToProcess(const QString &data){

    process->write((data+"\n").toLocal8Bit());
    //process->write((data+"\n").toLatin1();

    //    QTextCodec *codec = QTextCodec::codecForName("UTF-16LE");
    //    process->write(codec->fromUnicode(data+"\n"));

    //QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-16LE"));

    // process->write((data+"\n").toStdString().c_str());


}

void Process::processErrorOccured(QProcess::ProcessError error){
    QString errorString = "";

    switch(error){
    case QProcess::FailedToStart:
        errorString = "The process failed to start. Either the invoked program is missing, or you may have insufficient permissions to invoke the program.";
        break;
    case QProcess::Crashed:
        errorString = "The process crashed some time after starting successfully.";
        break;
    case QProcess::Timedout:
        errorString = "The operation timed out.";
        break;
    case QProcess::WriteError:
        errorString = "An error occurred when attempting to write to the process. For example, the process may not be running, or it may have closed its input channel.";
        break;
    case QProcess::ReadError:
        errorString = "An error occurred when attempting to read from the process. For example, the process may not be running.";
        break;
    case QProcess::UnknownError:
        errorString = "An unknown error occurred.";
        break;
    default:
        errorString = "An unknown error occurred.";

    }

    if(process->state() == QProcess::NotRunning){
        m_running = false;
        emit signalProcessStateChanged(m_running, errorString);
    }else{
        emit signalProcessOutputRead(errorString);

    }



}

void Process::processStarted(){

    m_running = true;
    emit signalProcessStateChanged(m_running);

}














}
