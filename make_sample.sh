# This file creates the spoken word samples to
# be included in the main program.
# Usage:
# make_sample.sh WORD 
# This creates a file named WORD.h containing the WORD spoken
# by the computer.
#
# This needs the MacOS "say" command, or some replacement for it
# on other platforms. SOX and xxd from the VIM package
say $1 -o $1.aiff
sox $1.aiff -b 4 -r 4e3   -b 8  $1.u8 gain -n
sox $1.aiff -r 8e3 -c 1 -b 16  $1.aif gain -n channels 1
xxd -i $1.u8 > $1.h

