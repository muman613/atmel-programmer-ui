#include <QDebug>
#include <QFile>
#include <QString>
#include <QStringList>
#include <QObject>
#include <QRegularExpression>
#include <QMetaProperty>

#include "flashscript.h"
#include "flashenv.h"

flashScript::flashScript(QObject * parent)
    :   QObject(parent)
{
}

flashScript::flashScript(QString flashFilePath, QObject * parent)
    :   QObject(parent)
{
    loadScriptFromFile(flashFilePath);
}

/**
 * @brief Load script from file.
 *
 * @param flashFilePath - String containing path and filename.
 * @param env - flashEnv object containing current settings.
 * @return true on success, false on failure.
 */
bool flashScript::loadScriptFromFile(const QString & flashFilePath)
{
    qDebug() << Q_FUNC_INFO;

    clear();

    QFile       scriptFile(flashFilePath);
    bool        result = false;

    if (scriptFile.open(QIODevice::ReadOnly)) {
        auto scriptData = scriptFile.readAll();

        script = QString(scriptData).trimmed().split("\n");
        qDebug() << script;

        scriptFile.close();
        loadedPath = flashFilePath;
        result = true;
    }

    return result;
}

/**
 * @brief Load script from passed string.
 *
 * @param script_string - String containing flash script
 * @param env - flashEnv object containing current settings.
 * @return true on success, false on failure.
 */
bool flashScript::loadScriptFromString(const QString &script_string)
{
    qDebug() << Q_FUNC_INFO;

    clear();
    script = script_string.trimmed().split("\n");
    return (script.size() > 0);
}

/**
 * @brief Get original script into QByteArray
 * @return
 */
QByteArray  flashScript::getScript() const {
    QByteArray scr = script.join("\n").toLocal8Bit();

    return scr;
}

bool flashScript::parse(flashEnv *env)
{
    qDebug() << Q_FUNC_INFO;

    if (env != nullptr) {
        qDebug() << "Performing variable replacement...";
        parsedScript.clear();

        QString newLine;

        for (const auto & scriptLine : script) {
            newLine = parseScriptLine(scriptLine, env);
            parsedScript.push_back(newLine);
        }
    }

    return (parsedScript.size() > 0);
}

/**
 * @brief Use QProcess to spawn a new process. Commands are stored in the
 *        `parsedScript` QStringList
 * @return
 */
bool flashScript::spawnProcess()
{
    QString cmdLine;

    cmdLine = parsedScript[cmdIndex];
    QStringList args = cmdLine.split(" ");
    QString exe = args[0];
    args.removeFirst();

    qDebug() << "Executing command :" << cmdLine;
    qDebug() << "exe=" << exe << " args :" << args;

    process = new QProcess(this);


    connect(process, &QProcess::started, [=]() {
        QString sId = QString("%1:%2").arg(scriptId).arg(cmdIndex + 1);
        cmdInProgress = true;
        qDebug() << "Process started";
//        emit commandStart(prgrmrIndex, command);
        emit scriptStarted(sId);
    } );

    connect(process,
        QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        [=](int exitCode, QProcess::ExitStatus exitStatus) {
//            QString exitMsg = exitCodeToString(exitCode);
//            qDebug() << "Process finished" << exitMsg << exitStatus;
            qDebug() << "Process finished | code : " << exitCode << " status : " << exitStatus;

            QString sId = QString("%1:%2").arg(scriptId).arg(cmdIndex + 1);

            emit scriptCompleted(sId);

            delete process;
            process = nullptr;
            cmdInProgress = false;

            // If there are more commands in the list, then start them in order...
            if (++cmdIndex < parsedScript.size()) {
                qDebug() << "Execute next command :" << parsedScript[cmdIndex];
                spawnProcess();
            }
//            emit commandEnd(prgrmrIndex, command);
        }
    );

    connect(process, &QProcess::readyReadStandardOutput, [=]() {
        QByteArray stdOut = process->readAllStandardOutput();
        qDebug() << stdOut;
//        emit statusText(prgrmrIndex, AtmelProgrammer::STREAM_STDOUT, stdOut);
    });

    connect(process, &QProcess::readyReadStandardError, [=]() {
        QByteArray stdErr = process->readAllStandardError();
        qDebug() << stdErr;
//        emit statusText(prgrmrIndex, AtmelProgrammer::STREAM_STDERR, stdErr);
    });

    process->setProgram(exe);
    process->setArguments(args);

    process->start();

    return process->waitForStarted();
}

void flashScript::execute(flashEnv * env)
{
    qDebug() << Q_FUNC_INFO;
    if (parse(env)) {
        cmdIndex = 0;
        spawnProcess();
    }
}

/**
 * @brief Replace all "${}" strings with values from environment.
 *
 * @param line
 * @param env
 * @return
 */
QString flashScript::parseScriptLine(const QString &line, flashEnv * env)
{
    Q_ASSERT(env != nullptr);

    bool                bDone = false;
    QString             newLine = line.trimmed();
    QRegularExpression  re("\\$\\{(\\w+)\\}");

    while (!bDone) {
        QRegularExpressionMatch match = re.match(newLine);

        if (match.hasMatch()) {
            QString varName = match.captured(1);
//          qDebug() << "matched" << varName;
            QString replaceStr = env->property(varName.toLocal8Bit().data()).toString();
            if (environContains(env, varName)) {
                newLine.replace(match.capturedStart(0), match.capturedLength(0), replaceStr);
            } else {
                qDebug() << "Unable to replace" << varName << "!";
                newLine.replace(match.capturedStart(0), match.capturedLength(0), "_NA_");
            }
        } else {
            bDone = true;
        }
    }

    return newLine;
}

/**
 * @brief Determine if the property exists in the QObject...
 *
 * @param obj - QObject to inspect.
 * @param propName - Property name to search for.
 * @return
 */
bool flashScript::environContains(QObject *obj, QString propName)
{
    const QMetaObject * meta = obj->metaObject();
    return (meta->indexOfProperty(propName.toLocal8Bit().data()) != -1)?true:false;
}

