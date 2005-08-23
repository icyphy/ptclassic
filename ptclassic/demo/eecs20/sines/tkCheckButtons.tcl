# Tcl/Tk source for a panel of buttons
#
# Authors: Edward A. Lee and Wan-Teh Chang
# Version: %W%	%G%
#
# Copyright (c) 1990-%Q% The Regents of the University of California.
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
# See the file $PTOLEMY/copyright for copyright notice,
# limitation of liability, and disclaimer of warranty provisions.
#

set s $ptkControlPanel.buttons_$starID

# If a window with the right name already exists, we assume it was
# created by a previous run of the very same star, and hence can be
# used for this new run.  Some trickiness occurs, however, because
# parameter values may have changed, including the number of outputs.

if {[winfo exists $s]} {
    set window_previously_existed 1
} {
    set window_previously_existed 0
}

if {!$window_previously_existed} {
    frame $s
    pack $s -after $ptkControlPanel.high -fill x -padx 10
    frame $s.b
}

# Create the individual event buttons.
for {set i 1} {$i <= [set ${starID}(numOutputs)]} {incr i} {
    # Don't use ptkMakeButton here because I want more compact spacing.
    set but $s.b.b$i
    catch {destroy $but}
    checkbutton $but -text $i -variable button_$i$starID
    pack $but -fill x -expand yes -anchor e
}

if {!$window_previously_existed} {
    pack $s.b -fill x
    tkwait visibility $s
}

proc goTcl_$starID {starID} {
    global $starID
    for {set i 1} {$i <= [set ${starID}(numOutputs)]} {incr i} {
	global button_$i$starID
	lappend outs [set button_$i$starID]
    }
    eval setOutputs_$starID $outs
}

# Remove the buttons
proc destructorTcl_$starID {starID} "
    destroy $s
"

unset s window_previously_existed
