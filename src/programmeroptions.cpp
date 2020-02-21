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

    if (obj.contains("fuses")) {
        qDebug() << "Found fuses";

        QJsonObject fuseObj = obj["fuses"].toObject();

        if (fuseObj.contains("H")) {
            fuseH = fuseObj["H"].toString();
        }
        if (fuseObj.contains("L")) {
            fuseL = fuseObj["L"].toString();
        }
        if (fuseObj.contains("E")) {
            fuseE = fuseObj["E"].toString();
        }
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

    obj["fuses"]        = QJsonObject{
                            { "H", fuseH },
                            { "L", fuseL },
                            { "E", fuseE },
                          };

    // Save the script lines as JSON array...
    QJsonArray  scriptArray;
    QStringList scriptLines = QString(flashscript).split("\n");

    foreach(const QString & line, scriptLines) {
        scriptArray.push_back(line);
    }

    obj["script"]       = scriptArray;

    return true;
}

/**
 * @brief Load options from JSON configuration file.
 * @param filePath - Path of file to load configuration from.
 * @param option_vec - QVector of options found in the config file.
 * @return true if file was loaded.
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

/**
 * @brief Save all programmer options to a JSON file.
 * @param filePath - Path of file to save to.
 * @param option_vec - Options stored in a QVector
 * @return true
 */
bool saveOptionsToJSON(const QString &filePath, const optionVec &option_vec) {
    QFile   configFile(filePath);

    if (configFile.open(QIODevice::WriteOnly)) {
        QJsonObject     root;
        int             index = 1;

        foreach(auto prgrmrOpt, option_vec) {
            QJsonObject     programmerObj;
            QString         sectionName = QString("programmer%1").arg(index++);

            prgrmrOpt.saveOptionsToObject(programmerObj);

            root.insert(sectionName, programmerObj);
        }

        QJsonDocument doc(root);
        if (configFile.write(doc.toJson()) == -1) {
            qWarning() << "Unable to save JSON configuration";
        }
    }

    return true;
}
