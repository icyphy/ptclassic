# Tcl/Tk source for a panel of buttons
#
# Authors: Edward A. Lee and Wan-Teh Chang
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

set s $ptkControlPanel.buttons_$starID

# If a window with the right name already exists, we assume it was
# created by a previous run of the very same star, and hence can be
# used for this new run.  Some trickiness occurs, however, because
# parameter values may have changed, including the number of outputs.

if {[winfo exists $s]} {
    set window_previously_existed 1
} {
    set window_previously_existed 0
}

if {!$window_previously_existed} {
    if {[set ${starID}(put_in_control_panel)]} {
	frame $s
	pack $s -after $ptkControlPanel.high -fill x -padx 10
    } {
        toplevel $s
        wm title $s "Buttons"
        wm iconname $s "Buttons"
    }
    frame $s.b
    if {[set ${starID}(label)] != {}} {
	message $s.msg -width 12c
	pack $s.msg -expand 1
    }
}
if {[set ${starID}(label)] != {}} {
    $s.msg configure -text [set ${starID}(label)]
}

# A global variable is monitored as a way to determine when to return from
# the go method if "synchronous" is TRUE.
set tkButtonsWait_$starID 0

# If simultaneous events are allowed, this is called when the
# "PUSH TO PRODUCE EVENTS" button is pressed.
proc ptkButtonsClearButtons {win numOut starID val} {
    for {set i 1} {$i <= $numOut} {incr i} {
	if {[lindex [$win.b.b$i configure -relief] 4] == {sunken}} {
	    setOutput_$starID $i $val
	    $win.b.b$i configure -relief raised
	}
    }
    global tkButtonsWait_$starID
    set tkButtonsWait_$starID 1
}

# If simultaneous events are not allowed, this is called when
# individual event buttons are pressed.
proc setOut_$starID {num} "
    setOutput_$starID \$num [set ${starID}(value)]
    global tkButtonsWait_$starID
    set tkButtonsWait_$starID 1
"

# Create the individual event buttons.
for {set i 1} {$i <= [set ${starID}(numOutputs)]} {incr i} {
    # Don't use ptkMakeButton here because I want more compact spacing.
    set but $s.b.b$i
    catch {destroy $but}
    button $but -text [lindex [set ${starID}(identifiers)] [expr $i-1]]
    pack $but -fill x -expand yes -anchor e

    if {[set ${starID}(allow_simultaneous_events)]} {
	# If simultaneous events are allowed, then we do nothing
	# at the time individual event buttons are pressed.  
	# In particular, leave the pressed button sunken.
	$but configure -command "$but configure -relief sunken"
    } {
	$but configure -command "setOut_$starID $i"
    }
}

if {[set ${starID}(allow_simultaneous_events)]} {
    # Create a button that causes output events to be produced.
    # It is put in a frame with class Attention to get a different color.
    catch {destroy $s.b.sync}
    frame $s.b.sync -class Attention
    ptkMakeButton $s.b.sync b "PUSH TO PRODUCE EVENTS" \
	    "ptkButtonsClearButtons $s [set ${starID}(numOutputs)] \
	    $starID [set ${starID}(value)]"
    pack $s.b.sync -fill x
}

if {!$window_previously_existed} {
    pack $s.b -fill x
    tkwait visibility $s
}

# If "synchronous" is YES, the star's go method does a "tkwait"
# on the variable tkButtonsWait_$starID.  This way, the star's go
# method does not return until tkButtonsWait_$starID is modified.
#
# In order for the STOP and DISMISS buttons in the run control
# panel to abort the "tkwait", we use a kludgy solution.  We do a
# "trace" on the global variable ptkRunFlag($univName), where univName
# is the name of the universe.  Whenever ptkRunFlag($univName) is
# written, the trace modifies tkButtonsWait_$starID in order to abort
# the "tkwait".  This works because callback commands of the STOP and
# DISMISS buttons set ptkRunFlag($univName) to STOP_PENDING and ABORT,
# respectively.

proc tkButtonsTrace_$starID {name1 name2 op} "
    upvar \$name1 x
    if \{\$x(\$name2) == \{ABORT\} ||
            \$x(\$name2) == \{STOP_PENDING\}\} \{
	global tkButtonsWait_$starID
	set tkButtonsWait_$starID 1
    \}
"

if {[set ${starID}(synchronous)]} {
    proc goTcl_$starID {starID} {
	tkwait variable tkButtonsWait_$starID
    }

    # The name of the universe, by convention, is the first component of
    # the full name of the star.  The components are separated by ".".
    set tmp_univName [lindex [split [set ${starID}(fullName)] .] 0]

    # The "trace" command allows a variable to have more than one copy
    # of the same trace.  We add the trace only if it is not already
    # there.
    set tmp_traceList [trace vinfo ptkRunFlag($tmp_univName)]
    if {[lsearch -exact $tmp_traceList "w tkButtonsTrace_$starID"] == -1} {
        trace variable ptkRunFlag($tmp_univName) w tkButtonsTrace_$starID
    }
    unset tmp_univName tmp_traceList
} {
    proc goTcl_$starID {starID} {}
}

proc destructorTcl_$starID {starID} {
    global $starID ptkControlPanel ptkRunFlag

    # Remove the buttons
    destroy $ptkControlPanel.buttons_$starID

    set univName [lindex [split [set ${starID}(fullName)] .] 0]
    trace vdelete ptkRunFlag($univName) w tkButtonsTrace_$starID

    rename tkButtonsTrace_$starID {}
    rename setOut_$starID {}
    rename ptkButtonsClearButtons {}

    global tkButtonsWait_$starID
    unset tkButtonsWait_$starID
}

unset s window_previously_existed
