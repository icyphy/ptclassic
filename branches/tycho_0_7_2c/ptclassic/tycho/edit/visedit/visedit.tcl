# Package load file for the Tycho tycho.edit.visedit package
#
# @Author: John Reekie
#
# @Version: @(#)visedit.tcl	1.6 04/29/98
#
# @Copyright (c) 1998 The Regents of the University of California.
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
# 						PT_COPYRIGHT_VERSION_2
# 						COPYRIGHTENDKEY
##########################################################################

# Based on pp 344-346 of Harrison and McClellan's "Effective Tcl/Tk
# Programming" book

package require tycho.kernel.basic
if [info exists tk_version] {
    package require tycho.kernel.gui
    package require tycho.edit.slate
}

package provide tycho.edit.visedit 2.0

set env(VISEDIT_LIBRARY) [file dirname [info script]]
if { [lsearch -exact $auto_path $env(VISEDIT_LIBRARY)] == -1 } {
    lappend auto_path $env(VISEDIT_LIBRARY)
}

if [info exists tk_version] {
    ### Stylesheets
    ::tycho::register stylesheet "graphics" \
	    [file join $env(VISEDIT_LIBRARY) graphics.style] \
	    [file join ~ .Tycho styles graphics.style]

    ### MODE MAPPINGS
    ::tycho::register extensions "dag" .dag
    ::tycho::register extensions "forest" .fst
    ::tycho::register extensions "std" .std
    ::tycho::register extensions "indexbrowser" .idx

    ### MODES
    ########### graphical editors (alphabetical)

    # Directed-acyclic graph viewer
    ::tycho::register mode "dag" \
	    -command {::tycho::view EditDAG -file {%s}} \
	    -viewclass ::tycho::EditDAG \
	    -label {DAG Editor}  \
	    -category "graphics" \
	    -underline 0

    # NOTE: Not useful on its own.
    # ::tycho::register mode {.fsm} \
	    #       {::tycho::view EditFSM -file {%s}} \
	    #       {Finite state machine editor}

    # Tree viewer
    ::tycho::register mode "forest" \
	    -command {::tycho::view EditForest -file {%s}} \
	    -viewclass ::tycho::EditForest \
	    -label {Forest Editor}  \
	    -category "graphics" \
	    -underline 0

    # State-transition diagram editor
    ::tycho::register mode "std" \
	    -command {::tycho::view EditSTD -file {%s}} \
	    -viewclass ::tycho::EditSTD \
	    -label {State Transition Diagram Editor}  \
	    -category "graphics" \
	    -underline 0

    #### HELP MENU ENTRIES

    # Tycho class diagram
    ::tycho::register help classdiagram \
	    -label "Class Diagram" \
	    -underline 6 \
	    -command {::tycho::openContext \
	    [file join \$TYCHO doc tychoClasses.dag]}

}
