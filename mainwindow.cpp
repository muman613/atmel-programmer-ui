#include <QFileDialog>
#include <QDebug>
#include <QSettings>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    QSettings iniSettings;

    ui->setupUi(this);

    fwPath          = iniSettings.value("fwPath").toString();
    atProgramPath   = iniSettings.value("atprogram", "D:/Program Files (x86)/Atmel/Studio/7.0/atbackend/atprogram.exe").toString();
    progTool        = iniSettings.value("progTool", "atmelice").toString();
    deviceId        = iniSettings.value("deviceId", "atmega328p").toString();
    progIF          = iniSettings.value("progIF", "isp").toString();

    ui->firmwareName->setText(fwPath);

    if (QFile::exists(atProgramPath)) {
        ui->consoleText->append("Programmer executable exists!");
    } else {
        ui->consoleText->append("Programmer executable not found!");
    }
}

MainWindow::~MainWindow()
{
    QSettings iniSettings;

    iniSettings.setValue("fwPath",      QVariant(fwPath));
    iniSettings.setValue("atprogram",   QVariant(atProgramPath));
    iniSettings.setValue("progTool",    QVariant(progTool));
    iniSettings.setValue("deviceId",    QVariant(deviceId));
    iniSettings.setValue("progIF",      QVariant(progIF));

    delete ui;
}


void MainWindow::on_browseButton_clicked()
{
    QFileDialog     chooseFile(this, "Select Firmware File");

    qDebug() << Q_FUNC_INFO;

    chooseFile.setNameFilter(tr("Hex (*.hex)"));
    chooseFile.setViewMode(QFileDialog::Detail);
    chooseFile.setFileMode(QFileDialog::ExistingFile);

    if (chooseFile.exec()) {
        QStringList FileNames = chooseFile.selectedFiles();
        fwPath = FileNames[0];
        qDebug() << "Firmware path =" << fwPath;
        ui->firmwareName->setText(fwPath);
    }
}

void MainWindow::on_programButton_clicked()
{
    qDebug() << Q_FUNC_INFO;

#if 1
    if (executeCommand("list")) {
        qDebug() << "OK";
    }
#else
    programmerProc = new QProcess();

    connect(programmerProc, &QProcess::started, [=]() {
        qDebug() << "Process started";
    } );

    connect(programmerProc,
            QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [=](int exitCode, QProcess::ExitStatus exitStatus) {
                qDebug() << "Process finished" << exitCode << exitStatus;
            }
    );

    connect(programmerProc, &QProcess::readyReadStandardOutput, [=]() {
        QByteArray stdOut = programmerProc->readAllStandardOutput();
        ui->consoleText->append(stdOut);

    });

    programmerProc->setProgram(atProgramPath);
    programmerProc->setArguments(QStringList("help"));
    programmerProc->start();

    programmerProc->waitForStarted();

    qDebug() << programmerProc->readAllStandardOutput();
    qDebug() << "Done";
#endif
}

void MainWindow::on_verifyButton_clicked()
{

}

void MainWindow::on_eraseButton_clicked()
{

}

bool MainWindow::executeCommand(const QString &arguments)
{
    if (cmdInProgress || (programmerProc != nullptr)) {
        qDebug() << "Attempted to start command while previous command in progress...";
        return false;
    }

    programmerProc = new QProcess();

    connect(programmerProc, &QProcess::started, [=]() {
        cmdInProgress = true;
        qDebug() << "Process started";
    } );

    connect(programmerProc,
            QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [=](int exitCode, QProcess::ExitStatus exitStatus) {
                qDebug() << "Process finished" << exitCode << exitStatus;
                delete programmerProc;
                programmerProc = nullptr;
                cmdInProgress = false;
            }
    );

    connect(programmerProc, &QProcess::readyReadStandardOutput, [=]() {
        QByteArray stdOut = programmerProc->readAllStandardOutput();
        ui->consoleText->append(stdOut);

    });

    QStringList argList;

    argList.append("-t");
    argList.append(progTool);
    argList.append("-d");
    argList.append(deviceId);
    argList.append("-i");
    argList.append(progIF);
    argList.append(arguments);


    programmerProc->setProgram(atProgramPath);
//    programmerProc->setArguments(QStringList("h   elp"));
//    programmerProc->start();
    programmerProc->setArguments(argList);

    qDebug() << programmerProc->arguments();
    programmerProc->start();

    programmerProc->waitForStarted();

    return true;
}
