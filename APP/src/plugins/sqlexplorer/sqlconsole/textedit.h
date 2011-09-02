
#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include <QTextEdit>

class QCompleter;
class QAbstractItemModel;

class TextEdit : public QTextEdit
{
    Q_OBJECT

public:
    TextEdit(QWidget *parent = 0);
    ~TextEdit();

    void setCompleter(QCompleter *c);

    QCompleter *completer() const;
    


protected:
    void keyPressEvent(QKeyEvent *e);
    void wheelEvent( QWheelEvent * e);

public slots:
	void setCommand(const QString &cmd);

private slots:
    void insertCompletion(const QString &completion);

private:
    
    QString textUnderCursor() const;

private:
    QCompleter *c;
};

#endif // TEXTEDIT_H

