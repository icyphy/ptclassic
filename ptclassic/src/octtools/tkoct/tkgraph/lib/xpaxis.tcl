#
# xpaxis.tcl :: XP Tk Graph Library
#
# Options and bindings for the XP Axis Widget.
# The core axis widget is the "xpaxis" (axisTk.c).  This provides
# the binding functions and secondary windows.
#
proc xpaxis.tcl { } { }

#
# Add the options and bindings.  We put it in a proc so we can have local
# variables.
# The proc is invoked and then destroyed.  The bindings invoke functions
# defined below.
#
proc xpAxis.classbind { } {
    global XpBackground XpActiveBackground XpForeground XpActiveForeground
    global XpTickColor XpActiveTickColor
    option add *XPAxis*background		$XpBackground		30
    option add *XPAxis*activeBackground		$XpActiveBackground	30
    option add *XPAxis*foreground		$XpForeground		30
    option add *XPAxis*activeForeground		$XpActiveForeground	30
    option add *XPAxis*tickColor		$XpTickColor		30
    option add *XPAxis.scroll.foreground	$XpTickColor		31
    option add *XPAxis.scroll.activeForeground	$XpActiveTickColor	31

    set numberBind {xpAxis.ValPopup %W [%W region %x %y] %A}

    bind XPAxis 0 $numberBind
    bind XPAxis 1 $numberBind
    bind XPAxis 2 $numberBind
    bind XPAxis 3 $numberBind
    bind XPAxis 4 $numberBind
    bind XPAxis 5 $numberBind
    bind XPAxis 6 $numberBind
    bind XPAxis 7 $numberBind
    bind XPAxis 8 $numberBind
    bind XPAxis 9 $numberBind
    bind XPAxis <KeyPress-period> $numberBind
    bind XPAxis <KeyPress-minus> $numberBind
    bind XPAxis <Enter> "xpFocus.enter %W"
    bind XPAxis m "xpAxis.MenuPopup %W %X %Y"

    #
    # Now make the axis look like a menubutton
    #
    bind XPAxis <Any-Enter> {set tk_priv(inMenuButton) %W}
    bind XPAxis <Any-Leave> {set tk_priv(inMenuButton) {}}
    bind XPAxis <1> {xpAxis.MenuPost %W %X %Y}
    bind XPAxis <Any-ButtonRelease-1> {
	if {($tk_priv(posted) == "%W") && ($tk_priv(inMenuButton) == "%W")} {
	    [lindex [$tk_priv(posted) config -menu] 4] activate 0
	} else {
	    tk_mbUnpost
	}
    }
    bind XPAxis <2> {
	if {($tk_priv(posted) == "")} {
	    set tk_priv(dragging) %W
	    [lindex [$tk_priv(dragging) config -menu] 4] post %X %Y
	}
    }
    bind XPAxis <B2-Motion> {
	if {$tk_priv(dragging) != ""} {
	    [lindex [$tk_priv(dragging) config -menu] 4] post %X %Y
	}
    }
    bind XPAxis <ButtonRelease-2> {set tk_priv(dragging) ""}
}
xpAxis.classbind
rename xpAxis.classbind ""




proc xpAxisCreate { aw args } {
    set side top
    set args [topgetopt -any { {side} } $args]
    case $side {
      top { set sa "-orient horiz" }
      bottom { set sa "-orient horiz" }
      left { set sa "-orient vert" }
      right { set sa "-orient vert" }
    }

    eval xpaxis {$aw} -side $side $args

    upvar #0 $aw avars
    set avars(infonode) "(xpole)Axis Widget"
    topWin.TraceWindowOptions $aw scale showlabel showscrollbar autosize tickmode tickfmt
    trace var avars(tickscale)     rw "xpAxis.TraceTickAttrs $aw scale"
    trace var avars(ticktype)      rw "xpAxis.TraceTickAttrs $aw type"
    trace var avars(change-config)  w "xpAxis.TraceConfCB $aw"

    eval scrollbar {$aw.scroll} $sa -com {"$aw scroll"}
    xpAxis.MenuMake $aw
    $aw conf -scrollwindow $aw.scroll -scrollcommand "$aw.scroll set" \
      -menu $aw.m
}

#
# This is called whenver aw(tickscale) or aw(ticktype) is read or written.
# We echo the changes to/from aw(format).
#
proc xpAxis.TraceTickAttrs { aw which n1 n2 op } {
    upvar #0 $aw avars
#puts stdout "TraceTickAttrs: $aw $which $n2 $op"
    case $op in {
      r	{ set avars($n2) [fmt$which $avars(tickfmt)] }
      w	{ set avars(tickfmt) [fmt$which $avars(tickfmt) $avars($n2)] }
    }
}

#
# This is a trace on "change-config": it resets all the variables
# that reflect the axis's config.
#
proc xpAxis.TraceConfCB { aw n1 n2 op } {
    upvar #0 $aw avars
    topWin.ResetWindowTraces $aw
    set avars(tickscale) [fmtscale $avars(tickfmt)]
    set avars(ticktype) [fmttype $avars(tickfmt)]
}

proc xpAxis.SetVar { w which val } {
    upvar #0 $w avars
    set avars($which) $val
}

proc xpAxis.GrowFull { w } {
    %W grow 1.05 full
}

proc xpAxis.ValPopdown { w } {
    upvar #0 $w avars
    set popup $avars(limitpopup)
    set avars(popupactive) 0
    grab release $popup.entry
    focus none
    xpFocus.enter $w
    windowunmap $popup
}

proc xpAxis.ValPopupDone { w } {
    upvar #0 $w avars
    set popup $avars(limitpopup)
    set newval [$popup.entry get]
    if [catch {$w val $avars(popupregion) $newval} error] {
	puts stdout "Invalid limit: ``$newval''"
    }
    xpAxis.ValPopdown $w
}

proc xpAxis.ValPopup { w region {initText ""} } {
    upvar #0 $w avars

    if { [info exist avars(limitpopup)] } {
	set popup $avars(limitpopup)
    } else {
	set avars(limitpopup) $w.limpop
	set avars(popupactive) 0
	set popup $avars(limitpopup)
	toplevel $popup
	wm transient $popup $w
	pack append $popup [label $popup.label -text Lo -width 4] {left}
	pack append $popup [entry $popup.entry -relief sunken -width 6] \
	  {left fillx}
	bind $popup.entry <Escape> "xpAxis.ValPopdown $w"
	bind $popup.entry <Return> "xpAxis.ValPopupDone $w"
#	windowmap $popup
    }
    if { $avars(popupactive) } {
	$popup.entry insert insert $initText
	return
    }
    
    set layout [$w popuplayout $popup]

    case $region {
      {l*} {
	set loc [lindex $layout 0]; set curVal [$w val lo]; set region lo
      }
      {m*} {
	set loc [lindex $layout 1]; set curVal [$w val mid]; set region mid
      }
      {h*} {
	set loc [lindex $layout 2]; set curVal [$w val hi]; set region hi
      }
      {c*} {
	set loc [lindex $layout 1]; set curVal [$w val clip]; set region clip
      }
      default {
	error "$w: Unknown region ``$region''"
      }
    }
    $popup.label conf -text $region
    set oldval [$popup.entry get]
    if { ![catch {prsdbl $oldval} oldval2] && $oldval2==$curVal } {
	set curVal $oldval
    }

    set avars(popupregion) $region
    topEntry.set $popup.entry [expr {"$initText"!="" ? $initText : $curVal }]
    append geom + [expr {[lindex $loc 0]+[winfo rootx $w]}] \
		+ [expr {[lindex $loc 1]+[winfo rooty $w]}]
#puts stdout "layout $layout; loc $loc; geom $geom"
    wm geom $popup $geom
    windowmap $popup
    focus $popup.entry
    grab set $popup.entry
    set avars(popupactive) 1
}

#
# Contruct the popup menu for axis widget {aw}.
#
proc xpAxis.MenuMake { aw } {
    upvar #0 $aw avars
    set m $aw.m; menu $m
    set albl [lindex [$aw conf -label] 4]
    set albl [expr {"$albl"=="" ? "Axis Menu" : "$albl Axis"}]
    $m add command -lab $albl -state disabled
    $m add sep
    $m add check -lab "Auto Scale"      -acc ^a -var ${aw}(autosize)
    $m add check -lab "Show Label"      -acc ^l -var ${aw}(showlabel)
    $m add check -lab "Show Scrollbar"  -acc ^s -var ${aw}(showscrollbar)
    $m add sep
    $m add cascade -lab "Scale" -under 0 -acc ==> -menu $m.scale
	menu $m.scale; set v ${aw}(scale)
	$m.scale add radio -lab Linear   -und 0 -acc l -var $v -val lin
	$m.scale add radio -lab Log	 -und 2 -acc g -var $v -val log
	$m.scale add radio -lab Dslog    -und 0 -acc o -var $v -val dslog
    $m add cascade -lab "Label" -under 0 -acc ==> -menu $m.label
	menu $m.label; set v ${aw}(tickscale) 
	$m.label add radio -lab "Natural" -und 0 -acc n -var $v -val -
	$m.label add radio -lab "dB-10"   -und 3 -acc d -var $v -val d
	$m.label add radio -lab "dB-20"   -und 3 -acc D -var $v -val D
	$m.label add radio -lab "PI"      -und 0 -acc p -var $v -val P
    $m add cascade -lab "Format" -under 0 -acc ==> -menu $m.format
	menu $m.format; set v ${aw}(ticktype)
	$m.format add radio -lab "Engineer"   -und 0 -acc E -var $v -val E
	$m.format add radio -lab "Scientific" -und 0 -acc S -var $v -val e
	$m.format add radio -lab "Short Sci"  -und 1 -acc G -var $v -val g
	$m.format add radio -lab "Fixed Pt"   -und 0 -acc F -var $v -val f
    $m add cascade -lab "Ticks" -under 0 -acc ==> -menu $m.ticks
	menu $m.ticks; set v ${aw}(tickmode)
	$m.ticks add radio -lab "None"    -und 0 -var $v -val none
	$m.ticks add radio -lab "Inside"  -und 0 -var $v -val inside
	$m.ticks add radio -lab "Outside" -und 0 -var $v -val outside
    $m add sep
    $m add command -lab "Set Lo"   -und 4 -acc L -com "xpAxis.ValPopup $aw lo"
    $m add command -lab "Set Mid"  -und 4 -acc M -com "xpAxis.ValPopup $aw mid"
    $m add command -lab "Set Hi"   -und 4 -acc H -com "xpAxis.ValPopup $aw hi"
    $m add command -lab "Set Clip" -und 4 -acc C -com "xpAxis.ValPopup $aw clip"
    $m add command -lab "Zoom Full" -und 5 -acc f -com "$aw grow 1.05 full"
    $m add command -lab "Zoom Out"  -und 5 -acc x -com "$aw grow 2.0"
    $m add command -lab "Zoom In"   -und 5 -acc X -com "$aw grow 0.5"
    $m add sep
    $m add command -lab "Help"    -com "help $avars(infonode)"
    $m add command -lab "Dismiss" -acc "^z" -com "xpAxis.MenuDismiss $aw"

    topWin.BindAccelsOnce XPAxis $aw.m %W.m
    topWin.BindAccels $aw.scroll $aw.m
}

proc xpAxis.MenuDismiss { aw } {
    global tk_priv
    if { "$aw.m" == "$tk_priv(posted)" } {
	tk_mbUnpost
    } else {
	$aw.m unpost
    }
}

#
# Popup the menu created by {xpAxis.MenuMake}.  This is invoked by bindings
# on the widget itself.  The menu is popup'd in "persistent" mode: it must
# be explicitly dismissed.
#
proc xpAxis.MenuPopup { aw rootX rootY } {
    set menu [lindex [$aw conf -menu] 4]
    if {"$menu"==""} return
    $menu post $rootX $rootY
}

#
# Based on tk_mbPost() from menu.tcl.
# The menu is popup'd as in a menubutton: it will go away when the mouse
# button is released.  This also cooperates with other menubuttons in the
# system.
#
proc xpAxis.MenuPost {w rootX rootY} {
    global tk_priv tk_strictMotif
    if {([lindex [$w config -state] 4] == "disabled")
	    || ($w == $tk_priv(posted))} {
	return
    }
    set menu [lindex [$w config -menu] 4]
    if {$menu == ""} return
    set cur $tk_priv(posted)
    if {$cur != ""} tk_mbUnpost
    set tk_priv(relief) [lindex [$w config -relief] 4]
    $w config -relief raised
    set tk_priv(posted) $w
    if {$tk_priv(focus) == ""} {
	set tk_priv(focus) [focus]
    }
    set tk_priv(activeBg) [lindex [$menu config -activebackground] 4]
    set tk_priv(activeFg) [lindex [$menu config -activeforeground] 4]
    if $tk_strictMotif {
	$menu config -activebackground [lindex [$menu config -background] 4]
	$menu config -activeforeground [lindex [$menu config -foreground] 4]
    }
    $menu activate none
    focus $menu
#    $menu post [winfo rootx $w] [expr [winfo rooty $w]+[winfo height $w]]
    $menu post $rootX $rootY
#    if [catch {set grab $tk_priv(menuBarFor[winfo toplevel $w])}] {
	set grab $w
#    } else {
#	if [lsearch $tk_priv(menusFor$grab) $w]<0 {
#	    set grab $w
#	}
#    }
    set tk_priv(cursor) [lindex [$grab config -cursor] 4]
    $grab config -cursor arrow
    set tk_priv(grab) $grab
    grab -global $grab
}

##
## This function is invoked to construct the axis-configuration dialog box.
##
#proc xpAxis.MakeConfigure { w rootx rooty } {
#    upvar #0 $w avars
#    set wd $w.dlg
#    if {! [info exists avars(dlg)]} then {
#        toplevel $wd
#	set avars(dlg) $wd
#        wm transient $wd $w
#
#	set wdm [frame $wd.menurow]
#	pack append $wd $wdm { top fillx }
#
#	menubutton $wdm.scale -text "AxisScale" -menu $wdm.scale.m
#	menu $wdm.scale.m ; set v ${w}(scale)
#	$wdm.scale.m add radio -label lin -var $v -val lin
#	$wdm.scale.m add radio -label log -var $v -val log
#	$wdm.scale.m add radio -label dslog -var $v -val dslog
#	pack append $wdm $wdm.scale { left }
#
#	menubutton $wdm.tickscale -text "LblScale" -menu $wdm.tickscale.m
#	menu $wdm.tickscale.m ; set v ${w}(tickscale) 
#	$wdm.tickscale.m add radio -label "normal" -var $v -val -
#	$wdm.tickscale.m add radio -label "dB-10" -var $v -val d
#	$wdm.tickscale.m add radio -label "dB-20" -var $v -val D
#	$wdm.tickscale.m add radio -label "PI" -var $v -val P
#	pack append $wdm $wdm.tickscale { left }
#
#	menubutton $wdm.ticktype -text "LblType" -menu $wdm.ticktype.m
#	menu $wdm.ticktype.m ; set v ${w}(ticktype)
#	$wdm.ticktype.m add radio -label "eng" -var $v -val E
#	$wdm.ticktype.m add radio -label "sci" -var $v -val e
#	$wdm.ticktype.m add radio -label "gsci" -var $v -val g
#	$wdm.ticktype.m add radio -label "fix" -var $v -val f
#	pack append $wdm $wdm.ticktype { left }
#
#	checkbutton $wd.autofull -text "Auto Full" -var ${w}(autosize)
#	pack append $wd $wd.autofull { top }
#
#	set wdv [frame $wd.vals -geom 10x200]
#	label $wdv.titlelbl -text "Limit"
#	place $wdv.titlelbl -in $wdv -anchor nw
#	label $wdv.titleval -text "Value"
#	place $wdv.titleval -in $wdv.titlelbl -relx 1.1 -y 0 -anchor nw
#        xpAxis.MakeConfEntry $w $wdv $wdv.title 0 Lo lo
#        xpAxis.MakeConfEntry $w $wdv $wdv.title 1 Hi hi
#        xpAxis.MakeConfEntry $w $wdv $wdv.title 2 wClip wClip
#        xpAxis.MakeConfEntry $w $wdv $wdv.title 3 vClip vClip
#        xpAxis.MakeConfEntry $w $wdv $wdv.title 4 Fmt fmt
#	set avars(vallist) { lo hi wClip vClip fmt }
#	pack append $wd $wdv { top fillx }
#
#	set wdb [frame $wd.buts]
#	pack append $wd $wdb { bottom fillx }
#	button $wdb.ok -text "Ok" -command "windowunmap $wd"
#	pack append $wdb $wdb.ok { left }
#	button $wdb.grow -text "Grow" -command "$w grow 2.0"
#	pack append $wdb $wdb.grow { left }
#	button $wdb.shrink -text "Shrink" -command "$w grow 0.5"
#	pack append $wdb $wdb.shrink { left }
#	button $wdb.full -text "Full" -command "xpAxis.GrowFull $w"
#	pack append $wdb $wdb.shrink { left }
#
#	bind $wd <Map> "xpAxis.ResetValEntries $w"
#    }
#    wm geometry $wd +$rootx+$rooty
#    windowmap $wd
#}
#
#proc xpAxis.MakeConfEntry { aw pnt master row label which } {
#    upvar #0 $aw avars
#
#    set dist [expr {1.05*$row+.9}]
#    label $pnt.${which}lbl -text $label
#    place $pnt.${which}lbl -in ${master}lbl -x 0 -rely $dist -bo outside
#    set we $pnt.${which}val
#    entry $we -relief sunken -width 10
##    $we insert 0 [set avars($which)]
#    place $we -in ${master}val -x 0 -rely $dist -bo outside
#    bind $we <Return> "xpAxis.ConfVal $aw $which"
#    bind $we <Leave> "xpAxis.ConfVal $aw $which"
#    bind $we <Up> "xpAxis.SwitchVal $aw $which -1"
#    bind $we <Down> "xpAxis.SwitchVal $aw $which 1"
#    bind $we <Escape> "xpAxis.ResetValEntry $aw $which"
#}
#
#proc xpAxis.ResetValEntry { aw which } {
#    upvar #0 $aw avars
#    set newval [set avars($which)]
#    set we $aw.dlg.vals.${which}val
#    $we delete 0 end
#    $we insert 0 [string trim $newval]
#    $we icursor 0
#}
#
#proc xpAxis.ResetValEntries { aw } {
#    foreach which "lo hi vClip wClip fmt" {
#	xpAxis.ResetValEntry $aw $which
#    }
#}
#
#proc xpAxis.ConfVal { aw which } {
#    upvar #0 $aw avars
#    set curval [string trim [set avars($which)]]
#    set newval [$aw.dlg.vals.${which}val get]
#    case [string trim $newval] in {
#        "" {
#	    xpAxis.ResetValEntry $aw $which
#	}
#	$curval {
#	}
#	default {
#	    if [catch "set avars($which) $newval" catch_err] then {
#		error $catch_err
#	    }
#	}
#    }
#    focus none
#}
#
#proc xpAxis.SwitchVal { aw which delta } {
#    upvar #0 $aw avars
#    xpAxis.ConfVal $aw $which
#    set len [llength $avars(vallist)]
#    set curidx [lsearch $avars(vallist) $which]
#    set newidx [expr {($curidx+$delta+$len)%$len}]
#    set newwhich [lindex $avars(vallist) $newidx]
#    focus $aw.dlg.vals.${newwhich}val
#}


#	{ tickscale } {
#    	    set newval $avars($n2)
#	    set avars(fmt) [fmtscale $avars(fmt) $newval]
#	}
#	{ ticktype } {
#    	    set newval $avars($n2)
#	    set avars(fmt) [fmttype $avars(fmt) $newval]
#	}
#	{ xform } {
#	    upvar #0 $aw.scroll svars
#	    if [info exist svars] {
#		set newval $avars($n2)
#		set vLo [dflt -int floor [expr {([lindex $newval 0]-$svars(lo))/$svars(factor)}]]
#		set vHi [dflt -int ceil [expr {([lindex $newval 1]-$svars(lo))/$svars(factor)}]]
#		$aw.scroll set 1000 [expr {($vHi-$vLo)/2}] $vLo $vHi
#	    }
#	}

#proc xpAxis.TraceConfCB { aw n1 n2 op } {
#    global errorInfo
#    if [catch {xpAxis.TraceConfCB_core $aw $n2} catch_err] {
#	toperror "xpAxis.TraceConfCB: $catch_err\n$errorInfo"
#    }
#}

#
# This procedure is invoked bind two axis widgets togther 
# so that their aspect ratio is preserved.  It registers a trace
# with both widgets to track changes.
#
proc xpAxis.AspectOn { w1 w2 } {
    global xpole
    upvar #0 $w1 avars1 $w2 avars2
    topdbgmsg -pkg xp.axis.zoom "aspect: start on $w1 $w2"
    set avars1(aspect) $w2
    set avars2(aspect) $w1
    trace variable avars1(xform) w "xpAxis.AspectTraceCB"
    trace variable avars2(xform) w "xpAxis.AspectTraceCB"
    catch {unset xpole(axis-$w1.wMid)}
    catch {unset xpole(axis-$w2.wMid)}
}

proc xpAxis.abs x { return [expr {$x < 0.0 ? -$x : $x}] }

proc xpAxis.getlengths { w wLenRef vLenRef wMidRef } {
    upvar #0 $w avars
    upvar $wLenRef wLen $vLenRef vLen $wMidRef wMid
    
    set vLen [lindex $avars(xform) 2]
#    set x [expr {[lindex $avars(xform) 0] - [lindex $avars(xform) 1]}]
    set wLo [lindex [$w conf -lo] 4]
    set wHi [lindex [$w conf -hi] 4]
    set x [expr {$wLo - $wHi}]
    set wLen [expr {$x < 0.0 ? -$x : $x}]
    set wMid [expr {($wLo + $wHi)/2.0}]
}

#
#  Perform a zoom operation (set winport), perserving aspect ratio.
#  Since the given zoom rectange is unlikely to have the desired aspect,
#  we choose the larger of the two rects defined by the given winport.
#  
#  For implementation, there are two approaches: we could zoom one axis
#  and let the aspect-tracing code zoom the other, or we can just zoom
#  both here.  It appears to be more robust to zoom both here.
#
proc xpAxis.AspectZoom { w1 lo1 hi1 w2 lo2 hi2 } {
    global xpole
    upvar #0 $w1 avars1 $w2 avars2

    set vLen1 [lindex $avars1(xform) 2]
    set wLen1 [xpAxis.abs [expr {$hi1-$lo1}]]
    set vLen2 [lindex $avars2(xform) 2]
    set wLen2 [xpAxis.abs [expr {$hi2-$lo2}]]
    topdbgmsg -pkg xp.axis.zoom "zoom: begin $w1/$lo1,$hi1,$vLen1 and $w2/$lo2,$hi2,$vLen2 [expr {$vLen2 * $wLen1}] [expr {$wLen2 * $vLen1}]"
    if {$vLen2 * $wLen1 > $wLen2 * $vLen1} then {
#	set xpole(axis-$w2.wMid) [expr {($lo2+$hi2)/2.0}]
	$w1 conf -rangepad 1.05 -lo $lo1 -hi $hi1
	set wMid2 [expr {($lo2+$hi2)/2.0}]
        set wLen2new [expr {($wLen1/($vLen1+0.0)) * $vLen2}]
        $w2 conf -rangepad 1.05 -lo [expr {$wMid2 - $wLen2new/2}] \
			    -hi [expr {$wMid2 + $wLen2new/2}]
    } else {
#	set xpole(axis-$w1.wMid) [expr {($lo1+$hi1)/2.0}]
	$w2 conf -rangepad 1.05 -lo $lo2 -hi $hi2
	set wMid1 [expr {($lo1+$hi1)/2.0}]
        set wLen1new [expr {($wLen2/($vLen2+0.0)) * $vLen1}]
        $w1 conf -rangepad 1.05 -lo [expr {$wMid1 - $wLen1new/2}] \
			    -hi [expr {$wMid1 + $wLen1new/2}]
    }
    topdbgmsg -pkg xp.axis.zoom "zoom: end $w1 and $w2"
}

proc xpAxis.AspectCheck { w1 w2 } {
    global xpole
    upvar #0 $w1 avars1 $w2 avars2

    topdbgmsg -pkg xp.axis.zoom "aspect: check on $w1 and $w2"
    # If both axis arnt in linear mode, give up
    if {$avars1(scale)!="lin" || $avars2(scale)!="lin"}	{
	topdbgmsg -pkg xp.axis.zoom "aspect: not linear"
        return
    }

    xpAxis.getlengths $w1 wLen1 vLen1 wMid1
    xpAxis.getlengths $w2 wLen2 vLen2 wMid2

    # See if we even have a chance at unit aspect
    if {$wLen1==0.0 || $vLen1<20 || $vLen2<20} {
	topdbgmsg -pkg xp.axis.zoom "aspect: too small $wLen1 $vLen1 $vLen2"
        return
    }
    
    # First test to see how far off we are in terms of pixels
    set vAdj [xpAxis.abs [expr {($wLen2/($wLen1+0.0)) * $vLen1 - $vLen2}]]
#puts stdout "axis: $w2 vAdj is $vAdj"
    if {$vAdj < 20} {
	topdbgmsg -pkg xp.axis.zoom "aspect: close enough"
        return
    }

    # Now do it for real...
    if [info exist xpole(axis-$w2.wMid)] then {
        set wMid2 $xpole(axis-$w2.wMid)
	unset xpole(axis-$w2.wMid)
    } else {
        set lh2 $avars2(xform)
#        set wMid2 [expr {([lindex $lh2 0] + [lindex $lh2 1])/2.0} ]
    }
    set wLen2new [expr {($wLen1/($vLen1+0.0)) * $vLen2}]
#puts stdout "axis: $w2 $wMid2 $wLen2new"
    $w2 conf -lo [expr {$wMid2 - $wLen2new/2}] -hi [expr {$wMid2 + $wLen2new/2}]
    topdbgmsg -pkg xp.axis.zoom "aspect: done $w2: $wLen2 -> $wLen2new"
}

proc xpAxis.AspectTraceCB { n1 n2 op } {
    global errorInfo
    upvar #0 $n1 avars1
    upvar #0 $avars1(aspect) avars2
    if [catch {xpAxis.AspectCheck $avars2(aspect) $avars1(aspect)} error] then {
	toperror "xpAxis: AspectCheck failed\n$errorInfo"
    }
}
