# Tcl/Tk source for a display that shows the values of the star inputs
#
# Author: Edward A. Lee
# Version: $Id$
#
# Copyright (c) 1993 The Regents of the University of California.
# All rights reserved.
# See the file $PTOLEMY/copyright for copyright notice,
# limitation of liability, and disclaimer of warranty provisions.
#

if {[set ${starID}(put_in_control_panel)]} \
   { set s $ptkControlPanel.low.label_$starID } \
   { set s $ptkControlPanel.label_$starID }

# If a window with the right name already exists, we assume it was
# created by a previous run of the very same star, and hence can be
# used for this new run.

if {![winfo exists $s]} {

    proc tkShowValueMakeWindow {putInCntrPan win label numInputs univ starID} {
	global ptkControlPanel
        if {$putInCntrPan} {
	    frame $win
	    pack append $ptkControlPanel.low $win top
        } {
            toplevel $win
            wm title $win "Show Values"
            wm iconname $win "Show Values"
        }

        frame $win.f
        message $win.msg -width 12c -text $label

	# The following flag is used if the waitBetweenOutputs parameter is set
	global $starID
	set ${starID}(tkShowValueWaitTrig) 0
        for {set i 0} {$i < $numInputs} {incr i} {
    	    button $win.f.m$i -relief raised -width 40 \
		-command "incr ${starID}(tkShowValueWaitTrig)" 
	    pack append $win.f $win.f.m$i {top frame w pady 4 expand filly}
        }
        pack append $win $win.msg {top expand} $win.f top

        if {!$putInCntrPan} {
            button $win.ok -text "DISMISS" -command \
		"ptkStop $univ; destroy $win"
            pack append $win $win.ok {top fillx}
        }
    }

    tkShowValueMakeWindow [set ${starID}(put_in_control_panel)] \
	$s [set ${starID}(label)] \
	[set ${starID}(numInputs)] [curuniverse] $starID

    proc tkShowValueSetValues {starID numInputs win} {
        set c $win.f
        set inputVals [grabInputs_$starID]
	for {set i 0} {$i < $numInputs} {incr i} {
            set in [lindex $inputVals $i]
            $win.f.m$i configure -text $in
	}
        update
    }

    proc tkShowValueWait {flag starID numInputs win} {
	if {$flag} {
	    # Get original background Colors
	    set origBgList  [$win.f.m0 configure -background]
            set origBgEnd [expr [llength $origBgList] - 1]
	    set origBg [lindex $origBgList $origBgEnd]
	    set origActBgList  [$win.f.m0 configure -activebackground]
            set origActBgEnd [expr [llength $origActBgList] - 1]
	    set origActBg [lindex $origActBgList $origActBgEnd] 

	    for {set i 0} {$i < $numInputs} {incr i} {
	        $win.f.m$i configure -bg [option get . pressMeBg PressMeBg]
	        $win.f.m$i configure \
               -activebackground [option get . pressMeActiveBg PressMeActiveBg]
	    }
	    global $starID
	    set ${starID}(tkShowValueWaitTrig) 0
	    tkwait variable ${starID}(tkShowValueWaitTrig)
	    for {set i 0} {$i < $numInputs} {incr i} {
	        $win.f.m$i configure -bg $origBg
	        $win.f.m$i configure -activebackground $origActBg
	    }
	}
    }

    # In the following definition, the value of starID and
    # numInputs is evaluated when the file is sourced.
    proc callTcl_$starID {starID} "
        tkShowValueSetValues $starID [set ${starID}(numInputs)] $s
	tkShowValueWait [set ${starID}(wait_between_outputs)] $starID \
		[set ${starID}(numInputs)] $s
    "
}
unset s
