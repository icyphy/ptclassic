# Definition of a bunch of utility procedures for the tcl/tk Ptolemy interface
# Author: Edward A. Lee
# Version: @(#)ptkBarGraph.tcl	1.9	2/29/96
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


#######################################################################
# Procedure to make an animated bar graph display of an array.
# Arguments:
#	w		toplevel window name to use (with the leading period)
#	desc		description to attach to the window
#	geo		geometry of the window
#	numBars		number of bars in the bar graph
#	barGraphWidth	width in centimeters
#	barGraphHeight	height in centimeters
proc ptkMakeBarGraph {w desc geo numBars barGraphWidth barGraphHeight univ} {
    catch {destroy $w}
    toplevel $w
    wm title $w "$desc"
    wm iconname $w "$desc"

    message $w.msg -width 5i -text "$desc"
    frame $w.cntr -bd 10
    label $w.cntr.label -text "Scale range:"
    set startScale [${w}rescale 1.0]
    label $w.cntr.value -width 20 -text "$startScale"
    button $w.cntr.dbl -text "Zoom Out (Z)" -command "changeBarScale $w 2.0"
    button $w.cntr.hv -text "Zoom In (z)" -command "changeBarScale $w 0.5"
    pack append $w.cntr \
	$w.cntr.label left \
	$w.cntr.value left \
	$w.cntr.dbl right \
	$w.cntr.hv right

    frame $w.pf -bd 10
    canvas $w.pf.plot -relief sunken -bd 3 \
	    -height ${barGraphHeight}c -width ${barGraphWidth}c
    pack append $w.pf $w.pf.plot {top fill expand}

    # bar entry, button and slider section, empty by default
    frame $w.high -bd 2
    frame $w.middle -bd 2
    frame $w.low -bd 2

    button $w.quit -text "DISMISS" -command "ptkStop $univ; destroy $w"
    pack append $w \
	    $w.msg {top fillx } \
	    $w.cntr {top fillx} \
	    $w.quit {bottom fillx } \
	    $w.low {bottom fillx } \
	    $w.middle {bottom fillx } \
	    $w.high {bottom fillx } \
	    $w.pf {top fill expand } 
		

    wm geometry $w $geo
    wm minsize $w 400 200
    tkwait visibility $w.pf.plot
    # Binding to redraw the plot when the window is resized.
    bind $w.pf.plot <Configure> "${w}redraw"

    # ``Accelerators'' for zooming. This requires a focus on mouse entry.
    bind $w.pf.plot <Enter> "focus $w.pf.plot"
    bind $w.pf.plot <KeyPress-z> "changeBarScale $w 0.5"
    bind $w.pf.plot <KeyPress-Z> "changeBarScale $w 2.0"
}

proc changeBarScale {w s} {
    set newScale [${w}rescale $s]
    $w.cntr.value configure -text "$newScale"
    ${w}redraw
}
