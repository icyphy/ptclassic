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

proc goTcl_$starID {starID} {
	global ptkControlPanel $starID

	# put the inputs into the form expected by the condition statement
	set inputList [grabInputs_$starID]
	set j 0
	foreach i $inputList {
    	    set input([incr j]) $i
	}

	if { [expr [set ${starID}(condition)] ] } {
	    # Break point has been reached
	    # implement the action defined in the break point script
	    set script [set ${starID}(Optional_Alternate_Script)] 
	    if { ([string length $script] > 0) } {
		eval $script
	    } else {
		# No readable script passed:  Default Script: Pause the run
		# Highlight myself
	 	ptkHighlight [set ${starID}(fullName)] 
		# Put explanation in the control window
		set s $ptkControlPanel
		set s.brpt $s.breakpoint_$starID
		if { ! [winfo exists $s.brpt] } {
		    # make overall, text, and entry frames
		    frame $s.brpt
		    frame $s.brpt.e
		    set messtext "Break Point:"  
		    # labels are used to get a matching font to control panel
		    label $s.brpt.m1 -text $messtext
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
		                $s.brpt.m2 {fillx top} $s.brpt.m3 {fillx top} \
		                $s.brpt.m4 {fillx top} $s.brpt.m5 {fillx top} \
		                $s.brpt.m6 {fillx top} $s.brpt.e {bottom} 
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
}
