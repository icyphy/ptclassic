# Tcl/Tk source for a panel of buttons
#
# Author: Edward A. Lee
# Version: $Id$
#
# Copyright (c) 1994 The Regents of the University of California.
# All rights reserved.
# See the file $PTOLEMY/copyright for copyright notice,
# limitation of liability, and disclaimer of warranty provisions.
#

set s $ptkControlPanel.buttons_$starID

# If a window with the right name already exists, we assume it was
# created by a previous run of the very same star, and hence can be
# used for this new run.  Some trickiness occurs, however, because
# parameter values may have changed, including the number of inputs.

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

# If allow_simultaneous_events is TRUE, then we do nothing at the time this is called.
if {[set ${starID}(allow_simultaneous_events)]} {
    proc setOut_$starID {num} {}
} {
    proc setOut_$starID {num} "setOutput_$starID \$num [set ${starID}(value)]"
}

proc ptkButtonsClearButtons {win numOut starID val} {
    for {set i 1} {$i <= $numOut} {incr i} {
	if {[lindex [$win.b.b$i configure -relief] 4] == {sunken}} {
	    setOutput_$starID $i $val
	    $win.b.b$i configure -relief raised
	}
    }
}

for {set i 1} {$i <= [set ${starID}(numOutputs)]} {incr i} {
    # Don't use ptkMakeButton here because I want more compact spacing
    # Also, customize the binding to the button
    set but $s.b.b$i
    catch {destroy $but}
    button $but -text [lindex [set ${starID}(identifiers)] [expr $i-1]] \
	    -command "setOut_$starID $i"
    pack $but -fill x -expand yes -anchor e
    bind $but <ButtonPress-1> "$but configure -relief sunken; $but invoke"

    if {[set ${starID}(allow_simultaneous_events)]} {
	# Do nothing.  In particular, leave the button sunken
	bind $but <ButtonRelease-1> ""
    } {
	bind $but <ButtonRelease-1> "$but configure -relief raised"
    }
}
if {[set ${starID}(allow_simultaneous_events)]} {
    # Create a button that causes output events to be produced
    # It is put in a frame with class Attention to get a different color.
    catch {destroy $s.b.sync}
    frame $s.b.sync -class Attention
    ptkMakeButton $s.b.sync b "PUSH TO PRODUCE EVENTS" \
	    "ptkButtonsClearButtons $s [set ${starID}(numOutputs)] $starID [set ${starID}(value)]"
    pack $s.b.sync -fill x
}

if {!$window_previously_existed} {

    pack $s.b -fill x

    proc destructorTcl_$starID {starID} {
	global $starID
	if {[set ${starID}(put_in_control_panel)]} {
	    # Remove the buttons from the control panel, if they still exist
	    global ptkControlPanel
	    destroy $ptkControlPanel.buttons_$starID
	}
    }

    tkwait visibility $s
}

unset s

