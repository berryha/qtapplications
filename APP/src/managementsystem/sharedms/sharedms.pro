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
include(../../HHSharedLibs.pri)

# Input
HEADERS += clientinfo.h \
    global_shared.h \
    networkmanager.h
FORMS += 
SOURCES += clientinfo.cpp \
    networkmanager.cpp
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
# QMAKE_TARGET_COMPANY	= "He Hui Team"
QMAKE_TARGET_PRODUCT = "Shared Library For Management System"
QMAKE_TARGET_DESCRIPTION = "Crossplatform Library Based On Qt"
