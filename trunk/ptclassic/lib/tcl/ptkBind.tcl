# Version: $Id$

#---------------------------------------------------------------------------
# Copyright (c) 1990-%Q% The Regents of the University of California.
# All rights reserved.
# 
# Permission is hereby granted, without written agreement and without
# license or royalty fees, to use, copy, modify, and distribute this
# software and its documentation for any purpose, provided that the
# above copyright notice and the following two paragraphs appear in all
# copies of this software.
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
# 
#                                               PT_COPYRIGHT_VERSION_2
#                                               COPYRIGHTENDKEY
#---------------------------------------------------------------------------
#
# Bindings for widget classes in the tcl/tk Ptolemy interface
#
# Authors: Wei-Jen Huang
#          Text Bindings by Alan Kamas
#	   Clean up and port to tk4.0 by Christopher Hylands
#---------------------------------------------------------------------------
#
# Entry bindings augmenting and overwriting start-up bindings defined
#  in tk.tcl: , , , ,  + various mouse-related bindings
# All entries share the global variable ptkKillBuffer for kill storage space
#
# Text bindings for the Text widget as well.  All Text bindings mirror
# the functionality of the Entry bindings.
#
# See entry.tcl and text.tcl for the complete bindings.  These two files
# are found in the tk library, possibly at $PTOLEMY/tcltk/tk/lib/tk
#---------------------------------------------------------------------------
# Emacs and shell style bindings
#---------------------------------------------------------------------------
#  For motion:
#---------------------------------------------------------------------------
#  M-B		|	Backward one word
#  M-F		|	Forward one word


#---------------------------------------------------------------------------
#  For erasing:
#---------------------------------------------------------------------------
#  C-U		|	Kill line
#  C-K		|	Kill to EOL
#  C-W		|	(Backward) Kill word
#  C-Y		|	Yank from Kill Buffer
#  C-H		|	Backspace
#  M-D		|	(Forward) Kill word
#  <space>	|	Kill current selection/insert space character
#		|	 depending on context
#---------------------------------------------------------------------------

# Focus
    bind Entry <Any-Enter> {
        #selection own %W
        focus %W
    }
    bind Text <Any-Enter> {
        selection own %W
	focus %W
    }

#  C-U          |       Kill line
    bind Entry <Control-u> {
      clipboard clear -displayof %W
      clipboard append -displayof %W [%W get]
      %W delete 0 end
    }
    bind Text <Control-u> {
      clipboard clear -displayof %W
      clipboard append -displayof %W \
	[%W get {insert linestart} {insert lineend}]
      if [%W compare {insert linestart} == {insert lineend}] {
	%W delete {insert linestart}
      } else {
	%W delete {insert linestart} {insert lineend}
      }
    }

#  C-K          |       Kill to EOL
    bind Entry <Control-k> {
	clipboard clear -displayof %W
	clipboard append -displayof %W [string range [%W get] \
					[%W index insert] [%W index end]]
	%W delete insert end
    }
    bind Text <Control-k> {
      clipboard clear -displayof %W
      clipboard append -displayof %W [%W get insert {insert lineend}]
      %W delete insert {insert lineend}
      puts "[%W get insert {insert lineend}]"
    }

#  C-W          |       (Backward) Kill word
    # FIXME: consecutive Control-w's should cause kill buffer to grow
    # Note that we override C-W in tk4.0 entry.tcl (why?)
    bind Entry <Control-w> {
      clipboard clear -displayof %W
      set insert [%W index insert]
      set start [string wordstart [%W get] $insert]
      clipboard append -displayof %W [string range [%W get] $start $insert]
      if {$insert == $start} {
	%W delete $start
      } {
	%W delete $start $insert
      }
    }

    # Note that we override C-W in tk4.0 text.tcl (why?)
    bind Text <Control-w> {
      clipboard clear -displayof %W
      clipboard append -displayof %W [%W get {insert -1c wordstart} insert]
      if [%W compare insert == {insert -1c wordstart} ] {
	%W delete insert
      } {
	%W delete {insert -1c wordstart} insert
      }
    }

      
#  M-D          |       (Forward) Kill word
    bind Entry <Key-Escape><Key-d> {
      clipboard clear -displayof %W
      catch {
	clipboard append -displayof %W [string range [%W get] \
					[%W index insert] \
					  [string wordend [%W get] \
					   [%W index insert]]]
	%W delete [%W index insert] [string wordend [%W get] [%W index insert]]
      }
    }
    bind Text <Key-Escape><Key-d> {
	#text_ForwardWordAndSave %W
	clipboard clear -displayof %W
	clipboard append -displayof %W [%W get insert {insert wordend}]
	%W delete insert {insert wordend}
    }

    bind Entry <Key-Escape> {puts -nonewline ""}
    bind Text <Key-Escape> {puts -nonewline ""}


# Set up the same bindings for Escape as tk4.0's entry.tcl sets up for Meta
#  M-B          |       Backward one word
    bind Entry <Key-Escape><Key-b> {
      tkEntrySetCursor %W \
		[string wordstart [%W get] [expr [%W index insert] - 1]]
    }
    bind Text <Key-Escape><Key-b> {
	tkTextSetCursor %W {insert - 1c wordstart}
    } 

#  M-F          |       Forward one word
     bind Entry <Key-Escape><Key-f> {
       # The next line is from the tk4.0 entry.tcl.
       tkEntrySetCursor %W [string wordend [%W get] [%W index insert]]
    }
    bind Text <Key-Escape><Key-f> {
       # The next line is from the tk4.0 text.tcl.
       tkTextSetCursor %W {insert wordend}
    } 

    # Kill current selection/insert space character depending on context
    bind Entry <space> {
	if {[selection own] == [focus]} {
	   if [info exists errorInfo] {
		set temp $errorInfo
		unset errorInfo
		set exists 1
	   } else {set exists 0}
	   catch {selection get}
	   if {! [info exists errorInfo]} {
              if {[selection own -displayof %W] == "%W"} {
		clipboard clear -displayof %W
		catch {
		  clipboard append -displayof %W 0 end
		  %W delete sel.first sel.last
		}
	      }
             selection clear %W
	   } else {
		%W insert insert %A
		tkEntrySeeInsert %W
	   }
	   if {$exists} { set errorInfo $temp }
	} else {
		%W insert insert %A
		tkEntrySeeInsert %W
	}
    }

proc ptkRecursiveBind {widget keySeq action} {
    bind $widget $keySeq \
	"[bind [winfo class $widget] $keySeq]; $action"
    foreach child [winfo children $widget] {
	ptkRecursiveBind $child $keySeq $action
    }
}
