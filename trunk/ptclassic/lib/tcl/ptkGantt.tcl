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
# 1. The star labels do not com up when the chart is first created.
#    This is bcause of the wat the calling interface is set up, and
#    can't be fixed without fixing the call i/f first.
#
# 2. There's some redundant work going on when first displaying
#     and when zooming. Part of this is due to the poor call i/f.
#
# 3. The whole thing is generallly a mess. The code needs to be
#    migrated into itcl and some new objects abstracted from it:
#    a scrolling canvas would be a good start.
#
# 4. There's a hive of procs at the end that belong in library files.
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
set ptkGantt_Parms(smallFont)  \
	-adobe-helvetica-medium-r-normal-*-10-*-*-*-*-*-*-*

# -adobe-courier-medium-r-normal--10-100-75-75-m-60-iso8859-1

# Dummy procedure for exitting Gantt chart
proc ptkGanttExit {} {
}

# Print Gantt chart
proc ptkGantt_PrintChart { chartName } {
    global env
    set universe [string trimleft $chartName .gantt_]
    if {![info exists env(PRINTER)]} {
	set ptkGantt_Printer "lp"
    } else {
	set ptkGantt_Printer $env(PRINTER)
    }
    if ![winfo exists $chartName.print] {
	# create a toplevel window
	toplevel $chartName.print
	wm title $chartName.print "Print Gantt Chart"
	# top label for window
	message $chartName.print.msg -width 10c -pady 2m -padx 1m \
		-text "Print $universe:"
	pack $chartName.print.msg -fill x
	# label for entry window
	label $chartName.print.label -text "Printer:"
	# create entry widget for printer name
	entry $chartName.print.entry -relief sunken -bd 2
	# place default of lp or env var in entry
	$chartName.print.entry insert 0 $ptkGantt_Printer
	pack $chartName.print.label $chartName.print.entry -fill x 
	button $chartName.print.ok -text Print -command \
		"ptkGantt_Print $chartName {[$chartName.print.entry get]}; \
		destroy $chartName.print"
	button $chartName.print.cancel -text Cancel -command \
		"destroy $chartName.print"
	pack $chartName.print.ok $chartName.print.cancel -side left -fill x \
		-expand 1 -after $chartName.print.entry
	# set up binding for return to print
	bind $chartName.print.entry <Return> "ptkGantt_Print $chartName \
		{[$chartName.print.entry get]}; destroy $chartName.print"
    }
}

proc ptkGantt_Print { chartName printer } {
    set OUTPUT [open "|lpr -P$printer" w]
    puts $OUTPUT [${chartName}.canvas postscript -colormode gray \
	    -pageheight 10i -pagewidth 8i -rotate 1]
    close $OUTPUT
}


proc ptkGantt_SaveChart { chartName } {
    global PTOLEMY
    # check to make sure that Tycho.tcl has been sourced
    if {"" == [info classes ::tycho::File]} {
	# this sourcing wants to bring up tycho so we supress console and 
	# welcome windows and ensure that tycho does not exit after browser
	# exits
	set tychoWelcomeWindow 0
	set tychoConsoleWindow 0
	set tychoExitWhenNoMoreWindows 0
	set tychoShouldWeDoRegularExit 0
	source $PTOLEMY/tycho/kernel/Tycho.tcl
    }

    # create instance of the FileBrowser class
    set filename [DialogWindow::newModal FileBrowser [format "%s%s" . [string \
	    trimleft $chartName .gantt_]] -text "Save As File:"]
    if {$filename == {NoName}} {
	error {Cannot use name NoName.}
    }
    # Ignore if the user cancels
    if {$filename != {}} {
	return [ptkGantt_SaveFile $chartName $filename]
    }
}

proc ptkGantt_SaveFile {chartName filename} {
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
    puts $FILEOUT "no_processors $ptkGantt_Layout($chartName.numprocs)"
    puts $FILEOUT "period $ptkGantt_Layout($chartName.period)"

    # throw in an extra return
    puts -nonewline $FILEOUT "\n"

    # now with a foreach loop write each of the processors
    for {set i 1} { $i <= $ptkGantt_Layout($chartName.numprocs)} { incr i} {
	# another for loop for the individual stars of the processor
	for {set k [llength $ptkGantt_Data($chartName.$i)]} {$k > 0} \
		{incr k -1} {
	    # write a tab, processor name followed by a "{0}"
	    # this bit of info was never used by the original gantt chart
	    set procStarList [lindex $ptkGantt_Data($chartName.$i) \
		    [expr $k - 1]]
	    puts -nonewline $FILEOUT "\t[lindex $procStarList 0] {0} "
	    # write start and finish times of star
	    puts $FILEOUT "([lindex $procStarList 1] [lindex \
		    $procStarList 2])"
	}
	puts $FILEOUT "\$end proc\$"
    }
    # place min., percentage and optimum at bottom of file
    puts $FILEOUT "min $ptkGantt_Data($chartName.min)"
    puts $FILEOUT "percentage $ptkGantt_Data($chartName.prcent)"
    puts $FILEOUT "optimum $ptkGantt_Data($chartName.opt)"
    puts $FILEOUT "runtime $ptkGantt_Data($chartName.runtime)"

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


# # This proc was created to facilitate moving both the ruler and chart along
# # the x-axis at the same time.
# proc ptkGantt_DualMove { chartName dir args } {
# puts "ptkGantt_DualMove $chartName $dir $args"
# 
#     set arg_num [llength $args]
#     if { ($arg_num == 1) } {
# 	if { $dir == "x" } {
# 	    ${chartName}.chart.graph xview $args
# 	    ${chartName}.chart.ruler xview $args
# 	} else {
# 	    ${chartName}.chart.graph yview $args
# 	    ${chartName}.proclabel yview $args
# 	}
#     } elseif {$arg_num == 2 } {
# 	if { $dir == "x" } {
# 	    ${chartName}.chart.graph xview moveto [lindex $args 1]
# 	    ${chartName}.chart.ruler xview moveto [lindex $args 1]
# 	} else {
# 	    ${chartName}.chart.graph yview moveto [lindex $args 1]
# 	    ${chartName}.proclabel yview moveto [lindex $args 1]
# 	}
#     } else {
# 	if { $dir == "x" } {
# 	    ${chartName}.chart.graph xview scroll [lindex $args 1] units
# 	    ${chartName}.chart.ruler xview scroll [lindex $args 1] units
# 	} else {
# 	    ${chartName}.chart.graph yview scroll [lindex $args 1] units
# 	    ${chartName}.proclabel yview scroll [lindex $args 1] units
# 	}
#     }
# }

proc ptkGantt_Zoom { universe num_procs period dir } {
    global ptkGantt_Layout

    set chartName .gantt_${universe}

    set zoomFactor [$chartName.mbar.zoomindex cget -text]

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
    $chartName.mbar.zoomindex configure -text $zoomFactor

    if { $zoomFactor <= 1.0 } {
	$chartName.mbar.zoomout configure -state disabled
    } elseif {$zoomFactor >= 2.0} {
	$chartName.mbar.zoomout configure -state normal
    }

    #
    # Delete the ruler and the stars, update layout, and redraw.
    #
    ptkGantt_Redraw $chartName \
	    [winfo width  $chartName.canvas] \
	    [winfo height $chartName.canvas]

    #
    # Force a redraw, to multiple zoom requests .. 
    #
    update idletasks
}


proc ptkGantt_Redraw {chart canvasWidth canvasHeight} {
    global ptkGantt_Layout

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

    set chartName .gantt_${universe}

    if ![winfo exists ${chartName}.canvas] {


	# first we add the zoom buttons...not done earlier because we need
	# to pass tons of variables not yet set earlier
	button $chartName.mbar.zoomin \
		-text    "Zoom In" \
		-command "ptkGantt_Zoom $universe $num_procs $period in"

	button $chartName.mbar.zoomout \
		-text    "Zoom Out" \
		-state    disabled \
		-command "ptkGantt_Zoom $universe $num_procs $period out"
	
	label $chartName.mbar.zoomlabel -text "  Zoom factor: "
	label $chartName.mbar.zoomindex -text "1.0"

	label $chartName.mbar.cursorlabel -text "  Cursor: "
	label $chartName.mbar.cursorindex -text "0"


	pack    $chartName.mbar.zoomin      $chartName.mbar.zoomout \
		$chartName.mbar.zoomlabel   $chartName.mbar.zoomindex \
		$chartName.mbar.cursorlabel $chartName.mbar.cursorindex \
		-side left


	# Now we create the label for the processors

	canvas ${chartName}.proclabel \
		-width [winfo pixels $chartName $ptkGantt_Parms(labelWidth)c] \
		-bg    bisque

	pack ${chartName}.proclabel \
		-side     left \
		-anchor   n -padx 4 -fill y

	#
	# Create the label above the chart now. This prevents
	# messiness with changing canvas size
	# when packing later. The label text is set later by a
	# call from the scheduler.
	#
	label $chartName.label -text "  "
	pack  $chartName.label -side top


	#
	# Create the scrollbar
	#
	scrollbar ${chartName}.scroll \
		-command "$chartName.canvas xview" \
		-orient   horizontal

	pack $chartName.scroll -side bottom -fill x -padx 4


	#
	# Create a frame for the canvas. The sunken relief offsets the
	# scrolling region nicely from the background.
	#
	canvas ${chartName}.frame \
		-bg          bisque \
		-borderwidth 2 \
		-relief      sunken
 
	pack ${chartName}.frame \
		-padx 4    -pady   4 \
		-fill both -expand on
 
	# 
	# Create the main canvas, pack it, and draw it so that we
	# can figure out its size. The scroll region is set a bit
	# later, after we know the size of the canvas.
	#
	canvas $chartName.canvas \
		-bg              bisque \
		-xscrollcommand "${chartName}.scroll set"

	pack   $chartName.canvas \
		-in  $chartName.frame \
		-fill both -expand on \
		-padx 4    -pady 4

	update idletasks

	#
	# Update the chart layout parameters, and draw the ruler. The chart
	# is drawn one box at a time later on...
	#
	ptkGantt_UpdateParms $chartName \
		[winfo width $chartName.canvas] [winfo height $chartName.canvas] \
		$period $num_procs 1.0

	ptkGantt_DrawProcLabels $chartName
	ptkGantt_DrawRuler $chartName

	#
	# Set the scroll bar region
	#
	$chartName.canvas configure -scrollregion \
		"0 0 $ptkGantt_Layout($chartName.scrollWidth) 0"

        #
        # Initialize the chart data. This is updated each time a new star
	# is added to the chart.
	#
	foreach p [interval 1 $num_procs] {
	    set ptkGantt_Data($chartName.$p) {}
	}

	#
	# Draw the "marker" control
	#
	ptkGantt_DrawMarker $chartName -initialize
		
	#
	# Force it to the left hand edge. Unfortunately, the stars
	# won't be highlighted because they don't exist yet. The calling
	# interface to the Gantt chart is badly in need of an overhaul...
	#
	ptkGantt_MoveMarker $chartName 0

	# add dismiss button
	button $chartName.dismiss -relief raised -text "DISMISS" -command \
		"ptkClearHighlights; destroy $chartName; ptkGanttExit"
	pack $chartName.dismiss -before $chartName.scroll -side bottom -fill x

    }

    #
    # Draw a single box, and record its data in a global array.
    #
    if {$start < $fin} {
	lappend ptkGantt_Data($chartName.$proc) \
		[ptkGantt_DrawBox $chartName $proc $star_name $start $fin]
    }
}


proc ptkGantt_DrawProcLabels {chart} {
    global ptkGantt_Layout ptkGantt_Parms

    $chart.proclabel delete processor

    set base [expr [winfo y $chart.canvas] - [winfo y $chart.proclabel]]

    set layoutPositions [spread $ptkGantt_Layout($chart.numprocs) \
	    [expr $base + $ptkGantt_Layout($chart.graphTop) ]\
	    [expr $base + $ptkGantt_Layout($chart.graphBottom) ]]

    foreach* location $layoutPositions -counter n {
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
	if { [lmember [$chart.canvas gettags $star] "box"] } {
	    lappend starlist $star
	    if { ! [lmember $previous $star] } {
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

	#
	# Unhighlight highlighted stars
	#
#	set highlighted [$chart.canvas find withtag highlight]
#	foreach box $highlighted {
#	    $chart.canvas itemconfigure $box -width 1
#	}
#	$chart.canvas dtag highlight
	ptkClearHighlights

	#
	# Highlight each star in the new list
	#
	set stardata {}
	set ptkGantt_Data($chart.highlighted) $starlist
	foreach star $starlist {
	    set tags [$chart.canvas gettags $star]

	    #
	    # Highlight the star by thickening the border
	    #
#	    $chart.canvas itemconfigure $star \
#		    -width  2 \
#		    -tags  [concat $tags "highlight"]

	    # Raise the box so the outline shows properly

#	    $chart.canvas raise $star

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
	# Force a display update
	#
	update idletasks
    }
}


proc ptkGantt_RedrawLabels {chart stars} {
    global ptkGantt_Parms ptkGantt_Layout

    $chart.proclabel delete star

    set base [expr [winfo y $chart.canvas] - [winfo y $chart.proclabel]]
    set layoutPositions [spread $ptkGantt_Layout($chart.numprocs) \
	    [expr $base + $ptkGantt_Layout($chart.graphTop) ]\
	    [expr $base + $ptkGantt_Layout($chart.graphBottom) ]]

    foreach s $stars {
	assign star proc $s

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
    set t [mapRange $leftEdge $rightEdge [list $x] 0 $period]
    set t [expr int($t)]

    if { $t < 0 } {
	set t 0
    } elseif { $t >= $period } {
	set t [expr $period - 1]
    }

    set x [mapRange 0 $period [list [expr $t+0.5]] $leftEdge $rightEdge]

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

    set x [mapRange 0 $period [list [expr $t+0.5]] $leftEdge $rightEdge]

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



# Here is the somewhat useless move mouse to bar function

proc ptkGantt_MoveMouse {} {
    
}

proc ptkGantt_SetNumProcs { num } {
    global ptkGantt_NumProcs
    set ptkGantt_NumProcs $num
}

proc ptkGantt_SetPeriod { num } {
    global ptkGantt_Period
    set ptkGantt_Period $num
}

proc ptkGantt_SetMin { num } {
    global ptkGantt_Min
    set ptkGantt_Min $num
}

proc ptkGantt_SetPercentage { num } {
    global ptkGantt_Percentage
    set ptkGantt_Percentage $num
}

proc ptkGantt_SetOptimum { num } {
    global ptkGantt_Optimum
    set ptkGantt_Optimum $num
}

proc ptkGantt_SetRuntime { num } {
    global ptkGantt_Runtime
    set ptkGantt_Runtime $num
}

proc ptkGantt_MakeLabel {universe period min prcent opt {runtime 0}} {
    global ptkGantt_Data

    set chartName .gantt_${universe}
    set prcent [format %.5f $prcent]
    set opt [format %.5f $opt]
    $chartName.label configure \
	    -text [format "Period = %d (vs. PtlgMin %d), busy time =\
	    %.2f%% (vs. max %.2f%%)" \
	    $period $min $prcent $opt]

    # place all these datum in ptkGantt_Data array
    set ptkGantt_Data($chartName.min) $min
    set ptkGantt_Data($chartName.prcent) $prcent
    set ptkGantt_Data($chartName.opt) $opt
    set ptkGantt_Data($chartName.runtime) $runtime

#     set chartName .gantt_${universe}
#     set prcent [format %.5f $prcent]
#     set opt [format %.5f $opt]
#     label $chartName.label \
# 	    -text [format "Period = %d (vs. PtlgMin %d), busy time =\
# 	    %.2f%% (vs. max %.2f%%)" \
# 	    $period $min $prcent $opt]
# 
#     pack $chartName.label -side top -before $chartName.frame
}

proc ptkGantt_Bindings {universe num_procs} {

    set chartName .gantt_${universe}
    # Here we set up the bindings
    bind $chartName <Enter> "focus ${chartName}.mbar"
    bind ${chartName}.mbar <Control-d> "ptkClearHighlights; destroy $chartName"

    #
    # Add some bindings so the cursor can be moved with the
    # cursor keys
    #
    bind $chartName.mbar <Left> \
	    "ptkGantt_UpdateMarker $chartName \
	   \[expr \$ptkGantt_Layout($chartName.markerTime) - 1\]"

    bind $chartName.mbar <Right> \
	    "ptkGantt_UpdateMarker $chartName \
	   \[expr \$ptkGantt_Layout($chartName.markerTime) + 1\]"

    bind $chartName.mbar <Shift-Left> \
	    "ptkGantt_UpdateMarker $chartName \
	   \[expr \$ptkGantt_Layout($chartName.markerTime) - 10\]"

    bind $chartName.mbar <Shift-Right> \
	    "ptkGantt_UpdateMarker $chartName \
	   \[expr \$ptkGantt_Layout($chartName.markerTime) + 10\]"


    #
    # A click anywhere on the canvas moves the marker to that
    # position. Dragging moves the marker as well.
    #
    bind $chartName.canvas <Button-1> \
	    "ptkGantt_MoveMarker $chartName %x"

    bind $chartName.canvas <B1-Motion> \
	    "ptkGantt_MoveMarker $chartName %x"

#   bind ${chartName}.chart.graph <Button-3> "ptkGantt_MoveMouse"

    #
    # If the window is resized, redraw the chart.
    #
    bind $chartName.canvas <Configure> \
	    [concat ptkGantt_Redraw $chartName {%w %h}]
}

proc ptkGanttDisplay { universe {inputFile ""} {standalone 0} } {

    global ptkGantt_Data ptkGantt_Layout
    set ganttChartName .gantt_${universe}
    toplevel $ganttChartName

    # Set a default window size
    wm geometry $ganttChartName 840x320

    # Here we have the menu bar.
    frame $ganttChartName.mbar -relief raised -bd 2
    pack $ganttChartName.mbar -side top -fill x
    menubutton $ganttChartName.mbar.file -text "File" -underline 0 -menu \
	    $ganttChartName.mbar.file.menu
    pack $ganttChartName.mbar.file -side left
    button $ganttChartName.mbar.help -relief raised -text "Help" -command \
	    ptkGantt_HelpButton
    pack $ganttChartName.mbar.help -side right
    
    menu $ganttChartName.mbar.file.menu -tearoff 0
    $ganttChartName.mbar.file.menu add command -label "Print Chart..." \
	    -command "ptkGantt_PrintChart $ganttChartName"
    if { ! $standalone } {
    $ganttChartName.mbar.file.menu add command -label "Save..." \
	    -command "ptkGantt_SaveChart $ganttChartName"
    }
    set exitcommand "ptkClearHighlights; destroy $ganttChartName; ptkGanttExit"
    $ganttChartName.mbar.file.menu add separator
    $ganttChartName.mbar.file.menu add command -label "Exit" \
	    -command $exitcommand -accelerator "Ctrl+d"

    # Here is where we open the file and parse its contents

    set EXISTS [file exists $inputFile]
    if { $EXISTS == 0 } {
	ptkMessage "ptkGanttDisplay: No file name given. \
		    Expecting input from the scheduler."
    } else {
	set GFILE_ID [open $inputFile r]
	set proc_num 1
	set NUMCHARS 0
	while { $NUMCHARS >= 0 } {
	    
	    set NUMCHARS [gets $GFILE_ID LINEARR]
	    if (!$NUMCHARS) continue;
	    switch [lindex $LINEARR 0] {
		no_processors { set ptkGantt_Layout($ganttChartName.numprocs)\
			[lindex $LINEARR 1];
			continue}
		period { set ptkGantt_Layout($ganttChartName.period) \
			[lindex $LINEARR 1];
			continue}
		min { set ptkGantt_Data($ganttChartName.min) \
			[lindex $LINEARR 1];
			continue}
		percentage { set ptkGantt_Data($ganttChartName.prcent) \
			[lindex $LINEARR 1];
			continue}
		optimum { set ptkGantt_Data($ganttChartName.opt) \
			[lindex $LINEARR 1];
			continue}
		runtime { set ptkGantt_Data($ganttChartName.runtime) \
			[lindex $LINEARR 1]l
			continue}
	    	\$end { incr proc_num ; continue}
		default { ptkGantt_DrawProc $universe \
			$ptkGantt_Layout($ganttChartName.numprocs) \
			$ptkGantt_Layout($ganttChartName.period) \
			$proc_num [lindex $LINEARR 0] [string trimleft \
			[lindex $LINEARR 2] (] [string trimright \
			[lindex $LINEARR 3] )]}
	    }
	}   
	close $GFILE_ID
	ptkGantt_MakeLabel $universe $ptkGantt_Layout($ganttChartName.period) \
		$ptkGantt_Data($ganttChartName.min) \
		$ptkGantt_Data($ganttChartName.prcent) \
		$ptkGantt_Data($ganttChartName.opt) \
		$ptkGantt_Data($ganttChartName.runtime)
	ptkGantt_Bindings $universe $ptkGantt_Layout($ganttChartName.numprocs)
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
    set increment  [axisIncrement 0 $period $virtualWidth $labelWidth 10]
    if { $increment < 1 } {
	set increment 1
    }
    set axisValues    [rangeValues 0 $period $increment]
    set axisLocations [mapRange    0 $period $axisValues $leftEdge $rightEdge]

    set rulerBase   [winfo pixels $chart.canvas $ptkGantt_Parms(padtop)c]
    set tickLength  [winfo pixels $chart.canvas $ptkGantt_Parms(tickLength)c]
    set labelOffset [winfo pixels $chart.canvas $ptkGantt_Parms(labelOffset)c]

    set graphTop [winfo pixels $chart.canvas \
	    [expr $ptkGantt_Parms(padtop) + $ptkGantt_Parms(rulerpad)]c]
    set graphBottom [expr $canvasHeight \
	    - [winfo pixels $chart.canvas $ptkGantt_Parms(padbottom)c]]
    set graphHeight [expr $graphBottom - $graphTop]

    set procLocations [mapRange 0 $numprocs \
	    [interval 0 $numprocs] $graphTop $graphBottom]


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

    foreach* i $axisValues x $axisLocations {
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

    foreach i [interval 1 $numprocs] {
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
    assign x0 x1 \
	    [mapRange 0 $period [list $start $fin] $leftEdge $rightEdge]


    set procLocations $ptkGantt_Layout($chart.procLocations)
    assign y0 y1 \
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
    set width [rectWidth [$canvas bbox $t]]
    $canvas delete $t

    return $width
}

#
# rectWidth: return the width of a rectangle
#
proc rectWidth {rect} {
    return [expr [x1 $rect] - [x0 $rect]]
}



#
# x, y
#
# Extract coordinates from a 2-list
#
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



#
# mapping.tcl
#
# Utility functions for mapping between number ranges, scaling
# intervals, and so on.
#


#
# Given a number, round up or down to the nearest power of two.
#
proc roundUpTwo {x} {
    set exp [expr ceil (log($x)/log(2))]
    set x   [expr pow(2,$exp)]
}

proc roundDownTwo {x} {
    set exp [expr floor (log($x)/log(2))]
    set x   [expr pow(2,$exp)]
}


#
# Given a number, round up to the nearest power of ten
# times 1, 2, or 5.
#
proc axisRoundUp {x} {
    set exp [expr floor (log10($x))]
    set x [expr $x * pow(10, -$exp)]

    if {$x > 5.0} {
	set x 10.0
    } elseif {$x > 2.0} {
	set x 5.0
    } elseif {$x > 1.0 } {
	set x 2.0
    }

    set x [expr $x * pow(10,$exp)]
    return $x
}


#
# Given a range, space, field width, and padding, figure out how
# the field increment so they will fit.
#
proc axisIncrement {low high space width padding} {
    set maxnum   [expr $space / ($width+$padding)]
    set estimate [expr (double($high) - $low) / ($maxnum)]
    set estimate [axisRoundUp $estimate]

    return $estimate
}


#
# Given a range and an increment, return the list of numbers
# within that range and on that increment.
#
proc rangeValues {low high inc} {
    set result {}
    set n      1

    set val [roundUpTo $low $inc]
    while {$val <= $high} {
	lappend result $val
	set val [expr $val + $n * $inc]
    }

    return $result
}


#
# Given two ranges and a list of numbers in the first range,
# produce the mapping of that list to the second range.
#
proc mapRange {low high values lowdash highdash} {
    set result {}

    set scale [expr (double($highdash) - $lowdash) / ($high - $low)]
    foreach n $values {
	lappend result [expr $lowdash + ($n-$low) * $scale]
    }

    return $result
}


#
# Given two numbers, round the first up or down to the
# nearest multiple of the second.
#
proc roundDownTo {x i} {
    return [expr $i * (floor($x/$i))]
}

proc roundUpTo {x i} {
    return [expr $i * (ceil($x/$i))]
}



#
# foreach*
#
# Multi-argument version of foreach. Stops when any one of the
# argument lists runs out of elements.
#
# As for loop{},. "body" cannot contain return commands. As
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
proc foreach* {args} {
    set c 0

    set v [readopt counter args]
    if {$v != ""} {
	upvar $v counter
    }

    while {[llength $args] > 2} {
	upvar   [lindex $args 0] i$c
	set l$c [lindex $args 1]

	set args [ldrop $args 2]
	incr c
    }

    if {[llength $args] != 1} {
	ptkMessage "error: wrong number of args to foreach*"
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
	    set l$i [ltail [set l$i]]
	    if {[lnull [set l$i]]} {
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
# assign
#
# Assign elements of a list to multiple variables. Doesn't
# care if the list is longer than the number of variables, ot
# the list is too short. (Should probably at least put an assertion
# in here...)
#
proc assign {args} {
    foreach* var [linit $args] val [llast $args] {
	upvar $var v
	set v $val
    }
}


#
# list.itcl
#
# Utility functions on lists. See also syntax.tcl.
#

proc lhead {list} {
    return [lindex $list 0]
}

#proc lhead {list} {
#    return [lindex $list 0
#}

proc ltail {list} {
    return [lreplace $list 0 0]
}

proc linit {list} {
    return [ltake $list [expr [llength $list] - 1]]
}

proc llast {list} {
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
proc lnull {list} {
    return [expr (! [string match "?*" $list]) \
	         || [string match "{}" $list]]
}


#
# Take or drop list elements
#
proc ltake {list n} {
    return [lrange $list 0 [expr $n - 1]]
}

proc ldrop {list n} {
    return [lreplace $list 0 [expr $n-1]]
}

proc ldropUntil {list item} {
    set index [lsearch -exact $list $item]
    if { $index == -1 } {
	return {}
    } else {
	return [ldrop $list $index]
    }
}


#
# Make a list containing n copies of the specified item
#
proc lcopy {n item} {
    set result {}
    loop $n {
	lappend result $item
    }
    return $result
}


#
# Return list of n integers in the range x to y
#
proc interval {x y} {
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
proc lmember {list item} {
    return [expr [lsearch -exact $list $item] != -1]
}


#
# Remove an item from a list
#
proc ldelete {list item} {
    set i [lsearch -exact $list $item]

    if { $i != -1 } {
	return [lreplace $list $i $i]
    }

    return $list
}


#
# Return list of n numbers in the range x to y, but with
# half the interval before the first and last numbers.
#
# This is useful for spacing graphical elements "evenly" along
# a given distance.
#
proc spread {n x y} {
    set result {}
    set i      0
    set delta  [expr (double($y) - $x) / $n]

    set x [expr $x + $delta / 2]
    while { $i < $n } {
	lappend result [expr $x + $i * $delta]

	incr i +1
    }

    return $result
}



