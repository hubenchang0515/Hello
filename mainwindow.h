#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDirModel>
#include <QScrollBar>
#include <QProcess>
#include <csyntaxhighlighter.h>
#include <newprojectdialog.h>
#include <buildthread.h>
#include "helpdialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:

public slots:
    void clearLog(void);
    void appendLog(const QString& text,const QColor& color=Qt::black);
    void open(void);
    void openProject(const QString& path);
    void save(void);
    void saveFile(const QString& file);
    void quit(void);
    void build(void);
    void doubleClickFile(const QModelIndex& index);
    void openFile(const QString& filename);

private:
    Ui::MainWindow *ui;
    QDirModel *model;
    CSyntaxHighlighter *highlighter;
    NewProjectDialog* newDialog;
    HelpDialog* help;
    QThread workThread;
    BuildThread* buildThread;
    QString rootpath;
    QString currentFile;
    QString projectName;
    QProcessEnvironment env;
    bool isFileChanged = false;
    void envInit(void);
    void uiInit(void);
    void connections(void);
};

#endif // MAINWINDOW_H
