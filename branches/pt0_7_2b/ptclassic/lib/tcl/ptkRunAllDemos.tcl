# Definition of the control panel for the run-all-demos command
# Author: Wan-teh Chang
# Version: $Id$
#
# Copyright (c) 1990-1993 The Regents of the University of California.
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


#######################################################################
# Procedure to bring up the control panel for run-all-demos
#
proc ptkRunAllDemos { name octHandle } {
    global ptkControlPanel ptkOctHandles
    set ptkOctHandles($name) $octHandle
    set ptkControlPanel .run_all_demos_$octHandle
    
    if {[winfo exists $ptkControlPanel]} {
            ptkImportantMessage .error \
		"You already have a run control window open for $name"
	    return
    }

    catch {destroy $ptkControlPanel}
    # Sets the Class of the Window.  This is used to set all options
    #   for widgets used in the Contol window
    toplevel $ptkControlPanel -class PigiControl
    wm title $ptkControlPanel "Run-all-demos $name"
    wm iconname $ptkControlPanel "Run-all-demos $name"

    message $ptkControlPanel.msg \
	    -width 25c \
	    -text "Run-all-demos control panel\nfor palette $name" \
	    -justify center

    # Display info on the current universe
    label $ptkControlPanel.univName -width 40

    # Display the number of iterations
    label $ptkControlPanel.iter -width 40

    # The following empty frames are created so that they are available
    # to stars to insert custom controls into the control panel.
    # By convention, we tend to use "high" for entries, "middle"
    # for buttons, and "low" for sliders.  The full name for the
    # frame is .run_${octHandle}.$position, where $name is the name of the
    # universe, and $position is "high", "middle", or "low".
    frame $ptkControlPanel.high
    frame $ptkControlPanel.middle
    frame $ptkControlPanel.low

    # Define the panel of control buttons
    frame $ptkControlPanel.panel -bd 10
	button $ptkControlPanel.panel.abortAll -text "Abort all" \
		-command {ptkImportantMessage .error "Abort all: not implemented"} \
		-width 14
	button $ptkControlPanel.panel.abortCurrent -text "Abort current" \
		-command {ptkAbort [curuniverse]} -width 14
	pack $ptkControlPanel.panel.abortAll -side left -fill both -expand yes
	pack $ptkControlPanel.panel.abortCurrent -side right \
		-fill both -expand yes

    # Animation is off by default
    ptkGrAnimation 0

    frame $ptkControlPanel.disfr
    # DISMISS button does nothing for now, no command.
    button $ptkControlPanel.disfr.dismiss -text "DISMISS (not implemented)"
    pack $ptkControlPanel.disfr.dismiss -side top -fill both -expand yes

    pack $ptkControlPanel.msg -fill both -expand yes
    pack $ptkControlPanel.univName
    pack $ptkControlPanel.iter
    pack $ptkControlPanel.panel
    pack $ptkControlPanel.high -fill x -padx 10 -expand yes
    pack $ptkControlPanel.middle -fill x -padx 10 -expand yes
    pack $ptkControlPanel.low -fill x -padx 10 -expand yes
    pack $ptkControlPanel.disfr -fill both -expand yes

    wm geometry $ptkControlPanel +400+400
    focus $ptkControlPanel
    wm protocol $ptkControlPanel WM_DELETE_WINDOW \
	"ptkRunAllDemosDel $name $ptkControlPanel $octHandle"
}


#######################################################################
# Procedure to delete a run-all-demos control window
#
proc ptkRunAllDemosDel { name window octHandle } {
    catch {unset ptkOctHandles($name)}

    # most important thing: destroy the window
    catch {destroy $window}
}


#######################################################################
# Procedure to delete a universe and its baggage
#
proc ptkDelLite { name octHandle } {
    global ptkRunFlag

    if {![info exists ptkRunFlag($name)]} {
	# Assume the window has been deleted already and ignore command
	return
    }
    if [regexp {^ACTIVE$|^PAUSED$} $ptkRunFlag($name)] {
	ptkStop $name 
	update
    }
    if [regexp {^STOP_PENDING$|^ABORT$} $ptkRunFlag($name)] {
	# If the universe hasn't stopped already, try again later
	after 200 ptkDelLite $name $octHandle
	return
    }
    catch {unset ptkRunFlag($name)}
    catch {unset ptkOctHandles($name)}
    deluniverse $name
}

#######################################################################
#procedure to compile and run a universe by run-all-demos
#a blend of ptkRunControl (everything without the control panel part)
#and ptkGo.  No debugging, animation.
proc ptkCompileRun {name octHandle} {
    global ptkRunFlag ptkOctHandles
    set ptkOctHandles($name) $octHandle

    set ptkRunFlag($name) IDLE

    # For now, we allow only one run at a time.
    set univ [curuniverse]
    if {[info exists ptkRunFlag($univ)] && \
	([regexp {^ACTIVE$|^STOP_PENDING$|^ABORT$} $ptkRunFlag($univ)] || \
	($univ != $name && $ptkRunFlag($univ) == {PAUSED})) } {
            ptkImportantMessage .error "Sorry.  Only one run at time. "
	    return
    }
    global ptkControlPanel
    after 200 ptkUpdateCount $name $octHandle
    # So that error highlighting, etc. works
    if {$univ != $name} {ptkSetHighlightFacet $octHandle}
    # Make sure the button relief gets displayed
    update
    # catch errors and reset the run flag.
    if {[catch {
	$ptkControlPanel.univName configure -text "Compiling universe $name"
	$ptkControlPanel.iter configure -text ""
	update
        ptkCompile $octHandle
	set ptkRunFlag($name) ACTIVE
	$ptkControlPanel.univName configure -text "Running universe $name"
	set numIter [ptkGetRunLength $octHandle]
	$ptkControlPanel.iter configure -text "$numIter iterations"
        run $numIter
	if {$ptkRunFlag($name) != {ABORT}} { wrapup } {
	    # Mark an error if the system was aborted
	    set ptkRunFlag($name) ERROR
	}
	ptkClearRunFlag $name $octHandle
    } msg] == 1} {
	# An error has occurred.
	ptkClearRunFlag $name $octHandle
	# Mark an error
	set ptkRunFlag($name) ERROR
	error $msg
    }
}