#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QProcess>
#include <QPair>

#include "atmelprogrammer.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

using prgrmrPair = QPair<QString, QString>;
using prgrmrPairList = QList<prgrmrPair>;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_browseButton_clicked();
    void on_programButton_clicked();
    void on_verifyButton_clicked();
    void on_eraseButton_clicked();
    void on_actionClear_Console_triggered();
//    void on_deviceId_currentTextChanged(const QString &arg1);
    void on_actionSave_Console_to_file_triggered();
    void on_infoButton_clicked();
    void on_actionAbout_triggered();

    void parmsChanged(int index);

protected:

//    bool            executeCommand(const QString & arguments, QStringList * extraArgs = nullptr);
    void            enableControls(bool en);

//    QString         exitCodeToString(int code);

    bool            getProgrammerList();

private:
    Ui::MainWindow *ui;

    AtmelProgrammer     Prgrmr0;
    AtmelProgrammer     Prgrmr1;
    AtmelProgrammer     Prgrmr2;

    void                UpdateUI();
    prgrmrPairList      programmerList;

};
#endif // MAINWINDOW_H
