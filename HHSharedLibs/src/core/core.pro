include(core.pri)
isEmpty(LIB_NAME):LIB_NAME = HHSharedCore

# ##
TEMPLATE = lib

# CONFIG += static
CONFIG += shared
TARGET = $${LIB_NAME}

# VERSION = 2009.09.08.1055
QT += core \
    sql
QT -= gui

# include config file
include(../buildlib_config.pri)

DEFINES += CORE_LIB_LIBRARY_EXPORT

# #
# DEFINES += MYSHAREDLIB_LIBRARY_EXPORT
# #
# build_all:!build_pass {
# CONFIG -= build_all
# CONFIG += release
# }
# ##
# UI_DIR = ./ui
# OBJECTS_DIR = ./tmp
# MOC_DIR = ./tmp
# RCC_DIR = ./tmp
# Input
HEADERS += logdebug.h \
    cryptography/tea/teacrypt.h \
    cryptography/cryptography.h \
    plugin/abstractplugininterface.h \
    plugin/coreinterface.h \
    plugin/pluginmanager.h \
    plugin/corepluginbase.h \
    userbase.h \
    utilities.h \
    settingscore.h \
    global_core.h \
    database/databaseutility.h \
    mysharedlib_global.h \
    singleton.h \
    user.h \
    error.h
FORMS += 
SOURCES += cryptography/tea/teacrypt.cpp \
    cryptography/cryptography.cpp \
    plugin/pluginmanager.cpp \
    plugin/corepluginbase.cpp \
    userbase.cpp \
    utilities.cpp \
    settingscore.cpp \
    database/databaseutility.cpp \
    singleton.cpp \
    user.cpp \
    error.cpp
RESOURCES += 
win32:LIBS += -Lresources/lib \
    -lwsock32 \
    -lws2_32

# ##
# DESTDIR = $${LIB_PATH}
# DLLDESTDIR = ../../bin
# define config DLLDESTDIR paths
# CONFIG( debug, debug|release ):DLLDESTDIR = $${BIN_PATH}/debug
# else:DLLDESTDIR = $${BIN_PATH}/release
# ##
# unix:target.path += /usr/lib
# win32:target.path += %windir%/system32
target.path += $$[QT_INSTALL_LIBS]
INSTALLS += target

# headers.files += $$HEADERS
# headers.path = $$HHSHAREDLIBS_HEADERS_TARGET_PATH
# INSTALLS += headers
# define some usefull values
# QMAKE_TARGET_COMPANY	= "He Hui Team"
QMAKE_TARGET_PRODUCT = "Core Library"
QMAKE_TARGET_DESCRIPTION = "Crossplatform NON-GUI Library Based On Qt"
