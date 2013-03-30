# #
TEMPLATE = lib
CONFIG += plugin shared
TARGET = sqlexplorer

# VERSION = 2009.09.08.1055
QT += core \
    gui \
    widgets \
    sql

# include config file
include( ../config.pri )
include(../../HHSharedLibs.pri)
INCLUDEPATH += $$UI_DIR \
    $$PWD

# ##
UI_DIR = ./ui
DEPENDPATH += . \
    databaseexplorer \
    dataexplorer \
    sqlconsole
INCLUDEPATH += . \
    databaseexplorer \
    dataexplorer \
    sqlconsole

# Input
HEADERS += sqlexplorerplugin.h \
    sqlexplorer.h \
    databaseexplorer/databaseexplorer.h \
    dataexplorer/dataexplorer.h \
    sqlconsole/sqlconsole.h \
    sqlconsole/textedit.h \
    sqlconsole/highlighter.h
FORMS += sqlexplorer.ui \
    databaseexplorer/databaseexplorer.ui \
    dataexplorer/dataexplorer.ui \
    sqlconsole/sqlconsole.ui
SOURCES += sqlexplorerplugin.cpp \
    sqlexplorer.cpp \
    databaseexplorer/databaseexplorer.cpp \
    dataexplorer/dataexplorer.cpp \
    sqlconsole/sqlconsole.cpp \
    sqlconsole/textedit.cpp \
    sqlconsole/highlighter.cpp
RESOURCES += sqlexplorer.qrc

# define some usefull values
QMAKE_TARGET_PRODUCT = "SQL Tool"
QMAKE_TARGET_DESCRIPTION = "Crossplatform SQL Tool Based On Qt"
