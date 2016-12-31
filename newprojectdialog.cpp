#include "newprojectdialog.h"
#include <QDir>
#include <QValidator>
#include <QMessageBox>
#include <QDebug>

NewProjectDialog::NewProjectDialog(const QString& rootpath,QWidget *parent) : QDialog(parent)
{
    this->rootpath = rootpath;
    setWindowTitle(tr("新建项目"));
    layout = new QGridLayout(this);
    labelUuid = new QLabel("UUID:",this);
    labelUuid->setAlignment(Qt::AlignRight);
    labelName = new QLabel(tr("项目名称:"),this);
    labelName->setAlignment(Qt::AlignRight);
    uuid = new QLineEdit(this);
    name = new QLineEdit(this);
    watchface = new QRadioButton(tr("表盘"),this);
    app = new QRadioButton(tr("应用"),this);
    ok = new QPushButton(tr("确定"),this);
    cancel = new QPushButton(tr("取消"),this);
    layout->addWidget(labelUuid,0,0,1,1);
    layout->addWidget(uuid,0,1,1,2);
    layout->addWidget(labelName,1,0,1,1);
    layout->addWidget(name,1,1,1,2);
    layout->addWidget(watchface,2,1);
    layout->addWidget(app,2,2);
    layout->addWidget(ok,3,0);
    layout->addWidget(cancel,3,2);
    QValidator *v = new QRegExpValidator(QRegExp("[0-9A-Za-z]*"),this);
    uuid->setValidator(v);
    name->setValidator(v);
    watchface->setChecked(true);
    app->setChecked(false);
    connect(ok,&QPushButton::clicked,[&]{newProject(uuid->text(),name->text());});
    connect(cancel,&QPushButton::clicked,[&]{this->close();});
    connect(watchface,&QRadioButton::clicked,[&]{app->setChecked(false);});
    connect(app,&QRadioButton::clicked,[&]{watchface->setChecked(false);});
}

void NewProjectDialog::reset(void)
{
    uuid->clear();
    name->clear();
    watchface->setChecked(true);
    app->setChecked(false);
}

bool NewProjectDialog::newProject(const QString& uuid,const QString& name)
{
    /* 检查输入是否合法 */
    if(uuid=="")
    {
        QMessageBox::critical(this,tr("错误"),tr("UUID不能为空。"));
        return false;
    }
    if(name=="")
    {
        QMessageBox::critical(this,tr("错误"),tr("项目名称不能为空。"));
        return false;
    }
    if(QFile::exists(rootpath + "/Project/" + name))
    {
        QMessageBox::critical(this,tr("错误"),name + tr(" 已存在。"));
        return false;
    }

    /* 回到Project目录 */
    QDir::setCurrent(rootpath+"/Project");
    QDir dir;
    /* 创建项目目录 */
    dir.mkdir(name);
    /* 进入项目目录 */
    QDir::setCurrent(name);
    /* 创建目录 */
    dir.mkdir("resources");
    dir.mkdir("resources/files");
    dir.mkdir("resources/fonts");
    dir.mkdir("resources/images");
    dir.mkdir("build");

    /* 生成appinfo.json的内容 */
    QString json = "{\r\n";
    json += "\t\"uuid\" : \"" + uuid + "\",\r\n";
    json += "\t\"name\" : \"" + name + "\",\r\n";
    json += "\t\"icon\" : \"\",\r\n";
    json += "\t\"version\" : \"0.0.1\",\r\n";
    json += "\t\"watchface\" : \"";
    if(watchface->isChecked())
    {
        json += "true\",\r\n";
    }
    else
    {
        json += "false\",\r\n";
    }
    json += "\t\"resources\" : []\r\n}\r\n";
    /* 创建appinfo.json并写入内容 */
    QFile file("appinfo.json");
    file.open(QFile::WriteOnly);
    file.write(json.toStdString().c_str());
    file.close();

    /* 创建main.c */
    file.setFileName(name + ".c");
    file.open(QFile::WriteOnly);
    file.close();

    /* 创建.mbpro文件 */
    file.setFileName(name + ".mbpro");
    file.open(QFile::WriteOnly);
    file.write("这个文件实际上并没有什么卵用，只是为了方便程序打开项目而已。\r\n");
    file.write("Infact this file is useless,just let program can open project easy.\r\n");
    file.close();

    /* 发送创建完成的信号 */
    emit created(QDir::currentPath() + "/" + name + ".mbpro");

    close();
    return true;
}


