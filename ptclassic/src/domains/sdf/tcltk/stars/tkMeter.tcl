# Tcl/Tk source for a bar meter display
#
# Author: Edward A. Lee
# Version: $Id$
#
# Copyright (c) 1993 The Regents of the University of California.
# All rights reserved.
# See the file $PTOLEMY/copyright for copyright notice,
# limitation of liability, and disclaimer of warranty provisions.
#

set s $ptkControlPanel.meter_$starID

# If a window with the right name already exists, we assume it was
# created by a previous run of the very same star, and hence can be
# used for this new run.

if {![winfo exists $s]} {

    if {[set ${starID}(put_in_control_panel)]} {
	frame $s
	pack after $ptkControlPanel.low $s top
    } {
        toplevel $s
        wm title $s "Bar Meters"
        wm iconname $s "Bar Meters"
    }

    frame $s.f
    message $s.msg -width 12c -text [set ${starID}(label)]
    for {set i 0} {$i < [set ${starID}(numInputs)]} {incr i} {
    	ptkMakeMeter $s.f m$i "" [set ${starID}(low)] [set ${starID}(high)]
    }
    pack append $s $s.msg {top expand} $s.f top

    if {![set ${starID}(put_in_control_panel)]} {
        button $s.ok -text "DISMISS" -command \
		"ptkStop [curuniverse]; destroy $s"
        pack append $s $s.ok {top fillx}
    }

    proc tkMeterSetValues {starID numInputs win} {
        set c $win.f
        set inputVals [grabInputs_$starID]
	for {set i 0} {$i < $numInputs} {incr i} {
            set in [lindex $inputVals $i]
            ptkSetMeter $win.f m$i $in
	}
    }

    proc goTcl_$starID {starID} "
        tkMeterSetValues $starID [set ${starID}(numInputs)] $s
    "

    proc destructorTcl_$starID {starID} {
	global $starID
	if {[set ${starID}(put_in_control_panel)]} {
	    # Remove the meters from the control panel, if they still exist
	    global ptkControlPanel
	    destroy $ptkControlPanel.meter_$starID
	}
    }
}
unset s
