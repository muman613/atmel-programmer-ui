#ifndef FLASHSCRIPT_H
#define FLASHSCRIPT_H

#include <QDebug>
#include <QString>
#include <QStringList>

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
        return script.begin();
    }

    QStringList::iterator end() {
        return script.end();
    }

    void clear() {
        script.clear();
    }

    QByteArray getScript() const;

private:
    QString         loadedPath;
    QStringList     script;
};

#endif // FLASHSCRIPT_H
