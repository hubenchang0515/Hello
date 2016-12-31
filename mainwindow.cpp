#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QProcess>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    rootpath = QDir::currentPath();
    model = new QDirModel(ui->files);
    highlighter = new CSyntaxHighlighter(ui->editor->document());
    newDialog = new NewProjectDialog(rootpath,this);
    help = new HelpDialog(this);
    buildThread = new BuildThread();
    buildThread->moveToThread(&workThread);
    envInit();
    uiInit();
    connections();
}

MainWindow::~MainWindow()
{
    delete ui;
}

/* 初始化运行环境 */
void MainWindow::envInit(void)
{  
    env = QProcessEnvironment::systemEnvironment();
    env.insert("PATH",rootpath + "/mingw4.8.1/bin");
    env.insert("PATH",env.value("PATH")+";" + rootpath + + "/gcc-arm-none-eabi-5_3-2016q1/bin");
    env.insert("PATH",env.value("PATH")+";" + rootpath + + "/maibu/bin");
    qputenv("PATH",env.value("PATH").toStdString().c_str());
    QDir::setCurrent("./Project");
}

/* 初始化UI界面 */
void MainWindow::uiInit(void)
{
    /* 设置标题 */
    setWindowTitle("Hello");
    /* 设置字体 */
    int fontId = QFontDatabase::addApplicationFont(":/res/UbuntuMono-B.ttf");
    QString font = QFontDatabase::applicationFontFamilies(fontId).at(0);
    ui->editor->setFont(QFont(font,11));
    /* 设置窗口部件的比例 */
    ui->hSplitter->setStretchFactor(0,2);
    ui->hSplitter->setStretchFactor(1,8);
    ui->vSplitter->setStretchFactor(0,7);
    ui->vSplitter->setStretchFactor(1,3);
    /* 设置tab键的长度为4个空格 */
    ui->editor->setTabStopWidth(4*ui->editor->fontMetrics().width(' '));
    ui->log->setTabStopWidth(4*ui->log->fontMetrics().width(' '));
    /* 设置QTextEdit log只读 */
    ui->log->setReadOnly(true);
    /* 设置QTreeView files */
    model->setSorting(QDir::DirsFirst|QDir::Type);
    ui->files->setModel(model);
    for(int i=1;i<model->columnCount();i++)
    {
        ui->files->hideColumn(i);
    }
    ui->files->setRootIndex(model->index("."));

    //model->setHeaderData(0,Qt::Vertical,"Project");
}

/* 信号、槽绑定 */
void MainWindow::connections(void)
{
    connect(ui->action_new,&QAction::triggered,[&]{newDialog->reset();newDialog->exec();});
    connect(ui->action_open,&QAction::triggered,this,&MainWindow::open);
    connect(ui->action_save,&QAction::triggered,this,&MainWindow::save);
    connect(ui->action_quit,&QAction::triggered,this,&MainWindow::quit);
    connect(ui->action_cut,&QAction::triggered,ui->editor,&QPlainTextEdit::cut);
    connect(ui->action_copy,&QAction::triggered,ui->editor,&QPlainTextEdit::copy);
    connect(ui->action_paste,&QAction::triggered,ui->editor,&QPlainTextEdit::paste);
    connect(ui->action_build,&QAction::triggered,this,&MainWindow::build);
    connect(ui->action_undo,&QAction::triggered,ui->editor,&QPlainTextEdit::undo);
    connect(ui->action_redo,&QAction::triggered,ui->editor,&QPlainTextEdit::redo);
    connect(ui->action_help,&QAction::triggered,help,&HelpDialog::show);
    connect(ui->actionQt,&QAction::triggered,[&]{QMessageBox::aboutQt(this);});
    connect(newDialog,&NewProjectDialog::created,this,&MainWindow::openProject);
    connect(buildThread,&BuildThread::putLog,this,&MainWindow::appendLog);
    connect(ui->files,&QTreeView::doubleClicked,this,&MainWindow::doubleClickFile);
    connect(ui->editor,&QPlainTextEdit::textChanged,[&]{isFileChanged = true;});

}

/* 清除QTextEdit log的内容 */
void MainWindow::clearLog(void)
{
    ui->log->clear();
}

/* 在QTextEdit log中添加日志内容 */
void MainWindow::appendLog(const QString &text, const QColor &color)
{
    QTextCharFormat fmt;
    fmt.setForeground(color);
    ui->log->setCurrentCharFormat(fmt);
    ui->log->insertPlainText(text);
    QScrollBar* scroll = ui->log->verticalScrollBar();
    scroll->setSliderPosition(scroll->maximum());
}


/* 点击打开，打开一个项目 */
void MainWindow::open(void)
{
    QString file = QFileDialog::getOpenFileName(this,tr("打开项目"),
                                                  QDir::currentPath(),
                                                  tr("麦步Pro (*.mbpro)"));
    /* mbpro文件标识该目录为麦步手表的项目,打开该目录下任何文件均可 */
    openProject(file);
}

/* 打开一个项目目录,参数是项目目录下的任意一个文件 */
void MainWindow::openProject(const QString &path)
{
    QFileInfo fileinfo(path);
    QDir::setCurrent(fileinfo.path());
    ui->files->setRootIndex(model->index(fileinfo.path()));
    projectName = fileinfo.baseName();
    model->refresh();
}

/* 点击保存，保存文件 */
void MainWindow::save(void)
{
    QString file;
    if(currentFile.isEmpty())
    {
        QString file = QFileDialog::getSaveFileName(this,tr("保存文件"),QDir::currentPath());
    }
    else
    {
        file = currentFile;
    }

    saveFile(file);
}

/* 保存文件 */
void MainWindow::saveFile(const QString &filename)
{
    QFile file(filename);
    file.open(QFile::WriteOnly);
    file.write(ui->editor->toPlainText().toStdString().c_str());
    file.close();
    isFileChanged = false;
}

/* 退出 */
void MainWindow::quit(void)
{
    if(isFileChanged)//文件被修改，询问是否保存
    {
        QMessageBox::StandardButton button = QMessageBox::question(this,tr("退出"),tr("是否保存?"),
                                                                   QMessageBox::Yes|
                                                                   QMessageBox::No|
                                                                   QMessageBox::Cancel);
        if(button == QMessageBox::Yes) //点击yes，保存并关闭
        {
            save();
            close();
        }
        else if(button == QMessageBox::No) //点击no，不保存直接关闭
        {
            close();
        }
        //点击cancel，函数直接返回，取消退出。
    }
    else  //文件没有被修改，直接退出
    {
        close();
    }
}

/* 构建项目 */
void MainWindow::build(void)
{
    if(projectName.isEmpty())
    {
        //没有打开项目，不能构建
        QMessageBox::critical(this,tr("构建"),tr("请先打开项目。"));
        return;
    }
    if(!currentFile.isEmpty())
    {
        save();
    }
    clearLog();
    buildThread->build(projectName);

}

/* 双击文件打开 */
void MainWindow::doubleClickFile(const QModelIndex &index)
{
    QFileInfo file = model->fileInfo(index);
    if(file.isFile() && file.fileName() != currentFile)  //点击的是文件
    {
        if(isFileChanged  && !currentFile.isEmpty()) //当前文件没修改过，询问是否保存
        {
            QMessageBox::StandardButton button = QMessageBox::question(this,tr("退出"),tr("是否保存?"),
                                                                       QMessageBox::Yes|
                                                                       QMessageBox::No|
                                                                       QMessageBox::Cancel);
            if(button == QMessageBox::Yes)
            {
                save();
                openFile(file.absoluteFilePath());
            }
            else if(button == QMessageBox::No)
            {
                openFile(file.absoluteFilePath());
            }
            //点击取消，直接返回
        }
        else
        {
            openFile(file.absoluteFilePath());
        }
    }
}

/* 打开文件 */
void MainWindow::openFile(const QString &filename)
{
    currentFile = filename;
    QFile file(filename);
    file.open(QFile::ReadOnly);
    ui->editor->setPlainText(file.readAll());
    isFileChanged = false;
    model->refresh();
}
