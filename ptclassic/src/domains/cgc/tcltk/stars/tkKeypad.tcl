# Tcl/Tk source for a telephone keypad, to be used with the TkSource star
# For use with a two-output, zero-input TclScript star.
#
# Authors: Edward A. Lee and Douglas Niehaus
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

set s .${uniqueSymbol}keypad
toplevel $s
wm title $s "Keypad"
wm iconname $s "Keypad"

set freq1 { 697 697 697 770 770 770 852 852 852 941 941 941 }

set freq2 { 1209 1336 1477 1209 1336 1477 1209 1336 1477 1209 1336 1477 }

frame $s.f -bd 10 -bg [ptkColor LightGray]
canvas $s.f.pad -relief raised -bd 5 -bg [ptkColor CadetBlue3] \
	-height 5.75c -width 4.5c
pack append $s.f $s.f.pad top

pack append $s $s.f top

set c $s.f.pad
set keynum 1
for {set row 0} {$row < 4} {incr row} {
    for {set col 0} {$col < 3} {incr col} {
	set x1 [expr 0.5+1.25*$col]
	set y1 [expr 0.5+1.25*$row]
	set x2 [expr 1.5+1.25*$col]
	set y2 [expr 1.5+1.25*$row]
	set keyid [$c create rect ${x1}c ${y1}c ${x2}c ${y2}c \
		       -outline [ptkColor white] \
		       -fill [ptkColor red3] ]

	set keyind [expr $keynum-1] 
	$c bind $keyid <ButtonPress-1> \
	    "${uniqueSymbol}setOutputs 1.0 [lindex $freq1 $keyind] [lindex $freq2 $keyind]; $c itemconfigure $keyid -fill [ptkColor salmon]"
	$c bind $keyid <ButtonRelease-1> \
	    "${uniqueSymbol}setOutputs 0.0 0.0 0.0; $c itemconfigure $keyid -fill [ptkColor red3] "
	set keytext $keynum
	if {$keynum == 10} {set keytext * }
	if {$keynum == 11} {set keytext 0}
	if {$keynum == 12} {set keytext # }

	set textid [$c create text [expr $x1+0.5]c [expr $y1+0.5]c \
	    -anchor center \
	    -fill [ptkColor white] -text $keytext \
	    -font [option get . mediumfont CGC] ]

	$c bind $textid <ButtonPress-1> \
	    "${uniqueSymbol}setOutputs 1.0 [lindex $freq1 $keyind] [lindex $freq2 $keyind]; $c itemconfigure $keyid -fill [ptkColor salmon]"
	$c bind $textid <ButtonRelease-1> \
	    "${uniqueSymbol}setOutputs 0.0 0.0 0.0; $c itemconfigure $keyid -fill [ptkColor red3]"

	set keynum [expr $keynum+1]
    }
}

proc ${uniqueSymbol}callTcl {} {
	global uniqueSymbol
	${uniqueSymbol}setOutputs 0.0 0.0 0.0
}
