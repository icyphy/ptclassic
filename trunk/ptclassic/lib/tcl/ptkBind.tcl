# Version: $Id$

#---------------------------------------------------------------------------
# Copyright (c) 1993 The Regents of the University of California.
# All rights reserved.
#
# Permission is hereby granted, without written agreement and without
# license or royalty fees, to use, copy, modify, and distribute this
# software and its documentation for any purpose, provided that the above
# copyright notice and the following two paragraphs appear in all copies
# of this software.
#
# IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY
# FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
# ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
# THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
#
# THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
# INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
# PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
# CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
# ENHANCEMENTS, OR MODIFICATIONS.
#---------------------------------------------------------------------------
#
# Bindings for widget classes in the tcl/tk Ptolemy interface
#
# Author: Wei-Jen Huang
#
#---------------------------------------------------------------------------
#
# Entry bindings augmenting and overwriting start-up bindings defined
#  in tk.tcl: , , , ,  + various mouse-related bindings
# All entries share the global variable ptkKillBuffer for kill storage space
#
#---------------------------------------------------------------------------
# Emacs and shell style bindings
#---------------------------------------------------------------------------
#  For motion:
#---------------------------------------------------------------------------
#  C-F		|	Forward one character
#  C-B		|	Backward one character
#  C-A		|	Goto beginning of line
#  C-E		|	Goto EOL
#  M-F		|	Forward one word
#  M-B		|	Backward one word
#---------------------------------------------------------------------------
#  For erasing:
#---------------------------------------------------------------------------
#  C-U		|	Kill line
#  C-W		|	(Backward) Kill word
#  C-K		|	Kill to EOL
#  C-D		|	Delete insertion point character
#  C-Y		|	Yank from Kill Buffer
#  C-H		|	Backspace
#  C-?		|	Backspace
#  M-D		|	(Forward) Kill word
#  <space>	|	Kill current selection/insert space character
#		|	 depending on context
#---------------------------------------------------------------------------


    bind Entry <Control-f> {
	%W icursor [expr [%W index insert]+1]; tk_entrySeeCaret %W
    }
    bind Entry <Control-b> {
	%W icursor [expr [%W index insert]-1]; tk_entrySeeCaret %W
    }
    bind Entry <Control-a> {
	%W icursor 0; tk_entrySeeCaret %W
    }
    bind Entry <Control-e> {
	%W icursor end; tk_entrySeeCaret %W
    }
    bind Entry <Control-u> {
	set ptkKillBuffer [%W get]
	%W delete 0 end
    }
    bind Entry <Control-k> {
	set ptkKillBuffer [
	 string range [%W get] [%W index insert] [%W index end]
	]
	%W delete insert end; tk_entrySeeCaret %W
    }
    bind Entry <Control-d> {
	%W delete insert; tk_entrySeeCaret %W
    }
    bind Entry <Any-Enter> {
	focus %W
    }
    bind Entry <Control-y> {
	if [info exists ptkKillBuffer] {
		%W insert insert $ptkKillBuffer
	}
    }

    # This line copied and modified from Tk3.6 (file "tk.tcl")
    bind Entry <Control-w> {entry_BackWordAndSave %W; tk_entrySeeCaret %W}
    # FIXME: consecutive Control-w's should cause kill buffer to grow
    
    bind Entry <Key-Escape><Key-d> {
	entry_ForwardWordAndSave %W; tk_entrySeeCaret %W}

    bind Entry <Key-Escape> {puts -nonewline ""}
    bind Entry <Key-Escape><Key-b> {
	set curs [expr [%W index insert]-1]
	set x [entry_BackWordIdx %W $curs]
	%W icursor $x
    }

    bind Entry <Key-Escape><Key-f> {
	set curs [expr [%W index insert]+1]
	set x [entry_ForwardWordIdx %W $curs]
	%W icursor $x
    }

    #bind third mouse button to scanning
    bind Entry <3> {%W scan mark %x}
    bind Entry <B3-Motion> {%W scan dragto %x}

    #bind second mouse button to selection insertion
    bind Entry <2> {%W insert insert [selection get]; tk_entrySeeCaret %W}
    bind Entry <B2-Motion> ""

# (reenabled and fixed):

    bind Entry <space> {
	if {[selection own] == [focus]} {
	   if [info exists errorInfo] {
		set temp $errorInfo
		unset errorInfo
		set exists 1
	   } else {set exists 0}
	   catch {selection get}
	   if {! [info exists errorInfo]} {
		set ptkKillBuffer [
		  string range [%W get] [%W index sel.first] \
			[%W index sel.last]
		  ]
		%W delete sel.first sel.last
		selection clear %W
	   } else {
		%W insert insert %A
		tk_entrySeeCaret %W
	   }
	   if {$exists} { set errorInfo $temp }
	} else {
		%W insert insert %A
		tk_entrySeeCaret %W
	}
    }

proc ptkRecursiveBind {widget keySeq action} {
    bind $widget $keySeq \
	"[bind [winfo class $widget] $keySeq]; $action"
    foreach child [winfo children $widget] {
	ptkRecursiveBind $child $keySeq $action
    }
}

# This procedure comes directly from tk_entryBackword
#   (Tk3.6, file "entry.tcl")
# It has been modified to save the deletion into ptkKillBuffer
# FIXME: Allow the buffer to expand when this binding is invoked
#  successively

proc entry_BackWordIdx {w curs} {
    set string [$w get]
    if {$curs < 0} {return 0}
    for {set x $curs} {$x > 0} {incr x -1} {
        if {([string first [string index $string $x] " \t"] < 0)
                && ([string first [string index $string [expr $x-1]] " \t"]
                >= 0)} {
            break
	}
    }
    return $x
}
	
proc entry_ForwardWordIdx {w curs} {
    set string [$w get]
    set length [string length $string]
    if {$curs >= $length} {return $length}
    for {set x $curs} {$x < $length} {incr x} {
        if {([string first [string index $string $x] " \t"] >= 0)
                && ([string first [string index $string [expr $x-1]] " \t"]
                < 0)} {
            break
	}
    }
    return $x
}

proc entry_BackWordAndSave w {
    global ptkKillBuffer
    set curs [expr [$w index insert]-1]
    set x [entry_BackWordIdx $w $curs]
    set ptkKillBuffer [string range [$w get] $x $curs]
    $w delete $x $curs
}
proc entry_ForwardWordAndSave w {
    global ptkKillBuffer
    set curs [$w index insert]
    set x [entry_ForwardWordIdx $w [expr $curs+1]]
    set ptkKillBuffer [string range [$w get] $curs $x]
    $w delete $curs $x
}
