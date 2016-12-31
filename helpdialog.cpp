#include "helpdialog.h"

HelpDialog::HelpDialog(QWidget* parent) :
    QDialog(parent)
{
    layout = new QGridLayout(this);
    tb = new QTextBrowser(this);
    layout->addWidget(tb);
    tb->setSource(QUrl("qrc:/res/API Manual.html"));
    resize(640,400);
    setWindowTitle(tr("帮助"));
    setWindowFlags(Qt::Window);
}

