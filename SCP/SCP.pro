#-------------------------------------------------
#
# Project created by QtCreator 2016-05-31T11:00:13
#
#-------------------------------------------------

QT       += core gui sql
QT       += network
QT       += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SCP
TEMPLATE = app


SOURCES += main.cpp\
        siviso.cpp \
    blanco.cpp \
    ppi.cpp \
    signal.cpp \
    dbasepostgresql.cpp \
    wconfig.cpp

HEADERS  += siviso.h \
    blanco.h \
    ppi.h \
    signal.h \
    dbasepostgresql.h \
    wconfig.h

FORMS    += siviso.ui \
    wconfig.ui

RESOURCES += \
    resource.qrc

DISTFILES +=
