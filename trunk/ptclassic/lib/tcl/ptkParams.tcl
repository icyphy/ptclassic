# $Id$

# Edit-Parameters Dialog Box

# For storage, there exist two global array--one which stores lists of
# current parameter-type-value triplets and the other which (upon
# invocation of an edit-parameters command) stores the original triplet
# values as back-ups.

# Width of entry in average-sized chars of font
set ed_MaxEntryLength 60

# Global variable containing information whether the entry has been destroyed
#  Should be unique
set ed_EntryDestroyFlag 0

# Global variable containing the number for a toplevel window
set ed_ToplevelNumbers(ed_Num) 0

# Entry Bindings in addition to the start-up bindings defined
#  in tk.tcl: , , , ,  + various mouse-related bindings
# Old bindings if any are overwritten

# Useless bindings:
#    bind Entry <Control-Key> "+ [bind Entry <Any-Key>]"
#    bind Entry <Control-t> {ed_AddScroll %W}

    bind Entry <Control-f> {+
	%W icursor [expr [%W index insert]+1]; tk_entrySeeCaret %W
    }
    bind Entry <Control-b> {+
	%W icursor [expr [%W index insert]-1]; tk_entrySeeCaret %W
    }
    bind Entry <Control-a> {+
	%W icursor 0; tk_entrySeeCaret %W
    }
    bind Entry <Control-e> {+
	%W icursor end; tk_entrySeeCaret %W
    }
    bind Entry <Control-k> {+
	%W delete insert end; tk_entrySeeCaret %W
    }
    bind Entry <Control-d> {+
	%W delete insert; tk_entrySeeCaret %W
    }

# An attempt at binding <space> to deleting the current selection
#    bind Entry <space> {
#	if {([selection own] != "") && \
#		([%W index sel.first] != [%W index sel.last])} {
#		%W delete sel.first sel.last
#		%W select clear; selection clear %W
#	} else {
#		if {"%A" != ""} {
#			%W insert insert %A
#			tk_entrySeeCaret %W
#			ed_CheckForChange %W
#			%W select clear; selection clear %W
#		}
#	}
#    }

# **ed_RestoreParam
# Discards all changes

proc ed_RestoreParam {facet number} {
    global paramBAKArray
    ptkSetParams $facet $number $paramBAKArray($facet,$number)
}

# **proc ed_SetEntryButtons
# Procedure to make buttons function in the same manner as the scrollbar
#  widget.  Configures the buttons according to the position and view in the
#  entry widget.  Takes a frame containing the entry and buttons as the first
#  argument.

proc ed_SetEntryButtons {frame numStor numPossVis leftIdx rightIdx} {
    if {$leftIdx > 0} {
	$frame.left config -fg black
    } else { $frame.left config -fg [lindex [$frame.left config -bg] 4] }
    if {$rightIdx < [expr $numStor-1]} {
	$frame.right config -fg black
    } else { $frame.right config -fg [lindex [$frame.right config -bg] 4] }
}	

proc ed_MkEntryButton {frame label} {
	global ed_MaxEntryLength
	pack append [frame $frame -bd 2] \
	   [label $frame.label -text "$label:  " -anchor w] left \
[button $frame.right -bitmap @~whuang/but_entry/right -relief flat -command \
	     "$frame.entry view \[expr \[$frame.entry index @0\]+1\]"] right \
[button $frame.left -bitmap @~whuang/but_entry/left -relief flat -command \
	     "$frame.entry view \[expr \[$frame.entry index @0\]-1\]"] right
	pack before $frame.left \
	   [entry $frame.entry -scroll "ed_SetEntryButtons $frame" \
		-relief sunken -width $ed_MaxEntryLength] {right}

# Try a percentage:
#
#	bind $frame.right <3> \
#	     "$frame.entry view \[expr \[$frame.entry index @0\]+1\]"
#	bind $frame.left <3> \
#	     "$frame.entry view \[expr \[$frame.entry index @0\]-1\]"

}

# Procedure to check whether the entry has overflowed.  Appends a '*' to the
#  label if this is the case.
# Need to bind all editing keys in each entry to execute this procedure.
#proc ed_CheckForChange e {
#	set label [file root $e].l
#	set name [lindex [$label config -text] 4]
#	set numChars [$e index end]
#	set lastIndex [$e index @[winfo width $e]]
#	set firstIndex [$e index @0]
#	if {[regsub {\*$} "$name" {} ltext]==1} {
#		if {(($lastIndex == $numChars) && ($firstIndex == 0)) || \
#			([expr $lastIndex-$firstIndex] > $numChars) } {
#			$label config -text $ltext
#		}
#		ed_AddScroll $e
#	} else {
#		if {($firstIndex > 0) || ($numChars > $lastIndex)} {
#			ed_AddScroll $e
#			$label config -text "${name}*"
#		}
#	}
#}

# **ed_UpdateParam
# Compares with previous parameter types and values; if they differ,
#  then the new values are written.

proc ed_UpdateParam {facet number name args} {
   global paramArray
   if {[llength $args] == 2} {
	set value [lindex $args 1]
	set type [list [lindex $args 0]]
   } else {
	set value [lindex $args 0]
   }
   set count 0
   foreach param $paramArray($facet,$number) {
        if {[lindex $param 0] == $name} {
                if {[lindex $param 2] != $value} {
		   set okay 1
		} else {set okay 0}
		if {[llength $args] == 2} {
		   if {[lindex $param 1] != $type} {
			set okay 1
		   }
		} else { set type [lindex $param 1] }
		if {$okay} {
		 set param [lreplace $param 1 2 $type $value]
#                 puts "**$paramArray($facet,$number)\n$count $count $param"
		 set paramArray($facet,$number) \
                 [lreplace $paramArray($facet,$number) $count $count "$param"]
                 ptkSetParams $facet $number $paramArray($facet,$number)
		}
                return
	}
        incr count
   }
}

# **ed_AddParamDialog 
# Creates the dialog box to add parameters

proc ed_AddParamDialog {facet number} {
    global ed_MaxEntryLength ed_ToplevelNumbers
    set num $ed_ToplevelNumbers($facet,$number)
    global paramArray paramBAKArray
    set top .o$num
    set ask .a$num

    catch {destroy $ask}
    toplevel $ask
    wm title $ask "Add parameter"
    message $ask.m -font -Adobe-times-medium-r-normal--*-180* -relief raised\
	-width 400 -bd 1 -text "Enter the name, type and value below."
    pack append [frame $ask.fname] \
	[label $ask.fname.l -text "Name:" -anchor w] {top expand fillx} \
	[scrollbar $ask.fname.s -relief sunken -orient horiz -command \
		"$ask.fname.e view"] {bottom fillx} \
	[entry $ask.fname.e -width $ed_MaxEntryLength -relief sunken \
		-scroll "$ask.fname.s set"] bottom
    pack append [frame $ask.ftype] \
	[label $ask.ftype.l -text "Type:" -anchor w] {top expand fillx} \
	[scrollbar $ask.ftype.s -relief sunken -orient horiz -command \
		"$ask.ftype.e view"] {bottom fillx} \
	[entry $ask.ftype.e -width $ed_MaxEntryLength -relief sunken \
		-scroll "$ask.ftype.s set"] bottom
    pack append [frame $ask.fval] \
	[label $ask.fval.l -text "Value:" -anchor w] {top expand fillx} \
	[scrollbar $ask.fval.s -relief sunken -orient horiz -command \
		"$ask.fval.e view"] {bottom fillx} \
	[entry $ask.fval.e -width $ed_MaxEntryLength -relief sunken \
		-scroll "$ask.fval.s set"] bottom
    pack append [frame $ask.b] \
	[button $ask.b.dismiss -text Dismiss -command \
		"ed_AddParam $facet $number \
		\[$ask.fname.e get\] \[$ask.ftype.e get\] \[$ask.fval.e get\]
		destroy $ask"] {left expand fill} \
	[button $ask.b.done -text Done -command \
		"$ask.b.dismiss invoke; destroy $top"] {left expand fill} \
	[button $ask.b.cancel -text Cancel -command "destroy $ask"] \
		{left expand fill}
    pack append $ask $ask.m {top fillx} $ask.fname {top fillx} \
	$ask.ftype {top fillx} $ask.fval {top fillx} $ask.b {top fillx}

    grab $ask
    focus $ask.fname.e
}

# **ed_YesNoDialog
# Queries given mesg and returns to caller the user-response (ie., '1'
#  for an affirmative response and '0' otherwise).

proc ed_YesNoDialog {mesg} {
    global result
    set times -Adobe-times-medium-r-normal--*-180*
    set w .yn
    while [winfo exists $w] {
	incr num
	set w .yn{$num}
    }
    toplevel $w
    wm title $w Dialog
    frame $w.f -relief raised -bd 2
    message $w.f.m -font $times -relief raised\
        -width 400 -bd 1 -text $mesg
    frame $w.f.yes -relief sunken -bd 2
    frame $w.f.no -relief sunken -bd 2
    pack append $w.f.yes [button $w.f.yes.b -command "set result 1
	destroy $w" -text Yes -font $times] {padx 10 pady 10}
    pack append $w.f.no [button $w.f.no.b -command "set result 0
	destroy $w" -text No -font $times] {padx 10 pady 10}
    pack append $w.f $w.f.m {top fillx} \
	$w.f.yes {bottom left expand padx 1c pady 1c} \
	$w.f.no {bottom left expand padx 1c pady 1c}
    pack append $w $w.f {expand fill}
    tkwait visibility $w
    focus $w
    grab $w
    tkwait window $w
    return $result
}

# **ed_AddParam
# The procedure calls ptkSetParams to add a parameter.
#  Checks to make sure that parameters with the same name aren't overwritten

proc ed_AddParam {facet number name type value} {
    upvar 1 localNum localNum
    if {[string match $name ""] || [string match $type ""]} {
	ptkImportantMessage .error \
		"You have not entered sufficient information"
	return
    }
    global ed_MaxEntryLength ed_ToplevelNumbers
    set num $ed_ToplevelNumbers($facet,$number)
    global paramArray paramBAKArray
    set top .o$num
    set f $top.f.c.f
    set overWriteParam 0

    set count $ed_ToplevelNumbers($facet,$number,count)
    incr ed_ToplevelNumbers($facet,$number,count)

    set ed_ToplevelNumbers($facet,$number,$count) $name
#    if [winfo exists $f.par.f$count] {
#	if [ed_YesNoDialog "Parameter: \"$name\" exists.  Overwrite?"] {
#		destroy $f.par.f$name
#		set overWriteParam 1
#	} else {
#		return
#	}
#    }

    ed_MkEntryButton $f.par.f$count $name
    
    bind $f.par.f$count.entry <Any-Leave> \
	"ed_UpdateParam $facet $number [list $name] \[%W get\]"
    bind $f.par.f$count.entry <Return> \
	"ed_UpdateParam $facet $number [list $name] \[%W get\]"
    $f.par.f$count.entry insert 0 "$value"

    pack append $f.par $f.par.f$count {top expand fillx pady 1m}
    if {$overWriteParam} {
	ed_UpdateParam $facet $number [list $name] $type $value
    } else {
	lappend paramArray($facet,$number) [list "$name" "$type" "$value"]
	ptkSetParams $facet $number $paramArray($facet,$number)
#            puts "**$paramArray($facet,$number)\n$count"
    }
}


# **ed_ChangeCursor
# Recursively changes the cursor in a window and it's children
# Stores the old cursors in the global variable ed_PrevCursor
# Note that Tcl_Eval recursions cannot exceed a certain depth;
#  consequently, window arguments to ed_ChangeCursor cannot have sub-children
#  of children beyond the same depth.  (In any event, sub-windows beyond five
#  levels are unlikely.)

proc ed_ChangeCursor {w cursor} {
   global ed_PrevCursor
   set ed_PrevCursor($w) [lindex [$w config -cursor] 4]
   $w config -cursor $cursor
   set children [winfo children $w]
   if [string compare $children ""] {
	foreach child $children {
	   ed_ChangeCursor $child $cursor
	}
   }
}

# **ed_RestoreCursor
# Recursively restores the former cursors as stored in the global variable
#  ed_PrevCursor
proc ed_RestoreCursor w {
   global ed_PrevCursor
   $w config -cursor $ed_PrevCursor($w)
   unset ed_PrevCursor($w)
   set children [winfo children $w]
   if [string compare $children ""] {
      foreach c [winfo children $w] {
	ed_RestoreCursor $c
      }
   }
}

# **ed_RemoveParam
# Removes a parameter by rebinding the entries, labels and scrollbars
# Loops to check whether an entry has been destroyed.
# Focus has been eliminated during removal so the evident operation is
#  to click the mouse to select a parameter to remove
# Buttons in the same window as "Click Remove" button are rebound
#  to have cancellation as the top priority.

proc ed_RemoveParam {facet number top button} {
    global ed_EntryDestroyFlag

    set oldB1EntryBinding [bind Entry <1>]
    set oldB1LabelBinding [bind Label <1>]
    set oldB1ScrollbarBinding [bind Scrollbar <1>]
    set oldB1ButtonCmdq [lindex [$button.q config -command] 4]
    set oldB1ButtonCmdok [lindex [$button.ok config -command] 4]
    set oldB1ButtonCmdadd [lindex [$button.add config -command] 4]

    set oldFocus [focus]
    grab $top
    focus none

    $button.q config -command {set ed_EntryDestroyFlag 1}
    $button.ok config -command {set ed_EntryDestroyFlag 1}
    $button.add config -command {set ed_EntryDestroyFlag 1}

    ed_ChangeCursor $top.f.c.f pirate
    bind Entry <1> "ed_Remove $facet $number %W"
    bind Label <1> "ed_Remove $facet $number %W"
    bind Scrollbar <1> "ed_Remove $facet $number %W"
    while {! $ed_EntryDestroyFlag} {
	after 500
	update
    }
    bind Entry <1> $oldB1EntryBinding
    bind Label <1> $oldB1LabelBinding
    bind Scrollbar <1> $oldB1ScrollbarBinding
    $button.q config -command $oldB1ButtonCmdq
    $button.ok config -command $oldB1ButtonCmdok
    $button.add config -command $oldB1ButtonCmdadd

    if [winfo exists $oldFocus] { focus $oldFocus }
    ed_RestoreCursor $top.f.c.f
    set ed_EntryDestroyFlag 0
}

# **ed_Remove
# Removes the relevant parameter
# Signals that removal has occurred by setting ed_EntryDestroyFlag to 1

proc ed_Remove {facet number winName} {
    global ed_EntryDestroyFlag paramArray ed_ToplevelNumbers
    regsub {^(.*\.f[^\.]+)((\.entry)|)$} $winName {\1} window
    destroy $window
    regsub {^.*\.f([^\.]+)$} $window {\1} countd
    set name $ed_ToplevelNumbers($facet,$number,$countd)
    set count 0
    foreach param $paramArray($facet,$number) {
	if {[string match [lindex $param 0] $name]} {
		set paramArray($facet,$number) \
		   [lreplace $paramArray($facet,$number) $count $count]
		ptkSetParams $facet $number $paramArray($facet,$number)
		break
	}
        incr count
    }
    set ed_EntryDestroyFlag 1
}


# **ptkEditParams
# Formerly, ptkShowFacetNum
# Currently, under the above name, opens up an edit-parameters dialog box.
# To make sure that parentheses organized as ``}{''  are treated as ``} {''
#  a substitution is made.

proc ptkEditParams {facet number} {
    global ed_MaxEntryLength ed_ToplevelNumbers
    if {[info exists ed_ToplevelNumbers($facet,$number)] && \
	[winfo exists .o$ed_ToplevelNumbers($facet,$number)]} {
	ptkImportantMessage .error \
	    "Already editing the parameters for this object."
	return
    }
    set ed_ToplevelNumbers($facet,$number,count) 0

    set num $ed_ToplevelNumbers(ed_Num)
    incr ed_ToplevelNumbers(ed_Num)
    set ed_ToplevelNumbers($facet,$number) $num
    global paramArray paramBAKArray
    set top .o$num

#    set paramArray(addBorder) 0

    set ed_GetResult [ptkGetParams $facet $number]

#    regsub -all {(\})(\{)} $ed_GetResult {\1 \2} ed_GetResult
    if {$ed_GetResult == ""} {
	ptkImportantMessage .error "ed_GetParams returns {}"
	return
    }
    if {$ed_GetResult == "NIL"} {
        # This instance cannont have parameters
        return
    }
    set editType [lindex $ed_GetResult 0]
    if {[string compare [lindex $editType 0] Edit]} {
	ptkImportantMessage .error \
	  "For OctInstanceHandle: \"$number\", \"$editType\" unrecognized"
	return
    }

    if {[ptkIsBus $number]} {
	set editType "Bus Width"
    } elseif {[ptkIsDelay $number]} {
	set editType Delay
    } else {
	set editType "Parameters"
#	set editType "[lindex $editType 1] Parameters"
    }

    toplevel $top
    wm title $top "Edit Params: $number"
    wm iconname $top "Edit Params"
    wm minsize $top 0 0

    bind $top <Destroy> "catch \"unset ed_ToplevelNumbers($facet,$number\"
							 destroy %W"

    frame $top.f -relief raised -bd 2
    label $top.header -font -Adobe-times-medium-r-normal--*-180* \
	-relief	raised -text "Edit $editType"
    pack append $top $top.header {top fillx} \
	$top.f {bottom expand fill}

    set c $top.f.c
    set u $top.f.b
    frame $u -bd 2
    canvas $c
    pack append $top.f $u {bottom fillx} $c {bottom expand fill}

    pack append $u \
	   [button $u.ok -text Dismiss -command \
		"catch \"unset paramArray($facet,$number) \ \
		paramArrayBAK($facet,$number\"; destroy $top"] \
		{left expand fillx} \
	   [button $u.q -text Cancel -command \
	        "ed_RestoreParam $facet $number
		catch \"unset paramArray($facet,$number) \
		paramArrayBAK($facet,$number\"; destroy $top"] \
		{left expand fillx}
    if {!([ptkIsBus $number] || [ptkIsDelay $number] || [ptkIsStar $number])} {
	pack append $u \
	   [button $u.add -text "Add parameter" -command \
		"ed_AddParamDialog $facet $number"] {left expand fillx} \
	   [button $u.remove -text "Click Remove" -command \
		"$u.remove config -relief sunken
		ed_RemoveParam $facet $number $top $u
		$u.remove config -relief raised"] \
		{left expand fillx}
    }
    set f $c.f
    frame $f
    frame $f.par -bd 10
    pack append $f \
	$f.par {left expand fill}

    set paramList [lindex $ed_GetResult 1]
    set paramArray($facet,$number) $paramList
    set paramBAKArray($facet,$number) $paramList
    if {[llength $paramList] == 0} {
	ptkImportantMessage .error \
		"Error: Star has no parameters"
	return
    }

    foreach param $paramList {
	set name [lindex $param 0]
	if [string match NIL $name] {
		if [ptkIsStar $number] {
			destroy $top
			ptkImportantMessage .error \
				"Error: Star of galaxy has no parameters"
			return
		} else { 
			set paramArray($facet,$number) {}
			wm title $top "Edit Params: $facet"
			break
		}
	}
#	if [info exist ed_ToplevelNumbers($facet,$number,"n_$name")] {
#		ptkImportantMessage .error \
#		"Warning: Parameter \"$name\" is listed more than once."
#	}
	set count $ed_ToplevelNumbers($facet,$number,count)
	incr ed_ToplevelNumbers($facet,$number,count)

	set ed_ToplevelNumbers($facet,$number,$count) $name
	set value [lindex $param 2]

	ed_MkEntryButton $f.par.f$count $name
	bind $f.par.f$count.entry <Any-Leave> \
		"ed_UpdateParam $facet $number [list $name] \[%W get\]"
	bind $f.par.f$count.entry <Return> \
		"ed_UpdateParam $facet $number [list $name] \[%W get\]"
	$f.par.f$count.entry insert 0 "$value"
	pack append $f.par $f.par.f$count {top fillx expand \
		pady  1m}
    }
#    grab $top
    if [winfo exists $f.par.f0.e] {focus $f.par.f0.e}
    $c create window 0 0 -anchor nw -window $f -tags frameWindow
    set mm [winfo fpixels $c 1m]
#    bind $c <Configure> "ed_ConfigFrame $top"
    bind $f.par <Configure> "ed_ConfigCanvas $top $facet $number"
}

## **ed_ConfigFrame
## Attempts to resize according to user-requested resizing of toplevel window
## Not extremely successful..  Will require some more work.
#
#proc ed_ConfigFrame {top} {
##puts "CONFIG_FRAME"
#    set c $top.f.c
#    set f $c.f
#    set mm [winfo fpixels $c 1m]
#    set width [expr [winfo width $c]-2*$mm]
#    set height [winfo height $f]
#    $c itemconfigure frameWindow -width $width -height $height
#}

# **ed_Dummy
# Because entry widgets refuse to accept empty arguments for xscroll
# commands, this dummy procedure serves as a nop scrollbar widget command

proc ed_Dummy args {}

# **ed_AddScroll
# Adds a scrollbar to an entry widget with window extension ``.e''
# If the scrollbar exists, then it is removed; however, the positioning
#  is different.
# Assumes existence of label with extension ``.l''

proc ed_AddScroll {entry} {
    set common [file root $entry]
    if [winfo exists $common.s] {
	destroy $common.s
	$entry config -scroll ed_Dummy
	pack append $common \
		$common.l {left expand fillx} \
		$common.e right
    } else {
	scrollbar $common.s -relief sunken -orient horiz \
		-command "$entry view"
        $entry config -scroll "$common.s set"
        pack append $common \
		$common.s {bottom fillx} \
		$common.l {top expand fillx} \
		$common.e {right expand fillx}
    }
}

# **ed_configCanvas
# An attempt at resizing the canvas widget to fit the actual size
#  of the overall frame containing all the other windows
# Will likely require a cooperative effort with ed_ConfigFrame

proc ed_ConfigCanvas {top facet number} {

    set c $top.f.c
    set f $c.f

#puts "CONFIG_CANVAS"
    set mm [winfo fpixels $c 1m]
#    set maxWidth [winfo fpixels $c 5.5i]
#    set maxHeight [winfo fpixels $c 7.5i]
    set maxWidth [winfo fpixels $c 7.5i]
    set maxHeight [winfo fpixels $c 8.5i]
    set scrollWidth [expr [winfo width $f]+2*$mm]
    set scrollHeight [expr [winfo height $f]+2*$mm]
    set canvWidth $scrollWidth
    set canvHeight $scrollHeight
    set existh [winfo exist $top.f.hscroll]
    set existv [winfo exist $top.f.vscroll]

    if {$scrollWidth > $maxWidth} {
	if {! $existh} {
		scrollbar $top.f.hscroll -orient horiz -relief sunken \
			-command "$c xview"
		$c config -xscroll "$top.f.hscroll set"
		pack before $c $top.f.hscroll {bottom fillx}
	}
	set canvWidth $maxWidth
    } else {
		set scrollWidth 0
    }
    if {$scrollHeight > $maxHeight} {
	if {! $existv} {
		scrollbar $top.f.vscroll -relief sunken -command "$c yview"
		$c config -yscroll "$top.f.vscroll set"
		pack before $c $top.f.vscroll {right filly}
	}
	set canvHeight $maxHeight
    } else {
		set scrollHeight 0
    }

    $c configure -scrollregion "0 0 $scrollWidth $scrollHeight"
    $c configure -width $canvWidth -height $canvHeight

}
