#ifndef SQLEXPLORER_H
#define SQLEXPLORER_H

#include <QMainWindow>
//#include "../ui/ui_sqlexplorer.h"
#include "ui_sqlexplorer.h"


namespace HEHUI{

class SqlExplorer : public QMainWindow
{
    Q_OBJECT

public:
    SqlExplorer(QWidget *parent = 0);
    ~SqlExplorer();

    static bool isRunning(){return running;}

    void setConfirmOnCloseDBConnection(bool confirm);

protected:
    void languageChange();
    void closeEvent(QCloseEvent *e);

private:
    void initUI();

    void openTable(DataExplorer *de, const QSqlDatabase &database, const QString &tableName);

    void updateWindowTitle();


public slots:
    void slotTabPageChanged();
    DataExplorer *slotNewDataExplorerTab(const QString &tabText= QString(tr("Untitled")));
    void slotcloseTab();

    void slotOpenTable(const QString &tableName, bool openInNewPage);
    void slotShowTableRecordCount(const QString &tableName);
    void slotShowMetaData(const QString &tableName);
    void slotShowQueryResult(const QString &queryString);
    void slotConnectDatabase();
    void slotCloseDatabase(const QString &connectionName);


private slots:
    void slotUpdateFrames();
    void slotUpdateDatabaseComboBox(bool resetComboBox);
    void slotUpdateTableComboBox(bool resetComboBox);

    void on_databaseComboBox_activated ( const QString & text);
    void on_tableComboBox_activated ( const QString & text);
    void on_pagingCheckBox_stateChanged ( int state );
    void on_metadataToolButton_clicked ();


private:
    Ui::SqlExplorerUI ui;


    QList<QPair<DataExplorer*,QString> > dataExplorerTabPages;

    static bool running;

    QStringList newlyCreatedconnections;

    bool m_confirmOnCloseDBConnection;





};

} //namespace HEHUI

#endif // SQLEXPLORER_H
