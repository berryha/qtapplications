TEMPLATE        = subdirs

SUBDIRS += sqlexplorer \
        usermanager \
        dnslookup

win32{
        SUBDIRS += systeminfo
}
