# Package load file for the tycho.util.tydoc package
#
# @Author: John Reekie
#
# @Version: $Id$
#
# @Copyright (c) 1995-1998 The Regents of the University of California.
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

package require tycho.kernel.gui
package require tycho.kernel.html

package provide tycho.util.tydoc 2.0

global env auto_path
set env(TYDOC_LIBRARY) [file dirname [info script]]
if { [lsearch -exact $auto_path $env(TYDOC_LIBRARY)] == -1 } {
    lappend auto_path $env(TYDOC_LIBRARY)
}


########### special viewing modes
# FIXME: .idoc should bring up "generic" class viewer.
# Language-specific cases are handled by the mode variable
# in the header string.
# ::tycho::register extensions "idoc" .idoc
# ::tycho::register extensions "itclclass" .itclclass
# ::tycho::register extensions "javaclass" .javaclass

::tycho::register stylesheet "tydoc" \
	[file join {$TYCHO} lib tydoc tydoc.style] \
	[file join ~ .Tycho styles tydoc.style]

# Cliff's IDoc viewer (still under construction)
#::tycho::register mode "idoc" \
#	-command {::tycho::view IDoc -file {%s}} \
#	-viewclass ::tycho::IDoc \
#	-label {IDoc Viewer}  \
#	-category "html" \
#	-underline 0

::tycho::register mode "itclclass" \
	-command {::tycho::view ItclViewer -file {%s}} \
	-viewclass ::tycho::ItclViewer \
	-label {Itcl Class Viewer}  \
	-category "html" \
	-underline 0

::tycho::register mode "javaclass" \
	-command {::tycho::view JavaViewer -file {%s}} \
	-viewclass ::tycho::JavaViewer \
	-label {Java Class Viewer}  \
	-category "html" \
	-underline 0

