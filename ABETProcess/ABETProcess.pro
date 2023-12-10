QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += ../ABETProcessor/
INCLUDEPATH += ../../QXlsx/QXlsx/header/

SOURCES += \
    ../ABETProcessor/ABETProcessor.cpp \
    ../ABETProcessor/main.cpp \
    FilePushButton.cpp \
    formcreatespreadsheets.cpp \
    mainwindow.cpp \
    pidata.cpp

HEADERS += \
    ../ABETProcessor/ABETProcessor.h \
    FilePushButton.h \
    formcreatespreadsheets.h \
    mainwindow.h \
    pidata.h

FORMS += \
    ../ABETProcessor/ABETProcessor.ui \
    formcreatespreadsheets.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    ../ABETProcessor/ABETProcessor.qrc

linux {
    #sudo apt-get install libblas-dev liblapack-dev
    greaterThan(QT_MAJOR_VERSION, 5): LIBS += /home/arash/Projects/QXlsx/libQXlsx6.a
    lessThan(QT_MAJOR_VERSION, 6): LIBS += /home/arash/Projects/QXlsx/libQXlsx.a
}
