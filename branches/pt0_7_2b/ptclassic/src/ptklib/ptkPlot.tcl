# $Id$
#wm withdraw .
set auto_path "$tk_library/demos $auto_path"
#set lineColor cadetBlue
set lineColor lightSteelBlue1
#set lineColor #1f1f1f
#set textColor forestgreen
set textColor springgreen3
#set textColor DimGrey
set titleTextColor seagreen2
#set titleTextColor black
set textFont -Adobe-times-medium-r-normal--*-180*

bind Menubutton <Any-Enter> {+regsub {^\.[^.]*\.[^.]*\.(.*)} %W  {\1} ext
   if [info exists instr($ext)] {set bottomDisplay "$instr($ext)"}
}

bind Menubutton <Any-Leave> {+set bottomDisplay ""}

set instr(f.f) {Open data files to add to plot, save current plot}
set instr(f.o) { Modify appearance of the plot }
set instr(f.p) { Print current plot }
set instr(f.q) { Quit this application }

# Utility procedures for stroking out a rectangle and printing what's
# underneath the rectangle's area.

proc itemMark {c x y} {
    global areaX1 areaY1
    set areaX1 [$c canvasx $x]
    set areaY1 [$c canvasy $y]
    $c delete area
}

proc itemStroke {c x y} {
    global areaX1 areaY1 areaX2 areaY2
    set x [$c canvasx $x]
    set y [$c canvasy $y]
    if {($areaX1 != $x) && ($areaY1 != $y)} {
	$c delete area
	$c addtag area withtag [$c create rect $areaX1 $areaY1 $x $y \
		-outline maroon]
	set areaX2 $x
	set areaY2 $y
    }
}

proc mkCanv {pw canvW canvH} {
   global canvasInfo
   canvas $pw.c -relief raised -width $canvW -height $canvH
   pack after $pw.l $pw.c {bottom expand fill}
   bind $pw.c <3> "itemStartDrag $pw.c %x %y"
   bind $pw.c <B3-Motion> "itemDrag $pw.c %x %y"
   bind $pw.c <Configure> {
	if {[info commands redrawCanvas] != ""} {redrawCanvas %w %h}
   }

   bind $pw.c <2> {findClosest %W [%W canvasx %x] [%W canvasy %y]}
   bind $pw.c <1> "itemMark $pw.c %x %y"
   bind $pw.c <B1-Motion> {
	updateCoordDisp %W %x %y
	itemStroke %W %x %y
   }
   bind $pw.c <ButtonRelease-1> "zoomInArea $pw area"
   bind $pw.c <Any-Enter> "$pw.l configure -anchor w"
   bind $pw.c <Any-Leave> "$pw.l configure -anchor center"

   bind $pw.c <Motion> {updateCoordDisp %W %x %y}
}

proc findClosest {canv currX currY} {
   global bottomDisplay
   set item [$canv find closest $currX $currY]
   set itemTags [$canv gettags $item]
   if {[lsearch $itemTags data] != -1} {
	set coords [$canv coords $item]
	set xyvals [revMap [expr ([lindex $coords 0]+[lindex $coords 2])/2] \
		[expr ([lindex $coords 1]+[lindex $coords 3])/2]]
	set bottomDisplay "$bottomDisplay \
			Closest: ([lindex $xyvals 0],[lindex $xyvals 1])"
   }
}

proc zoomInArea {pw tag} {
   set canv $pw.c
   if {[$canv find withtag $tag] == ""} {return}
   set coords [$canv coords $tag]
#   puts "[revMap [lindex $coords 0] [lindex $coords 1]] \
#	[revMap [lindex $coords 2] [lindex $coords 3]]"
#   wm geometry [winfo parent $pw] [winfo width $pw]x[winfo height $pw]
   set xyPair1 [revMap [lindex $coords 0] [lindex $coords 1]]
   set xyPair2 [revMap [lindex $coords 2] [lindex $coords 3]]
   if {$xyPair1 != "" && $xyPair2 != ""} {
	pack unpack $pw
	eval "zoomIn $xyPair1 $xyPair2"
   }
   $canv delete $tag
}

proc updateCoordDisplay {canv x y} {
   global bottomDisplay
   set coords [revMap [$canv canvasx $x] [$canv canvasy $y]]
   if {$coords != ""} {
        set bottomDisplay \
		"Coordinates ([lindex $coords 0],[lindex $coords 1])"
   } else {
	  set bottomDisplay ""
   }
}

proc mkCanvTitle {pw title xCoord yCoord font} {
   global titleTextColor
# For printing purposes,
#   set font "-Adobe-Helvetica-Bold-R-Normal--*-140-*"
   $pw.c create text $xCoord $yCoord -text $title -fill $titleTextColor \
	-font $font -tags nonData
}

proc mkCanvColorLine {pw x1 y1 x2 y2 color args} {
   global lineColor
   if {[llength $args] > 0} {
     $pw.c create line $x1 $y1 $x2 $y2 -fill $color -tags "$args"
   } else {
     $pw.c create line $x1 $y1 $x2 $y2 -fill $color -tags "data"
   }
}

proc mkCanvLine {pw x1 y1 x2 y2 args} {
   global lineColor
   if {[llength $args] > 0} {
     $pw.c create line $x1 $y1 $x2 $y2 -fill $lineColor -tags "nonData $args"
   } else {
     $pw.c create line $x1 $y1 $x2 $y2 -fill $lineColor -tags nonData
   }
}

proc drawXTick {pw x ybot1 ybot2 ytop1 ytop2} {
   global lineColor
   $pw.c create line $x $ybot1 $x $ybot2 -fill $lineColor -tags {tick nonData}
   $pw.c create line $x $ytop1 $x $ytop2 -fill $lineColor -tags {tick nonData}
}

proc drawYTick {pw y xbot1 xbot2 xtop1 xtop2} {
   global lineColor
   $pw.c create line $xbot1 $y $xbot2 $y -fill $lineColor -tags {tick nonData}
   $pw.c create line $xtop1 $y $xtop2 $y -fill $lineColor -tags {tick nonData}
}

proc labelTick {pw label x y orientation} {
   global textColor
   $pw.c create text $x $y -text $label -anchor $orientation -fill $textColor\
	-tags {nonData tick}
}

proc createOval {pw x1 y1 x2 y2 color} {
   set id [$pw.c create oval $x1 $y1 $x2 $y2 -fill $color -outline gainsboro \
	-width 1 -tags data]
   return $id
}

# A misnomer, it moves lines also

proc moveOval {pw id x1 y1 x2 y2} {
#after 3000
#update
#   gets stdin
   $pw.c coords $id $x1 $y1 $x2 $y2
#   puts stdout "ID moved: $id "
#   update
}

proc makeInterpMenu {menu} {
   global numInterps
   upvar #0 interp interp

   set interpreters [winfo interp]

   set localInterp [winfo name .]
   set localInterpIdx [lsearch $interpreters $localInterp]
   set interpreters [lsort \
	[lreplace $interpreters $localInterpIdx $localInterpIdx]]

#  To avoid an inadvertent selection only upon $menu invocation.

   if {[info exists interp] && ([lsearch $interpreters $interp] == -1)} {
	unset interp
   }
   if [info exists numInterps] {
	$menu delete 0 [expr $numInterps+1]
   }

   foreach i $interpreters {
	$menu add radio -label $i -var interp -val $i
   }
   $menu add separator
   $menu add command -label $localInterp
   set numInterps [llength $interpreters]
}

proc ptkPlotInit {toplevel identifier loc} {
    toplevel $toplevel
#    if {$loc != ""} {
	wm geometry $toplevel $loc
#    }
    wm minsize $toplevel 0 0
    wm title $toplevel $identifier
    wm iconname $toplevel $identifier
}

# Monitor textfile var --> titlebar decoration label
proc makePlotMenu {top identifier w} {
   global scale type gridLine bounded instr bottomDisplay env saveName \
	showAxes
   wm title $top "$identifier: $w"
   option add *Font -Adobe-Helvetica-Bold-R-Normal--*-120-*
#   option add *Canvas*Text.foreground cadetBlue
   option add *Background grey35
#   option add *Background #b6b6b6
   option add *selector #b03060 100
#   option add *Canvas*Background grey35
#   option add *Menubutton*Background grey35
#   option add *activeForeground black
#   option add *activeBackground grey21
   option add *Foreground aquamarine

   if (![info exists env(PRINTER)]) {
	$env(PRINTER) = sp524
   }
   set pw $top.$w
   frame $pw
   pack append $top $pw {expand fill}

   frame $pw.f -relief raised -bd 3 -bg grey35
   menubutton $pw.f.f -text "File" -underline 0 -relief flat -menu $pw.f.f.m
#   menubutton $pw.f.r -text "Rescale" -underline 0 -relief flat -menu $pw.f.r.m
#   menubutton $pw.f.l -text "Line" -underline 0 -relief flat -menu $pw.f.i.m
#   menubutton $pw.f.s -text "Scatter" -underline 0 -relief flat -menu $pw.f.s.m
#   menubutton $pw.f.e -text "Edit" -underline 0 -relief flat -menu $pw.f.e.m
   menubutton $pw.f.o -text "Options" -underline 0 -relief flat -menu $pw.f.o.m
#   menubutton $pw.f.i -text "Info" -underline 0 -relief flat -menu $pw.f.i.m
   menubutton $pw.f.p -text "Print" -underline 0 -relief flat -menu $pw.f.p.m
   menubutton $pw.f.q -text "Exit" -underline 1 -relief flat -menu $pw.f.q.m

   menu $pw.f.q.m
   pack append $pw.f $pw.f.f {left} \
	$pw.f.o {left} $pw.f.p {left} \
	$pw.f.q {left}

   menu $pw.f.f.m
#   $pw.f.f.m add command -label "New"	  -command "clearPlot $pw"
   $pw.f.f.m add command -label "Open"	  -command "openFile $pw"
#   $pw.f.f.m add command -label "Close"	  -command "closePlot $pw"
#   $pw.f.f.m add separator
#   $pw.f.f.m add cascade -label "Template ==>" -menu $pw.f.f.m.t
##  $pw.f.f.m add command -label "Import"
#   $pw.f.f.m add separator
###  Possibly specify "Save $needed" and specify in the icon bar with a star?
   $pw.f.f.m add command -label "Save"	  -command "saveAsFile"
   $pw.f.f.m add command -label "Save As"  -command \
	"askForFile $pw.ask"
#   $pw.f.f.m add separator
#   $pw.f.f.m add command -label "Receive"  -command "receiveFile"
#   $pw.f.f.m add command -label "Send"	  -command "sendFile"

#menu $pw.f.r.m
#menu $pw.f.l.m
#menu $pw.f.s.m

#   menu $pw.f.e.m
#   $pw.f.e.m add command -label "rescale" -command "$pw.c delete tick;rescale"
#   $pw.f.e.m add command -label "Undo"	  -command "undo"
#   $pw.f.e.m add command -label "Copy"	  -command "copy"
#   $pw.f.e.m add command -label "Paste"	  -command "paste"
#   $pw.f.e.m add command -label "Cut"	  -command "cut"

   menu $pw.f.o.m
#   $pw.f.o.m add cascade -label "Scale ==>"	  -menu $pw.f.o.m.s
#   $pw.f.o.m add cascade -label "Type  ==>"	  -menu $pw.f.o.m.t
#   $pw.f.o.m add separator
   $pw.f.o.m add check -var gridLine -label "Show grid" -onval 1 -offval 0
#   $pw.f.o.m add check -var bounded  -label "Show box" -onval 1 -offval 0
   $pw.f.o.m add check -var showAxes  -label "Show xy axes" -onval 1 -offval 0

#   menu $pw.f.i.m
#   $pw.f.i.m add cascade -label "Interps ==>" \
#	-command "makeInterpMenu $pw.f.i.m.i" -menu $pw.f.i.m.i

   menu $pw.f.p.m -postcommand \
	"$pw.f.p.m entryconfigure 0 -label \"Print to \$env(PRINTER)\""
   $pw.f.p.m add command -command "printCanv $pw.c"
   $pw.f.p.m add command -label "Print to file" -command "printCanvFile $pw.c"
   $pw.f.p.m add separator
   $pw.f.p.m add command -label "Select Printer" -command \
	"selectPrinter $pw.sel"
   $pw.f.p.m add cascade -label "Orientation ==>" -menu $pw.f.p.m.o

   menu $pw.f.p.m.o
   $pw.f.p.m.o add radio	  -label "Portrait"  -var orient -val 0
   $pw.f.p.m.o add radio	  -label "Landscape" -var orient -val 1
   $pw.f.p.m.o invoke 0

   tk_menuBar $pw.f $pw.f.f $pw.f.o $pw.f.p $pw.f.q
   tk_bindForTraversal $pw
   bind $pw <Any-Enter> "focus $pw"
#   bind $pw <Any-Leave> "focus none"

#   menu $pw.f.f.m.t
#   $pw.f.f.m.t add command -label "Read"	   -command "readTemplate"
#   $pw.f.f.m.t add command -label "Save"	   -command "saveTemplate"
#   $pw.f.f.m.t add command -label "Receive" -command "receiveTemplate"
#   $pw.f.f.m.t add command -label "Send"	   -command "sendTemplate"

#   menu $pw.f.o.m.s
#   $pw.f.o.m.s add radio	  -label "linear"  -variable scale -value lin
#   $pw.f.o.m.s add radio   -label "log"	   -variable scale -value log
#   $pw.f.o.m.s add radio   -label "polar"   -variable scale -value pol
#   $pw.f.o.m.s invoke 0

#   menu $pw.f.o.m.t
#   $pw.f.o.m.t add radio	  -label "scatter" -var type -value 0
#   $pw.f.o.m.t add radio	  -label "line"	   -var type -value 1

#   $pw.f.o.m.t add radio	  -label "line & scatter" -variable type \
#	-value 1
#   $pw.f.o.m.t add radio   -label "bar"	   -var type -value 3
#   $pw.f.o.m.t invoke 0

#   $pw.f.i.m add cascade -label "Help    ==>"	  -menu $pw.f.i.m.h
#   menu $pw.f.i.m.h
#   $pw.f.i.m.h add command -label "None presently available"

#   menu $pw.f.i.m.i

   pack append $pw $pw.f {top fillx} \
	[frame $pw.bf] {bottom fillx} \
	[label $pw.l -textvar bottomDisplay] {bottom fillx}
   
   pack append $pw.bf \
	[button $pw.bf.r -text rescale \
		-command "$pw.c delete tick; rescale"] {left expand fillx}\
	[button $pw.bf.l -text line \
		-command "convertType $pw.c 1"] {left expand fillx}\
	[button $pw.bf.s -text scatter \
		-command "convertType $pw.c 0"] {left expand fillx}

   bind $pw.f.q.m <Return> "destroy $pw; exit"
   bind $pw.f.q <1> "destroy $pw; exit"
#   bind $pw.f.r <1> "$pw.c delete tick; rescale"
#   bind $pw.f.l <1> "convertType $pw.c 1"
#   bind $pw.f.s <1> "convertType $pw.c 0"


#  These variables should be unique in the global space
#   trace variable type w "convertType $pw.c"
#   trace var bounded w "showBounded $pw.c"
   trace var gridLine w "showGrid $pw.c"
   trace var showAxes w "showXYAxes $pw.c"
#   after 3000 {puts stdout "Type is $type"}
}

proc convertType {canvas type args} {
    if {[llength [$canvas gettags data]] != 0} {
	$canvas delete data
	redrawData -type $type
    }
}

proc openFile {pw} {
    if [info exists $pw.open] {
	popupMessage .error "An open dialog box already exists."
	return
    }

    global cwd fname env openType lineColor
    toplevel $pw.open 
    wm transient $pw.open $pw
    frame $pw.open.left
    frame $pw.open.left.l_e -bd 1m
    pack append $pw.open.left.l_e \
	[label $pw.open.left.l_e.labelD -fg green -text Directory:] \
		{left top frame nw} \
	[entry $pw.open.left.l_e.entryD -fg green -relief sunken \
		-bg darkSlateGrey -textvar cwd]\
		{top frame w expand fillx} \
	[label $pw.open.left.l_e.label -fg green -text "File To Open:"] \
		{left top frame w} \
	[entry $pw.open.left.l_e.entry -fg green -relief sunken -textvar \
		fname -fg green -bg DarkSlateGrey] {bottom frame w \
		expand fillx}
    frame $pw.open.left.l_s -bd 1m
    listbox $pw.open.left.l_s.list -fg green -bg darkSlateGrey\
	-relief sunken -yscroll "$pw.open.left.l_s.scroll set" -setgrid 1
# For now although support may be avail
    tk_listboxSingleSelect $pw.open.left.l_s.list
    scrollbar $pw.open.left.l_s.scroll -fg green -bg darkSlateGrey -command \
	"$pw.open.left.l_s.list yview" -bg darkSlateGrey -relief sunken \
	-activeforeground grey60 -fg grey45
    pack append $pw.open.left.l_s $pw.open.left.l_s.scroll {right filly} \
	$pw.open.left.l_s.list {left expand fill frame w}

    pack append $pw.open.left $pw.open.left.l_e {top fillx frame w} \
	$pw.open.left.l_s {bottom expand fill frame w}

    set cwd $env(PWD)
    insertInListbox $pw.open.left.l_s.list

    bind $pw.open.left.l_e.entryD <Control-f> {
	%W icursor [expr [%W index insert]+1] }
    bind $pw.open.left.l_e.entryD <Control-b> {
	%W icursor [expr [%W index insert]-1] }	
    bind $pw.open.left.l_e.entryD <Return> "insertInListbox \
	$pw.open.left.l_s.list"

    bind $pw.open.left.l_e.entry <Control-f> {
	%W icursor [expr [%W index insert]+1] }
    bind $pw.open.left.l_e.entry <Control-b> {
	%W icursor [expr [%W index insert]-1] }	
    bind $pw.open.left.l_e.entry <Return> "retrieve $pw.open.left"

    bind $pw.open.left.l_s.list <Double-1> \
	"insertInListbox $pw.open.left.l_s.list \[selection get\]"

    frame $pw.open.right -bd 2 -bg darkSlateGrey -relief sunken
    frame $pw.open.right.t -bd 2 -relief sunken -bg DarkSlateGrey
    button $pw.open.right.t.ok -text "OK" -command \
	"retrieve $pw.open.left"
    button $pw.open.right.t.can -text "Cancel" -command "destroy $pw.open"
    pack append $pw.open.right.t \
	$pw.open.right.t.ok {left padx 3m} \
	$pw.open.right.t.can {left padx 3m}

    frame $pw.open.right.b -bd 2 -relief sunken -bg DarkSlateGrey
    label $pw.open.right.b.label -text "Plot as: " -fg $lineColor -anchor w
    radiobutton $pw.open.right.b.scat -text scatter -var openType -val 0 \
	-bg darkSlateGrey -anchor w
    radiobutton $pw.open.right.b.line -text line -var openType -val 1 \
	-bg darkSlateGrey -anchor w
#    radiobutton $pw.open.right.b.both -text "line & scatter" \
#	-var openType -val 2 -bg darkSlateGrey -anchor w
    pack append $pw.open.right.b $pw.open.right.b.label {top fillx} \
	$pw.open.right.b.scat {top fillx} \
	$pw.open.right.b.line {top fillx} 

    pack append $pw.open.right $pw.open.right.t {top pady 1i} \
	$pw.open.right.b {bottom pady 1i}
    pack append $pw.open $pw.open.right {right filly} \
	$pw.open.left {left expand fill} 

    grab set $pw.open
    focus $pw.open.left.l_e.entry

    set width [expr ([winfo screenwidth $pw.open]/2-[winfo pixels $pw 2i])]
    set height [expr ([winfo screenheight $pw.open]/2-[winfo pixels $pw 2i])]
    wm geometry $pw.open  +$width+$height

}

proc retrieve {win} {
    global cwd fname openType
    if {$fname == ""} {
	popupMessage .error "You have not selected a file"
	return
    }
    if {(![info exists openType]) || ($openType == "")} {
	set openType "0"
    }
    if [string match {[$~/]?*} $fname] {
	set file [glob $fname]
	if {[file exists $file] && [file isfile $file]} {
		readData $file $openType
		set cwd [file dirname $file]
		destroy [file root $win]
#		insertInListbox $win.l_s.list
	} else {
		popupMessage .error "\"$file\" does not exist as a file!"
	}
    } elseif {[file exists $cwd/$fname] && [file isfile $file]} {
	   destroy [file root $win]
	   readData $cwd/$fname $openType
    } else { popupMessage .error "\"$fname\" does not exist as a file!" }
}

proc insertInListbox {w args} {
#       puts stdout "\"$w\" \"$args\""
    global cwd fname openType
#	puts stdout $cwd
    regsub {(.)/$} $cwd {\1} cwd
    if {! [file isdirectory $cwd]} {
	popupMessage .error "\"$cwd\" does not exist!"
	return 
    }
    if {[llength $args] > 0} {
        set theSelection [lindex $args 0]
	if [file isdirectory $cwd/$theSelection] {
		if {$theSelection == "../"} {
			if {$cwd != "/"} {set cwd "[file dirname $cwd]"}
		} else {
			set theSelection [string trimright $theSelection /]
			if {$cwd == "/"} { set cwd /$theSelection } else {
				set cwd $cwd/$theSelection }
		}
		insertInListbox $w
	} else {
		set files ""
		foreach file $args {
			if [file isdirectory $file] {
				popupMessage .error \
					"Directory \"$file\" ignored"
				continue
			}
			lappend files $file
			readData $cwd/$file $openType
			destroy [winfo parent [winfo parent $w]]
		}
		set fname $files
	}
    } else {
	$w delete 0 end
	if {$cwd != "/"} {set files [glob -nocomplain "$cwd/{.,}*"]} else {
		set files [glob -nocomplain "/{.,}*"] }
	foreach file $files {
		set file [file tail $file]
		if {$file == "."} continue
		if [file isdirectory $cwd/$file] {set file "$file/"}
		$w insert end $file
	}
    }
}

proc askForFile ask {
    catch {destroy $ask}
    global saveName textFont
    toplevel $ask
    label $ask.label -font $textFont \
	-text "Enter the filename below.                 " -anchor w
    entry $ask.entry -relief sunken -textvar saveName -fg green \
	-bg darkSlateGrey -font $textFont

    pack append $ask $ask.label {top fill} $ask.entry {top fillx} \
	[button $ask.ok -text OK -command "saveAsFile; destroy $ask"] \
	{bottom left expand fillx} \
	[button $ask.cancel -text Cancel -command "destroy $ask"] \
	{bottom right expand fillx}
    grab set $ask
    focus $ask.entry
    bind $ask.entry <Return> {saveAsFile; destroy [file root %W]}
    tkwait window $ask
}

proc saveAsFile {} {
    upvar #0 saveName saveName
    if {[info exists saveName] && $saveName != ""} {
	saveToFile $saveName
    } else { popupMessage .error "Need to specify a file" }
}

proc selectPrinter sel {
    catch {destroy $sel}
    toplevel $sel
    global printer textFont env
    label $sel.label -font $textFont -text \
	"Enter the printer below.             " -anchor w
    entry $sel.entry -relief sunken -textvar printer -fg green \
	-bg darkSlateGrey -font $textFont
    grab set $sel
    focus $sel.entry
    pack append $sel $sel.label {top fill} $sel.entry {top fillx} \
	[button $sel.ok -text OK -command \
	"destroy $sel; set env(PRINTER) \$printer"] {bottom left expand fillx}\
	[button $sel.cancel -text Cancel -command "destroy $sel"] \
	{bottom right expand fillx}
    bind $sel.entry <Return> {
        if {$printer != ""} {
		destroy [file root %W]
		set env(PRINTER) P$printer
	}
    }
}

proc printCanv canvas {
    global env
    $canvas postscript -rotate $orient -file /tmp/$canvas.ps
    exec lpr -P$env(PRINTER) /tmp/$canvas.ps
    exec /bin/rm /tmp/$canvas.ps
}

proc printCanvFile canvas {
    global saveName PSName orient
    set temp [file root $canvas].ask
    askForFile $temp
    if {$saveName == ""} {return}
    if {[file extension $saveName] != ".ps"} {
	set PSName $saveName.ps
    } else {
	set PSName $saveName
    }
    $canvas postscript -rotate $orient -file $PSName
}

#proc configScale args {
#   global scaleMax numIterations scaleVal
#   if {$numIterations < 0} {
#	set scaleMax 500
#   } else { set scaleMax $numIterations }
#   set scaleVal 0
#   if {[llength $args] > 0} {
#	.scale configure -from 0 -to $scaleMax -tickinterval \
#		[expr $scaleMax/5]
#	.scale set $scaleVal
#   }
#}
#configScale
#scale .scale -orient horizontal -from 0 -to $scaleMax \
#	-tickinterval [expr $scaleMax/5] -bg steelblue1
#.scale set $scaleVal
#pack after .quit .scale {bottom expand fillx}
#
#proc setScale {} {
#   global scaleMax scaleVal
#   incr scaleVal
#   if {$scaleVal > $scaleMax} {
#	incr scaleMax 500
#	.scale configure -from [expr $scaleVal-1] -to $scaleMax
#   }
#   if {! [expr $scaleVal%[expr $scaleMax/10]]} {
#	.scale set $scaleVal
#   }
#}

#trace var numIterations w configScale


#trace var lastX r {puts stdout "lastX being read as a global" }
