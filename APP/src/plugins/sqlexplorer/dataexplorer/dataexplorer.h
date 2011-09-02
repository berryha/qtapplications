
#ifndef DATAEXPLORER_H
#define DATAEXPLORER_H

#include <QtGlobal>
#include <QSqlDatabase>

#include <QTextDocument>

//#include "./ui/ui_dataexplorer.h"
#include "ui_dataexplorer.h"

class QKeyEvent;
class QMime;
class QMouseEvent;
class QMenu;


namespace HEHUI{

class DataExplorer : public QWidget
{
    Q_OBJECT

public:

    enum OperationType{SHOW_TABLE_DATA = 0, SHOW_TABLE_RECORD_COUNT, SHOW_TABLE_METADATA, SHOW_QUERY_RESULT};


    DataExplorer( /*MainWindow *m,*/ QWidget *parent = 0);
    ~DataExplorer();

	

    void showDataInTableView(const QSqlDatabase & database, const QString &queryString, OperationType operationType);

    void resetTableViewModel();

    void setActiveDatabase(const QSqlDatabase & database);
    QSqlDatabase activeDatabase() const;


    QString activeDatabaseName() const;


    QString activeTable() const;

    int rowCount();

    quint32 recordCountInTable(const QSqlDatabase & database, const QString &tableName);


private:
    void initUI();

    void updateActions();

    void showTableData(/*const QSqlDatabase & database,*/ const QString &tableName);
    void showTableRecordCount(/*const QSqlDatabase & database,*/ const QString &tableName);
    void showMetaData(/*const QSqlDatabase & database,*/ const QString &tableName);
    void showQueryResult(/*const QSqlDatabase & database,*/ const QString &queryString);



	
	
signals:
    void signalUpdateTablesRequested();

protected:
	void languageChange();


public slots:

    void on_actionInsertRow_triggered()
    { slotInsertRow(); }
    void on_actionDeleteRow_triggered()
    { slotDeleteRow(); }
	void on_actionExportData_triggered()
    { slotExportData(); }
	void on_actionImportData_triggered()
    { slotImportData(); }
	void on_actionPrint_triggered()
    { slotDataPrint(); }
	void on_actionPrintPreview_triggered()
    { slotDataPrintPreview(); }


private slots:

	void slotShowCustomContextMenu(const QPoint & pos);

	void slotInsertRow();
	void slotDeleteRow();

	bool slotExportData();
	void slotImportData();

	void slotDataPrint();
	void slotDataPrintPreview();


private:
	Ui::DataExplorerUI ui;
	 	
	 
	QSqlDatabase curDatabase;


	QString curTable;
	QString dataFileSavePath;



};

} //namespace HEHUI

#endif // DATAEXPLORER_H
