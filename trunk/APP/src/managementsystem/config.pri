
# include global  config file
include( ../config_global.pri )


VER_MAJ = 2013
VER_MIN = 2
VER_PAT = 19
VER_CMP = 1

VERSION = $${VER_MAJ}.$${VER_MIN}.$${VER_PAT}.$${VER_CMP}


# define some usefull values
QMAKE_TARGET_COMPANY	= "He Hui"
QMAKE_TARGET_PRODUCT	= "Management System"
QMAKE_TARGET_DESCRIPTION    = "Crossplatform Tools Based On Qt Of LGPL Edition"
QMAKE_TARGET_COPYRIGHT	= "Copyright (C) 2007 - 2014 He Hui"

APP_ORG = "He Hui"
APP_AUTHOR = "He Hui"
APP_AUTHOR_EMAIL = "hehui@hehui.org"
APP_LICENSE = "LGPL"


# define variable for source code
#DEFINES *= "APP_NAME=\"\\\"$${QMAKE_TARGET_PRODUCT}\\\"\"" \
DEFINES	*= "APP_VERSION=\"\\\"$${VERSION}\\\"\"" \
        "APP_ORG=\"\\\"$${APP_ORG}\\\"\"" \
        "APP_ORG_DOMAIN=\"\\\"$${PACKAGE_DOMAIN}\\\"\"" \
        "APP_AUTHOR=\"\\\"$${APP_AUTHOR}\\\"\"" \
        "APP_AUTHOR_EMAIL=\"\\\"$${APP_AUTHOR_EMAIL}\\\"\"" \
        "APP_LICENSE=\"\\\"$${APP_LICENSE}\\\"\"" \
        "APP_COPYRIGHTS=\"\\\"$${QMAKE_TARGET_COPYRIGHT}\\\"\""



#####Release Builed With Debug Info####
#win32-g++:QMAKE_CXXFLAGS_RELEASE += -g
#win32-g++:QMAKE_CFLAGS_RELEASE += -g
#win32-g++:QMAKE_LFLAGS_RELEASE =
#win32-msvc2010:QMAKE_CXXFLAGS_RELEASE += /Zi /Od /DEBUG
#win32-msvc2010:QMAKE_CFLAGS_RELEASE += /Zi /Od /DEBUG
#win32-msvc2010:QMAKE_LFLAGS_RELEASE = /DEBUG
