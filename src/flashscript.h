#ifndef FLASHSCRIPT_H
#define FLASHSCRIPT_H

#include <QDebug>
#include <QString>
#include <QStringList>
#include "flashenv.h"

class flashScript {
public:
    flashScript();
    flashScript(QString flashFilePath);

    bool        loadScriptFromFile(const QString & flashFilePath);
    bool        loadScriptFromString(const QString & script);

    int         size() const {
        return script.length();
    }

    QStringList::iterator  begin() {
        return parsedScript.begin();
    }

    QStringList::iterator end() {
        return parsedScript.end();
    }

    void clear() {
        qDebug() << Q_FUNC_INFO;
        script.clear();
        parsedScript.clear();
        loadedPath.clear();
    }

    QByteArray      getScript() const;
    bool            parse(flashEnv *env);

private:
    QString         parseScriptLine(const QString & line, flashEnv * env);

    bool            environContains(QObject * obj, QString propName);

    QString         loadedPath;
    QStringList     script;
    QStringList     parsedScript;
};

#endif // FLASHSCRIPT_H
