# Definition of a bunch of control panels for the tcl/tk Ptolemy interface
# Author: Edward A. Lee
# Version: @(#)ptkControl.tcl	1.61 12/10/97
#
# Copyright (c) 1990-1997 The Regents of the University of California.
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
# Need a default val for ptkTimeFlag or run will error and die
set ptkTimeFlag 0
# The global variable ptkControlPanel is the name of the master control
# panel window that is currently being used for a run.  This name is therefore
# available to stars that wish to insert frames into the control panel.
# It is equal to .run_$octHandle.
set ptkControlPanel ""

#######################################################################
# Set the ptkRunFlag to FINISHED, indicating the run is over, and pop up the
# GO button.
#
proc ptkClearRunFlag { name octHandle } {
    global ptkRunFlag ptkStepAction
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
# Procedure to initialize the run control panel for those parts
# of the window that do not depend on the Oct interface (octHandle arg)
# Used by ptkRunControl and ptkRunControlStandalone
#
proc ptkRunControlInit { name ctrlPanel title msgtext } {
    global ptkDebug ptkRunFlag ptkRunEventLoop ptkScriptOn

    # Reset global settings
    # -- set ptkRunFlag as open run panel but with nothing running yet
    set ptkDebug($name) 0
    set ptkRunFlag($name) IDLE
    set ptkRunEventLoop($name) 1
    set ptkScriptOn($name) 0

    catch {destroy $ctrlPanel}
    # Sets the Class of the Window.  This is used to set all options
    #   for widgets used in the Contol window
    toplevel $ctrlPanel -class PigiControl
    wm title $ctrlPanel $title
    wm iconname $ctrlPanel $title

    message $ctrlPanel.msg -width 25c -text $msgtext -justify center

    # The following empty frames are created so that they are available
    # to stars to insert custom controls into the control panel.
    # By convention, we tend to use "high" for entries, "middle"
    # for buttons, and "low" for sliders.  The full name for the
    # frame is .run_${octHandle}.$position, where $name is the name of the
    # universe, and $position is "high", "middle", or "low".
    frame $ctrlPanel.high
    frame $ctrlPanel.middle
    frame $ctrlPanel.low
}

#######################################################################
# Procedure to initialize the run control panel for standalone programs
# as an alternative to ptkRunControl; it defines those parts of the
# run control panel that do not depend on the Oct interface
# Used by the compile-SDF target
#
proc ptkRunControlStandalone { name } {
    set ctrlPanel .run_$name
    ptkRunControlInit $name $ctrlPanel $name "Standalone version of $name"

    global ptkControlPanel
    set ptkControlPanel $ctrlPanel

    pack $ctrlPanel.msg -fill both
    pack $ctrlPanel.high -fill x -padx 10
    pack $ctrlPanel.middle -fill x -padx 10
    pack $ctrlPanel.low -fill x -padx 10

    wm geometry $ctrlPanel +400+400
    focus $ctrlPanel
}

#######################################################################
# Procedure to run a universe.
#
proc ptkRunControl { name octHandle } {
    global ptkDebug ptkOctHandles ptkRunFlag ptkRunEventLoop ptkScriptOn

    set ctrlPanel .run_$octHandle
    if {[info exists ptkRunFlag($name)] && [winfo exists $ctrlPanel]} {
	wm deiconify $ctrlPanel
	raise $ctrlPanel
	return
    }

    ptkRunControlInit $name $ctrlPanel "Run $name" "Control panel for $name"
    set ptkOctHandles($name) $octHandle

    # Ensure panel is not shown until it is fully constructed
    wm withdraw $ctrlPanel

    # Animation is off by default
    ptkGrAnimation 0

    # Build subframe of user-selected "visible parameters"
    frame $ctrlPanel.vparams
    ptkBuildVParams $ctrlPanel.vparams $octHandle

    frame $ctrlPanel.options
	checkbutton $ctrlPanel.options.script -text "Script" \
	    -variable ptkScriptOn($name) -relief flat \
	    -command "ptkToggleScript $name $octHandle"
	checkbutton $ctrlPanel.options.debug -text "Debug" \
	    -variable ptkDebug($name) -relief flat \
	    -command "ptkSetOrClearDebug $name $octHandle"
	pack append $ctrlPanel.options \
	    $ctrlPanel.options.debug {right padx 20} \
	    $ctrlPanel.options.script {right padx 20}

    # Define the entry that controls the number of iterations
    # Note: This may be replaced by the user with the script window
    # defined below.
    set defNumIter [ptkGetRunLength $octHandle]
    frame $ctrlPanel.iter -bd 10
	label $ctrlPanel.iter.label -text "When to stop:"
        entry $ctrlPanel.iter.entry -relief sunken
	$ctrlPanel.iter.entry insert @0 $defNumIter
	pack $ctrlPanel.iter.label -side left
	pack $ctrlPanel.iter.entry -side left -fill x -expand yes

    # Alternative to the iter frame: a script window
    frame $ctrlPanel.tclScript -bd 10
        pack [label $ctrlPanel.tclScript.label -text "Tcl Script:" ] \
            -side top -anchor w
        pack [frame $ctrlPanel.tclScript.tframe] \
	    -side top -fill both -expand 1
        scrollbar $ctrlPanel.tclScript.tframe.vscroll \
	    -relief flat \
	    -command "$ctrlPanel.tclScript.tframe.text yview"
        pack $ctrlPanel.tclScript.tframe.vscroll \
	    -side right -fill y
        text $ctrlPanel.tclScript.tframe.text \
	    -wrap word -height 8 -width 40 -setgrid true \
	    -yscrollcommand "$ctrlPanel.tclScript.tframe.vscroll set" \
	    -relief sunken -bd 2
	set scriptValue [ptkGetStringProp $octHandle script]
	if {$scriptValue == ""} {
	    set scriptValue "run $defNumIter\nwrapup\n"
	}
	$ctrlPanel.tclScript.tframe.text insert end $scriptValue
        pack $ctrlPanel.tclScript.tframe.text -expand yes -fill both

    # Define the panel of control buttons
    frame $ctrlPanel.panel -bd 10
	frame $ctrlPanel.panel.gofr  -relief sunken -bd 2
	button $ctrlPanel.panel.gofr.go -text " GO <Return> " \
	    -command "ptkGo $name $octHandle" -width 14
	pack append $ctrlPanel.panel.gofr \
	    $ctrlPanel.panel.gofr.go {fill}
		
	button $ctrlPanel.panel.pause -text "PAUSE <Space>" \
		-command "ptkPause $name $octHandle" -width 14
	button $ctrlPanel.panel.abort -text "ABORT <Escape>" \
		-command "ptkAbort $name" -width 14
	pack append $ctrlPanel.panel \
	    $ctrlPanel.panel.gofr {left fill} \
	    $ctrlPanel.panel.pause {left fill} \
	    $ctrlPanel.panel.abort {right fill}

    # The debug panel will be filled with buttons when debugging is
    # turned on.  It starts out off always.
    frame $ctrlPanel.debug -bd 10

    frame $ctrlPanel.disfr
    button $ctrlPanel.disfr.dismiss -text "DISMISS" -command \
	"ptkRunControlDel $name $octHandle $defNumIter"
    pack append $ctrlPanel.disfr \
	$ctrlPanel.disfr.dismiss {top fillx}

    # Newer syntax for pack command used below
    pack $ctrlPanel.msg -fill both
    pack $ctrlPanel.vparams -fill x
    pack $ctrlPanel.options
    if {[ptkGetStringProp $octHandle usescript] == "1"} {
	set ptkScriptOn($name) 1
	pack $ctrlPanel.tclScript -expand 1 -fill both \
	    -after $ctrlPanel.options
    } {
        pack $ctrlPanel.iter -anchor w -fill x
    }
    pack $ctrlPanel.panel
    pack $ctrlPanel.high -fill x -padx 10
    pack $ctrlPanel.middle -fill x -padx 10
    pack $ctrlPanel.low -fill x -padx 10
    pack $ctrlPanel.disfr -fill x

    wm geometry $ctrlPanel +400+400
    focus $ctrlPanel
    wm protocol $ctrlPanel WM_DELETE_WINDOW \
	"ptkRunControlDel $name $octHandle $defNumIter"

    # These bindings should apply everywhere in the control panel,
    # except in the tclScript subframe.
    bind $ctrlPanel <Return> \
	"if {\[string first {$ctrlPanel.tclScript} {%W}\] != 0} {ptkGo $name $octHandle}"
    bind $ctrlPanel <Escape> \
	"if {\[string first {$ctrlPanel.tclScript} {%W}\] != 0} {ptkAbort $name}"
    bind $ctrlPanel <space> \
	"if {\[string first {$ctrlPanel.tclScript} {%W}\] != 0} {ptkPause $name $octHandle}"
    bind $ctrlPanel <Control-d> \
	"if {\[string first {$ctrlPanel.tclScript} {%W}\] != 0} {ptkRunControlDel $name $octHandle $defNumIter}"

    focus $ctrlPanel.iter.entry

    set olduniverse [curuniverse]

    # EAL: The following call was here for a long time, but appears
    # to be entirely redundant, since ptkCompile creates a new universe.
    # newuniverse $name

    # Allow a target-specific procedure to customize the control panel
    # Note that we want this to happen before ptkCompile in case an error is
    # reported inside ptkCompile.  Hence must get target name the hard way.

    set targetToBe [lindex [ptkGetTargetNames $octHandle] 0]
    set targetcust ${targetToBe}-ControlPanel
    if {[info proc $targetcust] == $targetcust} {
      $targetcust $name $octHandle $ctrlPanel
    }

    if {[catch {ptkCompile $octHandle} msg] == 1} {
	# An error has occurred.
	ptkClearRunFlag $name $octHandle
	# Mark an error
	set ptkRunFlag($name) ERROR
	if {$msg != ""} { ptkImportantMessage .error $msg }
    }

    curuniverse $olduniverse

    # Show the finished control panel
    wm deiconify $ctrlPanel
}

#######################################################################
# Procedure to toggle between scripted runs and non-scripted runs
#
proc ptkToggleScript { name octHandle } {
    global ptkScriptOn
    set ctrlPanel .run_$octHandle
    if {$ptkScriptOn($name)} {
	# Switching to scripted runs
	pack forget $ctrlPanel.iter
	pack $ctrlPanel.tclScript -expand 1 -fill both \
	    -after $ctrlPanel.options
	focus $ctrlPanel.tclScript.tframe.text
    } {
	# Reverting to non-scripted run
	pack forget $ctrlPanel.tclScript
	pack $ctrlPanel.iter -anchor w -fill x \
	    -after $ctrlPanel.options
	focus $ctrlPanel.iter.entry
    }
}

#######################################################################
# Procedure to open or close the debug section of the control panel
#
proc ptkSetOrClearDebug { name octHandle } {
    global ptkDebug ptkVerboseErrors ptkTimeFlag
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
		-command "ptkStep $name $octHandle" -width 10
	    button $w.debug.left.buttons.earlyend -text "EARLY END" \
		-command "ptkEarlyEnd $name" -width 10
	    pack append $w.debug.left.buttons \
	        $w.debug.left.buttons.step {left fill expand} \
	        $w.debug.left.buttons.earlyend {left fill expand}
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
	    frame $w.debug.anim
	    checkbutton $w.debug.anim.tx -text "Textual Animation" \
	        -variable ptkTxAnimationFlag -relief flat \
	        -command {ptkTxAnimation $ptkTxAnimationFlag}
	    checkbutton $w.debug.anim.gr -text "Graphical Animation" \
	        -variable ptkGrAnimationFlag -relief flat \
	        -command {ptkGrAnimation $ptkGrAnimationFlag}
	    checkbutton $w.debug.anim.time -text "Time the Run" \
	        -variable ptkTimeFlag -relief flat
	    pack append $w.debug.anim \
	        $w.debug.anim.tx {top frame w} \
	        $w.debug.anim.gr {top frame w} \
	        $w.debug.anim.time {top frame w}
	    pack append $w.debug \
	        $w.debug.left {left frame w} \
	        $w.debug.anim {left fill expand}
	}
	# Animation and timing is off by default
	# Note that since pre and post actions are global, there is no point
	# in associating the universe name with the Animation flags.
	ptkGrAnimation 0
	ptkTxAnimation 0
	set ptkTimeFlag 0
	ptkUpdateCount $name $octHandle
    } {
	# Turning debug off.  Turn off animation first.
	ptkGrAnimation 0
	ptkTxAnimation 0

	# Turn off timing
	set ptkTimeFlag 0

	# Close control panel.
	catch {pack forget $w.debug}
	# Disable verbose Tcl errors
	set ptkVerboseErrors 0
    }
}

#######################################################################
# Procedure to build the display of "visible" universe parameters
# Each parameter has a subframe of myFrame that is named like "p_ParamName".
# (It'd be easier if we could use just the parameter name as the frame name,
# but Tk doesn't like names that begin with uppercase letters.)
#
proc ptkBuildVParams { myFrame octHandle } {
    set visibleList [ptkGetStringProp $octHandle "VisibleParameters"]
    # quick exit if feature not in use
    if {[llength $visibleList] == 0} { return }
    # set width depending on longest visible name
    set labWidth 5
    foreach pname $visibleList {
	set plen [string length $pname]
	if {$plen > $labWidth} { set labWidth $plen }
    }
    incr labWidth 2
    # insert entries into the control panel
    set univParamList [lindex [ptkGetParams $octHandle NIL] 1]
    if {$univParamList == "NIL"} { set univParamList {} }
    foreach param $univParamList {
	set pname [lindex $param 0]
	if {[lsearch -exact $visibleList $pname] >= 0} {
	    set subFrame $myFrame.p_$pname
	    frame $subFrame
	    label $subFrame.label -text "$pname:" -width $labWidth -anchor w
	    entry $subFrame.entry -relief sunken
	    $subFrame.entry insert end [lindex $param 2]
	    pack $subFrame.label -side left
	    pack $subFrame.entry -side left -fill x -expand 1
	    pack $subFrame -side top -fill x -padx 10 -pady 5
	}
    }
}

#######################################################################
# Procedure to update Oct from entries in the "visible" universe parameters
#
proc ptkUpdateVParams { myFrame octHandle } {
    # Extract parameter names and values from the widget tree
    set pNames {}
    set pValues {}
    foreach frm [winfo children $myFrame] {
	if [regsub {^p_} [winfo name $frm] {} pName] {
	    lappend pNames $pName
	    lappend pValues [$frm.entry get]
	}
    }
    if {[llength $pNames] == 0} { return }
    # Updating the Oct settings for the universe parameters
    # requires constructing a new list of all the parameters.
    # We avoid marking the facet dirty unnecessarily.
    set updateNeeded 0
    set oldParamList [lindex [ptkGetParams $octHandle NIL] 1]
    if {$oldParamList == "NIL"} { set oldParamList {} }
    set newParamList {}
    foreach param $oldParamList {
	set pIndex [lsearch -exact $pNames [lindex $param 0]]
	if {$pIndex >= 0} {
	    set newVal [lindex $pValues $pIndex]
	    if {$newVal != [lindex $param 2]} {
		set param [lreplace $param 2 2 $newVal]
		set updateNeeded 1
	    }
	}
	lappend newParamList $param
    }
    if {$updateNeeded} {
	ptkSetParams $octHandle NIL $newParamList
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
proc ptkGrAnimation { on } {

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
#
proc ptkHighlightStar { star args } {
    global ptkColorList

    ptkClearHighlights
    if {([llength $args] == 1)} {
	if {[lsearch [array names ptkColorList] [lindex $args 0]] == -1} {
	    return "Invalid color specification"
	}
	set tcl_col [ptkColor [lindex $args 0]]
	set red [format "%d" [format "0x%s" [string range $tcl_col 1 2]]]
	set green [format "%d" [format "0x%s" [string range $tcl_col 3 4]]]
	set blue [format "%d" [format "0x%s" [string range $tcl_col 5 6]]]
	# these colors are weak in intensity, here we multiply them
	set red [expr $red * 256]
	set green [expr $green * 256]
	set blue [expr $blue * 256]
    } else {
	if {[llength $args] >= 1} {
	    set red [lindex $args 0]
	} else { set red 65535 }
	if {[llength $args] >= 2} {
	    set green [lindex $args 1]
	} else { set green 0 }
	if {[llength $args] >= 3} {
	    set blue [lindex $args 2]
	} else { set blue 0 }
    }
    ptkHighlight $star $red $green $blue
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
proc ptkRunControlDel { name octHandle defNumIter } {
    global ptkDebug ptkOctHandles ptkRunEventLoop ptkRunFlag ptkScriptOn

    set ctrlPanel .run_$octHandle

    if {![info exists ptkRunFlag($name)]} {
	# Assume the window has been deleted already and ignore command
	return
    }

    # Stop the universe if it's running
    if [regexp {^ACTIVE$|^PAUSED$} $ptkRunFlag($name)] {
	ptkAbort $name 
    }
    if [regexp {^STOP_PENDING$|^ABORT$} $ptkRunFlag($name)] {
	# If the universe hasn't stopped already, try again later
	after 200 ptkRunControlDel $name $octHandle $defNumIter
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

    # update the oct facet only if the number of iterations has changed.
    if {$defNumIter != [$ctrlPanel.iter.entry get]} {
         ptkSetRunLength $octHandle [$ctrlPanel.iter.entry get]
    }

    # The global variable ptkControlPanel is the control panel
    # for the currently running universe.  It is possible that
    # another universe is currently running.  Since some tcltk
    # stars' destructors use the variable ptkControlPanel, we
    # temporatily set ptkControlPanel to the control panel of
    # the universe to be deleted so that the destructors can work
    # correctly.  After deleting the universe, we restore 
    # ptkControlPanel to its old value.
    global ptkControlPanel
    set oldCtrlPanel $ptkControlPanel
    set ptkControlPanel $ctrlPanel
    deluniverse $name
    set ptkControlPanel $oldCtrlPanel

    catch {unset ptkDebug($name)}
    catch {unset ptkOctHandles($name)}
    catch {unset ptkRunEventLoop($name)}
    catch {unset ptkRunFlag($name)}
    catch {unset ptkScriptOn($name)}

    catch {destroy $ctrlPanel}
}

#######################################################################
# stop a run early without error
# This amounts to an invasive change of simulation behavior
# and is recommended only for experts
proc ptkEarlyEnd { name } {
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
}

#######################################################################
# a number of places call ptkStop where they really ought to call
# ptkAbort; provide synonym for backwards compatibility
proc ptkStop { name } {
    ptkAbort $name
}

#######################################################################
# procedure to stop a run without running wrapup;
# any run-control-window script will see an error from "run",
# and will be terminated if it doesn't catch the error
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
    abort
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
# ptkCondTime
# Time the given script if and only if the ptkTimeFlag global
# variable is set. The result is displayed.
proc ptkCondTime {script} {
    global ptkTimeFlag
    if $ptkTimeFlag {
	set elapsedTime [time {
	    eval $script
	} 1]
	set timeInSec [expr [lindex $elapsedTime 0]/1000000.0]
	ptkMessage \
	    "The run of [curuniverse] took $timeInSec seconds"
    } {
	eval $script
    }
}

#######################################################################
#procedure to go
proc ptkGo {name octHandle} {
    global ptkRunFlag ptkRunEventLoop ptkControlPanel

    # For now, we allow only one run at a time.
    set univ [curuniverse]
    if {[info exists ptkRunFlag($univ)] && \
	([regexp {^ACTIVE$|^STOP_PENDING$|^ABORT$} $ptkRunFlag($univ)] || \
	($univ != $name && $ptkRunFlag($univ) == {PAUSED})) } {
            ptkImportantMessage .error "Sorry.  Only one run at time. "
	    return
    }

    set ctrlPanel .run_$octHandle

    # Turn the event loop on (or off) for this run 
    ptkSetEventLoop $ptkRunEventLoop($name)

    # arrange to call the routine which updates the
    # iteration count displayed in the debug window.
    # the debug flag controls it being called after 
    # the first time, so this places the control of the
    # update in the ptkUpdateCount routine.
    after 200 ptkUpdateCount $name $octHandle

    # update button relief
    catch {$ctrlPanel.panel.gofr.go configure -relief sunken}
    catch {$ctrlPanel.panel.pause configure -relief raised}

    # So that error highlighting, etc. works
    ptkClearHighlights
    ptkSetHighlightFacet $octHandle

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

    # we were not PAUSED, so we have lots of stuff to do.
    # Make sure the change in button relief is displayed
    # before we start cranking
    update idletasks

    # catch errors and reset the run flag.
    if {[catch {
	# Update any visible parameter entries into the Oct database.
	ptkUpdateVParams $ctrlPanel.vparams $octHandle

        # Allow a target-specific procedure to execute.
	# Again, the correct target may not be set yet.
	set targetToBe [lindex [ptkGetTargetNames $octHandle] 0]
	set targetcust ${targetToBe}-CustomGo
        if {[info proc $targetcust] == $targetcust} {
            $targetcust $name $octHandle $ctrlPanel
        }

        ptkCompile $octHandle

	# OK to set the flag to ACTIVE.  Must not do this before
	# ptkCompile, because ptkCompile looks at the flag to see
	# whether the prior run terminated successfully.
	set ptkRunFlag($name) ACTIVE

	# Set the global indicator of the active control panel
	# (which is used by tcltk stars, for example).  Must not
	# do this before ptkCompile, either, because ptkCompile
	# may cause destruction of an older universe and some
	# tcltk star destructors look at ptkControlPanel.
	set ptkControlPanel $ctrlPanel

	global ptkScriptOn 

	if { $ptkScriptOn($name) } {
	    # we want to run the script. This means we have to
	    # get the contents of the text window, and then get
	    # the TCL interpreter to evaluate it
	    ptkSetStringProp $octHandle script \
	        [$ctrlPanel.tclScript.tframe.text get 0.0 end]
	    ptkCondTime \
		[$ctrlPanel.tclScript.tframe.text get 0.0 end]
	} {
	    # default run: just run through the specified number
	    # of iterations, finishing by invoking
	    # wrapup if no error occurred.
            set numIter [$ctrlPanel.iter.entry get]
	    ptkCondTime "run $numIter; wrapup"
	}

        # we have finished running
	ptkClearRunFlag $name $octHandle
    } msg] == 1} {
	# An error has occurred.
	ptkClearRunFlag $name $octHandle
	# Mark an error
	set ptkRunFlag($name) ERROR
	# Report the error, unless it was just caused by a user abort
	if {$msg != "" && $msg != "Aborted"} {
	    ptkImportantMessage .error $msg
	}
    }
}
