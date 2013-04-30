include(sharedim.pri)
isEmpty(LIB_NAME):LIB_NAME = HHSharedIM

# ##
TEMPLATE = lib

# CONFIG += static
CONFIG += shared
TARGET = $${LIB_NAME}
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
HEADERS += groupbase.h \
    constants_global_shared.h \
    imuserbase.h \
    sharedimlib.h \
    udtprotocol.h \
    tcpserver.h \
    rtp.h \
    resourcesmanager.h \
    filemanager.h
SOURCES += groupbase.cpp \
    imuserbase.cpp \
    udtprotocol.cpp \
    tcpserver.cpp \
    rtp.cpp \
    resourcesmanager.cpp \
    filemanager.cpp
FORMS += 
RESOURCES += resources.qrc

DEFINES += SHAREDIMLIB_LIBRARY_EXPORT

# ##
unix:target.path += /usr/lib
win32:target.path += %windir%/system32
INSTALLS += target

# define some usefull values
# QMAKE_TARGET_COMPANY	= "He Hui Team"
QMAKE_TARGET_PRODUCT = "Shared Library For IM"
QMAKE_TARGET_DESCRIPTION = "Crossplatform Shared Library For IM Based On Qt"