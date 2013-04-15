include(gui.pri)
isEmpty(LIB_NAME):LIB_NAME = HHSharedGUI

# ##
TEMPLATE = lib

# CONFIG += static
CONFIG += shared
TARGET = $${LIB_NAME}

# VERSION = 2009.09.08.1055
# ##
QT += core \
    gui \
    widgets \
    printsupport \
    sql \
    xml

# include config file
include( ../buildlib_config.pri )
HHSharedLibs += HHSharedCore
include(../core/core.pri)

include (./widgetbase/itembox/itembox.pri)

DEFINES += GUI_LIBRARY_EXPORT

# DEPENDPATH += . \
# UI_DIR \
# ../core
# INCLUDEPATH += . \
# UI_DIR \
# ../core
# ########## Link shared libs #############
# SHARED_LIB_FOLDER = ../lib
# SHARED_CORE_FOLDER = ../core
# include($$SHARED_CORE_FOLDER/core.pri)
# !include($$SHARED_CORE_FOLDER/core.pri):error( "Can not include $$SHARED_CORE_FOLDER/core.pri !" )
# LIBS += -L$$SHARED_LIB_FOLDER -lHHSharedCore
# define link libs
# LIBS += -L$$SHARED_LIB_FOLDER

CONFIG( debug, debug|release ):

# Debug
# unix:LIBS += -lHHSharedCore_debug
# win32:LIBS += -lHHSharedCored$${VER_MAJ}
# mac:LIBS += -lHHSharedCored
else:
# Release
# win32:LIBS += -lHHSharedCore$${VER_MAJ}
# else:LIBS += -lHHSharedCore
# ## Export the library
# DEFINES += MYSHAREDLIB_LIBRARY_EXPORT
# #
# build_all:!build_pass {
# CONFIG -= build_all
# CONFIG += release
# }

# ##
UI_DIR = ./ui

# OBJECTS_DIR = ./tmp
# MOC_DIR = ./tmp
# RCC_DIR = ./tmp
# Input
HEADERS += screenshot/screenshot.h \
    screenshot/selecttargetimagewidget.h \
    dataexport/dataoutputdialog.h \
    preference/preferenceswindow.h \
    plugin/pluginmanager/pluginmanagerwindow.h \
    plugin/pluginmanager/plugininfomodel.h \
    plugin/guiinterface.h \
    progress/progressdlg.h \
    widgetbase/systemtrayiconbase.h \
    imageresourcebase.h \
    widgetbase/expandlistview/categorylistview.h \
    widgetbase/expandlistview/expandlistview.h \
    widgetbase/expandlistview/expandlistviewmanager.h \
    widgetbase/expandlistview/shared/abstractexpandlistviewInterface.h \
    widgetbase/expandlistview/shared/expandlistviewbase.h \
    widgetbase/expandlistview/treewidget.h \
    plugin/guipluginbase.h \
    login/loginbase.h \
    settingsbase.h \
    databaseconnecter/databaseconnecter.h \
    databaseconnecter/databaseconnecterdialog.h \
    login/logindlg.h \
    dataprint.h \
    widgetbase/mainwindowbase.h \
    widgetbase/widgetbase.h \
    guilib.h \
    widgetbase/expandlistview/shared/filterwidget.h \
    widgetbase/expandlistview/shared/sheet_delegate.h \
    widgetbase/expandlistview/shared/iconloader.h
FORMS += screenshot/screenshot.ui \
    screenshot/selecttargetimagewidget.ui \
    dataexport/dataoutputdialog.ui \
    preference/preferenceswindow.ui \
    plugin/pluginmanager/pluginmanagerwindow.ui \
    progress/progressdlg.ui \
    databaseconnecter/databaseconnecterdialog.ui \
    login/logindlg.ui
SOURCES += screenshot/screenshot.cpp \
    screenshot/selecttargetimagewidget.cpp \
    dataexport/dataoutputdialog.cpp \
    preference/preferenceswindow.cpp \
    plugin/pluginmanager/pluginmanagerwindow.cpp \
    plugin/pluginmanager/plugininfomodel.cpp \
    progress/progressdlg.cpp \
    widgetbase/systemtrayiconbase.cpp \
    imageresourcebase.cpp \
    widgetbase/expandlistview/categorylistview.cpp \
    widgetbase/expandlistview/expandlistview.cpp \
    widgetbase/expandlistview/expandlistviewmanager.cpp \
    widgetbase/expandlistview/shared/abstractexpandlistviewInterface.cpp \
    widgetbase/expandlistview/shared/expandlistviewbase.cpp \
    widgetbase/expandlistview/shared/filterwidget.cpp \
    widgetbase/expandlistview/shared/iconloader.cpp \
    widgetbase/expandlistview/shared/sheet_delegate.cpp \
    widgetbase/expandlistview/treewidget.cpp \
    plugin/guipluginbase.cpp \
    login/loginbase.cpp \
    settingsbase.cpp \
    databaseconnecter/databaseconnecter.cpp \
    databaseconnecter/databaseconnecterdialog.cpp \
    login/logindlg.cpp \
    dataprint.cpp \
    widgetbase/mainwindowbase.cpp \
    widgetbase/widgetbase.cpp

RESOURCES += widgetbase/expandlistview/expandlistview.qrc \
            gui.qrc
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
QMAKE_TARGET_PRODUCT = "GUI Library"
QMAKE_TARGET_DESCRIPTION = "Crossplatform GUI Library Based On Qt"
