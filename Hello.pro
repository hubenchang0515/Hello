#-------------------------------------------------
#
# Project created by QtCreator 2016-12-29T22:51:36
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Hello
TEMPLATE = app
CONFIG += c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    csyntaxhighlighter.cpp \
    newprojectdialog.cpp \
    buildthread.cpp \
    helpdialog.cpp

HEADERS  += mainwindow.h \
    csyntaxhighlighter.h \
    newprojectdialog.h \
    buildthread.h \
    helpdialog.h

FORMS    += mainwindow.ui

RESOURCES += \
    res.qrc
