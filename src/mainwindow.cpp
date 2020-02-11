#include <QFileDialog>
#include <QDebug>
#include <QSettings>
#include <QMessageBox>
#include <QTimer>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "optiondialog.h"

QStringList supportedDevices = {
    "atmega162",
    "atmega328p",
    "atmega1280",
    "atmega2560"
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QTimer::singleShot(500, this, [=]() {
       allocateProgrammers();
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::enableControls(bool en)
{
    Q_UNUSED(en)
//    ui->infoButton->setEnabled(en);
//    ui->programButton->setEnabled(en);
//    ui->verifyButton->setEnabled(en);
//    ui->eraseButton->setEnabled(en);
//    ui->browseButton->setEnabled(en);
//    ui->firmwareName->setEnabled(en);
}


/**
 * @brief MainWindow::getProgrammerList gets the list of programmers connected.
 *
 * @return
 */
bool MainWindow::getProgrammerList()
{
    AtmelProgrammer::getProgrammerList(programmerList);

    qDebug() << programmerList;

    return true;
}

void MainWindow::allocateProgrammers()
{
    qDebug() << Q_FUNC_INFO;

    getProgrammerList();

    for (int i = 0 ; i < programmers.size() ; i++) {
        delete programmers[i];
    }
    programmers.clear();

    if (programmerList.size() < 3) {
        qDebug() << "Insufficient programmers available!";
    }

    for (int i = 0 ; i < 3 ; i++) {
        QString grpbx       = QString("programmer%1").arg(i + 1);
        QWidget * grpBox    = ui->centralwidget->findChild<QWidget*>(grpbx);

        if (i < programmerList.size()) {
            QString prgrmrId    = QString("id_%1").arg(i + 1);
            QString devId       = QString("device_%1").arg(i + 1);
            QString fwPath      = QString("fw_%1").arg(i + 1);
            QString browser     = QString("browse_%1").arg(i + 1);
            QString options     = QString("options_%1").arg(i + 1);

            AtmelProgrammer * newPrgrmr = new AtmelProgrammer(this, i);
            programmers.push_back(newPrgrmr);
            newPrgrmr->initialize();
            QComboBox * idWidget = grpBox->findChild<QComboBox*>(prgrmrId);
            Q_ASSERT(idWidget != nullptr);

            for (auto prgrmr : programmerList) {
                idWidget->addItem(prgrmr.second);
            }

            QString tmpString = newPrgrmr->getSerialNum();

            if (!tmpString.isEmpty()) {
                idWidget->setCurrentText(newPrgrmr->getSerialNum());
            }

            QComboBox * devWidget = grpBox->findChild<QComboBox*>(devId);
            Q_ASSERT(devWidget != nullptr);

            for (auto devid : supportedDevices) {
                devWidget->addItem(devid);
            }

            tmpString = newPrgrmr->getDeviceId();
            if (!tmpString.isEmpty()) {
                devWidget->setCurrentText(tmpString);
            }

            QLineEdit * fwWidget = grpBox->findChild<QLineEdit*>(fwPath);
            Q_ASSERT(fwWidget != nullptr);

            tmpString = newPrgrmr->getFirmware();
            if (!tmpString.isEmpty()) {
                fwWidget->setText(tmpString);
            }

            QPushButton * browseBut  = grpBox->findChild<QPushButton*>(browser);
            QPushButton * optionsBut = grpBox->findChild<QPushButton*>(options);
            Q_ASSERT(browseBut != nullptr);
            Q_ASSERT(optionsBut != nullptr);

            // Connect the dots...
            connect(idWidget, QOverload<const QString &>::of(&QComboBox::currentTextChanged), [=](const QString & text) {
               qDebug() << "Programmer " << i << " S/N changed to " << text;
               programmers[i]->setSerialNum(text);
            });
            connect(devWidget, QOverload<const QString &>::of(&QComboBox::currentTextChanged), [=](const QString & text) {
               qDebug() << "Programmer " << i << " device changed to " << text;
               programmers[i]->setDeviceId(text);
            });
            connect(fwWidget, QOverload<const QString &>::of(&QLineEdit::textChanged), [=](const QString & text) {
               qDebug() << "Programmer " << i << " firmware changed to " << text;
               programmers[i]->setFirmware(text);
            });
            connect(browseBut, QOverload<bool>::of(&QPushButton::clicked), [=](bool clicked) {
               qDebug() << "Browse button for prgrmr " << i << " clicked " << clicked;
               QFileDialog     chooseFile(this, "Select Firmware File");

               chooseFile.setNameFilter(tr("Hex (*.hex)"));
               chooseFile.setViewMode(QFileDialog::Detail);
               chooseFile.setFileMode(QFileDialog::ExistingFile);

               if (chooseFile.exec()) {
                   QStringList FileNames = chooseFile.selectedFiles();
                   QString fwPath = FileNames[0];
                   programmers[i]->setFirmware(fwPath);
                   qDebug() << "Firmware path =" << fwPath;
                   fwWidget->setText(fwPath);
               }
            });
            connect(optionsBut, QOverload<bool>::of(&QPushButton::clicked), [=](bool clicked) {
               qDebug() << "Options button for prgrmr " << i << " clicked " << clicked;

               optionDialog     options;

               options.setInterface(programmers[i]->getInterface());

               if (options.exec()) {
                   programmers[i]->setInterface(options.getInterface());
               }

            });

            connect(newPrgrmr, &AtmelProgrammer::statusText, [=](int index, AtmelProgrammer::streamId id,  QByteArray text) {
                qDebug() << "Status Text :" << index << id << text;
                if (id == AtmelProgrammer::STREAM_STDERR) {
                    ui->console->setFontWeight(QFont::Bold);
                }
                ui->console->append(text);
                if (id == AtmelProgrammer::STREAM_STDERR) {
                    ui->console->setFontWeight(QFont::Normal);
                }
            });

            connect(newPrgrmr, &AtmelProgrammer::commandStart, [=](int ndx, QString command) {
                qDebug() << "Command Started :" << ndx << command;
                enableProgrammer(ndx, false);

                QString sMsg = QString("Started '%1' of programmer %2").arg(command).arg(ndx);
                ui->console->append(sMsg);

                busyPrgmCnt++;
                if (busyPrgmCnt != 0) {
                    ui->programButton->setEnabled(false);
                    ui->verifyButton->setEnabled(false);
                }
            });

            connect(newPrgrmr, &AtmelProgrammer::commandEnd, [=](int ndx, QString command) {
                qDebug() << "Command Ended :" << ndx << command;
                enableProgrammer(ndx, true);

                QString sMsg = QString("Ended '%1' of programmer %2").arg(command).arg(ndx);
                ui->console->append(sMsg);
                busyPrgmCnt--;
                if (busyPrgmCnt == 0) {
                    ui->programButton->setEnabled(true);
                    ui->verifyButton->setEnabled(true);
                }
            });
        } else {
            qDebug() << "Disable programmer " << i + 1;
            grpBox->setDisabled(true);
        }
    }
}

void MainWindow::enableProgrammer(int index, bool en)
{
    QString grpbx       = QString("programmer%1").arg(index + 1);
    QWidget * grpBox    = ui->centralwidget->findChild<QWidget*>(grpbx);
    Q_ASSERT(grpBox != nullptr);

    grpBox->setEnabled(en);
}


void MainWindow::on_actionClear_Console_triggered()
{
    ui->console->clear();
}

void MainWindow::on_actionSave_Console_to_file_triggered()
{
    QString logFilename = QFileDialog::getSaveFileName(this, tr("Save Log to File"));

    if (!logFilename.isEmpty()) {
        QFile       logFile(logFilename);

        if (logFile.open(QIODevice::WriteOnly)) {
            logFile.write(ui->console->document()->toPlainText().toLocal8Bit());
        }
    }
}


void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, "Atmel Programmer", "(C) 2020 Wunder-Bar\nProgrammer : Michael Uman");
}

void MainWindow::on_programButton_clicked()
{
    for (auto prgrmr : programmers) {
        int index           = prgrmr->index() + 1;
        QString grpbx       = QString("programmer%1").arg(index);
        QWidget * grpBox    = ui->centralwidget->findChild<QWidget*>(grpbx);

        grpBox->setEnabled(false);

        prgrmr->program();
    }
}
