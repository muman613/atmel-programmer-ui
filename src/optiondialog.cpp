#include <QDebug>
#include <QDialog>
#include "optiondialog.h"
#include "ui_optiondlg.h"

/**
 * List of supported interfaces.
 */

static QStringList interfaceList = {
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

void optionDialog::setFriendlyName(const QString &name)
{
    ui->friendlyName->setText(name);
}

QString optionDialog::getFriendlyName() const
{
    return ui->friendlyName->text();
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
bool optionDialog::getVerbose() const
{
    return ui->verboseCheck->isChecked();
}

void optionDialog::setFlashScript(const QByteArray &script)
{
    ui->flashScript->setText(script);
}

QByteArray optionDialog::getFlashScript() const
{
    return ui->flashScript->toPlainText().toLocal8Bit();
}

void optionDialog::setFuses(const QString &fuseH, const QString &fuseL, const QString &fuseE)
{
    ui->hfuse->setText(fuseH);
    ui->lfuse->setText(fuseL);
    ui->efuse->setText(fuseE);
}

void optionDialog::setFuses(const QStringList &fuseList)
{
    if (fuseList.size() == 3) {
        setFuses(fuseList[0], fuseList[1], fuseList[2]);
    } else {
        qWarning() << Q_FUNC_INFO << "called with insufficient parameters";
    }
}

void optionDialog::getFuses(QString &fuseH, QString &fuseL, QString &fuseE)
{
    fuseH = ui->hfuse->text();
    fuseL = ui->hfuse->text();
    fuseE = ui->efuse->text();
}

QStringList optionDialog::getFuses()
{
    QStringList fuseList = {
        ui->hfuse->text(),
        ui->lfuse->text(),
        ui->efuse->text(),
    };
    return fuseList;
}
