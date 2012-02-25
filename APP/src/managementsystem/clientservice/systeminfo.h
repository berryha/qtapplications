#ifndef SYSINFODLG_H
#define SYSINFODLG_H

#include <QObject>
#include <QProcess>



class SystemInfo: public QObject {
    Q_OBJECT

public:

    SystemInfo(QObject *parent = 0);
    ~SystemInfo();

    static bool isRunning(){return running;}

    QString systemInfoFilePath() const{return m_systemInfoFilePath;}


signals:
    void signalScanFinished(bool ok, const QString &message);


public slots:
    void slotScanSystem(bool rescan = false);
    void stopProcess();


private slots:
    void slotScannerExit( int exitCode, QProcess::ExitStatus exitStatus);



private:

    static bool running;

    QProcess *process;

    QString everestDirPath;
    QString m_systemInfoFilePath;


};

#endif // SYSINFODLG_H

