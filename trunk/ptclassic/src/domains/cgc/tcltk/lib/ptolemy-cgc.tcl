# Tcl/Tk source to include in CGC applications running under
# the CGCTclTkTarget.
#
# Author: Edward A. Lee
# Version: $Id$
#
# Copyright (c) 1993 The Regents of the University of California.
# All rights reserved.
# See the file ~ptolemy/copyright for copyright notice,
# limitation of liability, and disclaimer of warranty provisions.
#
set REPORT_TCL_ERRORS 1

source [info library]/init.tcl
source $tk_library/tk.tcl

# Procedure to expand a filename that might begin with
# an environment variable.  For example, if the value of
# of the environment variable PTOLEMY is /usr/tools/ptolemy, then 
# 	expandEnvVars \$PTOLEMY/tmp
# returns /usr/tools/ptolemy/tmp
#
proc expandEnvVars { path } {
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

# Read Ptolemy color settings from the Ptolemy Library
source [expandEnvVars \$PTOLEMY/tcl/lib/ptkColor.tcl]

# Read Ptolemy options settings from the Ptolemy Library
source [expandEnvVars \$PTOLEMY/tcl/lib/ptkOptions.tcl]

# Read utilities from Ptolemy library
source [expandEnvVars \$PTOLEMY/tcl/lib/ptkBarGraph.tcl]

message .header -relief raised \
	-width 400 -borderwidth 1 -text "Control panel for $applicationName"

# stop/start control
button .go -text " GO <Return> "
bind .go <ButtonRelease-1> "go"

button .pause -text "PAUSE <Space>"
bind .pause <ButtonRelease-1> "pause"

button .stop -text "STOP <Escape>" -command "stop"

frame .control -borderwidth 10
pack append .control .go {left expand fill} \
	    .pause {left expand fill} \
	    .stop {right expand fill}

# number of iterations control
frame .numberIters -bd 10
entry .numberIters.entry -relief sunken -width 10 -insertofftime 0
label .numberIters.label
pack append .numberIters .numberIters.label left .numberIters.entry right
.numberIters.label config -text "Number of Iterations:"
.numberIters.entry insert 0 $numIterations

# star entry section, empty by default
frame .high -bd 10

# star button section, empty by default
frame .middle -bd 10

# star slider section, empty by default
frame .low -bd 10

# quit button
button .quit -text QUIT -command "stop; destroy ."

# overall structure
pack append . \
	.header {top fill} \
	.control {top expand fill} \
	.numberIters {top expand fill} \
	.high {top expand fill} \
	.middle {top expand fill} \
	.low {top expand fill} \
	.quit {bottom expand fill}

# return causes a run
bind . <Return> "go"
bind . <space> "pause"
bind .numberIters.entry <Return> "focus .; go"
bind .numberIters.entry <Escape> "stop"
bind . <Escape> "stop"

# procedure to stop a run
proc stop {} {
	stopCmd
	.go configure -relief raised
	.pause configure -relief raised
}

# procedure to pause a run
proc pause {} {
	.pause configure -relief sunken
	.go configure -relief raised
	pauseCmd
}

#procedure to go
proc go {} {
	.go configure -relief sunken
	.pause configure -relief raised
	goCmd
}

# procedure to update the number of iterations
proc updateIterations {} {
	global numIterations
	set numIterations [.numberIters.entry get]
	return $numIterations
}

# procedure to issue an error message
proc popupMessage {w text} {
#   catch {destroy $w}
    toplevel $w
    wm title $w "Message box"
    wm iconname $w "Message"

    button $w.ok -text "OK <Return>" -command "destroy $w"
    message $w.msg -width 5i -text $text
    pack append $w $w.msg {top fill expand} $w.ok {top fill expand}

    wm geometry $w +300+300
    tkwait visibility $w
    focus $w
    grab $w
    bind $w <Return> "destroy $w"
    tkwait window $w
}

# procedure to issue an error message from any internal tk error
proc tkerror message {
    popupMessage .error "Background error in Tk"
    global REPORT_TCL_ERRORS
    if {$REPORT_TCL_ERRORS == 1} {popupMessage .error $message}
}

# procedure to make an entry for a star parameter in the master control panel
#	win	window name into which to put the entry
#	name	name of the entry
#	desc	description of the entry
#	default		the initial value in the entry
#	callback	the name of the callback procedure to register changes
proc makeEntry {win name desc default callback} {
    set s $win.$name
    frame $s
    entry $s.entry -relief sunken -width 20 -insertofftime 0 
    label $s.label -text "${desc}:"
    pack append $s $s.label left $s.entry right
    pack append $win $s {top fill expand}
    $s.entry insert 0 $default
    bind $s.entry <Return> \
	"$callback \[$win.$name.entry get\]; focus ."
    bind $s.entry <Escape> "stop"
}

# procedure to make a button for a star parameter in the master control panel
# Arguments:
#	win	window name into which to put the button
#	name	name of the button
#	desc	description of the button
#	callback	the name of the callback procedure
proc makeButton {win name desc callback} {
    set s $win.$name
    button $s -text "$desc" -command $callback
    pack append $win $s {top fill expand}
    bind $s <ButtonPress-1> "$s configure -relief sunken; $s invoke"
    bind $s <ButtonRelease-1> "$s configure -relief raised"
}

# Procedure to make a scale in some window.
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
proc makeScale {win name desc position callback} {
    set s $win.$name
    frame $s
    label $s.msg -text "${desc}:"
    label $s.value -width 6
    scale $s.scale -orient horizontal -from 0 -to 100 -length 7c \
	-command $callback -showvalue 0
    $s.scale set $position
    pack append $s $s.msg left $s.scale right $s.value right
    pack append $win $s {top fill}
}

