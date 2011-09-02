#ifndef SYSINFODLG_H
#define SYSINFODLG_H

#include <QWidget>
#include <QProcess>
#include <QProgressBar>



//#include "../ui/ui_systeminfo.h"
#include "ui_systeminfo.h"

//#include "../../shared/gui/databaseconnecter/databaseconnecter.h"
#include "HHSharedGUI/hdatabaseconnecter.h"


class SystemInfo: public QMainWindow {
	Q_OBJECT

public:

        SystemInfo(bool isYDAdmin = false, QWidget *parent = 0);
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

	void slotCleaner();
	void slotCleanTempFiles(const QString &path);

	void slotBackupMyDocs();
	void slotBackupEmails();

	void slotUploadSystemInfo();

	void slotUploadSystemInfoToSitoyDBServer();

    void slotQuerySystemInfo();
    unsigned int createPropertyNO();
    bool slotIsPropertyNOValid();


     //重新设置界面信息
    void slotResetAllInfo();
     //重新从界面上获取信息
    void slotGetAllInfo();



    void on_idLineEdit_editingFinished();
    void on_deptComboBox_currentIndexChanged(const QString & text);
    void on_dateEditMotherboardDate_dateChanged ( const QDate & date );
    void on_toolButtonQuery_clicked();
    void on_toolButtonUpload_clicked();
    void on_toolButtonScan_clicked();


private:
	void initStatusBar();
	void copyFiles(const QStringList &list);
	bool copyFile(const QString &source, const QString &target, bool confirmFileExists);


private:
	Ui::SystemInfoMainWindowUI ui;

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


	//OS Info
	QString os;
	QString installationDate;
	QString workgroup;
	QString computerName;
	QString windowsDir;

	QStringList drivesInfo;

        QStringList usersInfo;


	//User Info
	QString userName;
	bool isAdmin;
	QString myDocuments;

	QString emailStoreRoot;
	int emailFolderSize;
	QString biggestDBXFile;

	int tempDirSize;
	int ieTempDirSize;

        //Devices Info
        QString cpu;
        QString dmiUUID;
        QString motherboardName;
        QString chipset;
        QString memory;

        QString video;
        QString monitor;
        QString audio;
        QString partitionsTotalSize;

        QStringList network1Info;
        QStringList network2Info;

        //Other Info
	QString pNo;
	QString remark;
	QString dept;
	QString area;

	QDate motherboardMDate;
	QDate monitorMDate;

        QString serviceNumber;

	bool recordExists;

    QRegExpValidator *validator;

    DatabaseConnecter *dc;

    bool isYDAdmin;


};

#endif // SYSINFODLG_H

