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

set s $ptkControlPanel.label_$starID

# If a window with the right name already exists, we assume it was
# created by a previous run of the very same star, and hence can be
# used for this new run.

if {![winfo exists $s]} {

    proc tkTextMakeWindow {win label numInputs univ wait starID} {
	global ptkControlPanel
        toplevel $win
        wm title $win "Text Display"
        wm iconname $win "Text Display"

	tkwait visibility $win

        frame $win.f
        message $win.msg -width 12c -text $label

	# The following flag is used if the waitBetweenOutputs parameter is set
	global $starID
	set ${starID}(tkTextWaitTrig) 0
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
	    # The following flag is used if the wait_between_outputs
	    # parameter is set
	    set ${starID}(tkTextWaitTrig) 0
	    button $win.cont -relief raised -width 40 -bg AntiqueWhite \
                -command "incr ${starID}(tkTextWaitTrig)" -text CONTINUE
            pack append $win $win.cont {top fillx}
	}
        button $win.ok -text "DISMISS" -command \
		"ptkStop $univ; destroy $win"
        pack append $win $win.ok {top fillx}
    }

    tkTextMakeWindow $s [set ${starID}(label)] [set ${starID}(numInputs)] \
		[curuniverse] [set ${starID}(wait_between_outputs)] $starID

    proc tkTextSetValues {starID numInputs win numLines} {
        set c $win.f
        set inputVals [grabInputs_$starID]
	for {set i 0} {$i < $numInputs} {incr i} {
            set in [lindex $inputVals $i]
            $win.f.m$i.t yview -pickplace end
            $win.f.m$i.t insert end $in
            $win.f.m$i.t insert end "\n"
	}
	global $starID
	incr ${starID}(lineCount)
	if {[set ${starID}(lineCount)] >= $numLines} {
	    incr ${starID}(lineCount) -1
	    for {set i 0} {$i < $numInputs} {incr i} {
		$win.f.m$i.t delete 1.0 2.0
	    }
	}
        update
    }

    proc tkTextWait {flag starID numInputs win} {
	if {$flag} {
	    $win.cont configure -bg {orange1}
	    $win.cont configure -activebackground {tan3}
	    global $starID
	    set ${starID}(tkTextWaitTrig) 0
	    tkwait variable ${starID}(tkTextWaitTrig)
	    $win.cont configure -bg {AntiqueWhite}
	    $win.cont configure -activebackground {burlywood}
	}
    }

    global $starID
    set ${starID}(lineCount) 0

    # In the following definition, the value of starID and
    # numInputs is evaluated when the file is sourced.
    proc callTcl_$starID {starID} "
        tkTextSetValues $starID [set ${starID}(numInputs)] $s \
		[set ${starID}(number_of_past_values)]
	tkTextWait [set ${starID}(wait_between_outputs)] $starID \
		[set ${starID}(numInputs)] $s
    "
}
unset s
