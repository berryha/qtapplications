include(network.pri)
isEmpty(LIB_NAME):LIB_NAME = HHSharedNetwork

# ##
TEMPLATE = lib

# CONFIG += static
CONFIG += shared
TARGET = $${LIB_NAME}

# VERSION = 2009.09.08.1055
QT += core \
    network \
    concurrent
QT -= gui

# include config file
include( ../buildlib_config.pri )
INCLUDEPATH += . \
    UI_DIR \
    ./udp \
    ./tcp
DEPENDPATH += $${INCLUDEPATH}

DEFINES += NETWORK_LIBRARY_EXPORT

# #
# UI_DIR = ./ui
# OBJECTS_DIR = ./tmp
# MOC_DIR = ./tmp
# RCC_DIR = ./tmp
# Input
HEADERS += \
#    rudp/rudpsocket.h \
#    rudp/rudppacket.h \
#    rudp/rudp_global.h \
#    rudp/rudpchannel.h \
#    rudp/rudppacketstreamoperator.h \
#    packethandler/packetparserbase.h \
#    networkmanagerbase.h \
    packethandler/packetstreamoperator.h \
    udp/udpsocket.h \
    udp/multicast/ipmulticast.h \
    udp/multicast/ipmulticastsocketbase.h \
    packethandler/packet.h \
    packethandler/packethandlerbase.h \
    networkutilities.h \
    global_network.h \
    networklib.h \
    tcp/tcpbase.h

FORMS += 
SOURCES += \
#    rudp/rudpsocket.cpp \
#    rudp/rudppacket.cpp \
#    rudp/rudpchannel.cpp \
#    rudp/rudppacketstreamoperator.cpp \
#    packethandler/packetparserbase.cpp \
#    networkmanagerbase.cpp \
    udp/udpsocket.cpp \
    udp/multicast/ipmulticastsocketbase.cpp \
    packethandler/packet.cpp \
    packethandler/packethandlerbase.cpp \
    networkutilities.cpp \
    packethandler/packetstreamoperator.cpp \
    tcp/tcpbase.cpp

#win32 {
#    HEADERS += udp/multicast/multicastwin.h
#    SOURCES += udp/multicast/multicastwin.cpp
    
#    LIBS += -lws2_32

#    win32-g++{
#        LIBS += -Lresources/lib \
#        -lwsock32
#    }
#}
#unix {
#    HEADERS += udp/multicast/multicastlinux.h
#    SOURCES += udp/multicast/multicastlinux.cpp
#}

RESOURCES +=

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
QMAKE_TARGET_PRODUCT = "Network Library"
QMAKE_TARGET_DESCRIPTION = "Crossplatform Network Library Based On Qt"
