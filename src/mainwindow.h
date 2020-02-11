#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QProcess>
#include <QPair>
#include <QVector>

#include "atmelprogrammer.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionClear_Console_triggered();
    void on_actionSave_Console_to_file_triggered();

    void on_actionAbout_triggered();
    void on_programButton_clicked();

    void on_verifyButton_clicked();

protected:

    void                enableControls(bool en);
    bool                getProgrammerList();
    void                allocateProgrammers();
    void                enableProgrammer(int index, bool en);

private:
    Ui::MainWindow *    ui = nullptr;

    prgrmrVec           programmers;
    prgrmrPairList      programmerList;
    int                 busyPrgmCnt = 0;
};
#endif // MAINWINDOW_H
