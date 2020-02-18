#include "flashscript.h"
#include <QDebug>
#include <QFile>
#include <QString>
#include <QStringList>
#include <QObject>

flashScript::flashScript()
{

}

flashScript::flashScript(QString flashFilePath)
{
    loadScriptFromFile(flashFilePath);
}

bool flashScript::loadScriptFromFile(const QString & flashFilePath)
{
    qDebug() << Q_FUNC_INFO;

    clear();

    QFile       scriptFile(flashFilePath);
    bool        result = false;

    if (scriptFile.open(QIODevice::ReadOnly)) {
        auto scriptData = scriptFile.readAll();

        script = QString(scriptData).trimmed().split("\n");
        scriptFile.close();
        loadedPath = flashFilePath;
        result = true;
    }

    return result;
}

bool flashScript::loadScriptFromString(const QString &script_string)
{
    qDebug() << Q_FUNC_INFO;

    clear();
    loadedPath.clear();

    script = script_string.split("\n");

    return (script.size() > 0);
}

QByteArray  flashScript::getScript() const {
    QByteArray scr = script.join("\n").toLocal8Bit();

    return scr;
}

