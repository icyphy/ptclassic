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
# The global array ptkRunFlag($name), indexed by the name of the universe,
# has the following values:
#	undefined		no active run window, universe not created
#	IDLE			active run window, but no run yet
#	ACTIVE			active run
#	PAUSED			paused run
#	FINISHED		finished a run
#	STOP_PENDING		pending stop requested

# Need a dummy value in ptkRunFlag(main) for startup
set ptkRunFlag(main) IDLE

#######################################################################
# Set the ptkRunFlag to IDLE, indicating the run is over, and pop up the
# GO button.
#
proc ptkClearRunFlag { name } {
    global ptkRunFlag
    set  cntrWindow .run_$name
    catch {$cntrWindow.panel.go configure -relief raised}
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
# The argument is the default number of iterations.
#
proc ptkRunControl { name octHandle } {
    global ptkRunFlag
    set w .run_$name
    if {[info exists ptkRunFlag($name)] && [winfo exists $w]} {
            ptkImportantMessage .error \
		"Sorry.  Only one run window for $name at time. "
	    return
    }
    # Mark an open window, but with no run yet.
    set ptkRunFlag($name) IDLE

    catch {destroy $w}
    toplevel $w
    wm title $w "Run $name"
    wm iconname $w "Run $name"

    set defNumIter [ptkGetRunLength $octHandle]

    message $w.msg -font -Adobe-times-medium-r-normal--*-180* -width 25c \
            -text "Control panel for $name" -justify center

    # Define the entry that controls the number of iterations
    frame $w.iter -bd 10
	label $w.iter.label -text "Number of iterations:"
        entry $w.iter.entry -relief sunken -width 10 -bg wheat3
	$w.iter.entry insert @0 $defNumIter
	pack append $w.iter $w.iter.entry right $w.iter.label left

    # The following empty frames are created so that they are available
    # to stars to insert custom controls into the control panel.
    # By convention, we tend to use "high" for entries, "middle"
    # for buttons, and "low" for sliders.  The full name for the
    # frame is .run_${name}.$position, where $name is the name of the
    # universe, and $position is "high", "middle", or "low".
    frame $w.high -bd 10
    frame $w.middle -bd 10
    frame $w.low -bd 10

    # Define the panel of control buttons
    frame $w.panel -bd 10
	button $w.panel.go -text " GO <Return> " \
	    -command "ptkGo $name $octHandle"
		
	button $w.panel.pause -text "PAUSE <Space>" -command "ptkPause $name"
	button $w.panel.stop -text "STOP <Escape>" -command "ptkStop $name"
	pack append $w.panel \
	    $w.panel.go {left expand fill} \
	    $w.panel.pause {left expand fill} \
	    $w.panel.stop {right expand fill}

    button $w.dismiss -text "DISMISS" -command \
	"ptkRunControlDel $name $w $octHandle $defNumIter"

    pack append $w \
	$w.msg {top fill expand} \
	$w.iter top \
	$w.panel top \
	$w.high top \
	$w.middle top \
	$w.low top \
	$w.dismiss {top fill expand}

    wm geometry $w +400+400
    set prevFocus [focus]
    focus $w
    wm protocol $w WM_DELETE_WINDOW \
	"ptkRunControlDel $name $w $octHandle $defNumIter"

    set olduniverse [curuniverse]
    newuniverse $name
    ptkCompile $octHandle
    curuniverse $olduniverse

    bind $w.iter.entry <Return> \
		"ptkGo $name $octHandle"
    bind $w.iter.entry <Escape> "ptkStop $name"
    bind $w.iter.entry <space> "ptkPause $name"
    bind $w <Return> "ptkGo $name $octHandle"
    bind $w <space> "ptkPause $name"
    bind $w <Escape> "ptkStop $name"
    bind $w <Control-d> "ptkRunControlDel $name $w $octHandle $defNumIter"
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
    if {$ptkRunFlag($name) == {ACTIVE} || \
	    $ptkRunFlag($name) == {STOP_PENDING} } {
	ptkImportantMessage .message {System is still running.  Please stop it.}
	return
    }
    unset ptkRunFlag($name)
    # update the oct facet only if the number of iterations has changed.
    if {$defNumIter != [$window.iter.entry get]} {
         ptkSetRunLength $octHandle [$window.iter.entry get]
    }
    deluniverse $name
    destroy $window
}

#######################################################################
# procedure to stop a run
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
    # Finish processing the run command
    update
    set cntrWindow .run_$name
    catch {$cntrWindow.panel.go configure -relief raised}
    catch {$cntrWindow.panel.pause configure -relief raised}
    if {[catch {wrapup} msg] == 1} {
	ptkClearRunFlag $name
	error $msg
    }
    ptkClearRunFlag $name
}

#######################################################################
# procedure to pause a run
proc ptkPause { name } {
    global ptkRunFlag
    #Ignore the command if the system is not running
    if {![info exists ptkRunFlag($name)] || \
    	$ptkRunFlag($name) != {ACTIVE}} return
    set cntrWindow .run_$name
    catch {$cntrWindow.panel.go configure -relief raised}
    catch {$cntrWindow.panel.pause configure -relief sunken}
    set ptkRunFlag($name) PAUSED
    tkwait variable ptkRunFlag($name)
}

#######################################################################
#procedure to go
proc ptkGo {name octHandle} {
    global ptkRunFlag
    # For now, we allow only one run at a time.
    set univ [curuniverse]
    if {[info exists ptkRunFlag($univ)] && \
        ($ptkRunFlag($univ) == {ACTIVE} || \
	 ($univ != $name && $ptkRunFlag($univ) == {PAUSED} ) || \
	 $ptkRunFlag($univ) == {STOP_PENDING})} {
        ptkImportantMessage .error \
		"Sorry.  Only one run at time. "
	return
    }
    set  cntrWindow .run_$name
    catch {$cntrWindow.panel.go configure -relief sunken}
    catch {$cntrWindow.panel.pause configure -relief raised}
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
        set w .run_$name
        set numIter [$w.iter.entry get]
        run $numIter
	wrapup
	ptkClearRunFlag $name
    } msg] == 1} {
	ptkClearRunFlag $name
	error $msg
    }
}
