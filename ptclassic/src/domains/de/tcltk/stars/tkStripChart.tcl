# Tcl/Tk source for StripChart for DE domain
#
# Author: Eduardo N. Spring and Edward A. Lee
# Version: $Id$
# 
# This script file is a tcl/tk source file for StripChart stars (which are
# derived from TclScript star). A separate plot is generated for each 
# input porthole, and the plots are stacked vertically.
# It can configured to use any of three plotting styles,
# 	zero-order hold
#	connected dots (with straight lines between them)
#	unconnected dots.
# The entire history is preserved, and a scrollbar permits scanning the
# history.
#
# Placing the mouse over a point will cause the time and value of the
# point to be displayed in the upper right of the display.
# The first mouse button can be used to select a specific point in the 
# plot.  Lines are drawn to the axes to enable more precise visual 
# assessment of the position in time and value of the point.
#
# Missing features:
#   - zooming either vertically or horizontally.
#   - the time axis values should be displayed outside the canvas
#   - printing is almost useless, since it just prints the canvas
#
 
###########################################################################
proc tkStripChartHelp { } {
    ptkMessage {
StripChart widget styles:
  - hold: Zero-order hold.
  - connect: Connected dots.
  - dot: Dots only, no lines.
StripChart widget mouse commands:
  - Moving the mouse over a point will display the value in the upper right.
  - Clicking the first mouse button over a point will draw lines to the axes.
  - Clicking the second mouse button will clear these lines.
    }
}

#############################################################################
# Configuration parameters.  Should these be options in the database?

set ${starID}(dotRadius) 2	;# Radius of dots, in pixels
set ${starID}(bordX) 20		;# "Zero" position, x direction, in pixels

#############################################################################
# Storage scheme
#
# Global information is stored in the $starID array.
# The data information is redundant so that it can be indexed by point
# number or by object ID.  The following array entries track the data:
# 
# Point value and clock time indexed by plot number and point count:
#	${starID}(pointValue,$plotNum,$count)
#	${starID}(pointTime,$plotNum,$count)
# Point value and clock time indexed by plot number and point object ID:
#	${starID}(objectValue,$plotNum,$object)
#	${starID}(objectTime,$plotNum,$object)
# Scaled point value and clock time indexed by plot number and point count:
#	${starID}(scaledValue,$plotNum,$count)
#	${starID}(scaledTime,$plotNum,$count)
# Scaled point value and clock time indexed by plot number and point object ID:
#	${starID}(scObjValue,$plotNum,$object)
#	${starID}(scObjTime,$plotNum,$object)
# Count of the number of points plotted, indexed by plot number
#	${starID}(count,$plotNum)
# Number of scroll windows passed in a given plot
#	${starID}(scrollCount,$plotNum)
# Previous scaled point value and clock time (the previously plotted point)
#	${starID}(prevScaledValue,$plotNum)
#	${starID}(prevScaledTime,$plotNum)
# The low and high point of the vertical range plotted
#       ${starID}(lowYvalue,$plotNum)
#       ${starID}(highYvalue,$plotNum)
# The plot style
#       ${starID}(plotstyle,$plotNum)

#############################################################################
# tkStripChartInit
#
proc tkStripChartInit {starID} {
    global $starID
    
    # Break out the signal labels individually for easier access.
    regsub -all \n "[set ${starID}(signalLabels)]" \40 PP
    regsub -all \n "[set ${starID}(style)]" \40 SS

    for {set plotNum 0} \
	{$plotNum < [set ${starID}(numInputs)]} \
	{incr plotNum} { 
	    set ${starID}(count,$plotNum) 0
	    set ${starID}(scrollCount,$plotNum) 1
	    set ${starID}(prevScaledValue,$plotNum) 0
	    set ${starID}(prevScaledTime,$plotNum) 0
	    set ${starID}(lowYvalue,$plotNum) \
		[expr round([lindex [set ${starID}(yRangeList)] \
				 [expr $plotNum*2]])]
	    set ${starID}(highYvalue,$plotNum) \
		[expr round([lindex [set ${starID}(yRangeList)] \
				 [expr 1+($plotNum*2)]])]

	    if {[set ${starID}(lowYvalue,$plotNum)] >= \
		    [set ${starID}(highYvalue,$plotNum)]} {
		error "yRange is invalid for plot number $plotNum"
	    }

	    set ${starID}(PLabel$plotNum) [lindex $PP $plotNum]
	    set ${starID}(plotstyle,$plotNum) [lindex $SS $plotNum]
	}
}

###########################################################################
# tkStripChartMkWindow
# Make the main strip chart window.
# Arguments:
# 	top		toplevel based on StarID
# 	stopTime	stop time of the run
# 	starID		identifier for the star in charge of the chart
# 	univ		the universe name
#
proc tkStripChartMkWindow { top stopTime starID univ} { 
    global $starID

    set numInputs [set ${starID}(numInputs)]

    catch {destroy $top}
    toplevel $top
  
    wm title $top "[set ${starID}(title)]           Star ID= $starID"
    wm iconname $top "[set ${starID}(title)]"
    # request window positioning
    wm geometry $top [set ${starID}(position)]
    
    # The following allows the window to resize
    wm minsize $top 200 200

    frame $top.mBar -relief raised -bd 2
    $top.mBar config -cursor {hand2 red}
    menubutton $top.mBar.com -text Commands -underline 0 -menu $top.mBar.com.m 
    # FIXME: zooming  is not yet supported
    # menubutton $top.mBar.zoom -text Zoom -underline 0 -menu $top.mBar.zoom.m 

    pack $top.mBar -side top -fill x
    pack $top.mBar.com -side left -padx 2m

    label $top.mBar.timeLabel -text Time:
    label $top.mBar.time -text 0 -width 9 -relief sunken -bg [ptkColor wheat3]
    label $top.mBar.valueLabel -text Value:
    label $top.mBar.value -width 9 -relief sunken -bg [ptkColor wheat3]
    pack $top.mBar.value -side right -padx 2m
    pack $top.mBar.valueLabel -side right -padx 2m
    pack $top.mBar.time -side right -padx 2m
    pack $top.mBar.timeLabel -side right -padx 2m

    button $top.ok -text DISMISS -command "ptkStop $univ; destroy $top"
    pack $top.ok -side bottom -fill x
 
    menu $top.mBar.com.m 
    $top.mBar.com.m add command -label "Refresh" \
	-command "tkStripChartRefresh $top $starID $numInputs $stopTime"
    $top.mBar.com.m add command -label "Help" \
	-command "tkStripChartHelp"

    pack [frame $top.pf -bd 2m] -side top -fill both -expand 1

    # Control all the plots with one scrollbar
    scrollbar $top.pf.scroll -orient horizontal \
	-command "tkStripChartScroll $top $numInputs"
    pack $top.pf.scroll -in $top.pf -side bottom -fill x

    for {set m 0} {$m < $numInputs} {incr m} {
	set InputName [set ${starID}(PLabel$m)]

	frame $top.d$m
	label $top.d$m.input -text "$InputName"
	pack $top.d$m.input -side top

	# Create the canvas for labeling the vertical axis
	set ht [set ${starID}(height_cm)]
	canvas $top.d$m.yBar  -bg LightCyan1 -relief sunken \
		-height ${ht}c -width 14m
	pack $top.d$m.yBar -side left -fill y

	# Create the canvas for plotting the points.
	canvas $top.d$m.c -bg LightCyan1 -relief sunken \
		-height ${ht}c \
		-width [set ${starID}(width_cm)]c \
		-closeenough 3
	pack $top.d$m.c -side top -fill both -expand 1
	pack $top.d$m -in $top.pf -side top -fill both -expand 1

	# set up the scroll region based on the window width
	# How many time windows do we expect for the given stop time?
	set numWindows [expr ($stopTime+2.0)/[set ${starID}(TimeWindow)]]
	set curWidth [lindex [$top.d$m.c configure -width] 4]
	set scrollWidth [expr $numWindows*$curWidth]
	# The horizontal distance corresponding to one time unit.
	set ${starID}(unitDistance) [expr $curWidth/[set ${starID}(TimeWindow)]]
	$top.d$m.c configure -xscrollcommand "$top.pf.scroll set"
	$top.d$m.c configure -scrollregion "0 0 $scrollWidth ${ht}c"
	$top.d$m.c configure -scrollincrement 40

	tkwait visibility $top.d$m.yBar
	tkStripChartMkYScale $top.d$m.yBar $m $starID

	# Bindings for the canvas
	# Change each dot to white when the cursor enters it.
	$top.d$m.c bind node <Any-Enter> \
	    "$top.d$m.c itemconfigure current -fill white
             tkStripChartShowValue $top $top.d$m.c $m $starID"

	$top.d$m.c bind node <Any-Leave> \
            "$top.d$m.c itemconfigure current -fill red"

	bind $top.d$m.c <Button-1> \
            "$top.d$m.c delete lineToAxes
	     tkStripChartLinesToAxes $top $top.d$m.c $m $starID"

	bind $top.d$m.c <Button-2> \
            "$top.d$m.c delete lineToAxes"
    } 
    tkStripChartPaginate $top $starID $stopTime

    # Command to execute when resizing
    bind $top <Configure> \
	"tkStripChartRefresh $top $starID $numInputs $stopTime"
}

###########################################################################
# tkStripChartScroll
#
proc tkStripChartScroll { top numPlots position } {
    for {set m 0} {$m < $numPlots} {incr m} {
	$top.d$m.c xview $position
    }
}

###########################################################################
# tkStripChartPaginate
# Procedure to scroll the canvas each time window
# 
proc tkStripChartPaginate { top starID stopTime } {
    global $starID

    set bordX [set ${starID}(bordX)]
    set unitDistance [set ${starID}(unitDistance)]
    set endPoint [expr ($stopTime*$unitDistance)+$bordX]
    set timeWindow [set ${starID}(TimeWindow)]

    for {set m 0} {$m < [set ${starID}(numInputs)]} {incr m} {

        # Position in the vertical direction for text, in pixels
        set textYposition [expr 10+[set ${starID}(scaledValueZero,$m)]]
  
	if {$stopTime < $timeWindow} {
	    $top.d$m.c create text ${bordX} $textYposition \
		-text "0" -fill blue
	    $top.d$m.c create text $endPoint $textYposition \
		-text "$stopTime" -fill blue
	} else {  
	    for {set t 0} {$t <= $stopTime} {set t [expr $t+$timeWindow]} {
		$top.d$m.c create text \
		    [expr ($t*$unitDistance)+$bordX] $textYposition \
		    -text "$t" -fill blue
	    }
	}
    } 
}

############################################################################
# ptkRoundUp
# Return the smallest number larger than the positive argument that
# is 1, 2.5, or 5 times some power of 10.
# This is based on code by David Harrison in the xgraph program.
# It might be sufficiently generically useful that we should install
# it more globally.
#
proc ptkRoundUp { value } {
    if {$value <= 0} {error "ptkRoundUp: argument must be positive!"}
    set order 1.0
    while {$value < 1} {
        set value [expr double($value)*10.0]
        set order [expr double($order)/10.0]
    }
    while {$value > 10} {
        set value [expr double($value)/10.0]
        set order [expr double($order)*10.0]
    }
    if {$value > 5} {set value 10.0} \
    elseif {$value > 2.5} {set value 5.0} \
    elseif {$value > 1} {set value 2.5} \
    else {set value 1.0}
    return [expr $value*$order]
}

###########################################################################
# tkStripChartMkScale
# Procedure to label the vertical scale
#
proc tkStripChartMkYScale { canv plotNum starID} {
    global $starID   
    
    $canv delete all

    # Margin of 20 pixels left on the bottom
    set winHeight [winfo height $canv]
    set bordY [set ${starID}(bordY) [expr $winHeight-20]]

    # Get the low and the high values to plot
    set low [set ${starID}(lowYvalue,$plotNum)]
    set high [set ${starID}(highYvalue,$plotNum)]
    set dy [expr $high-$low] 
       
    # One unit of value corresponds to the following height, where
    # the "-10" ensures that points and labels near the top of the window show.
    set unitHeight \
	[set ${starID}(unitHeight,$plotNum) \
	    [expr ($bordY-10)/double($dy)]]
    set ${starID}(scaledValueZero,$plotNum) \
	[expr $bordY+[expr $unitHeight*$low]]

    # For very small windows, we do not divide the vertical axis.
    # For small windows, we divide the vertical axis in two.
    # For medium sized windows, we divide it in four.
    # For large windows, we divide it in eight.
    if {$winHeight < 60.0} {set div 1.0} \
    elseif {$winHeight < 120.0} {set div 2.0} \
    elseif {$winHeight < 250.0} {set div 4.0} \
    else {set div 8.0}

    set tickIncrement [ptkRoundUp [expr $dy/$div]]
    set tickYposition $bordY
    set tickYincrement [expr $tickIncrement*$unitHeight]
    for {set tick $low} \
	{$tick <= $high} \
	{set tick [expr $tick+$tickIncrement] } {
	    $canv create line 14m ${tickYposition} \
		12m ${tickYposition} -fill black
	    $canv create text 7m ${tickYposition} -text "$tick" -fill blue
	    set tickYposition [expr $tickYposition-$tickYincrement]
    }                   
}
   
###########################################################################
# tkStripChartMkSetValues
# Read the inputs to the star and plot the points corresponding
# to new events.  The point will be plotted even if it is out of
# range along the vertical axis.
# 
proc tkStripChartMkSetValues {starID numInputs top} {
    global $starID
  
    # Read the inputs
    set ParticleVals [grabInputs_$starID]
    set ParticleState [grabInputsState_$starID]
    
    # Iterate over the portholes
    for {set i 0} {$i < $numInputs} {incr i} {

	# Plot a point only if an event has occured on this input
	if {[lindex $ParticleState $i] == 1} {
	    tkStripChartPlotPoint \
		    $top.d$i.c \
		    [set ${starID}(arrivalTime)] \
		    [lindex $ParticleVals $i] \
		    $i $starID
	}
    }
}
 
###########################################################################
# tkStripChartPlotPoint
# Plot a point.
# 
proc tkStripChartPlotPoint { canv x y plotNum starID} { 
    global $starID
   
    set bordX [set ${starID}(bordX)]
    set bordY [set ${starID}(bordY)]
    set unitDistance [set ${starID}(unitDistance)]
    set unitHeight [set ${starID}(unitHeight,$plotNum)]
   
    set dotRadius [set ${starID}(dotRadius)]
    set count [set ${starID}(count,$plotNum)]
    # scaledTime is the x position of the new point, in pixels.
    set scaledTime [expr ($x*$unitDistance)+$bordX]
    set scaledValueZero [set ${starID}(scaledValueZero,$plotNum)]
    # scaledValue is the y position of the new point, in pixels.
    set scaledValue [expr $scaledValueZero-[expr $unitHeight*$y]]
    set st [set ${starID}(plotstyle,$plotNum)]

    if {$count==0} {
	set ${starID}(prevScaledTime,$plotNum) $scaledTime
	set ${starID}(prevScaledValue,$plotNum) $scaledValue
    } else {
	set px [set ${starID}(prevScaledTime,$plotNum)]
	set py [set ${starID}(prevScaledValue,$plotNum)]
	if { $st=="hold" } {
	    set line1 \
		[$canv create line \
			${px} ${py} \
			${scaledTime} ${py} \
			-fill blue]
	    $canv lower $line1
	    set line2 \
		[$canv create line \
			${scaledTime} ${py} \
			${scaledTime} ${scaledValue} \
			-fill blue]
	    # Make lines lowest in the stacking order
	    $canv lower $line2
	}
	if { $st=="connect" } {
	    set line \
		[$canv create line \
			${px} ${py} \
			${scaledTime} ${scaledValue} \
			-fill blue]
	    $canv lower $line
	}
	set ${starID}(prevScaledTime,$plotNum) $scaledTime
	set ${starID}(prevScaledValue,$plotNum) $scaledValue 
     } 
      
    set new [$canv create oval \
	    [expr $scaledTime-$dotRadius] [expr $scaledValue-$dotRadius] \
	    [expr $scaledTime+$dotRadius] [expr $scaledValue+$dotRadius] \
	    -outline black -fill red -tags node]

    # keep a list of plotted particles' IDs to be used in event binding
    lappend ${starID}(nodeIDs,$plotNum) $new

    # Store data in global array
    set ${starID}(pointValue,$plotNum,$count) $x
    set ${starID}(pointTime,$plotNum,$count) $y
    set ${starID}(scaledValue,$plotNum,$count) $scaledValue
    set ${starID}(scaledTime,$plotNum,$count) $scaledTime
    set ${starID}(objectValue,$plotNum,$new) $x
    set ${starID}(objectTime,$plotNum,$new) $y
    set ${starID}(scObjValue,$plotNum,$new) $scaledValue
    set ${starID}(scObjTime,$plotNum,$new) $scaledTime
      
    # scroll each canvas based when time has advanced by timeWindow or more
    set timeWindow [set ${starID}(TimeWindow)]
    if {[expr $x/$timeWindow] >= [set ${starID}(scrollCount,$plotNum)]} {
	$canv xview [expr round([expr $scaledTime/40])]
	incr ${starID}(scrollCount,$plotNum)
    }
    incr ${starID}(count,$plotNum)
}

###########################################################################
# tkStripChartRefresh
# Redraw all objects.
#
proc tkStripChartRefresh {win starID numPlots stopTime } {
    global $starID

    # To ensure that the window resizing has been completed,
    update

    for {set plotNum 0} {$plotNum < $numPlots} {incr plotNum} {
	tkStripChartMkYScale $win.d$plotNum.yBar  $plotNum $starID
	$win.d$plotNum.c delete all
	set ${starID}(nodeIDs,$plotNum) {}
	set count [set ${starID}(count,$plotNum)]
	set ${starID}(count,$plotNum) 0
	for {set i 0} {$i < $count} {incr i} {
	    set x [set ${starID}(pointValue,$plotNum,$i)]
	    set y [set ${starID}(pointTime,$plotNum,$i)]
	    tkStripChartPlotPoint $win.d$plotNum.c $x $y \
		$plotNum $starID
	}
    }
    tkStripChartPaginate $win $starID $stopTime
}

#####################################################################
# Procedure to popup an entry window to get the range of particles  to
# be zoomed. It grabs application focus and blocks other procedures to
# work untill a range to be set.
# THIS IS NOT CURRENTLY USED AND WOULD HAVE TO BE MODIFIED TO BE USED.
#
proc tkStripChartTimeRange { timeEntry } {
  global E1 E2 starID s
  global $starID

  toplevel $timeEntry -bd 2
  wm title $timeEntry "Ptolemy  Time Range"
  wm iconname $timeEntry "TimeRange" 
  wm geometry $timeEntry 300x75+520+0
  wm minsize $timeEntry 300 75
  wm maxsize $timeEntry 490 75

  frame $timeEntry.s -bd 2
  label $timeEntry.s.f1 -text "Display Samples From Time"
  entry $timeEntry.s.f2 -width 5 -relief sunken -textvariable E1
  label $timeEntry.s.f3 -text "To"
  entry $timeEntry.s.f4 -width 5 -relief sunken -textvariable E2
  pack  $timeEntry.s.f1 $timeEntry.s.f2 $timeEntry.s.f3 $timeEntry.s.f4 \
                                               -side left -expand 1 -fill x
  frame $timeEntry.dot -bd 2 -width 90
  label $timeEntry.dot.lab  -text "Dots Option: "
  radiobutton $timeEntry.dot.on -text "On" -var ${starID}(DotsOnOff) -relief flat -val 1
  radiobutton $timeEntry.dot.off -text "Off" -var ${starID}(DotsOnOff) -relief flat -val 0
  pack append $timeEntry.dot $timeEntry.dot.lab {left padx 20} \
                             $timeEntry.dot.on left $timeEntry.dot.off left 

  button $timeEntry.ok -text OK -relief raised -command "destroy $timeEntry"  
  pack $timeEntry.s $timeEntry.dot $timeEntry.ok -side top -fill x

  grab set $timeEntry
  tkwait window $timeEntry
}
  

##########################################################################
# tkStripChartShowValue
# Show the value of an event and its arrival time.
#
proc tkStripChartShowValue  { top canv plotNum starID } {   
   global $starID

   set curN [$canv find withtag current]
   if {($curN !="") &&
       [info exists ${starID}(objectValue,$plotNum,$curN)]} {
	set objectValue [set ${starID}(objectValue,$plotNum,$curN)]
	set objectTime [set ${starID}(objectTime,$plotNum,$curN)]
	$top.mBar.time configure -text [format "%8.2f" $objectValue]
	$top.mBar.value configure -text [format "%8.2f" $objectTime]
   }
}

##############################################################################
# tkStripChartLinesToAxes
# Draw lines from a point to both scale bars
#
proc tkStripChartLinesToAxes { top canv plotNum starID} {

    global $starID

    set curN [$canv find withtag current]
    if {($curN !="") && \
        [info exists ${starID}(objectValue,$plotNum,$curN)]} {
	    set scObjValue [set ${starID}(scObjValue,$plotNum,$curN)]
	    set scObjTime [set ${starID}(scObjTime,$plotNum,$curN)]
	    set line1 \
		[$canv create line \
		    0 ${scObjValue} \
		    ${scObjTime} ${scObjValue} \
		    -fill red -tag lineToAxes]
	    $canv lower $line1
	    set line2 \
		[$canv create line \
		    ${scObjTime} ${scObjValue} \
		    ${scObjTime} [expr 20+[set ${starID}(bordY)]] \
		    -fill red -tag lineToAxes]
	    $canv lower $line2
    }
}

# **************************************************************************
# Procedure to call the print procedure from ptkPlot (Tcl script for XYPlot)
# **************************************************************************
proc tkStripChartPrint { top } {
    global In

    ptkPrintXYPlot $top $top.d$In.c "Strip Chart"
}

#########################################################################
# Runtime procedure 
#
proc goTcl_$starID {starID} "
   tkStripChartMkSetValues $starID \
        [set ${starID}(numInputs)] \
        $ptkControlPanel.stripChart_${starID}
"

#########################################################################
# main call to create the display window
#

tkStripChartInit $starID

# Create window.
# First argument is the main window name.
# It is a child of the control panel so that when the control panel
# is dismissed, this window will also be dismissed.
# Second argument is the stopTime of the current control panel.
#
tkStripChartMkWindow $ptkControlPanel.stripChart_${starID} \
    [$ptkControlPanel.iter.entry get] \
    $starID \
    [curuniverse]

focus $ptkControlPanel.stripChart_${starID}.pf
