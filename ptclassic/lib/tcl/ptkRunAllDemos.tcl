# Definition of the control panel for the run-all-demos command
# Author: Wan-teh Chang
# Version: $Id$
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


#######################################################################
# Procedure to bring up the control panel for run-all-demos
#
proc ptkRunAllDemos {name octHandle} {
    global ptkControlPanel
    set ptkControlPanel .run_all_demos_$octHandle
    
    if {[winfo exists $ptkControlPanel]} {
        ptkImportantMessage .error \
	    "You already have a run-all-demos control window open for $name"
	raise $ptkControlPanel
	return
    }

    catch {destroy $ptkControlPanel}
    # Sets the Class of the Window.  This is used to set all options
    # for widgets used in the Control window
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
    # frame is $ptkControlPanel.$position, where ptkControlPanel is
    # a global variable available to the stars, and $position is "high",
    # "middle", or "low".
    frame $ptkControlPanel.high
    frame $ptkControlPanel.middle
    frame $ptkControlPanel.low

    # Define the panel of control buttons
    frame $ptkControlPanel.panel -bd 10
    button $ptkControlPanel.panel.abortAll -text "Abort all" \
	-command {ptkImportantMessage .error "Abort all: not implemented"} \
	-width 14
    button $ptkControlPanel.panel.abortCurrent -text "Abort current" \
	-width 14
    pack $ptkControlPanel.panel.abortAll -side left -fill both -expand yes
    pack $ptkControlPanel.panel.abortCurrent -side right \
	-fill both -expand yes

    # Event loop is on by default
    ptkSetEventLoop on

    # Animation is off by default
    ptkGrAnimation 0

    ptkClearHighlights

    frame $ptkControlPanel.disfr
    # DISMISS button does nothing for now, no command binding.
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
	"ptkRunAllDemosDel $name $octHandle"
}


#######################################################################
# Procedure to delete a run-all-demos control window
#
proc ptkRunAllDemosDel { name octHandle } {
    # destroy the window
    catch {destroy .run_all_demos_$octHandle}
}


#######################################################################
# Procedure to delete a universe and its baggage
#
proc ptkDelLite { name octHandle } {
    global ptkControlPanel ptkOctHandles ptkRunFlag

    if {![info exists ptkRunFlag($name)]} {
	# Assume the universe has been deleted already and ignore command
	return
    }
    # Assertion: universe should have stopped already.
    if ![regexp {^FINISHED$|^ERROR$} $ptkRunFlag($name)] {
	puts stderr "Universe \"$name\" hasn't stopped already when we try to delete it.\nSomething went wrong.  Please report this bug."
	# Still go on and delete the universe.
    }
    unset ptkRunFlag($name)
    unset ptkOctHandles($name)
    $ptkControlPanel.panel.abortCurrent configure -command ""
    deluniverse $name
}

#######################################################################
# Procedure to compile and run a universe by run-all-demos.
# A blend of ptkRunControl (everything without the control panel part)
# and ptkGo.  No debugging, animation.
proc ptkCompileRun {name octHandle} {
    global ptkRunFlag ptkOctHandles ptkControlPanel

    # For now, we allow only one run at a time.
    set univ [curuniverse]
    if {[info exists ptkRunFlag($univ)] && \
	([regexp {^ACTIVE$|^STOP_PENDING$|^ABORT$} $ptkRunFlag($univ)] || \
	($univ != $name && $ptkRunFlag($univ) == {PAUSED})) } {
            ptkImportantMessage .error "Sorry.  Only one run at time. "
	    return
    }

    set ptkOctHandles($name) $octHandle
    set ptkRunFlag($name) IDLE

    # So that error highlighting, etc. works
    if {$univ != $name} {ptkSetHighlightFacet $octHandle}

    # catch errors and reset the run flag.
    if {[catch {
	$ptkControlPanel.univName configure -text "Compiling universe $name"
	$ptkControlPanel.iter configure -text ""
	update idletasks
        ptkCompile $octHandle

	set ptkRunFlag($name) ACTIVE
	$ptkControlPanel.univName configure -text "Running universe $name"
	$ptkControlPanel.panel.abortCurrent configure \
	    -command "ptkAbort $name"
	if {[ptkGetStringProp $octHandle usescript] == "1"} {
	    # we want to run the script.  This means we have to
	    # retrieve the script from the oct facet, and then get
	    # the Tcl interpreter to evaluate it.
	    $ptkControlPanel.iter configure -text "A scripted run"
	    set scriptValue [ptkGetStringProp $octHandle script]
	    if {$scriptValue == ""} {
		set numIter [ptkGetRunLength $octHandle]
		set scriptValue "run $defNumIter\nwrapup\n"
	    }
	    eval $scriptValue
	} {
	    # default run: just run through the specified number
	    # of iterations, finishing by invoking
	    # wrapup if no error occurred.
	    set numIter [ptkGetRunLength $octHandle]
	    $ptkControlPanel.iter configure -text "$numIter iterations"
            run $numIter

	    if {[info exists ptkRunFlag($name)] &&
		$ptkRunFlag($name) != {ABORT}} { wrapup } {
	        # Mark an error if the system was aborted
	        set ptkRunFlag($name) ERROR
	    }
	}

	# we have finished running
	set ptkRunFlag($name) FINISHED
    } msg] == 1} {
	# An error has occurred.
	# Mark an error
	set ptkRunFlag($name) ERROR
	error $msg
    }
}
