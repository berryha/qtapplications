include(udt.pri)
isEmpty(LIB_NAME):LIB_NAME = HHSharedUDT

# ##
TEMPLATE = lib

# CONFIG += static
CONFIG += shared
TARGET = $${LIB_NAME}

# VERSION = 2009.09.08.1055
QT += core \
    network
QT -= gui

# include config file
include( ../buildlib_config.pri )
#UDT
include( ./udt/build_with_udt.pri )


INCLUDEPATH += . \
    UI_DIR \
    ./udt \
    ./udt/src
DEPENDPATH += $${INCLUDEPATH}

# #
# UI_DIR = ./ui
# OBJECTS_DIR = ./tmp
# MOC_DIR = ./tmp
# RCC_DIR = ./tmp
# Input
HEADERS += \
            mysharedlib_global.h \
    udtprotocolbase.h \
    udtprotocolforfiletransmission.h
SOURCES += \
    udtprotocolbase.cpp \
    udtprotocolforfiletransmission.cpp
RESOURCES += 


win32 { 
    #DEFINES += WIN32 __MINGW__ _WIN32_WINNT=0x0501
    DEFINES += WIN32 _WIN32_WINNT=0x0501
    win32-g++:DEFINES += __MINGW__

    HEADERS +=
    SOURCES += 

    LIBS += -lws2_32

    #LIBS += -Lresources/lib/win \
    #    #-lwsock32 \
    #    -lws2_32 \
    #    -ludt

}
unix { 
    HEADERS += 
    SOURCES += 
    #LIBS += -Lresources/lib/linux \
    #    -ludt
}

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
QMAKE_TARGET_COMPANY = "UDT"
QMAKE_TARGET_PRODUCT = "UDT4"
QMAKE_TARGET_DESCRIPTION = "UDP-based data transfer protocol"
QMAKE_TARGET_COPYRIGHT = "Copyright (C) 2001 - 2011 Yunhong Gu. All rights reserved."
PACKAGE_DOMAIN = "http://udt.sf.net"




