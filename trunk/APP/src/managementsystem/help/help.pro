# ##
TEMPLATE = app
TARGET = help

# VERSION = 2009.09.08.1055
QT += core \
    gui \
    sql \
    network

# include config file
include( ../config.pri )
HHSharedLibs += HHSharedCore \
    HHSharedGUI \
    HHSharedNetwork \
    HHSharedUDT
win32:HHSharedLibs += HHSharedWindowsManagement
include(../../HHSharedLibs.pri)
HHSharedLibs += HHSharedMS
include(../sharedms/sharedms.pri)

# ##
UI_DIR = ./ui

INCLUDEPATH += $$UI_DIR \
    $$PWD \
    systemsummaryinfo

# Input
HEADERS += networkmanager/bulletinboardpacketsparser.h \
    remoteassistance/remoteassistance.h \
    updatepassword/updatepasswordwidget.h \
    bulletinboard/bulletinboardwidget.h \
    systemsummaryinfo/systemsummaryinfo.h \
    bulletinboardobject.h \
    networkmanager/resourcesmanagerinstance.h \
    mainwindow.h
FORMS += remoteassistance/remoteassistance.ui \
    updatepassword/updatepasswordwidget.ui \
    bulletinboard/bulletinboardwidget.ui \
    systemsummaryinfo/systemsummaryinfo.ui \
    mainwindow.ui
SOURCES += networkmanager/bulletinboardpacketsparser.cpp \
    remoteassistance/remoteassistance.cpp \
    updatepassword/updatepasswordwidget.cpp \
    bulletinboard/bulletinboardwidget.cpp \
    systemsummaryinfo/systemsummaryinfo.cpp \
    bulletinboardobject.cpp \
    networkmanager/resourcesmanagerinstance.cpp \
    mainwindow.cpp \
    main.cpp
RESOURCES += resources.qrc

# win32:RC_FILE = iconresource.rc
win32:APP_ICON = resources/images/app.ico
mac:ICON = ./resources/images/app.icns


# define some usefull values
QMAKE_TARGET_PRODUCT = "Help"
QMAKE_TARGET_DESCRIPTION = "Help For Sitoy Based On Qt"
DEFINES *= "APP_NAME=\"\\\"$${QMAKE_TARGET_PRODUCT}\\\"\""
