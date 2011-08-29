# ##
TEMPLATE = lib
CONFIG += plugin
TARGET = bulletinboard

# VERSION = 2009.09.08.1055
QT += core \
    gui \
    sql \
    network

# include config file
include( ../../config.pri )
HHSharedLibs += HHSharedCore \
    HHSharedGUI \
    HHSharedNetwork
win32:HHSharedLibs += HHSharedWindowsManagement
include(../../../HHSharedLibs.pri)
HHSharedLibs += HHSharedMS
include(../../sharedms/sharedms.pri)
INCLUDEPATH += $$UI_DIR \
    $$PWD

# ##
UI_DIR = ./ui

# Input
HEADERS += networkmanager/networkmanagerinstance.h \
    networkmanager/bulletinboardpacketsparser.h \
    remoteassistance/remoteassistance.h \
    updatepassword/updatepasswordwidget.h \
    bulletinboard/bulletinboardwidget.h \
    bulletinboardplugin.h \
    systemsummaryinfo.h \
    bulletinboardobject.h
FORMS += remoteassistance/remoteassistance.ui \
    updatepassword/updatepasswordwidget.ui \
    bulletinboard/bulletinboardwidget.ui \
    systemsummaryinfo.ui
SOURCES += networkmanager/networkmanagerinstance.cpp \
    networkmanager/bulletinboardpacketsparser.cpp \
    remoteassistance/remoteassistance.cpp \
    updatepassword/updatepasswordwidget.cpp \
    bulletinboard/bulletinboardwidget.cpp \
    bulletinboardplugin.cpp \
    systemsummaryinfo.cpp \
    bulletinboardobject.cpp
RESOURCES += bulletinboard.qrc

# define some usefull values
QMAKE_TARGET_PRODUCT = "Bulletin Board"
QMAKE_TARGET_DESCRIPTION = "Bulletin Board For Sitoy Based On Qt"
