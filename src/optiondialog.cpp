#include <QDialog>
#include "optiondialog.h"
#include "ui_optiondlg.h"


QStringList interfaceList = {
    "aWire",
    "debugWire",
    "HVPP",
    "HVSP",
    "ISP",
    "JTAG",
    "PDI",
    "UPDI",
    "TPI",
    "SWD"
};

optionDialog::optionDialog(QWidget * pParent)
    :   QDialog(pParent)
{
    ui = new Ui_optionDialog();
    ui->setupUi(this);


    for (auto iface: interfaceList) {
        ui->ifCombo->addItem(iface);
    }
}

void optionDialog::setInterface(const QString &interface)
{
    ui->ifCombo->setCurrentText(interface);
}

QString optionDialog::getInterface() const
{
    return ui->ifCombo->currentText();
}

