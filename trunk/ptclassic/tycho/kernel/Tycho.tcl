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
	set tycho $ptolemy
	set TYCHO $ptolemy
    }
}

if {![info exists tycho]} {
    # Neither environment variable is set.
    # See whether there is a ~ptolemy or ~ptdesign user, in that order,
    # that has tycho installed.
    if [file exists [glob ~ptolemy/tycho]] {
	set tycho [glob ~ptolemy]
	set TYCHO $tycho
    } {
	if [file exists [glob ~ptdesign/tycho]] {
	    set tycho [glob ~ptdesign]
	    set TYCHO $tycho
	}
    }
}

if {![info exists tycho]} {
    # All of the above failed, give up
    error {To run tycho, set your environment variable TYCHO to the tycho home}
}

global tychokernel
set tychokernel $tycho/tycho/kernel

# Note that it is NOT acceptable to rely on the TCL_LIBRARY
# and TK_LIBRARY environment variables, because most other tcl
# apps rely on them.  We have to set them here.

set env(TCL_LIBRARY) $TYCHO/tcltk/itcl-2.0b2/lib/tcl7.4
set env(TK_LIBRARY) $TYCHO/tcltk/itcl-2.0b2/lib/tk4.0
set tk_library $env(TK_LIBRARY)

uplevel #0 {
    source $TYCHO/tcltk/itcl-2.0b2/lib/tcl7.4/init.tcl
    source $TYCHO/tcltk/itcl-2.0b2/lib/tk4.0/tk.tcl
    source $TYCHO/tcltk/itcl-2.0b2/lib/itcl2.0/init.itcl
    source $TYCHO/tcltk/itcl-2.0b2/lib/itk2.0/init.itk
}

source $tychokernel/Lib.tcl

# FIXME: The following should be handled by auto-loading.
uplevel #0 {
    source $TYCHO/tycho/html_library/html_library.tcl
}

# FIXME: The following should be optional, depending on command-line args.
::tycho::Console .mainConsole -text "Welcome to Tycho\n" -geometry +0+0
wm deiconify .mainConsole
