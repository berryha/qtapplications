# ##
TEMPLATE = lib
CONFIG += plugin shared
TARGET = dnslookup

QT       += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


# include config file
include( ../config.pri )
HHSharedLibs += HHSharedCore \
    HHSharedGUI
include(../../HHSharedLibs.pri)


# #
INCLUDEPATH += $$UI_DIR \
    $$PWD

# ##
UI_DIR = ./ui


SOURCES += dnslookup.cpp \
    dnslookupplugin.cpp

HEADERS  += \
    dnslookup.h \
    dnslookupplugin.h

FORMS    += \
    dnslookup.ui

RESOURCES += resource.qrc

# define some usefull values
QMAKE_TARGET_PRODUCT = "DNS Lookup"
QMAKE_TARGET_DESCRIPTION = "Crossplatform DNS Lookup Based On Qt"

