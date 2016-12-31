#ifndef NEWPROJECTDIALOG_H
#define NEWPROJECTDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QLayout>

class NewProjectDialog : public QDialog
{
    Q_OBJECT
public:
    explicit NewProjectDialog(const QString& rootpath,QWidget *parent = 0);

signals:
    void created(const QString& path);

public slots:
    void reset(void);
    bool newProject(const QString& uuid,const QString& name);

private:
    QString rootpath;
    QLabel* labelUuid;
    QLabel* labelName;
    QLineEdit* uuid;
    QLineEdit* name;
    QRadioButton* watchface;
    QRadioButton* app;
    QGridLayout* layout;
    QPushButton* ok;
    QPushButton* cancel;
};

#endif // NEWPROJECTDIALOG_H
