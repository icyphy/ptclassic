# Tcl/Tk source used as the default for the TclScript star in the SDF domain
# This particular demonstration script requires a two input, one output
# instance of this star.
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
# It is assumed that the global symbol ptkControlPanel is the name
# of the control panel used to control the operation of the system.

# The run control window will be the parent window with the following
# choice of name.  The effect of this is that when the control window
# is deleted, so are all the children windows.

set s $ptkControlPanel.field_$starID

# If the window doesn't already exist, create it.
# If the window does exist, assume it was created by a previous run
# of this same star, and hence is configured properly to work as is.
# To check whether it was created by a previous run, we check the
# existence of the variable ballId_$starID.

if {![winfo exists $s] || ![info exists ballId_$starID]} {
    catch {destroy $s}
    toplevel $s
    wm title $s "Playing Field"
    wm iconname $s "Playing Field"

    frame $s.f -bd 10
    canvas $s.f.pad -relief sunken -height 5c -width 10c
    scale $s.f.slider -orient horizontal -from 0 -to 100 \
	-length 10c -command setOut_$starID -showvalue 0
    button $s.f.ok -text "DISMISS" -command "ptkStop [curuniverse]; destroy $s"
    pack append $s.f $s.f.pad top $s.f.slider {top} $s.f.ok {top fillx}
    pack append $s $s.f top

    set c $s.f.pad
    set ballRadius 0.5
    set x1 [expr 5.0-$ballRadius]
    set y1 [expr 2.5-$ballRadius]
    set x2 [expr 5.0+$ballRadius]
    set y2 [expr 2.5+$ballRadius]

    set ballId_$starID [$c create oval ${x1}c ${y1}c ${x2}c ${y2}c \
        -outline [ptkColor black] -fill [ptkColor tan3] ]

    # Conditionally define procedures, only if they haven't been defined before
    proc setOut_$starID {value} "
	setOutputs_$starID \[expr {\$value/20.0}]
    "

    proc goTcl_$starID {starID} {
        global ballId_$starID
	global ptkControlPanel
        set s $ptkControlPanel.field_$starID
        set c $s.f.pad
        set ballRadius 0.5
        set inputVals [grabInputs_$starID]
    	set xin [lindex $inputVals 0]
        set yin [lindex $inputVals 1]
        set x1 [expr {$xin-$ballRadius}]
        set y1 [expr {$yin-$ballRadius}]
        set x2 [expr $x1+2*$ballRadius]
        set y2 [expr $y1+2*$ballRadius]
        after 15
        $c coords [set ballId_$starID] ${x1}c ${y1}c ${x2}c ${y2}c
    }

    # Initialize the output
    setOut_$starID 0
} {
    # If the window previously existed, use the current value of the slider
    setOut_$starID [$s.f.slider get]
}
unset s
