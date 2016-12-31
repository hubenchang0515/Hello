#ifndef HELPDIALOG_H
#define HELPDIALOG_H

#include <QDialog>
#include <QTextBrowser>
#include <QLayout>

class HelpDialog : public QDialog
{
public:
    HelpDialog(QWidget* parent=0);

private:
    QTextBrowser* tb;
    QGridLayout* layout;
};

#endif // HELPDIALOG_H
