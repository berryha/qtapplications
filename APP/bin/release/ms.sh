#!/bin/sh

makeAbsolute() {
    case "$1" in
	/*)
	    # already absolute, return it
	    echo "$1"
	    ;;
	*)
	    # relative, prepend $2 made absolute
	    echo `makeAbsolute "$2" "$PWD"`/"$1" | sed 's,/\.$,,'
	    ;;
    esac
}

if test -L "$0"; then
    # Try readlink(1)
    readlink=`type readlink 2>/dev/null` || readlink=
    if test -n "$readlink"; then
	# We have readlink(1), so we can use it
	me=`readlink -nf "$0"`
    else
	# No readlink(1), so let's try ls -l
	me=`ls -l "$0" | sed 's/^.*-> //'`
	base=`dirname "$0"`
	me=`makeAbsolute "$me" "$base"`
    fi
else
    me="$0"
fi

bindir=`dirname "$me"`
libdir=`cd "${bindir}/../../lib" ; pwd`

set APP_HOME=/opt/workspace/qt/ms
LD_LIBRARY_PATH="${libdir}:${LD_LIBRARY_PATH}"
#LD_LIBRARY_PATH="${APP_HOME}/shared/lib:${APP_HOME}/messenger/lib:${APP_HOME}/managementsystem/lib:${LD_LIBRARY_PATH}"
export LD_LIBRARY_PATH
exec "${bindir}/app" ${1+"$@"}