#
# topwin.tcl :: Misc helper function for tk windows
#
# Note that the "top" doesn't refer to toplevel windows, but rather
# to my "top" library.
#

proc topwin.tcl { } { }

#
# Return true if window {w} exists.  There is some amount of subtlety here
# because window deletion is a multi-stage processes, and some aspects
# of the window disappear before others.
#
proc topWin.ExistB { w } {
    return [expr { "[info commands $w]"!="" && ![catch {winfo parent $w}] } ]
}

proc topWin.SafeDestroy { w } {
    if [topWin.ExistB $w] {
	destroy $w
    }
}

proc topWin.IdleDestroy { w } {
    after 1 "topWin.SafeDestroy $w"
}

#
# Derive a new window name from {w}.  Note that we DONT just
# use an incrementing serial number.  The returned value is a two-element list:
# the first element is a window name guarenteed not to exist, and the second
# is intended to be a human-digestable suffix to distingish this window.
#
proc topWin.GenName { w } {
    set w [string tolower [string trim $w]]
    regsub -all "\[\]\[,() \t\]" $w {-} w
    if { ! [topWin.ExistB $w] } {
	return [list $w ""]
    }
    for {set sn 2} {[topWin.ExistB $w-$sn]} {incr sn} { }
    return [list $w-$sn #$sn $w]
}

#
# This just returns the window name from the above function, dropping
# the human-digestible suffix.
#
proc topWin.MakeName { w } {
    return [lindex [topWin.GenName $w] 0]
}

#
# Helper to deal with the screwed up "." only window name.
#
proc topWin.MergeNames { parent window } {
    if { "$parent"=="." } {
	return ".$window"
    }
    return "$parent.$window"
}

#
# topWin.TopWrap widget_cmd ?-name wdgname? \
#   ?-toppath toppath? ?-subpath subpath? \
#   ?-geometry geometry? ?-title title? ?-iconname iconname? \
#   ?-? wdg_args...
# Create a toplevel window with a normal widget inside.  The widget is
# thus "wrapped" by a toplevel window. {widget_cmd} and all remaining
# unparsed options are used as a command to create the wrapped widget:
#	widget_cmd fullpath remaining_args
# where fullpath is constructed as described below.
# 
# The {wdgname} argument may be: {} or {subpath} or {toppath subpath}.
# The caller may also directly specify {toppath} and/or {subpath}; if
# so, {name} will override the portion it specifies.
# If {subpath} has a leading ``.'', the {toppath} will be derived from it.
# {toppath}, if specified, must be a valid window name, and its parent
# must exist.  However, {toppath} may already exist, in which case a
# suffix will be appended.
#
# The single "-" argument can be used to seperate argument to this procedure
# from arguments to {widget_cmd}.  Its use is not required.
# 
proc topWin.TopWrap { widget_cmd args } {
    set name		""
    set toppath		""
    set toppathvar	""
    set subpath		""
    set geom		""
    set title		""
    set iconname	""

    set optlist { name toppath subpath toppathvar
      {geom} {geometry geom} title iconname }
    set args [topgetopt -any $optlist $args]

    case [llength $name] {
      0 { }
      1 { set subpath [lindex $name 0] }
      2	{ set toppath [lindex $name 0] ; set subpath [lindex $name 1] }
      default {
	error "topWin.TopWrap: invalid name: ``$name''"
      }
    }
    if { "[string index $subpath 0]"=="." } {
	set mid [string last "." $subpath]
	set toppath [string range $subpath 0 [expr {$mid - 1}]]
	set subpath [string range $subpath [expr {$mid + 1}] end]
    }
    if { "$toppath"=="." || "$toppath"=="" } {
	set toppath .top
    }
    if { "$subpath"=="" } {
	set subpath w
    }
    set topinfo [topWin.GenName $toppath]
    set toppath [lindex $topinfo 0]
    set toplbl [lindex $topinfo 1]
    toplevel $toppath
    set subpath [topWin.MakeName $toppath.$subpath]
    if { "$toppathvar"!="" } {
        upvar $toppathvar pv
	set pv $toppath
    }

    set wdg [eval {$widget_cmd} {$subpath} $args]
    pack append $toppath $subpath { top expand fill }

    if { "$geom"!="" } {
        wm geom $toppath $geom
    }
    if { "$title"!="" } {
	if { "$toplbl"!="" } {
	    append title " $toplbl"
	}
        wm title $toppath "$title"
    }
    if { "$iconname"!="" } {
	if { "$toplbl"!="" } {
	    append iconname " $toplbl"
	}
        wm iconname $toppath "$iconname"
    }
    wm minsize $toppath 20 20
    # global $wdg
    # trace var $wdg u "topWin.DestroyOnUnsetCB $wdg"
    return $wdg
}

proc topWin.DestroyOnUnsetCB { wdglist n1 n2 op } {
    foreach w $wdglist {
        topWin.SafeDestroy $w
    }
    foreach w $wdglist {
	global $w
	catch {unset $w}
    }
}

proc topWin.LbldEntry { w {lblargs ""} {entryargs ""} {frameargs ""} } {
    eval frame $w $frameargs
    
    pack append $w [eval label {$w.label} $lblargs] { left }
    pack append $w [eval entry {$w.entry} $entryargs] { left expand fill }
    bind $w.entry <Destroy> "topWin.Unset %W"

    upvar #0 $w.entry wvars
    if [info exist wvars(text)] {
        $w insert 0 $text
    } else {
	set wvars(text) ""
    }
    return $w
}

proc topWin.BindTree { specs seq com } {
    foreach spec $specs {
	bind $spec $seq $com
	topWin.BindTree [winfo children $spec] $seq $com
    }
}

proc topEntry.set { w text } {
    upvar #0 $w wvars
    set wvars(text) $text
    $w delete 0 end
    $w insert 0 $text
}
proc topEntry.restore { w } {
    upvar #0 $w wvars
    $w delete 0 end
    $w insert 0 $wvars(text)
    focus none
}
proc topEntry.return { w } {
    upvar #0 $w wvars
    set wvars(text) [$w get]
    focus none
}
proc topEntry.deltaCursor { w delta } {
    $w icursor [expr {[$w index insert]+$delta}]
    tk_entrySeeCaret $w
}
bind Entry <Key-Left> "topEntry.deltaCursor %W -1"
bind Entry <Key-Right> "topEntry.deltaCursor %W 1"
bind Entry <Key-Escape> "topEntry.restore %W"
bind Entry <Key-Return> "topEntry.return %W"

proc topLabel.set { w text } {
    $w conf -text $text
}

proc topWin.Unset { w } {
    upvar #0 $w wvars
    catch {unset wvars}
    return ""
}

proc topWin.DestroyChildren { w } {
    foreach wkid [winfo children $w] {
	catch {destroy $wkid}
    }
}

proc topWin.MakeDevNullWin {} {
    if [winfo exist .devnull] return
    toplevel .devnull
    wm geom .devnull =1x1+0+0
    bind .devnull <Any-Key> {puts stdout "Please wait..."}
    bind .devnull <Any-Button> {puts stdout "Please wait..."}
    tkwait vis .devnull
}


proc topWin.GetTopLevel { w args } {
    set ret [eval toplevel {$w} $args]
    return $ret
}

# The procedure below may be invoked when the mouse pointer enters a
# button widget.  It records the button we're in and changes the
# state of the button to active unless the button is disabled.
proc topEntry.Enter w {
    global tk_priv tk_strictMotif XpActiveBackground
    if {[lindex [$w config -state] 4] == "normal"} {
	if {!$tk_strictMotif} {
	    $w config -background $XpActiveBackground
	}
    }
}

# The procedure below may be invoked when the mouse pointer leaves a
# button widget.  It changes the state of the button back to
# inactive.
proc topEntry.Leave w {
    global tk_priv tk_strictMotif XpBackground
    if {!$tk_strictMotif} {
	$w config -background $XpBackground
    }
}


#
# Traverse {menu}, and install accelerators onto {winSpec}.
# Accelerator sequences may be any sequence of "normal" characters,
# or a normal char prefixed by "^" for Control.
#
proc topWin.BindAccels { winSpec menu {cmdPrefix ""} } {
    if { "$cmdPrefix"=="" } {
	set cmdPrefix $menu
    }
    set lastIdx [$menu index last]
    for {set idx 0} {$idx <= $lastIdx} {incr idx} {
	if [catch {$menu entryconf $idx -acc} acc] continue
	set acc [lindex $acc 4]
	if { "$acc"!="" && "$acc"!="==>" } {
    	    regsub -all "\\^(.)" $acc "<Control-\\1>" acc
    	    regsub -all "<(.)>" $acc "<Key-\\1>" acc
	    bind $winSpec $acc "$cmdPrefix invoke $idx"
	}
	if { ! [catch {$menu entryconf $idx -menu} submenu] } {
	    set submenu [lindex $submenu 4]
	    if { "$submenu"!="" } {
	        topWin.BindAccels $winSpec $submenu $cmdPrefix.[winfo name $submenu]
	    }
	}
    }
}

proc topWin.BindAccelsOnce { winSpec menu {cmdPrefix ""} } {
    global top
    if [info exist top(bindaccel-$winSpec)] return
    set top(bindaccel-$winSpec) 1
    topWin.BindAccels $winSpec $menu $cmdPrefix
}

proc topWin.TraceWindowOptions { win args } {
    upvar #0 $win wvars 

    if { "$args" == "" } {
	set optinfos [$win conf]
	foreach optinfo $optinfos {
	    lappend args [lindex $optinfo 0]
	}
    }
    set wvars(optionlist) $args
    foreach opt $args {
#puts stdout "TraceWindowOptions: $win $opt"
	set wvars($opt) [lindex [$win conf -$opt] 4]
	trace var wvars($opt) r "_topWin.TraceWO_R $win"
	trace var wvars($opt) w "_topWin.TraceWO_W $win"
	trace var wvars($opt) u "_topWin.TraceWO_U $win"
    }
    set wvars(optiontrace) 1
}

proc topWin.ResetWindowTraces { win args } {
    upvar #0 $win wvars 
    if { "$args" == "" } {
	set args $wvars(optionlist)
    }
    set wvars(optiontrace) 0
    foreach opt $args {
	set wvars($opt) [lindex [$win conf -$opt] 4]
    }
    set wvars(optiontrace) 1
}

proc _topWin.TraceWO_R { win n1 n2 op } {
    upvar #0 $win wvars
    set wvars($n2) [lindex [$win conf -$n2] 4]
#puts stdout "TraceWO_R: read $win $n1 $n2 ``$wvars($n2)''"
}

proc _topWin.TraceWO_W { win n1 n2 op } {
    upvar #0 $win wvars
    if { $wvars(optiontrace) && "$wvars($n2)"!="[lindex [$win conf -$n2] 4]" } {
#puts stdout "TraceWO_W: write $win $n1 $n2 ``$wvars($n2)''"
        $win conf -$n2 $wvars($n2)
    }
}

proc _topWin.TraceWO_U { win n1 n2 op } {
#puts stdout "TraceWO_U: unset $win $n1 $n2\n[info level -1]"
}
