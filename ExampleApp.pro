#-------------------------------------------------
#
# Project created by QtCreator 2011-12-07T10:42:07
#
#-------------------------------------------------


QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ExampleApp
TEMPLATE = app

# Investigar esto
### VERSION = $$system(git --algo)

SOURCES += main.cpp\
        mainwindow.cpp \
    conndialog.cpp \
    loginmodel.cpp \
    pwchangedialog.cpp \
    dbutil.cpp \
    strutil.cpp \
    dbinfodialog.cpp \
    roleadmindialog.cpp \
    paramsdialog.cpp \
    testwidget.cpp

HEADERS  += mainwindow.h \
    conndialog.h \
    loginmodel.h \
    pwchangedialog.h \
    dbutil.h \
    strutil.h \
    dbinfodialog.h \
    roleadmindialog.h \
    paramsdialog.h \
    testwidget.h

CODECFORTR = UTF-8

TRANSLATIONS += \
    ts/ExampleApp_es.ts

RESOURCES += \
    resources.qrc

win32 {
RC_FILE = win32.rc
}

OTHER_FILES += \
    win32.rc \
    TODO.txt





















