#ifndef ATMELPROGRAMMER_H
#define ATMELPROGRAMMER_H

#include <QDebug>
#include <QObject>
#include <QProcess>

using prgrmrPair = QPair<QString, QString>;
using prgrmrPairList = QList<prgrmrPair>;

class AtmelProgrammer : public QObject
{
    Q_OBJECT
public:
    explicit AtmelProgrammer(QObject *parent = nullptr, int index = 0);
    ~AtmelProgrammer();

    static bool     getProgrammerList(prgrmrPairList & prgrmrlist);

    bool            program();
    bool            chiperase();
    bool            verify();

    static QString  exitCodeToString(int code);

    bool            isConfigured() const;

    void            setFirmware(const QString & firmwarePath) {
        fwPath = firmwarePath;
        emit parmsChanged(prgrmrIndex);
    }

    QString         getFirmware() const {
        return fwPath;
    }

    void            setTool(const QString & tool) {
        progTool = tool;
        emit parmsChanged(prgrmrIndex);
    }

    QString         getTool() const {
        return progTool;
    }

    void            setInterface(const QString & interface) {
        progIF = interface;
        emit parmsChanged(prgrmrIndex);
    }

    QString         getInterface() const {
        return progIF;
    }

    void            setSerialNum(const QString & serialNum) {
        qDebug() << "Serialnumber changed to" << serialNum;
        progSN = serialNum;
        emit parmsChanged(prgrmrIndex);
    }

    QString         getSerialNum() const {
        return progSN;
    }

    void            setDeviceId(const QString & devid) {
        qDebug() << Q_FUNC_INFO;
        deviceId = devid;
        emit parmsChanged(prgrmrIndex);
    }

    QString         getDeviceId() const {
        return deviceId;
    }

signals:
    void            commandStart(int index, QString command);
    void            commandEnd(int index, QString command);
    void            parmsChanged(int index);
    void            statusText(int index, QByteArray text);

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
};

#endif // ATMELPROGRAMMER_H
