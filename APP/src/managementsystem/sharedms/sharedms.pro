include(sharedms.pri)
isEmpty(LIB_NAME):LIB_NAME = HHSharedMS

# ##
TEMPLATE = lib

# CONFIG += static
CONFIG += shared
TARGET = $${LIB_NAME}

# VERSION = 2009.09.08.1055
QT += core \
    sql \
    network

# include config file
include(../config.pri)

HHSharedLibs += HHSharedCore \
    HHSharedNetwork \
    HHSharedUDT
include(../../HHSharedLibs.pri)

# Input
HEADERS += clientinfo.h \
    global_shared.h \
    udtprotocol.h \
    resourcesmanager.h \
    filemanager.h \
    sharedmslib.h \
    tcpserver.h \
    rtp.h
FORMS += 
SOURCES += clientinfo.cpp \
    udtprotocol.cpp \
    resourcesmanager.cpp \
    filemanager.cpp \
    tcpserver.cpp \
    rtp.cpp
RESOURCES += resources.qrc

DEFINES += SHAREDMSLIB_LIBRARY_EXPORT

win32 { 
    #DEFINES += WIN32 __MINGW__ _WIN32_WINNT=0x0501
    DEFINES += WIN32 _WIN32_WINNT=0x0501
    #DEFINES += WIN32 LEGACY_WIN32 _WIN32_WINNT=0x0500
    win32-g++:DEFINES += __MINGW__

    HEADERS +=
    SOURCES +=
    #LIBS += -lws2_32


    # DLLDESTDIR = $${HHSHAREDLIBS_BIN_TARGET_PATH}
    DLLDESTDIR = $$[QT_INSTALL_BINS]
    QMAKE_DISTCLEAN += $$[QT_INSTALL_BINS]\\$${TARGET}.dll
}



# ##
unix:target.path += /usr/lib
win32:target.path += %windir%/system32
INSTALLS += target

# define some usefull values
# QMAKE_TARGET_COMPANY	= "He Hui Team"
QMAKE_TARGET_PRODUCT = "Shared Library For Management System"
QMAKE_TARGET_DESCRIPTION = "Crossplatform Library Based On Qt"
