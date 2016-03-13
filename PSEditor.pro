#-------------------------------------------------
#
# Project created by QtCreator 2016-03-03T12:13:38
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PSEditor
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    dlgNewItem.cpp \
    dlgabout.cpp

HEADERS  += mainwindow.h \
    dlgNewItem.h \
    dlgabout.h

FORMS    += mainwindow.ui \
    dlgNewItem.ui \
    dlgabout.ui

RESOURCES += \
    res.qrc
