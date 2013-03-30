# ##
TEMPLATE = lib
CONFIG += plugin shared
TARGET = bulletinboard

# VERSION = 2009.09.08.1055
QT += core \
    gui \
    widgets \
    sql \
    network \
    concurrent

# include config file
include( ../../config.pri )
HHSharedLibs += HHSharedCore \
    HHSharedGUI \
    HHSharedNetwork \
    HHSharedUDT
win32:HHSharedLibs += HHSharedWindowsManagement
include(../../../HHSharedLibs.pri)
HHSharedLibs += HHSharedMS
include(../../sharedms/sharedms.pri)
# ##
UI_DIR = ./ui
INCLUDEPATH += $$UI_DIR \
    $$PWD



# Input
HEADERS += \
    networkmanager/bulletinboardpacketsparser.h \
    remoteassistance/remoteassistance.h \
    updatepassword/updatepasswordwidget.h \
    bulletinboard/bulletinboardwidget.h \
    bulletinboardplugin.h \
    systemsummaryinfo.h \
    bulletinboardobject.h \
    networkmanager/resourcesmanagerinstance.h
FORMS += remoteassistance/remoteassistance.ui \
    updatepassword/updatepasswordwidget.ui \
    bulletinboard/bulletinboardwidget.ui \
    systemsummaryinfo.ui
SOURCES += \
    networkmanager/bulletinboardpacketsparser.cpp \
    remoteassistance/remoteassistance.cpp \
    updatepassword/updatepasswordwidget.cpp \
    bulletinboard/bulletinboardwidget.cpp \
    bulletinboardplugin.cpp \
    systemsummaryinfo.cpp \
    bulletinboardobject.cpp \
    networkmanager/resourcesmanagerinstance.cpp
RESOURCES += bulletinboard.qrc

# define some usefull values
QMAKE_TARGET_PRODUCT = "Bulletin Board"
QMAKE_TARGET_DESCRIPTION = "Bulletin Board For Sitoy Based On Qt"
DEFINES *= "APP_NAME=\"\\\"$${QMAKE_TARGET_PRODUCT}\\\"\""
