# VERSION: $Id$
#
#---------------------------------------------------------------------------
# Copyright (c) 1993 The Regents of the University of California.
# All rights reserved.
#
# Permission is hereby granted, without written agreement and without
# license or royalty fees, to use, copy, modify, and distribute this
# software and its documentation for any purpose, provided that the above
# copyright notice and the following two paragraphs appear in all copies
# of this software.
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
#---------------------------------------------------------------------------
#
#  Programmer: Edward A. Lee
#
# This tcl/tk program defines a dialog box for printing facets, and
# formulates the appropriate prfacet command line

#########################################################################
# Initialize the global variables.
# Note that this is done here so that the user's configuration is remembered
# across multiple invocations of the print command
#
set ptkPortrait 1
set ptkPrintToFile 0
set ptkPrHeight 4
set ptkPrWidth 4
set ptkPrVertOffset 0
set ptkPrHorOffset 0
set ptkPrlabels 1
set ptkPrBW 0
set ptkPrPS 1
set ptkPrFileName ""

proc ptkPrintFacet {name} {
    catch {destroy .print}
    toplevel .print
    wm title .print "Print facet"
    wm iconname .print "Print facet"

    pack [message .print.msg -text "Print facet" -width 400] -fill both

    #########################################################################
    # Choose portrait or landscape mode
    #
    frame .print.portland -relief groove -bd 3
    radiobutton .print.portland.portrait -text "Portrait" \
	-variable ptkPortrait -relief flat -value 1
    radiobutton .print.portland.landscape -text "Landscape" \
	-variable ptkPortrait -relief flat -value 0
    radiobutton .print.portland.larger -text "Larger of the two" \
	-variable ptkPortrait -relief flat -value 2
    pack .print.portland.portrait -side left
    pack .print.portland.landscape -side left
    pack .print.portland.larger -side right
    pack .print.portland -padx 5 -pady 5

    #########################################################################
    # Print/Cancel
    #
    pack [frame .print.cntr] -side bottom -fill x -expand 1 -padx 5 -pady 5
    frame .print.cntr.prfr -relief sunken -bd 2
    button .print.cntr.prfr.print -text "PRINT" -command "ptkPrfacet $name"
    button .print.cntr.cancel -text "CANCEL" -command "destroy .print"

    pack .print.cntr.prfr.print -fill x -expand 1
    pack .print.cntr.prfr -side left -fill x -expand 1
    pack .print.cntr.cancel -side right -fill x -expand 1


    #########################################################################
    # Print PS/EPSI
    #
    frame .print.prps -relief groove -bd 3
    radiobutton .print.prps.ps -text "PostScript" -variable ptkPrPS \
	-relief flat -value 1 -command ptkPrfacetPS
    radiobutton .print.prps.epsi -text "EPSI" \
	-variable ptkPrPS -relief flat -value 0 -command ptkPrfacetEPSI
    pack .print.prps.ps -side left
    pack .print.prps.epsi -side right
    pack .print.prps -side bottom -anchor w -padx 5 -pady 5 -fill x

    #########################################################################
    # Print to file only
    #
    pack [entry .print.file -relief sunken -textvariable ptkPrFileName] \
	-side bottom -padx 5 -pady 5 -fill x
    bind .print.file <Return> "ptkPrfacet $name"
    bind .print.file <Tab> "focus .print.size.b.height"
    # Guess about the number of characters in the window here.
    # Tk returns useless numbers when asked about the width of the widget
    set leftEdge [expr {[string length $name] - 41}]
    if {$leftEdge < 0} {set leftEdge 0}
    .print.file view $leftEdge
    .print.file icursor end

    global ptkPrFileName
    set ptkPrFileName ${name}.ps
    pack [frame .print.f] -padx 5 -pady 5 -side bottom -fill x
    pack [checkbutton .print.f.on -text "To file only:" \
	-variable ptkPrintToFile -relief flat ] \
	-side left -anchor w 

    #########################################################################
    # Printer
    #
    frame .print.p
    pack [label .print.p.plabel -text "Printer: "] \
	 -side left -fill none -anchor nw
    pack [entry .print.p.printer -relief sunken -width 10] -side right
    bind .print.p.printer <Return> "ptkPrfacet $name"
    bind .print.p.printer <Tab> "focus .print.file"
    pack .print.p -side bottom -anchor w -padx 5 -pady 5
    global env
    if [catch {set printer $env(PRINTER)}] {set printer lw}
    .print.p.printer insert @0 $printer

    #########################################################################
    # Print BW/Color
    #
    frame .print.prbw -relief groove -bd 3
    radiobutton .print.prbw.bw -text "Black & White" -variable ptkPrBW \
	-relief flat -value 1
    radiobutton .print.prbw.color -text "Color" \
	-variable ptkPrBW -relief flat -value 0
    pack .print.prbw.bw -side left
    pack .print.prbw.color -side right
    pack .print.prbw -side bottom -anchor w -padx 5 -pady 5 -fill x

    #########################################################################
    # Print labels
    #
    frame .print.prlabels -relief groove -bd 3
    radiobutton .print.prlabels.pr -text "Print labels" -variable ptkPrlabels \
	-relief flat -value 1
    radiobutton .print.prlabels.nopr -text "Don't print labels" \
	-variable ptkPrlabels -relief flat -value 0
    pack .print.prlabels.pr -side left
    pack .print.prlabels.nopr -side right
    pack .print.prlabels -side bottom -anchor w -padx 5 -pady 5 -fill x

    #########################################################################
    # Optionally specify the width and/or height
    #
    pack [frame .print.size] -side left
    frame .print.size.b
    frame .print.size.a
    frame .print.size.c
    frame .print.size.d

    # Height
    pack [label .print.size.b.hlabel -text "Max. height (inches):" -width 21] \
	 -side left -fill none -anchor nw
    pack [entry .print.size.b.height -relief sunken -width 10] -side right
    bind .print.size.b.height <Return> "ptkPrfacet $name"
    bind .print.size.b.height <Tab> "focus .print.size.a.width"
    global ptkPrHeight
    .print.size.b.height insert @0 $ptkPrHeight

    # Width
    pack [label .print.size.a.wlabel -text "Max. width (inches): " -width 21] \
	 -side left -anchor w -fill none
    pack [entry .print.size.a.width -relief sunken -width 10] -side right
    bind .print.size.a.width <Return> "ptkPrfacet $name"
    bind .print.size.a.width <Tab> "focus .print.size.c.voffset"
    global ptkPrWidth
    .print.size.a.width insert @0 $ptkPrWidth

    # Vertical Offset
    pack [label .print.size.c.ovlabel -text "Vertical offset:     " -width 21] \
	 -side left -anchor w -fill none
    pack [entry .print.size.c.voffset -relief sunken -width 10] -side right
    bind .print.size.c.voffset <Return> "ptkPrfacet $name"
    bind .print.size.c.voffset <Tab> "focus .print.size.d.hoffset"
    global ptkPrVertOffset
    .print.size.c.voffset insert @0 $ptkPrVertOffset

    # Horizontal Offset
    pack [label .print.size.d.ohlabel -text "Horizontal offset:   " -width 21] \
	 -side left -anchor w -fill none
    pack [entry .print.size.d.hoffset -relief sunken -width 10] -side right
    bind .print.size.d.hoffset <Return> "ptkPrfacet $name"
    bind .print.size.d.hoffset <Tab> "focus .print.p.printer"
    global ptkPrHorOffset
    .print.size.d.hoffset insert @0 $ptkPrHorOffset

    pack .print.size.b .print.size.a .print.size.c .print.size.d -anchor w \
	-expand 1
}

proc ptkPrfacet {name} {

    # Set the printer environment variable
    global env
    set env(PRINTER) [.print.p.printer get]

    # Note that our technology apparently won't support the -C option to
    # generate color postscript code.  It needs to have 8x8 patterns
    set command "prfacet"

    global ptkPrintToFile
    if {$ptkPrintToFile} {
	global ptkPrPS
    	if { $ptkPrPS } { 
            append command " -TOFILE "
	} else {
	    append command " -TOEPSI "
	}
	global ptkPrFileName
	append command $ptkPrFileName
	append command " -L"
    }

    global ptkPortrait
    if {$ptkPortrait == 0} {append command " -l"}
    if {$ptkPortrait == 1} {append command " -p"}
    if {$ptkPortrait == 2} {append command " -a"}

    # Use global variables here so that print configuration is remembered
    # across multiple invocations of the print command
    global ptkPrHeight ptkPrWidth
    set ptkPrHeight [.print.size.b.height get]
    set ptkPrWidth [.print.size.a.width get]
    if {$ptkPrWidth == ""} {set ptkPrWidth $ptkPrHeight}
    if {$ptkPrHeight == ""} {set ptkPrHeight $ptkPrWidth}
    if {$ptkPrWidth != ""} {append command " -b " ${ptkPrWidth}x$ptkPrHeight}

    global ptkPrVertOffset ptkPrHorOffset
    set ptkPrVertOffset [.print.size.c.voffset get]
    set ptkPrHorOffset [.print.size.d.hoffset get]
    append command " -O " ${ptkPrVertOffset}x$ptkPrHorOffset

    global ptkPrlabels
    if {$ptkPrlabels} {append command " -x"}

    global ptkPrBW
    if {! $ptkPrBW} {append command " -C"}

    append command " " $name

    puts $command
    uplevel exec "$command"
    destroy .print
}

proc ptkPrfacetPS {} {
	global ptkPrFileName
	regsub "\.epsi$" $ptkPrFileName ".ps" ptkPrFileName
}

proc ptkPrfacetEPSI {} {
	global ptkPrFileName
	regsub "\.ps$" $ptkPrFileName ".epsi" ptkPrFileName
}


