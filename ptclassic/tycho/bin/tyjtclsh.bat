@echo off
rem MSDOS batch script Tcl Blend under NT

rem @author: Christopher Hylands
rem @version: @(#)jtclsh.bat	1.1 11/24/97
rem @copyright: Copyright (c) 1997 The Regents of the University of California.
rem All rights reserved.
rem See the file $TYCHO/copyright for copyright notice,
rem limitation of liability, and disclaimer of warranty provisions.

rem Check the TYCHO variable.
if not "%tycho%" == "" set TYCHO=c:\tycho0.2.1devel

rem Make sure that we can find the parts of Tycho we need
if exist %tycho%\kernel\Tycho.tcl goto tychoexists
echo %tycho\kernel\Tycho.tcl does not exist! exiting.
exit
:tychoexists
set CLASSPATH=%tycho%
echo Starting jtclsh with $TYCHO = "%tycho%" 
echo 1 "%1" 2 "%2" 3 "%3"
tclsh80 %tycho%/lib/windows/jtclsh.tcl %1 %2 %3 %4 %5
