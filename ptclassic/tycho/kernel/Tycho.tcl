# Master initialization file for the Tycho tcl/tk Ptolemy interface.
# Sourcing this file will open one or more tycho editors, depending
# on the command-line arguments.  If no command-line arguments are given,
# then a console window is opened.
#
# Author: Edward A. Lee
# Version: $Id$
#
# Copyright (c) 1990-%Q% The Regents of the University of California.
# All rights reserved.
#
# Permission is hereby granted, without written agreement and without
# license or royalty fees, to use, copy, modify, and distribute this
# software and its documentation for any purpose, provided that the above
# copyright notice and the following two paragraphs appear in all copies
# of this software.
#
# IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY
# FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
# ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
# THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
#
# THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
# INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
# PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
# CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
# ENHANCEMENTS, OR MODIFICATIONS.
#                                                         COPYRIGHTENDKEY

# We don't use the toplevel window called ".", so withdraw it.
wm withdraw .

########################################################################
# If the environment variable TYCHO is set, use that to determine
# where the tycho libraries are.  Otherwise, use PTOLEMY.  If neither
# is set, try to find a user named ptolemy.  If all this fails, issue
# an error message.
#
# FIXME:  We should include in the startup message information about
# where the version of tycho that we are running is installed.
#
global env tk_library tk_version
global ptolemy PTOLEMY
global tycho TYCHO

if [info exist env(TYCHO)] {
    set tycho $env(TYCHO)
    set TYCHO $env(TYCHO)
}

if [info exist env(PTOLEMY)] {
    set ptolemy $env(PTOLEMY)
    set PTOLEMY $env(PTOLEMY)
    if {![info exists tycho]} {
	set tycho $ptolemy/tycho
	set TYCHO $ptolemy/tycho
    }
}

if {![info exists tycho]} {
    # Neither environment variable is set.
    # See whether there is a ~ptolemy or ~ptdesign user, in that order,
    # that has tycho installed.
    if [file exists [glob ~ptolemy/tycho]] {
	set tycho [glob ~ptolemy/tycho]
	set TYCHO $tycho
    } {
	if [file exists [glob ~ptdesign/tycho]] {
	    set tycho [glob ~ptdesign/tycho]
	    set TYCHO $tycho
	}
    }
}

if {![info exists tycho]} {
    # All of the above failed, give up
    error {To run tycho, set your environment variable TYCHO to the tycho home}
}

# If the ptolemy variables are not yet set, set them relative to tycho.
if {![info exists ptolemy]} {
    set ptolemy $tycho/..
    set PTOLEMY $ptolemy
}

global tychokernel
set tychokernel $tycho/kernel
if {![info exists tychoRegExit]} {
    set tychoRegExit 1
}

# Note that it is NOT normally acceptable to rely on the TCL_LIBRARY
# and TK_LIBRARY environment variables, because most other tcl
# apps rely on them.  We have to set them here.  However, if Tycho
# is distributed without Ptolemy, we don't know where the libraries
# are, so we have no choice but to rely on the previously set values
# of these variables.  In this case, we assume that we are running
# under itkwish, and therefore the initialization files have already
# been sourced.

if [file isdirectory $PTOLEMY/tcltk/itcl/lib] {
    set env(TCL_LIBRARY) $PTOLEMY/tcltk/itcl/lib/tcl
    set env(TK_LIBRARY) $PTOLEMY/tcltk/itcl/lib/tk
    set tk_library $env(TK_LIBRARY)
    
    uplevel #0 {
	source $PTOLEMY/tcltk/itcl/lib/tcl/init.tcl
	source $PTOLEMY/tcltk/itcl/lib/tk/tk.tcl
	source $PTOLEMY/tcltk/itcl/lib/itcl/init.itcl
	source $PTOLEMY/tcltk/itcl/lib/itk/init.itk
    }
}
uplevel #0 {
    source $tychokernel/Lib.tcl
}

if {![info exists tychoWelcomeWindow]} {
    # If tychoWelcomeWindow is 0, then no 'Mr. Tycho' window is opened
    set tychoWelcomeWindow 1
}
if {![info exists tychoConsoleWindow]} {
    # If tychoWelcomeWindow is 0, then no console window is opened.
    set tychoConsoleWindow 1
}
set tychoOpenFiles 0

# Source ~/.tycho if it exists.  ~/.tycho contains user modifications.
set tychostartfile [glob -nocomplain ~/.tycho]
if {$tychostartfile != {} && \
	[file exists $tychostartfile] && \
	[file readable $tychostartfile]} {
    uplevel #0 {source $tychostartfile}
}
unset tychostartfile

# Check this in case we are being sourced within a interpreter where
# argv has been unset.
if {[info exists argv]} {
    foreach file $argv {
	if {$file == {-nowelcome}} {
	    set tychoWelcomeWindow 0
	} elseif {$file == {-noconsole}} {
	    set tychoConsoleWindow 0
	} else {
	    set tychoOpenFiles 1
	    File::openContext $file
	}
    }
}

# Retrieve binary and version info. If the two variables already exist,
# then we must be in a tysh binary we they are set by SetVersionInfo
# which is in TyConsole.cc. Otherwise we are being called from itkwish,
# or another script. 
if {![info exists TychoBinaryInfo]} {
    set TychoBinaryInfo itkwish
}
if {![info exists TychoVersionInfo]} {
    set TychoVersionInfo "Version 0.6"
}
# To disable the welcome message, set the global variable
# tychoWelcomeMessage to zero before sourcing this file.
# This can be done in your .tycho file.
# FIXME: This should be implemented as a command-line option
# rather than a global variable.
#
if {$tychoWelcomeWindow != 0} {
    ::tycho::welcomeMessage $TychoBinaryInfo $TychoVersionInfo
}
# If tychoRegExit is 0, then disable exiting when the last window is
# killed.
if {$tychoRegExit == 0} {
    ::tycho::TopLevel::exitWhenNoMoreWindows 0
}
# If there are no command-line arguments, and the -noconsole
# option was not given, open a console window
if {$tychoOpenFiles == 0} {
    if {$tychoConsoleWindow != 0} {
	uplevel #0 {::tycho::Console .mainConsole \
		-master 1 -text "Welcome to Tycho\n" -geometry +0+0}
	wm deiconify .mainConsole
    }
}

unset tychoWelcomeWindow
unset tychoConsoleWindow
unset tychoOpenFiles

