# Tcl/Tk source for a moving ball
# Demonstration for use with a two-input, zero-output TclScript star.
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
set ballRadius 0.5
set x1 [expr 5.0-$ballRadius]
set y1 [expr 2.5-$ballRadius]
set x2 [expr 5.0+$ballRadius]
set y2 [expr 2.5+$ballRadius]

# FIX ME: The following name should be somehow unique
set ballId [$c create oval ${x1}c ${y1}c ${x2}c ${y2}c \
	-outline tan4 -fill red3 ]

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
#	after 30
#	update
	$c coords $ballId ${x1}c ${y1}c ${x2}c ${y2}c
}
