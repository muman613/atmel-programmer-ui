#ifndef FLASHSCRIPT_H
#define FLASHSCRIPT_H

#include <QDebug>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QProcess>
#include "flashenv.h"

class flashScript : public QObject {
    Q_OBJECT
public:
    flashScript(QObject * parent = nullptr);
    flashScript(QString flashFilePath, QObject * parent = nullptr);

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

    void            execute(flashEnv *env);

    void            setId(int id) {
        scriptId = id;
    }
signals:
    void            scriptStarted(QString ident);
    void            scriptCompleted(QString ident);

private:
    QString         parseScriptLine(const QString & line, flashEnv * env);

    bool            environContains(QObject * obj, QString propName);

    bool            spawnProcess();

    int             scriptId;

    QString         loadedPath;
    QStringList     script;
    QStringList     parsedScript;

    bool            cmdInProgress   = false;
    int             cmdIndex        = 0;
    QProcess *      process         = nullptr;
};

#endif // FLASHSCRIPT_H
