#!/bin/sh

set -e

rm -rf autom4te.cache
rm -f aclocal.m4 ltmain.sh

test -n "$srcdir" || srcdir=`dirname "$0"`
test -n "$srcdir" || srcdir=.
(
  cd "$srcdir" &&
  (
      autoreconf -vif
  )
)

if [ -z "$NOCONFIGURE" ]; then
  exec "$srcdir"/configure -C "$@"
fi

