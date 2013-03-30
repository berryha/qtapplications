# ##
TEMPLATE = lib
CONFIG += plugin shared
TARGET = systeminfo

# VERSION = 2009.09.08.1055
QT += core \
    gui \
    widgets \
    sql

# include config file
include( ../config.pri )
include(../../HHSharedLibs.pri)

INCLUDEPATH += $$UI_DIR \
    $$PWD



# ##
UI_DIR = ./ui

# Input
HEADERS += \
    systeminfoplugin.h \
    systeminfo.h
FORMS += systeminfo.ui
SOURCES += systeminfoplugin.cpp \
    systeminfo.cpp
RESOURCES += systeminfo.qrc

win32 {
    LIBS += -luser32 \
        -lNetAPI32

    win32-g++{
        INCLUDEPATH += resources/lib/WinAPI_GCC
        LIBS += -Lresources/lib/WinAPI_GCC
    }

}


#unix:target.path += ../../bin/plugins/hehui
#win32:target.path += %windir%/system32/plugins/hehui
#INSTALLS += target

# define some usefull values
QMAKE_TARGET_PRODUCT = "System Information Collector"
QMAKE_TARGET_DESCRIPTION = "Crossplatform System Information Collector Based On Qt"

