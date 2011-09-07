
TEMPLATE = subdirs

CONFIG += ordered \
    debug_and_release

SUBDIRS += managementsystem \
    plugins \
    #messenger \
    test

TRANSLATIONS += ts_zh_CN.ts \
    ts_zh_TW.ts
