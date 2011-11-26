# ##
TEMPLATE = lib
CONFIG += plugin
TARGET = controlcenter

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
HEADERS += \
    networkmanager/controlcenterpacketsparser.h \
    taskmanagement/taskinfoviewwidget.h \
    taskmanagement/taskmanagement.h \
    announcement/announcement.h \
    clientinfomodel/clientinfomodel.h \
    systemmanagement/systemmanagementwidget.h \
    controlcenter.h \
    controlcenterplugin.h \
    constants.h \
    networkmanager/resourcesmanagerinstance.h
FORMS += taskmanagement/taskinfoviewwidget.ui \
    taskmanagement/taskmanagement.ui \
    announcement/announcement.ui \
    systemmanagement/systemmanagementwidget.ui \
    controlcenter.ui
SOURCES += \
    networkmanager/controlcenterpacketsparser.cpp \
    taskmanagement/taskinfoviewwidget.cpp \
    taskmanagement/taskmanagement.cpp \
    announcement/announcement.cpp \
    clientinfomodel/clientinfomodel.cpp \
    systemmanagement/systemmanagementwidget.cpp \
    controlcenter.cpp \
    controlcenterplugin.cpp \
    networkmanager/resourcesmanagerinstance.cpp
RESOURCES += controlcenter.qrc

# define some usefull values
QMAKE_TARGET_PRODUCT = "Control Center"
QMAKE_TARGET_DESCRIPTION = "Crossplatform Control Center For Sitoy Based On Qt"

