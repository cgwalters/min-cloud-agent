#!/bin/sh

test -n "$srcdir" || srcdir=`dirname "$0"`
test -n "$srcdir" || srcdir=.

olddir=`pwd`
cd $srcdir

function missing_tool() {
        echo "*** No $1 found, please install it ***"
        exit 1
}

# This function takes the name of a binary to search for in the system's path
# as its sole argument. In the event the binary is not found, it will raise
# a human-parseable error using missing_tool() and exit 1.
function check_binary_in_path() {
        TOOL=$1

        CHECK_TOOL=`which $TOOL`
        if test -z $CHECK_TOOL; then
                missing_tool $TOOL
        fi
}

check_binary_in_path "autoreconf"
check_binary_in_path "libtool"

mkdir -p m4

autoreconf --force --install --verbose

cd $olddir
test -n "$NOCONFIGURE" || "$srcdir/configure" "$@"
