#-------------------------------------------------
#
# Project created by QtCreator 2013-06-30T18:42:56
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GyroLog
TEMPLATE = app


SOURCES += main.cpp\
        dialog.cpp

HEADERS  += dialog.h

FORMS    += dialog.ui

include(qextserialport/qextserialport.pri)
