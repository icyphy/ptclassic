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

set ptkRunFlag 0

#######################################################################
# Procedure to run a universe.
# The argument is the default number of iterations.
#
proc ptkRunControl { name defNumIter } {
# FIX ME: more than one run control window might be desired at once
    global ptkRunFlag
    if {$ptkRunFlag == 1} {
        ptkImportantMessage .error {Sorry.  Only one run window at time. }
	return
    }
    set ptkRunFlag 1
    set w .runCntrWindow
    catch {destroy $w}
    toplevel $w
    wm title $w "Run $name"
    wm iconname $w "Run $name"

    message $w.msg -font -Adobe-times-medium-r-normal--*-180* -width 25c \
            -text $name -justify center
    frame $w.iter -bd 10
	label $w.iter.label -text "Number of iterations:"
        entry $w.iter.entry -relief sunken -width 20
	$w.iter.entry insert @0 $defNumIter
	bind $w.iter.entry <Return> "ptkGo $w \[$w.iter.entry get]"
	bind $w.iter.entry <Escape> ptkStop
	bind $w.iter.entry <space> ptkPause
	pack append $w.iter $w.iter.entry right $w.iter.label left
    frame $w.panel -bd 10
	button $w.panel.go -text " GO <Return> " \
	    -command "ptkGo $w \[$w.iter.entry get]"
	button $w.panel.pause -text "PAUSE <Space>" -command ptkPause
	button $w.panel.stop -text "STOP <Escape>" -command ptkStop
	pack append $w.panel \
	    $w.panel.go {left expand fill} \
	    $w.panel.pause {left expand fill} \
	    $w.panel.stop {right expand fill}
    button $w.dismiss -text "DISMISS" -command "set ptkRunFlag 0; destroy $w"
    pack append $w \
	$w.msg {top fill expand} \
	$w.iter top \
	$w.panel top \
	$w.dismiss {top fill expand}

    bind $w <Return> "ptkGo $w \[$w.iter.entry get]"
    bind $w <space> ptkPause
    bind $w <Escape> ptkStop

    set ptkRunFlag 1
    wm geometry $w +400+400
    set prevFocus [focus]
    focus $w
# The following two commands would ideally not be necessary, but are now.
#   grab $w
#   tkwait window $w
#   focus $prevFocus
}

#######################################################################
# procedure to stop a run
proc ptkStop {} {
        ptkImportantMessage .error {Stop not implemented yet}
}

#######################################################################
# procedure to pause a run
proc ptkPause {} {
        ptkImportantMessage .error {Pause not implemented yet}
}

#######################################################################
#procedure to go
proc ptkGo {cntrWindow numIter} {
    global ptkRunFlag
    if {$ptkRunFlag == 2} {
        ptkImportantMessage .error {Sorry.  Only one run window at time. }
	return
    }
    set ptkRunFlag 2
    $cntrWindow.panel.go configure -relief sunken
#   $cntrWindow.panel.pause configure -relief raised
    run $numIter
    wrapup
# Need to set the number of iterations in the oct database at this point,
# preferably only if they have changed.  How to do this?
    catch {$cntrWindow.panel.go configure -relief raised}
    set ptkRunFlag 1
}
