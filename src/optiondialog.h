#ifndef OPTIONDIALOG_H
#define OPTIONDIALOG_H

#include <QDialog>
#include "ui_optiondlg.h"



class optionDialog : public QDialog, public Ui::optionDialog
{
public:
    optionDialog(QWidget * pParent = nullptr);

    void setInterface(const QString & tool);
    QString getInterface() const;

    void setFriendlyName(const QString & name);
    QString getFriendlyName() const;

    void setVerbose(bool bVerbose);
    bool getVerbose() const;

    void setFlashScript(const QByteArray &  script);
    QByteArray getFlashScript() const;

    void setFuses(const QString & fuseH, const QString & fuseL, const QString & fuseE);
    void setFuses(const QStringList & fuseList);
    void getFuses(QString & fuseH, QString & fuseL, QString & fuseE);
    QStringList getFuses();

private:
    Ui_optionDialog * ui = nullptr;

};

#endif // OPTIONDIALOG_H
