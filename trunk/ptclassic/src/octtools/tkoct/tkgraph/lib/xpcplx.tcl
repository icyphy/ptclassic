#
# xpcplx.tcl -- support for widgets that deal with
# complex values.  This really should be part of the eem library, but...
#

global XpFixedFont
option add *XPCplx.val.Font $XpFixedFont 30

proc xpcplx.tcl { } { }

#
# Display a titled complex value label.  The title {lbl} is displayed
# to the left.  The title is actually a menu button with an assocciated
# menu.  The menu (and keybindings) may be used to control how
# the complex number is displayed.
#
proc xpCplx.CreateLabel { w lbl {coord "ri"} {fmt "%g"} } {
    frame $w -class XPCplx
    pack append $w [menubutton $w.lbl -text $lbl] { left expand fill }
    pack append $w [label $w.val] { left expand fill }
    xpCplx.Config $w.val label $coord $fmt
    xpCplx.Menu $w.val $w.lbl.m
    $w.lbl conf -menu $w.lbl.m
    return $w
}

#
# Display a titled complex value entry field.  Same as above, except
# that the entry widget can be modified.
#
proc xpCplx.CreateEntry { w titleargs entryargs {frameargs ""} 
  {coord ""} {fmt ""} {command ""} } {
    upvar #0 $w.val wvars 
    if { "$coord" == "" } {
	set coord "ri"
    }
    if { "$fmt" == "" } {
	set fmt "%g"
    }
    eval frame {$w} -class XPCplx $frameargs
    pack append $w [eval menubutton {$w.lbl} $titleargs] { left fill }
    pack append $w [eval entry {$w.val} $entryargs] { left expand fill }
    xpCplx.Config $w.val entry $coord $fmt
    xpCplx.Menu $w.val $w.lbl.m
    $w.lbl conf -menu $w.lbl.m
    # Escape is handled by the lower-level topEntry.* funcs
    if { "$command" != "" } {
	set wvars(command) $command
    }
    bind $w.val <Return> "xpCplx.Ok $w.val"
    return $w
}


proc xpCplx.Config { w type {coord "ri"} {fmt "%g"} } {
    upvar #0 $w wvars 
    case $type {
      entry {
	set wvars(setfunc) topEntry.set
      }
      default {
	set wvars(setfunc) topLabel.set
      }
    }
    set wvars(coord) $coord
    set wvars(fmt) $fmt
    set wvars(val) 0
    xpCplx.Bind $w $w
    trace var wvars w "xpCplx.TraceCB $w"
    bind $w <Destroy> "topWin.Unset %W"
}

proc xpCplx.Bind { w var } {
    bind $w <Control-r> "set ${var}(coord) ri"
    bind $w <Control-p> "set ${var}(coord) mpd"
    bind $w <Control-P> "set ${var}(coord) mpr"
}

proc xpCplx.Menu { w menu } {
    menu $menu
    $menu add radio -lab "Rectangular" -acc "^r" -var ${w}(coord) -val "ri"
    $menu add radio -lab "Polar (deg)" -acc "^p" -var ${w}(coord) -val "mpd"
    $menu add radio -lab "Polar (rad)" -acc "^P" -var ${w}(coord) -val "mpr"
    xpCplx.Bind [winfo parent $menu] $w
}

proc xpCplx.Get { w } {
    return [cplx ident [$w get]]
}

# 
# Invoked whenever <Return> is pressed in entry widget.  Get the value
# into real/imag form, verify that it is ok, and set variable.
#
proc xpCplx.Ok { w } {
    upvar #0 $w wvars
    if [catch {xpCplx.Get $w} res] {
	puts stdout "Error: Bad complex number: $res"
	return
    }
    focus none
    if { [info exist wvars(command)] } {
	uplevel #0 $wvars(command) [list $res]
    }
}

proc xpCplx.SetWdg { w val } {
    upvar #0 $w wvars
    set coord $wvars(coord)
    case $coord {
      mpr	{ set fmt "$wvars(fmt) %5.2f" }
      mpd	{ set fmt "$wvars(fmt) %4.0f" }
      default	{ set fmt $wvars(fmt) }
    }
    set fmtval [cplx -fmt $fmt -$coord ident $val]
# puts stdout "xpCplx: ``$fmtval''"
    $wvars(setfunc) $w $fmtval
}

proc xpCplx.TraceCB { w n1 n2 op } {
    upvar #0 $w wvars
    if [catch {xpCplx.SetWdg $w $wvars(val)} error] {
	tkerror "xpCplx.Trace: $w: invalid complex number"
    }
}

proc xpCplx.Set { w val } {
    upvar #0 $w wvars
    set wvars(val) $val
}

proc xpCplx.Restore { w } {
    upvar #0 $w wvars
    xpCplx.SetWdg $w $wvars(val)
}
