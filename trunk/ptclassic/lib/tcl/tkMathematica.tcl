# Author: Brian L. Evans, Steve X. Gu, and John Novak
# Version: @(#)tkMathematica.tcl	1.4	3/7/96
#
# Copyright (c) 1990-1996 The Regents of the University of California.
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
#
# Creates a Tk window that evaluates Mathematica commands and displays
# results.  Graphics and sound are displayed in separate windows.
#
# This script assumes that a Tcl command called "mathematica" has
# been registered with Tcl.

# Counter of how many TkMathematica windows exist
set tkMathematica(Count) 0

# Initialize global settings for each TkMathematica window stored
# in an associative array of the form tkMathematica_$name
proc tkMathematicaGlobals {name} {
	upvar #0 tkMathematica_$name nameInfo
	set nameInfo(ExprCount) 0
	set nameInfo(InputExpr) ""
}

# Configure a top-level window
proc tkMathematicaWindow {name} {
	global tkMathematica
	upvar #0 tkMathematica_$name nameInfo

	set w .tkMathematica_Window$name
	if { [info exists $w] && [winfo exists $w] } {
	  raise $w
	} else {
	  incr tkMathematica(Count)
	  tkMathematicaGlobals $name
	  toplevel $w
	  wm title $w "TkMath: Tcl/Tk Interface to Mathematica"
	  wm iconname $w "TkMath"

	  # Create a frame for all buttons
	  frame $w.buttons
	  frame $w.labels

	  # Create all the buttons
	  button $w.buttons.quit \
		-text Quit \
		-command "destroy $w
			  incr tkMathematica(Count) -1
			  unset tkMathematica_$name
			  if { $tkMathematica(Count) == 0 } {
				mathematica end tkMathematica
			  }"

	  button $w.buttons.clearEntry -text "Clear Entry" \
	  	-command "$w.entry delete @0 end"
	  button $w.buttons.clearText -text "Clear History" \
		-command "$w.text2 delete 1.0 end
			  tkMathematicaGlobals $name"

	  # Create a text widget and a scrollbar widget, and bind them together
	  text $w.text2 -relief raised -bd 2 \
		-yscrollcommand "$w.scrollbar set" -setgrid true
	  scrollbar $w.scrollbar -relief flat -command "$w.text2 yview"

	  # Create a label widget and an entry widget
	  label $w.labels.label1 -text "Enter the Mathematica command:"
	  entry $w.entry -width 6 -relief sunken \
		-textvariable tkMathematica_${name}(InputExpr)

	  # Pack everything
	  pack $w.buttons.clearEntry $w.buttons.clearText $w.buttons.quit \
		-side left -expand 1 -padx 5m -pady 2m -fill x
	  pack $w.buttons -side bottom -fill x
	  pack $w.labels.label1 -side left -fill x
	  pack $w.labels $w.entry -side top -padx 3m -pady 0.5m -fill x
	  pack $w.scrollbar -side right -fill y
	  pack $w.text2 -expand yes -fill both

	  focus $w.entry

	  # Bind the entry widget with the action defined in SetEntry
	  bind $w.entry <Return> \
		"incr tkMathematica_${name}(ExprCount)
		 tkMathematicaSend \$tkMathematica_${name}(ExprCount) \
				   \$tkMathematica_${name}(InputExpr) \
				   $w
		 $w.entry delete @0 end"
	}
}

proc tkMathematicaSend {count input window} {
	# Check the connection to Mathematica
	if { [mathematica status] } {
		mathematica start tkMathematica
		if { [mathematica status] } {
			error "Error: Cannot start Mathematica"
			return -code error
		}
	}

	# Display the results
	# FIXME: the call to mathematica eval prevents subsequent
	# code from being executed, so the result is never displayed
	set output [mathematica eval $input]
	tkMathematicaDisplay $count $input $output $window
}

proc tkMathematicaDisplay {count input output window} {
	$window.text2 insert end "In\[$count\]:= $input\n"
	if { "$output" == "" } {
		$window.text2 insert end "\n"
        } else {
		$window.text2 insert end "Out\[$count\]=\n$output\n\n"
	}
        $window.text2 tag add wholeText 1.0 end
        $window.text2 tag configure wholeText -relief raised
}

tkMathematicaWindow PigiConsole
