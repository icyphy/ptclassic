#
# xpplanegraph.tcl :: XP Tk Graph Library
#
# Encapsulation and binding of the XP Plane Graph Widget.
# The core of this is the "xpplanegraph" widget (planeTk.c) which handles
# all the actual mapping and drawing.  This is just provides fluff.
# The user can over-ride the options and binding here; more advanced
# users can provide their own encapsulation function.
#
# We refer to the core window as the XPPlaneGraph; we refer to
# the encapsulation as the XPPlaneWindow.  Those names are also the class
# names.
#
proc xpplanegraph.tcl { } { }


#
# Add global bindings & options
# These are in a proc: the proc is invoked and then destroyed.
# The procs invoked by the bindings are below.
#
proc xpPlane.bindClass { } {
    global XpBackground XpForeground XpActiveBackground XpActiveForeground
    global XpTickColor XpAnnoColor XpNormColor XpSelColor XpInsertBackground

    option add *XPPlaneGraph*background		$XpBackground		30
    option add *XPPlaneGraph*activeBackground	$XpActiveBackground	30
    option add *XPPlaneGraph*insertBackground	$XpInsertBackground	30
    option add *XPPlaneGraph*foreground		$XpForeground		30
    option add *XPPlaneGraph*activeForeground	$XpActiveForeground	30
    option add *XPPlaneGraph.draw.foreground	$XpAnnoColor		30
    option add *XPPlaneGraph.draw.normColor	$XpNormColor		30
    option add *XPPlaneGraph.draw.selColor	$XpSelColor		30

    bind XPPlaneWidget z		"xpPlane.insertSilar %W %x %y 1"
    bind XPPlaneWidget p		"xpPlane.insertSilar %W %x %y -1"
    bind XPPlaneWidget e		"xpPlane.editSilar %W %x %y"
    bind XPPlaneWidget k		"xpPlane.deleteSilar %W %x %y"
    bind XPPlaneWidget <Key-minus>	"xpPlane.powerSilar %W %x %y -1"
    bind XPPlaneWidget <Key-plus>	"xpPlane.powerSilar %W %x %y 1"

    bind XPPlaneWidget <Shift-ButtonPress-1> \
      "xpPlane.rubberBegin %W xpPlane.rubberSelectAdd SelectAdd %x %y"

    bind XPPlaneWidget <ButtonPress>	"xpPlane.buttonPress %W %b %x %y"
    bind XPPlaneWidget <Any-ButtonRelease>	"xpPlane.buttonRelease %W %b %x %y"

    bind XPPlaneWidget <Any-Motion>	"xpPlane.motion %W %x %y"
    bind XPPlaneWidget <Any-Enter>	"xpPlane.enter %W"
    bind XPPlaneWidget <Any-Leave>	"xpPlane.leave %W"
}
xpPlane.bindClass
rename xpPlane.bindClass ""

proc xpPlane.addbind { w menu args } {
    foreach win "$w $w.draw $menu $args" {
	bind $win c [list $menu invoke "Conjugate Pairs"]
	bind $win f [list $menu invoke "Zoom Full"]
	bind $win x [list $menu invoke "Zoom Out"]
	bind $win X [list $menu invoke "Zoom In"]
    }
}

proc xpPlane.addmenu { w menu } {
    set wd $w.draw

    $menu add com -lab "Global Actions:" -state disabled
    $menu add check -lab "Conjugate Pairs" -acc c -var ${w}.draw(conj)
    $menu add com -lab "New Zero (@origin)" -acc "z" \
      -com "xpPlane.insertSilar $wd -1 -1 1"
    $menu add com -lab "New Pole (@origin)" -acc "p" \
      -com "xpPlane.insertSilar $wd -1 -1 -1"
    $menu add com -lab "Zoom Full" -acc "f" -com "xpPlane.grow $wd"
    $menu add com -lab "Zoom Out"  -acc "x" -com "xpPlane.grow $wd 2.0"
    $menu add com -lab "Zoom In"   -acc "X" -com "xpPlane.grow $wd 0.5"

#    $menu add com -lab "Selected Pole/Zero Actions:" -state disabled
    $menu add com -lab "Pole/Zero Actions:" -state disabled
    $menu add com -lab "Edit" -com "xpPlane.editSilar $wd -1 -1" -acc "e"
    $menu add com -lab "Delete" -com "xpPlane.deleteSilar $wd -1 -1" -acc "k"
    $menu add com -lab "Increment power" \
      -com "xpPlane.powerSilar $wd -1 -1 1" -acc "+"
    $menu add com -lab "Decrement power" \
      -com "xpPlane.powerSilar $wd -1 -1 -1" -acc "-"

    # Mouse Button 2 is used for things that require an x/y position.
    set v ${w}.draw(b2mode)
    $menu add com -lab "Mouse Mid Btn Modes:" -state disabled
    $menu add radio -lab "New Zero" -acc "Z" -var $v -val zero
    $menu add radio -lab "New Pole" -acc "P" -var $v -val pole
    $menu add radio -lab "Zoom Region" -acc "R" -var $v -val zoomRegion
}

proc xpPlane.motion { w x y } {
    upvar #0 $w pvars
    set xyloc [$w invmappt "$x $y"]
    xpCplx.Set $pvars(ptrwin) $xyloc
    if { [info exist pvars(motionNotify)] } {
	$pvars(motionNotify) $w $x $y
    }
}

#
# When we leave the drawing window, we want to mark the ptr-loc display
# as stale or invalid.  We used to do this by replacing it with
# dots.  Now we just change the color.
#
proc xpPlane.enter { w } {
    upvar #0 $w pvars
    global XpForeground XpPremptFocus
    $pvars(ptrwin) conf -fg $XpForeground
    if { $XpPremptFocus } {
	focus $w
    }
}
proc xpPlane.leave { w } {
    upvar #0 $w pvars
    # set dots "."
    # In iso8859 fonts, octel char 267 is a small dot near the pixel middle
    # regsub -all . [lindex [$pvars(ptrwin) conf -text] 4] "\267" dots
    # $pvars(ptrwin) conf -text $dots
    global XpDimForeground
    $pvars(ptrwin) conf -fg $XpDimForeground
}

proc xpPlane.setState { w state } {
    upvar #0 $w pvars
    set pvars(state) $state
    if { "$state"=="" } {
	set state $pvars(defstatetext)
    }
    $pvars(statewin) conf -text $state
}

#
# This function returns the current set of selected silars.
# If this set is empty, it will attempt to select the nearest silar at
# the point {x y}.  If this is still empty, a message is displayed
# and an empty list is returned.
#
proc xpPlane.getSelected { w x y } {
    set idxs [$w silarindex select]
    if {[llength $idxs] == 0} then {
	set idxs [$w silarindex "@$x $y"]
	if {[llength $idxs] == 0} then {
    	    puts stdout "No singularities selected."
	} else {
	    $w silarmodify $idxs = = select
	}
    }
    return $idxs
}

proc xpPlane.insertSilar { wd x y power } {
    upvar #0 $wd pvars
    set graph $pvars(graph)
    foreach idx [$wd silarindex all] {
	$wd silarmodify $idx = = !select
    }
    set loc [expr { $x<0 ? "0 0" : [$wd invmappt "$x $y"] } ]
    set yZero [$graph.lAxis xform win draw 0]
    set conjFlag ""
    if { $x>=0 && $pvars(conj) } {
	if { $yZero-8 < $y && $y < $yZero+8 } {
	    set conjFlag ""
	    set loc "[lindex $loc 0] 0"
	} else {
	    set conjFlag "conj"
	}
    }
    $wd silarinsert $loc $power "$conjFlag select"
}

proc xpPlane.deleteSilar { wd x y } {
    set idxs [xpPlane.getSelected $wd $x $y]
    foreach idx $idxs {
        $wd silardelete $idx
    }
}

proc xpPlane.powerSilar { wd x y powdelta } {
    set idxs [xpPlane.getSelected $wd $x $y]
    foreach idx $idxs {
	set curpow [lindex [$wd silarinfo $idx] 1]
	$wd silarmodify $idx = [expr $curpow+$powdelta]
    }
}

proc xpPlane.dragBegin { w x y } {
    upvar #0 $w pvars
    global xpole
    set xpole(plane-silidxs) [xpPlane.getSelected $w $x $y]
    xpPlane.setState $w "Drag"
    set xpole(plane-viewx) $x
    set xpole(plane-viewy) $y
    set xpole(plane-zeroy) [lindex [$w mappt {0 0}] 1]
    set flipCode [expr { $y<=$xpole(plane-zeroy) ? "+" : "-"} ]
    foreach idx $xpole(plane-silidxs) {
	set silar [$w silarinfo "$flipCode $idx"]
	set silarloc [lindex $silar 0]
	$w silarmodify $idx $silarloc
    }
    set pvars(motionNotify) xpPlane.dragMotion
}

proc xpPlane.dragMotion { w x y } {
    upvar #0 $w pvars
    global xpole
    set dx [expr $x-$xpole(plane-viewx)]
    set dy [expr $y-$xpole(plane-viewy)]
#    set flipCode [expr { $y<=$xpole(plane-zeroy) ? "+" : "-"} ]
    foreach idx $xpole(plane-silidxs) {
	set silar [$w silarinfo $idx]
	set curloc [$w mappt [lindex $silar 0]]
	set newloc "[expr [lindex $curloc 0]+$dx] [expr [lindex $curloc 1]+$dy]"
	set wnewloc [$w invmappt $newloc]
	if { $pvars(conj) } {
	    if { [string match "*conj*" [lindex $silar 2]] } {
	        set vd [expr {[lindex $curloc 1] - $xpole(plane-zeroy)} ]
		if { $vd > -5 && $vd < 5 } {
		    $w silarmodify $idx = = !conj
		}
	    } else {
	        set vd [expr {$y - $xpole(plane-zeroy)} ]
		if { $vd < -20 || $vd > 20 } {
		    $w silarmodify $idx = = conj
		} else {
		    set wnewloc "[lindex $wnewloc 0] 0"
		}
	    }
	}
	$w silarmodify $idx $wnewloc
    }
    set xpole(plane-viewx) $x
    set xpole(plane-viewy) $y
}

proc xpPlane.dragEnd { w x y } {
    upvar #0 $w pvars
    xpPlane.setState $w ""
    unset pvars(motionNotify)
}

#
# Get dialog window for controling a silar.  We keep a list of
# old, unmapped ones in pvars(silardlg).  If we cant get an old one,
# then make one.
#
proc xpPlane.GetSilarDlg { wd } {
    upvar #0 $wd pvars

    set dlg [lindex $pvars(silardlg) 0]
    if { "$dlg"!="" } {
	set pvars(silardlg) [lreplace $pvars(silardlg) 0 0]
	return $dlg
    }
    set dlg [topWin.MakeName $wd.silardlg]
    toplevel $dlg
    bind $dlg <Destroy> "topWin.Unset %W"

    pack append $dlg [xpCplx.CreateEntry $dlg.loc \
      {-text Loc} {-rel sunken -width 20} {-rel raised -bd 2}] {top fill}
    pack append $dlg [frame $dlg.row] {top fill}
    pack append $dlg.row [topWin.LbldEntry $dlg.row.pow \
      {-text Power} {-rel sunken -width 4} {-rel raised -bd 2}] \
      {left fill expand}
    pack append $dlg.row [checkbutton $dlg.row.sel \
      -text "Sel" -var ${dlg}(sel) -com "xpPlane.ReadSilarDlg $wd $dlg"] \
      {left fill expand}

    foreach subw "$dlg.loc.val $dlg.row.pow.entry" {
        bind $subw <Return> "xpPlane.ReadSilarDlg $wd $dlg"
	# Would like to use <FocusOut> but doesnt seem to work
        # bind $subw <FocusOut> "xpPlane.ReadSilarDlg $wd $dlg"
    }

    set dd $dlg.buts; pack append $dlg [frame $dd] { top fillx }
    foreach bi {"ok Ok Ok" "update Update Read" "kill Delete Kill" "cancel Cancel Popdown"} {
        pack append $dd [button $dd.[lindex $bi 0] -text [lindex $bi 1] \
          -com "xpPlane.[lindex $bi 2]SilarDlg $wd $dlg"] { left fillx expand }
    }
    return $dlg
}

proc xpPlane.OkSilarDlg { wd dlg } {
    xpPlane.ReadSilarDlg $wd $dlg
    xpPlane.PopdownSilarDlg $wd $dlg
}

proc xpPlane.KillSilarDlg { wd dlg } {
    upvar #0 $dlg dlgvars
    $wd silardelete $dlgvars(silaridx)
    xpPlane.PopdownSilarDlg $wd $dlg
}

proc xpPlane.ReadSilarDlg { wd dlg args } { 
    upvar #0 $dlg dlgvars

    $wd silarmodify $dlgvars(silaridx) [xpCplx.Get $dlg.loc.val] \
      [$dlg.row.pow.entry get] [expr { $dlgvars(sel) ? "" : "!"} ]sel

    # This isnt required: the notifyCmd will do this
    # xpPlane.WriteSilarDlg $wd $dlg
}

proc xpPlane.WriteSilarDlg { wd dlg args } {
    upvar #0 $dlg dlgvars
    set silarinfo [$wd silarinfo $dlgvars(silaridx)]
    xpCplx.Set $dlg.loc.val [lindex $silarinfo 0]
    topEntry.set $dlg.row.pow.entry [lindex $silarinfo 1]
    set dlgvars(sel) [expr { [lsearch $silarinfo sel] > 0 }]
}

proc xpPlane.PopdownSilarDlg { wd dlg } {
    upvar #0 $wd pvars $dlg dlgvars
    set silaridx $dlgvars(silaridx)
    windowunmap $dlg
    $wd silarnotify $silaridx off
    unset pvars(silnotify$silaridx)
    focus none
    lappend pvars(silardlg) $dlg
}

proc xpPlane.BeginSilarDlg { wd idx } {
    upvar #0 $wd pvars

    if { [info exist pvars(silnotify$idx)] } {
	set dlg [lindex $pvars(silnotify$idx) 2]
    } else {
        set dlg [xpPlane.GetSilarDlg $wd]
    }
    upvar #0 $dlg dlgvars
    set dlgvars(silaridx) $idx

    xpPlane.WriteSilarDlg $wd $dlg

    set pvars(silnotify$idx) "xpPlane.WriteSilarDlg $wd $dlg"
    $wd silarnotify $idx on
    set vLoc [$wd mappt [lindex [$wd silarinfo $idx] 0]]
    set rootx [expr {[winfo rootx $wd]+[lindex $vLoc 0]-20} ]
    set rooty [expr {[winfo rooty $wd]+[lindex $vLoc 1]+10} ]
    wm transient $dlg $wd
    wm geom $dlg +$rootx+$rooty
    wm minsize $dlg 50 50
    windowmap $dlg
}

proc xpPlane.editSilar { wd x y } {
    set idxs [xpPlane.getSelected $wd $x $y]
    foreach idx $idxs {
	xpPlane.BeginSilarDlg $wd $idx
    }
}

proc xpPlane.notifyCB { wd idx } {
    upvar #0 $wd pvars

#puts stdout "xpPlane.notifyCB $idx"
    if { [info exist pvars(silnotify$idx)] } {
	eval $pvars(silnotify$idx) {$idx}
    }
}

# proc xpPlane.CheckNotify { wd } {
#    upvar #0 $wd pvars
#    set curOn [expr { "[lindex [$wd conf -notifycommand] 4]"!="" } ]
#    foreach ele [array names pvars] {
#	if { [string match silnotify* $ele] } {
#	    if { ! $curOn } {
#        	$wd conf -notifycommand xpPlane.notifyCB
#	    }
#	    return 1
#	}
#    }
#    if { $curOn } {
#        $wd conf -notifycommand ""
#    }
#    return 0
# }

proc xpPlane.grow { w {factor ""} } {
    if { "$factor"=="" } {
	set xaxiswdg [lindex [$w c -xaxis] 4]
	set yaxiswdg [lindex [$w c -yaxis] 4]
	set bb [$w silarbb all]
	xpAxis.AspectZoom $xaxiswdg [lindex $bb 0] [lindex $bb 2] \
	  $yaxiswdg [lindex $bb 1] [lindex $bb 3]
    } else {
        set xaxiswdg [lindex [$w c -xaxis] 4]
        $xaxiswdg grow $factor
    }
}


proc xpPlane.rubberSelectAdd { w x y width height } {
    if [
	catch {
	    if {$width > 3 && $height > 3} then {
		foreach idx [$w silarindex "<$x $y $width $height"] {
		    $w silarmodify $idx = = select
		}
	    } else {
		$w silarmodify "@$x $y" = = select
	    }
    	}
    ] then {
	puts stdout "No singularites to select"
    }
}

proc xpPlane.rubberSelect { w x y width height } {
    foreach idx [$w silarindex all] {
	$w silarmodify $idx = = !select
    }
    xpPlane.rubberSelectAdd $w $x $y $width $height
}

proc xpPlane.rubberZoom { w x1 y1 width height } {
    set p1 [$w invmappt "$x1 $y1"]
    set p2 [$w invmappt "[expr {$x1 + $width}] [expr {$y1 + $height}]"]
    if {$width < 10 && $height < 10} {
    } else {
#    if {$width >= 10 && $height < 10} then {
#        set xaxiswdg [lindex [$w conf -xaxis] 4]
#        $xaxiswdg conf -lo [lindex $p1 0] -hi [lindex $p2 0]
#    }
#    if {$height >= 10 && $width < 10} then {
#        set yaxiswdg [lindex [$w conf -yaxis] 4]
#        $yaxiswdg conf -lo [lindex $p1 1] -hi [lindex $p2 1]
#    }
#    if {$height >= 10 && $width >= 10} then { }
	xpAxis.AspectZoom \
	  [lindex [$w conf -xaxis] 4] [lindex $p1 0] [lindex $p2 0] \
	  [lindex [$w conf -yaxis] 4] [lindex $p1 1] [lindex $p2 1]
    }
}

proc xpPlane.rubberFunc { w boxType x1 y1 width height } {
    global xpole
    xpPlane.setState $w ""
    $xpole(plane-rubberFunc) $w $x1 $y1 $width $height
}

proc xpPlane.rubberBegin { w func name x y } {
    global xpole
    xpPlane.setState $w $name
    set xpole(plane-rubberFunc) $func
    $w.rubber begin $x $y
}

proc xpPlane.buttonPress { wd butNum x y } {
    upvar #0 $wd pvars

    case $pvars(b${butNum}mode) {
      selectRegion {
        xpPlane.rubberBegin $wd xpPlane.rubberSelect Select $x $y
      }
      selectAddRegion {
        xpPlane.rubberBegin $wd xpPlane.rubberSelectAdd SelectAdd $x $y
      }
      zoomRegion {
       xpPlane.rubberBegin $wd xpPlane.rubberZoom Zoom $x $y
      }
      drag {
        xpPlane.dragBegin $wd $x $y
      }
      zero {
        xpPlane.insertSilar $wd $x $y 1
      }
      pole {
        xpPlane.insertSilar $wd $x $y -1
      }
      default {
	 puts stderr "xpPlane: Unknown mode ``b${butNum}mode''"
      }
    }
}

proc xpPlane.buttonRelease { wd butNum x y } {
    upvar #0 $wd pvars
    case $pvars(b${butNum}mode) {
      drag {
    	xpPlane.dragEnd $wd $x $y
      }
    }
}

proc xpPlane.VarsTraceCB { graph n1 n2 op } {
    upvar #0 $graph.draw pvars
    case $n2 in {
      gain {
	xpCplx.Set $graph.loc.gain.val $pvars(gain)
      }
    }
}

proc xpPlane.GainCB { graph val } {
    upvar #0 $graph.draw pvars
    set pvars(gain) $val
}

proc xpPlane.MakeLocWin { w } {
    upvar #0 $w.draw pvars

    pack append $w [frame $w.loc] {top fill}
    pack append $w.loc [label $w.loc.state] { top filly }
    set pvars(statewin) $w.loc.state
    place [xpCplx.CreateEntry $w.loc.gain \
      [list -text "Gain:"] [list -width 20] {} "" "" "xpPlane.GainCB $w" ] \
      -relx 0.0 -anc nw
    bind $w.loc.gain.val <Any-Enter> {topEntry.Enter %W}
    bind $w.loc.gain.val <Any-Leave> {topEntry.Leave %W}
    xpCplx.Set $w.loc.gain.val 1.0
    place [xpCplx.CreateLabel $w.loc.ptr "Ptr:" ri "%6.^3E"] -relx 1.0 -anc ne
    xpCplx.Set $w.loc.ptr.val "0.0 0.0"
    set pvars(ptrwin) $w.loc.ptr.val

    trace var pvars w "xpPlane.VarsTraceCB $w"
}

proc xpPlane.InitVars { wd } {
    upvar #0 $wd pvars

    set pvars(conj) 1
    set pvars(locfmt) "%.3E"
    set pvars(b1mode) selectRegion
    set pvars(b2mode) zoomRegion
    set pvars(b3mode) drag
    set pvars(silardlg) ""
    set pvars(domain) ""
    set pvars(state) ""
    set pvars(defstatetext) ""
}

#
# Take care to kill off menus which might have traces going, otherwise
# the unsets will fail.
#
proc xpPlane.SafeDestroy { w } {
    topWin.SafeDestroy $w.topbar
    topWin.SafeDestroy $w
}

#
# Encapsulation window.  The {w} passed in should be of class
# XPPlaneWindow for this to work.
#
proc xpSimplePlaneGraph { w } {
    upvar #0 $w.draw pvars
    set pvars(graph) $w
    xpPlane.InitVars $w.draw

    menubutton $w.title -text "Complex Plane" -menu $w.title.m
    pack append $w $w.title { top filly }
    set dd $w.title.m
    menu $dd
    xpPlane.addmenu $w $dd

    xpPlane.MakeLocWin $w

    xpAxisCreate $w.lAxis -side right -flip true
    pack append $w $w.lAxis {left filly}

    xpaxis $w.bAxis -side top
    pack append $w $w.bAxis {bottom fillx}

    xpplanewidget $w.draw -xaxis $w.bAxis -yaxis $w.lAxis
    pack append $w $w.draw {top expand fill}

    $w.lAxis conf -dataWdg $w.draw -command "$w.draw axisxf"
    $w.bAxis conf -dataWdg $w.draw -command "$w.draw axisxf"
    xpAxis.AspectOn $w.lAxis $w.bAxis

    rubber $w.draw.rubber $w.draw -command "xpPlane.rubberFunc $w.draw"
    trace var pvars u "xpSPG.TraceDestroy $w"
}

proc xpSPG.TraceDestroy { graph n1 n2 op } {
#    if [xp.windowExistB $graph] {
#	destroy $graph
#    }
}
