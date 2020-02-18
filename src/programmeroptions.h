#ifndef PROGRAMMEROPTIONS_H
#define PROGRAMMEROPTIONS_H

#include <QDebug>
#include <QString>
#include <QJsonObject>
#include <QVector>

class programmerOptions {
public:
    programmerOptions();
    virtual ~programmerOptions();

    void            clear();

    bool            loadOptionsFromObject(QJsonObject & obj);
    bool            saveOptionsToObject(QJsonObject & obj);

    friend QDebug operator << (QDebug dbg, const programmerOptions & options);

    QString         friendlyName;
    QString         atProgramPath;
    QString         fwPath;
    QString         progTool;
    QString         progIF;
    QString         progSN;
    QString         deviceId;
    QString         fuseH;
    QString         fuseL;
    QString         fuseE;
    QString         lockBits;
    QByteArray      flashscript;

    bool            verbose;
};

using optionVec = QVector<programmerOptions>;


bool loadOptionsFromJSON(const QString & filePath, optionVec & options);

#endif // PROGRAMMEROPTIONS_H
