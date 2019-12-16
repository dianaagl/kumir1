#ifndef LATINHIGHLIGHTER_H
#define LATINHIGHLIGHTER_H

#include <QtCore>
#include <QtGui>

class LatinHighlighter
    : public QSyntaxHighlighter
{
public:
    explicit LatinHighlighter(QTextEdit * editor);
protected:
    void highlightBlock(const QString &text);

    QRegExp rxLatinSymbols;
};

#endif // LATINHIGHLIGHTER_H
