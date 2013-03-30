
#include <QtSql>
#include <QtDebug>
#include <QMenu>


#include "databaseexplorer.h"


DatabaseExplorer::DatabaseExplorer(QWidget *parent) :
	QWidget(parent)
{
	ui.setupUi(this);
	initUI();

	slotRefresh();

}

void DatabaseExplorer::languageChange()
{

	ui.retranslateUi(this);

}

void DatabaseExplorer::initUI(){
	ui.databaseList->setUniformRowHeights(true);
	ui.databaseList->header()->setStretchLastSection(false);
    ui.databaseList->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
	ui.databaseList->setItemHidden(ui.databaseList->headerItem(), true);

//        itemPopupMenu = new QMenu(this);
//        itemPopupMenu->addAction(ui.actionOpenTableInNewTabPage);
//        itemPopupMenu->addAction(ui.actionShowRecordCount);
//        itemPopupMenu->addAction(ui.actionShowMetadata);
//        itemPopupMenu->addAction(ui.actionRefresh);
//        itemPopupMenu->addSeparator();
//        itemPopupMenu->addAction(ui.actionConnectDatabase);
//        itemPopupMenu->addAction(ui.actionDisconnectDatabase);

	connect(ui.databaseList, SIGNAL(customContextMenuRequested(QPoint)), this,
			SLOT(slotShowTreeItemMenu(QPoint)));



}


void DatabaseExplorer::slotShowTreeItemMenu(const QPoint &pos) {

	QTreeWidget *treeWidget = qobject_cast<QTreeWidget*> (sender());

	if (!treeWidget){
		return;
	}

	QTreeWidgetItem *item = treeWidget->itemAt(pos);

	//刷新Actions
	//Refresh actions
	if (!item){
		on_databaseList_currentItemChanged(0, 0);
		//return;
	}else{
		on_databaseList_currentItemChanged(item, 0);
	}

        QMenu itemPopupMenu(this);
        itemPopupMenu.addAction(ui.actionOpenTableInNewTabPage);
        itemPopupMenu.addAction(ui.actionShowRecordCount);
        itemPopupMenu.addAction(ui.actionShowMetadata);
        itemPopupMenu.addAction(ui.actionRefresh);
        itemPopupMenu.addSeparator();
        itemPopupMenu.addAction(ui.actionConnectDatabase);
        itemPopupMenu.addAction(ui.actionDisconnectDatabase);


        QAction *action = itemPopupMenu.exec(treeWidget->viewport()->mapToGlobal(pos));
	if (action == ui.actionOpenTableInNewTabPage){
		if (!item->parent()){
			return;
		}
		setActiveItem(item->parent());
		emit signalOpenTable(item->text(0),true);

        }else if (action == ui.actionShowRecordCount) {
            if (!item->parent()){
                    return;
            }
            setActiveItem(item->parent());
            emit signalShowRecordCountRequested(item->text(0));

        }else if (action == ui.actionShowMetadata) {
		if (!item->parent()){
			return;
		}
		setActiveItem(item->parent());
		emit signalMetaDataRequested(item->text(0));

	} else if(action == ui.actionRefresh){
		slotRefresh();
	}else if(action == ui.actionConnectDatabase){
		emit signalConnectDatabaseRequested();
	}else if(action == ui.actionDisconnectDatabase){
		emit signalCloseDatabaseRequested(item->data(0, Qt::UserRole).toString());
		//closeDatabaseConnection(item->data(0, Qt::UserRole).toString());
	}

}

static QString qDBCaption(const QSqlDatabase &db) {
	//QString nm = db.driverName().remove(0, 1);
	QString nm = db.driverName();
	nm.append(QLatin1Char(':'));
	if (!db.userName().isEmpty()) {
		nm.append(db.userName()).append(QLatin1Char('@'));
		nm.append(db.hostName());
		nm.append(QLatin1Char('/'));
		nm.append(db.databaseName());
	} else if (QString(db.databaseName()).startsWith(
			"DRIVER={Microsoft Access Driver (*.mdb)}")) {
		nm.append(QString(db.databaseName()).section('=', 3, 3));
	} else {
		nm.append(db.databaseName());
		//nm.append(QFileInfo(db.databaseName()).fileName());
	}
	return nm;
}

QSqlDatabase DatabaseExplorer::activeDatabase() const {
	return QSqlDatabase::database(activeDb);
}

void DatabaseExplorer::setNewlyCreatedconnections(const QStringList &connections){
    this->m_newlyCreatedconnections = connections;
    slotRefresh();
}

static void qSetBold(QTreeWidgetItem *item, bool bold) {
	QFont font = item->font(0);
	font.setBold(bold);
	item->setFont(0, font);
}

void DatabaseExplorer::setActiveItem(QTreeWidgetItem *item) {
	for (int i = 0; i < ui.databaseList->topLevelItemCount(); ++i) {
		if (ui.databaseList->topLevelItem(i)->font(0).bold())
			qSetBold(ui.databaseList->topLevelItem(i), false);
	}

        if (!item){
            return;
        }


	qSetBold(item, true);
	activeDb = QSqlDatabase::connectionNames().value(ui.databaseList->indexOfTopLevelItem(item));
        item->setExpanded(true);

//        if(!m_newlyCreatedconnections.contains(activeDb)){
//            item->setForeground(0, QBrush(Qt::red));
//        }


}


/*
void DatabaseExplorer::closeDatabaseConnection(const QString &connectionName){

	QMessageBox::StandardButton reply;
	reply = QMessageBox::question(this, tr("Confirm"), tr("Really close the connection?<br>%1").arg(connectionName),
				QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
	if (reply == QMessageBox::No) {
		return;
	}else{
		//TODO:关闭所有相关的TabPage
		//Close all related TabPage
		QSqlDatabase::database(connectionName).close();
		QSqlDatabase::removeDatabase(connectionName);
		slotRefresh();
	}

}
*/

void DatabaseExplorer::slotRefresh() {
	ui.databaseList->clear();
	QStringList connectionNames = QSqlDatabase::connectionNames();

	if(!connectionNames.size()){
		emit signalConnectDatabaseRequested();
		return;
	}

	bool gotActiveDb = false;
	for (int i = 0; i < connectionNames.count(); ++i) {
		QTreeWidgetItem *root = new QTreeWidgetItem(ui.databaseList);
		QSqlDatabase db = QSqlDatabase::database(connectionNames.at(i), false);
		root->setText(0, qDBCaption(db));
		root->setData(0, Qt::UserRole, connectionNames.at(i));
		if (connectionNames.at(i) == activeDb) {
			gotActiveDb = true;
			setActiveItem(root);

		}

                bool isNewlyCreatedconnection = m_newlyCreatedconnections.contains(connectionNames.at(i));
                if(!isNewlyCreatedconnection){
                    root->setData(0, Qt::ForegroundRole, QBrush(Qt::red));
                    root->setIcon(0, QIcon(":/resources/images/warning.png"));
                }

                if (db.isOpen()) {
                    QStringList tables = db.tables();
                    for (int t = 0; t < tables.count(); ++t) {
                        QTreeWidgetItem *table = new QTreeWidgetItem(root);
                        table->setText(0, tables.at(t));
                        if(!isNewlyCreatedconnection){
                            table->setData(0, Qt::ForegroundRole, QBrush(Qt::red));
                        }
                    }
                }


	}

	if (!gotActiveDb) {
		activeDb = connectionNames.value(0);
		setActiveItem(ui.databaseList->topLevelItem(0));
	}

	ui.databaseList->doItemsLayout(); // HACK
}


void DatabaseExplorer::on_databaseList_itemActivated(QTreeWidgetItem *item, int /* column */) {

    if (!item){
        return;
    }

    if (!item->parent()) {
        setActiveItem(item);
    } else {
        setActiveItem(item->parent());
        emit signalOpenTable(item->text(0), false);
    }

}

void DatabaseExplorer::on_databaseList_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *) {
	ui.actionOpenTableInNewTabPage->setEnabled(current && current->parent());
	ui.actionShowMetadata->setEnabled(current && current->parent());
        ui.actionShowRecordCount->setEnabled(current && current->parent());
	ui.actionDisconnectDatabase->setEnabled(current && !current->parent());
}

