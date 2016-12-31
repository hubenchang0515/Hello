#include "buildthread.h"
#include <QFile>
#include <QDebug>

BuildThread::BuildThread(QObject* parent) :
    QThread(parent)
{
    cmd = new QProcess();
    cmd->moveToThread(this);
    connect(cmd,(void(QProcess::*)(int,QProcess::ExitStatus))&QProcess::finished,this,&BuildThread::cmdfinished,Qt::DirectConnection);
}

void BuildThread::build(const QString& project)
{
    projectName = project;
    start();
}

/* 重载虚函数run，线程执行的内容 */
void BuildThread::run(void)
{
    cmd->start("pack");
    flag = PACK1;
    exec();
}

/* QProcess cmd执行完，打印日志 */
void BuildThread::cmdfinished(int exitCode, QProcess::ExitStatus status)
{
    /*消除警告*/
    (void)exitCode;
    (void)status;

    QString standardOutput = cmd->readAllStandardOutput();
    QString standardError = cmd->readAllStandardError();

    emit putLog(standardOutput,Qt::black);
    emit putLog(standardError,Qt::red);
    if(flag == PACK1)
    {
        cmd->start("arm-none-eabi-gcc -c -W -Wall -Ibuild -mthumb -mcpu=cortex-m0 " + projectName + ".c");
        flag = COMPILE;
    }
    else if(flag == COMPILE)
    {
        QFile::remove("build/maibu_code");
        QFile::copy(projectName + ".o","build/maibu_code");
        cmd->start("pack");
        flag = PACK2;
    }
    else
    {
        quit();
    }
}

