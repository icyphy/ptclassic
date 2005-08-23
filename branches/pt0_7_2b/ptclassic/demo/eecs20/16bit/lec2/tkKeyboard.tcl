# Tcl/Tk source for a pinao keyboard, to be used with the TkSource star
# For use with a three-output, zero-input TclScript star.
#
# Authors: Edward A. Lee and Douglas Niehaus
# Version: @(#)tkKeyboard.tcl	1.6	11/7/95
#
# Copyright (c) 1990-1995 The Regents of the University of California.
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

set ${uniqueSymbol}Frequency {}

set ${uniqueSymbol}Color {}

set ${uniqueSymbol}lastKey 1

proc ptkPianoKeyboardBinding {key keyid uniqueSymbol octave freq color lookupid} {
    global ${uniqueSymbol}Frequency
    global ${uniqueSymbol}DownCmd
    global ${uniqueSymbol}UpCmd
    global ${uniqueSymbol}Color
    lappend ${uniqueSymbol}Frequency [expr $octave*$freq]
    lappend ${uniqueSymbol}Color $color
    set cmd "${uniqueSymbol}setOutputs 1.0 [expr $octave*$freq]; $key itemconfigure $keyid -fill gold"
    $key bind $keyid <ButtonPress-1> $cmd
    set ${uniqueSymbol}DownCmd($lookupid) $cmd
    set cmd "${uniqueSymbol}setOutputs 0.0 0.0; $key itemconfigure $keyid -fill $color;set lastColor \[lindex \[set ${uniqueSymbol}Color \] \[expr  \[set ${uniqueSymbol}lastKey\]-1\]\]; $key itemconfigure \[set ${uniqueSymbol}lastKey\] -fill \$lastColor"
    set ${uniqueSymbol}UpCmd($lookupid) $cmd
    $key bind $keyid <ButtonRelease-1> $cmd
    $key bind $keyid <Button1-Motion> \
	"set lastColor \[lindex \[set ${uniqueSymbol}Color \] \[expr  \[set ${uniqueSymbol}lastKey\]\-1]\]; $key itemconfigure \[set ${uniqueSymbol}lastKey\] -fill \$lastColor ; set newKey \[$key find closest %x %y\]; set newFreq \[lindex \[set ${uniqueSymbol}Frequency \] \[expr \$newKey-1\]\]; set ${uniqueSymbol}lastKey \$newKey; ${uniqueSymbol}setOutputs 1.0 \$newFreq; $key itemconfigure \$newKey -fill gold"

}

# FIXME: There is no error checking in here.
#
proc ptkPianoPlayTune {uniqueSymbol tempo tune} {
    if {[llength $tune] == 0} return
    global ${uniqueSymbol}DownCmd
    global ${uniqueSymbol}UpCmd
    set note [lindex $tune 0]
    set continuation [list ptkPianoPlayTune $uniqueSymbol $tempo [lrange $tune 1 end]]
    if {[lindex $note 0] == {rest}} {
	after [expr int($tempo*[lindex $note 1])] $continuation
    } {
	set id "[lindex $note 0] [lindex $note 1]"
	uplevel #0 [set ${uniqueSymbol}DownCmd($id)]
	after [expr int($tempo*[lindex $note 2])] \
		"[set ${uniqueSymbol}UpCmd($id)]; $continuation"
    }   
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
    set whitekeynames {A B C D E F G}
    set blackkeys { 234.66 277.19 311.12 367.5 412.5 }
    set blackkeynames {A# C# D# F# G#}

    frame $s.f -bd 10
    canvas $s.f.pad -bd 5 -bg AntiqueWhite3 \
	-height 4.5c -width 31.5c
    pack append $s.f $s.f.pad top

    pack append $s $s.f top

    set c $s.f.pad

    # begin with the white keys
    set octposition 0
    set octaveindex 1
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
		[lindex $whitekeys $key] white "$octaveindex [lindex $whitekeynames $key]"
       }
       set octposition [expr $octposition+7]
       incr octaveindex
    }

    # next do the black keys
    set octposition 0
    set octaveindex 1
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
		[lindex $blackkeys $key] black "$octaveindex [lindex $blackkeynames $key]"

	   set x1 [expr $x1+0.75]
	   if {$key == 0} {set x1 [expr $x1+0.75]}
	   if {$key == 2} {set x1 [expr $x1+0.75]}
	   set x2 [expr $x1+0.375]
      }
      set octposition [expr $octposition+7]
      incr octaveindex
   }

   # Tempo is in millisenconds per full note
   set tempo 1250
   set tune {
       {2 G 0.5}
       {3 D 0.5}
       {3 A# 0.75}
       {3 A 0.25}

       {2 G 0.25}
       {3 A# 0.25}
       {3 A 0.25}
       {2 G 0.25}
       {2 F# 0.25}
       {3 A 0.25}
       {2 D 0.5}

       {2 G 0.25}
       {2 D 0.25}
       {3 A 0.25}
       {2 D 0.25}
       {3 A# 0.25}
       {3 A 0.125}
       {2 G 0.125}
       {3 A 0.25}
       {2 D 0.25}
   }
   # Put in a button to play a tune
   pack [button .middle.tune -text {Play Tune} \
	   -command [list ptkPianoPlayTune $uniqueSymbol $tempo $tune]] \
	   -fill x -expand yes -anchor e -pady 5
}

${uniqueSymbol}createWindow

proc ${uniqueSymbol}callTcl {} {
        global uniqueSymbol
        ${uniqueSymbol}setOutputs 0.0 0.0
}






