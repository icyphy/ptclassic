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
		# Highlight myself
	 	ptkHighlight [set ${starID}(fullName)] 
		# Put explanation in the control window
		set s $ptkControlPanel
		set s.brpt $s.breakpoint_$starID
		if { ! [winfo exists $s.brpt] } {
		    # make overall, text, and entry frames
		    frame $s.brpt
		    frame $s.brpt.e
		    label $s.brpt.m1 -text \
"Break Point:"  
		    label $s.brpt.m2 -anchor w -text \
"The highlighted star has hit a conditional breakpoint."  
		    label $s.brpt.m3 -anchor w -text \
"You may optionally change the condition by editing the" 
		    label $s.brpt.m4 -anchor w -text \
"line below and pressing return."
		    label $s.brpt.m5 -anchor w -text \
"To clear this breakpoint change the condition to 0 (zero)"  
		    label $s.brpt.m6 -anchor w -text \
"Press the GO button to continue."
		    ptkMakeEntry $s.brpt.e cond Condition \
                                 "[set ${starID}(condition)]" \
"$ptkControlPanel.panel.gofr.go invoke; set ${starID}(condition) "
		    pack append $s.brpt $s.brpt.m1 {fillx top} \
		                        $s.brpt.m2 {fillx top} \
		                        $s.brpt.m3 {fillx top} \
		                        $s.brpt.m4 {fillx top} \
		                        $s.brpt.m5 {fillx top} \
		                        $s.brpt.m6 {fillx top} \
		    			$s.brpt.e {bottom} 
		    pack after $s.low $s.brpt {top frame s}
		    # make sure messages are displayed before the pause
		    update
		}
		$ptkControlPanel.panel.pause invoke 
		ptkClearHighlights
                catch {pack unpack $s.brpt}
		catch {destroy $s.brpt}
	}
}
