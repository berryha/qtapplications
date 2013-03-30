

#include <QTextEdit>
#include <QFile>
#include <QApplication>
#include "highlighter.h"

Highlighter::Highlighter(QTextEdit *parent) :
    QSyntaxHighlighter(parent)
{


    addKeywords(loadKeywordFromFile(":/text/resources/text/sqlkeyword"));


}



QStringList Highlighter::loadKeywordFromFile(const QString &fileName){

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly))
        return QStringList();

    QStringList words;
    while (!file.atEnd()) {
        QByteArray line = file.readLine();
        if (!line.isEmpty())
            words << line.trimmed();
    }

    return words;
}

void Highlighter::addKeywords(const QStringList &keywordList){

    HighlightingRule rule;

    keywordFormat.setForeground(Qt::darkMagenta);
    keywordFormat.setFontWeight(QFont::Bold);
    foreach (const QString &keyword, keywordList) {
        rule.pattern = QRegExp("\\b" + keyword + "\\b", Qt::CaseInsensitive);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

}


void Highlighter::highlightBlock(const QString &text){

    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }



}

















