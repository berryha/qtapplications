#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QSyntaxHighlighter>

class Highlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    explicit Highlighter(QTextEdit *parent = 0);


    QStringList loadKeywordFromFile(const QString &fileName);
    void addKeywords(const QStringList &keywordList);

protected:
    void highlightBlock(const QString &text);

private:
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QTextCharFormat dangerousKeywordFormat;
    QTextCharFormat keywordFormat;
    QTextCharFormat tableFormat;


signals:

public slots:




};

#endif // HIGHLIGHTER_H
