#include <QDebug>
#include <QSettings>
#include "atmelprogrammer.h"

AtmelProgrammer::AtmelProgrammer(QObject *parent, int index)
    : QObject(parent),
      prgrmrIndex(index),
      atProgramPath("atprogram.exe")
{
    QSettings iniSettings;

    iniSettings.beginGroup(QString("Programmer%1").arg(prgrmrIndex));

    // Load settings from persistent storage, if not set use defaults...
    // "D:/Program Files (x86)/Atmel/Studio/7.0/atbackend/atprogram.exe"
    fwPath          = iniSettings.value("fwPath").toString();
    atProgramPath   = iniSettings.value("atprogram",    "atprogram.exe").toString();
    progTool        = iniSettings.value("progTool",     "atmelice").toString();
    deviceId        = iniSettings.value("deviceId",     "atmega328p").toString();
    progIF          = iniSettings.value("progIF",       "isp").toString();
    progSN          = iniSettings.value("ProgSN").toString();

    iniSettings.endGroup();

    qDebug() << "------------------------------";
    qDebug() << "Index" << index;
    qDebug() << "fwPath " << fwPath;
    qDebug() << "progTool" << progTool;
    qDebug() << "deviceId" << deviceId;
    qDebug() << "progIF" << progIF;
    qDebug() << "progSN" << progSN;
    qDebug() << "------------------------------";
}

AtmelProgrammer::~AtmelProgrammer()
{
    QSettings iniSettings;

    iniSettings.beginGroup(QString("Programmer%1").arg(prgrmrIndex));

    iniSettings.setValue("fwPath",      QVariant(fwPath));
    iniSettings.setValue("atprogram",   QVariant(atProgramPath));
    iniSettings.setValue("progTool",    QVariant(progTool));
    iniSettings.setValue("deviceId",    QVariant(deviceId));
    iniSettings.setValue("progIF",      QVariant(progIF));
    iniSettings.setValue("ProgSN",      QVariant(progSN));

    iniSettings.endGroup();
}

typedef struct _codeEntry {
    int         code;
    QString     msg;
} CODE_ENTRY;

/**
 * @brief Convert exit code to meaningful text.
 * @param code - error code
 * @return String
 */
QString AtmelProgrammer::exitCodeToString(int code)
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

bool AtmelProgrammer::isConfigured() const
{
    if (fwPath.isEmpty() ||
        atProgramPath.isEmpty() ||
        progSN.isEmpty() ||
        progIF.isEmpty() ||
        deviceId.isEmpty() ||
        progTool.isEmpty())
    {
        return false;
    } else {
        return true;
    }
}

bool AtmelProgrammer::program()
{
    QStringList extraArgs;
    qDebug() << Q_FUNC_INFO;

    extraArgs.append("-fl");        // Verify Flash memory
    extraArgs.append("-e");
    extraArgs.append("--verify");
    extraArgs.append("-f");         // -f [filename]
    extraArgs.append(fwPath);

    if (executeCommand("program", &extraArgs)) {

    }

    return true;
}

bool AtmelProgrammer::chiperase()
{
    if (!isConfigured()) {
        qDebug() << "Programmer " << prgrmrIndex << " not configured!";
        return false;
    }

    if (executeCommand("chiperase")) {
        qDebug() << "OK";
    }

    return true;
}

bool AtmelProgrammer::verify()
{
    QStringList extraArgs;

    extraArgs.append("-fl");        // Verify Flash memory
    extraArgs.append("-f");         // -f [filename]
    extraArgs.append(fwPath);

    if (executeCommand("verify", &extraArgs)) {

    }

    return true;
}

bool AtmelProgrammer::executeCommand(const QString &command, QStringList * extraArgs)
{
    QMutexLocker        lock(&prgrmrMutex);

    if (cmdInProgress || (programmerProc != nullptr)) {
        qDebug() << "Attempted to start command while previous command in progress...";
        return false;
    }

    programmerProc = new QProcess();

    connect(programmerProc, &QProcess::started, [=]() {
        cmdInProgress = true;
        qDebug() << "Process started";
        emit commandStart(prgrmrIndex, command);
    } );

    connect(programmerProc,
            QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [=](int exitCode, QProcess::ExitStatus exitStatus) {
                QString exitMsg = exitCodeToString(exitCode);
                qDebug() << "Process finished" << exitMsg << exitStatus;
                if (exitCode != 0) {
//                    ui->consoleText->append(exitMsg);
//                    ui->consoleText->setFontWeight(QFont::Bold);
//                    ui->consoleText->append(programmerProc->readAllStandardError());
//                    ui->consoleText->setFontWeight(QFont::Normal);
                    qDebug() << programmerProc->readAllStandardError();
                }
                delete programmerProc;
                programmerProc = nullptr;
                cmdInProgress = false;
                emit commandEnd(prgrmrIndex, command);
            }
    );

    connect(programmerProc, &QProcess::readyReadStandardOutput, [=]() {
        QByteArray stdOut = programmerProc->readAllStandardOutput();
        qDebug() << stdOut;
        emit statusText(prgrmrIndex, stdOut);
    });

    QStringList argList;

    argList.append("-t");
    argList.append(progTool);
    argList.append("-d");
    argList.append(deviceId);
    argList.append("-i");
    argList.append(progIF);
    argList.append("-s");
    argList.append(progSN);

    argList.append(command);

    if (extraArgs) {
        argList += *extraArgs;
    }

    programmerProc->setProgram(atProgramPath);
    programmerProc->setArguments(argList);

    qDebug() << programmerProc->arguments();
    programmerProc->start();

    return true;
}

bool AtmelProgrammer::getProgrammerList(prgrmrPairList & prgrmrlist)
{
    QProcess programmer;

    prgrmrlist.clear();

    programmer.setProgram("atprogram.exe");
    programmer.setArguments({ "list" });

    programmer.start();

    programmer.waitForFinished();

    QString stdOut = programmer.readAllStandardOutput();

    QStringList lines = stdOut.split("\n", QString::SplitBehavior::SkipEmptyParts);
    for (auto line : lines) {
        QString name = line.left(16).trimmed();
        QString sn = line.mid(16).trimmed();

        qDebug() << "name" << name << "s/n" << sn;

        prgrmrlist.push_back(prgrmrPair(name,sn));
    }

    return true;
}
