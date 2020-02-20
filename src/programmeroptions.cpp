#include <QDebug>
#include <QFile>
#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QDebug>
#include "programmeroptions.h"

QDebug operator <<(QDebug dbg, const programmerOptions &options)
{
    QDebugStateSaver saver(dbg);

//    dbg.noquote();
    dbg.nospace();

    dbg << "( " << options.friendlyName << ", " << options.progTool << ", " <<
           options.progIF << ", " << options.progSN << ", " <<
           options.fwPath <<
           " )";

    return dbg;
}

/**
 * @brief programmerOptions::programmerOptions
 */
programmerOptions::programmerOptions()
    : atProgramPath("atprogram.exe")
{
    qDebug() << Q_FUNC_INFO;
}

/**
 * @brief programmerOptions::~programmerOptions
 */
programmerOptions::~programmerOptions()
{
    qDebug() << Q_FUNC_INFO;
}

/**
 * @brief Clear programmer options.
 */
void programmerOptions::clear()
{
    qDebug() << Q_FUNC_INFO;

    friendlyName.clear();
    atProgramPath = "atprogram.exe";
    fwPath.clear();
    progTool.clear();
    progIF.clear();
    progSN.clear();
    deviceId.clear();
}

/**
 * @brief Load programmer options from JSON object.
 * @param obj
 * @return
 */
bool programmerOptions::loadOptionsFromObject(QJsonObject &obj)
{
    qDebug() << Q_FUNC_INFO;

    clear();

    if (obj.contains("name")) {
        friendlyName = obj["name"].toString();
    }

    if (obj.contains("atprogram")) {
        atProgramPath = obj["atprogram"].toString();
    }

    if (obj.contains("fw")) {
        fwPath = obj["fw"].toString();
    }

    if (obj.contains("tool")) {
        progTool = obj["tool"].toString();
    }

    if (obj.contains("if")) {
        progIF = obj["if"].toString();
    }

    if (obj.contains("sn")) {
        progSN = obj["sn"].toString();
    }

    if (obj.contains("device")) {
        deviceId = obj["device"].toString();
    }

    if (obj.contains("verbose")) {
        verbose = obj["verbose"].toBool();
    }

    if (obj.contains("script")) {
        QJsonArray scriptArray = obj["script"].toArray();
        QStringList p;

        for (auto line : scriptArray) {
            qDebug() << line.toString();
            p.push_back(line.toString());
        }

        flashscript = p.join("\n").toLocal8Bit();
    }

    return true;
}

/**
 * @brief Save the options to a JSON object.
 *
 * @param obj
 * @return
 */
bool programmerOptions::saveOptionsToObject(QJsonObject &obj)
{
    qDebug() << Q_FUNC_INFO;

    obj["name"]         = friendlyName;
    obj["atprogram"]    = atProgramPath;
    obj["fw"]           = fwPath;
    obj["tool"]         = progTool;
    obj["if"]           = progIF;
    obj["sn"]           = progSN;
    obj["device"]       = deviceId;
    obj["verbose"]      = verbose;

//    QJsonArray scriptArray;

//    obj["script"]       = QJsonArray( flashscript.split("\n");

//    obj.insert("name",          QJsonValue(friendlyName));
//    obj.insert("atprogram",     QJsonValue(atProgramPath));
//    obj.insert("fw",            QJsonValue(fwPath));
//    obj.insert("tool",          QJsonValue(progTool));
//    obj.insert("if",            QJsonValue(progIF));
//    obj.insert("sn",            QJsonValue(progSN));
//    obj.insert("device",        QJsonValue(deviceId));
//    obj.insert("verbose",       QJsonValue(verbose));
//    obj.insert("script",        QJsonValue(QString(flashscript)));

    return true;
}

/**
 * @brief Load options f
 * @param filePath
 * @param option_vec
 * @return
 */
bool loadOptionsFromJSON(const QString &filePath, optionVec &option_vec)
{
    bool bResult = false;

    qDebug() << Q_FUNC_INFO;

    QFile       configFile(filePath);

    if (configFile.open(QIODevice::ReadOnly)) {
        QString         fileContents = configFile.readAll();
        QJsonDocument   doc = QJsonDocument::fromJson(fileContents.toLocal8Bit());

        if (doc.isObject()) {
            QJsonObject     rootObj = doc.object();

            for (const auto obj : rootObj) {
                QJsonObject prgrmObj = obj.toObject();
                programmerOptions options;

                if (options.loadOptionsFromObject(prgrmObj)) {
                    qDebug() << options;
                    option_vec.push_back(std::move(options));
                }
            }

            bResult = true;
        }
    } else {
        qDebug() << "ERROR : " << configFile.error();
    }

    return bResult;
}
