#ifndef FLASHENV_H
#define FLASHENV_H

#include <QObject>

class flashEnv : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString tool READ tool WRITE setTool NOTIFY envChanged)
    Q_PROPERTY(QString interface READ interface WRITE setInterface NOTIFY envChanged)
    Q_PROPERTY(QString serial READ serial WRITE setSerial NOTIFY envChanged)
    Q_PROPERTY(QString device READ device WRITE setDevice NOTIFY envChanged)
    Q_PROPERTY(QString fwPath READ fwPath WRITE setFwpath NOTIFY envChanged)

public:
    explicit flashEnv(QObject *parent = nullptr);

    void setTool(QString tool) {
        if (tool != m_tool) {
            m_tool = tool;
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

    void setFwpath(const QString & fwPath) {
        if (fwPath != m_fwPath) {
            m_fwPath = fwPath;
            emit envChanged();
        }
    }
    QString fwPath() const {
        return m_fwPath;
    }

private:
    QString m_tool;           // atmelice/etc
    QString m_interface;            // jtag/isp/etc
    QString m_serial;               // SN of Atmelice device
    QString m_device;               // device (atmega328p)
    QString m_fwPath;               // path to fw hex image

signals:
    void envChanged();

};

#endif // FLASHENV_H
