#ifndef ATMELPROGRAMMER_H
#define ATMELPROGRAMMER_H

#include <QDebug>
#include <QObject>
#include <QProcess>
#include <QMutex>

class AtmelProgrammer;

using prgrmrPair = QPair<QString, QString>;
using prgrmrPairList = QList<prgrmrPair>;

using prgrmrVec = QVector<AtmelProgrammer *>;

class AtmelProgrammer : public QObject
{
    Q_OBJECT
public:

    enum streamId {
        STREAM_STDOUT,
        STREAM_STDERR,
    };

    explicit AtmelProgrammer(QObject *parent = nullptr, int index = 0);
    ~AtmelProgrammer();

    void            initialize();

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
            qDebug() << Q_FUNC_INFO;
            deviceId = devid;
            emit parmsChanged(prgrmrIndex);
        }
    }

    QString         getDeviceId() const {
        return deviceId;
    }

signals:
    void            commandStart(int index, QString command);
    void            commandEnd(int index, QString command);
    void            parmsChanged(int index);
    void            statusText(int index, AtmelProgrammer::streamId id,  QByteArray text);

private:
    int             prgrmrIndex = 0;

    bool            executeCommand(const QString &command,
                                   QStringList * extraArgs = nullptr);

    bool            cmdInProgress = false;

    QString         progTool;
    QString         deviceId;
    QString         progIF;
    QString         progSN;

    QString         atProgramPath;
    QString         fwPath;

    QString         fuseH, fuseL, fuseE;

    QProcess *      programmerProc = nullptr;
    QMutex          prgrmrMutex;
};

#endif // ATMELPROGRAMMER_H
