
#include <QFile>
#include <QCompleter>
#include <QKeyEvent>
#include <QtDebug>
#include <QApplication>
#include <QModelIndex>
#include <QAbstractItemModel>
#include <QAbstractItemView>
#include <QScrollBar>
#include <QTextCursor>

#include "textedit.h"
//#include "settings.h"

TextEdit::TextEdit(QWidget *parent)
    : QTextEdit(parent), c(0)
{
    //	QString toolTip = tr("Notice:<br>");
    //	toolTip += tr("1.SQLite can only execute one statement one time! <br>");
    //	toolTip += tr("2. 'CTRL' + '/' : Auto completions! <br>");
    //	toolTip += tr("3. 'CTRL' + 'Mouse Wheel' or '[' or ']' : Zoom in or out! <br>");
    //	setToolTip(toolTip);


    //   QString toolTip = QString(tr("<table border=0 cellspacing=0 cellpadding=3>"
    //                                "<tr><th>Tips</th></tr>"
    //                                "<tr><td>1.</td><td>1.SQLite can only execute one statement one time!</td></tr>"
    //                                "<tr><td>2.</td><td>'CTRL' + '/' : Auto completions!</td></tr>"
    //                                "<tr><td>3.</td><td>'CTRL' + 'Mouse Wheel' or '[' or ']' : Zoom in or out!</td></tr>"
    //                                "<tr><td></td><td></td></tr>"
    //                                "</table>"));

    //   setToolTip(toolTip);
    //   setWhatsThis(toolTip);



}

TextEdit::~TextEdit()
{


}

void TextEdit::setCommand(const QString &cmd){
    //setText(cmd);
    QTextCursor tc = textCursor();
    tc.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
    tc.insertText(cmd);

}


void TextEdit::setCompleter(QCompleter *completer)
{
    if (c)
        QObject::disconnect(c, 0, this, 0);

    c = completer;

    if (!c)
        return;

    c->setWidget(this);
    c->setCompletionMode(QCompleter::PopupCompletion);
    c->setCaseSensitivity(Qt::CaseInsensitive);
    QObject::connect(completer, SIGNAL(activated(const QString&)),
                     this, SLOT(insertCompletion(const QString&)));
}


QCompleter *TextEdit::completer() const
{
    return c;
}

void TextEdit::insertCompletion(const QString& completion)
{
    QTextCursor tc = textCursor();

    //在当前字符后插入
    //Insert after current Cursor
    //int extra = completion.length() - c->completionPrefix().length();
    //tc.insertText(completion.right(extra));

    //删除后插入
    //Insert after delete current word
    tc.movePosition(QTextCursor::StartOfWord, QTextCursor::KeepAnchor);
    tc.insertText(completion);

    tc.movePosition(QTextCursor::EndOfWord);
    setTextCursor(tc);
}

QString TextEdit::textUnderCursor() const
{
    QTextCursor tc = textCursor();
    tc.select(QTextCursor::WordUnderCursor);
    return tc.selectedText();
}




void TextEdit::keyPressEvent(QKeyEvent *e)
{
    bool isZoomIn = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_BracketLeft);
    bool isZoomOut = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_BracketRight);
    if(isZoomIn){
        zoomIn(1);
    }else if(isZoomOut){
        zoomOut(1);
    }

    if (c && c->popup()->isVisible()) {
        switch (e->key()) {
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Escape:
        case Qt::Key_Tab:
        case Qt::Key_Backtab:
            e->ignore();
            return; // let the completer do default behavior
        default:
            break;
        }

    }

    //bool isShortcut = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_E); // CTRL+E
    bool isShortcut = ((e->modifiers() & Qt::AltModifier) && e->key() == Qt::Key_Slash); // CTRL+/
    if (!c || !isShortcut) // dont process the shortcut when we have a completer
        QTextEdit::keyPressEvent(e);

    const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
    if (!c || (ctrlOrShift && e->text().isEmpty()))
        return;

    static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-="); // end of word
    bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
    QString completionPrefix = textUnderCursor();

    if (!isShortcut && (hasModifier || e->text().isEmpty()|| completionPrefix.length() < 3
                        || eow.contains(e->text().right(1)))) {
        c->popup()->hide();
        return;
    }

    if (completionPrefix != c->completionPrefix()) {
        c->setCompletionPrefix(completionPrefix);
        c->popup()->setCurrentIndex(c->completionModel()->index(0, 0));
    }
    QRect cr = cursorRect();
    cr.setWidth(c->popup()->sizeHintForColumn(0)
                + c->popup()->verticalScrollBar()->sizeHint().width());
    c->complete(cr); // popup it up!
}


void TextEdit::wheelEvent( QWheelEvent * e){
    if(QApplication::keyboardModifiers() == Qt::ControlModifier){

        int numDegrees = e->delta() / 8;
        int numSteps = numDegrees / 15;

        int newFontWeight = fontWeight()+numSteps;
        if(newFontWeight>=0 && newFontWeight<=99){
            zoomIn(numSteps);
            //update();
        }

        e->accept();
    }else{

        QTextEdit::wheelEvent(e);
    }



}



