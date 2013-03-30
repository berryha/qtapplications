
#include <QtWidgets>
#include <QObject>


#include "sqlconsole.h"

//#include "mainwindow.h"
//#include "../../shared/core/settingsbase.h"


SqlConsole::SqlConsole(QWidget *parent/*, MainWindow *h*/)
    : QWidget(parent)/*, completer(0), mw(h)*/
{
	ui.setupUi(this);
	
    completer = new QCompleter(this);
    completer->setModel(modelFromFile(":/text/resources/text/sqlkeyword"));
    completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    ui.textEdit->setCompleter(completer);

    ui.textEdit->installEventFilter(this);

    cmdIndex = -1;

    highlighter = new Highlighter(ui.textEdit);

}


SqlConsole::~SqlConsole(){
	commandList.clear();

}

void SqlConsole::languageChange()
{

	ui.retranslateUi(this);

}


bool SqlConsole::eventFilter(QObject *obj, QEvent *event) {
	if (obj == ui.textEdit && event->type() == QEvent::KeyRelease) {
		QKeyEvent *keyEvent = static_cast<QKeyEvent *> (event);
		if(keyEvent->key() == Qt::Key_Up ){
			setCommand(++cmdIndex);
		}else if(keyEvent->key() == Qt::Key_Down){
			setCommand(--cmdIndex);
		}

		if(keyEvent->key() == Qt::Key_Escape){
			ui.textEdit->clear();
		}


		return true;
	}else{
		// standard event processing
		return QObject::eventFilter(obj, event);
	}

}

void SqlConsole::on_toolButtonOpenSQLScriptFile_clicked(){
    QString fileName = QFileDialog::getOpenFileName(this, tr("SQL Script File"), QDir::currentPath(), tr("SQL Script (*.sql);;All Files (*.*)"));
    if(fileName.trimmed().isEmpty()){
        return;
    }

    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QString msg = file.errorString();
        QMessageBox::critical(this, tr("Error"), msg);
        qCritical()<< msg;
        return;
    }


    QStringList codecs;
    QList<QByteArray> availableCodecs = QTextCodec::availableCodecs();
    QByteArray locale = QTextCodec::codecForLocale()->name();
    availableCodecs.removeAll(locale);
    availableCodecs.prepend(locale);
    foreach (QByteArray ba, availableCodecs) {
        codecs << ba;
    }

    QString selectedCodec = QInputDialog::getItem(this, tr("Select Codec"), tr("Select the codec of the file:"), codecs, 0, false);
    if(selectedCodec.isEmpty()){
        selectedCodec = locale;
    }

    QTextStream stream(&file);
    stream.setCodec(selectedCodec.toLocal8Bit());
    stream.setAutoDetectUnicode(true);

    ui.textEdit->setText(stream.readAll());


}

void SqlConsole::on_submitButton_clicked() {
	QString queryString = ui.textEdit->toPlainText();

	if(queryString.startsWith("delete", Qt::CaseInsensitive) || queryString.startsWith("drop", Qt::CaseInsensitive)){
		QMessageBox::StandardButton reply;
		reply = QMessageBox::question(this, tr("Dangerous Operation"), tr("<font color=red>Really delete data?</font>"),
					QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
		if (reply == QMessageBox::No) {
			return;
		}
	}


	emit signalQueryRequested(queryString);

	saveCommand(queryString);
	ui.textEdit->clear();
	ui.textEdit->setFocus();
	cmdIndex = -1;

}


void SqlConsole::saveCommand(const QString &command)
{
	//TODO:
	//QStringList commandList = Settings::instance()->getCommandList();
//	if(commandList.count()>=MAX_COM_NUM){
//		commandList.pop_back();
//		commandList.prepend(command);
//	}
	//Settings::instance()->setCommandList(commandList);



	if(commandList.count()>=MAX_COM_NUM){
		commandList.pop_back();
	}
	commandList.prepend(command);


}


void SqlConsole::setCommand(int index){
	ui.textEdit->clear();

	int cmdCount = commandList.size();
	if(!cmdCount){
		cmdIndex = -1;
		return;
	}

	QString cmd;

	if(index >= cmdCount){
		index = cmdCount-1;
		//ui.textEdit->setText(commandList.at(index));
		cmd = commandList.at(index);
	}else if(index < 0){
		//ui.textEdit->setText("");
		cmd = "";
		index = -1;
	}else{
		//ui.textEdit->setText(commandList.at(index));
		cmd = commandList.at(index);
	}

	cmdIndex = index;

	ui.textEdit->setCommand(cmd);

//	QTextCursor tc = ui.textEdit->textCursor();
//	tc.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
//	//tc.insertText(completion);
//	qDebug()<<"~~~~~~~~~~~~~~~tc.movePosition";

}



QAbstractItemModel *SqlConsole::modelFromFile(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly))
        return new QStringListModel(completer);

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QStringList words;
    
    while (!file.atEnd()) {
        QByteArray line = file.readLine();
        if (!line.isEmpty())
            words << line.trimmed();
    }

    QApplication::restoreOverrideCursor();
    return new QStringListModel(words, completer);
}



