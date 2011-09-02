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
include(../../HHSharedLibs.pri)

# Input
HEADERS += groupbase.h \
    constants_global_shared.h \
    imuserbase.h \
    networkmanager/networkmanager.h
SOURCES += groupbase.cpp \
    imuserbase.cpp \
    networkmanager/networkmanager.cpp
FORMS += 
RESOURCES += resources.qrc

# ##
unix:target.path += /usr/lib
win32:target.path += %windir%/system32
INSTALLS += target

# define some usefull values
# QMAKE_TARGET_COMPANY	= "He Hui Team"
QMAKE_TARGET_PRODUCT = "Shared Library For IM"
QMAKE_TARGET_DESCRIPTION = "Crossplatform Shared Library For IM Based On Qt"
