#
# xpdefvals.tcl
#
# Default values (mainly color) for xp widgets.  The xp widget scripts
# use the tcl variables here to configure the colors of widgets
# via the option resources.  (e.g., the variables defined here are
# used in "option add" statements in the widget tcl code.
# This provides a central place to control the color of widgets.
# If any variable is already defined, it will not be over-written;
# thus applications can easily change colors globally.
#
# This file should be automatically produced from the xpDefaults.h file
# (or v.v.), but I dont have enough experience yet.
#
proc xpdefvals.tcl { } { }

global TopColorScreenB
if ![info exists TopColorScreenB] then {
    set TopColorScreenB [expr {[windowdepth] > 1}]
}
global TopColorReverseB
if ![info exists TopColorReverseB] then {
    set TopColorReverseB 0
}


proc xpDefVal.Color { color bw } {
    global TopColorScreenB TopColorReverseB
    if { $TopColorScreenB || "$bw"=="" } {
	return $color
    }
    if $TopColorReverseB {
	case $bw in {
	  Black { set bw White }
	  White { set bw Black }
	}
    }
    return $bw
}

proc xpDefVal.CondSet { var color { bw "" } } {
    global $var
    if { ! [info exists $var] } then {
	set $var [xpDefVal.Color $color $bw]
    }
}

proc xpSetDefVals { } {
    # If {XpPremptFocus} is true, then we are using a click-to-focus model.
    # Certain windows with many keybindings will "steal" the focus when
    # the mouse pointer enters.  This is not desirable, but it works.
    global XpPremptFocus
    if { ! [info exist XpPremptFocus] } {
	if [catch {focus pointer on}] {
#	    puts stderr "Warning: focus-follows-pointer mode not available."
	    set XpPremptFocus 1
	} else {
	    focus none
	    set XpPremptFocus 0
	}
    }

    # If {XpPropagateKeys} is true, then the tk-core will propgate key events
    # up to through our parents.  If this is not available, we must
    # replicate the bindings on all children of the highest level window
    # where the bindings apply.
    global XpPropagateKeys
    if { ! [info exist XpPropagateKeys] } {
	if [catch {focus propagate on}] {
#	    puts stderr "Warning: key propagation mode not available."
	    set XpPropagateKeys 0
	} else {
	    set XpPropagateKeys 1
	}
    }

    # Normal font the built-in Tk default
    xpDefVal.CondSet XpNormalFont "*-Helvetica-Bold-R-Normal--*-120-*"
    xpDefVal.CondSet XpLabelFont "*-Helvetica-Medium-R-Normal--*-120-*"
    xpDefVal.CondSet XpTitleFont "*-Helvetica-Bold-R-Normal--*-140-*"
    xpDefVal.CondSet XpFixedFont "*-Fixed-Bold-R-Normal--*-120-*"

    global XpTitleFont
    option add *title.Font $XpTitleFont 30

    # Background: should be pretty obvious
    xpDefVal.CondSet XpBackground	"#600040"	"Black"
    # ActiveBackground: active background (buttons)
    xpDefVal.CondSet XpActiveBackground	"#800060"	"Black"
    # Foreground: generic text
    xpDefVal.CondSet XpInsertBackground	"#0f0"		"Black"
    # xpDefVal.CondSet XpForeground	"#f0b0f0"	"White"
    xpDefVal.CondSet XpForeground	"#fb4"		"White"
    xpDefVal.CondSet XpDimForeground	"#b81"		"White"
    # ActiveForeground: generic active text (buttons)
    xpDefVal.CondSet XpActiveForeground	"#fb4"		"White"
    # TickColor: axis widget labels & ticks
    #xpDefVal.CondSet XpTickColor	"#f0b0f0"	"White"
    # xpDefVal.CondSet XpTickColor	"#9f0"		"White"
    xpDefVal.CondSet XpTickColor	"#8df"		"White"
    xpDefVal.CondSet XpActiveTickColor	"#8be"		"White"
    # AnnoColor: unit circle & axes lines
    xpDefVal.CondSet XpAnnoColor	"#f77"		"White"
    # NormColor: color of a "normal" object
    xpDefVal.CondSet XpNormColor	"#ffffff"	"White"
    # SelColor: color of a selected object
    xpDefVal.CondSet XpSelColor		"#00ff00"	"White"
}
xpSetDefVals
rename xpSetDefVals ""

proc xpFocus.enter { w } {
    global XpPremptFocus
    if $XpPremptFocus {
	focus $w
    }
}
