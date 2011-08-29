#ifndef SQLCONSOLE_H
#define SQLCONSOLE_H

#ifndef MAX_COM_NUM
#define MAX_COM_NUM 5
#endif



#include <QWidget>
#include <QAbstractItemModel>
#include <QCompleter>
#include <QString>
#include <QAction>

#include "textedit.h"
//#include "../ui/ui_sqlconsole.h"
#include "ui_sqlconsole.h"

#include "highlighter.h"


class SqlConsole : public QWidget {
Q_OBJECT

public:
	SqlConsole(QWidget *parent= 0/*, MainWindow *h*/);
	~SqlConsole();



protected:
	void languageChange();

signals:
	void signalQueryRequested(const QString &queryString);

public slots:

        void on_toolButtonOpenSQLScriptFile_clicked();

        void on_submitButton_clicked();

	void on_clearButton_clicked() {
		ui.textEdit->clear();
		ui.textEdit->setFocus();
	}

private slots:

	void on_useSQLConsoleGB_clicked() {
		if (ui.useSQLConsoleGB->isChecked()) {
			ui.textEdit->clear();
			ui.textEdit->setFocus();
		}
		//	ui.clearButton->setEnabled(ui.useSQLConsoleGB->isChecked());	
	}

	void saveCommand(const QString &command);
	void setCommand(int index);



protected:
	bool eventFilter(QObject *obj, QEvent *event);

private:
	Ui::SqlConsoleUI ui;

	//MainWindow *mw;
	QAbstractItemModel *modelFromFile(const QString& fileName);
	QCompleter *completer;

	QStringList commandList;
	int cmdIndex;

        Highlighter *highlighter;


};

#endif // SQLCONSOLE_H
