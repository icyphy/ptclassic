# Tcl/Tk source for a slider controler
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
#	ptkControlPanel
#	TkSlider_identifier
#	TkSlider_low
#	TkSlider_high
#	TkSlider_value
# where the last three are given values corresponding to parameter values.

if {![info exists putInCntrPan]} {set putInCntrPan 1}

if {$putInCntrPan} \
   { set s $ptkControlPanel.low.${uniqueSymbol}slider } \
   { set s $ptkControlPanel.${uniqueSymbol}slider }

# If a window with the right name already exists, we assume it was
# created by a previous run of the very same star, and hence can be
# used for this new run.

if {![winfo exists $s]} {

    if {$putInCntrPan} {
	frame $s
	pack append $ptkControlPanel.low $s top
    } {
        toplevel $s
        wm title $s "Sliders"
        wm iconname $s "Sliders"
    }

    proc ${uniqueSymbol}setOut {position} "
        ${uniqueSymbol}setOutputs \
	\[expr {$TkSlider_low+($TkSlider_high-$TkSlider_low)*\$position/100.0}]
    "

    frame $s.f
    set position [expr \
	100*($TkSlider_value-$TkSlider_low)/($TkSlider_high-$TkSlider_low)]
    ptkMakeScale $s.f m $TkSlider_identifier $position ${uniqueSymbol}setOut
    pack append $s $s.f top
}

# Initialize the output
${uniqueSymbol}setOutputs $TkSlider_value
