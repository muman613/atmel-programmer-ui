#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QProcess>

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
    void on_browseButton_clicked();

    void on_programButton_clicked();

    void on_verifyButton_clicked();

    void on_eraseButton_clicked();


protected:

    bool            executeCommand(const QString & arguments);

private:
    Ui::MainWindow *ui;

    bool            cmdInProgress = false;

    QString         progTool;
    QString         deviceId;
    QString         progIF;

    QString         atProgramPath;
    QString         fwPath;

    QProcess *      programmerProc = nullptr;
};
#endif // MAINWINDOW_H
