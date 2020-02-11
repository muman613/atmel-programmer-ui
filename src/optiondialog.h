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
    void setVerbose(bool bVerbose);
    bool getVerbose() const;

private:
    Ui_optionDialog * ui = nullptr;

};

#endif // OPTIONDIALOG_H
