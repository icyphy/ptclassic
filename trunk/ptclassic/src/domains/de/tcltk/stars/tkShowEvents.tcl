# Tcl/Tk source for a display that shows events
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

set s $ptkControlPanel.events_$starID

# If a window with the right name already exists, we assume it was
# created by a previous run of the very same star, and hence can be
# used for this new run.  Some trickiness occurs, however, because
# parameter values may have changed, including the number of inputs.

if {[winfo exists $s]} {
    set window_previously_existed 1
} {
    set window_previously_existed 0
}

if {!$window_previously_existed} {

    proc tkShowEventMakeWindow {putInCntrPan win label numInputs univ starID ids} {
	if {![winfo exists $win]} {
	    global ptkControlPanel
            if {$putInCntrPan} {
	        frame $win
	        pack $win -after $ptkControlPanel.low -fill x -expand 1
            } {
                toplevel $win
                wm title $win "Show Events"
                wm iconname $win "Show Events"
            }

            frame $win.f
	    if {$label != {}} {
		message $win.msg -width 12c
		pack $win.msg -expand 1
	    }
            pack $win.f -fill both -expand 1 -padx 10 -pady 10
	}
	if {$label != {}} {
	    $win.msg configure -text $label
	}

	# The following flag is used if the waitBetweenOutputs parameter is set
	global $starID
	set ${starID}(tkShowEventWaitTrig) 0
        for {set i 0} {$i < $numInputs} {incr i} {
	    catch {destroy $win.f.att$i}
	    frame $win.f.att$i -class Attention
    	    button $win.f.att$i.button -relief raised -width 40 \
		-command "incr ${starID}(tkShowEventWaitTrig)" \
		-state disabled

	    $win.f.att$i.button configure \
		    -text "[lindex $ids $i]: no event yet"

	    pack $win.f.att$i.button -fill x -expand 1
	    pack $win.f.att$i -fill x -expand 1
        }

        if {!$putInCntrPan} {
	    if {![winfo exists $win.ok]} {
                button $win.ok -text "DISMISS" -command \
		    "catch {incr ${starID}(tkShowEventWaitTrig)}
		     ptkStop $univ
		     destroy $win"
                pack $win.ok -fill x -expand 1
	    }
	    # It would be nice to allow user resizing of the window,
	    # but it is totally mysterious in Tk how to do this in
	    # a reasonable way without hardwiring in some guess about
	    # a reasonable minimum size.
        }
	if {[set ${starID}(wait_between_outputs)]} {
	    # Arrange for the tkShowEventWaitTrig variable
	    # to be updated if the run status changes (for example,
	    # a halt is requested)
	    global ptkRunFlag
	    trace variable ptkRunFlag($univ) w tkShowEventReleaseWait
	}
    }
}

proc tkShowEventReleaseWait {runflag univ op} "
	global ${starID}
	incr ${starID}(tkShowEventWaitTrig)
"

tkShowEventMakeWindow [set ${starID}(put_in_control_panel)] \
	$s [set ${starID}(label)] \
	[set ${starID}(numInputs)] [curuniverse] $starID \
	[set ${starID}(identifiers)]

if {!$window_previously_existed} {

    proc tkShowEventSetValues {starID numInputs win ids} {
        set c $win.f
        set inputVals [grabInputs_$starID]
	set inputState [grabInputsState_$starID]
	for {set i 0} {$i < $numInputs} {incr i} {
            set in [lindex $inputVals $i]
	    set st [lindex $inputState $i]
	    if {$st} {
		$win.f.att$i.button configure \
		    -text "[lindex $ids $i]: $in at time [schedtime]" \
		    -background [ptkColor white]
	    } {
		$win.f.att$i.button configure -background [ptkColor burlywood1]
	    }
	}
    }

    proc tkShowEventWait {flag starID numInputs win univ} {
	if {$flag} {
	    global $starID
	    set ${starID}(tkShowEventWaitTrig) 0
	    for {set i 0} {$i < $numInputs} {incr i} {
	        $win.f.att$i.button configure -state normal
	    }
	    tkwait variable ${starID}(tkShowEventWaitTrig)
	    for {set i 0} {$i < $numInputs} {incr i} {
		# In case the window was dismissed, trap errors
	        catch {$win.f.att$i.button configure -state disabled}
	    }
	}
    }
}

# In the following definition, the value of $s
# is evaluated when the file is sourced.
proc goTcl_$starID {starID} "
        tkShowEventSetValues $starID [set ${starID}(numInputs)] $s \
		[list [set ${starID}(identifiers)]]
	tkShowEventWait [set ${starID}(wait_between_outputs)] $starID \
		[set ${starID}(numInputs)] $s [curuniverse]
"

proc destructorTcl_$starID {starID} {
        global $starID
        if {[set ${starID}(put_in_control_panel)]} {
            # Remove the meters from the control panel, if they still exist
            global ptkControlPanel
            destroy $ptkControlPanel.events_$starID
        }
}
unset s
