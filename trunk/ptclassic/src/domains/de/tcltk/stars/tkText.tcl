# Tcl/Tk source for a display that shows the values of the star inputs
#
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
# See the file $PTOLEMY/copyright for copyright notice,
# limitation of liability, and disclaimer of warranty provisions.
#

set s $ptkControlPanel.label_$starID

# If a window with the right name already exists, we assume it was
# created by a previous run of the very same star, and hence can be
# used for this new run.

if {![winfo exists $s]} {

    proc ptkTextMakeWindow {win label numInputs univ wait starID} {
        toplevel $win
        wm title $win "Text Display"
        wm iconname $win "Text Display"

        frame $win.f
        message $win.msg -width 12c -text $label

	# shorthand for refering to the star's data structure
	upvar #0 $starID param
	global $starID

        for {set i 0} {$i < $numInputs} {incr i} {
    	    frame $win.f.m$i
	    scrollbar $win.f.m$i.s -relief flat -command "$win.f.m$i.t yview"
    	    text $win.f.m$i.t -relief raised -bd 2 -width 40 -height 10 \
		-yscrollcommand "$win.f.m$i.s set" -setgrid true
	    pack append $win.f $win.f.m$i {top fill expand}
	    pack append $win.f.m$i $win.f.m$i.s {right filly} \
		$win.f.m$i.t {expand fill}
        }
        pack append $win $win.msg {top fill} $win.f {top fill expand}

	set ${starID}(tkTextWaitTrig) 0
	if {$wait} {
	    # The following flag is used if the wait_between_outputs
	    # parameter is set
	    frame $win.att -class Attention
	    button $win.att.cont -relief raised -width 40 \
                -command "incr ${starID}(tkTextWaitTrig)" -text CONTINUE \
		-state disabled
            pack append $win.att $win.att.cont {top fillx}
            pack append $win $win.att {top fillx}
	}
        button $win.ok -text "DISMISS" -command \
		"catch {incr ${starID}(tkTextWaitTrig)}
		 ptkStop $univ
		 destroy $win"
        pack append $win $win.ok {top fillx}

	if {[set ${starID}(wait_between_outputs)]} {
	    # Arrange for the tkTextWaitTrig variable
	    # to be updated if the run status changes (for example,
	    # a halt is requested)
	    global ptkRunFlag
	    trace variable ptkRunFlag($univ) w tkTextReleaseWait
	}
    }

    global $starID

    proc tkTextReleaseWait {runflag univ op} "
	global ${starID}
	incr ${starID}(tkTextWaitTrig)
    "

    ptkTextMakeWindow $s [set ${starID}(label)] [set ${starID}(numInputs)] \
		[curuniverse] [set ${starID}(wait_between_outputs)] $starID

    # The following is needed to avoid a seg fault if an error occurs
    # (Tk bug)
    update

    set ${starID}(lineCount) 0

    # Store the window name in the star data structure
    set ${starID}(win) $s

    proc goTcl_$starID {starID} {

	# Define a shorthand for referring to parameters.
	# Need both notations here.
	upvar #0 $starID param
	global $starID

	set c $param(win).f
	set inputVals [grabInputs_$starID]
	for {set i 0} {$i < $param(numInputs)} {incr i} {
	    set in [lindex $inputVals $i]
	    $param(win).f.m$i.t yview -pickplace end
	    $param(win).f.m$i.t insert end [schedtime]
	    $param(win).f.m$i.t insert end ": "
	    $param(win).f.m$i.t insert end $in
	    $param(win).f.m$i.t insert end "\n"
	}
	incr ${starID}(lineCount)
	if {$param(lineCount) >= $param(number_of_past_values)} {
	    incr ${starID}(lineCount) -1
	    for {set i 0} {$i < $param(numInputs)} {incr i} {
		$param(win).f.m$i.t delete 1.0 2.0
	    }
	}

	if {$param(wait_between_outputs)} {
	    set ${starID}(tkTextWaitTrig) 0
	    $param(win).att.cont configure -state normal
	    tkwait variable ${starID}(tkTextWaitTrig)
	    catch {$param(win).att.cont configure -state disabled}
	}
    }
}
unset s
