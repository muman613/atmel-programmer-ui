#include <QDialog>
#include "optiondialog.h"
#include "ui_optiondlg.h"

/**
 * List of supported interfaces.
 */

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

/**
 * @brief Constructor for option dialog box
 *
 * @param pParent Parent window of dialog.
 */
optionDialog::optionDialog(QWidget * pParent)
    :   QDialog(pParent)
{
    ui = new Ui_optionDialog();
    ui->setupUi(this);

    // Add all the interfaces supported...
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

/**
 * @brief Set the verbose flag to be used by the programmer.
 * @param bVerbose true to enable verbose output.
 */
void optionDialog::setVerbose(bool bVerbose)
{
    ui->verboseCheck->setChecked(bVerbose);
}


/**
 * @brief Return the current verbose setting for programmer.
 * @return true if verbose is set.
 */
bool optionDialog::getVerbose()
{
    return ui->verboseCheck->isChecked();
}
