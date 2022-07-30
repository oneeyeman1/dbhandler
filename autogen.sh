#!/bin/sh
#
# Keeping autogen.sh in case some IDE insists on calling it. Otherwise
# you can just run autoreconf and remove autogen.sh.

cd "$(dirname "$0")"
autoreconf -if
