#ifndef ATMELPROGRAMMER_H
#define ATMELPROGRAMMER_H

#include <QDebug>
#include <QObject>
#include <QProcess>
#include <QMutex>
#include "programmeroptions.h"
#include "flashscript.h"

class AtmelProgrammer;
class QComboBox;

// Useful types...
using prgrmrPair        = QPair<QString, QString>;
using prgrmrPairList    = QList<prgrmrPair>;
using prgrmrVec         = QVector<AtmelProgrammer *>;

/**
 * @brief The AtmelProgrammer class
 */
class AtmelProgrammer : public QObject
{
    Q_OBJECT
public:

    explicit AtmelProgrammer(QObject *parent = nullptr, int index = 0);
    ~AtmelProgrammer();

    friend QDebug operator<<(QDebug debug, const AtmelProgrammer &p);

    void            addSupportedDevices(QComboBox * pComboBox);

    void            initialize();

    void            setProgrammerOptions(const programmerOptions & opts);
    void            getProgrammerOptions(programmerOptions & opts);

    int             index() const {
        return prgrmrIndex;
    }

    static bool     getProgrammerList(prgrmrPairList & prgrmrlist);

    bool            program();
    bool            chiperase();
    bool            verify();

    static QString  exitCodeToString(int code);

    bool            isConfigured() const;

    void            setFirmware(const QString & firmwarePath) {
        if (firmwarePath != fwPath) {
            qDebug() << "Firmware changed to" << firmwarePath;
            fwPath = firmwarePath;
            emit parmsChanged(prgrmrIndex);
        }
    }

    QString         getFirmware() const {
        return fwPath;
    }

    void            setTool(const QString & tool) {
        if (tool != progTool) {
            qDebug() << "Tool changed to" << tool;
            progTool = tool;
            emit parmsChanged(prgrmrIndex);
        }
    }

    QString         getTool() const {
        return progTool;
    }

    void            setInterface(const QString & interface) {
        if (interface != progIF) {
            qDebug() << "Interface changed to" << interface;
            progIF = interface;
            emit parmsChanged(prgrmrIndex);
        }
    }

    QString         getInterface() const {
        return progIF;
    }

    void            setSerialNum(const QString & serialNum) {
        if (progSN != serialNum) {
            qDebug() << "Serialnumber changed to" << serialNum;
            progSN = serialNum;
            emit parmsChanged(prgrmrIndex);
        }
    }

    QString         getSerialNum() const {
        return progSN;
    }

    void            setDeviceId(const QString & devid) {
        if (devid != deviceId) {
            qDebug() << "deviceId changed to" << devid;
            deviceId = devid;
            emit parmsChanged(prgrmrIndex);
        }
    }

    QString         getDeviceId() const {
        return deviceId;
    }

    void            setVerbose(bool en) {
        verbose = en;
    }
    bool            getVerbose() const {
        return verbose;
    }

    void            setFriendlyname(const QString & name)
    {
        if (name != friendlyName) {
            qDebug() << "Friendly name changed to" << name;
            friendlyName = name;
            emit parmsChanged(prgrmrIndex);
        }
    }

    QString         getFriendlyName() const {
        return friendlyName;
    }

    void            setFlashScript(const QByteArray & script);
    QByteArray      getFlashScript() const;


    void            setFuses(const QString & hFuse, const QString & lFuse, const QString & eFuse);
    void            setFuses(const QStringList & fuselist);
    QStringList     getFuses(); // HLE order

signals:
    void            commandStart(int index, QString command);
    void            commandEnd(int index, QString command);
    void            parmsChanged(int index);
    void            statusText(int index, flashScript::streamId id,  QByteArray text);

private:
    int             prgrmrIndex = 0;
    bool            verbose = false;

    bool            executeCommand(const QString &command,
                                   QStringList * extraArgs = nullptr);

    bool            cmdInProgress = false;

    QString         friendlyName;

    QString         fwPath;
    QString         progTool;
    QString         deviceId;
    QString         progIF;
    QString         progSN;

    QString         atProgramPath;

    QString         fuseH,
                    fuseL,
                    fuseE;

    flashScript     flash_script;

    QProcess *      programmerProc = nullptr;
    QMutex          prgrmrMutex;

    QStringList     scriptStatus;
};

#endif // ATMELPROGRAMMER_H
