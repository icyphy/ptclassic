# Tcl/Tk source used as the default for the TclScript star in the SDF domain
# This particular demonstration script requires a two input, one output
# instance of this star.
#
# Author: Edward A. Lee
# Version: $Id$
#
# Copyright (c) 1993 The Regents of the University of California.
# All rights reserved.
# See the file $PTOLEMY/copyright for copyright notice,
# limitation of liability, and disclaimer of warranty provisions.
#
# The TclScript star guarantees that prior to reading this it will have
# set the tcl variable named "uniqueSymbol" to some unique string.
# If this file is used more than once by more than one instance of the
# TkScript star, then each instance will use a different string for
# uniqueSymbol.  So this symbol can be used to create unique names
# for windows.
#
# The following functions are defined by the star and registered as
# Tcl procedures before this script is executed:
# 	${uniqueSymbol}setOutputs
# 	${uniqueSymbol}grabInputs
# The first is used by the script to define the value of the outputs
# of the star.  The second is used to read the values of the inputs.
# The script must also define a procedure:
#	${uniqueSymbol}callTcl
# If the star is set for synchronous operation with Tcl, this procedure
# would normally read the inputs (if any) using ${uniqueSymbol}grab_inputs
# and set the outputs (if any) using ${uniqueSymbol}setOutputs.
# It will be called each time the star fires.
# If the star is set for asynchronous operation, then the procedure
# will be called only once before the main loop begins.
# This default script can be used either way, but it creates much
# less of a drain on the computation time if the star is set to
# operate asynchronously.

set s .${uniqueSymbol}field
catch {destroy $s}
toplevel $s
wm title $s "Playing Field"
wm iconname $s "Playing Field"

frame $s.f -bd 10
canvas $s.f.pad -relief sunken -bg AntiqueWhite3 -height 5c -width 10c
scale $s.f.slider -orient horizontal -from 0 -to 100 -bg tan4 \
	-sliderforeground bisque1 -fg bisque1 -length 10c \
        -command ${uniqueSymbol}setOut -showvalue 0
button $s.f.ok -text "DISMISS" -command "destroy $s"
pack append $s.f $s.f.pad top $s.f.slider {top} $s.f.ok {top fillx}
pack append $s $s.f top

proc ${uniqueSymbol}setOut {value} {
	global uniqueSymbol
	${uniqueSymbol}setOutputs [expr $value/20]
}

# Initialize the output
${uniqueSymbol}setOut 0

set c $s.f.pad
set ballRadius 0.5
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
        set ballRadius 0.5
        set inputVals [${uniqueSymbol}grabInputs]
        set xin [lindex $inputVals 0]
        set yin [lindex $inputVals 1]
        set x1 [expr {$xin-$ballRadius}]
        set y1 [expr {$yin-$ballRadius}]
        set x2 [expr $x1+2*$ballRadius]
        set y2 [expr $y1+2*$ballRadius]
        after 15
        update
        $c coords $ballId ${x1}c ${y1}c ${x2}c ${y2}c
}
