#include <QFileDialog>
#include <QDebug>
#include <QSettings>
#include <QMessageBox>
#include "mainwindow.h"
#include "ui_mainwindow.h"

QStringList supportedDevices = {
    "atmega328p",
    "atmega2560"
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    QSettings iniSettings;

    ui->setupUi(this);

    for (auto device : supportedDevices) {
        ui->deviceId->addItem(device);
    }

    // Load settings from persistent storage, if not set use defaults...
    fwPath          = iniSettings.value("fwPath").toString();
    atProgramPath   = iniSettings.value("atprogram", "D:/Program Files (x86)/Atmel/Studio/7.0/atbackend/atprogram.exe").toString();
    progTool        = iniSettings.value("progTool", "atmelice").toString();
    deviceId        = iniSettings.value("deviceId", "atmega328p").toString();
    progIF          = iniSettings.value("progIF", "isp").toString();

    ui->firmwareName->setText(fwPath);
    ui->deviceId->setCurrentText(deviceId);

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


void MainWindow::on_infoButton_clicked()
{
    qDebug() << Q_FUNC_INFO;

    if (executeCommand("info")) {
        qDebug() << "OK";
    }
}

void MainWindow::on_programButton_clicked()
{
    QStringList extraArgs;

    extraArgs.append("-fl");        // Verify Flash memory
    extraArgs.append("-e");
    extraArgs.append("--verify");
    extraArgs.append("-f");         // -f [filename]
    extraArgs.append(fwPath);

    if (executeCommand("program", &extraArgs)) {

    }

}

void MainWindow::on_verifyButton_clicked()
{
    QStringList extraArgs;

    extraArgs.append("-fl");        // Verify Flash memory
    extraArgs.append("-f");         // -f [filename]
    extraArgs.append(fwPath);

    if (executeCommand("verify", &extraArgs)) {

    }
}

void MainWindow::on_eraseButton_clicked()
{
    if (executeCommand("chiperase")) {
        qDebug() << "OK";
    }
}

bool MainWindow::executeCommand(const QString &command, QStringList * extraArgs)
{
    if (cmdInProgress || (programmerProc != nullptr)) {
        qDebug() << "Attempted to start command while previous command in progress...";
        return false;
    }

    ui->consoleText->append(QString("-").repeated(80));

    programmerProc = new QProcess();

    connect(programmerProc, &QProcess::started, [=]() {
        cmdInProgress = true;
        enableControls(false);
        qDebug() << "Process started";
    } );

    connect(programmerProc,
            QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [=](int exitCode, QProcess::ExitStatus exitStatus) {
                QString exitMsg = exitCodeToString(exitCode);
                qDebug() << "Process finished" << exitMsg << exitStatus;
                if (exitCode != 0) {
                    ui->consoleText->append(exitMsg);
                    ui->consoleText->setFontWeight(QFont::Bold);
                    ui->consoleText->append(programmerProc->readAllStandardError());
                    ui->consoleText->setFontWeight(QFont::Normal);
                }
                delete programmerProc;
                programmerProc = nullptr;
                cmdInProgress = false;
                enableControls(true);
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
    argList.append(command);
    if (extraArgs) {
        argList += *extraArgs;
    }

    programmerProc->setProgram(atProgramPath);
    programmerProc->setArguments(argList);

    qDebug() << programmerProc->arguments();
    programmerProc->start();

//    programmerProc->waitForStarted();

    return true;
}

void MainWindow::enableControls(bool en)
{
    ui->infoButton->setEnabled(en);
    ui->programButton->setEnabled(en);
    ui->verifyButton->setEnabled(en);
    ui->eraseButton->setEnabled(en);
    ui->browseButton->setEnabled(en);
    ui->firmwareName->setEnabled(en);
}

typedef struct _codeEntry {
    int         code;
    QString     msg;
} CODE_ENTRY;

QString MainWindow::exitCodeToString(int code)
{
    CODE_ENTRY codeTable[] = {
        { 0, "Success", },
        { 1, "Unexpected Error", },
        { 2, "User Interrupt Error", },
        { 10, "Command Error", },
        { 11, "Command Argument Error", },
        { 12, "Missing Command Error", },
        { 13, "Command Parse Error", },
        { 14, "Tool Error", },
        { 15, "Resource Error", },
        { 16, "Tcf Error", },
        { 17, "Timeout Error", },
        { 18, "Missing or Wrong Input Error", },
        { 19, "Old Fw Exception", },
        { 20, "Backend Creation Error", },
        { 21, "Option Parse Error", },
        { 22, "JSON Parse Error", },
        { 23, "Verify Error", },
    };

    for (auto entry : codeTable) {
        if (entry.code == code)
            return entry.msg;
    }

    return "Unrecognized Error Code";
}

void MainWindow::on_actionClear_Console_triggered()
{
    ui->consoleText->clear();
}

void MainWindow::on_deviceId_currentTextChanged(const QString &arg1)
{
    qDebug() << "User selected device" << arg1;

    deviceId = arg1;
}

void MainWindow::on_actionSave_Console_to_file_triggered()
{
    QString logFilename = QFileDialog::getSaveFileName(this, tr("Save Log to File"));

    if (!logFilename.isEmpty()) {
        QFile       logFile(logFilename);

        if (logFile.open(QIODevice::WriteOnly)) {
            logFile.write(ui->consoleText->document()->toPlainText().toLocal8Bit());
        }
    }
}


void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, "Atmel Programmer", "(C) 2020 Wunder-Bar\nProgrammer : Michael Uman");
}
