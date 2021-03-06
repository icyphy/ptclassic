# Tcl/Tk source for a panel of buttons
#
# Authors: Edward A. Lee and Wan-Teh Chang
# Version: @(#)tkButtons.tcl	1.3    4/18/95
#
# Copyright (c) 1990-1995 The Regents of the University of California.
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
    if {[set ${starID}(put_in_control_panel)]} {
	frame $s
	pack $s -after $ptkControlPanel.high -fill x -padx 10
    } {
        toplevel $s
        wm title $s "Buttons"
        wm iconname $s "Buttons"
    }
    frame $s.b
    if {[set ${starID}(label)] != {}} {
	message $s.msg -width 12c
	pack $s.msg -expand 1
    }
}
if {[set ${starID}(label)] != {}} {
    $s.msg configure -text [set ${starID}(label)]
}

# Create the individual event buttons.
for {set i 1} {$i <= [set ${starID}(numOutputs)]} {incr i} {
    # Don't use ptkMakeButton here because I want more compact spacing.
    set but $s.b.b$i
    catch {destroy $but}
    button $but -text [lindex [set ${starID}(identifiers)] [expr $i-1]]
    pack $but -fill x -expand yes -anchor e

    if {[set ${starID}(allow_simultaneous_events)]} {
	# If allow_simultaneous_events is TRUE, then we do nothing
	# at the time an event button is pressed.  In particular,
	# leave the pressed button sunken.
	$but configure -command "$but configure -relief sunken"
    } {
	$but configure -command "setOutput_$starID $i [set ${starID}(value)]"
    }
}


# If allow_simultaneous_events is TRUE, then this is called when the
# "PUSH TO PRODUCE EVENTS" button is pressed.
proc tkButtonsClearButtons_$starID {win numOut starID val} {
    for {set i 1} {$i <= $numOut} {incr i} {
	if {[lindex [$win.b.b$i configure -relief] 4] == {sunken}} {
	    setOutput_$starID $i $val
	    $win.b.b$i configure -relief raised
	}
    }
}
if {[set ${starID}(allow_simultaneous_events)]} {
    # Create a button that causes output events to be produced.
    # It is put in a frame with class Attention to get a different color.
    catch {destroy $s.b.sync}
    frame $s.b.sync -class Attention
    ptkMakeButton $s.b.sync b "PUSH TO PRODUCE EVENTS" \
	    "tkButtonsClearButtons_$starID $s [set ${starID}(numOutputs)] \
	    $starID [set ${starID}(value)]"
    pack $s.b.sync -fill x
}

if {!$window_previously_existed} {

    pack $s.b -fill x

    proc destructorTcl_$starID {starID} "
	# Remove the buttons
	destroy $s

	rename tkButtonsClearButtons_$starID \{\}
    "

    tkwait visibility $s
}

unset s window_previously_existed
