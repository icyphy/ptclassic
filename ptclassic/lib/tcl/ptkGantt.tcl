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

proc ptkGantt_PrintChart { chartName } {
    global env
    set universe [string trimleft $chartName .gantt_]
    if [winfo exists .print] {
	.print.msg configure -text "You have requested to print $universe \
		to $env(PRINTER). Is this OK?"
    } else {
	if {![info exists env(PRINTER)]} { 
	    set env(PRINTER) "lp"
	}
	toplevel .print
	message .print.msg  -width 10c -relief sunken -pady 2m -padx 1m \
		-text "You have requested to print $universe to  \
		$env(PRINTER). Is this OK?"
	pack .print.msg
	button .print.ok -text OK -command "ptkGantt_Print $chartName; \
		destroy .print"
	button .print.cancel -text Cancel -command {destroy .print}
	button .print.change -text "Change Printer" -command \
		"ptkGantt_ChangePrinter $chartName"
	pack .print.ok .print.cancel .print.change -side left -fill x -expand 1
    }
}

proc ptkGantt_ChangePrinter { chartName } {
    global env
    toplevel .printer
    label .printer.label -text "Output printer:"
    entry .printer.entry -relief sunken -bd 2 -textvariable env(PRINTER)
    pack .printer.label .printer.entry -side left -expand 1 -fill x
    button .printer.ok -text OK -command "destroy .printer; \
	    ptkGantt_PrintChart $chartName"
    pack .printer.ok -fill x -expand 1 -side bottom -after .printer.entry
    focus .printer.entry
}

proc ptkGantt_Print { chartName } {
    global env
    set OUTPUT [open "|lpr -P$env(PRINTER)" w]
    puts $OUTPUT [${chartName}.chart.graph postscript -colormode gray \
	    -pageheight 10i -pagewidth 8i -rotate 1]
    close $OUTPUT
}


proc ptkGantt_HelpButton {} {
    ptkMessage {
Gantt Chart help:
  - Use the zoom buttons to zoom along the x-axis only.
  - The print chart functionality only works if you first
       drag the gantt chart window to display the entire
       chart area and then use the print function.
       Note: The processor albel and the ruler will not be
             printed.
  - Use Control-d or Exit from the file menu to close the
       gantt chart.  It will not be closed by the run
       control panel.
   }
}


# This proc was created to facilitate moving both the ruler and chart along
# the x-axis at the same time.
proc ptkGantt_DualMove { chartName dir args } {
    set arg_num [llength $args]
    if { ($arg_num == 1) } {
	if { $dir == "x" } {
	    ${chartName}.chart.graph xview $args
	    ${chartName}.chart.ruler xview $args
	} else {
	    ${chartName}.chart.graph yview $args
	    ${chartName}.chart.label yview $args
	}
    } elseif {$arg_num == 2 } {
	if { $dir == "x" } {
	    ${chartName}.chart.graph xview moveto [lindex $args 1]
	    ${chartName}.chart.ruler xview moveto [lindex $args 1]
	} else {
	    ${chartName}.chart.graph yview moveto [lindex $args 1]
	    ${chartName}.chart.label yview moveto [lindex $args 1]
	}
    } else {
	if { $dir == "x" } {
	    ${chartName}.chart.graph xview scroll [lindex $args 1] units
	    ${chartName}.chart.ruler xview scroll [lindex $args 1] units
	} else {
	    ${chartName}.chart.graph yview scroll [lindex $args 1] units
	    ${chartName}.chart.label yview scroll [lindex $args 1] units
	}
    }
}

proc ptkGantt_Zoom { universe num_procs period dir } {

    set chartName .gantt_${universe}
    if {$dir == "in"} {
	$chartName.mbar.zoomindex configure -text \
		[expr 2.0*[lindex [$chartName.mbar.zoomindex configure -text] \
		4]]
    } else {
	$chartName.mbar.zoomindex configure -text \
		[expr 0.5*[lindex [$chartName.mbar.zoomindex configure -text] \
		4]]
    }

    set ganttFont -adobe-courier-medium-r-normal--10-100-75-75-m-60-iso8859-1
    if {$dir == "in"} {
	set scale 2.0 
    } else {
	set scale 0.5
    }

    # delete labels and bar then scale boxes and ruler
    ptkClearHighlights
    $chartName.chart.graph delete slabel bar
    $chartName.chart.graph scale box 1c 0 $scale 1.0
    $chartName.chart.ruler scale all 1c 0 $scale 1.0

    # set new scrollregion
    set new_width [expr [expr [lindex [$chartName.mbar.zoomindex configure \
	    -text] 4] * $period] + 2]
    $chartName.chart.graph configure -scrollregion \
	    "0 0 ${new_width}c ${num_procs}i"
    $chartName.chart.ruler configure -scrollregion "0 0 ${new_width}c 1.5c"
    $chartName.chart.graph configure -width ${new_width}c
    $chartName.chart.ruler configure -width ${new_width}c

    foreach box [$chartName.chart.graph find withtag box] {
	set star_name [string range [lindex [$chartName.chart.graph gettags \
		$box] [lsearch -glob [$chartName.chart.graph gettags $box] \
		"star_*"]] 5 end]
	set coords [$chartName.chart.graph coords $box]
	if {[expr [string length $star_name] * 10] <= [expr [lindex $coords 2]\
		- [lindex $coords 0]] } {
	    set proc [string range [lindex [$chartName.chart.graph gettags \
		    $box] [lsearch -glob [$chartName.chart.graph gettags $box]\
		    "p*"]] 1 end]		    
	    ${chartName}.chart.graph create text [lindex $coords 0] \
		    [expr $proc -1]i -text $star_name -font $ganttFont \
		    -anchor nw -fill white -tags "slabel"
	}
    }
}

proc ptkGantt_DrawProc { universe num_procs period proc star_name start fin } {

    set ptkGantt_Colors { red cyan green steelblue lime green sienna pink \
	    yellow orangered blue coral tan khaki magenta mediumaquamarine \
	    forestgreen }

    set chartName .gantt_${universe}
    set ganttFont -adobe-courier-medium-r-normal--10-100-75-75-m-60-iso8859-1


    if ![winfo exists ${chartName}.chart] {

	# first we add the zoom buttons...not done earlier because we need
	# to pass tons of variables not yet set earlier
	button $chartName.mbar.zoomin -text "Zoom In" -command \
		"ptkGantt_Zoom $universe $num_procs $period in"
	button $chartName.mbar.zoomout -text "Zoom Out" -command \
		"ptkGantt_Zoom $universe $num_procs $period out"
	label $chartName.mbar.zoomlabel -text "   Zoom factor: "
	label $chartName.mbar.zoomindex -text "1.0" -relief sunken 
	pack $chartName.mbar.zoomin $chartName.mbar.zoomout \
		$chartName.mbar.zoomlabel $chartName.mbar.zoomindex -side left

	# Here we create the ruler

	canvas ${chartName}.chart
	pack ${chartName}.chart

	canvas ${chartName}.chart.ruler -width [expr $period + 2]c \
		-height 1.5c -xscrollcommand "${chartName}.xscroll set" \
		-scrollregion "0i 0i [expr $period + 2]c 1.5c"
	pack ${chartName}.chart.ruler -in ${chartName}.chart
	${chartName}.chart.ruler create line 1c 0.5c 1c 1c  \
		[expr $period + 1]c 1c [expr $period + 1]c \
		0.5c
	for { set i 0 } { $i < $period } { incr i } {
	    set x [expr $i + 1]
	    ${chartName}.chart.ruler create line ${x}c 0.5c ${x}c 1c
	    ${chartName}.chart.ruler create text $x.15c .75c -text $i \
		    -anchor sw
	}
	${chartName}.chart.ruler create text [expr $period + 1].15c \
		.75c -text $period -anchor sw

	# Now we create the chart

	canvas ${chartName}.chart.graph -width [expr $period + 2]c \
		-height ${num_procs}i -xscrollcommand \
		"${chartName}.xscroll set" -yscrollcommand  \
		"${chartName}.yscroll set" -bg black -scrollregion \
		"0i 0i [expr $period + 2]c ${num_procs}i"
	pack ${chartName}.chart.graph -in ${chartName}.chart

	# Now we create the label for the processors

	canvas ${chartName}.chart.label -width 1c -height [expr \
		$num_procs + 1]i -yscrollcommand \
		"${chartName}.yscroll set" -scrollregion "0i 0i 1c [expr \
		$num_procs  + 1]i"
	pack ${chartName}.chart.label -before ${chartName}.chart.ruler -side \
		left -anchor n
	for {set i 0} {$i < $num_procs } {incr i} {
	    ${chartName}.chart.label create text 4 [expr $i + 1]i -text \
		    "Proc\n[expr $i + 1]\n" -anchor nw -fill black \
		    -justify center
	}

	scrollbar ${chartName}.yscroll -command "ptkGantt_DualMove \
		$chartName y"
	pack ${chartName}.yscroll -side right -fill y -before \
		${chartName}.chart
	scrollbar ${chartName}.xscroll -command "ptkGantt_DualMove  \
		$chartName x" -orient horizontal
	pack ${chartName}.xscroll -fill x -before ${chartName}.chart -side \
		bottom

    }
    if {$start < $fin} {
	set tags_list [list box star_$star_name p$proc]
	${chartName}.chart.graph create rectangle [expr $start + 1]c \
	[expr $proc - 1]i [expr $fin + 1]c \
		${proc}i -fill [lindex $ptkGantt_Colors \
		[expr $proc - 1]] -outline black -tags $tags_list
	if {[expr [string length $star_name] * 10] <= [expr \
		($fin - $start)*28.3] } {
	    ${chartName}.chart.graph create text [expr $start + 1]c \
		    [expr $proc -1]i -width [expr $fin - $start]i \
		    -text $star_name -font $ganttFont -anchor nw -fill white \
		    -tags "slabel"
	}
	pack ${chartName}.chart.graph
    }
}

# Here is the ptkGanttHighlight proc to highlight the stars

proc ptkGanttHighlight { universe stars_list } {
    global ptkColorList

    set ptkGantt_Colors { red cyan green steelblue lime green sienna pink \
	    yellow orangered blue coral tan khaki magenta mediumaquamarine \
	    forestgreen }
    
    ptkClearHighlights
    for { set i [llength $stars_list] } { $i > 0 } { incr i -1 } {
	set k [llength $stars_list]
	set fullName "${universe}."
	if { [lindex $stars_list [expr $k-$i]] == "noops" } {
	    set fullName ""
	} else {
	    append fullName [lindex $stars_list [expr $k-$i]]
	}
	set color [lindex $ptkGantt_Colors [expr $k - $i]]
	if {[lsearch [array names ptkColorList] $color] == -1} {
	    return "Invalid color specification"
	}
	set tcl_col [ptkColor $color]
	set red [format "%d" [format "0x%s" [string range $tcl_col 1 2]]]
	set green [format "%d" [format "0x%s" [string range $tcl_col 3 4]]]
	set blue [format "%d" [format "0x%s" [string range $tcl_col 5 6]]]
	# these colors are weak in intensity, here we multiply them
	set red [expr $red * 256]
	set green [expr $green * 256]
	set blue [expr $blue * 256]
	
	if { $fullName == "" } {
	    continue
	} else {
	    ptkHighlight $fullName $red $green $blue
	}
    }
}

# Here is the code to draw the bar and highlight the selections

proc ptkGantt_MoveMarker { chartName num_procs x-coord } {

    set ptkGantt_Colors { red cyan green steelblue lime green sienna pink \
	    yellow orangered blue coral tan khaki magenta mediumaquamarine \
	    forestgreen }


    #convert x-coord to canvas coordinate
    set x-coord [${chartName}.chart.graph canvasx ${x-coord}]

    # here is where we clean up the last bar and highlights

    ${chartName}.chart.graph delete bar
    set highlighted [${chartName}.chart.graph find withtag highlight]
    foreach box $highlighted {
	set ptag [lindex [${chartName}.chart.graph gettags $box] \
		[lsearch -glob [${chartName}.chart.graph gettags $box] "p*"]]
	set col [lindex $ptkGantt_Colors [expr [string trimleft $ptag p] - 1]]
	${chartName}.chart.graph dtag $box highlight
	${chartName}.chart.graph itemconfigure $box -fill $col -width 1 \
		-outline black

    }

    for { set i 1 } { $i <= $num_procs } { incr i } {
	set ptkGantt_StarsList {}
	set ptkGantt_StarsArray($i) {}
	set ptkGantt_Instances($i) {}
    }

    # now we can find the new boxes to highlight

    ${chartName}.chart.graph create line ${x-coord} 0i ${x-coord} [lindex \
	    [${chartName}.chart.graph configure -height] 4] -tags { bar } \
	    -fill white
    set bar_coords [${chartName}.chart.graph coords bar]
    set star_list [${chartName}.chart.graph find overlapping [lindex \
	    $bar_coords 0] [lindex $bar_coords 1] [lindex $bar_coords 2] \
	    [lindex $bar_coords 3]]
    foreach box $star_list {
	if {([lsearch -exact [${chartName}.chart.graph gettags $box] "bar"] \
		== -1)&&([lsearch -exact [${chartName}.chart.graph gettags \
		$box] "slabel"] == -1) } {
	    set ptag [string trimleft [lindex [${chartName}.chart.graph \
		    gettags $box] [lsearch -glob [${chartName}.chart.graph \
		    gettags $box] "p*"]] p]
	    set col [lindex $ptkGantt_Colors [expr [string trimleft $ptag p] \
		    - 1]]
	    set new_tags [concat [${chartName}.chart.graph gettags $box] \
		    "highlight"]
	    ${chartName}.chart.graph itemconfigure $box -width 2 -fill $col \
		    -outline white -tags $new_tags

	    # make sure we add these stars to the highlight list
	    set star_name [lindex [${chartName}.chart.graph gettags $box] \
		    [lsearch -glob [${chartName}.chart.graph gettags $box] \
		    "star_*"]]
	    set ptkGantt_StarsArray($ptag) [string range $star_name 5 end]
	    set ptkGantt_Instances($ptag) [lindex [split [string trimleft \
		    $star_name "star_"] .] 1]
	}
    }

    for { set i 1 } { $i <= $num_procs } { incr i } {
	if { $ptkGantt_StarsArray($i) == "" } {
	    lappend ptkGantt_StarsList "noops"
	} else {
	    lappend ptkGantt_StarsList $ptkGantt_StarsArray($i)
	}
    }
    set universe [string range $chartName 7 end]
    ptkGanttHighlight $universe $ptkGantt_StarsList
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

proc ptkGantt_MakeLabel {universe period min prcent opt} {

    set chartName .gantt_${universe}
    set prcent [format %.5f $prcent]
    set opt [format %.5f $opt]
    label $chartName.label -relief sunken -text "period =  \
	    $period (vs. PtlgMin $min), busy time = \
	    ${prcent}% (vs. max ${opt}%)"
    pack $chartName.label -side bottom -before $chartName.yscroll
}

proc ptkGantt_Bindings {universe num_procs} {

    set chartName .gantt_${universe}
    # Here we set up the bindings
    bind $chartName <Enter> "focus ${chartName}.mbar"
    bind ${chartName}.mbar <Control-d> "ptkClearHighlights; destroy $chartName"
    bind ${chartName}.chart.graph <Button-1> "ptkGantt_MoveMarker $chartName \
	    $num_procs %x"
    bind ${chartName}.chart.graph <Button-3> "ptkGantt_MoveMouse"

}

proc ptkGanttDisplay { universe {inputFile ""} } {

    set ganttChartName .gantt_${universe}

    toplevel $ganttChartName

    wm minsize $ganttChartName 100 50
    wm maxsize $ganttChartName 1000 800
    # wm geometry $ganttChartName 500x400

    # Here we have the menu bar.

    frame $ganttChartName.mbar -relief raised -bd 2
    pack $ganttChartName.mbar -side top -fill x
    menubutton $ganttChartName.mbar.file -text "File" -underline 0 -menu \
	    $ganttChartName.mbar.file.menu
    pack $ganttChartName.mbar.file -side left
    button $ganttChartName.mbar.help -relief flat -text "Help" -command \
	    ptkGantt_HelpButton
    pack $ganttChartName.mbar.help -side right
    
    menu $ganttChartName.mbar.file.menu -tearoff 0
    $ganttChartName.mbar.file.menu add command -label "Print Chart..." \
	    -command "ptkGantt_PrintChart $ganttChartName"
    $ganttChartName.mbar.file.menu add command -label "Exit" -command \
	    "ptkClearHighlights; destroy $ganttChartName" -accelerator "Ctrl+d"
    
    # Here is where we open the file and parse its contents

    set EXISTS [file exists $inputFile]
    if { $EXISTS == 0 } {
	puts "No file name given.  Expecting input from scheduler."
    } else {
	set GFILE_ID [open $inputFile r]
	set proc_num 1
	set NUMCHARS 0
	while { $NUMCHARS >= 0 } {
	    
	    set NUMCHARS [gets $GFILE_ID LINEARR]
	    if (!$NUMCHARS) continue;
	    switch [lindex $LINEARR 0] {
		no_processors { set num_procs [lindex $LINEARR 1];
			continue}
		period { set period [lindex $LINEARR 1];
			continue}
		min { set min [lindex $LINEARR 1];
			continue}
		percentage { set prcent [lindex $LINEARR 1];
			continue}
		optimum { set opt [lindex $LINEARR 1];
			continue}
		runtime { set runtime [lindex $LINEARR 1]l
			continue}
	    	\$end { incr proc_num ; continue}
		default { ptkGantt_DrawProc $universe $num_procs $period \
			$proc_num [lindex $LINEARR 0] [string trimleft \
			[lindex $LINEARR 2] (] [string trimright \
			[lindex $LINEARR 3] )]}
	    }
	}   
	close $GFILE_ID
	ptkGantt_MakeLabel $universe $period $min $prcent $opt
	ptkGantt_Bindings $universe $num_procs
    }
}

