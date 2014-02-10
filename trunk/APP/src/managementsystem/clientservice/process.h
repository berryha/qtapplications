/*
 * process.h
 *
 *  Created on: 2010-11-17
 *      Author: HeHui
 */

#ifndef PROCESS_H_
#define PROCESS_H_


#include <QObject>
#include <QProcess>


namespace HEHUI {

class Process : public QObject{
    Q_OBJECT
public:
    Process(QObject *parent = 0);
    virtual ~Process();

    bool isRunning();

signals:
    void signalProcessStateChanged(bool running, const QString &message = "");
    void signalProcessOutputRead(const QString &output);


public slots:
    void startProcess(const QString &exeFilePath = "cmd.exe");
    void stopProcess();
    void writeDataToProcess(const QString &data);



private slots:
    void readProcessOutput();
    void readProcessError();
    void processErrorOccured(QProcess::ProcessError error);

    void processStarted();

private:

    QString m_exeFilePath;

    QProcess *process;

    bool m_running;


};

}

#endif /* PROCESS_H_ */
