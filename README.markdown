EBU STL tools
=============

A collection of tools to manage EBU STL files

EBU Shift
---------

A simple tool to generate a timecode shifted version of a STL with a CLI.

usage: ebu_shift -i inputstl -t [-]HHMMSSFF outputstl

options:
-i: input file
-t: timecodeshift. Format is (-)HHMMSSFF where HH is hours, MM is minutes, SS is seconds and FF frames. A negative value will delay subtitles as a positive value will display them sooner.


EBU dump
--------

Dump in human readable way most data.

usage: ebu_dump [-f] file.stl

options:
-f: Show full dump.