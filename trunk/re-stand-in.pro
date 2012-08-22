#-------------------------------------------------
#
# Project created by QtCreator 2012-05-07T19:50:16
#
#-------------------------------------------------

QT       += core gui sql network

TARGET = re-stand-in
TEMPLATE = app


SOURCES += main.cpp\
        rsi.cpp \
    settings.cpp \
    parser.cpp \
    log.cpp

HEADERS  += rsi.h \
    standard_settings.hpp

FORMS    += rsi.ui

RESOURCES += resources.qrc
