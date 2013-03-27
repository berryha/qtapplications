TEMPLATE        = subdirs

SUBDIRS += sqlexplorer \
        usermanager

win32{
        SUBDIRS += systeminfo
}
