
TEMPLATE = subdirs

CONFIG += ordered \
    debug_and_release

SUBDIRS += core \
           network \
           udt \
           gui \
           service

win32{
	SUBDIRS += windowsmanagement
}


