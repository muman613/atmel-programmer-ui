#ifndef PROGRAMMEROPTIONS_H
#define PROGRAMMEROPTIONS_H

#include <QDebug>
#include <QString>
#include <QJsonObject>
#include <QVector>

/**
 * @brief The programmerOptions class
 */
class programmerOptions {
public:
    programmerOptions();
//    virtual ~programmerOptions();

    void            clear();

    bool            loadOptionsFromObject(QJsonObject & obj);
    bool            saveOptionsToObject(QJsonObject & obj);

    // Provided debug overload
    friend QDebug operator << (QDebug dbg, const programmerOptions & options);

    QString         friendlyName;   ///< Friendly name used when displayed
    QString         atProgramPath;  ///< Path to the atprogram.exe
    QString         fwPath;         ///< Path to the .hex image file
    QString         progTool;       ///< -t Tool option for atprogram
    QString         progIF;         ///< -i Interface option for atprogram
    QString         progSN;         ///< -s Serial # of Atmelice programmer
    QString         deviceId;       ///< -d Device id of target device
    QString         fuseH;          ///< Fuse High Byte
    QString         fuseL;          ///< Fuse Low Byte
    QString         fuseE;          ///< Extended Fuse Byte
    QString         lockBits;       ///< Lock bits
    QByteArray      flashscript;    ///< Script text in a byte array

    bool            verbose;        ///< -v Verbose option of atprogram
};

using optionVec = QVector<programmerOptions>;


bool loadOptionsFromJSON(const QString & filePath, optionVec & options);
bool saveOptionsToJSON(const QString &filePath, const optionVec &option_vec);

#endif // PROGRAMMEROPTIONS_H
