#
# xpseqgraph.tcl :: Simple Sequence Graph (SSG)
#
#
proc xpseqgraph.tcl { } { }

#
# Set up class bindings for XPSeqGraph and options
#
proc xpSSG.classbind { } {
    global XpBackground XpForeground XpActiveBackground XpActiveForeground
    option add *XPSeqGraph*background		$XpBackground		30
    option add *XPSeqGraph*activeBackground	$XpActiveBackground	30
    option add *XPSeqGraph*foreground		$XpForeground		30
    option add *XPSeqGraph*activeForeground	$XpActiveForeground	30

    bind XPSeqWidget f "xpSSG.zoomFull %W"
    bind XPSeqWidget x "xpSSG.grow %W 2.0 dummy"
    bind XPSeqWidget X "xpSSG.grow %W 0.5 dummy"
    bind XPSeqWidget <ButtonPress-2>	"%W.rubber begin %x %y"
    bind XPSeqWidget <ButtonPress-3>	"xpSSG.dragBegin %W %x %y"
    bind XPSeqWidget <Any-ButtonRelease> "xpSSG.buttonRelease %W %b %x %y"
    bind XPSeqWidget <Any-Motion>	"xpSSG.motion %W %x %y"
    bind XPSeqWidget <Any-Enter>	"xpSSG.enter %W"
    bind XPSeqWidget <Any-Leave>	"xpSSG.leave %W"
}
xpSSG.classbind
rename xpSSG.classbind ""

proc xpSSG.enter { wd } {
    upvar #0 $wd svars
    global XpForeground XpDimForeground XpPremptFocus
    foreach locinfo $svars(locWins) {
	[lindex $locinfo 2] conf -fg $XpForeground
    }
    if { $XpPremptFocus } {
	focus $wd
    }
}

proc xpSSG.leave { wd } {
    upvar #0 $wd svars
    global XpForeground XpDimForeground
    foreach locinfo $svars(locWins) {
	[lindex $locinfo 2] conf -fg $XpDimForeground
    }
}

proc xpSSG.dragBegin { wd x y } {
    upvar #0 $wd svars
    set curPtInfo [$wd ptindex @$x,$y]
#puts stdout "xpSSG.dragBegin $curPtInfo"
    if { "$curPtInfo" == "" } {
	set svars(state) "normal"
	puts stderr "No points to select."
	return
    }
    set svars(curSeq) [lindex $curPtInfo 0]
    set axisName [lindex [$wd seqconf $svars(curSeq) -axis] 4]
    set svars(curAxis) [lindex [$wd axisconf $axisName -path] 4]
    set svars(curPt) #[lindex $curPtInfo 1]
    set svars(state) drag
}

proc xpSSG.motion { wd x y } {
    upvar #0 $wd svars
    case $svars(state) {
      drag {
	set wy [$svars(curAxis) xform draw win $y]
	$wd seqvalue $svars(curSeq) $svars(curPt) $wy
      }
    }

    # Update loc windows
    foreach locinfo $svars(locWins) {
	# locinfo = { axisPath x|y cmd } 
	set vval [set [lindex $locinfo 1]]
	if { [winfo ismapped [lindex $locinfo 0]] } {
	    set wval [[lindex $locinfo 0] xform draw fmtwin+2 $vval]
#	    set wval [[lindex $locinfo 0] xform draw win $vval]
	} else {
	    set wval ""
	}
	[lindex $locinfo 2] conf -text $wval
    }
}

proc xpSSG.buttonRelease { wd b x y } {
    upvar #0 $wd svars
    catch {xpSSG.motion $wd $x $y}
    case $svars(state) {
      drag {
      }
    }
    set svars(state) normal
}

#
# Stuff in the legend is doing all kinds of var traces.  We must kill
# these off before killing the main window, since otherwise attempts
# to unset global vars will fail (b/c they are being traced).
#
proc xpSSG.destroyNow { w } {
    topWin.SafeDestroy $w.legend
    topWin.SafeDestroy $w
}

proc xpSSG.addmenu { w wd menu } {
    $menu add com -lab "Zoom Full" -acc f -com "xpSSG.zoomFull $wd"
    $menu add com -lab "Zoom Out"  -acc x -com "xpSSG.grow $wd 2.0 dummy"
    $menu add com -lab "Zoom In"   -acc X -com "xpSSG.grow $wd 0.5 dummy"
    $menu add com -lab "Zap Window" -acc "^z" -com "after 1 xpSSG.destroyNow $w"
}

proc xpSSG.addbind { w wd menu } {
    bind $menu <Control-z> [list $menu invoke "Zap Window"]
}

proc xpSSG.grow { w factor fullP } {
    set axislist [$w axislist]
    foreach axis $axislist {
	set axiswdg [lindex $axis 2]
    	$axiswdg grow $factor $fullP
    }
}

proc xpSSG.zoomFull { widget } {
    set domAxis [lindex [$widget axislist] 0]
    [lindex $domAxis 2] grow 1.05 full
    update
    xpSSG.grow $widget 1.05 full
}

proc xpSSG.zoomBox { w boxType x1 y1 width height } {
    set ax [$w axislist]
    if { $width >= 20 } {
	set domain [lindex $ax 0]
	set axiswdg [lindex $domain 2]
	set x2 [expr {$x1 + $width}]
	$axiswdg conf -lo [$axiswdg x d w $x1] -hi [$axiswdg x d w $x2]
    }
    if { $height >= 20 } {
	set y2 [expr {$y1 + $height}]
	foreach axis [lrange $ax 1 end] {
	    set axiswdg [lindex $axis 2]
	    $axiswdg conf -lo [$axiswdg x d w $y1] -hi [$axiswdg x d w $y2]
	}
    }
}

proc xpSSG.InitVars { w } {
    upvar #0 $w.draw svars
    set svars(state) normal
    set svars(locWins) ""
}

proc xpSimpleSeqGraph { w } {
    # Fault in bindings for xpaxis
    xpaxis.tcl
    upvar #0 $w.draw svars
    label $w.title -text "Frequency Response"
    pack append $w $w.title { top filly }

    frame $w.legend
    pack append $w $w.legend {top fill}

    xpaxis $w.lAxis -side right -flip true
    pack append $w $w.lAxis {left filly}

    xpaxis $w.rAxis -side left -flip true
    pack append $w $w.rAxis {right filly}

    xpaxis $w.bAxis -side top
    pack append $w $w.bAxis {bottom fillx}

    label $w.loc -text "you are here"
    pack append $w $w.loc {top fill}

    xpseqwidget $w.draw
    $w.draw axisconfigure 0 -name b -path $w.bAxis
    $w.draw axisconfigure 1 -name l -path $w.lAxis
    $w.draw axisconfigure 2 -name r -path $w.rAxis
    pack append $w $w.draw {top expand fill}

    $w.lAxis conf -dataWdg $w.draw -command "$w.draw axisxf l"
    $w.rAxis conf -dataWdg $w.draw -command "$w.draw axisxf r"
    $w.bAxis conf -dataWdg $w.draw -command "$w.draw axisxf b"

    rubber $w.draw.rubber $w.draw -command "xpSSG.zoomBox $w.draw"
    xpSSG.LegendWatch $w
}

proc xpSSG.SeqExistB { graph name } {
    upvar #0 $graph.draw svars
    return [expr { (![info exist svars(destroy-$name)]) && "[$graph.draw seqindex $name]"!="none"} ]
}

proc xpSSG.LegendWatch { graph } {
    upvar #0 $graph.draw svars
    set svars(legendwins) ""
    trace variable svars(legends) w "xpSSG.LegendRefresh $graph"
}

proc xpSSG.LegendRefresh { graph n1 n2 ops } {
    upvar #0 $graph.draw svars
    foreach gls $svars(legendwins) {
	set mb [lindex $gls 1]
# puts stdout "gls ``$gls''"
	if { [topWin.ExistB $mb] } {
	    $mb conf -bitmap $graph.draw.[lindex $gls 0]
	}
    }
}

proc xpSSG.AddLine { graph name color } {
    $graph.draw seqcreate $name -fg $color
    xpSSG.AddLineLegend $graph $name
}

proc xpSSG.AddLineLegend { graph name } {
    set gl $graph.legend
    set mb $gl.$name
    pack append $gl [menubutton $mb] { left fill expand }
    xpSSG.BindLineLegend $graph $name $mb
}

proc xpSSG.BindLineLegend { graph name mb } {
    upvar #0 $graph.draw svars
    lappend svars(legendwins) [list $name $mb]

    set dvar $graph.draw.$name; set m $mb.m
    $mb conf -bitmap $dvar -menu $m
    menu $m

    $m add cascade -lab "Line Style" -menu $m.line
    menu $m.line; set v ${dvar}(linestyle)
    $m.line add radio -lab Solid	-var $v -val "solid"	-acc ls
    $m.line add radio -lab Fence	-var $v -val "fence"	-acc lf
    $m.line add radio -lab None		-var $v -val "none"	-acc ln
    $m.line add sep ; set v ${dvar}(linexor)
    $m.line add check -lab xor		-var $v
    $m.line add sep ; set v ${dvar}(linewidth)
    $m.line add radio -lab 0		-var $v -val "0"	-acc l0
    $m.line add radio -lab 1		-var $v -val "1"	-acc l1
    $m.line add radio -lab 3		-var $v -val "3"	-acc l3
    $m.line add radio -lab 5		-var $v -val "5"	-acc l5
    $m.line add radio -lab 9		-var $v -val "9"	-acc l9
    bind $mb <l><Key>	"xpSSG.LineBind $graph $name %A"

    $m add cascade -lab "Mark Style" -menu $m.mark
    menu $m.mark ; set v ${dvar}(markstyle)
    $m.mark add radio -lab Circle	-var $v -val "circle"	-acc mc
    $m.mark add radio -lab Dot		-var $v -val "dot"	-acc md
    $m.mark add radio -lab Box		-var $v -val "box"	-acc mb
    $m.mark add radio -lab Cross	-var $v -val "cross"	-acc mx
    $m.mark add radio -lab None		-var $v -val "none"	-acc mn
    $m.mark add sep ; set v ${dvar}(markwidth)
    $m.mark add radio -lab 3		-var $v -val "3"	-acc m3
    $m.mark add radio -lab 5		-var $v -val "5"	-acc m5
    $m.mark add radio -lab 7		-var $v -val "7"	-acc m7
    $m.mark add radio -lab 9		-var $v -val "9"	-acc m9
    bind $mb <m><Key>	"xpSSG.MarkBind $graph $name %A"

    $m add command -lab "Zap" -acc "^z" -com "xpSSG.DelLine $graph $name"
    bind $mb <Control-z> "$m invoke Zap"

}

proc xpSSG.LineBind { graph name key } {
    case $key in {
        s	{ set val solid }
        f	{ set val fence }
        n	{ set val none }
        [0-9]	{ xpSSG.SetSeqVar $graph $name linewidth $key; return }
	x	{ xpSSG.TogSeqVar $graph $name linexor; return }
	default	{ return }
   }
   xpSSG.SetSeqVar $graph $name linestyle $val
}

proc xpSSG.MarkBind { graph name key } {
    case $key in {
        c	{ set val circle }
        d	{ set val dot }
        b	{ set val box }
        x	{ set val cross }
        n	{ set val none }
        [0-9]	{ xpSSG.SetSeqVar $graph $name markwidth $key; return }
	default	{ return }
   }
   xpSSG.SetSeqVar $graph $name markstyle $val
}

#
# We are quite likely invoked by the menu assocatied with the legend.
# We cant destroy the legend immediately, b/c the unpost caused by
# the button-release event will fail.  Thus we delay the destruction of
# the legend.
#
# Note: the long term solution is to change the name of the sequence
# here (syncronously, but not delete it completely).  Then later (async)
# we can delete the real thing.
#
proc xpSSG.DelLine { graph name } {
#puts stdout "xpSSG.DelLine $name"
    upvar #0 $graph.draw svars
    if { ! [xpSSG.SeqExistB $graph $name] } {
	error "xpSSG.DelLine: $graph: sequence ``$name'' doesnt exist"
    }
    set mbw ""
    foreach gls $svars(legendwins) {
	set mb [lindex $gls 1]
	if { "[lindex $gls 0]"=="$name" && [topWin.ExistB $mb] } {
	    set mbw $mb
	    break
	}
    }
    set svars(destroy-$name) 1
    after 1 xpSSG.DelLineNow $graph $name $mbw
}

#
# This mess is further complicated by the traces done by the menus.
# When we delete the seq., the graphwidget will attempt to unset
# a global variable; however, the menus dont like this.  Thus must first
# get rid of menus.
#
proc xpSSG.DelLineNow { graph name mb } {
    upvar #0 $graph.draw svars
    unset svars(destroy-$name)
    topWin.SafeDestroy $mb
    $graph.draw seqdelete $name
    xpSSG.CheckAxes $graph
}

#
# Pack any used axis, and
# Unpack any unused axis.  Never unpack the orgin axis.
#
proc xpSSG.CheckAxes { graph } {
    upvar #0 $graph.draw svars

    foreach ai [lrange [$graph.draw axislist] 1 end] {
#puts stdout "xpCSG.DelSeq: $ai"
	set packvar packcmd-[lindex $ai 2]
	if { [info exist svars($packvar)] } {
	    if { [lindex $ai 3]==0 } {
		pack unpack [lindex $ai 2]
	    } else {
		eval pack $svars($packvar)
	    }
	}
    }
}

proc xpSSG.SetSeqVar { graph name var val } {
    upvar #0 $graph.draw.$name dvars
    set dvars($var) $val
}

proc xpSSG.TogSeqVar { graph name var } {
    upvar #0 $graph.draw.$name dvars
    if { $dvars($var) } then {
        set dvars($var) 0
    } else {
        set dvars($var) 1
    }
}

proc xpSSG.AddFakeData { graph } {
    xpSSG.AddLine $graph beorn blue
    $graph.draw seqaddxy beorn 2 3   3 6   4 5   5 1   6 7   7 6

    xpSSG.AddLine $graph phil red
    $graph.draw seqaddxy phil 2 7   3 4   4 6   5 2   6 1   7 3
}
