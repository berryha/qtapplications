# ##
TARGET = test
TEMPLATE = app
QT += core \
    gui \
    sql \
    network

# include config file
include( ../config_global.pri )

HHSharedLibs += HHSharedGUI \
    HHSharedCore \
    HHSharedNetwork \
    HHSharedUDT

include(../HHSharedLibs.pri)


VER_MAJ = 2010
VER_MIN = 6
VER_PAT = 26
VER_CMP = 1
VERSION = $${VER_MAJ}.$${VER_MIN}.$${VER_PAT}.$${VER_CMP}

# #
INCLUDEPATH += $$UI_DIR \
    $$PWD

# ####
UI_DIR = ./ui

# ###
DEPENDPATH += . \
    shared \
    about \
    mainwindow
INCLUDEPATH += . \
    shared \
    mainwindow \
    about

# Input
HEADERS += rudp/clientpacketsparser.h \
    screenshot/selecttargetimagewidget.h \
    screenshot/screenshot.h \
    rudp/rudp.h \
    shared/global_app.h \
    about/aboutdialog.h \
    mainwindow/mainwindow.h \
    shared/settings.h \
    udt/udtsocket.h \
    udt/udt.h \
    tcp/tcp.h \
    tcp/tcpwidget.h
SOURCES += rudp/clientpacketsparser.cpp \
    screenshot/selecttargetimagewidget.cpp \
    screenshot/screenshot.cpp \
    rudp/rudp.cpp \
    shared/settings.cpp \
    main.cpp \
    about/aboutdialog.cpp \
    mainwindow/mainwindow.cpp \
    udt/udtsocket.cpp \
    udt/udt.cpp \
    tcp/tcp.cpp \
    tcp/tcpwidget.cpp
FORMS += screenshot/selecttargetimagewidget.ui \
    screenshot/screenshot.ui \
    rudp/rudp.ui \
    about/aboutdialog.ui \
    mainwindow/mainwindow.ui \
    udt/udt.ui \
    tcp/tcpwidget.ui
RESOURCES += resources.qrc
win32:RC_FILE = iconresource.rc
mac:ICON = ./resources/images/app.icns

win32 {
    #DEFINES += WIN32 __MINGW__ _WIN32_WINNT=0x0501
    DEFINES += WIN32 _WIN32_WINNT=0x0501
    win32-g++:DEFINES += __MINGW__

    HEADERS +=
    SOURCES +=

    #LIBS += -lws2_32

}
