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
# Version: $Id$
# Programmer: Matt Tavis
#           : John Reekie
#
#
# KNOWN BUGS:
#
# 3. The whole thing is generallly a mess. The code needs to be
#    migrated into itcl and some new objects abstracted from it:
#    a scrolling canvas would be a good start.
#
# 5. The scroll bar doesn't quite cover the whole chart when the zoom
#    factor is 1.0. Odd....
#
# 6. In the example I tried, the period and busy time stats are the
#    same for actual and best possible (or whatever it is).
#
# 7. Global data is not being deleted when the window is closed.
#    The easiest way to fix this would be to migrate to itcl...
#
# 8. The exclusion flags on redrawing are a massive kludge.
#


#
# Global arrays for keeping a) Gantt chart parameters and b)
# data for individual charts.
# 
# ptkGantt_Parms
# ptkGantt_Data
# ptkGantt_Layout
#

# Processor colors. This needs to be fixed: the colors are ghastly...

set ptkGantt_Parms(colors) \
	{red steelblue limegreen pink green yellow orangered blue \
	 coral tan khaki magenta mediumaquamarine forestgreen sienna}

# Layout parameters, in cm

set ptkGantt_Parms(padtop) 1.0
set ptkGantt_Parms(padbottom) 0.5
set ptkGantt_Parms(padleft) 0.5
set ptkGantt_Parms(padright) 1.0
set ptkGantt_Parms(rulerpad) 0.35

set ptkGantt_Parms(tickLength)  0.35
set ptkGantt_Parms(labelOffset) 0.25

# star labels

set ptkGantt_Parms(labelWidth)  3
# set ptkGantt_Parms(smallFont)  \
# 	-adobe-helvetica-medium-r-normal-*-10-*-*-*-*-*-*-*

# -adobe-courier-medium-r-normal--10-100-75-75-m-60-iso8859-1

# Dummy procedure for exitting Gantt chart
proc ptkGanttExit {} {
}

# Print Gantt chart
proc ptkGantt_PrintChart { chart } {
    global env
    set universe [string trimleft $chart .gantt_]
    if {![info exists env(PRINTER)]} {
	set ptkGantt_Printer "lp"
    } else {
	set ptkGantt_Printer $env(PRINTER)
    }
    if ![winfo exists $chart.print] {
	# create a toplevel window
	toplevel $chart.print
	wm title $chart.print "Print Gantt Chart"
	# top label for window
	message $chart.print.msg -width 10c -pady 2m -padx 1m \
		-text "Print $universe:"
	pack $chart.print.msg -fill x
	# label for entry window
	label $chart.print.label -text "Printer:"
	# create entry widget for printer name
	entry $chart.print.entry -relief sunken -bd 2
	# place default of lp or env var in entry
	$chart.print.entry insert 0 $ptkGantt_Printer
	pack $chart.print.label $chart.print.entry -fill x 
	button $chart.print.ok -text Print -command \
		"ptkGantt_Print $chart {[$chart.print.entry get]}; \
		destroy $chart.print"
	button $chart.print.cancel -text Cancel -command \
		"destroy $chart.print"
	pack $chart.print.ok $chart.print.cancel -side left -fill x \
		-expand 1 -after $chart.print.entry
	# set up binding for return to print
	bind $chart.print.entry <Return> "ptkGantt_Print $chart \
		{[$chart.print.entry get]}; destroy $chart.print"
    }
}

proc ptkGantt_Print { chart printer } {
    set OUTPUT [open "|lpr -P$printer" w]
    puts $OUTPUT [$chart.canvas postscript -colormode gray \
	    -pageheight 10i -pagewidth 8i -rotate 1]
    close $OUTPUT
}


proc ptkGantt_SaveChart { chart } {
    global PTOLEMY

    # create instance of the FileBrowser class
    set filename [::tycho::queryfilename "Save As File:" \
            [string trimleft $chart .gantt_]]

    puts "ptkGantt_SaveChart: $filename"
    if {$filename == {NoName}} {
	error {Cannot use name NoName.}
    }
    # Ignore if the user cancels
    if {$filename != {}} {
	return [ptkGantt_SaveFile $chart $filename]
    }
}

proc ptkGantt_SaveFile {chart filename} {
    global ptkGantt_Layout ptkGantt_Data
    # check to see if file exists
    if {[file exists $filename]} {
	if ![askuser "File \"$filename\" exists.  Overwrite?"] {
	    error {Did not save file.}
	}
    }
    # open file and begin writing
    set FILEOUT [open $filename w]

    # first write out period and # of processors
    puts $FILEOUT "no_processors $ptkGantt_Layout($chart.numprocs)"
    puts $FILEOUT "period $ptkGantt_Layout($chart.period)"

    # throw in an extra return
    puts -nonewline $FILEOUT "\n"

    # now with a foreach loop write each of the processors
    for {set i 1} { $i <= $ptkGantt_Layout($chart.numprocs)} { incr i} {
	# another for loop for the individual stars of the processor
	for {set k [llength $ptkGantt_Data($chart.$i)]} {$k > 0} \
		{incr k -1} {
	    # write a tab, processor name followed by a "{0}"
	    # this bit of info was never used by the original gantt chart
	    set procStarList [lindex $ptkGantt_Data($chart.$i) \
		    [expr $k - 1]]
	    puts -nonewline $FILEOUT "\t[lindex $procStarList 0] {0} "
	    # write start and finish times of star
	    puts $FILEOUT "([lindex $procStarList 1] [lindex \
		    $procStarList 2])"
	}
	puts $FILEOUT "\$end proc\$"
    }
    # place min., percentage and optimum at bottom of file
    puts $FILEOUT "min $ptkGantt_Data($chart.min)"
    puts $FILEOUT "percentage $ptkGantt_Data($chart.prcent)"
    puts $FILEOUT "optimum $ptkGantt_Data($chart.opt)"
    puts $FILEOUT "runtime $ptkGantt_Data($chart.runtime)"

    # close file 
    close $FILEOUT
}

proc ptkGantt_HelpButton {} {
    ptkMessage {
Gantt Chart help:
  - The zoom buttons scale the time axis.
  - Click and drag on the chart to move the cursor.
  - Left and right cursor keys moves the cursor by one
       time unit. Shift-left and shift-right move by ten
       time units.
  - To print, reset the zoom factor to 1 so the entire
       chart is displayed. The processor labels are not
       printed.
  - Use Control-d or Exit from the file menu to close the
       Gantt chart.  It will not be closed by the Run
       control panel.
   }
}


proc ptkGantt_Zoom { universe num_procs period dir } {
    global ptkGantt_Layout

    set chart .gantt_${universe}

    set zoomFactor [$chart.mbar.zoomindex cget -text]

    #
    # prevent zoom out if zoom factor is already 1.
    #
    if {$dir == "out" && $zoomFactor <= 1.0} {
	return
    }

    if {$dir == "in"} {
	set scale 2.0 
    } else {
	set scale 0.5
    }

    set zoomFactor [expr $scale * $zoomFactor]
    $chart.mbar.zoomindex configure -text $zoomFactor

    if { $zoomFactor <= 1.0 } {
	$chart.mbar.zoomout configure -state disabled
    } elseif {$zoomFactor >= 2.0} {
	$chart.mbar.zoomout configure -state normal
    }

    #
    # Delete the ruler and the stars, update layout, and redraw.
    #
    ptkGantt_Redraw $chart \
	    [winfo width  $chart.canvas] \
	    [winfo height $chart.canvas]

    #
    # Force a redraw, to prevent multiple zoom requests .. 
    #
    update idletasks
}


proc ptkGantt_Redraw {chart canvasWidth canvasHeight} {
    global ptkGantt_Layout ptkGantt_Data

    #
    # Redraw only if setup is complete
    #
    if { ! $ptkGantt_Data($chart.okToRedraw) } {
	return
    }
    set ptkGantt_Data($chart.onScreen) 1

    set zoomFactor [$chart.mbar.zoomindex cget -text]

    #
    # Delete the ruler, update layout parameters, and redraw.
    #
    $chart.canvas delete tick label slabel marker
    ptkGantt_UpdateParms \
	    $chart \
	    $canvasWidth \
	    $canvasHeight \
	    $ptkGantt_Layout($chart.period) \
	    $ptkGantt_Layout($chart.numprocs) \
	    $zoomFactor

    ptkGantt_DrawRuler    $chart
    ptkGantt_UpdateChart  $chart
    ptkGantt_DrawMarker   $chart

    ptkGantt_DrawProcLabels $chart

    #
    # Update the scroll bar parameters
    #
    $chart.canvas configure -scrollregion \
	    "0 0 $ptkGantt_Layout($chart.scrollWidth) $canvasHeight"

    ptkGantt_UpdateMarker $chart $ptkGantt_Layout($chart.markerTime)
    ptkGantt_UpdateHighlights $chart -force
}



proc ptkGantt_DrawProc { universe num_procs period proc star_name start fin } {
    global ptkGantt_Data ptkGantt_Parms ptkGantt_Layout

    set chart .gantt_${universe}

    if ![winfo exists $chart.canvas] {

	# first we add the zoom buttons...not done earlier because we need
	# to pass tons of variables not yet set earlier
	button $chart.mbar.zoomin \
		-text    "Zoom In" \
		-command "ptkGantt_Zoom $universe $num_procs $period in"

	button $chart.mbar.zoomout \
		-text    "Zoom Out" \
		-state    disabled \
		-command "ptkGantt_Zoom $universe $num_procs $period out"
	
	label $chart.mbar.zoomlabel -text "  Zoom factor: "
	label $chart.mbar.zoomindex -text "1.0"

	label $chart.mbar.cursorlabel -text "  Cursor: "
	label $chart.mbar.cursorindex -text "0"

	pack    $chart.mbar.zoomin      $chart.mbar.zoomout \
		$chart.mbar.zoomlabel   $chart.mbar.zoomindex \
		$chart.mbar.cursorlabel $chart.mbar.cursorindex \
		-side left

	# Now we create the label for the processors

	canvas $chart.proclabel \
		-width [winfo pixels $chart $ptkGantt_Parms(labelWidth)c] \
		-bg    bisque

	pack $chart.proclabel \
		-side     left \
		-anchor   n -padx 4 -fill y

	#
	# Create the label above the chart now. This prevents
	# messiness with changing canvas size
	# when packing later. The label text is set later by a
	# call from the scheduler.
	#
	label $chart.label -text "  "
	pack  $chart.label -side top


	#
	# add dismiss button
	#
	button $chart.dismiss -relief raised -text "DISMISS" -command \
		"ptkClearHighlights; destroy $chart; ptkGanttExit"
	pack $chart.dismiss -side bottom -fill x


	#
	# Create the scrollbar
	#
	scrollbar $chart.scroll \
		-command "$chart.canvas xview" \
		-orient   horizontal

	pack $chart.scroll -side bottom -fill x -padx 4


	#
	# Create a frame for the canvas. The sunken relief offsets the
	# scrolling region nicely from the background.
	#
	canvas $chart.frame \
		-bg          bisque \
		-borderwidth 2 \
		-relief      sunken
 
	pack $chart.frame \
		-padx 4    -pady   4 \
		-fill both -expand on
 
	# 
	# Create the main canvas and pack it.
	#
	canvas $chart.canvas \
		-bg              bisque \
		-xscrollcommand "$chart.scroll set"

	pack   $chart.canvas \
		-in  $chart.frame \
		-fill both -expand on \
		-padx 4    -pady 4


	#
	# A click anywhere on the canvas moves the marker to that
	# position. Dragging moves the marker as well.
	#
	bind $chart.canvas <Button-1> \
		"ptkGantt_MoveMarker $chart %x"
	
	bind $chart.canvas <B1-Motion> \
		"ptkGantt_MoveMarker $chart %x"


	#
	# When the canvas is resized, redraw the chart.
	#
	bind $chart.canvas <Configure> \
		[concat ptkGantt_Redraw $chart {%w %h}]

        #
        # Initialize the chart data. This is updated each time a new star
	# is added to the chart.
	#
	foreach p [_ptkGantt_interval 1 $num_procs] {
	    set ptkGantt_Data($chart.$p) {}
	}

	#
	# Initialize the chart layout parameters (with bogus canvas
	# sizes: they are set properly when the window is displayed).
	#
	ptkGantt_UpdateParms $chart 600 480 $period $num_procs 1.0


	#
	# Create and initialize the marker
	#
	ptkGantt_DrawMarker $chart -initialize

	#
	# Allow redrawing
	#
	set ptkGantt_Data($chart.okToRedraw) 1
    }

    #
    # Draw a single box at zero co-ordinates, and record
    # its data in a global array.
    #
    # If the chart has already appeared on the screen, then draw
    # a box at the correct position.
    #
    if {$start < $fin} {
	if { $ptkGantt_Data($chart.onScreen) } {
	    lappend ptkGantt_Data($chart.$proc) \
		    [ptkGantt_DrawBox $chart $proc $star_name $start $fin]
	} else {
	    set box [$chart.canvas create rectangle 0 0 0 0 \
		    -fill [lindex $ptkGantt_Parms(colors) [expr $proc - 1]] \
		    -tags [list box star_$star_name p$proc]]
	    
	    lappend ptkGantt_Data($chart.$proc) \
		    [list $star_name $start $fin $box]
	}
    }
}


proc ptkGantt_DrawProcLabels {chart} {
    global ptkGantt_Layout ptkGantt_Parms

    $chart.proclabel delete processor

    set base [expr [winfo y $chart.canvas] - [winfo y $chart.proclabel]]

    set layoutPositions [::tycho::spread $ptkGantt_Layout($chart.numprocs) \
	    [expr $base + $ptkGantt_Layout($chart.graphTop) ]\
	    [expr $base + $ptkGantt_Layout($chart.graphBottom) ]]

#	    -indented

    _ptkGantt_foreach* location $layoutPositions -counter n {
	$chart.proclabel create text \
		[expr   [winfo width $chart.proclabel] / 2] \
		[expr   $location - 8] \
		-text   [expr $n + 1] \
		-anchor  s \
		-tags    processor
    }
}



# Here is the ptkGanttHighlight proc to highlight the stars

proc ptkGantt_Highlight {universe star color} {
    global ptkColorList

    #
    # Figure out the correct color for ptk ???
    #
#    if {[lsearch [array names ptkColorList] $color] == -1} {
#	return "Invalid color specification"
#    }

    set tcl_col [ptkColor $color]
    set red   [format "%d" [format "0x%s" [string range $tcl_col 1 2]]]
    set green [format "%d" [format "0x%s" [string range $tcl_col 3 4]]]
    set blue  [format "%d" [format "0x%s" [string range $tcl_col 5 6]]]

    # these colors are weak in intensity, here we multiply them

    set red   [expr $red   * 256]
    set green [expr $green * 256]
    set blue  [expr $blue  * 256]
	
    #
    # Call ptk to set the star color
    #
    ptkHighlight "$universe.$star" $red $green $blue
}


proc ptkGantt_UpdateHighlights { chart args } {
    global ptkGantt_Parms ptkGantt_Layout ptkGantt_Data

    getflag force args

    #
    # Figure out which stars are under the marker, and
    # if any of them are new.
    #
    set candidates [$chart.canvas find overlapping \
	    $ptkGantt_Layout($chart.markerPosn) \
	    $ptkGantt_Layout($chart.graphTop) \
	    $ptkGantt_Layout($chart.markerPosn) \
	    $ptkGantt_Layout($chart.graphBottom) ]
    set starlist {}
    if { [info exists ptkGantt_Data($chart.highlighted)] } {
	set previous $ptkGantt_Data($chart.highlighted)
    } else {
	set previous {}
    }
    set update off

    foreach star $candidates {
	if { [::tycho::lmember [$chart.canvas gettags $star] "box"] } {
	    lappend starlist $star
	    if { ! [::tycho::lmember $previous $star] } {
		set update on
	    }
	}
    }

    #
    # If any stars have changed, update the display. It would be nice
    # just to update those that have changed, but ptk doesn't support
    # this right now.
    #
    if { $update == "on" || $force } {

	ptkClearHighlights

	#
	# Highlight each star in the new list
	#
	set stardata {}
	set ptkGantt_Data($chart.highlighted) $starlist
	foreach star $starlist {
	    set tags [$chart.canvas gettags $star]

            # 
	    # Figure out the star parameters
	    #
	    set star_name [lindex $tags [lsearch -glob $tags "star_*"]]
	    set star_name [string range $star_name 5 end]
	    set proc [string trimleft \
		    [lindex $tags [lsearch -glob $tags "p*"]] p]

	    #
	    # Highlight the star on the dataflow diagram
	    #
	    set universe [string range $chart 7 end]
	    set color    [lindex $ptkGantt_Parms(colors) [expr $proc - 1]]

	    ptkGantt_Highlight $universe $star_name $color

	    lappend stardata [list $star_name $proc]
	}

	#
	# Redraw the processor and current star labels
	#
	ptkGantt_RedrawLabels $chart $stardata


	#
	# raise the star labels and the marker to make them visible again
	#
#	$chart.canvas raise slabel
	$chart.canvas raise marker

	#
	# Force a display update, so that the marker follows the mouse
	#
	update idletasks
    }
}


proc ptkGantt_RedrawLabels {chart stars} {
    global ptkGantt_Parms ptkGantt_Layout

    $chart.proclabel delete star

    set base [expr [winfo y $chart.canvas] - [winfo y $chart.proclabel]]
    set layoutPositions [::tycho::spread $ptkGantt_Layout($chart.numprocs) \
	    [expr $base + $ptkGantt_Layout($chart.graphTop) ]\
	    [expr $base + $ptkGantt_Layout($chart.graphBottom) ] ]
#	    -indented R
    foreach s $stars {
	_ptkGantt_assign star proc $s

	set location [lindex $layoutPositions [expr $proc-1]]
	$chart.proclabel create text \
		[expr [winfo width $chart.proclabel] / 2] $location \
		-text   $star \
		-anchor  n \
		-font    $ptkGantt_Parms(smallFont) \
		-tags    star
    }
}


proc ptkGantt_MoveMarker { chart x } {
    global ptkGantt_Parms ptkGantt_Layout ptkGantt_Data

    #
    # Get some layout parameters
    #
    set period $ptkGantt_Layout($chart.period)
    set leftEdge $ptkGantt_Layout($chart.leftEdge)
    set rightEdge $ptkGantt_Layout($chart.rightEdge)

    #
    # Convert screen hit into canvas co-ordinates (needed if
    # not whole canvas is displayed).
    #
    set x [$chart.canvas canvasx $x]

    #
    # Calculate the corresponding time, and adjust the canvas
    # position to place it halfway in a time unit.
    #
    set t [::tycho::mapRange $leftEdge $rightEdge [list $x] 0 $period]
    set t [expr int($t)]

    if { $t < 0 } {
	set t 0
    } elseif { $t >= $period } {
	set t [expr $period - 1]
    }

    set x [::tycho::mapRange 0 $period \
            [list [expr $t+0.5]] $leftEdge $rightEdge]

    #
    # Update the cursor position display
    #
    $chart.mbar.cursorindex configure -text $t

    #
    # Move the marker
    #
    $chart.canvas move $ptkGantt_Layout($chart.markerItem) \
	    [expr $x - $ptkGantt_Layout($chart.markerPosn)] 0

    #
    # Update the marker parameters
    #
    set ptkGantt_Layout($chart.markerPosn) $x
    set ptkGantt_Layout($chart.markerTime) $t

    #
    # Update the highlighting
    #
    ptkGantt_UpdateHighlights $chart
}


proc ptkGantt_UpdateMarker { chart t } {
    global ptkGantt_Parms ptkGantt_Layout ptkGantt_Data

    #
    # Get some layout parameters
    #
    set period $ptkGantt_Layout($chart.period)
    set leftEdge $ptkGantt_Layout($chart.leftEdge)
    set rightEdge $ptkGantt_Layout($chart.rightEdge)

    #
    # Get the marker time, and adjust the canvas
    # position to place it halfway in a time unit.
    #
    if { $t < 0 } {
	set t 0
    } elseif { $t >= $period } {
	set t [expr $period - 1]
    }

    set x [::tycho::mapRange 0 $period \
            [list [expr $t+0.5]] $leftEdge $rightEdge]

    #
    # Move the marker
    #
    $chart.canvas move $ptkGantt_Layout($chart.markerItem) \
	    [expr $x - $ptkGantt_Layout($chart.markerPosn)] 0

    #
    # Update the marker parameters
    #
    set ptkGantt_Layout($chart.markerPosn) $x
    set ptkGantt_Layout($chart.markerTime) $t

    $chart.mbar.cursorindex configure -text $t
    $chart.canvas raise marker

    #
    # Now adjust the canvas position so that the marker is in the
    # center of the display.
    #
    set leftoff [expr ($x - $ptkGantt_Layout($chart.displayWidth) / 2) \
	    / $ptkGantt_Layout($chart.scrollWidth)]
    if { $leftoff < 0.0 } {
	set leftoff 0.0
    } elseif {$leftoff > 1.0} {
	set leftoff 1.0
    }

    $chart.canvas xview moveto $leftoff

    #
    # Update highlighting
    #
    ptkGantt_UpdateHighlights $chart
}


proc ptkGantt_MakeLabel {universe period min prcent opt {runtime 0}} {
    global ptkGantt_Data

    set chart .gantt_${universe}
    set prcent [format %.5f $prcent]
    set opt [format %.5f $opt]
    $chart.label configure \
	    -text [format "Period = %d, busy time = %.2f%%" \
	    $period $prcent]

#    $chart.label configure \
	    -text [format "Period = %d (vs. PtlgMin %d), busy time =\
	    %.2f%% (vs. max %.2f%%)" \
	    $period $min $prcent $opt]

    # place all these datum in ptkGantt_Data array
    set ptkGantt_Data($chart.min) $min
    set ptkGantt_Data($chart.prcent) $prcent
    set ptkGantt_Data($chart.opt) $opt
    set ptkGantt_Data($chart.runtime) $runtime
}

proc ptkGantt_Bindings {universe num_procs} {
    # Null procedure: all bindings have been moved to where
    # the widgets are created (ptkGantt_DrawProc and ptkGantt_Display)
}

proc ptkGanttDisplay { universe {inputFile ""} {standalone 0} } {

    global env ptkGantt_Data ptkGantt_Layout ptkGantt_Parms
    global tychoWelcomeWindow tychoConsoleWindow
    global tychoExitWhenNoMoreWindows tychoShouldWeDoRegularExit

    # make sure that tycho is running
    ptkStartTycho
    # set font using tycho font manager
    set ptkGantt_Parms(smallFont) [::tycho::font helvetica 10]

    set chart .gantt_${universe}

    #
    # Disable redrawing until setup is complete, and create window
    #
    set ptkGantt_Data($chart.okToRedraw) 0
    set ptkGantt_Data($chart.onScreen)   0
    toplevel $chart

    # Set a default window size
    wm geometry $chart 840x320

    # Here we have the menu bar.
    frame $chart.mbar -relief raised -bd 2
    pack $chart.mbar -side top -fill x
    menubutton $chart.mbar.file -text "File" -underline 0 -menu \
	    $chart.mbar.file.menu
    pack $chart.mbar.file -side left
    button $chart.mbar.help -relief raised -text "Help" -command \
	    ptkGantt_HelpButton
    pack $chart.mbar.help -side right
    
    menu $chart.mbar.file.menu -tearoff 0
    $chart.mbar.file.menu add command -label "Print Chart..." \
	    -command "ptkGantt_PrintChart $chart"
    if { ! $standalone } {
    $chart.mbar.file.menu add command -label "Save..." \
	    -command "ptkGantt_SaveChart $chart"
    }
    set exitcommand "ptkClearHighlights; destroy $chart; ptkGanttExit"
    $chart.mbar.file.menu add separator
    $chart.mbar.file.menu add command -label "Exit" \
	    -command $exitcommand -accelerator "Ctrl+d"


    # Entering the window focuses on the menu bar.

    bind $chart <Enter> "focus $chart.mbar"

    # Here we set up the bindings
    bind $chart <Enter> "focus $chart.mbar"
    bind $chart.mbar <Control-d> "ptkClearHighlights; destroy $chart"
    
    #
    # Add some bindings so the cursor can be moved with the
    # cursor keys
    #
    bind $chart.mbar <Left> \
	    "ptkGantt_UpdateMarker $chart \
	    \[expr \$ptkGantt_Layout($chart.markerTime) - 1\]"

    bind $chart.mbar <Right> \
	     "ptkGantt_UpdateMarker $chart \
	    \[expr \$ptkGantt_Layout($chart.markerTime) + 1\]"

    bind $chart.mbar <Shift-Left> \
	    "ptkGantt_UpdateMarker $chart \
	    \[expr \$ptkGantt_Layout($chart.markerTime) - 10\]"

    bind $chart.mbar <Shift-Right> \
	    "ptkGantt_UpdateMarker $chart \
	    \[expr \$ptkGantt_Layout($chart.markerTime) + 10\]"

    #
    # If there is a schedule file, open it and parse its contents.
    # Otherwise do nothing, since the scheduler will supply
    # data later.
    #
    if { [file exists $inputFile] } {
	set GFILE_ID [open $inputFile r]
	set proc_num 1
	set NUMCHARS 0
	while { $NUMCHARS >= 0 } {
	    
	    set NUMCHARS [gets $GFILE_ID LINEARR]
	    if (!$NUMCHARS) continue;
	    switch [lindex $LINEARR 0] {
		numberOfProcessors { set ptkGantt_Layout($chart.numprocs)\
			[lindex $LINEARR 1];
			continue}
		period { set ptkGantt_Layout($chart.period) \
			[lindex $LINEARR 1];
			continue}
		min { set ptkGantt_Data($chart.min) \
			[lindex $LINEARR 1];
			continue}
		percentage { set ptkGantt_Data($chart.prcent) \
			[lindex $LINEARR 1];
			continue}
		optimum { set ptkGantt_Data($chart.opt) \
			[lindex $LINEARR 1];
			continue}
		runtime { set ptkGantt_Data($chart.runtime) \
			[lindex $LINEARR 1]l
			continue}
	    	\$end { incr proc_num ; continue}
		default { ptkGantt_DrawProc $universe \
			$ptkGantt_Layout($chart.numprocs) \
			$ptkGantt_Layout($chart.period) \
			$proc_num [lindex $LINEARR 0] [string trimleft \
			[lindex $LINEARR 2] (] [string trimright \
			[lindex $LINEARR 3] )]}
	    }
	}   
	close $GFILE_ID
	ptkGantt_MakeLabel $universe $ptkGantt_Layout($chart.period) \
		$ptkGantt_Data($chart.min) \
		$ptkGantt_Data($chart.prcent) \
		$ptkGantt_Data($chart.opt) \
		$ptkGantt_Data($chart.runtime)
    }
}


proc ptkGantt_UpdateParms {chart canvasWidth canvasHeight period numprocs zoomFactor} {
    global ptkGantt_Parms ptkGantt_Layout

    set leftEdge   [winfo pixels $chart.canvas $ptkGantt_Parms(padleft)c]
    set rightPad   [winfo pixels $chart.canvas $ptkGantt_Parms(padright)c]
    set displayWidth [expr \
	    $canvasWidth \
	    - $rightPad  \
	    - $leftEdge]

    set virtualWidth [expr $zoomFactor * $displayWidth]
    set rightEdge    [expr $leftEdge   + $virtualWidth]
    set scrollWidth  [expr $rightEdge  + $rightPad]

    set labelWidth [gantt_measureLabel $chart.canvas $period]
    set increment  [::tycho::axisIncrement 0 $period $virtualWidth $labelWidth 10]
    if { $increment < 1 } {
	set increment 1
    }
    set axisValues    [::tycho::rangeValues 0 $period $increment]
    set axisLocations [::tycho::mapRange 0 $period $axisValues \
            $leftEdge $rightEdge]

    set rulerBase   [winfo pixels $chart.canvas $ptkGantt_Parms(padtop)c]
    set tickLength  [winfo pixels $chart.canvas $ptkGantt_Parms(tickLength)c]
    set labelOffset [winfo pixels $chart.canvas $ptkGantt_Parms(labelOffset)c]

    set graphTop [winfo pixels $chart.canvas \
	    [expr $ptkGantt_Parms(padtop) + $ptkGantt_Parms(rulerpad)]c]
    set graphBottom [expr $canvasHeight \
	    - [winfo pixels $chart.canvas $ptkGantt_Parms(padbottom)c]]
    set graphHeight [expr $graphBottom - $graphTop]

    set procLocations [::tycho::mapRange 0 $numprocs \
	    [_ptkGantt_interval 0 $numprocs] $graphTop $graphBottom]


    set ptkGantt_Layout($chart.period)        $period
    set ptkGantt_Layout($chart.numprocs)      $numprocs

    set ptkGantt_Layout($chart.leftEdge)      $leftEdge
    set ptkGantt_Layout($chart.rightEdge)     $rightEdge
    set ptkGantt_Layout($chart.scrollWidth)   $scrollWidth

    set ptkGantt_Layout($chart.displayWidth)  $displayWidth

    set ptkGantt_Layout($chart.graphTop)      $graphTop
    set ptkGantt_Layout($chart.graphBottom)   $graphBottom

    set ptkGantt_Layout($chart.axisValues)    $axisValues
    set ptkGantt_Layout($chart.axisLocations) $axisLocations
    set ptkGantt_Layout($chart.rulerBase)     $rulerBase
    set ptkGantt_Layout($chart.tickLength)    $tickLength
    set ptkGantt_Layout($chart.labelOffset)   $labelOffset
    set ptkGantt_Layout($chart.procLocations) $procLocations
 }


proc ptkGantt_DeleteParms {chart} {
    global ptkGantt_Layout

    unset ptkGantt_Layout($chart.period)
    unset ptkGantt_Layout($chart.numprocs)

    unset ptkGantt_Layout($chart.leftEdge)
    unset ptkGantt_Layout($chart.rightEdge)
    unset ptkGantt_Layout($chart.scrollWidth)

    unset ptkGantt_Layout($chart.displayWidth)

    unset ptkGantt_Layout($chart.graphTop)
    unset ptkGantt_Layout($chart.graphBottom)

    unset ptkGantt_Layout($chart.axisValues)
    unset ptkGantt_Layout($chart.axisLocations)
    unset ptkGantt_Layout($chart.rulerBase)
    unset ptkGantt_Layout($chart.tickLength)
    unset ptkGantt_Layout($chart.labelOffset)
    unset ptkGantt_Layout($chart.procLocations)
 }


proc ptkGantt_DrawRuler {chart} {
    global ptkGantt_Layout

    set leftEdge      $ptkGantt_Layout($chart.leftEdge)
    set rightEdge     $ptkGantt_Layout($chart.rightEdge)
 
    set rulerBase     $ptkGantt_Layout($chart.rulerBase)
    set tickLength    $ptkGantt_Layout($chart.tickLength)
    set labelOffset   $ptkGantt_Layout($chart.labelOffset)

    set axisValues    $ptkGantt_Layout($chart.axisValues)
    set axisLocations $ptkGantt_Layout($chart.axisLocations)

    _ptkGantt_foreach* i $axisValues x $axisLocations {
	$chart.canvas create line \
		$x [expr $rulerBase-$tickLength] \
		$x $rulerBase \
		-tags tick

	$chart.canvas create text \
		[expr $x + 6] [expr $rulerBase-$labelOffset] \
		-text  [format {%.0f} $i] \
		-anchor sw \
		-tags   label
    }
    $chart.canvas create line \
	    $leftEdge  $rulerBase \
	    $rightEdge $rulerBase \
	    -tags tick
}


proc ptkGantt_DrawMarker {chart args} {
    global ptkGantt_Layout

    if { [getflag initialize args] } {
	set ptkGantt_Layout($chart.markerTime) 0
	set ptkGantt_Layout($chart.markerPosn) $ptkGantt_Layout($chart.leftEdge)
    }

    set y0 [expr $ptkGantt_Layout($chart.rulerBase) + 1]
    set y2 [expr $ptkGantt_Layout($chart.graphTop) - 1]
    set y3 [expr $ptkGantt_Layout($chart.graphBottom) + 1]
    set d  [expr ($y2 - $y0) / 2.0]
    set y1 [expr $y0 + $d]
    set y4 [expr $y3 + $d]

    set x1 $ptkGantt_Layout($chart.markerPosn)
    set x0 [expr $x1 - $d]
    set x2 [expr $x1 + $d]

    set ptkGantt_Layout($chart.markerPosn) $x1
    set ptkGantt_Layout($chart.markerItem) \
	    [$chart.canvas create polygon \
	        $x1 $y2 $x0 $y1 $x1 $y0 $x2 $y1 \
		$x1 $y2 $x1 $y3 \
		$x2 $y4 $x0 $y4 $x1 $y3 \
		-fill cyan2 -outline cyan3 \
		-tags marker]
}


proc ptkGantt_UpdateChart {chart} {
    global ptkGantt_Data ptkGantt_Layout

    set numprocs $ptkGantt_Layout($chart.numprocs)
    foreach i [_ptkGantt_interval 1 $numprocs] {
	set stars $ptkGantt_Data($chart.$i)
	foreach s $stars {
	    eval ptkGantt_MoveBox $chart $i $s
	}
    }
}


proc ptkGantt_DrawBox {chart proc star_name start fin} {
    global ptkGantt_Parms ptkGantt_Layout

    set box [$chart.canvas create rectangle 0 0 0 0 \
 	    -fill [lindex $ptkGantt_Parms(colors) [expr $proc - 1]] \
 	    -tags [list box star_$star_name p$proc]]

    ptkGantt_MoveBox $chart $proc $star_name $start $fin $box
    $chart.canvas raise $ptkGantt_Layout($chart.markerItem)

    return [list $star_name $start $fin $box]
}


proc ptkGantt_MoveBox {chart proc star_name start fin box} {
    global ptkGantt_Parms ptkGantt_Layout

    set period    $ptkGantt_Layout($chart.period)

    set leftEdge  $ptkGantt_Layout($chart.leftEdge)
    set rightEdge $ptkGantt_Layout($chart.rightEdge)
    _ptkGantt_assign x0 x1 \
	    [::tycho::mapRange 0 $period \
            [list $start $fin] $leftEdge $rightEdge]


    set procLocations $ptkGantt_Layout($chart.procLocations)
    _ptkGantt_assign y0 y1 \
	    [lrange $procLocations [expr $proc-1] $proc]

    $chart.canvas coords $box $x0 $y0 $x1 $y1

    #
    # Draw the star label if it will fit. This could be
    # made better!
    #
    if {[expr [string length $star_name] * 10] \
	    <= [expr $x1 - $x0] } {
	$chart.canvas create text \
		[expr $x0+2] [expr $y0+2] \
		-width [expr $x1 - $x0 - 4] \
		-text  $star_name \
		-font  $ptkGantt_Parms(smallFont) \
		-anchor nw \
		-fill   black \
		-tags  "slabel"
    }
}


#####
# Stuff lifted from John Reekie's tcl libraries. This will be deleted
# from here pretty soon.

proc gantt_measureLabel {canvas value} {
    set t [$canvas create text 0 0 -text [format {%.0f} $value]]
    set width [_ptkGantt_rectWidth [$canvas bbox $t]]
    $canvas delete $t

    return $width
}

#
# _ptkGantt_rectWidth: return the width of a rectangle
#
proc _ptkGantt_rectWidth {rect} {
     return [expr [x1 $rect] - [x0 $rect]]
}



#
# x, y
#
# Extract coordinates from a 2-list
proc x {xy} {
    return [lindex $xy 0]
}

proc y {xy} {
    return [lindex $xy 1]
}


#
# x0, y0, x1, y1
#
# Extract coordinates from a 4-list
#
proc x0 {xy} {
    return [lindex $xy 0]
}

proc y0 {xy} {
    return [lindex $xy 1]
}

proc x1 {xy} {
    return [lindex $xy 2]
}

proc y1 {xy} {
    return [lindex $xy 3]
}



# mapping.tcl
#
# Utility functions for mapping between number ranges, scaling
# intervals, and so on.
#


#
# Given a number, round up or down to the nearest power of two.
#
#proc roundUpTwo {x} {
#    set exp [expr ceil (log($x)/log(2))]
#    set x   [expr pow(2,$exp)]
#}

#proc roundDownTwo {x} {
#    set exp [expr floor (log($x)/log(2))]
#    set x   [expr pow(2,$exp)]
#}



# Given a number, round up to the nearest power of ten
# times 1, 2, or 5.
#
#proc axisRoundUp {x} {
#    set exp [expr floor (log10($x))]
#    set x [expr $x * pow(10, -$exp)]
#
#    if {$x > 5.0} {
#	set x 10.0
#    } elseif {$x > 2.0} {
#	set x 5.0
#    } elseif {$x > 1.0 } {
#	set x 2.0
#    }
#
#    set x [expr $x * pow(10,$exp)]
#    return $x
#}

 
#
# Given a range, space, field width, and padding, figure out how
# the field increment so they will fit.
#
#proc axisIncrement {low high space width padding} {
#    set maxnum   [expr $space / ($width+$padding)]
#    set estimate [expr (double($high) - $low) / ($maxnum)]
#    set estimate [::tycho::axisRoundUp $estimate]
#
#    return $estimate
#}

# 
#
# Given a range and an increment, return the list of numbers
# within that range and on that increment.
#
#proc rangeValues {low high inc} {
#    set result {}
#    set n      1
#
#    set val [roundUpTo $low $inc]
#    while {$val <= $high} {
#	lappend result $val
#	set val [expr $val + $n * $inc]
#    }
#
#    return $result
#}


#
# Given two ranges and a list of numbers in the first range,
# produce the mapping of that list to the second range.
#
#proc mapRange {low high values lowdash highdash} {
#    set result {}
#
#    set scale [expr (double($highdash) - $lowdash) / ($high - $low)]
#foreach n $values {
#lappend result [expr $lowdash + ($n-$low) * $scale]
#}
#
#return $result
#}


#
# Given two numbers, round the first up or down to the
# nearest multiple of the second.
#
#proc roundDownTo {x i} {
#    return [expr $i * (floor($x/$i))]
#}

#proc roundUpTo {x i} {
#    return [expr $i * (ceil($x/$i))]
#}
# 
# 
# 
#
# _ptkGantt_foreach*
#
# Multi-argument version of foreach. Stops when any one of the
# argument lists runs out of elements.
#
#As for loop{},. "body" cannot contain return commands. As
# for loop{}, this could be fixed later if necessary.
#
# Also as for loop{}, the "-counter" option introduces the name
# of a variable that counts from 0 to n-1, where n is the length
# of the shortest argument list.
#
# How does it work? I wish you hadn't asked.... For each variable
# name, it creates a local variable called i0, i1 etc, and binds
# the local variable to the passed variable with upvar{}. For each
# argument list, it creates local variable called l0, l1 etc.
#
# Then, in a frenzy of list heading and tailing, it sets each i(n)
# to the head element of the corresponding l(n), and evaluates the
# body in the caller's context (so that variable names passed to
# this procedure reference the appropriate i(n)). It takes the tail
# of the lists and, provided none are empty, does the same thing again.
#
# This is very inefficient, of course, and should be one of the
# first things recoded in C when things need speeding up.
#
proc _ptkGantt_foreach* {args} {
    set c 0

    set v [readopt counter args]
    if {$v != ""} {
	upvar $v counter
    }

    while {[llength $args] > 2} {
	upvar   [lindex $args 0] i$c
	set l$c [lindex $args 1]

	set args [_ptkGantt_ldrop $args 2]
	incr c
    }

    if {[llength $args] != 1} {
	ptkMessage "error: wrong number of args to _ptkGantt_foreach*"
	return
    }

    set body [lindex $args 0]

    set done    0
    set counter 0
    while { ! $done } {
	for {set i 0} {$i < $c} {incr i} {
	    set i$i [lindex [set l$i] 0]
	}

	uplevel $body

	for {set i 0} {$i < $c} {incr i} {
	    set l$i [_ptkGantt_ltail [set l$i]]
	    if {[_ptkGantt_lnull [set l$i]]} {
		set done 1
	    }
	}

	incr counter
    }
}


#
# loop
#
# Loop $n times. Called as "loop n body." The -counter option
# introduces the name of a variable that ranges from 0 to $n-1.
#
# "body" cannot contain return commands. This could be fixed
# if it ever becomes a problem --- see pg 123 of Ousterhout's
# book.
#
proc loop {args} {
set v [readopt counter args]
if {$v != ""} {
upvar $v counter
}

set n    [lindex $args 0]
set body [lindex $args 1]
for {set counter 0} {$counter < $n} {incr counter} {
uplevel $body
}
}


#
# readopt
#
# Read an option argument, like getopt{}, but instead of setting
# a variable return the read value of the option.
#
# The argument list is modified as for getopt{}.
#
# Example:
#
#    set thing [readopt fred args]
#
# Note that readopt{} does not make getopt{} redundant, since getopt{]
# does not change the option variable if the option is not present.
#
proc readopt {option listname} {

upvar $listname l

set t [lsearch -exact $l -$option]
if { $t != -1 } {
set v [lindex   $l [expr $t+1]]
set l [lreplace $l $t [expr $t+1]]

return $v
}
return ""
}


#
# getflag
#
# Like getopt{}, but set the option variable to 1 if the
# option flag is there, else 0. Delete the option flag 
# from the argument list.
#
proc getflag {option listname} {

upvar $listname l
upvar $option   v

set t [lsearch -exact $l -$option]
if { $t != -1 } {
set v 1
set l [lreplace $l $t $t]

return 1
} else {
set v 0

return 0
}
}


#
# _ptkGantt_assign
#
# Assign elements of a list to multiple variables. Doesn't
# care if the list is longer than the number of variables, ot
# the list is too short. (Should probably at least put an assertion
# in here...)
#
proc _ptkGantt_assign {args} {
    _ptkGantt_foreach* var [_ptkGantt_linit $args] val [_ptkGantt_llast $args] {
	upvar $var v
	set v $val
    }
}


#
# list.itcl
#
# Utility functions on lists. See also syntax.tcl.
#

#proc lhead {list} {
#    return [lindex $list 0]
#}

proc _ptkGantt_ltail {list} {
    return [lreplace $list 0 0]
}

proc _ptkGantt_linit {list} {
    return [_ptkGantt_ltake $list [expr [llength $list] - 1]]
}

proc _ptkGantt_llast {list} {
return [lindex $list [expr [llength $list] - 1]]
}


#
# Test for a null list (or element). Written the way it is
# because a) `==' cannot be used if the list starts with a number and b
# llength is not so good because it traverses the whole list.
#
# The second case checks for the list being null but indicated
# by empty braces. I'm confused as to why I need this...
#
proc _ptkGantt_lnull {list} {
return [expr (! [string match "?*" $list]) \
|| [string match "{}" $list]]
}


#
# Take or drop list elements
#
proc _ptkGantt_ltake {list n} {
return [lrange $list 0 [expr $n - 1]]
}

proc _ptkGantt_ldrop {list n} {
return [lreplace $list 0 [expr $n-1]]
}

#proc ldropUntil {list item} {
#    set index [lsearch -exact $list $item]
#    if { $index == -1 } {
#	return {}
#    } else {
#	return [_ptkGantt_ldrop $list $index]
#    }
#}


#
# Make a list containing n copies of the specified item
#
#proc lcopy {n item} {
#    set result {}
#    loop $n {
#	lappend result $item
#    }
#    return $result
#}


#
# Return list of n integers in the range x to y
# #
proc _ptkGantt_interval {x y} {
     set result {}
 
     while { $x <= $y } {
 	lappend result $x
 	incr x +1
     }
 
     return $result
}



#
# Test whether an element is in a list
#
#proc lmember {list item} {
#    return [expr [lsearch -exact $list $item] != -1]
#}


#
# Remove an item from a list
#
#proc ldelete {list item} {
#    set i [lsearch -exact $list $item]
#
#    if { $i != -1 } {
#        return [lreplace $list $i $i]
#    }
#
#    return $list
#}


#
# Return list of n numbers in the range x to y, but with
# half the interval before the first and last numbers.
#
# This is useful for spacing graphical elements "evenly" along
# a given distance.
#
#proc spread {n x y} {
#    set result {}
#    set i      0
#    set delta  [expr (double($y) - $x) / $n]
#
#    set x [expr $x + $delta / 2]
#    while { $i < $n } {
#        lappend result [expr $x + $i * $delta]
#
#        incr i +1
#    }
#
#    return $result
#}



