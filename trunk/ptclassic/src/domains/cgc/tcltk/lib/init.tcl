# Tcl/Tk source to include in CGC applications running under
# the CGCTclTkTarget.
#
# EXPERIMENTAL: DO NOT USE!!!!
#
# Currently assumes custom controls -- needs to be fixed
# to support non-custom controls as well.
#
# Author: John Reekie
# Version: $Id$
#
# Copyright (c) %Q% The Regents of the University of California.
# All rights reserved.
# 
# Permission is hereby granted, without written agreement and without
# license or royalty fees, to use, copy, modify, and distribute this
# software and its documentation for any purpose, provided that the
# above copyright notice and the following two paragraphs appear in all
# copies of this software.
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
# 
# 						PT_COPYRIGHT_VERSION_2
# 						COPYRIGHTENDKEY
# See the file ~ptolemy/copyright for copyright notice,
# limitation of liability, and disclaimer of warranty provisions.
#

if { ![info exists env(PTOLEMY)] } {
    set env(PTOLEMY) [glob ~ptolemy]
}
set ptolemy $env(PTOLEMY)
set PTOLEMY $env(PTOLEMY)

# Start Tycho running
#
# FIXME: We really only want some small parts of Tycho.
#
proc ptkStartTycho { } {
    # If the TYCHO global variable does not exist, then tycho
    # has not been loaded.
    uplevel #0 {
	if {![info exists TYCHO]} {
	    # Disable the standard welcomes.
	    set tychoConsoleWindow 0
	    set tychoWelcomeWindow 0
	    set tychoExitWhenNoMoreWindows 0
	    set tychoShouldWeDoRegularExit 0
	    if [info exists env(TYCHO)] {
		set path $env(TYCHO)
	    } else {
		set path $ptolemy/tycho
	    }
	    source $path/kernel/Tycho.tcl
	}
    }
}

ptkStartTycho
wm deiconify .


# Connect to a control in the custom control panel
#
proc connectControl {starname ctrlname callback} {
    global customControlPanel
    $customControlPanel connect $starname $ctrlname $callback
}



# Source the files that define custom control panels
#
# FIXME: Use autoloading
#
#source $ptolemy/src/domains/cgc/tcltk/lib/custom.itcl
