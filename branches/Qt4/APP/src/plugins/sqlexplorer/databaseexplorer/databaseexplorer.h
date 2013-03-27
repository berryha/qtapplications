#ifndef DATABASEEXPLORER_H
#define DATABASEEXPLORER_H

#include <QWidget>

#include "ui_databaseexplorer.h"
#include "../dataexplorer/dataexplorer.h"

class QTreeWidget;
class QTreeWidgetItem;
class QSqlDatabase;
class QMenu;



class DatabaseExplorer: public QWidget {
Q_OBJECT
public:
	DatabaseExplorer(QWidget *parent = 0);

	void initUI();

	 QSqlDatabase activeDatabase() const;

         void setNewlyCreatedconnections(const QStringList &connections);

protected:
	void languageChange();


signals:
	void signalOpenTable(const QString &tableName, bool openInNewPage);
        void signalShowRecordCountRequested(const QString &tableName);

	//void signalTableActivated(const QString &table);
	//void signalOpenTableInNewTabPage(const QString &table);
	void signalMetaDataRequested(const QString &tableName);
	void signalConnectDatabaseRequested();
	void signalCloseDatabaseRequested(const QString &connectionName);


public slots:
	void slotRefresh();




private slots:
	void on_databaseList_itemActivated(QTreeWidgetItem *item, int column);
	void on_databaseList_currentItemChanged(QTreeWidgetItem *current,
		QTreeWidgetItem *previous);

	void slotShowTreeItemMenu(const QPoint &pos);
	//void closeDatabaseConnection(const QString &connectionName);


private:
	Ui::DatabaseExplorerUI ui;

	void setActiveItem(QTreeWidgetItem *);


//	QMenu *itemPopupMenu;


	QString activeDb;

        QStringList m_newlyCreatedconnections;



};

#endif // DATABASEEXPLORER_H
