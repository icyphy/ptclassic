#
# xpscale.tcl
#
# Like the Tk scale widget, but supports "doubles" (floating point).
#

proc xpscale.tcl { } { }

proc xpscale { ds args } {
    upvar #0 $ds wvars
    set scale		linear
    set wval		0.5
    set wmin		""
    set wmax		""
    set wlo		0.0
    set whi		1.0
    set command		""
    set orient		horiz
    set width		15
    set length		100
    set autoexpand	1
    set entrywidth	6
    set showvalue	0
    set editvalue	1
    set showlimits	0
    set editlimits	1
    set stacking	right
    set integer		0

    topgetopt { scale wval wmin wmax wlo whi integer \
      command orient width length autoexpand \
      entrywidth showvalue editvalue showlimits editlimits stacking } $args

    #
    # Sanity check arguments for consistency
    #
    if { $editlimits }			{ set showlimits 0 }
    if { $editlimits || $showlimits }	{ set showvalue 1; set stacking top }
    if { $editvalue }			{ set showvalue 0 }

    set wvars(scale)		$scale
    set wvars(wval)		$wval
    set wvars(wlo)		$wlo
    set wvars(whi)		$whi
    set wvars(wmin)		$wmin
    set wvars(wmax)		$wmax
    set wvars(autoexpand)	$autoexpand
    set wvars(entrywidth)	$entrywidth
    set wvars(orient)		$orient
    set wvars(integer)		$integer

    set wvars(command)		""
    set wvars(accept_slider)	1
    set wvars(adjust_slider)	1
    set wvars(adjust_entry)	1

    frame $ds
    scale $ds.scale -from 0 -to 10000 -showvalue 0 \
      -orient $orient -width $width -length $length \
      -command "xpscale.SliderMove $ds"
    pack append $ds $ds.scale "$stacking expand fill"
    bind $ds.scale x "xpscale.Grow $ds 2.0"
    bind $ds.scale X "xpscale.Grow $ds 0.5"
    bind $ds.scale L "xpscale.EntryPopup $ds wlo"
    bind $ds.scale H "xpscale.EntryPopup $ds whi"

    set wvars(valuefunc) topNull
    if { $showvalue } {
	label $ds.wval
	set wvars(valuefunc) topLabel.set
    }
    if { $editvalue } {
        entry $ds.wval -rel sunken -width $entrywidth
	set wvars(valuefunc) topEntry.set
        xpscale.EntryBind $ds wval
    }
    if { $showvalue || $editvalue } {
        pack append $ds $ds.wval "$stacking"
    }

    set wvars(limitsfunc) topNull
    if { $showlimits } {
        label $ds.wlo
        label $ds.whi
	set wvars(limitsfunc) topLabel.set
    }
    if { $editlimits } {
        entry $ds.wlo -rel sunken -width $entrywidth
        xpscale.EntryBind $ds wlo
        entry $ds.whi -rel sunken -width $entrywidth
        xpscale.EntryBind $ds whi
	set wvars(limitsfunc) topEntry.set
    }
    if { $showlimits || $editlimits } {
        $wvars(limitsfunc) $ds.wlo $wvars(wlo)
        $wvars(limitsfunc) $ds.whi $wvars(whi)
        place $ds.wlo -in $ds -relx 0.0 -rely 1.0 -anc sw
        place $ds.whi -in $ds -relx 1.0 -rely 1.0 -anc se
    }

    trace var wvars(wval)	w "xpscale.ConfigTraceWvalCB $ds"
    trace var wvars(wlo)	w "xpscale.ConfigTraceLimsCB $ds"
    trace var wvars(whi)	w "xpscale.ConfigTraceLimsCB $ds"

    rename $ds $ds.frame
    proc $ds { cmd args } [concat xpscale.Cmds $ds \$cmd \$args]

    #
    # Trigger an update of the scale and label/entry value
    # However, make sure we dont trigger the command until after this
    #
    set wvars(wval) $wval
    set wvars(command)		$command
    return $ds
}

proc xpscale.Cmds { ds cmd theargs } {
    upvar #0 $ds wvars
#puts stdout "xpscale.Cmds $ds $cmd $theargs"
    case $cmd {
      set {
	set wvars(wval) $theargs
      }
      get {
	return $wvars(wval)
      }
    }
}

proc xpscale.EntryPopup { ds which } {
    upvar #0 $ds wvars

    if { [info exist wvars(limitpopup)] } {
	set popup $wvars(limitpopup)
    } else {
	set wvars(limitpopup) $ds.limpop
	set popup $wvars(limitpopup)
	toplevel $popup
	wm transient $popup $ds
	entry $popup.entry -relief sunken -width $wvars(entrywidth)
	pack append $popup $popup.entry { top }
	windowmap $popup
    }

    bind $popup.entry <Escape> "focus none; windowunmap $popup"
    bind $popup.entry <Return> "xpscale.EntryPopupDone $ds $which"

    case $wvars(orient) {
      h* {
	set layout [list						\
	  [list 0 0]							\
	  [list [expr [winfo width $ds]-[winfo width $popup]] 0]	\
	]
      }
      v* {
	set layout [list						\
	  [list 0 0]							\
	  [list [expr [winfo height $ds]-[winfo height $popup]] 0]	\
	]
      }
    }
    case $which {
      wlo {
	set loc [lindex $layout 0]
      }
      whi {
	set loc [lindex $layout 1]
      }
    }

    append geom + [expr {[lindex $loc 0]+[winfo rootx $ds]}] \
		+ [expr {[lindex $loc 1]+[winfo rooty $ds]}]
    wm geom $popup $geom
    windowmap $popup
    focus $popup.entry
}

proc xpscale.EntryPopupDone { ds which } {
    upvar #0 $ds wvars

    focus none
    set popup $wvars(limitpopup)
    windowunmap $popup
    set entryval [$popup.entry get]
    set wvars($which) $entryval
}

proc xpscale.EntryBind { ds which } {
    upvar #0 $ds wvars

    bind $ds.$which <Return> "xpscale.EntryDone $ds $which"
    bind $ds.$which <Escape> "xpscale.EntryRestore $ds $which"
}

proc xpscale.EntryRestore { ds which } {
    upvar #0 $ds wvars

    focus none
    topEntry.set $ds.$which $wvars($which)
}

proc xpscale.EntryDone { ds which } {
    upvar #0 $ds wvars

    focus none
    set entryval [$ds.$which get]
    set wvars(adjust_entry) 0
    set wvars($which) $entryval
    if { "$entryval"!="$wvars($which)" } {
        topEntry.set $ds.$which $wvars($which)
    }
    set wvars(adjust_entry) 1
}

#
# For derivation and explaination of algorithm here, see the
# function xpAxisPadRange() in "tkgraph/axisTk.c".
# Denote the original window length (hi - lo) as {w} and the new as {w'}.  
# For linear:	w' = factor * w, and factor must be positive
#	1.0 ==> no change
#	2.0 ==> double the win range
#	0.5 ==> halve the win range
#
proc xpscale.Grow { ds factor } {
    upvar #0 $ds wvars

    case $wvars(scale) {
      "lin*" {
	set wDelta [expr { ($wvars(whi) - $wvars(wlo)) * ($factor-1) / 2.0 }]
	set wvars(wlo) [expr {$wvars(wlo) - $wDelta}]
	set wvars(whi) [expr {$wvars(whi) + $wDelta}]
      }
      default {
	error "scale: Unknown scale ``$wvars(scale)''"
      }
    }
}

proc xpscale.SliderMove { w vVal } {
    upvar #0 $w wvars

    if { $wvars(accept_slider) } {
	set wVal [xpscale.ViewToWin $w $vVal]
#puts stdout "scale: $vVal $wVal $wvars(wlo) $wvars(whi)"
	set wvars(adjust_slider) 0
	set wvars(wval) $wVal
	set wvars(adjust_slider) 1
    }
}

#proc xpscale.SetValLocked { semaphore varname value } {
#    upvar $semaphore sema 
#    set oldsema
#}

proc xpscale.WarpValue { ds val } {
    upvar #0 $ds wvars

    if { $wvars(integer) } {
	set val [dflt -int rint $val]
    }
    if { "$wvars(wmin)"!="" && $val < $wvars(wmin) } {
	set val $wvars(wmin)
    }
    if { "$wvars(wmax)"!="" && $val > $wvars(wmax) } {
	set val $wvars(wmax)
    }
    return $val
}

#
# This is tricky.  The entry interface has more resolution than the
# slider, thus when the wval entry is changed, we want to set
# the slider to the new approx loc. cooresponding to the entered value,
# but actually use the entered value.  In contrast, when the end points
# are moved we want to take the value from the slider so that it can
# perform limiting for us.
# Note that setting the slider will will cause a CB to SliderMove...
# we need to discard this event for the wval entry, and use it for the limits.
#
proc xpscale.ConfigTraceWvalCore { ds } {
    upvar #0 $ds wvars

    set wvars(wval) [xpscale.WarpValue $ds $wvars(wval)]
# puts stdout "xpscale.ConfigTraceWvalCore: warped $wvars(wval)"
    set wval $wvars(wval)
    if { $wvars(autoexpand) } {
	if { $wval < $wvars(wlo) } {
	    set wvars(adjust_entry) 1
	    set wvars(wlo) $wval
	}
	if { $wval > $wvars(whi) } {
	    set wvars(adjust_entry) 1
	    set wvars(whi) $wval
	}
    }
    if { $wvars(adjust_slider) } {
	set wvars(accept_slider) 0
        $ds.scale set [xpscale.WinToView $ds $wval]
	set wvars(accept_slider) 1
    }
    if { $wvars(adjust_entry) } {
        $wvars(valuefunc) $ds.wval $wval
    }
    if { "$wvars(command)"!="" } {
	if [catch {uplevel #0 $wvars(command) $wval} error] {
	    tkerror "xpscale: command failed (wval changed)"
	}
    }
}

#
# This is the trace callback.  We call the "real" function inside a catch
# so that any syntax errors may be reported and not swallowed by the
# trace mechanism.
#
proc xpscale.ConfigTraceWvalCB { ds n1 n2 op } {
    if [catch {xpscale.ConfigTraceWvalCore $ds} error] {
	tkerror "xpscale: Programming error in trace CB:\n$error"
    }
}

proc xpscale.ConfigTraceLimsCB { ds n1 n2 op } {
    upvar #0 $ds wvars

    set wval [set wvars($n2) [xpscale.WarpValue $ds $wvars($n2)]]
    if { $wvars(adjust_slider) } {
        set oldcom [lindex [$ds.scale conf -com] 4]
        $ds.scale conf -com topNull
        $ds.scale set [xpscale.WinToView $ds $wvars(wval)]
        $ds.scale conf -com $oldcom
    }
    if { $wvars(adjust_entry) } {
        $wvars(limitsfunc) $ds.$n2 $wval
    }
}


proc xpscale.ViewToWin { w vVal } {
    upvar #0 $w wvars

    if { [set wLen [expr { $wvars(whi) - $wvars(wlo) }]] <= 0 } {
	return $wvars(wlo)
    }
    case $wvars(scale) {
      "lin*" {
	return [expr { $vVal / 10000.0 * $wLen + $wvars(wlo)} ]
      }
    }
    error "scale: Unknown scale ``$wvars(scale)''"
}

proc xpscale.WinToView { w wVal } {
    upvar #0 $w wvars

    if { [set wLen [expr { $wvars(whi) - $wvars(wlo) }]] <= 0 } {
	return $wvars(wlo)
    }
# puts stderr "xpscale.WinToView: $wVal - $wvars(wlo)"
    case $wvars(scale) {
      "lin*" {
	return [dflt -int rint [expr { ($wVal - $wvars(wlo)) * 10000.0 / $wLen } ] ]
      }
    }
    error "scale: Unknown scale ``$wvars(scale)''"
}

