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
#	undefined or 0		no active run window, universe not created
#	1			active run window, but no run yet
#	2			active run
#	3			paused run
#	4			finished run

set ptkRunFlag(main) 0

#######################################################################
# Return the ptkRunFlag for a given name, or return zero if none exists.
#
proc ptkGetRunFlag { name } {
    global ptkRunFlag
    if {[info exists ptkRunFlag($name)] && [set temp $ptkRunFlag($name)] > 0} {
	return $temp
    } { return 0 }
}

#######################################################################
# Set the ptkRunFlag to 4, indicating the run is over, and pop up the
# GO button.
#
proc ptkClearRunFlag { name } {
    global ptkRunFlag
    set  cntrWindow .run_$name
    catch {$cntrWindow.panel.go configure -relief raised}
    set ptkRunFlag($name) 4
}

#######################################################################
# Procedure to run a universe.
# The argument is the default number of iterations.
#
proc ptkRunControl { name octHandle } {
    global ptkRunFlag
    set w .run_$name
    if {[ptkGetRunFlag $name] > 0 && [winfo exists $w]} {
            ptkImportantMessage .error \
		"Sorry.  Only one run window for $name at time. "
	    return
    }
    # Mark an open window, but with no run yet.
    set ptkRunFlag($name) 1

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
    if {$ptkRunFlag($name) == 2} "ptkStop $name"
	 set ptkRunFlag($name) 0
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
    # Ignore if the named system is not running
    if {$ptkRunFlag($name) != 2} return
    halt
    # Finish processing the run command
    update
    set cntrWindow .run_$name
    catch {$cntrWindow.panel.go configure -relief raised}
    catch {$cntrWindow.panel.pause configure -relief raised}
    wrapup
    ptkClearRunFlag $name
}

#######################################################################
# procedure to pause a run
proc ptkPause { name } {
    ptkImportantMessage .error {Pause not implemented yet}
#   global ptkRunFlag
#   #Ignore the command if the system is not running
#   if {$ptkRunFlag($name) != 2} return
#   set olduniv [curuniverse]
#   curuniverse $name
#   halt
#   curuniverse $olduniv
#   # Finish processing the run command
#   update
#   set cntrWindow .run_$name
#   catch {$cntrWindow.panel.go configure -relief raised}
#   catch {$cntrWindow.panel.pause configure -relief sunken}
#   set ptkRunFlag($name) 3
}

#######################################################################
#procedure to go
proc ptkGo {name octHandle} {
    set  cntrWindow .run_$name
    set w .run_$name
    set numIter [$w.iter.entry get]
    global ptkRunFlag
    # For now, we allow only one run at a time.
    if {$ptkRunFlag([curuniverse]) == 2} {
        ptkImportantMessage .error \
		"Sorry.  Only one run at time. "
	return
    }
    set prevRunFlag $ptkRunFlag($name)
    set ptkRunFlag($name) 2
    catch {$cntrWindow.panel.go configure -relief sunken}
    catch {$cntrWindow.panel.pause configure -relief raised}
    if {$prevRunFlag != 3} {
        curuniverse $name
        ptkCompile $octHandle
        run $numIter
    } { cont }
    if {$ptkRunFlag($name) != 3} {
	wrapup
	ptkClearRunFlag $name
    }
}
