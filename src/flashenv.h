#ifndef FLASHENV_H
#define FLASHENV_H

#include <QObject>
#include <QDebug>

class flashEnv : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString CMD READ cmd WRITE setCmd NOTIFY envChanged)
    Q_PROPERTY(QString TOOL READ tool WRITE setTool NOTIFY envChanged)
    Q_PROPERTY(QString IF READ interface WRITE setInterface NOTIFY envChanged)
    Q_PROPERTY(QString SN READ serial WRITE setSerial NOTIFY envChanged)
    Q_PROPERTY(QString DEVID READ device WRITE setDevice NOTIFY envChanged)
    Q_PROPERTY(QString IMAGE READ image WRITE setImage NOTIFY envChanged)
    Q_PROPERTY(QString FUSES MEMBER m_fuses NOTIFY envChanged)
    Q_PROPERTY(QString VERBOSE MEMBER m_verbose NOTIFY envChanged)

public:
    explicit flashEnv(QObject *parent = nullptr);

    void setCmd(const QString & command) {
        if (command != m_cmd) {
            m_cmd = command;
            emit envChanged();
        }
    }

    QString cmd() const {
        return m_cmd;
    }

    void setTool(QString prgrmr) {
        if (prgrmr != m_tool) {
            m_tool = prgrmr;
            emit envChanged();
        }
    }

    QString tool() const {
        return m_tool;
    }

    void setInterface(const QString & interface) {
        if (interface != m_interface) {
            m_interface = interface;
            emit envChanged();
        }
    }

    QString interface() const {
        return m_interface;
    }

    void setSerial(const QString & serial) {
        if (serial != m_serial) {
            m_serial = serial;
            emit envChanged();
        }
    }

    QString serial() const {
        return m_serial;
    }

    void setDevice(const QString & device) {
        if (device != m_device) {
            m_device = device;
            emit envChanged();
        }
    }

    QString device() const {
        return m_device;
    }

    void setImage(const QString & fwPath) {
        if (fwPath != m_fwPath) {
            m_fwPath = fwPath;
            emit envChanged();
        }
    }
    QString image() const {
        return m_fwPath;
    }

    void setVerbose(const bool flag) {
        m_verbose = (flag)?"-v":"";
        if (flag != bVerbose) {
            bVerbose = flag;
            emit envChanged();
        }
    }

    bool verbose() const {
        return bVerbose;
    }

    void setFuseH(quint8 value) {
        m_fuseh = value;
        formatFuse();
    }
    void setFuseL(quint8 value) {
        m_fusel = value;
        formatFuse();
    }
    void setFuseE(quint8 value) {
        m_fusee = value;
        formatFuse();
    }

    void setFuses(const QStringList & fuseList) {
        bool isok = false;
        if (fuseList.size() == 3) {
            m_fuseh = fuseList[0].toInt(&isok, 0);
            m_fusel = fuseList[1].toInt(&isok, 0);
            m_fusee = fuseList[2].toInt(&isok, 0);

            formatFuse();
        }
    }

    /* NOTE: The order for the fuses following --values is low/high/ext. */

    void formatFuse() {
        m_fuses = QString("%1%2%3").arg(QString::number(m_fusel, 16).toUpper()).arg(QString::number(m_fuseh, 16).toUpper()).arg(QString::number(m_fusee, 16).toUpper());
    }

private:
    QString m_cmd;                  // 'atprogram.exe'
    QString m_tool;                 // atmelice/etc
    QString m_interface;            // jtag/isp/etc
    QString m_serial;               // SN of Atmelice device
    QString m_device;               // device (atmega328p)
    QString m_fwPath;               // path to fw hex image
    QString m_verbose;              // Enable/Disable verbose output
    QString m_fuses;

    int     m_fuseh;
    int     m_fusel;
    int     m_fusee;

    bool    bVerbose = false;
signals:
    void envChanged();

};

#endif // FLASHENV_H
