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
#	numberOfValues
# where the last three are given values corresponding to parameter values.

set s $ptkControlPanel.${uniqueSymbol}label


# If a window with the right name already exists, we assume it was
# created by a previous run of the very same star, and hence can be
# used for this new run.

if {![winfo exists $s]} {

    proc tkTextMakeWindow {win label numInputs univ wait} {
	global ptkControlPanel
        toplevel $win
        wm title $win "Bar Meters"
        wm iconname $win "Bar Meters"

        frame $win.f
        message $win.msg -font -Adobe-times-medium-r-normal--*-180* -width 12c \
	    -text $label

	# The following flag is used if the waitBetweenOutputs parameter is set
	global tkTextWaitTrig
	set tkTextWaitTrig 0
        for {set i 0} {$i < $numInputs} {incr i} {
    	    frame $win.f.m$i
    	    text $win.f.m$i.t -relief raised -bd 2 -width 40 -height 10 \
		-bg AntiqueWhite \
		-yscrollcommand "$win.f.m$i.s set" -setgrid true
	    scrollbar $win.f.m$i.s -relief flat -command "$win.f.m$i.t yview"
	    pack append $win.f $win.f.m$i {top fill expand}
	    pack append $win.f.m$i $win.f.m$i.s {right filly} \
		$win.f.m$i.t {expand fill}
        }
        pack append $win $win.msg {top fill} $win.f {top fill expand}

	if {$wait} {
	    # The following flag is used if the waitBetweenOutputs
	    # parameter is set
	    global tkTextWaitTrig
	    set tkTextWaitTrig 0
	    button $win.cont -relief raised -width 40 -bg AntiqueWhite \
                -command "incr tkTextWaitTrig" -text CONTINUE
            pack append $win $win.cont {top fillx}
	}
        button $win.ok -text "DISMISS" -command \
		"ptkStop $univ; destroy $win"
        pack append $win $win.ok {top fillx}
    }

    tkTextMakeWindow $s $label $numInputs [curuniverse] $waitBetweenOutputs

    proc tkTextSetValues {uniqueSymbol numInputs win numLines} {
        set c $win.f
        set inputVals [${uniqueSymbol}grabInputs]
	for {set i 0} {$i < $numInputs} {incr i} {
            set in [lindex $inputVals $i]
            $win.f.m$i.t yview -pickplace end
            $win.f.m$i.t insert end $in
            $win.f.m$i.t insert end "\n"
	}
	global ${uniqueSymbol}LineCount
	incr ${uniqueSymbol}LineCount
	if {[set ${uniqueSymbol}LineCount] >= $numLines} {
	    incr ${uniqueSymbol}LineCount -1
	    for {set i 0} {$i < $numInputs} {incr i} {
		$win.f.m$i.t delete 1.0 2.0
	    }
	}
        update
    }

    proc tkTextWait {flag uniqueSymbol numInputs win} {
	if {$flag} {
	    $win.cont configure -bg {orange}
	    $win.cont configure -activebackground {tan3}
	    global tkTextWaitTrig
	    set tkTextWaitTrig 0
	    tkwait variable tkTextWaitTrig
	    $win.cont configure -bg {AntiqueWhite}
	    $win.cont configure -activebackground {tan}
	}
    }

    global ${uniqueSymbol}LineCount
    set ${uniqueSymbol}LineCount 0

    # In the following definition, the value of uniqueSymbol and
    # numInputs is evaluated when the file is sourced.
    proc ${uniqueSymbol}callTcl {} "
        tkTextSetValues $uniqueSymbol $numInputs $s $numberOfValues
	tkTextWait $waitBetweenOutputs $uniqueSymbol $numInputs $s
    "
}
