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
# When this file is sourced, it is assumed that the following global
# variables have been set:
#	uniqueSymbol
#	numInputs
#	label
#	putInCntrPan
# where the last three are given values corresponding to parameter values.

if {![info exists putInCntrPan]} {set putInCntrPan 1}

if {$putInCntrPan} \
   { set s $ptkControlPanel.low.${uniqueSymbol}label } \
   { set s $ptkControlPanel.${uniqueSymbol}label }


# If a window with the right name already exists, we assume it was
# created by a previous run of the very same star, and hence can be
# used for this new run.

if {![winfo exists $s]} {

    proc tkShowValueMakeWindow {putInCntrPan win label numInputs univ} {
	global ptkControlPanel
        if {$putInCntrPan} {
	    frame $win
	    pack append $ptkControlPanel.low $win top
        } {
            toplevel $win
            wm title $win "Bar Meters"
            wm iconname $win "Bar Meters"
        }

        frame $win.f
        message $win.msg -font -Adobe-times-medium-r-normal--*-180* -width 12c \
	    -text $label

	# The following flag is used if the waitBetweenOutputs parameter is set
	global tkShowValueWaitTrig
	set tkShowValueWaitTrig 0
        for {set i 0} {$i < $numInputs} {incr i} {
    	    button $win.f.m$i -relief raised -width 40 -bg AntiqueWhite \
		-command "incr tkShowValueWaitTrig" 
	    pack append $win.f $win.f.m$i {top frame w pady 4 expand filly}
        }
        pack append $win $win.msg {top expand} $win.f top

        if {!$putInCntrPan} {
            button $win.ok -text "DISMISS" -command \
		"ptkStop $univ; destroy $win"
            pack append $win $win.ok {top fillx}
        }
    }

    tkShowValueMakeWindow $putInCntrPan $s $label $numInputs [curuniverse]

    proc tkShowValueSetValues {uniqueSymbol numInputs win} {
        set c $win.f
        set inputVals [${uniqueSymbol}grabInputs]
	for {set i 0} {$i < $numInputs} {incr i} {
            set in [lindex $inputVals $i]
            $win.f.m$i configure -text $in
	}
        update
    }

    proc tkShowValueWait {flag uniqueSymbol numInputs win} {
	if {$flag} {
	    for {set i 0} {$i < $numInputs} {incr i} {
	        $win.f.m$i configure -bg {orange}
	        $win.f.m$i configure -activebackground {tan3}
	    }
	    global tkShowValueWaitTrig
	    set tkShowValueWaitTrig 0
	    tkwait variable tkShowValueWaitTrig
	    for {set i 0} {$i < $numInputs} {incr i} {
	        $win.f.m$i configure -bg {AntiqueWhite}
	        $win.f.m$i configure -activebackground {tan}
	    }
	}
    }

    # In the following definition, the value of uniqueSymbol and
    # numInputs is evaluated when the file is sourced.
    proc ${uniqueSymbol}callTcl {} "
        tkShowValueSetValues $uniqueSymbol $numInputs $s
	tkShowValueWait $waitBetweenOutputs $uniqueSymbol $numInputs $s
    "
}
