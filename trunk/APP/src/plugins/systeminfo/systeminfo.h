#ifndef SYSINFODLG_H
#define SYSINFODLG_H


#include <QMainWindow>
#include <QProcess>
#include <QProgressBar>



#include "ui_systeminfo.h"

#include "HHSharedGUI/hdatabaseconnecter.h"

namespace HEHUI {


class SystemInfo: public QMainWindow {
    Q_OBJECT

public:

    SystemInfo(const QString &adminName, QWidget *parent = 0);
    ~SystemInfo();

    static bool isRunning(){return running;}

    void retranslateUi();

protected:
    //bool event ( QEvent * event );
    void closeEvent(QCloseEvent *);
    bool eventFilter(QObject *obj, QEvent *event);
    void languageChange();


public slots:



private slots:
    void slotResetStatusBar(bool show);

    void slotScanSystem();
    void slotScannerExit( int exitCode, QProcess::ExitStatus exitStatus);
    void slotReadReport();


    void slotUploadSystemInfo();
    void slotUploadSystemInfoToSitoyDB();


//    void slotUploadSystemInfoToSitoyDBServer();

    void slotQuerySystemInfo();


    //重新设置界面信息
    void slotResetAllInfo();
    //重新从界面上获取信息
    void slotGetAllInfo();



    void on_toolButtonQuerySystemInfo_clicked();
    void on_toolButtonUpload_clicked();
    void on_toolButtonScan_clicked();

    void getNewComputerName();
    void on_pushButtonRenameComputer_clicked();

    void setComputerName(const QString &newName);

private:
    void initStatusBar();

    QString getEnvironmentVariable(const QString &environmentVariable);
    QString getJoinInformation(bool *isJoinedToDomain = 0, const QString &serverName = "");
    QString getComputerName();
    bool setComputerNameWithAPI(const QString &computerName);
    bool isNT6OS();


private:
    Ui::SystemInfoMainWindowUI ui;

    int m_sn;

    QString m_adminName;
    QString m_computerName;
    QString m_workgroup;
    QString m_users;
    QString m_os;

    static bool running;
    static qint64 failedFilesTotalSize;

    bool isScanning;
    //bool isUploaded;
    //bool isUploadedToSitoyDB;

    QWidget *m_progressWidget;
    QLayout* hlayout;
    QLabel *label;
    QProgressBar *progressBar;


    QProcess *process;
    QString m_systemInfoFilePath;

    bool recordExists;

//    QRegExpValidator *validator;

    DatabaseConnecter *dc;
    QSqlQueryModel *queryModel;

    bool isYDAdmin;

    static QMap<QString/*Short Name*/, QString/*Department*/>departments;



 ///////////////////////////////////
    //DevicesInfo
    QString cpu;
    QString memory;
    QString motherboardName;
    QString dmiUUID;
    QString chipset;
    QString video;
    QString monitor;
    QString audio;
    QStringList storages;
    QString storagesInfo;

    QString adapter1Name;
    QString adapter1HDAddress;
    QString adapter1IPAddress;
    QString nic1Info;
    QString adapter2Name;
    QString adapter2HDAddress;
    QString adapter2IPAddress;
    QString nic2Info;

    //OSInfo
    QString installationDate;
    QString windowsDir;
    QString osKey;

    //InstalledSoftwareInfo
    QStringList softwares;

    //
    QString serviceNumber;
    QString vender;
    QDate dateOfPurchase;
    QString warranty;
    QString registrant;
    QString remark;



};

}

#endif // SYSINFODLG_H

