#include <QDebug>
#include <QFile>
#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
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
        flashscript = obj["script"].toString().toLocal8Bit();
    }

    return true;
}

bool programmerOptions::saveOptionsToObject(QJsonObject &obj)
{
    Q_UNUSED(obj)
    return false;
}


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
