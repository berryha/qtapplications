include(server.pri)
isEmpty(LIB_NAME):LIB_NAME = HHSharedIMServer

# ##
TEMPLATE = lib

# CONFIG += static
CONFIG += shared
TARGET = $${LIB_NAME}

# ##
# TEMPLATE = lib
# CONFIG += shared
# TARGET = HHSharedServer
QT += core \
    sql \
    network \
    xml
QT -= gui

# include config file
include( ../config.pri )
include(../../HHSharedLibs.pri)
HHSharedLibs += HHSharedIM
include (../sharedim/sharedim.pri)

# #
UI_DIR = ./ui

# ##
DEPENDPATH += . \
    about \
    debug \
    mainwindow
INCLUDEPATH += . \
    mainwindow \
    about

# Input
HEADERS += \
    serverutilities.h \
    settings.h \
    server.h \
    app_constants_server.h \
    packetmanager/serverpacketsparser.h \
    usersmanager/usersmanager.h \
    networkmanager/servernetworkmanager.h \
    usersmanager/userinfo.h \
    usersmanager/group.h
SOURCES += \
    serverutilities.cpp \
    settings.cpp \
    server.cpp \
    packetmanager/serverpacketsparser.cpp \
    usersmanager/usersmanager.cpp \
    networkmanager/servernetworkmanager.cpp \
    usersmanager/userinfo.cpp \
    usersmanager/group.cpp
FORMS += 
RESOURCES += resources.qrc
win32 { 
    # DLLDESTDIR = $${HHSHAREDLIBS_BIN_TARGET_PATH}
    DLLDESTDIR = $$[QT_INSTALL_BINS]
    QMAKE_DISTCLEAN += $$[QT_INSTALL_BINS]\\$${TARGET}.dll
}

# ##
unix:target.path += /usr/lib
win32:target.path += %windir%/system32
INSTALLS += target

# define some usefull values
QMAKE_TARGET_PRODUCT = "Server Library For IM System"
QMAKE_TARGET_DESCRIPTION = "Crossplatform Server Library For IM System Based On Qt"