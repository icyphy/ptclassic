# Bindings for widget classes in the tcl/tk Ptolemy interface
# Author: Wei-Jen Huang
# Version: $Id$

# Entry bindings augmenting and overwriting start-up bindings defined
#  in tk.tcl: , , , ,  + various mouse-related bindings
# All entries share the global variable ptkKillBuffer for kill storage space

# Emacs/shell type bindings include:

# For motion:
#   Forward one character
#   Backward one character
#   Goto beginning of line
#   Goto EOL

# For deletions:
#   Kill line
#   Kill word				Not stored in ptkKillBuffer variable
#   Kill to EOL
#   Delete insertion point character
#   Undo kill
#   Backspace
#   Backspace

# Also:
#  <space>	Kill current selection/insert space character depending on
#		 context

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

# Appropriate?  Might need some refinement

    #bind third mouse button to scanning
    bind Entry <3> {%W scan mark %x}
    bind Entry <B3-Motion> {%W scan dragto %x}

    #bind second mouse button to selection insertion
    bind Entry <2> {%W insert insert [selection get]; tk_entrySeeCaret %W}
    bind Entry <B2-Motion> ""

    bind Entry <space> {
	if {[selection own] != ""} {
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
    }
