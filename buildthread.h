#ifndef BUILDTHREAD_H
#define BUILDTHREAD_H

#include <QObject>
#include <QThread>
#include <QProcess>
#include <QColor>

class BuildThread : public QThread
{
    Q_OBJECT

public:
    BuildThread(QObject* parent=0);
    QProcess* cmd;
    enum Flag{PACK1,COMPILE,PACK2};

signals:
    void putLog(const QString& text,const QColor& color);

public:
    void cmdfinished(int exitCode, QProcess::ExitStatus status);
    void build(const QString& project);

protected:
    void run(void);

private:
    QString projectName;
    Flag flag;
};

#endif // BUILDTHREAD_H
