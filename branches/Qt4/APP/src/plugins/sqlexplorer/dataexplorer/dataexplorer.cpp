//#include <QtGui>
//#include <QSqlTableModel>
#include <QtSql>
#include <QStandardItemModel>
#include <QApplication>
#include <QFileDialog>

 #include <QSqlQuery>

#include <QMessageBox>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QAction>
#include <QtEvents>
#include <QTextCodec>
#include <QStatusBar>
#include <QtDebug>


#include "dataexplorer.h"
//#include "dataiodialog.h"
#include "../databaseexplorer/databaseexplorer.h"

#include "HHSharedGUI/hdataoutputdialog.h"




namespace HEHUI{

DataExplorer::DataExplorer(/*MainWindow *w,*/ QWidget *parent) :
	QWidget(parent)//, mw(w), blockScroll(false),shiftPressed(false), newWindow(false)
{
	ui.setupUi(this);

	initUI();

}

DataExplorer::~DataExplorer(){


}

void DataExplorer::languageChange()
{
	ui.retranslateUi(this);
}


void DataExplorer::initUI(){

//	ui.dataViewTable->addAction(ui.actionInsertRow);
//	ui.dataViewTable->addAction(ui.actionDeleteRow);
//	ui.dataViewTable->addAction(ui.actionExportData);


	dataFileSavePath = QString(QDir::currentPath()+"/data");


	connect(ui.dataViewTable, SIGNAL(customContextMenuRequested(QPoint)), this,
		SLOT(slotShowCustomContextMenu(QPoint)));


}



void DataExplorer::setActiveDatabase(const QSqlDatabase & database){

	curDatabase = database;


}


QSqlDatabase DataExplorer::activeDatabase() const{
	return curDatabase;
}


QString DataExplorer::activeDatabaseName() const{
	QString databaseName = activeDatabase().databaseName();
	if (databaseName.startsWith("DRIVER={Microsoft Access Driver (*.mdb)}")) {
		databaseName = databaseName.section('=', 3, 3);
	}
	return databaseName;
}


QString DataExplorer::activeTable() const{
	return curTable;
}


int DataExplorer::rowCount(){
	QAbstractItemModel *model = ui.dataViewTable->model();
		if(!model){
			return 0;
		}

        return model->rowCount();

}

quint32 DataExplorer::recordCountInTable(const QSqlDatabase & database, const QString &tableName){

    quint32 count = 0;
    QString queryString = QString("SELECT COUNT(*) AS '%1' FROM %2 ").arg(tr("Record Count")).arg(tableName);

    QSqlQuery query(database);
    query.exec(queryString);
    if(query.lastError().type() == QSqlError::NoError){
        query.first();
        count = query.value(0).toUInt();
    }else{
        qWarning()<<QString("Can not fetch the record count of table '%1'! %2").arg(tableName).arg(query.lastError().text());
    }

    return count;

}

void DataExplorer::slotInsertRow() {
    QSqlTableModel *model = qobject_cast<QSqlTableModel *>(ui.dataViewTable->model());
	if (!model)
		return;

	QModelIndex insertIndex = ui.dataViewTable->currentIndex();
	int row = insertIndex.row() == -1 ? 0 : insertIndex.row();
	model->insertRow(row);
	insertIndex = model->index(row, 0);
	ui.dataViewTable->setCurrentIndex(insertIndex);
	ui.dataViewTable->edit(insertIndex);

	updateActions();
}

void DataExplorer::slotDeleteRow() {

/*
	QMessageBox msgBox(QMessageBox::question, tr("Dangerous Operation"),
			tr("<b>Really delete data? </b>"), 0, this);

	msgBox.addButton(tr("&Yes"), QMessageBox::AcceptRole);
	//     msgBox.addButton(tr("&Cancel"), QMessageBox::RejectRole);
	msgBox.setDefaultButton(msgBox.addButton(tr("&Cancel"),
			QMessageBox::RejectRole));
*/


	QMessageBox::StandardButton reply;
	reply = QMessageBox::question(this, tr("Dangerous Operation"), tr("<font color=red>Really delete data?</font>"),
				QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
	if (reply == QMessageBox::No) {
		return;
	}

	//if (msgBox.exec() == QMessageBox::AcceptRole) {

		QSqlTableModel *model =
				qobject_cast<QSqlTableModel *>(ui.dataViewTable->model());
		if (!model)
			return;

		model->setEditStrategy(QSqlTableModel::OnManualSubmit);

		QModelIndexList currentSelection = ui.dataViewTable->selectionModel()->selectedIndexes();
		for (int i = 0; i < currentSelection.count(); ++i) {
			if (currentSelection.at(i).column() != 0)
				continue;
			model->removeRow(currentSelection.at(i).row());
		}

		model->submitAll();
		model->setEditStrategy(QSqlTableModel::OnRowChange);

		updateActions();

	//} else
	//	return;

}


bool DataExplorer::slotExportData() {

        DataOutputDialog dlg(ui.dataViewTable, DataOutputDialog::EXPORT, this);

	dlg.exec();
	return true;

}

void DataExplorer::slotImportData(){
	QMessageBox::information(this,tr("~_~"),tr(" Not accomplished !"));

}


void DataExplorer::slotDataPrint(){
#ifndef QT_NO_PRINTER
//TODO
        DataOutputDialog dlg(ui.dataViewTable, DataOutputDialog::PRINT, this);
	dlg.exec();
#endif

}

void DataExplorer::slotDataPrintPreview(){
#ifndef QT_NO_PRINTER
//TODO
	QMessageBox::information(this,tr("~_~"),tr(" Not accomplished !"));
#endif

}



void DataExplorer::updateActions() {
	bool enableIns = qobject_cast<QSqlTableModel *>(ui.dataViewTable->model());
	//bool enableDel = enableIns && ui.dataViewTable->currentIndex().isValid();
	bool enableExp = ui.dataViewTable->currentIndex().isValid() && ui.dataViewTable->selectionModel()->selectedIndexes().size();
	bool enableDel =  enableIns&& enableExp;


	ui.actionInsertRow->setEnabled(enableIns);
	ui.actionDeleteRow->setEnabled(enableDel);
	ui.actionExportData->setEnabled(enableExp);
	ui.actionImportData->setEnabled(enableIns);
	ui.actionPrint->setEnabled(enableExp);


}

void DataExplorer::resetTableViewModel(){
	//删除先前的Model以解除可能的表锁定(e.g. M$ Access)
	//Delete the previous model to release the table(e.g. M$ Access)
	QAbstractItemModel *m = ui.dataViewTable->model();
	if(m){
		delete m;
		//ui.dataViewTable->model()->deleteLater();
	}

	QStandardItemModel *model = new QStandardItemModel();
	ui.dataViewTable->setModel(model);

	curTable = "";

}
  
void DataExplorer::showDataInTableView(const QSqlDatabase & database, const QString &string, OperationType operationType){
	if(!database.isValid()){
		QMessageBox::critical(this, tr("Fatal Error"), tr("Invalid database '%1'!").arg(database.databaseName()));
		return;
	}else if(database.connectionName() != curDatabase.connectionName() || database.databaseName() != curDatabase.databaseName()){
		setActiveDatabase(database);
	}

	if (string.isEmpty()){
		QMessageBox::critical(this, tr("Fatal Error"), tr("Empty string is not accepted!"));
		return;
	}


	//删除先前的Model以解除可能的表锁定(e.g. M$ Access)
	//Delete the previous model to release the table(e.g. M$ Access)
	QAbstractItemModel *model = ui.dataViewTable->model();
	if(model){
		delete model;
		//ui.dataViewTable->model()->deleteLater();
	}



	switch (operationType){
	case SHOW_TABLE_DATA:
		showTableData(/*database,*/ string);
		break;
        case SHOW_TABLE_RECORD_COUNT:
                showTableRecordCount(/*database,*/ string);
                break;
	case SHOW_TABLE_METADATA:
		showMetaData(/*database,*/ string);
		break;
	case SHOW_QUERY_RESULT:
		showQueryResult(/*database,*/ string);
		break;
	}



	updateActions();

}


void DataExplorer::showTableData(/*const QSqlDatabase & database,*/ const QString &tableName) {

	if(!activeDatabase().tables().contains(tableName,Qt::CaseInsensitive)){
		QMessageBox::critical(this, tr("Fatal Error"), tr("Unable to find table <b>'%1'</b> in database <b>'%2'</b>!").arg(tableName).arg(activeDatabase().databaseName()));
				return;
	}

	curTable = tableName;

        quint32 recordCount = recordCountInTable(curDatabase, curTable);
        if(recordCount > 1000){
            int rep = QMessageBox::question(this, tr("Too Many Records"),
                                            tr("<p>There are as many as <b>%1</b> records in table '%2'!</p>"
                                            " <p><font color = 'red'><b>The application may crash or it may appear to be frozen when showing all the content!</b></font></p>"
                                            "<p>Do you still want to continue?</p>").arg(recordCount).arg(curTable),
                                            QMessageBox::Yes | QMessageBox::No,
                                            QMessageBox::No
                                            );
            if(rep == QMessageBox::No){
                return;
            }
        }


        QSqlTableModel *model = new QSqlTableModel(ui.dataViewTable, curDatabase);
	model->setEditStrategy(QSqlTableModel::OnRowChange);
	model->setTable(tableName);
	//model->setFilter("id>500");
	model->select();

	if (model->lastError().type() != QSqlError::NoError) {
		QMessageBox::critical(this, tr("Fatal Error"), tr("%1")
		.arg(model->lastError().text()) );
		return;
        }

        while(model->canFetchMore()){
            model->fetchMore();
        }

	ui.dataViewTable->setModel(model);

	ui.dataViewTable->setEditTriggers(QAbstractItemView::DoubleClicked
			|QAbstractItemView::EditKeyPressed);


}

void DataExplorer::showTableRecordCount(/*const QSqlDatabase & database,*/ const QString &tableName){

    curTable = tableName;
    QString queryString = QString("SELECT COUNT(*) AS '%1' FROM %2").arg(tr("Record Count")).arg(tableName);

    QSqlQueryModel *model = new QSqlQueryModel(ui.dataViewTable);
    model->setQuery(QSqlQuery(queryString, activeDatabase()));
    if (model->lastError().type() != QSqlError::NoError) {
            //mw->statusBar()->showMessage(model->lastError().text());
            QMessageBox::warning(this, tr("Error!"), tr("%1")
            .arg(model->lastError().text()) );
            return;

    } else if(model->query().isSelect()){
            ui.dataViewTable->setModel(model);
    }


}

void DataExplorer::showMetaData(/*const QSqlDatabase & database,*/ const QString &tableName) {

	if(!activeDatabase().tables().contains(tableName,Qt::CaseInsensitive)){
		QMessageBox::critical(this, tr("Fatal Error"), tr("Unable to find table <b>'%1'</b> in database <b>'%2'</b>!").arg(tableName).arg(activeDatabase().databaseName()));
				return;
	}

	curTable = tableName;

	QSqlRecord rec = activeDatabase().record(tableName);
	QStandardItemModel *model = new QStandardItemModel();

	model->insertRows(0, rec.count());
	model->insertColumns(0, 7);

	model->setHeaderData(0, Qt::Horizontal, "Fieldname");
	model->setHeaderData(1, Qt::Horizontal, "Type");
	model->setHeaderData(2, Qt::Horizontal, "Length");
	model->setHeaderData(3, Qt::Horizontal, "Precision");
	model->setHeaderData(4, Qt::Horizontal, "Required");
	model->setHeaderData(5, Qt::Horizontal, "AutoValue");
	model->setHeaderData(6, Qt::Horizontal, "DefaultValue");

	for (int i = 0; i < rec.count(); ++i) {
		QSqlField fld = rec.field(i);
		model->setData(model->index(i, 0), fld.name());
		model->setData(model->index(i, 1),
				fld.typeID() == -1 ? QString(QVariant::typeToName(fld.type()))
						: QString("%1 (%2)").arg(QVariant::typeToName(fld.type())).arg(fld.typeID()));
		model->setData(model->index(i, 2), fld.length());
		model->setData(model->index(i, 3), fld.precision());
		model->setData(model->index(i, 4),
				fld.requiredStatus() == -1 ? QVariant("?") : QVariant(bool(fld.requiredStatus())));
		model->setData(model->index(i, 5), fld.isAutoValue());
		model->setData(model->index(i, 6), fld.defaultValue());
	}

	ui.dataViewTable->setModel(model);
	ui.dataViewTable->setEditTriggers(QAbstractItemView::NoEditTriggers);



}

void DataExplorer::showQueryResult(/*const QSqlDatabase & database,*/ const QString &queryString) {

	curTable = "";


        QString source = queryString;

        QSqlQuery query(activeDatabase());
        QString statement = "";

        QTextStream in(&source);
        in.setAutoDetectUnicode(true);
        while (!in.atEnd()) {
            QString line = in.readLine();

            if(line.startsWith("--", Qt::CaseInsensitive)){
                qDebug();
                qDebug("Single Line Comment Ignored:\n%s", qPrintable(line));
                qDebug();

                continue;
            }

            statement += (statement.isEmpty()?line:("\n" + line));

            QString temp = line.trimmed();
            temp.replace(";", "");
            if(statement.trimmed().startsWith("/*", Qt::CaseInsensitive) && temp.endsWith("*/", Qt::CaseInsensitive)){
                qDebug();
                qDebug("Multi Line Comment Ignored:\n%s", qPrintable(statement));
                qDebug();

                statement = "";
                continue;
            }

            if(line.endsWith(";") || in.atEnd()){
                if(!query.exec(statement)){
                    QSqlError error = query.lastError();
                    QString msg = QString("<p>Can not excute SQL!</p> %1 Error Type:%2 Error NO.:%3").arg(error.text()).arg(error.type()).arg(error.number());
                    QMessageBox::critical(this, tr("Error!"), msg );
                    qWarning();
                    qCritical("Can not excute SQL statement :\n%s\n%s", qPrintable(statement), qPrintable(error.text()));
                    qWarning();

                    statement = "";
                    continue;
                }else{
                    qDebug();
                    qDebug("SQL Statement Excuted:\n%s", qPrintable(statement));
                    qDebug();

                    statement = "";
                }
            }

        }






	QSqlQueryModel *model = new QSqlQueryModel(ui.dataViewTable);
        //model->setQuery(QSqlQuery(queryString, activeDatabase()));
        model->setQuery(query);

//	if (model->lastError().type() != QSqlError::NoError) {
//                QMessageBox::critical(this, tr("Error!"), tr("%1").arg(model->lastError().text()) );
//		return;

//	} elseif(model->query().isSelect()){

        if(queryString.contains("select", Qt::CaseInsensitive)){
            while(model->canFetchMore()){
                model->fetchMore();
            }
            //ui.dataViewTable->setModel(model);
        }
        if(queryString.contains("alter", Qt::CaseInsensitive)
				|| queryString.startsWith("drop", Qt::CaseInsensitive)
				|| queryString.startsWith("create", Qt::CaseInsensitive)
				|| queryString.startsWith("use", Qt::CaseInsensitive)
				){


		//TODO:更新数据库信息,更新所有的TabPage
		//TODO:Update database Info, update all TabPage
		emit signalUpdateTablesRequested();
	}


        ui.dataViewTable->setModel(model);





/*
	QRegExp regexp("\\s+from\\s+\\w+");
	int index = regexp.indexIn(queryString);
	if(index > -1){
		QString s = regexp.cap(0);
		QMessageBox::information(this, "s", s+"<br>"+QString::number(s.length()));
	}
*/
	

}


void DataExplorer::slotShowCustomContextMenu(const QPoint & pos){

	QTableView *tableView = qobject_cast<QTableView*> (sender());

	if (!tableView){
		return;
	}


	updateActions();

	QMenu menu(this);
	menu.addAction(ui.actionInsertRow);
	menu.addAction(ui.actionDeleteRow);
	menu.addSeparator();
	menu.addAction(ui.actionExportData);
	menu.addAction(ui.actionImportData);

#ifndef QT_NO_PRINTER

	menu.addSeparator();

	ui.actionPrint->setShortcut(QKeySequence::Print);
	menu.addAction(ui.actionPrint);

//	ui.actionPrintPreview->setShortcut(Qt::CTRL + Qt::Key_P);
//  menu.addAction(ui.actionPrintPreview);

#endif


	menu.exec(tableView->viewport()->mapToGlobal(pos));

}

/*
bool DataExplorer::isKDERunning() const {
	return !qgetenv("KDE_FULL_SESSION").isEmpty();
}
*/




















} //namespace HEHUI



