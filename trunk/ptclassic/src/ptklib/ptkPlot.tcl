# 
#  Tcl interface for creating a plotting utility
# 
#  Author: Wei-Jen Huang and E. A. Lee
#  Version: $Id$
# 
# Copyright (c) 1990-1994 The Regents of the University of California.
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
# 							COPYRIGHTENDKEY

global ptkPlotButtonFlag
set ptkPlotButtonFlag 0

proc ptkPlotPress {w x y} {
    global ptkPlotButtonFlag
    if {$ptkPlotButtonFlag == 0} {
	$w.pf.c create rectangle $x $y $x $y -tag zoomrect -outline #ffffff
	global ptkPlotButtonFlag
	set ptkPlotButtonFlag 1
    } {
	set ptkPlotButtonFlag 0
	ptkPlotMotion $w $x $y
	eval ptkXYPlotResize$w [$w.pf.c coords zoomrect]
	$w.pf.c delete zoomrect
    }
}

proc ptkPlotMotion {w x y} {
    global ptkPlotButtonFlag
    if $ptkPlotButtonFlag {
	set currentcoords [$w.pf.c coords zoomrect]
	if {[llength $currentcoords] == {4}} {
	    $w.pf.c coords zoomrect \
		    [lindex $currentcoords 0] \
		    [lindex $currentcoords 1] \
		    $x $y
	}
    }
}

proc ptkCreateXYPlot {w title geo univ} {
    catch {destroy $w}
    toplevel $w
    wm title $w "TkPlot: $title"
    wm iconname $w "TkPlot"
    wm geometry $w $geo
    # The following allows the window to resize
    wm minsize $w 200 200

    pack [frame $w.mbar -bd 2 -relief raised] -side top -fill x
    pack [button $w.mbar.pr -text "  Print  " -command "ptkPrintXYPlot $w \"$title\""] \
	    -side left
    pack [button $w.mbar.zf -text "zoom fit (f)" -command "ptkXYPlotZoomFit$w"] \
	    -side right
    pack [button $w.mbar.zo -text "zoom out (Z)" -command "ptkXYPlotZoom$w 1.5"] \
	    -side right
    pack [button $w.mbar.zi -text "zoom in (z) " -command "ptkXYPlotZoom$w 0.75"] \
	    -side right
    pack [button $w.mbar.zor -text "zoom original (o)" -command "ptkXYPlotZoomOriginal$w"] \
	    -side right
    pack [frame $w.pf -bd 2m] -side top -fill both -expand yes
    pack [canvas $w.pf.c -relief sunken] -expand yes -fill both
    bind $w.pf <KeyPress-f> "ptkXYPlotZoomFit$w"
    bind $w.pf <KeyPress-z> "ptkXYPlotZoom$w 0.75"
    bind $w.pf <KeyPress-Z> "ptkXYPlotZoom$w 1.5"
    bind $w.pf <KeyPress-o> "ptkXYPlotZoomOriginal$w 1.5"
    bind $w.pf <Control-d> "ptkStop $univ; destroy $w; proc $w.pf.c args \"\""
    bind $w.pf <q> "ptkStop $univ; destroy $w; proc $w.pf.c args \"\""
    bind $w.pf <Enter> "focus $w.pf"

    bind $w.pf.c <ButtonPress> "ptkPlotPress $w %x %y"
    bind $w.pf.c <Motion> "ptkPlotMotion $w %x %y"

    pack [button $w.quit -text DISMISS \
	    -command "ptkStop $univ; destroy $w; proc $w.pf.c args \"\""] \
	    -side bottom -fill x

    # To support redrawing when the window is resized.
    update
    bind $w.pf.c <Configure> "ptkXYPlotRedraw$w"
}

# possible FIXME: generalize ptkPrfacet.tcl routines so that they
# it can be consolidated with this routine.

proc ptkPrintXYPlot {w title} {
    catch "destroy ${w}_print"
    toplevel [set wpr ${w}_print]
    wm title $wpr "Print Plot: $title"
    wm iconname $wpr "Print PtkPlot"

    pack [message $wpr.msg -text "Print PtkPlot" -width 400] -fill both

    #########################################################################
    # Choose portrait or landscape mode
    #
    global ${wpr}Orient
    upvar #0 ${wpr}Orient orient; set orient 1
    frame $wpr.portland -relief groove -bd 3
    radiobutton $wpr.portland.portrait -text "Portrait" \
	-var ${wpr}Orient -relief flat -val 1
    radiobutton $wpr.portland.landscape -text "Landscape" \
	-var ${wpr}Orient -relief flat -val 0
    radiobutton $wpr.portland.larger -text "Larger of the two" \
	-var ${wpr}Orient -relief flat -val 2
    pack $wpr.portland.portrait -side left
    pack $wpr.portland.landscape -side left
    pack $wpr.portland.larger -side right
    pack $wpr.portland -padx 5 -pady 5

    #########################################################################
    # Print/Cancel
    #
    pack [frame $wpr.cntr] -side bottom -fill x -expand 1 -padx 5 -pady 5
    frame $wpr.cntr.prfr -relief sunken -bd 2
    button $wpr.cntr.prfr.print -text "PRINT" -command "ptkPrintXYPlotGo $w; destroy $wpr"
    button $wpr.cntr.cancel -text "CANCEL" -command "destroy $wpr"

    pack $wpr.cntr.prfr.print -fill x -expand 1
    pack $wpr.cntr.prfr -side left -fill x -expand 1
    pack $wpr.cntr.cancel -side right -fill x -expand 1

    #########################################################################
    # Print to file only
    #
    pack [entry $wpr.file -relief ridge -bg wheat3] \
       -side bottom -padx 5 -pady 5 -fill x
    bind $wpr.file <Return> "ptkPrintXYPlotGo $w; destroy $wpr"
    bind $wpr.file <Tab> "focus $wpr.size.b.height"
    $wpr.file insert @0 [pwd]/[lindex $title 0].ps
    # Guess about the number of characters in the window here.
    # Tk returns useless numbers when asked about the width of the widget
    set leftEdge [expr {[string length [pwd]/[lindex $title 0].ps] - 41}]
    if {$leftEdge < 0} {set leftEdge 0}
    $wpr.file xview $leftEdge
    $wpr.file icursor end
    pack [frame $wpr.f] -padx 5 -pady 5 -side bottom -fill x
    pack [checkbutton $wpr.f.on -text "To file only:" \
	-var ${wpr}ToFile -relief flat] -side left -anchor w

    #########################################################################
    # Printer
    #
    frame $wpr.p
    pack [label $wpr.p.plabel -text "Printer: "] \
	 -side left -fill none -anchor nw
    pack [entry $wpr.p.printer -relief ridge -bg wheat3 -width 10] -side right
    bind $wpr.p.printer <Return> "ptkPrintXYPlotGo $w; destroy $wpr"
    bind $wpr.p.printer <Tab> "focus $wpr.file"
    pack $wpr.p -side bottom -anchor w -padx 5 -pady 5
    global env
    if [info exist env(PRINTER)] {
	$wpr.p.printer insert @0 $env(PRINTER)
    } else {
	$wpr.p.printer insert @0 lw
    }

}

proc ptkPrintXYPlotGo w {
	
   global env
   set wpr ${w}_print
   set env(PRINTER) [$wpr.p.printer get]
   set canvWidth [winfo width $w]
   set canvHeight [winfo height $w]

   set psfile [$wpr.file get]
   set command "$w.pf.c postscript -file ${psfile}"

   upvar #0 ${wpr}Orient orient

   set orientation $orient
   if {$orient == 2} {
	if {$canvWidth < $canvHeight} {
		set orientation 1
	} else { set orientation 0 }
   }
   if {$orientation == 0} {
	# landscape
	if [expr {(10.25/$canvWidth)<(7.5/$canvHeight)}] {
		append command " -pagewidth 10.25i -rotate 1"
	} else {
		append command " -pageheight 7.5i -rotate 1"
	}
   } elseif {$orientation == 1} {
	# portrait
	if [expr {(7.5/$canvWidth)<(10.25/$canvHeight)}] {
		append command " -pagewidth 7.5i"
	} else {
		append command " -pageheight 10.25i"
	}
   }

   if {![file exists $psfile]} {
   	eval "$command"
   } else {
	exec mv ${psfile} ${psfile}.old
	eval "$command"
   }

   upvar #0 ${wpr}ToFile toFile
   if {! $toFile} {
	exec lpr -P$env(PRINTER) ${psfile}
	exec /bin/rm ${psfile}
	exec mv ${psfile}.old ${psfile}
   }
}

