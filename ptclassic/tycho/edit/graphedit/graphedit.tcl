# Package load file for the graphedit package
#
# @Author: John Reekie
#
# @Version: $Id$
#
# @Copyright (c) 1995-%Q% The Regents of the University of California.
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
package require tycho.kernel.model
if [info exists tk_version] {
    package require tycho.kernel.gui
    package require tycho.edit.slate
}
package provide tycho.edit.graphedit 1.0

global env auto_path
set env(GRAPHEDIT_LIBRARY) [file dirname [info script]]
if { [lsearch -exact $auto_path $env(GRAPHEDIT_LIBRARY)] == -1 } {
    lappend auto_path $env(GRAPHEDIT_LIBRARY)
}

if [info exists tk_version] {
    ### Stylesheets
    ::tycho::register stylesheet "graphedit" \
	    [file join $env(GRAPHEDIT_LIBRARY) graphedit.style] \
	    [file join ~ .Tycho styles graphedit.style]

    ### MODE MAPPINGS
    ::tycho::register extensions "graph" .graph

    ### MODES
    ########### graphical editors (alphabetical)

    # Basic graph viewer
    ::tycho::register mode "graph" \
	    -command {::tycho::view GraphEditor -file {%s}} \
	    -viewclass ::tycho::GraphEditor \
	    -label {Graph Editor}  \
	    -category "graphics" \
	    -underline 0
}
