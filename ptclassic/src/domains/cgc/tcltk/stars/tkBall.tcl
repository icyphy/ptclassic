# Tcl/Tk source for a moving ball
# Demonstration for use with a two-input, zero-output TclScript star.
#
# Author: Edward A. Lee
# Version: $Id$
#
# Copyright (c) 1993 The Regents of the University of California.
# All rights reserved.
# See the file $PTOLEMY/copyright for copyright notice,
# limitation of liability, and disclaimer of warranty provisions.
#
# See the documentation of the TclScript star for an explanation of
# how the Tcl/Ptolemy interface works.

set s .${uniqueSymbol}field
toplevel $s
wm title $s "Playing Field"
wm iconname $s "Playing Field"

frame $s.f -bd 10
canvas $s.f.pad -relief sunken -bg AntiqueWhite3 -height 5c -width 10c
pack append $s.f $s.f.pad top
pack append $s $s.f top

set c $s.f.pad
set ballRadius 0.25
set x1 [expr 5.0-$ballRadius]
set y1 [expr 2.5-$ballRadius]
set x2 [expr 5.0+$ballRadius]
set y2 [expr 2.5+$ballRadius]

# FIX ME: The following name should be somehow unique
set ballId [$c create oval ${x1}c ${y1}c ${x2}c ${y2}c \
	-outline white -fill firebrick4 ]

proc ${uniqueSymbol}callTcl {} {
	global uniqueSymbol
	global ballId
	set s .${uniqueSymbol}field
	set c $s.f.pad
	set ballRadius 0.25
	set inputVals [${uniqueSymbol}grabInputs]
	set xin [lindex $inputVals 0]
	set yin [lindex $inputVals 1]
	set x1 [expr {$xin-$ballRadius}]
	set y1 [expr {$yin-$ballRadius}]
	set x2 [expr $x1+2*$ballRadius]
	set y2 [expr $y1+2*$ballRadius]
	$c coords $ballId ${x1}c ${y1}c ${x2}c ${y2}c
}
