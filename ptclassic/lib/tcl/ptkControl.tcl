# Definition of a bunch of control panels for the tcl/tk Ptolemy interface
# Author: Edward A. Lee
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
#
# The global array ptkRunFlag($name), indexed by the name of the universe,
# has the following values:
#	undefined		no active run window, universe not created
#	IDLE			active run window, but no run yet
#	ACTIVE			active run
#	PAUSED			paused run
#	FINISHED		finished a run
#	STOP_PENDING		pending stop requested
#	ABORT			pending abort requested

# Need a dummy value in ptkRunFlag(main) for startup
set ptkRunFlag(main) IDLE

# The global variable ptkControlPanel is the name of the master control
# panel window that is currently being used for a run.  This name is therefore
# available to stars that wish to insert frames into the control panel.
# It is equal to .run_$octHandle.

#######################################################################
# Set the ptkRunFlag to IDLE, indicating the run is over, and pop up the
# GO button.
#
proc ptkClearRunFlag { name octHandle } {
    global ptkRunFlag
    set  cntrWindow .run_$octHandle
    catch {$cntrWindow.panel.go configure -relief raised}
    catch {$cntrWindow.panel.pause configure -relief raised}
    if {[info exists ptkStepAction($name)]} {
	cancelAction $ptkStepAction($name)
	unset ptkStepAction($name)
    }
    set ptkRunFlag($name) FINISHED
    return ""
}

#######################################################################
# Return "1" if the system has has been run at least once, "0" otherwise.
# This procedure is used by pigilib/compile.c.
#
proc ptkHasRun { name } {
    global ptkRunFlag
    if {![info exists ptkRunFlag($name)] || $ptkRunFlag($name) == {IDLE}} {
        return 0
    } { return 1 }
}


#######################################################################
# Procedure to run a universe.
#
proc ptkRunControl { name octHandle } {
    global ptkRunFlag
    global ptkDebug
    global ptkControlPanel
    set ptkDebug($name) 0
    set ptkControlPanel .run_$octHandle
    if {[info exists ptkRunFlag($name)] && [winfo exists $ptkControlPanel]} {
            ptkImportantMessage .error \
		"Sorry.  Only one run window for $name at time. "
	    return
    }
    # Mark an open window, but with no run yet.
    set ptkRunFlag($name) IDLE

    catch {destroy $ptkControlPanel}
    toplevel $ptkControlPanel
    wm title $ptkControlPanel "Run $name"
    wm iconname $ptkControlPanel "Run $name"

    set defNumIter [ptkGetRunLength $octHandle]

    message $ptkControlPanel.msg \
	    -font -Adobe-times-medium-r-normal--*-180* -width 25c \
            -text "Control panel for $name" -justify center

    # Define the entry that controls the number of iterations
    frame $ptkControlPanel.iter -bd 10
	label $ptkControlPanel.iter.label -text "Number of iterations:"
        entry $ptkControlPanel.iter.entry -relief sunken -width 10 -bg wheat3
	$ptkControlPanel.iter.entry insert @0 $defNumIter
	checkbutton $ptkControlPanel.iter.debug -text "Debug" \
	    -variable ptkDebug($name) -relief flat \
	    -command "ptkSetOrClearDebug $name $octHandle"
	pack append $ptkControlPanel.iter \
	    $ptkControlPanel.iter.label left \
	    $ptkControlPanel.iter.entry left \
	    $ptkControlPanel.iter.debug {right padx 20}

    # The following empty frames are created so that they are available
    # to stars to insert custom controls into the control panel.
    # By convention, we tend to use "high" for entries, "middle"
    # for buttons, and "low" for sliders.  The full name for the
    # frame is .run_${octHandle}.$position, where $name is the name of the
    # universe, and $position is "high", "middle", or "low".
    frame $ptkControlPanel.high -bd 10
    frame $ptkControlPanel.middle -bd 10
    frame $ptkControlPanel.low -bd 10

    # Define the panel of control buttons
    frame $ptkControlPanel.panel -bd 10
	button $ptkControlPanel.panel.go -text " GO <Return> " \
	    -command "ptkGo $name $octHandle" -width 14
		
	button $ptkControlPanel.panel.pause -text "PAUSE <Space>" \
		-command "ptkPause $name $octHandle" -width 14
	button $ptkControlPanel.panel.stop -text "STOP <Escape>" \
		-command "ptkStop $name" -width 14
	pack append $ptkControlPanel.panel \
	    $ptkControlPanel.panel.go {left expand fill expand} \
	    $ptkControlPanel.panel.pause {left expand fill expand} \
	    $ptkControlPanel.panel.stop {right expand fill expand}

    # The debug panel will be filled with buttons when debugging is
    # turned on.  It starts out off always.
    frame $ptkControlPanel.debug -bd 10 

    # Animation is off by default
    # Note that since pre and post actions are global, there is no point
    # in associating the universe name with the ptkGrAnimation flag.
    global ptkGrAnimationFlag
    set ptkGrAnimationFlag 0
    ptkGrAnimation 0

    button $ptkControlPanel.dismiss -text "DISMISS" -command \
	"ptkRunControlDel $name $ptkControlPanel $octHandle $defNumIter"

    pack append $ptkControlPanel \
	$ptkControlPanel.msg {top fill expand} \
	$ptkControlPanel.iter top \
	$ptkControlPanel.panel top \
	$ptkControlPanel.debug top \
	$ptkControlPanel.high top \
	$ptkControlPanel.middle top \
	$ptkControlPanel.low top \
	$ptkControlPanel.dismiss {top fill expand}

    wm geometry $ptkControlPanel +400+400
    focus $ptkControlPanel
    wm protocol $ptkControlPanel WM_DELETE_WINDOW \
	"ptkRunControlDel $name $ptkControlPanel $octHandle $defNumIter"

    set olduniverse [curuniverse]
    newuniverse $name
    ptkCompile $octHandle
    curuniverse $olduniverse

    bind $ptkControlPanel.iter.entry <Return> \
		"ptkGo $name $octHandle"
    bind $ptkControlPanel.iter.entry <Escape> "ptkStop $name"
    bind $ptkControlPanel.iter.entry <space> "ptkPause $name $octHandle"
    bind $ptkControlPanel <Return> "ptkGo $name $octHandle"
    bind $ptkControlPanel <space> "ptkPause $name $octHandle"
    bind $ptkControlPanel <Escape> "ptkStop $name"
    bind $ptkControlPanel <Control-d> \
	"ptkRunControlDel $name $ptkControlPanel $octHandle $defNumIter"
}

#######################################################################
# Procedure to open or close the debug section of the control panel
#
proc ptkSetOrClearDebug { name octHandle } {
    global ptkDebug
    set w .run_$octHandle
    if {$ptkDebug($name)} {
	# Turning debug on.  Enable verbose Tcl messages.
	set ptkVerboseErrors 1
	# Create control panel
	frame $w.debug.eph
	button $w.debug.eph.step -text "STEP" \
		-command "ptkStep $name $octHandle" -width 10
	button $w.debug.eph.abort -text "ABORT" -command "ptkAbort $name" \
		-width 10
	# Animation is off by default
	# Note that since pre and post actions are global, there is no point
	# in associating the universe name with the Animation flags.
	global ptkGrAnimationFlag ptkTxAnimationFlag
	set ptkGrAnimationFlag 0
	ptkGrAnimation 0
	set ptkTxAnimationFlag 0
	ptkTxAnimation 0
	frame $w.debug.eph.anim
	checkbutton $w.debug.eph.anim.gr -text "Graphical Animation" \
	    -variable ptkGrAnimationFlag -relief flat \
	    -command {ptkGrAnimation $ptkGrAnimationFlag}
	checkbutton $w.debug.eph.anim.tx -text "Textual Animation" \
	    -variable ptkTxAnimationFlag -relief flat \
	    -command {ptkTxAnimation $ptkTxAnimationFlag}
	pack append $w.debug.eph.anim $w.debug.eph.anim.tx {top frame w} \
	    $w.debug.eph.anim.gr {top frame w}
	pack append $w.debug.eph \
	    $w.debug.eph.step {left fill expand} \
	    $w.debug.eph.abort {left fill expand} \
	    $w.debug.eph.anim {left fill expand}
	pack append $w.debug $w.debug.eph left
    } {
	# Turning debug off.  Destroy control panel.
	catch {destroy $w.debug.eph}
	# Disable verbose Tcl errors
	set ptkVerboseErrors 0
    }
}


#######################################################################
# Procedure to turn on or off graphical animation
#
proc ptkGrAnimation { on} {
    global ptkGrAnimationAction
    if [info exists ptkGrAnimationAction] {
 	cancelAction $ptkGrAnimationAction
 	unset ptkGrAnimationAction
    }
    if {$on} {
	set ptkGrAnimationAction \
	    [registerAction pre "ptkHighlightStar"]
    } {
	ptkClearHighlights
    }
}

#######################################################################
# Procedure to light up a star.
# A one second delay is inserted to make this more useful when running
# open loop.
#
proc ptkHighlightStar { star } {
    ptkClearHighlights
    ptkHighlight $star
    after 150
}

#######################################################################
# Procedure to turn on or off textual animation
#
proc ptkTxAnimation { on} {
    global ptkTxAnimationAction
    if [info exists ptkTxAnimationAction] {
 	cancelAction $ptkTxAnimationAction
 	unset ptkTxAnimationAction
    }
    set win .ptkTxAnimationWindow
    if {$on} {
	if {![winfo exists $win]} {
	    toplevel $win
	    wm title $win "Textual Animation"
	    wm iconname $win "Textual Animation"
	    message $win.msg -font -Adobe-times-medium-r-normal--*-180* \
		-width 12c -text "Executing stars:"
	    frame $win.text
	    text $win.text.t -relief raised -bd 2 -width 60 -height 20 \
                -bg AntiqueWhite \
                -yscrollcommand "$win.text.s set" -setgrid true
            scrollbar $win.text.s -relief flat -command "$win.text.t yview"
            pack append $win.text $win.text.s {right filly} \
                $win.text.t {expand fill}
	    button $win.ok -text "DISMISS" -command "ptkTxAnimation off"
            pack append $win $win.msg {top fill} $win.text {top fill expand} \
		$win.ok {bottom fillx}
	}
	set ptkTxAnimationAction \
	    [registerAction pre "ptkPrintStarName"]
    } {
	set ptkTxAnmiationFlag 0
	catch {destroy $win}
    }
}

#######################################################################
# Procedure to print a star name to the text animation window
#
proc ptkPrintStarName { star } {
    set win .ptkTxAnimationWindow
    if [winfo exists $win] {
	$win.text.t yview -pickplace end
	$win.text.t insert end $star
	$win.text.t insert end "\n"
    }
}

#######################################################################
# Procedure to delete a control window
#
proc ptkRunControlDel { name window octHandle defNumIter} {
    global ptkRunFlag
    if {![info exists ptkRunFlag($name)]} {
	# Assume the window has been deleted already and ignore command
	return
    }
    if [regexp {^ACTIVE$|^PAUSED$|^STOP_PENDING$|^ABORT$} $ptkRunFlag($name)] {
	ptkImportantMessage .message {System is still running.  Please stop it.}
	return
    }
    unset ptkRunFlag($name)
    # update the oct facet only if the number of iterations has changed.
    if {$defNumIter != [$window.iter.entry get]} {
         ptkSetRunLength $octHandle [$window.iter.entry get]
    }
    deluniverse $name
    catch {destroy $window}
}

#######################################################################
# basic procedure to stop a run
proc ptkStop { name } {
    global ptkRunFlag
    if {![info exists ptkRunFlag($name)]} {
	# Assume the window has been deleted already and ignore command
	return
    }
    # Ignore if the named system is not running or paused
    if {$ptkRunFlag($name) != {ACTIVE} && \
	$ptkRunFlag($name) != {PAUSED}} return
    halt
    # Note that the following set will release the ptkPause proc
    set ptkRunFlag($name) STOP_PENDING
}

#######################################################################
# procedure to stop a run without running wrapup
proc ptkAbort { name } {
    global ptkRunFlag
    if {![info exists ptkRunFlag($name)]} {
	# Assume the window has been deleted already and ignore command
	return
    }
    # Ignore if the named system is not running or paused
    if {$ptkRunFlag($name) != {ACTIVE} && \
	$ptkRunFlag($name) != {PAUSED}} return
    halt
    # Note that the following set will release the ptkPause proc
    set ptkRunFlag($name) ABORT
}

#######################################################################
# procedure to pause a run
proc ptkPause { name octHandle } {
    global ptkRunFlag
    #Ignore the command if the system is not running
    if {![info exists ptkRunFlag($name)] || \
    	$ptkRunFlag($name) != {ACTIVE}} return
    set cntrWindow .run_$octHandle
    catch {$cntrWindow.panel.go configure -relief raised}
    catch {$cntrWindow.panel.pause configure -relief sunken}
    set ptkRunFlag($name) PAUSED
    tkwait variable ptkRunFlag($name)
}

#######################################################################
# procedure to single step (execute one star)
proc ptkStep { name octHandle } {
    # Ignore command if universe is ACTIVE or a stop or abort is pending
    global ptkRunFlag
    if {[info exists ptkRunFlag($name)] && \
        [regexp {^ACTIVE$|^STOP_PENDING$|^ABORT$} $ptkRunFlag($name)]} {
            return
    }
    global ptkStepAction
    if {![info exists ptkStepAction($name)]} {
    	set ptkStepAction($name) \
	    [registerAction post "ptkStepTrap $name $octHandle"]
    }
    ptkGo $name $octHandle
}

#######################################################################
# procedure invoked after a star executes in single step mode.
# If the star is an auto-fork, then we skip it, on the theory that its
# invocation is not interesting.
proc ptkStepTrap { name octHandle star } {
    if {[string first "auto-fork" $star] >= 0} return
    global ptkStepAction
    if {[info exists ptkStepAction($name)]} {
	cancelAction $ptkStepAction($name)
	unset ptkStepAction($name)
    }
    ptkPause $name $octHandle
}

#######################################################################
#procedure to go
proc ptkGo {name octHandle} {
    global ptkRunFlag
    # For now, we allow only one run at a time.
    set univ [curuniverse]
    if {[info exists ptkRunFlag($univ)] && \
	[regexp {^ACTIVE$|^STOP_PENDING$|^ABORT$} $ptkRunFlag($univ)] || \
	($univ != $name && $ptkRunFlag($univ) == {PAUSED}) } {
            ptkImportantMessage .error "Sorry.  Only one run at time. "
	    return
    }
    global ptkControlPanel
    set ptkControlPanel .run_$octHandle
    catch {$ptkControlPanel.panel.go configure -relief sunken}
    catch {$ptkControlPanel.panel.pause configure -relief raised}
    # So that error highlighting, etc. works
    if {$univ != $name} {ptkSetHighlightFacet $octHandle}
    if {[info exists ptkRunFlag($name)] && \
    	$ptkRunFlag($name) == {PAUSED}} {
	    # Setting of ptkRunFlag should
	    # trigger the tkwait in the ptkPause proc,
	    # thus allowing the run to continue.
    	    set ptkRunFlag($name) ACTIVE
	    return
    }
    set ptkRunFlag($name) ACTIVE
    # catch errors and reset the run flag.
    if {[catch {
        curuniverse $name
        ptkCompile $octHandle
        set w .run_$octHandle
        set numIter [$w.iter.entry get]
        run $numIter
	if {$ptkRunFlag($name) != {ABORT}} {wrapup}
	ptkClearRunFlag $name $octHandle
    } msg] == 1} {
	ptkClearRunFlag $name $octHandle
	error $msg
    }
}
