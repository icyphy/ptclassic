# Definition of a bunch of control panels for the tcl/tk Ptolemy interface
# Author: Edward A. Lee
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
#
# The global array ptkRunFlag($name), indexed by the name of the universe,
# has the following values:
#	undefined		no active run window, universe not created
#	IDLE			active run window, but no run yet
#	ACTIVE			active run
#	PAUSED			paused run
#	FINISHED		finished a run
#	ERROR			the last run terminated with an error
#	STOP_PENDING		pending stop requested
#	ABORT			pending abort requested

# Need a dummy value in ptkRunFlag(main) for startup
set ptkRunFlag(main) IDLE

# The global variable ptkControlPanel is the name of the master control
# panel window that is currently being used for a run.  This name is therefore
# available to stars that wish to insert frames into the control panel.
# It is equal to .run_$octHandle.

#######################################################################
# Set the ptkRunFlag to FINISHED, indicating the run is over, and pop up the
# GO button.
#
proc ptkClearRunFlag { name octHandle } {
    global ptkRunFlag
    set  cntrWindow .run_$octHandle
    catch {$cntrWindow.panel.gofr.go configure -relief raised}
    catch {$cntrWindow.panel.pause configure -relief raised}
    if {[info exists ptkStepAction($name)]} {
	cancelAction $ptkStepAction($name)
	unset ptkStepAction($name)
    }
    set ptkRunFlag($name) FINISHED
    return ""
}

#######################################################################
# Return "1" if the system has has been run successfully at least once,
# "0" otherwise.  This procedure is used by pigilib/compile.c.
#
proc ptkHasRun { name } {
    global ptkRunFlag
    if {![info exists ptkRunFlag($name)] || \
	$ptkRunFlag($name) == {IDLE} || \
	$ptkRunFlag($name) == {ERROR}} {
            return 0
    } { return 1 }
}


#######################################################################
# Procedure to run a universe.
#
proc ptkRunControl { name octHandle } {
    global ptkRunFlag ptkDebug ptkRunEventLoop ptkControlPanel ptkOctHandles \
	ptkScriptOn

    set ptkControlPanel .run_$octHandle
    if {[info exists ptkRunFlag($name)] && [winfo exists $ptkControlPanel]} {
	raise $ptkControlPanel
	return
    }

    set ptkDebug($name) 0
    set ptkRunEventLoop($name) 1
    set ptkScriptOn($name) 0
    set ptkOctHandles($name) $octHandle
    
    # Mark an open window, but with no run yet.
    set ptkRunFlag($name) IDLE

    catch {destroy $ptkControlPanel}
    # Sets the Class of the Window.  This is used to set all options
    #   for widgets used in the Contol window
    toplevel $ptkControlPanel -class PigiControl
    wm title $ptkControlPanel "Run $name"
    wm iconname $ptkControlPanel "Run $name"

    set defNumIter [ptkGetRunLength $octHandle]

    message $ptkControlPanel.msg \
	    -width 25c -text "Control panel for $name" -justify center

    frame $ptkControlPanel.options
	checkbutton $ptkControlPanel.options.debug -text "Debug" \
	    -variable ptkDebug($name) -relief flat \
	    -command "ptkSetOrClearDebug $name $octHandle"
	checkbutton $ptkControlPanel.options.script -text "Script" \
	    -variable ptkScriptOn($name) -relief flat \
	    -command "ptkToggleScript $name $octHandle"
	pack append $ptkControlPanel.options \
	    $ptkControlPanel.options.debug {right padx 20} \
	    $ptkControlPanel.options.script {right padx 20}

    # Define the entry that controls the number of iterations
    # Note: This may be replaced by the user with the script window
    # defined below.
    frame $ptkControlPanel.iter -bd 10
	label $ptkControlPanel.iter.label -text "When to stop:"
        entry $ptkControlPanel.iter.entry -relief sunken
	$ptkControlPanel.iter.entry insert @0 $defNumIter
	pack $ptkControlPanel.iter.label -side left
	pack $ptkControlPanel.iter.entry -side left -fill x -expand yes

    # Alternative to the iter frame: a script window
    frame $ptkControlPanel.tclScript -bd 10
        pack [label $ptkControlPanel.tclScript.label -text "Tcl Script:" ] \
            -side top -anchor w
        pack [frame $ptkControlPanel.tclScript.tframe] \
	    -side top -fill both -expand 1
        scrollbar $ptkControlPanel.tclScript.tframe.vscroll \
	    -relief flat \
	    -command "$ptkControlPanel.tclScript.tframe.text yview"
        pack $ptkControlPanel.tclScript.tframe.vscroll \
	    -side right -fill y
        text $ptkControlPanel.tclScript.tframe.text \
	    -wrap word -height 8 -width 40 -setgrid true \
	    -yscrollcommand "$ptkControlPanel.tclScript.tframe.vscroll set" \
	    -relief sunken -bd 2
	set scriptValue [ptkGetStringProp $octHandle script]
	if {$scriptValue == ""} {
	    set scriptValue "run $defNumIter\nwrapup\n"
	}
	$ptkControlPanel.tclScript.tframe.text insert end $scriptValue
        pack $ptkControlPanel.tclScript.tframe.text -expand yes -fill both

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
	frame $ptkControlPanel.panel.gofr  -relief sunken -bd 2
	button $ptkControlPanel.panel.gofr.go -text " GO <Return> " \
	    -command "ptkGo $name $octHandle" -width 14
	pack append $ptkControlPanel.panel.gofr \
	    $ptkControlPanel.panel.gofr.go {fill}
		
	button $ptkControlPanel.panel.pause -text "PAUSE <Space>" \
		-command "ptkPause $name $octHandle" -width 14
	button $ptkControlPanel.panel.stop -text "STOP <Escape>" \
		-command "ptkStop $name" -width 14
	pack append $ptkControlPanel.panel \
	    $ptkControlPanel.panel.gofr {left fill} \
	    $ptkControlPanel.panel.pause {left fill} \
	    $ptkControlPanel.panel.stop {right fill}

    # The debug panel will be filled with buttons when debugging is
    # turned on.  It starts out off always.
    frame $ptkControlPanel.debug -bd 10

    # Animation is off by default
    ptkGrAnimation 0

    frame $ptkControlPanel.disfr
    button $ptkControlPanel.disfr.dismiss -text "DISMISS" -command \
	"ptkRunControlDel $name $ptkControlPanel $octHandle $defNumIter"
    pack append $ptkControlPanel.disfr \
	$ptkControlPanel.disfr.dismiss {top fillx}

    # Newer syntax for pack command used below
    pack $ptkControlPanel.msg -fill both
    pack $ptkControlPanel.options
    if {[ptkGetStringProp $octHandle usescript] == "1"} {
	set ptkScriptOn($name) 1
	pack $ptkControlPanel.tclScript -expand 1 -fill both \
	    -after $ptkControlPanel.options
    } {
        pack $ptkControlPanel.iter -anchor w -fill x
    }
    pack $ptkControlPanel.panel
    pack $ptkControlPanel.high -fill x -padx 10
    pack $ptkControlPanel.middle -fill x -padx 10
    pack $ptkControlPanel.low -fill x -padx 10
    pack $ptkControlPanel.disfr -fill x

    wm geometry $ptkControlPanel +400+400
    focus $ptkControlPanel
    wm protocol $ptkControlPanel WM_DELETE_WINDOW \
	"ptkRunControlDel $name $ptkControlPanel $octHandle $defNumIter"

    set olduniverse [curuniverse]
    newuniverse $name
    if {[catch {ptkCompile $octHandle} msg] == 1} {
	# An error has occurred.
	ptkClearRunFlag $name $octHandle
	# Mark an error
	set ptkRunFlag($name) ERROR
	ptkImportantMessage .error $msg
    }

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
# Procedure to turn on or off the event loop during a run
#     - Alan Kamas
#
proc ptkSetRunInteractivity { name octHandle } {
    global ptkDebug ptkRunEventLoop ptkRunFlag
    set Panelwindow .run_$octHandle
    if {$ptkRunEventLoop($name)} {
	# TkEventLoop is active during a run
	# If the run is in progress, turn event loop on
	if {$ptkRunFlag($name)=={ACTIVE}||$ptkRunFlag($name)=={PAUSED}} {
	    ptkSetEventLoop on
	}
	# Return pause, run, and debug to active states
        if {[winfo exists $Panelwindow]} {
	    $Panelwindow.panel.pause configure -state normal
	    $Panelwindow.panel.stop configure -state normal
	    $Panelwindow.options.debug configure -state normal
	}
	# Debug is now allowed
	ptkSetOrClearDebug $name $octHandle
    } {
	# Turn off debug mode
	set ptkDebug($name) 0
	# Close down debug window
	ptkSetOrClearDebug $name $octHandle
	# mark the pause, stop, and debug buttons inactive
        if {[winfo exists $Panelwindow]} {
	    $Panelwindow.panel.pause configure -state disabled
	    $Panelwindow.panel.stop configure -state disabled
	    $Panelwindow.options.debug configure -state disabled
	}
	# If the run is in progress, turn off the event loop
	if {$ptkRunFlag($name)=={ACTIVE} || $ptkRunFlag($name)=={PAUSED} } {
	    # one last call to update to make sure that the pause/stop/debug
            # buttons get turned off
	    update idletasks
            # now turn off the event loop
	    ptkSetEventLoop off
	}
    }
}

#######################################################################
# Procedure to toggle between scripted runs and non-scripted runs
#
proc ptkToggleScript { name octHandle } {
    global ptkControlPanel ptkScriptOn
    set ptkControlPanel .run_$octHandle
    if {$ptkScriptOn($name)} {
	# Switching to scripted runs
	pack forget $ptkControlPanel.iter
	pack $ptkControlPanel.tclScript -expand 1 -fill both \
	    -after $ptkControlPanel.options
    } {
	# Reverting to non-scripted run
	pack forget $ptkControlPanel.tclScript
	pack $ptkControlPanel.iter -anchor w -fill x \
	    -after $ptkControlPanel.options
	wm geometry $ptkControlPanel ""
    }
}

#######################################################################
# Procedure to open or close the debug section of the control panel
#
proc ptkSetOrClearDebug { name octHandle } {
    global ptkDebug ptkVerboseErrors
    set w .run_$octHandle
    if {$ptkDebug($name)} {
	# Turning debug on.  Enable verbose Tcl messages.
	set ptkVerboseErrors 1
	# Create control panel
	pack after $w.panel $w.debug {top frame w}
	if {![winfo exists $w.debug.left]} {
	    # Left half of control panel
	    frame $w.debug.left
	    frame $w.debug.left.buttons
	    button $w.debug.left.buttons.step -text "STEP" \
		-command "ptkStep $name $octHandle" -width 9
	    button $w.debug.left.buttons.abort -text "ABORT" \
		-command "ptkAbort $name" -width 9
	    pack append $w.debug.left.buttons \
	        $w.debug.left.buttons.step {left fill expand} \
	        $w.debug.left.buttons.abort {left fill expand}
	    frame $w.debug.left.runcount -bd 10
	    label $w.debug.left.runcount.lbl -text "Count:"
	    label $w.debug.left.runcount.cnt -width 10
	    $w.debug.left.runcount.cnt configure -text "0"
	    pack append $w.debug.left.runcount \
	        $w.debug.left.runcount.lbl {left fill expand} \
	        $w.debug.left.runcount.cnt {left fill expand}
	    pack append $w.debug.left \
	        $w.debug.left.buttons {top fill expand} \
	        $w.debug.left.runcount {top fill expand}
	    frame $w.debug.anim -bd 10
	    checkbutton $w.debug.anim.gr -text "Graphical Animation" \
	        -variable ptkGrAnimationFlag -relief flat \
	        -command {ptkGrAnimation $ptkGrAnimationFlag}
	    checkbutton $w.debug.anim.tx -text "Textual Animation" \
	        -variable ptkTxAnimationFlag -relief flat \
	        -command {ptkTxAnimation $ptkTxAnimationFlag}
	    pack append $w.debug.anim \
	        $w.debug.anim.tx {top frame w} \
	        $w.debug.anim.gr {top frame w}
	    pack append $w.debug \
	        $w.debug.left {left frame w} \
	        $w.debug.anim {left fill expand}
	}
	# Animation is off by default
	# Note that since pre and post actions are global, there is no point
	# in associating the universe name with the Animation flags.
	ptkGrAnimation 0
	ptkTxAnimation 0
	ptkUpdateCount $name $octHandle
    } {
	# Turning debug off.  Turn off animation first.
	ptkGrAnimation 0
	ptkTxAnimation 0
	# Close control panel.
	catch {pack unpack $w.debug}
	# Disable verbose Tcl errors
	set ptkVerboseErrors 0
    }
}

#######################################################################
# Procedure to update the iteration count portion of the control panel
#
proc ptkUpdateCount { name octHandle } {
    global ptkDebug ptkRunFlag
    set win .run_$octHandle.debug.left.runcount.cnt
    # only update again if there is an active debug run in progress
    #   This prevents a huge pile up of ptkUpdateCounts due to stepping
    #   or pause-go-pause etc.
    if {[info exists ptkDebug($name)] && $ptkDebug($name) && [info exists ptkRunFlag($name)] && [regexp {^ACTIVE$} $ptkRunFlag($name)] } {
        catch {$win configure -text [schedtime]}
        after 200 ptkUpdateCount $name $octHandle
    }
}

#######################################################################
# Procedure to turn on or off graphical animation
#
proc ptkGrAnimation { on} {

    global ptkGrAnimationAction ptkGrAnimationFlag

    # Note that since pre and post actions are global, there is no point
    # in associating the universe name with the ptkGrAnimation flag.
    set ptkGrAnimationFlag $on
    if [info exists ptkGrAnimationAction] {
 	cancelAction $ptkGrAnimationAction
 	unset ptkGrAnimationAction
    }
    if {$on} {
	set ptkGrAnimationAction [registerAction pre "ptkHighlightStar"]
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
    update
}

#######################################################################
# Procedure to turn on or off textual animation
#
proc ptkTxAnimation { on } {
    global ptkTxAnimationFlag ptkTxAnimationAction
    set ptkTxAnimationFlag $on
    set win .ptkTxAnimationWindow
    if {$on} {
	if {![winfo exists $win]} {
	    toplevel $win
	    wm title $win "Textual Animation"
	    wm iconname $win "Textual Animation"
	    message $win.msg -width 12c -text "Executing stars:"
	    frame $win.text
	    text $win.text.t -relief raised -bd 2 -width 60 -height 20 \
                -bg [ptkColor AntiqueWhite] \
                -yscrollcommand "$win.text.s set" -setgrid true
            scrollbar $win.text.s -relief flat -command "$win.text.t yview"
            pack append $win.text $win.text.s {right filly} \
                $win.text.t {expand fill}
	    button $win.ok -text "DISMISS" -command "ptkTxAnimation 0"
            pack append $win $win.msg {top fill} $win.text {top fill expand} \
		$win.ok {bottom fillx}
	    tkwait visibility $win
	}
    }
    if [info exists ptkTxAnimationAction] {
 	cancelAction $ptkTxAnimationAction
 	unset ptkTxAnimationAction
    }
    if {$on} {
	set ptkTxAnimationAction [registerAction pre "ptkPrintStarName"]
    } {
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
    global ptkDebug ptkOctHandles ptkRunEventLoop ptkRunFlag ptkScriptOn

    if {![info exists ptkRunFlag($name)]} {
	# Assume the window has been deleted already and ignore command
	return
    }

    # Remember for next time whether the control panel is set
    # for scripted runs or simple runs.
    ptkSetStringProp $octHandle usescript $ptkScriptOn($name)

    # Turn debug off
    if {$ptkDebug($name)} {
	set ptkDebug($name) 0
	ptkSetOrClearDebug $name $octHandle
    }

    if [regexp {^ACTIVE$|^PAUSED$} $ptkRunFlag($name)] {
	ptkAbort $name 
	update
    }
    if [regexp {^STOP_PENDING$|^ABORT$} $ptkRunFlag($name)] {
	# If the universe hasn't stopped already, try again later
	after 200 ptkRunControlDel $name $window $octHandle $defNumIter
	return
    }
    catch {unset ptkDebug($name)}
    catch {unset ptkOctHandles($name)}
    catch {unset ptkRunEventLoop($name)}
    catch {unset ptkRunFlag($name)}
    catch {unset ptkScriptOn($name)}
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
    # Note that the following set will release the ptkPause proc
    set ptkRunFlag($name) STOP_PENDING
    halt
    global ptkOctHandles
    if {[info exists ptkOctHandles($name)]} {
	after 2000 "ptkForceReset STOP_PENDING $name $ptkOctHandles($name)"
    }
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
    # Note that the following set will release the ptkPause proc
    set ptkRunFlag($name) ABORT
    halt
    global ptkOctHandles
    if {[info exists ptkOctHandles($name)]} {
	after 2000 "ptkForceReset ABORT $name $ptkOctHandles($name)"
    }
}

#######################################################################
# Safety net: if after some time the ptkRunFlag has not been reset,
# then reset it.
proc ptkForceReset { trigger name octHandle } {
    global ptkRunFlag
    if {[info exists ptkRunFlag($name)] && $ptkRunFlag($name) == $trigger} {
	ptkClearRunFlag $name $octHandle
    }
}

#######################################################################
# procedure to pause a run
proc ptkPause { name octHandle } {
    global ptkRunFlag
    #Ignore the command if the system is not running
    if {![info exists ptkRunFlag($name)] || \
    	$ptkRunFlag($name) != {ACTIVE}} return
    set cntrWindow .run_$octHandle
    catch {$cntrWindow.panel.gofr.go configure -relief raised}
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
    global ptkRunFlag ptkRunEventLoop

    # For now, we allow only one run at a time.
    set univ [curuniverse]
    if {[info exists ptkRunFlag($univ)] && \
	([regexp {^ACTIVE$|^STOP_PENDING$|^ABORT$} $ptkRunFlag($univ)] || \
	($univ != $name && $ptkRunFlag($univ) == {PAUSED})) } {
            ptkImportantMessage .error "Sorry.  Only one run at time. "
	    return
    }

    global ptkControlPanel
    set ptkControlPanel .run_$octHandle

    # Turn the event loop on (or off) for this run 
    ptkSetEventLoop $ptkRunEventLoop($name)

    # arrange to call the routine which updates the
    # iteration count displayed in the debug window.
    # the debug flag controls it being called after 
    # the first time, so this places the control of the
    # update in the ptkUpdateCount routine.
    after 200 ptkUpdateCount $name $octHandle

    catch {$ptkControlPanel.panel.gofr.go configure -relief sunken}
    catch {$ptkControlPanel.panel.pause configure -relief raised}

    # So that error highlighting, etc. works
    if {$univ != $name} {ptkSetHighlightFacet $octHandle}
    ptkClearHighlights

    # if we are currently paused, then all we have to do
    # is set the status back to ACTIVE, and return to 
    # ensure things start up again.
    if {[info exists ptkRunFlag($name)] && \
    	$ptkRunFlag($name) == {PAUSED}} {
	    # Setting of ptkRunFlag should
	    # trigger the tkwait in the ptkPause proc,
	    # thus allowing the run to continue.
    	    set ptkRunFlag($name) ACTIVE
	    return
    }

    # we were not PAUSED, so we have extra stuff to do
    # besides setting the flag to ACTIVE, but get that
    # out of the way first, and make sure the change in 
    # button relief gets displayed
    set ptkRunFlag($name) ACTIVE
    update

    # catch errors and reset the run flag.
    if {[catch {
        curuniverse $name
        ptkCompile $octHandle
        set w .run_$octHandle

	global ptkScriptOn 

	if { $ptkScriptOn($name) } {
	    # we want to run the script. This means we have to
	    # get the contents of the text window, and then get
	    # the TCL interpreter to evaluate it
	    ptkSetStringProp $octHandle script \
	        [$ptkControlPanel.tclScript.tframe.text get 0.0 end]
	    eval [$ptkControlPanel.tclScript.tframe.text get 0.0 end]
	} {
	    # default run: just run through the specified number
	    # of iterations, finishing by invoking
	    # wrapup if no error occurred.
            set numIter [$w.iter.entry get]
            run $numIter
    
    	    if {[info exists ptkRunFlag($name)] &&
    	        $ptkRunFlag($name) != {ABORT}} { wrapup } {
    	        # Mark an error if the system was aborted
    	        set ptkRunFlag($name) ERROR
	    }
	}

        # we have finished running
	ptkClearRunFlag $name $octHandle
    } msg] == 1} {
	# An error has occurred.
	ptkClearRunFlag $name $octHandle
	# Mark an error
	set ptkRunFlag($name) ERROR
	error $msg
    }
}
