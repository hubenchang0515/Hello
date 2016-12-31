#ifndef CSYNTAXHIGHLIGHTER_H
#define CSYNTAXHIGHLIGHTER_H

#include <QSyntaxHighlighter>



class CSyntaxHighlighter : public QSyntaxHighlighter
{
public:
    CSyntaxHighlighter(QTextDocument* parent=0);
    ~CSyntaxHighlighter() = default;

protected:
    struct HighlightRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightRule> rules;
    void highlightBlock(const QString& text);
    /* 多行注释匹配 */
    QTextCharFormat multiLineCommentFormat;
    QRegExp commentStartExpression;
    QRegExp commentEndExpression;

};

#endif // CSYNTAXHIGHLIGHTER_H
