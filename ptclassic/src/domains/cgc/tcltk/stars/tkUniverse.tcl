# Tcl/Tk source for a Ptolemaic solar system
# Demonstration for use with a N-input, zero-output TclScript star.
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
wm title $s "Universe"
wm iconname $s "Universe"

set frameSize 20
set sunRadius 0.75
set earthRadius 0.5
set marsRadius 0.25
set venusRadius 0.25

frame $s.f -bd 10
canvas $s.f.pad -relief sunken -bg black \
	-height ${frameSize}c -width ${frameSize}c
pack append $s.f $s.f.pad top
pack append $s $s.f top

set c $s.f.pad

# Compute initial position and size for the Sun
set x1 [expr ($frameSize/2)-$sunRadius]
set y1 [expr ($frameSize/2)-$sunRadius]
set x2 [expr ($frameSize/2)+$sunRadius]
set y2 [expr ($frameSize/2)+$sunRadius]

set sunId [$c create oval ${x1}c ${y1}c ${x2}c ${y2}c \
	-outline white -fill yellow1 ]

# Compute initial position and size for the Earth
set x1 [expr ($frameSize/2)-$earthRadius]
set y1 [expr ($frameSize/2)-$earthRadius]
set x2 [expr ($frameSize/2)+$earthRadius]
set y2 [expr ($frameSize/2)+$earthRadius]
$c create oval ${x1}c ${y1}c ${x2}c ${y2}c \
        -outline white -fill deepSkyBlue2

# Compute initial position and size for Mars
set x1 [expr ($frameSize/2)-$marsRadius]
set y1 [expr ($frameSize/2)-$marsRadius]
set x2 [expr ($frameSize/2)+$marsRadius]
set y2 [expr ($frameSize/2)+$marsRadius]
set marsId [$c create oval ${x1}c ${y1}c ${x2}c ${y2}c \
	-outline white -fill firebrick ]

# Compute initial position and size for Venus
set x1 [expr ($frameSize/2)-$venusRadius]
set y1 [expr ($frameSize/2)-$venusRadius]
set x2 [expr ($frameSize/2)+$venusRadius]
set y2 [expr ($frameSize/2)+$venusRadius]
set venusId [$c create oval ${x1}c ${y1}c ${x2}c ${y2}c \
	-outline white -fill honeydew2 ]

proc ${uniqueSymbol}callTcl {} {
	global uniqueSymbol
	global sunId
	global marsId
	global venusId
	global sunRadius
	global marsRadius
	global venusRadius
	set s .${uniqueSymbol}field
	set c $s.f.pad
	set inputVals [${uniqueSymbol}grabInputs]

	# Set position of the Sun
	set xin [lindex $inputVals 0]
	set yin [lindex $inputVals 1]
	set x1 [expr {$xin-$sunRadius}]
	set y1 [expr {$yin-$sunRadius}]
	set x2 [expr $x1+2*$sunRadius]
	set y2 [expr $y1+2*$sunRadius]
	$c coords $sunId ${x1}c ${y1}c ${x2}c ${y2}c

	# Set position of Mars
	set xin [lindex $inputVals 2]
	set yin [lindex $inputVals 3]
	set x1 [expr {$xin-$marsRadius}]
	set y1 [expr {$yin-$marsRadius}]
	set x2 [expr $x1+2*$marsRadius]
	set y2 [expr $y1+2*$marsRadius]
	$c coords $marsId ${x1}c ${y1}c ${x2}c ${y2}c

	# Set position of Venus
	set xin [lindex $inputVals 4]
	set yin [lindex $inputVals 5]
	set x1 [expr {$xin-$venusRadius}]
	set y1 [expr {$yin-$venusRadius}]
	set x2 [expr $x1+2*$venusRadius]
	set y2 [expr $y1+2*$venusRadius]
	$c coords $venusId ${x1}c ${y1}c ${x2}c ${y2}c

	after 30
}
