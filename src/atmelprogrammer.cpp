#include <QDebug>
#include <QSettings>
#include <QComboBox>

#include "atmelprogrammer.h"


QDebug operator<<(QDebug debug, const AtmelProgrammer &p)
{
    QDebugStateSaver save(debug);

    debug.nospace();
    debug.noquote();

    debug << "( Programmer ";
    debug << p.prgrmrIndex << " TOOL : " << p.progTool << " | IF : " << p.progIF << " | SN : " << p.progSN << " )";

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

//static QStringList ss;

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
    friendlyName = QString("Programmer #%1").arg(index + 1);
    flash_script.setId(index + 1);

    // Connect-up the signal handler lambdas...
    connect(&flash_script, &flashScript::scriptStarted, [=](QString sId) {
        QStringList items = sId.split(":");
        int progId = items[0].toInt(),
            cmdId  = items[1].toInt();

        qDebug() << "lambda::scriptStarted(" << sId << ")";

        // Send commandStart signal only on first command in sequence...
        if (cmdId == 1) {
            emit commandStart((progId - 1), "program");
        }
    });

    connect(&flash_script, &flashScript::scriptCompleted, [=](QString sId) {
        QStringList items = sId.split(":");
        int progId = items[0].toInt();
        qDebug() << "lambda::scriptCompleted(" << sId << ")";
        emit commandEnd((progId - 1), "program");
    });

    connect(&flash_script, &flashScript::scriptOutput, [=](int prgrmrID, flashScript::streamId stream, QByteArray & data) {
        qDebug() << Q_FUNC_INFO << "|" << prgrmrID << stream << data;
        emit statusText(prgrmrID, stream, data);
    });
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
    iniSettings.setValue("friendlyName",QVariant(friendlyName));
    iniSettings.setValue("fuses",       QVariant(getFuses()));

    QByteArray flScr = flash_script.getScript();
    iniSettings.setValue("flashScript", QVariant(flScr));

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
    friendlyName    = iniSettings.value("friendlyName", friendlyName).toString();

    setFuses(iniSettings.value("fuses").toStringList());

    QByteArray flScr;

    flScr = iniSettings.value("flashScript").toByteArray();
    flash_script.loadScriptFromString(flScr);

    iniSettings.endGroup();

//    emit parmsChanged(prgrmrIndex);
}

/**
 * @brief AtmelProgrammer::setProgrammerOptions
 * @param opts
 */
void AtmelProgrammer::setProgrammerOptions(const programmerOptions &opts)
{
    qDebug() << "setProgrammerOptions :" << opts;

    progTool        = opts.progTool;
    deviceId        = opts.deviceId;
    progIF          = opts.progIF;
    progSN          = opts.progSN;
    friendlyName    = opts.friendlyName;
    fwPath          = opts.fwPath;
    verbose         = opts.verbose;
    fuseH           = opts.fuseH;
    fuseL           = opts.fuseL;
    fuseE           = opts.fuseE;

    flash_script.loadScriptFromString(opts.flashscript);
}

void AtmelProgrammer::getProgrammerOptions(programmerOptions &opts)
{
    qDebug() << Q_FUNC_INFO;

    opts.progTool       = progTool;
    opts.deviceId       = deviceId;
    opts.progIF         = progIF;
    opts.progSN         = progSN;
    opts.friendlyName   = friendlyName;
    opts.fwPath         = fwPath;
    opts.verbose        = verbose;
    opts.fuseH          = fuseH;
    opts.fuseL          = fuseL;
    opts.fuseE          = fuseE;

    opts.flashscript    = flash_script.getScript();
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

/**
 * @brief Set the flash script for this programmer
 * @param script
 */
void AtmelProgrammer::setFlashScript(const QByteArray &script)
{
    flash_script.loadScriptFromString(script);
}

/**
 * @brief Get the flash script from the programmer.
 * @return
 */
QByteArray AtmelProgrammer::getFlashScript() const
{
    return flash_script.getScript();
}

void AtmelProgrammer::setFuses(const QString &hFuse, const QString &lFuse, const QString &eFuse)
{
    if ((hFuse != fuseH) || (lFuse != fuseL) || (eFuse != fuseE)) {
        fuseH = hFuse;
        fuseL = lFuse;
        fuseE = eFuse;

        emit parmsChanged(prgrmrIndex);
    }
}

void AtmelProgrammer::setFuses(const QStringList &fuselist)
{
    if (fuselist.size() == 3) {
        setFuses(fuselist[0], fuselist[1], fuselist[2]);
    } else {
        qWarning() << "setFuses called with invalid # of elements in list";
    }
}

/**
 * @brief AtmelProgrammer::getFuses
 * @return QStringList containing fuseH, fuseL, fuseE order.
 */
QStringList AtmelProgrammer::getFuses()
{
    QStringList fuseList = { fuseH, fuseL, fuseE };
    return fuseList;
}

/**
 * @brief Start the programming here...
 * @return
 */
bool AtmelProgrammer::program()
{
    qDebug() << "executing flash script...";

    flashEnv env;

    // Set up the environment for variable replacement...
    env.setCmd("atprogram.exe");
    env.setTool(progTool);
    env.setInterface(progIF);
    env.setImage(fwPath);
    env.setDevice(deviceId);
    env.setSerial(progSN);
    env.setVerbose(verbose);
    env.setFuses(getFuses());

    flash_script.execute(&env);

    return true;
}

bool AtmelProgrammer::chiperase()
{
    if (!isConfigured()) {
        qDebug() << "Programmer " << prgrmrIndex << " not configured!";
        return false;
    }
#if 0
    if (executeCommand("chiperase")) {
        qDebug() << "OK";
    }
#endif

    return true;
}

bool AtmelProgrammer::verify()
{
    if (!isConfigured()) {
        qDebug() << "Programmer " << prgrmrIndex << " not configured!";
        return false;
    }

#if 0
    QStringList extraArgs;

    extraArgs.append("-fl");        // Verify Flash memory
    extraArgs.append("-f");         // -f [filename]
    extraArgs.append(fwPath);

    if (executeCommand("verify", &extraArgs)) {

    }
#endif

    return true;
}

#if 0
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
#endif

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

    return (prgrmrlist.size() > 0);
}
