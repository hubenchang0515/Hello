#include "csyntaxhighlighter.h"
#include <QDebug>
CSyntaxHighlighter::CSyntaxHighlighter(QTextDocument* parent) :
    QSyntaxHighlighter(parent)
{
    HighlightRule rule;
    QTextCharFormat fomat;
    QString pattern;

    /*常数高亮*/
    fomat.setForeground(Qt::darkMagenta);
    pattern = "\\b[0-9]+"; //八进制(0开头)和十进制
    rule.pattern = QRegExp(pattern);
    rule.format = fomat;
    rules.append(rule);
    pattern = "0x[0-9a-fA-f]+"; //十六进制
    rule.pattern = QRegExp(pattern);
    rule.format = fomat;
    rules.append(rule);

    /*关键字高亮*/
    fomat.setForeground(QColor(0xff00ff));
    QStringList KeywordPatterns;
    KeywordPatterns << "auto"      << "break"
                    << "case"      << "char"
                    << "const"     << "continue"
                    << "default"   << "do"
                    << "double"    << "else"
                    << "enum"      << "extern"
                    << "float"     << "for"
                    << "goto"      << "\\if"
                    << "int"       << "long"
                    << "register"  << "return"
                    << "short"     << "signed"
                    << "sizeof"    << "static"
                    << "struct"    << "switch"
                    << "typedef"   << "union"
                    << "unsigned"  << "void"
                    << "volatile"  << "while";

    foreach (const QString& pattern, KeywordPatterns)
    {
        rule.pattern = QRegExp("\\b" + pattern + "\\b");
        rule.format = fomat;
        rules.append(rule);
    }

    /* 函数高亮 */
    fomat.setForeground(QColor(0x8470FF));
    pattern = "\\b[A-Za-z_][A-Za-z0-9_]*(?=\\()";
    rule.pattern = QRegExp(pattern);
    rule.format = fomat;
    rules.append(rule);

    /* 预处理高亮 */
    fomat.setForeground(Qt::darkGreen);
    pattern = "^ *#.*";
    rule.pattern = QRegExp(pattern);
    rule.format = fomat;
    rules.append(rule);

    /* 单行注释高亮 */
    fomat.setForeground(Qt::gray);
    pattern = "//.*";
    rule.pattern = QRegExp(pattern);
    rule.format = fomat;
    rules.append(rule);

    /* 字符高亮 */
    fomat.setForeground(Qt::darkMagenta);
    pattern = "\'.*\'";
    rule.pattern = QRegExp(pattern);
    rule.format = fomat;
    rules.append(rule);

    /* 字符串高亮 */
    fomat.setForeground(Qt::darkMagenta);
    pattern = "\".*\"";
    rule.pattern = QRegExp(pattern);
    rule.format = fomat;
    rules.append(rule);

    /* 多行注释 */
    multiLineCommentFormat.setForeground(Qt::gray);
    commentStartExpression = QRegExp("/\\*");
    commentEndExpression = QRegExp("\\*/");

}

void CSyntaxHighlighter::highlightBlock(const QString& text)
{
    foreach (const HighlightRule &rule, rules)
    {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0)
        {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }
    /* 多行注释 */
    setCurrentBlockState(0);
    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = commentStartExpression.indexIn(text);

    while(startIndex >= 0)
    {
        int endIndex = commentEndExpression.indexIn(text, startIndex);
        int commentLength;

        if (endIndex == -1)
        {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        }
        else
        {
            commentLength = endIndex - startIndex
                                + commentEndExpression.matchedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
    }

}
