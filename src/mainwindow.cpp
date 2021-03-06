#include <QFileDialog>
#include <QDebug>
#include <QSettings>
#include <QMessageBox>
#include <QTimer>
#include <QStandardPaths>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "optiondialog.h"
#include "programmeroptions.h"
#include "help.h"

/**
 * @brief MainWindow Constructor
 * @param parent
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Check if the help file is available, if not disable help menu option...
    if (!QFile::exists(HELPFILENAME)) {
        QList<QMenu *> menu = ui->menubar->findChildren<QMenu*>("menu_Help");

        foreach(QAction * act, menu[0]->actions()) {
            if (act->objectName() == HELPMENU) {
                qDebug() << "Found menu" << act->text();
                act->setEnabled(false);
                ui->console->append("QtHelp not available!");
            }
        }
    }

    QTimer::singleShot(100, this, [=]() {
        // check if the atprogram.exe is in the path
        if (QStandardPaths::findExecutable("atprogram.exe").isEmpty()) {
            QMessageBox::critical(this, "Critical Error",
                                  "'atprogram.exe' is not in the path");
            close();
            return;
        }

        allocateProgrammers();

        // If there are no AtmelICE programmers found, display message and quit...
        if (programmers.size() == 0) {
            QMessageBox::critical(this, "Critical Error",
                                  "No Programmers Found");
            close();
        } else if (programmers.size() < 3) {
            QMessageBox::warning(this, "Warning",
                                 "Tool requires 3 programmers");
        }
    });
}

/**
 * @brief Main Window Destructor
 */
MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief MainWindow::getProgrammerList gets the list of programmers connected.
 *
 * @return
 */
bool MainWindow::getProgrammerList()
{
    AtmelProgrammer::getProgrammerList(programmerList);
    if (!programmerList.isEmpty())
        qDebug() << programmerList;
    else
        qDebug() << "NO PROGRAMMERS FOUND!";

    return !programmerList.isEmpty();
}

/**
 * @brief Get the list of programmers and allocate each one (up to three).
 */
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
        QGroupBox * grpBox  = ui->centralwidget->findChild<QGroupBox*>(grpbx);

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

            QString prgName = newPrgrmr->getFriendlyName();
            grpBox->setTitle(prgName);

            QString tmpString = newPrgrmr->getSerialNum();

            if (!tmpString.isEmpty()) {
                idWidget->setCurrentText(newPrgrmr->getSerialNum());
            }

            QComboBox * devWidget = grpBox->findChild<QComboBox*>(devId);
            Q_ASSERT(devWidget != nullptr);

            newPrgrmr->addSupportedDevices(devWidget);

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
               qDebug() << "Programmer " << i + 1 << " S/N changed to " << text;
               programmers[i]->setSerialNum(text);
            });
            connect(devWidget, QOverload<const QString &>::of(&QComboBox::currentTextChanged), [=](const QString & text) {
               qDebug() << "Programmer " << i + 1 << " device changed to " << text;
               programmers[i]->setDeviceId(text);
            });
            connect(fwWidget, QOverload<const QString &>::of(&QLineEdit::textChanged), [=](const QString & text) {
               qDebug() << "Programmer " << i + 1 << " firmware changed to " << text;
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
               qDebug() << "Options button for prgrmr " << i + 1 << " clicked " << clicked;

               optionDialog     options;

               options.setInterface(programmers[i]->getInterface());
               options.setVerbose(programmers[i]->getVerbose());
               options.setFriendlyName(programmers[i]->getFriendlyName());
               options.setFlashScript(programmers[i]->getFlashScript());
               options.setFuses(programmers[i]->getFuses());

               if (options.exec()) {
                   programmers[i]->setInterface(options.getInterface());
                   programmers[i]->setVerbose(options.getVerbose());
                   programmers[i]->setFriendlyname(options.getFriendlyName());
                   programmers[i]->setFlashScript(options.getFlashScript());
                   programmers[i]->setFuses(options.getFuses());

                   grpBox->setTitle(options.getFriendlyName());
               }
            });

            connect(newPrgrmr, &AtmelProgrammer::statusText, [=](int index, flashScript::streamId id,  QByteArray text) {
                QString stream = QString(text).replace("\r", "").trimmed();


                qDebug() << "Status Text :" << index << id << stream;
                if (id == flashScript::STREAM_STDERR) {
                    ui->console->setFontWeight(QFont::Bold);
                }
                QStringList lines = stream.split("\n");
                foreach (const QString & line, lines) {
                    QString format = QString("%1 - %2").arg(index).arg(line);
                    ui->console->append(format);
                }
                if (id == flashScript::STREAM_STDERR) {
                    ui->console->setFontWeight(QFont::Normal);
                }
            });

            connect(newPrgrmr, &AtmelProgrammer::commandStart, [=](int ndx, QString command) {
                QString friendlyName = this->programmers[ndx]->getFriendlyName();
                qDebug() << "Command Started :" << ndx + 1 << command;
                enableProgrammer(ndx, false);

                QString sMsg = QString("Started '%1' of programmer '%2'").arg(command).arg(friendlyName);
                ui->console->append(sMsg);

                busyPrgmCnt++;
                if (busyPrgmCnt != 0) {
                    ui->programButton->setEnabled(false);
                    ui->verifyButton->setEnabled(false);
                }
            });

            connect(newPrgrmr, &AtmelProgrammer::commandEnd, [=](int ndx, QString command) {
                QString friendlyName = this->programmers[ndx]->getFriendlyName();
                qDebug() << "Command Ended :" << ndx + 1 << command;
                enableProgrammer(ndx, true);

                QString sMsg = QString("Ended '%1' of programmer '%2'").arg(command).arg(friendlyName);
                ui->console->append(sMsg);
                busyPrgmCnt--;
                if (busyPrgmCnt == 0) {
                    ui->programButton->setEnabled(true);
                    ui->verifyButton->setEnabled(true);
                }
            });
            grpBox->setDisabled(false);
        } else {
            qDebug() << "Disable programmer " << i + 1;
            grpBox->setDisabled(true);
        }
    }

    QString sMsg = QString("Found %1 AtmelICE programmers.").arg(programmers.size());
    ui->console->append(sMsg);
}

/**
 * @brief Enable/Disable programmer by index #
 *
 * @param index - Index of Programmer to enable/disable
 * @param en - true to enable, false to disable.
 */
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

/**
 * @brief What to do when the about menu item is selected.
 */
void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, "Atmel Programmer",
                       "(C) 2020 Wunder-Bar\nProgrammer : Michael Uman");
}

/**
 * @brief What to do when the user hits the program button.
 */
void MainWindow::on_programButton_clicked()
{
    // iterate through all programmers and call the programmers program function
    for (auto prgrmr : programmers) {
        int index           = prgrmr->index() + 1;
        QString grpbx       = QString("programmer%1").arg(index);
        QString enabled     = QString("enabled_%1").arg(index);
//        QWidget * grpBox    = ui->centralwidget->findChild<QWidget*>(grpbx);
        QCheckBox * chkBox  = ui->centralwidget->findChild<QCheckBox*>(enabled);

        Q_ASSERT(chkBox != nullptr);

        if (chkBox->isChecked()) {
            // disable each programmers groupbox
            prgrmr->program();
        }
    }
}

/**
 * @brief What to do when the user hits the verify button.
 */
void MainWindow::on_verifyButton_clicked()
{
    for (auto prgrmr : programmers) {
        int index           = prgrmr->index() + 1;
        QString grpbx       = QString("programmer%1").arg(index);
        QWidget * grpBox    = ui->centralwidget->findChild<QWidget*>(grpbx);

        grpBox->setEnabled(false);

        prgrmr->verify();
    }
}

/**
 * @brief Allow user to select configuration file from the file dialog.
 */
void MainWindow::on_actionLoad_Configuration_triggered()
{
    QFileDialog     chooseFile(this, "Load Configuration File...");

    chooseFile.setNameFilter(tr("JSON (*.json)"));
    chooseFile.setViewMode(QFileDialog::Detail);
    chooseFile.setFileMode(QFileDialog::ExistingFile);

    if (chooseFile.exec()) {
        optionVec       opts;
        QString         configPath = chooseFile.selectedFiles()[0];

        if (loadOptionsFromJSON(configPath, opts)) {
            auto optIter = opts.begin();
            auto prgrmrIter = programmers.begin();

            qDebug() << opts;

            for ( ; (optIter != opts.end()) && (prgrmrIter != programmers.end()) ; optIter++, prgrmrIter++) {

                // Set the options on the programmer object...
                (*prgrmrIter)->setProgrammerOptions(*optIter);

                int id = (*prgrmrIter)->index() + 1;

                QString     grpbx   = QString("programmer%1").arg(id);
                QGroupBox * grpBox  = ui->centralwidget->findChild<QGroupBox*>(grpbx);

                QString prgrmrId    = QString("id_%1").arg(id);
                QString devId       = QString("device_%1").arg(id);
                QString fwPath      = QString("fw_%1").arg(id);
                QString browser     = QString("browse_%1").arg(id);
                QString options     = QString("options_%1").arg(id);

                QWidget * pWidget;

                pWidget = grpBox->findChild<QWidget*>(prgrmrId);
                static_cast<QComboBox*>(pWidget)->setCurrentText((*optIter).progSN);

                pWidget = grpBox->findChild<QWidget*>(devId);
                static_cast<QComboBox*>(pWidget)->setCurrentText((*optIter).deviceId);

                pWidget = grpBox->findChild<QWidget*>(fwPath);
                static_cast<QLineEdit*>(pWidget)->setText((*optIter).fwPath);

                qDebug() << grpBox;
            }
        }
    }
}

void MainWindow::on_actionSave_Configuration_triggered()
{
    QFileDialog     chooseFile(this, "Save Configuration File As...");

    chooseFile.setNameFilter(tr("JSON (*.json)"));
    chooseFile.setViewMode(QFileDialog::Detail);
    chooseFile.setFileMode(QFileDialog::AnyFile);
    chooseFile.setAcceptMode(QFileDialog::AcceptSave);

    if (chooseFile.exec()) {
        optionVec       options;
        QString         configName = chooseFile.selectedFiles()[0];

        qDebug() << "Saving as" << configName;

        foreach(auto  prgrmr, programmers) {
            int index = prgrmr->index();

            programmerOptions opts;

            prgrmr->getProgrammerOptions(opts);
            qDebug() << "programmer" << index << opts;

            options.push_back(opts);
        }

        saveOptionsToJSON(configName, options);
    }
}

void MainWindow::on_actionMulti_Programmer_Help_triggered()
{
    qDebug() << Q_FUNC_INFO;

    qDebug() << "help process" << helpProcess;

    if (helpProcess != nullptr) {
        if (helpProcess->state() == QProcess::Running) {
            qDebug() << "Killing running process";
            helpProcess->kill();
        }
        qDebug() << "deleting process";
        delete helpProcess;
        helpProcess = nullptr;
    }

    helpProcess = new QProcess(this);

    QString     helpFilename = QGuiApplication::applicationDirPath() +
                               QDir::separator() + HELPFILENAME;

    QStringList args;

    args.append("-collectionFile");
    args.append(helpFilename);

    helpProcess->setProgram("assistant.exe");
    helpProcess->setArguments(args);

    helpProcess->start();

    if (!helpProcess->waitForStarted()) {
        qDebug() << "Assistant has not started!";
    }
}
