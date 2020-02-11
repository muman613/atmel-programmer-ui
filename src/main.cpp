#include <QDebug>
#include <QDateTime>
#include <QApplication>
#include <QFileInfo>
#include "mainwindow.h"

#define LOGFILE_NAME        "programmer.log"

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QDateTime ts = QDateTime::currentDateTime().toLocalTime();
    QFileInfo fileName(context.file);

    FILE * fp = nullptr;

    fp = fopen(LOGFILE_NAME, "a");

    QByteArray localMsg = msg.toLocal8Bit();
//    const char *file = context.file ? context.file : "";
//    const char *function = context.function ? context.function : "";

    QString     funcLine = QString("(%1:%2)").arg(context.function).arg(context.line);

    switch (type) {
    case QtDebugMsg:
        fprintf(fp, "%s - Debug   : %-30s | %-70s : %s\n",
                ts.toString().toLocal8Bit().constData(),
                fileName.fileName().toLocal8Bit().constData(),
                funcLine.toLocal8Bit().constData(),
                localMsg.constData());
        break;
    case QtInfoMsg:
        fprintf(fp, "%s - Info    : %-30s | %-70s : %s\n",
                ts.toString().toLocal8Bit().constData(),
                fileName.fileName().toLocal8Bit().constData(),
                funcLine.toLocal8Bit().constData(),
                localMsg.constData());
        break;
    case QtWarningMsg:
        fprintf(fp, "%s - Warning : %-30s | %-70s : %s\n",
                ts.toString().toLocal8Bit().constData(),
                fileName.fileName().toLocal8Bit().constData(),
                funcLine.toLocal8Bit().constData(),
                localMsg.constData());
        break;
    case QtCriticalMsg:
        fprintf(fp, "%s - Critical: %-30s | %-70s : %s\n",
                ts.toString().toLocal8Bit().constData(),
                fileName.fileName().toLocal8Bit().constData(),
                funcLine.toLocal8Bit().constData(),
                localMsg.constData());
        break;
    case QtFatalMsg:
        fprintf(fp, "%s - Fatal   : %-30s | %-70s : %s\n",
                ts.toString().toLocal8Bit().constData(),
                fileName.fileName().toLocal8Bit().constData(),
                funcLine.toLocal8Bit().constData(),
                localMsg.constData());
        break;
    }

    fprintf(stdout, "%s\n", localMsg.constData());
    fflush(stdout);

    fclose(fp);
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(myMessageOutput);
    QApplication a(argc, argv);

    a.setOrganizationName("wunderbar");
    a.setApplicationName("atmelprogrammer");

    MainWindow w;
    w.show();
    return a.exec();
}
