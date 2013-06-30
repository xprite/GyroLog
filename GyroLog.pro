#-------------------------------------------------
#
# Project created by QtCreator 2013-06-30T18:42:56
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = GyroLog
TEMPLATE = app


SOURCES += main.cpp\
        dialog.cpp \
    qcustomplot.cpp

HEADERS  += dialog.h \
    qcustomplot.h

FORMS    += dialog.ui

include(qextserialport/qextserialport.pri)
