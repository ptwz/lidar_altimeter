# This file creates the spoken word samples to
# be included in the main program.
# Usage:
# make_sample.sh WORD 
# This creates a file named WORD.h containing the WORD spoken
# by the computer.
#
# This needs the MacOS "say" command, or some replacement for it
# on other platforms. SOX and python_wizard, my BlueWizard port to python
say $1 -o $1.aiff
sox $1.aiff $1.wav
python_wizard -w 2 -S -p -a -1 -m 0.7 -u .5 -F 25 -f arduino $1.wav > $1.h
