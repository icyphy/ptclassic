# Definition of a bunch of utility procedures for the tcl/tk Ptolemy interface
# Author: Edward A. Lee
# Version: $Id$
#
# Copyright (c) 1990-1993 The Regents of the University of California.
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
#                                                         COPYRIGHTENDKEY


#######################################################################
# Procedure to expand a filename that might begin with
# an environment variable.  For example, if the value of
# of the environment variable PTOLEMY is /usr/tools/ptolemy, then
#       expandEnvVars \$PTOLEMY/tmp
# returns /usr/tools/ptolemy/tmp
#
proc ptkExpandEnvVar { path } {
    if {[string first \$ $path] == 0} {
        global env
        set slash [string first / $path]
        set envvar [string range $path 1 [expr {$slash-1}]]
        set envval $env($envvar)
        return "$envval[string range $path $slash end]"
    } else {
        return $path
    }
}

#######################################################################
# procedure to make a bar-type meter in a control panel
#	win	window name into which to put the bar meter
#	name	name of the bar meter
#	desc	description of the meter
#	low	the value of the low end of the scale
#	high	the value of the high end of the scale
proc ptkMakeMeter {win name desc low high} {
    set s $win.$name
    catch {destroy $w}
    frame $s
    if {$desc != ""} {
        message $s.msg -font -Adobe-times-medium-r-normal--*-180* \
            -text "$desc" -width 10c
    }
    frame $s.bar -bd 10
	# Control for the low end of the meter
	entry $s.bar.low -relief sunken -width 5 -bg burlywood1

	# Set default values in case the parameters supplied are invalid
	global ptklowMeterEdge ptkhighMeterEdge
	set ptklowMeterEdge($s) -1.0
	set ptkhighMeterEdge($s) 1.0

	# Set the low boundary of the scale
	$s.bar.low insert 0 $low
	ptkSetMeterEdge low $low $s

	bind $s.bar.low <Return> "ptkSetMeterEdge low \[$s.bar.low get] $s"

	# overload display on the low side
	canvas $s.bar.lowOL -width 0.3c -height 0.6c \
	    -bg AntiqueWhite3 -relief sunken 
	$s.bar.lowOL create rect 0c 0c 0.3c 0.6c -fill AntiqueWhite3 \
	    -tags lowOL

	# The meter display itself
	canvas $s.bar.plot -relief sunken -bg AntiqueWhite3 \
            -height 0.6c -width 10c

	# High overload display
	canvas $s.bar.highOL -relief sunken -width 0.3c -height 0.6c \
	    -bg AntiqueWhite3
	$s.bar.highOL create rect 0c 0c 0.3c 0.6c -fill AntiqueWhite3 \
	    -tags highOL

	# Control for the high end of the meter
	entry $s.bar.high -relief sunken -width 5 -bg burlywood1
	$s.bar.high insert 0 $high
	ptkSetMeterEdge high $high $s
	bind $s.bar.high <Return> "ptkSetMeterEdge high \[$s.bar.high get] $s"

	pack append $s.bar \
	    $s.bar.low left \
	    $s.bar.lowOL left \
	    $s.bar.plot left \
	    $s.bar.highOL left \
	    $s.bar.high left
    if {$desc != ""} {pack append $s $s.msg {top fillx }}
    pack append $s $s.bar {top fillx}
    pack append $win $s top

    $s.bar.plot create rect 0c 0c 0c 0c -fill red -tags negative
    $s.bar.plot create rect 0c 0c 0c 0c -fill blue -tags positive
}

#######################################################################
# procedure to change either the high or the low boudary of a bar-type meter
# Arguments:
#	edge	low or high
#	value	new value
#	name	unique identifier for the meter (the window name)
proc ptkSetMeterEdge {edge value name} {
    # Check to see that it is a valid numeric value
    global ptklowMeterEdge ptkhighMeterEdge
    set trimVal [string trim $value]
    if {[regexp {^-?\+?[0-9]*\.?[0-9]*$} $trimVal]} {
	if {(($edge == "low") && ($trimVal < $ptkhighMeterEdge($name))) || \
	    (($edge == "high") && ($trimVal > $ptklowMeterEdge($name)))} {
		set ptk${edge}MeterEdge($name) $trimVal
		return
	}
    }
    ptkImportantMessage .error "Invalid $edge value"
    $name.bar.$edge delete 0 end
    $name.bar.$edge insert 0 [set ptk${edge}MeterEdge($name)]
}

#######################################################################
# procedure to set the value of a bar-type meter in a control panel
#	win	window name in which the bar meter was made
#	name	name of the bar meter
#	value	the current value to be displayed
proc ptkSetMeter {win name value} {
    set s $win.$name
    global ptklowMeterEdge ptkhighMeterEdge
    set low $ptklowMeterEdge($s)
    set high $ptkhighMeterEdge($s)
    set scale [expr {10.0/($high - $low)}]
    set origin [expr {- $scale * $low}]
    set x [expr {$scale * ($value - $low)}]
    if {$value < 0.0} {
	$s.bar.plot coords negative ${origin}c 0c ${x}c 0.6c
	$s.bar.plot coords positive ${origin}c 0c ${origin}c 0.6c
    } {
	$s.bar.plot coords positive ${origin}c 0c ${x}c 0.6c
	$s.bar.plot coords negative ${origin}c 0c ${origin}c 0.6c
    }
    if {$value > $high} { $s.bar.highOL itemconfigure highOL -fill red } \
		{ $s.bar.highOL itemconfigure highOL -fill AntiqueWhite3 }
    if {$value < $low}  { $s.bar.lowOL itemconfigure lowOL -fill blue } \
		{ $s.bar.lowOL itemconfigure lowOL -fill AntiqueWhite3 }
}

#######################################################################
# procedure to make an entry for a star parameter in a control panel
#	win	window name into which to put the entry
#	name	name of the entry
#	desc	description of the entry
#	default		the initial value in the entry
#	callback	the name of the callback procedure to register changes
proc ptkMakeEntry {win name desc default callback} {
    set s $win.$name
    catch {destroy $s}
    frame $s
    entry $s.entry -relief sunken -width 20 -insertofftime 0 -bg burlywood1
    label $s.label -text "${desc}:"
    pack append $s $s.label left $s.entry right
    pack append .$win $s {top fill expand}
    $s.entry insert 0 $default
    bind $s.entry <Return> \
	"$callback \[.$win.$name.entry get\]; focus ."
    bind $s.entry <Escape> "stop"
}

#######################################################################
# procedure to make a button for a star parameter in a control panel
# Arguments:
#	win	window name into which to put the button
#	name	name of the button
#	desc	description of the button
#	callback	the name of the callback procedure
proc ptkMakeButton {win name desc callback} {
    set s $win.$name
    catch {destroy $s}
    button $s -text "$desc" -fg tan4 -command $callback
    pack append .$win $s {top fill expand}
    bind $s <ButtonPress-1> "$s configure -relief sunken; $s invoke"
    bind $s <ButtonRelease-1> "$s configure -relief raised"
}

#######################################################################
# Procedure to make a scale in a control panel
# All scales in the control panel actually range from 0 to 100,
# but from the user perspective can range from low to high, where low
# and high are floating point numbers.
# Arguments:
#	win	window name into which to put the pane
#	name	name of the pane
#	desc	description of the scale slider
#	low	the low end of the range from the user perspective (flt. pt.)
#	high	the high end of the range from the user perspective (flt. pt.)
#	position	the initial value between 0 and 100 (int)
#	callback	the name of the callback procedure to register changes
proc ptkMakeScale {win name desc position callback} {
    set s $win.$name
    catch {destroy $s}
    frame $s
    label $s.msg -text "${desc}:"
    label $s.value -width 6
    scale $s.scale -orient horizontal -from 0 -to 100 -bg tan4 \
	-sliderforeground bisque1 -fg bisque1 -length 7c \
	-command $callback -showvalue 0
    $s.scale set $position
    pack append $s $s.msg left $s.scale right $s.value right
    pack append .$win $s {top fill}
}


#######################################################################
# Procedure to make an animated bar chart display of an array.
# Arguments:
#	w		toplevel window name to use (without the leading period)
#	desc		description to attach to the window
#	geo		geometry of the window
#	numBars		number of bars in the bar chart
#	barChartWidth	width in centimeters
#	barChartHeight	height in centimeters
proc ptkMakeBarChart {w desc geo numBars barChartWidth barChartHeight} {
    catch {destroy .$w}
    toplevel .$w
    wm title .$w "$desc"
    wm iconname .$w "$desc"

    message .$w.msg -font -Adobe-times-medium-r-normal--*-180* -width 5i \
            -text "$desc"
    frame .$w.cntr -bd 10
    label .$w.cntr.label -text "Full scale:"
    set startScale [${w}verticalScale 1.0]
    label .$w.cntr.value -width 10 -text $startScale
    button .$w.cntr.d10 -text "Scale/10" -command "changeBarScale $w 0.1"
    button .$w.cntr.hv -text "Scale/2" -command "changeBarScale $w 0.5"
    button .$w.cntr.dbl -text "Scale*2" -command "changeBarScale $w 2.0"
    button .$w.cntr.t10 -text "Scale*10" -command "changeBarScale $w 10.0"
    pack append .$w.cntr \
	.$w.cntr.label left \
	.$w.cntr.value left \
	.$w.cntr.d10 left \
	.$w.cntr.hv left \
	.$w.cntr.dbl left \
	.$w.cntr.t10 left

    frame .$w.pf -bd 10
    canvas .$w.pf.plot -relief sunken -bd 3 -bg AntiqueWhite3 \
	    -height ${barChartHeight}c -width ${barChartWidth}c
    pack append .$w.pf .$w.pf.plot {top fill expand}

    # bar entry, button and slider section, empty by default
    frame .$w.cp_high -bd 10
    frame .$w.cp_middle -bd 10
    frame .$w.cp_low -bd 10

    button .$w.quit -text "QUIT" -command "destroy .$w"
    pack append .$w .$w.msg {top fillx } \
		.$w.cntr {top fillx} \
		.$w.pf {top fill expand } \
		.$w.cp_high {top fillx } \
		.$w.cp_middle {top fillx } \
		.$w.cp_low {top fillx } \
		.$w.quit {top fillx }

    wm geometry .$w $geo
    wm minsize .$w 400 200
}

proc changeBarScale {w s} {
    set newScale [${w}verticalScale $s]
    .$w.cntr.value configure -text $newScale
}
