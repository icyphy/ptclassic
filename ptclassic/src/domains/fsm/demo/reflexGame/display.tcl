# Tcl/Tk source for the displays of the Reflex game,
#
# Author: Edward A. Lee, Bilung Lee
# Version: %W%   %G%
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

set s $ptkControlPanel.reflexGame

if {![winfo exists $s]} {
    toplevel $s
    wm title $s "Reflex Game"
    wm iconname $s "reflexGame"
} else {
    wm deiconify $s
    raise $s
}

set display $s.display
catch {destroy $display}

frame $display
pack $display -side bottom

frame $display.signal -bd 3 -relief groove
frame $display.info -bd 3 -relief groove
pack $display.signal $display.info -side top\
	-padx 3 -pady 3 -expand 1 -fill both

canvas $display.signal.canvas -relief sunken -bg black \
	-height 2.5c -width 5c
pack $display.signal.canvas -side top -expand 1 -fill both

set c $display.signal.canvas

# Indicator for whether the game is active or not.
# Red indicates inactive, and Green indicates active
set onOffId [$c create oval 0.5c 0.5c 1c 1c \
	-outline white -fill red ]
set onOffText [$c create text 1.25c 0.75c -anchor w -fill white \
	-text {Game Over}]

# Indicator that the user is supposed to watch
set indicator [$c create oval 0.5c 1.5c 1c 2c \
	-outline white -fill red]
set indicatorText [$c create text 1.25c 1.75c -anchor w -fill white \
	-text {Push Coin}]

# Display the time elapsed after "Coin" or "Ready" button is pressed,
# or after indicator is green.
set info $display.info
frame $info.timeElapsed -relief ridge
pack $info.timeElapsed -side left
label $info.timeElapsed.label -text "Time elapsed:"
label $info.timeElapsed.value -width 5 -text 0 -relief sunken -bd 3
pack $info.timeElapsed.label $info.timeElapsed.value -side left

proc goTcl_$starID {starID} "
    processInputs_$starID $starID $display
"

set lastTime 0
proc processInputs_$starID {starID w} {
    global onOffId onOffText
    global indicator indicatorText
    global lastTime

    # We only care about whether an event occurred at each input,
    # not about its value.
    set inputVals [grabInputsState_$starID]

    # For clarity, break up the inputs by name
    set redLt       [lindex $inputVals 0]
    set blueLt      [lindex $inputVals 1]
    set yellowLt    [lindex $inputVals 2]
    set greenLt    [lindex $inputVals 3]
    set normal_exit [lindex $inputVals 4]
    set tilt_on     [lindex $inputVals 5]

    if {$redLt} {  ;# redLt means game off.
	$w.signal.canvas itemconfigure $onOffId -fill red
	$w.signal.canvas itemconfigure $onOffText -text {Game Over}
	$w.signal.canvas itemconfigure $indicator -fill red
	$w.signal.canvas itemconfigure $indicatorText -text {Push Coin}
    }
    if {$blueLt} { ;# blueLt means game on.
	$w.info.timeElapsed.value configure -text 0 -bg bisque
	$w.signal.canvas configure -bg black
	$w.signal.canvas itemconfigure $onOffId -fill blue
	$w.signal.canvas itemconfigure $onOffText -text {Game On}
	$w.signal.canvas itemconfigure $indicator -fill red
	$w.signal.canvas itemconfigure $indicatorText -text {Push Ready}
    }
    if {$yellowLt} { ;# yellowLt means player pushed ready.
	$w.signal.canvas itemconfigure $indicator -fill yellow
	$w.signal.canvas itemconfigure $indicatorText -text {Watch Me}
    }
    if {$greenLt} {  ;# greenLt means player can push stop now.
	$w.signal.canvas itemconfigure $indicator -fill green
	$w.signal.canvas itemconfigure $indicatorText -text {Hit Stop NOW!}
	set lastTime [schedtime]
    }
    if {$normal_exit} {
	bell
	$w.info.timeElapsed.value configure\
		-text [expr "[schedtime]-$lastTime"] -bg red
    }
    if {$tilt_on} {
	$w.signal.canvas configure -bg grey
    }
}

tkwait visibility $display

