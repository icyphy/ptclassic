# Tcl/Tk source TkBreakPt, a star that pauses when its input values meet
#			   the user settable condition
#
# Author: Alan Kamas
# Version: $Date$ $Id$
#
# Copyright (c) 1995 The Regents of the University of California.
# All rights reserved.
# See the file $PTOLEMY/copyright for copyright notice,
# limitation of liability, and disclaimer of warranty provisions.
#

# set s $ptkControlPanel.buttons_$starID
# If a window with the right name already exists, we assume it was
# created by a previous run of the very same star, and hence can be
# used for this new run. 

# if {! [winfo exists $s] } {
#	frame $s
#        pack $s -after $ptkControlPanel.high -fill x -padx 10
#
#	frame $s.b
#	message $s.msg -width 12c
#        pack $s.msg -expand 1
#	$s.msg configure -text 	"Clear Break Point $starID"
##
#
#

proc goTcl_$starID {starID} {
	global ptkControlPanel $starID
	set inputList [grabInputs_$starID]
	# put the inputs into the form expected by the condition statement
	set j 0
	foreach i $inputList {
    	    set input([incr j]) $i
	}

	if { [expr [set ${starID}(condition)] ] } {
#	 	ptkHighlight [$starID]
		$ptkControlPanel.panel.pause invoke 
	}
}
