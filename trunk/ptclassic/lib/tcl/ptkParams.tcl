# Ptolemy edit-parameters dialog box--tcl routines
# Author: Wei-Jen Huang
# $Id$
#
# Copyright (c) 1990-1993 The Regents of the University of California.
# All rights reserved.
#
# With numerous contributions from Prof. Edward Lee and Alan Kamas
#
# For storage, there exist two global array--one which stores lists of
# current parameter-type-value triplets and the other which (upon
# invocation of an edit-parameters command) stores the original triplet
# values as back-ups.

# Entry bindings specific to the edit-parameters box include:
# 		Focus on next entry
# 		Focus on previous entry
# Tab		Focus on next entry
# Return	Focus on next entry

# Width of entry in average-sized chars of font
set ed_MaxEntryLength 60

# Global variable containing information whether the entry has been destroyed
#  Should be unique
set ed_EntryDestroyFlag 0

# Global variable containing the number for a toplevel window
set ed_ToplevelNumbers(ed_Num) 0

# **ed_RestoreParam
# Discards all changes

proc ed_RestoreParam {facet number args} {
    global paramBAKArray
    if {$args != "Target"} {
	ptkSetParams $facet $number $paramBAKArray($facet,$number)
    } else {
	ptkSetTargetParams $facet $number $paramBAKArray($facet,$number)
    }
}

# **proc ed_SetEntryButtons
# Procedure to make buttons function in the same manner as the scrollbar
#  widget.  Configures the buttons according to the position and view in the
#  entry widget.  Takes a frame containing the entry and buttons as the first
#  argument.

proc ed_SetEntryButtons {frame numStor numPossVis leftIdx rightIdx} {
	$frame.left config -fg black
    if {$leftIdx > 0} {
	$frame.left config -state normal -fg black
	bind $frame.left <Button-1> "tk_butDown %W
		$frame.entry view \[expr \[$frame.entry index @0\]-1\]
		after 200 ed_ShiftButtonViewLeft %W $frame.entry"
    } else {
	set bgColor [lindex [$frame.left config -bg] 4]
	$frame.left config -state disabled -fg $bgColor \
		-disabledforeground $bgColor
	bind $frame.left <Button-1> "ed_Dummy"
    }
    if {$rightIdx < [expr $numStor-1]} {
	$frame.right config -state normal -fg black
	bind $frame.right <Button-1> "tk_butDown %W
		$frame.entry view \[expr \[$frame.entry index @0\]+1\]
		after 200 ed_ShiftButtonViewRight %W $frame.entry"
    } else {
	set bgColor [lindex [$frame.right config -bg] 4]
	$frame.right config -state disabled -fg $bgColor \
		-disabledforeground $bgColor
	bind $frame.right <Button-1> "ed_Dummy"
    }
}

proc ed_ShiftButtonViewLeft {button entry} {
    if {[lindex [$button config -relief] 4] == "sunken"} {
	$entry view [expr [$entry index @0]-1]
	after 50 "ed_ShiftButtonViewLeft $button $entry"
    }
}

proc ed_ShiftButtonViewRight {button entry} {
    if {[lindex [$button config -relief] 4] == "sunken"} {
	$entry view [expr [$entry index @0]+1]
	after 50 "ed_ShiftButtonViewRight $button $entry"
    }
}

proc ed_MkEntryButton {frame label} {
	global ptolemy ed_MaxEntryLength
	pack append [frame $frame -bd 2] \
	   [label $frame.label -text "$label:  " -anchor w] left \
		[button $frame.right -bitmap @$ptolemy/tcl/lib/right.xbm \
			-command ed_Dummy -relief flat] right \
		[button $frame.left -bitmap @$ptolemy/tcl/lib/left.xbm \
			-command ed_Dummy -relief flat] right
    $frame.right config -fg [lindex [$frame.right config -bg] 4]
    $frame.left config -fg [lindex [$frame.left config -bg] 4]
    bind $frame.left <Button-1> "tk_butDown %W
      $frame.entry view \[expr \[$frame.entry index @0\]-1\]
      after 200 ed_ShiftButtonViewLeft %W $frame.entry"
    bind $frame.right <Button-1> "tk_butDown %W
      $frame.entry view \[expr \[$frame.entry index @0\]+1\]
      after 200 ed_ShiftButtonViewRight %W $frame.entry"
    bind $frame.left <ButtonRelease-1> "catch {tk_butUp %W}"
    bind $frame.right <ButtonRelease-1> "catch {tk_butUp %W}"
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

proc listEq {list1 list2} {
    set max [llength $list1]
    if {[llength $list2] != $max} {
	return 0
    }
    for {set i 0} {$i < $max} {incr i} {
	set subList1 [lindex $list1 $i]
	set subList2 [lindex $list2 $i]
	set length1 [llength $subList1]
	set length2 [llength $subList2]
	if {$length1 == 1 && $length2 == 1} {
		if {$subList1 != $subList2} {
			return 0
		}
	} elseif {!($length1 == 0 && $length2 == 0)} {
		if {$length1 != $length2} {
			return 0
		} else {
			if {![listEq $subList1 $subList2]} {
				return 0
			}
		}
	}
    }
    return 1
}

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
        if {[listEq [lindex $param 0] $name]} {
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
		 set paramArray($facet,$number) \
                 [lreplace $paramArray($facet,$number) $count $count "$param"]
		 if {$args != "Target"} {
                   ptkSetParams $fact $number $paramArray($facet,$number)
		 } else {
		   ptkSetTargetParams $fact $number \
			$paramArray($facet,$number)
		 }
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
    ed_MkEntryButton $ask.fname name
    ed_MkEntryButton $ask.ftype type
    ed_MkEntryButton $ask.fvalue value

    bind $ask.fname.entry <Tab> "focus $ask.ftype.entry"
    bind $ask.fname.entry <Control-n> "focus $ask.ftype.entry"
    bind $ask.fname.entry <Control-p> "focus $ask.fvalue.entry"

    bind $ask.ftype.entry <Tab> "focus $ask.fvalue.entry"
    bind $ask.ftype.entry <Control-n> "focus $ask.fvalue.entry"
    bind $ask.ftype.entry <Control-p> "focus $ask.fname.entry"

    bind $ask.fvalue.entry <Tab> "focus $ask.fname.entry"
    bind $ask.fvalue.entry <Control-n> "focus $ask.fname.entry"
    bind $ask.fvalue.entry <Control-p> "focus $ask.ftype.entry"

    pack append [frame $ask.b] \
	[frame $ask.b.okfr -relief sunken -bd 2] {left expand fill} \
	[button $ask.b.cancel -text "Cancel" \
		-command "destroy $ask"] {left expand fill}

    pack append $ask.b.okfr \
	[button $ask.b.okfr.f -text "  OK  " -command \
		"ed_AddParam $facet $number \
		\[$ask.fname.entry get\] \
		\[$ask.ftype.entry get\] \
		\[$ask.fvalue.entry get\]
		destroy $ask"] {expand fill}

    pack append $ask $ask.m {top fillx} $ask.fname {top fillx} \
	$ask.ftype {top fillx} $ask.fvalue {top fillx} $ask.b {top fillx}
    ptkRecursiveBind $ask <Return> \
		"ed_AddParam $facet $number \
		\[$ask.fname.entry get\] \
		\[$ask.ftype.entry get\] \
		\[$ask.fvalue.entry get\]
		destroy $ask"
    ptkRecursiveBind $ask <M-Delete> "destroy $ask"

    grab $ask
    focus $ask.fname.entry
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
    ptkRecursiveBind $f.par.f$count <Return> "ed_Apply $facet $number
                                $top.b.close invoke"
# For the bindings to work
#    ptkRecursiveBind $f.par.f$count <Any-Enter> {focus %W}
    
#    bind $f.par.f$count.entry <Any-Leave> \
#	"ed_UpdateParam $facet $number [list $name] \[%W get\]"
    bind $f.par.f$count.entry <Tab> "ed_NextEntry $count \
		$f.par $facet $number"
    bind $f.par.f$count.entry <Control-n> "ed_NextEntry $count \
		$f.par $facet $number"
    bind $f.par.f$count.entry <Control-p> "ed_PrevEntry $count \
		$f.par $facet $number"
    $f.par.f$count.entry insert 0 "$value"

    pack append $f.par $f.par.f$count {top expand fillx pady 1m}
    if {$overWriteParam} {
	ed_UpdateParam $facet $number [list $name] $type $value
    } else {
	lappend paramArray($facet,$number) [list "$name" "$type" "$value"]
	ptkSetParams $facet $number $paramArray($facet,$number)
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
# Buttons in the same window as "Remove Parameter" button are rebound
#  to have cancellation as the top priority.

proc ed_RemoveParam {facet number top button} {
    global ed_EntryDestroyFlag

    set oldB1EntryBinding [bind Entry <1>]
    set oldB1LabelBinding [bind Label <1>]
    set oldB1ScrollbarBinding [bind Scrollbar <1>]
    set oldB1ButtonCmdq [lindex [$button.q config -command] 4]
    set oldB1ButtonCmdok [lindex [$button.okfr.ok config -command] 4]
    set oldB1ButtonCmdadd [lindex [$button.add config -command] 4]

    set oldFocus [focus]
    grab $top
    focus none

    $button.q config -command {set ed_EntryDestroyFlag 1}
    $button.okfr.ok config -command {set ed_EntryDestroyFlag 1}
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
    $button.okfr.ok config -command $oldB1ButtonCmdok
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

proc ptkEditParams {facet number args} {

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


    if {$args == ""} {
	# number is target instance
	set ed_GetResult [ptkGetParams $facet $number]
    } else {
	# number is target name
	set ed_GetResult [ptkGetTargetParams $facet $number]
    }

    if {$ed_GetResult == "" || $ed_GetResult == "NIL"} {
        # This facet/instance cannot have parameters
        return
    }

    if {$args == "Target"} {
	set ed_GetResult "{Edit Target} [list $ed_GetResult]"
    }

    set editType [lindex $ed_GetResult 0]
    if {[string compare [lindex $editType 0] Edit]} {
	  ptkImportantMessage .error \
	   "For OctInstanceHandle: \"$number\", \"$editType\" unrecognized"
	return
    }

    if {$args == "Target"} {
	set editType "Target \"$number\""
    } elseif {[ptkIsBus $number]} {
	set editType "Bus Width"
    } elseif {[ptkIsDelay $number]} {
	  set editType Delay
    } else {
	  set editType "Parameters"
#	set editType "[lindex $editType 1] Parameters"
    }

    toplevel $top
#    wm title $top "Edit Params: $number"
    wm title $top "Edit Params"
    wm iconname $top "Edit Params"

    bind $top <Destroy> "catch \"unset ed_ToplevelNumbers($facet,$number\"
							 destroy %W"

    frame $top.f -relief raised -bd 2
    label $top.header -font -Adobe-times-medium-r-normal--*-180* \
	-relief	raised -text "Edit $editType"

    pack append $top $top.header {top fillx} \
	$top.f {top expand fill}

    set c $top.f.c
    canvas $c

    set u $top.b
    frame $u -bd 5
    pack append $top.f $u {bottom fillx} \
		$c {bottom expand fill}

    if {$args == "Target"} {
    pack append $u \
	   [frame $u.okfr -relief sunken -bd 2] \
		{left expand fillx} \
	   [button $u.apply -text "     Apply     " -command \
		"ed_Apply $facet $number Target"] \
		{left expand fillx} \
	   [button $u.close -text "     Close      " \
		-command "catch \"unset paramArray($facet,$number) \
			      paramArrayBAK($facet,$number\"; destroy $top"] \
		{left expand fillx} \
	   [button $u.q -text "     Cancel      " -command \
	        "ed_RestoreParam $facet $number Target; $u.close invoke"] \
		{left expand fillx}
    pack append $u.okfr \
	[button $u.okfr.ok -text "       OK       " -relief raised \
		-command "ed_Apply $facet $number Target
				$u.close invoke"] {expand fill}
    } else {
    pack append $u \
	   [frame $u.okfr -relief sunken -bd 2] \
		{left expand fillx} \
	   [button $u.apply -text "     Apply     " -command \
		"ed_Apply $facet $number"] \
		{left expand fillx} \
	   [button $u.close -text "     Close      " \
		-command "catch \"unset paramArray($facet,$number) \
			      paramArrayBAK($facet,$number\"; destroy $top"] \
		{left expand fillx} \
	   [button $u.q -text "     Cancel      " -command \
	        "ed_RestoreParam $facet $number; $u.close invoke"] \
		{left expand fillx}
    pack append $u.okfr \
	[button $u.okfr.ok -text "       OK       " -relief raised \
		-command "ed_Apply $facet $number
				$u.close invoke"] {expand fill}
    }

# Joe Buck's fix <jbuck@Synopsys.com> 11/93

    if {$number == "NIL" && $args != "Target"} {
	pack append $u \
	   [button $u.add -text " Add parameter " -command \
		"ed_AddParamDialog $facet $number"] \
		{left expand fillx} \
	   [button $u.remove -text "Remove parameter" -command \
		"$u.remove config -relief sunken
		ed_RemoveParam $facet $number $top $u
		$u.remove config -relief raised"] {left padx 2m}
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
#			wm title $top "Edit Params: $facet"
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
#	bind $f.par.f$count.entry <Any-Leave> \
#		"ed_UpdateParam $facet $number [list $name] \[%W get\]"
	bind $f.par.f$count.entry <Tab> "ed_NextEntry $count \
		$f.par $facet $number"
	bind $f.par.f$count.entry <Control-n> "ed_NextEntry $count \
		$f.par $facet $number"
	bind $f.par.f$count.entry <Control-p> "ed_PrevEntry $count \
		$f.par $facet $number"
	$f.par.f$count.entry insert 0 "$value"
	pack append $f.par $f.par.f$count {top fillx expand \
		pady  1m}
    }
    if [winfo exists $f.par.f0.e] { focus $f.par.f0.e }
    $c create window 0 0 -anchor nw -window $f -tags frameWindow
    set mm [winfo fpixels $c 1m]
#    bind $c <Configure> "ed_ConfigFrame $top"

#    ptkRecursiveBind $top <Return> "ed_UpdateOnMReturn $facet $number
#		catch \"unset paramArray($facet,$number) \
#			      paramArrayBAK($facet,$number\"
#					    destroy $top"

    if {$args != "Target"} {
        ptkRecursiveBind $top <M-Delete> \
	        "ed_RestoreParam $facet $number
		catch \"unset paramArray($facet,$number) \
		paramArrayBAK($facet,$number\"; destroy $top"
        ptkRecursiveBind $top <Return> "ed_Apply $facet $number
                                $u.close invoke"

        if {!([ptkIsBus $number] || [ptkIsDelay $number] || \
	      [ptkIsStar $number])} {
	    ptkRecursiveBind $top <M-a> "ed_AddParamDialog $facet $number"
	    ptkRecursiveBind $top <M-r> \
		"$u.remove config -relief sunken
		ed_RemoveParam $facet $number $top $u
		$u.remove config -relief raised"
	}

    } else {
        ptkRecursiveBind $top <M-Delete> \
	        "ed_RestoreParam $facet $number Target
		catch \"unset paramArray($facet,$number) \
		paramArrayBAK($facet,$number\"; destroy $top"
        ptkRecursiveBind $top <Return> "ed_Apply $facet $number Target
                                $u.close invoke"
    }

# For the bindings to work outside the entry widgets
#    ptkRecursiveBind $top <Any-Enter> {focus %W}


    wm withdraw $top
    update
    ed_ConfigCanvas $top $facet $number
    $top.f.c config -bg [lindex [$top.f.c.f config -bg] 4]
    wm deiconify $top
    update
    bind $f.par <Configure> "ed_ConfigCanvas $top $facet $number"

}

proc ed_UpdateOnMReturn {facet number} {
    global ed_ToplevelNumbers
    set top .o$ed_ToplevelNumbers($facet,$number)
    set focus [focus]
    if {$focus != "none" && [winfo toplevel $focus] == $top && \
	[winfo class $focus] == "Entry"} {
	if {[regsub {.*.f([0-9]+).entry} $focus {\1} count] != 1} {
	   ptkImportantMessage .error "Inappropriate entry: $focus"
	   return
	}
	ed_UpdateParam $facet $number \
		$ed_ToplevelNumbers($facet,$number,$count) \
		[$focus get]
    }
}

proc ed_Apply {facet number args} {
    global ed_ToplevelNumbers paramArray
    set changeFlag 0
    set top .o$ed_ToplevelNumbers($facet,$number)
    set w $top.f.c.f.par

    set newParamArray {}
    for {set curr 0} \
	{ $curr < $ed_ToplevelNumbers($facet,$number,count)} \
	{ incr curr } {
	    if [winfo exists $w.f$curr.entry] {
		set name $ed_ToplevelNumbers($facet,$number,$curr)
		set contents [list [$w.f$curr.entry get]]
		if {[llength $contents] == 2} {
		    set value [lindex $contents 1]
		    set type [list [lindex $contents 0]]
		} else {
		    set value [lindex $contents 0]
		}
		set count 0
		foreach param $paramArray($facet,$number) {
		   if {[listEq [lindex $param 0] $name]} {
			if  {[lindex $param 2] != $value} {
				set okay 1
			} else { set okay 0 }
			if {[llength $contents] == 2} {
				if {[lindex $param 1] != $type} {
					set okay 1
				}
			} else { set type [lindex $param 1] }
			if {$okay} {
				set param [lreplace $param 1 2 $type $value]
			}
			break
		   }
		   incr count
		}
		if $okay {set changeFlag 1}
	        lappend newParamArray $param
	    }
	}

   if $changeFlag {
        if {$args != "Target"} {
		ptkSetParams $facet $number $newParamArray
	} else {
		ptkSetTargetParams $facet $number $newParamArray
	}
	set paramArray($facet,$number) $newParamArray
   }
}

# **ed_NextEntry
# Procedure for moving from one entry to the next in the edit parameters box

proc ed_NextEntry {current w facet number} {
    global ed_ToplevelNumbers
    set foundNext 0
#    ed_UpdateOnMReturn $facet $number
    for {set curr [expr $current+1]} \
	{ $curr < $ed_ToplevelNumbers($facet,$number,count)} \
	{ incr curr } {
	    if [winfo exists $w.f$curr.entry] {
		focus $w.f$curr.entry
		set foundNext 1
		break
	    }
	}
        if {! $foundNext} {
		for {set curr 0} {$curr < $current} \
			{incr curr} {
				if [winfo exists $w.f$curr.entry] {
					focus $w.f$curr.entry
					break
				}
			}
	}
}

# **ed_PrevEntry
# Procedure for moving from one entry to the next in the edit parameters box

proc ed_PrevEntry {current w facet number} {
    global ed_ToplevelNumbers
    set foundNext 0
#    ed_UpdateOnMReturn $facet $number
    for {set curr [expr $current-1]} \
	{ $curr >= 0 } \
	{ incr curr -1 } {
	    if [winfo exists $w.f$curr.entry] {
		focus $w.f$curr.entry
		set foundNext 1
		break
	    }
	}
        if {! $foundNext} {
		for {set curr $ed_ToplevelNumbers($facet,$number,count)} \
			{$curr > $current} \
			{incr curr -1} {
				if [winfo exists $w.f$curr.entry] {
					focus $w.f$curr.entry
					break
				}
			}
	}
}

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

    set mm [winfo fpixels $c 1m]
#    set maxWidth [winfo fpixels $c 5.5i]
#    set maxHeight [winfo fpixels $c 7.5i]
    set maxWidth [winfo fpixels $c 7.5i]
    set maxHeight [winfo fpixels $c 8.5i]
    set scrollWidth [expr [winfo reqwidth $f]+2*$mm]
    set scrollHeight [expr [winfo reqheight $f]+2*$mm]
    set canvWidth $scrollWidth
    set canvHeight $scrollHeight
    set existh [winfo exist $top.f.hscroll]
    set existv [winfo exist $top.f.vscroll]

    if {$scrollWidth > $maxWidth} {
#	if {! $existh} {
#		scrollbar $top.f.hscroll -orient horiz -relief sunken \
#			-command "$c xview"
#		$c config -xscroll "$top.f.hscroll set"
#		pack before $c $top.f.hscroll {bottom fillx}
#	}
#	set canvWidth $maxWidth
#    } else {
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

    set buttonWidth [winfo reqwidth $top.b]
    set intWidth \
	[expr ($buttonWidth > $canvWidth) ? $buttonWidth : $canvWidth]
    if [winfo exist $top.f.vscroll] {
	set intWidth [expr $intWidth+[winfo reqWidth $top.f.vscroll]]
    }
    if {[string first . $intWidth] != -1} {
	    scan $intWidth "%d." intWidth
	    incr intWidth
    } else { set intWidth $intWidth }
   
    set topHeight [expr $canvHeight+[winfo reqheight $top.header]+[
	winfo reqheight $top.b]]
    if {[string first . $topHeight] != -1} {
	    scan $topHeight "%d." intHeight
	    incr intHeight
    } else { set intHeight $topHeight }
    wm minsize $top $intWidth 0
    wm maxsize $top $intWidth [winfo screenheight .]
    wm geometry $top ${intWidth}x${intHeight}

}


#############################################################################
#
# Radiobutton selections for Target Params and Edit-Domain
# Authors: Wei-Jen Huang and Alan Kamas

# General-purpose option selector
# One caveat, options cannot contain certain characters, notably '.'
# Executes a command string suitable for formatting

# Right now, ptkChooseOne generates a unique toplevel window name
#  using the global unique variable.
#  FIXME: Pass ptkChooseOne a window name as the first argument
#	and in the cases of Edit-Target Params and Edit-Domain
#	the window name can be generated from the facet-id

proc ptkChooseOne {
    optionList
    command 
    {direction "Choose one:"} } {

    global unique

#    if [winfo exist $w] {
#	ptkImportantMessage .error \
#		"Already editing target parameters for this facet"
#	return
#    }

    set w .ptkChooseOne$unique
    incr unique

    toplevel $w

    set b $w.buttonFrame

    label $w.label -text $direction -anchor w
    frame $w.optFrame -bd 3 -relief sunken
    frame $b -bd 3

    foreach opt $optionList {
       pack [radiobutton $w.optFrame.opt$opt -text $opt -var selVar$w \
		-value $opt -anchor w]\
	-side top -expand 1 -fill x
    }

   $w.optFrame.opt[lindex $optionList 0] invoke

   pack [frame $b.f -bd 2 -relief sunken] -side left -expand 1
   pack [button $b.cancel -text Cancel \
	-command "destroy $w; uplevel #0 \"unset selVar$w\""] \
        -side left -padx 3m -pady 3m -ipadx 1m -ipady 1m -expand 1

   pack [button $b.f.ok -text OK -command \
	"ptkFormatCmd \"$command\" selVar${w}; destroy $w"] \
	-padx 1m -pady 1m -ipadx 1m -ipady 1m

   pack $w.label -side top -fill x -expand 1
   pack $w.optFrame -side top -expand 1 -fill x
   pack $b -side top -expand 1 -fill x

}

# Formats 'cmd'

proc ptkFormatCmd {cmd radioVarName} {
   upvar $radioVarName radioVar
   eval [format $cmd $radioVar]
}

#
#############################################################################
