# Tcl/Tk source for a slider controler
#
# Author: Edward A. Lee
# Version: $Id$
#
# Copyright (c) 1993-1995 The Regents of the University of California.
# All rights reserved.
# See the file $PTOLEMY/copyright for copyright notice,
# limitation of liability, and disclaimer of warranty provisions.
#

set s $ptkControlPanel.slider_$starID

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
	pack after $ptkControlPanel.low $s top
    } {
        toplevel $s
        wm title $s "Sliders"
        wm iconname $s "Sliders"
    }
    frame $s.f
}

proc setOut_$starID {position} "
	set val \[expr {[set ${starID}(low)]+([set ${starID}(high)]-\
	[set ${starID}(low)])*\$position/100.0}]
        setOutputs_$starID \$val
        $s.f.m.value configure -text \$val
"

set position [expr \
	{round(100*([set ${starID}(value)]-[set ${starID}(low)])/ \
	([set ${starID}(high)]-[set ${starID}(low)]))}]
ptkMakeScale $s.f m [set ${starID}(identifier)] $position setOut_$starID
$s.f.m.value configure -text $position

if {!$window_previously_existed} {

    pack append $s $s.f top

    proc destructorTcl_$starID {starID} {
	global $starID
	if {[set ${starID}(put_in_control_panel)]} {
	    # Remove the slider from the control panel, if it still exists
	    global ptkControlPanel
	    destroy $ptkControlPanel.slider_$starID
	}
    }
    tkwait visibility $s
}

# Initialize the output
setOutputs_$starID [set ${starID}(value)]

unset s
