#include <QtWidgets>
#include <QtSql>

#include "sqlexplorer.h"
//#include "../../shared/gui/databaseconnecter/databaseconnecter.h"
#include "HHSharedGUI/hdatabaseconnecter.h"
#include "HHSharedCore/hdatabaseutility.h"


namespace HEHUI{


bool SqlExplorer::running = false;

SqlExplorer::SqlExplorer(QWidget *parent)
    : QMainWindow(parent)
{
	ui.setupUi(this);

	initUI();

	running = true;

}

SqlExplorer::~SqlExplorer()
{

	running = false;
}

void SqlExplorer::languageChange()
{

	ui.retranslateUi(this);

}

void SqlExplorer::closeEvent(QCloseEvent *e) {

    foreach(QString connectionName, newlyCreatedconnections){
        slotCloseDatabase(connectionName);
    }

    if(newlyCreatedconnections.size() == 0){
        e->accept();
        deleteLater();
    }else{
        e->ignore();
    }


}

void SqlExplorer::initUI(){

    connect(ui.tabWidget, SIGNAL(currentChanged(int)),
            this, SLOT(slotTabPageChanged()));

    //QTabBar *tabBar = qFindChild<QTabBar*>(ui.tabWidget);
    QTabBar *tabBar = ui.tabWidget->tabBar();
    QStyleOptionTab opt;
    if (tabBar) {
        opt.init(tabBar);
        opt.shape = tabBar->shape();
    }

    // workaround for sgi style
    //QPalette pal = palette();
    // pal.setColor(QPalette::Active, QPalette::Button, pal.color(QPalette::Active, QPalette::Window));
    // pal.setColor(QPalette::Disabled, QPalette::Button, pal.color(QPalette::Disabled, QPalette::Window));
    // pal.setColor(QPalette::Inactive, QPalette::Button, pal.color(QPalette::Inactive, QPalette::Window));

    QToolButton *newTabButton = new QToolButton(this);
    ui.tabWidget->setCornerWidget(newTabButton, Qt::TopLeftCorner);
    newTabButton->setCursor(Qt::ArrowCursor);
    newTabButton->setAutoRaise(true);
    newTabButton->setIcon(QIcon(":/resources/images/addtab.png"));
    QObject::connect(newTabButton, SIGNAL(clicked()), this, SLOT(slotNewDataExplorerTab()));
    newTabButton->setToolTip(tr("Add Page"));

    QToolButton *closeTabButton = new QToolButton(this);
    //closeTabButton->setPalette(pal);
    ui.tabWidget->setCornerWidget(closeTabButton, Qt::TopRightCorner);
    closeTabButton->setCursor(Qt::ArrowCursor);
    closeTabButton->setAutoRaise(true);
    closeTabButton->setIcon(QIcon(":/resources/images/closetab.png"));
    QObject::connect(closeTabButton, SIGNAL(clicked()), this, SLOT(slotcloseTab()));
    closeTabButton->setToolTip(tr("Close Page"));
    //closeTabButton->setEnabled(false);




    //DatabaseExplorer
    connect(ui.databaseExplorer, SIGNAL(signalOpenTable(const QString &, bool)), this, SLOT(slotOpenTable(const QString &, bool)));
    connect(ui.databaseExplorer, SIGNAL(signalShowRecordCountRequested(const QString &)), this, SLOT(slotShowTableRecordCount(const QString &)));
    connect(ui.databaseExplorer, SIGNAL(signalMetaDataRequested(const QString &)), this, SLOT(slotShowMetaData(const QString &)));
    connect(ui.databaseExplorer, SIGNAL(signalConnectDatabaseRequested()), this, SLOT(slotConnectDatabase()));
    connect(ui.databaseExplorer, SIGNAL(signalCloseDatabaseRequested(const QString &)), this, SLOT(slotCloseDatabase(const QString &)));


    //SqlConsole
    connect(ui.sqlConsole, SIGNAL(signalQueryRequested(const QString &)), this, SLOT(slotShowQueryResult(const QString &)));


    ui.pagingCheckBox->setChecked(false);
    ui.pagingFrame->setEnabled(false);


    //删除默认的第一页,生成一个新的DataExplorer
    //Delete the first default page, create a new DataExplorer
    QWidget *page = ui.tabWidget->widget(0);
    ui.tabWidget->removeTab(0);
    delete page;
    slotNewDataExplorerTab();

    slotUpdateDatabaseComboBox(true);

}


/*
void SqlExplorer::slotOpenTable(const QString &tableName){
	DataExplorer *de = qobject_cast<DataExplorer *>(ui.tabWidget->currentWidget());
	openTable(de, tableName);
}

void SqlExplorer::slotOpenTableInNewTabPage(const QString &tableName){
	DataExplorer *de = new DataExplorer();
	openTable(de, tableName);
}
*/

void SqlExplorer::setConfirmOnCloseDBConnection(bool confirm){
    this->m_confirmOnCloseDBConnection = confirm;

}

void SqlExplorer::slotOpenTable(const QString &tableName, bool openInNewPage){
	DataExplorer *de;
	if(openInNewPage){
		de = slotNewDataExplorerTab(tableName);
	}else{
		de = qobject_cast<DataExplorer *>(ui.tabWidget->currentWidget());
		if(!de){
			slotOpenTable(tableName, true);
		}
	}

	openTable(de, ui.databaseExplorer->activeDatabase(), tableName);

}

void SqlExplorer::openTable(DataExplorer *de, const QSqlDatabase & database, const QString &tableName){
	if(!de){
		return;
	}

	de->showDataInTableView(database, tableName, DataExplorer::SHOW_TABLE_DATA);

	int index = ui.tabWidget->indexOf(de);
	ui.tabWidget->setTabText(index, tableName);

	updateWindowTitle();

	slotUpdateFrames();

}

void SqlExplorer::slotShowTableRecordCount(const QString &tableName){
        DataExplorer *de;
        de = qobject_cast<DataExplorer *>(ui.tabWidget->currentWidget());
        if(!de){
                de = slotNewDataExplorerTab(tableName+"|"+tr("Record Count"));
        }

        de->showDataInTableView(ui.databaseExplorer->activeDatabase(), tableName, DataExplorer::SHOW_TABLE_RECORD_COUNT);

        int index = ui.tabWidget->indexOf(de);
        ui.tabWidget->setTabText(index, tableName+"|"+tr("Record Count"));

        updateWindowTitle();

        slotUpdateFrames();

}

void SqlExplorer::slotShowMetaData(const QString &tableName){
	DataExplorer *de;
	de = qobject_cast<DataExplorer *>(ui.tabWidget->currentWidget());
	if(!de){
                de = slotNewDataExplorerTab(tableName+"|"+tr("Metadata"));
	}

	de->showDataInTableView(ui.databaseExplorer->activeDatabase(), tableName, DataExplorer::SHOW_TABLE_METADATA);

	int index = ui.tabWidget->indexOf(de);
        ui.tabWidget->setTabText(index, tableName+"|"+tr("Metadata"));

	updateWindowTitle();

	slotUpdateFrames();

}

void SqlExplorer::slotShowQueryResult(const QString &queryString){
	DataExplorer *de;
	de = qobject_cast<DataExplorer *>(ui.tabWidget->currentWidget());
	if(!de){
		de = slotNewDataExplorerTab(tr("Query Result"));
	}

	de->showDataInTableView(ui.databaseExplorer->activeDatabase(), queryString, DataExplorer::SHOW_QUERY_RESULT);

	int index = ui.tabWidget->indexOf(de);
	ui.tabWidget->setTabText(index, tr("Query Result"));

	updateWindowTitle();

	slotUpdateFrames();

}

void SqlExplorer::slotConnectDatabase(){
	DatabaseConnecter databaseConnecter(this);
        QString connectionName = "";
        if(!databaseConnecter.connectToNewDatabase(&connectionName)){
		return ;
	}

        qWarning()<<"connectionName:"<<connectionName;
        if(!newlyCreatedconnections.contains(connectionName)){
            newlyCreatedconnections.append(connectionName);
        }

        //ui.databaseExplorer->slotRefresh();
        ui.databaseExplorer->setNewlyCreatedconnections(newlyCreatedconnections);


	slotUpdateDatabaseComboBox(true);



}

void SqlExplorer::slotCloseDatabase(const QString &connectionName){

    if(!newlyCreatedconnections.contains(connectionName)){
        QMessageBox::critical(this, tr("Error"), tr("It's dangerous to close the database connection which was connected in other place!"));
        return;
    }

    if(m_confirmOnCloseDBConnection){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, tr("Confirm"), tr("Really close the connection? <br> %1").arg(connectionName),
                                      QMessageBox::YesToAll | QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (reply == QMessageBox::No) {
                return;
            }else if(reply == QMessageBox::YesToAll){
                m_confirmOnCloseDBConnection = false;
            }

    }


    //关闭所有相关的TabPage
    //Close all related TabPage
    int tabPages = ui.tabWidget->count();
    for(int i = tabPages; i >= 0; --i){
        DataExplorer *de = qobject_cast<DataExplorer *>(ui.tabWidget->widget(i));
        if(!de){continue;}
        if(de->activeDatabase().connectionName() != connectionName){continue;}

        ui.tabWidget->removeTab(i);

        //删除对对象,不可使用deleteLater()
        //Delete the object, do not use "deleteLater()"!
        delete de;
        de = 0;

    }

        //TODO:如果关闭UserManager正在使用的SQL Server数据库，则会Crash
        //QSqlDatabase::removeDatabase(connectionName);
        DatabaseUtility::closeDBConnection(connectionName);

        newlyCreatedconnections.removeAll(connectionName);

        //ui.databaseExplorer->slotRefresh();
        ui.databaseExplorer->setNewlyCreatedconnections(newlyCreatedconnections);

        if(!ui.tabWidget->count()){slotNewDataExplorerTab();}

        slotUpdateDatabaseComboBox(true);

}

void SqlExplorer::slotTabPageChanged(){

	updateWindowTitle();

	slotUpdateFrames();

}

DataExplorer *SqlExplorer::slotNewDataExplorerTab(const QString &tabText){

	DataExplorer *de = new DataExplorer(this);
	//如果DataExplorer执行了"alter","drop","create"操作,则刷新DatabaseExplorer
	//If DataExplorer alter,drop or create table(s), refresh DatabaseExplorer
	//TODO:刷新其它相关的DataExplorer
	//TODO:refresh related DataExplorer
	connect(de, SIGNAL(signalUpdateTablesRequested()), ui.databaseExplorer, SLOT(slotRefresh()));

	ui.tabWidget->addTab(de, tabText);
	ui.tabWidget->cornerWidget(Qt::TopRightCorner)->setEnabled(ui.tabWidget->count() > 1);

	ui.tabWidget->setCurrentWidget(de);

	return de;

}

void SqlExplorer::slotcloseTab(){
	//如果只有一页，则返回
  // if(ui.tabWidget->count()==1){
   //     return;
	  // }

    QWidget *page = qobject_cast<QWidget *>(ui.tabWidget->currentWidget());

    ui.tabWidget->removeTab(ui.tabWidget->currentIndex());
    delete page;

    if(ui.tabWidget->count() < 1){
    	slotNewDataExplorerTab();
    }

	//如果只有一页，则关闭按钮不可用
    //ui.tabWidget->cornerWidget(Qt::TopRightCorner)->setEnabled(ui.tabWidget->count() > 1);



}

void SqlExplorer::updateWindowTitle(){
	DataExplorer *de = qobject_cast<DataExplorer *>(ui.tabWidget->currentWidget());
	if(de){
		QString tableName = de->activeTable();
		QString databaseName = de->activeDatabaseName();
		setWindowTitle(tr("Database:%1").arg(databaseName)+"|"+tr("Table:%2").arg(tableName));
	}else{
		setWindowTitle(ui.tabWidget->tabText(ui.tabWidget->currentIndex()));
	}


}




void SqlExplorer::slotUpdateDatabaseComboBox(bool resetComboBox){
	DataExplorer *de = qobject_cast<DataExplorer *>(ui.tabWidget->currentWidget());
	if(!de){return;}

	if(resetComboBox){
		ui.databaseComboBox->clear();
		ui.databaseComboBox->addItems(QSqlDatabase::connectionNames());
	}

	ui.databaseComboBox->setCurrentIndex(ui.databaseComboBox->findText(de->activeDatabase().connectionName(), Qt::MatchFixedString));

	QString text = ui.databaseComboBox->currentText();
	ui.databaseComboBox->setToolTip(text);
	ui.databaseComboBox->setStatusTip(text);

	slotUpdateTableComboBox(true);

}


void SqlExplorer::slotUpdateTableComboBox(bool resetComboBox){
	DataExplorer *de = qobject_cast<DataExplorer *>(ui.tabWidget->currentWidget());
	if(!de){return;}

	QStringList tables = de->activeDatabase().tables();
	QString activeTable = de->activeTable();

	if(resetComboBox){
		ui.tableComboBox->clear();
		ui.tableComboBox->addItems(tables);

	}else if(tables.size() && !activeTable.isEmpty() && !tables.contains(activeTable)){
		QMessageBox::critical(this, tr("Error"), tr("<font color=red>Table '%1' had been deleted or modified!</font>").arg(activeTable));
		slotcloseTab();

		return;
	}



	ui.tableComboBox->setCurrentIndex(ui.tableComboBox->findText(activeTable, Qt::MatchFixedString));

	QString text = ui.tableComboBox->currentText();
	ui.tableComboBox->setToolTip(text);
	ui.tableComboBox->setStatusTip(text);

        ui.totalRowsLabel->setText(QString::number(de->rowCount()));
//        ui.labelTotalPages->setText(QString::number(de->rowCount()));

	ui.metadataToolButton->setEnabled(!text.isEmpty());

}

void SqlExplorer::slotUpdateFrames(){
	slotUpdateDatabaseComboBox(false);
	slotUpdateTableComboBox(false);

}


void SqlExplorer::on_databaseComboBox_activated ( const QString & text){
	DataExplorer *de = qobject_cast<DataExplorer *>(ui.tabWidget->currentWidget());
	if(!de){return;}

	de->setActiveDatabase(QSqlDatabase::database(text));
	de->resetTableViewModel();

	slotUpdateTableComboBox(true);

	ui.databaseComboBox->setToolTip(text);
	ui.databaseComboBox->setStatusTip(text);

	ui.tabWidget->setTabText(ui.tabWidget->indexOf(de), QString(tr("Unknown")));

	updateWindowTitle();

}


void SqlExplorer::on_tableComboBox_activated ( const QString & text){
	DataExplorer *de = qobject_cast<DataExplorer *>(ui.tabWidget->currentWidget());
	if(!de){return;}

	openTable(de, QSqlDatabase::database(ui.databaseComboBox->currentText()), text);

	ui.tableComboBox->setToolTip(text);
	ui.tableComboBox->setStatusTip(text);
}

void SqlExplorer::on_pagingCheckBox_stateChanged ( int state ){
	ui.pagingFrame->setEnabled(state);

}

void SqlExplorer::on_metadataToolButton_clicked (){
	QString tableName = ui.tableComboBox->currentText();
	if(!tableName.isEmpty()){
		slotShowMetaData(tableName);
	}

}






















} //namespace HEHUI


