# Tcl/Tk source for a pinao keyboard, to be used with the TkSource star
# For use with a three-output, zero-input TclScript star.
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

proc ptkPianoKeyboardBinding {key keyid uniqueSymbol octave freq color} {
    $key bind $keyid <ButtonPress-1> \
	"${uniqueSymbol}setOutputs 1.0 [expr $octave*$freq]; $key itemconfigure $keyid -fill gold"
    $key bind $keyid <ButtonRelease-1> \
	"${uniqueSymbol}setOutputs 0.0 0.0; $key itemconfigure $keyid -fill $color"
# The following two commands, regrettably, do not work in the current
# version of Tk.
    $key bind $keyid <Button1-Leave> \
	"puts leave;${uniqueSymbol}setOutputs 0.0 0.0; $key itemconfigure $keyid -fill $color"
    $key bind $keyid <Button1-Enter> \
	"${uniqueSymbol}setOutputs 1.0 [expr $octave*$freq]; $key itemconfigure $keyid -fill gold"
}

proc ${uniqueSymbol}createWindow {} {
    global uniqueSymbol
    set s .${uniqueSymbol}keyboard
    toplevel $s
    wm title $s "Keyboard"
    wm iconname $s "Keyboard"

    # The following frequencies are computed using the circle of fifths.
    # The tempering is accomplished by geometrically distributing the
    # error through all the notes in the scale.
    #
    set whitekeys { 220 246.94 263.41 295.67 327.4 349.23 392.0}
    set blackkeys { 234.66 277.19 311.12 367.5 412.5 }

    frame $s.f -bd 10
    canvas $s.f.pad -bd 5 -bg AntiqueWhite3 \
	-height 4.5c -width 31.5c
    pack append $s.f $s.f.pad top

    pack append $s $s.f top

    set c $s.f.pad

    # begin with the white keys
    set octposition 0
    for {set octave 0.25} {$octave < 16.0} {set octave [expr $octave*2.0]} {
       set y1 0.0
       set y2 4.5
       for {set key 0} {$key < 7} {incr key} {
	    set x1 [expr 0.75*($key+$octposition)]
	    set x2 [expr $x1+0.75]
	    set keyid [$c create rect ${x1}c ${y1}c ${x2}c ${y2}c \
		       -outline black \
		       -fill white ]

	    ptkPianoKeyboardBinding $c $keyid $uniqueSymbol $octave \
		[lindex $whitekeys $key] white
       }
       set octposition [expr $octposition+7]
    }

    # next do the black keys
    set octposition 0
    for {set octave 0.25} {$octave < 16.0} {set octave [expr $octave*2.0]} {
       set x1 [expr 0.5625+0.75*$octposition]
       set y1 0.0
       set x2 [expr $x1+0.375]
       set y2 3.0
       for {set key 0} {$key < 5} {incr key} {
	   set keyid [$c create rect ${x1}c ${y1}c ${x2}c ${y2}c \
		       -outline black \
		       -fill black ]

	   ptkPianoKeyboardBinding $c $keyid $uniqueSymbol $octave \
		[lindex $blackkeys $key] black

	   set x1 [expr $x1+0.75]
	   if {$key == 0} {set x1 [expr $x1+0.75]}
	   if {$key == 2} {set x1 [expr $x1+0.75]}
	   set x2 [expr $x1+0.375]
      }
      set octposition [expr $octposition+7]
   }
}

${uniqueSymbol}createWindow

proc ${uniqueSymbol}callTcl {} {
        global uniqueSymbol
        ${uniqueSymbol}setOutputs 0.0 0.0
}
