#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setOrganizationName("wunderbar");
    a.setApplicationName("atmelprogrammer");

    MainWindow w;
    w.show();
    return a.exec();
}
