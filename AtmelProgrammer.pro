QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/flashenv.cpp \
    src/flashscript.cpp \
    src/programmeroptions.cpp \
    src/atmelprogrammer.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/optiondialog.cpp \
    src/wundersplit.cpp


HEADERS += \
    src/flashenv.h \
    src/flashscript.h \
    src/help.h \
    src/programmeroptions.h \
    src/atmelprogrammer.h \
    src/mainwindow.h \
    src/optiondialog.h \
    src/wundersplit.h

FORMS += \
    src/mainwindow.ui \
    src/optiondlg.ui

RC_ICONS = images/burn.ico

OTHER_FILES =   qtinstaller/config/config.xml \
                qtinstaller/packages/com.wunderbar.multiprogrammer/meta/package.xml \
                qtinstaller/packages/com.wunderbar.multiprogrammer/meta/installscript.qs \
                qtinstaller/packages/com.wunderbar.multiprogrammer/meta/license.txt \


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
else: target.path = $${PWD}/installer
!isEmpty(target.path): INSTALLS += target
