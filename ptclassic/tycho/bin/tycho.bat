@echo off
rem MSDOS batch script to start up Tycho

rem @author: Christopher Hylands
rem @version: $Id$
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

echo Starting Tycho with $TYCHO = "%tycho%"

if not "%1" == "-java" goto itkw
rem Run Java
rem cd %tycho%\java\tycho
java -classpath %classpath%;%java_home%\lib\classes.zip;%tycho%\java;%tycho%\java\tycho -Denv.PTOLEMY=%tycho% -Denv.TYCHO=/tycho0.2.1devel Tycho
goto end
:itkw

rem Run a vanilla itkwish
itkwish -f %tycho%/kernel/Tycho.tcl %1
:end
