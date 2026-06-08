#! /bin/bash
# by Hans Schou
set -e

# Dump GIF file to C array for Ardino IDE.
#

if [[ -z $1 ]]
then
	echo Error: no file name arg on command line
	exit 1
fi

echo "const uint8_t image[] PROGMEM = { //"
od -v -An -t x1 $1 | sed -e 's/\([0-9a-f]\+\)/0x\1,/g' -e 's|$| //|'
echo "};"
