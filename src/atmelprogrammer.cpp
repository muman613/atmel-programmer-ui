#include <QDebug>
#include <QSettings>
#include <QComboBox>

#include "atmelprogrammer.h"


QDebug operator<<(QDebug debug, const AtmelProgrammer &p)
{
    QDebugStateSaver save(debug);

    debug.nospace();
    debug.noquote();

#if 1
    debug << "( Programmer ";
    debug << p.prgrmrIndex << " TOOL : " << p.progTool << " | IF : " << p.progIF << " | SN : " << p.progSN << " )";
#else
    debug.noquote();
    debug.nospace();

    debug << "Programmer " << p.prgrmrIndex << ":" << "\n";
    debug << QString("-").repeated(20) << "\n";

    debug << "programmerPath : " << p.atProgramPath << "\n";
    debug << "fwPath         : " << p.fwPath << "\n";
    debug << "progTool       : " << p.progTool << "\n";
    debug << "deviceId       : " << p.deviceId << "\n";
    debug << "Interface      : " << p.progIF << "\n";
    debug << "serialNum      : " << p.progSN << "\n";
    debug << "verbose        : " << p.verbose << "\n";

    debug << QString("-").repeated(20) << "\n";
#endif

    return debug;
}

/**
 * List of supported devices.
 */

static QStringList supportedDevices = {
    "atmega162",
    "atmega328p",
    "atmega1280",
    "atmega2560"
};

/**
 * @brief AtmelProgrammer Constructor
 * @param parent
 * @param index
 */
AtmelProgrammer::AtmelProgrammer(QObject *parent, int index)
    : QObject(parent),
      prgrmrIndex(index),
      atProgramPath("atprogram.exe")
{

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
    iniSettings.setValue("bVerbose",    QVariant(verbose));

    iniSettings.endGroup();
}

/**
 * @brief Add supported devices to the combobox...
 * @param pComboBox
 */
void AtmelProgrammer::addSupportedDevices(QComboBox *pComboBox)
{
    pComboBox->clear();
    for (auto devid : supportedDevices) {
        pComboBox->addItem(devid);
    }
}

/**
 * @brief Retreive settings from the persistant storage (QSettings)
 *
 * NOTE: The path to `atprogram.exe` should be in the path environment.
 */
void AtmelProgrammer::initialize()
{
    qDebug() << Q_FUNC_INFO;

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
    verbose         = iniSettings.value("bVerbose",     false).toBool();

    iniSettings.endGroup();

#if 0
    qDebug() << "------------------------------";
    qDebug() << "Index" << prgrmrIndex;
    qDebug() << "fwPath " << fwPath;
    qDebug() << "progTool" << progTool;
    qDebug() << "deviceId" << deviceId;
    qDebug() << "progIF" << progIF;
    qDebug() << "progSN" << progSN;
    qDebug() << "------------------------------";
#endif

    emit parmsChanged(prgrmrIndex);
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

/**
 * @brief Run the `atprogram.exe` programmer passing exta arguments.
 *
 * @param command - atprogram.exe major command
 * @param extraArgs - atprogram.exe exta arguments
 * @return
 */
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
//                if (exitCode != 0) {
//                    QByteArray stdErr = programmerProc->readAllStandardError();
//                    emit statusText(prgrmrIndex, AtmelProgrammer::STREAM_STDERR, stdErr);
//                }
                delete programmerProc;
                programmerProc = nullptr;
                cmdInProgress = false;
                emit commandEnd(prgrmrIndex, command);
            }
    );

    connect(programmerProc, &QProcess::readyReadStandardOutput, [=]() {
        QByteArray stdOut = programmerProc->readAllStandardOutput();
        qDebug() << stdOut;
        emit statusText(prgrmrIndex, AtmelProgrammer::STREAM_STDOUT, stdOut);
    });

    connect(programmerProc, &QProcess::readyReadStandardError, [=]() {
        QByteArray stdErr = programmerProc->readAllStandardError();
        qDebug() << stdErr;
        emit statusText(prgrmrIndex, AtmelProgrammer::STREAM_STDERR, stdErr);
    });

    QStringList argList;

    // If verbose flag is set, pass -v to atprogram.exe
    if (verbose) {
        argList.append("-v");
    }

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

/**
 * @brief Use `atprogram.exe list` to get list of available programmers.
 *
 * @param prgrmrlist
 * @return
 */
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

        // skip 'simulator' entry in programmer list...
        if (name != "simulator") {
            prgrmrlist.push_back(prgrmrPair(name,sn));
        }
    }

    return (bool)(prgrmrlist.size() > 0);
}
