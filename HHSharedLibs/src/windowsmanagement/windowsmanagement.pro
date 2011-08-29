
include(windowsmanagement.pri)

isEmpty(LIB_NAME){
    LIB_NAME = HHSharedWindowsManagement
}

# ##
TEMPLATE = lib

# CONFIG += static
CONFIG += shared
TARGET = $${LIB_NAME}

# VERSION = 2009.09.08.1055
QT += core

# include config file
include( ../buildlib_config.pri )


# #
UI_DIR = ./ui

# OBJECTS_DIR = ./tmp
# MOC_DIR = ./tmp
# RCC_DIR = ./tmp
# Input
HEADERS += mysharedlib_global.h \
    windowsmanagement.h
FORMS += 
SOURCES += windowsmanagement.cpp
RESOURCES += 
win32 { 
    INCLUDEPATH += resources/lib/WinAPI \
        resources/lib/AutoIt3
    LIBS += -Lresources/lib/WinAPI \
        -lNetAPI32 \
        -lMpr \
        -lWinspool \
        -lVersion \
        -Lresources/lib/AutoIt3 \
        -lautoitx3
}



# ##
#DESTDIR = $${LIB_PATH}

# DLLDESTDIR = ../../bin
# define config DLLDESTDIR paths
#CONFIG( debug, debug|release ):DLLDESTDIR = $${BIN_PATH}/debug
#else:DLLDESTDIR = $${BIN_PATH}/release



# ##
#unix:target.path += /usr/lib
#win32:target.path += %windir%/system32
target.path += $$[QT_INSTALL_LIBS]
INSTALLS += target

#headers.files += $$HEADERS
#headers.path = $$HHSHAREDLIBS_HEADERS_TARGET_PATH
#INSTALLS += headers


# define some usefull values
#QMAKE_TARGET_COMPANY	= "He Hui Team"
QMAKE_TARGET_PRODUCT	= "M$ Windows Management Library"
QMAKE_TARGET_DESCRIPTION	= "Crossplatform M$ Windows Management Library Based On Qt"
#QMAKE_TARGET_COPYRIGHT	= "Copyright (C) 2007 - 2010 He Hui"

