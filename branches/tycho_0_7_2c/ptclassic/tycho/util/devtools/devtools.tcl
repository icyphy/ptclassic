# Package load file for the tycho.util.devtools package
#
# @Author: John Reekie
#
# @Version: @(#)devtools.tcl	1.4 04/24/98
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

package provide tycho.util.devtools 2.0
global env auto_path
set env(DEVTOOLS_LIBRARY) [file dirname [info script]]
if { [lsearch -exact $auto_path $env(DEVTOOLS_LIBRARY)] == -1 } {
    lappend auto_path $env(DEVTOOLS_LIBRARY)
}

### MODE MAPPINGS
::tycho::register extensions "javaprofile" .prof

### MODES
########### Compound viewers (alphabetical)

# "Diff" viewer -- will not run on the Macintosh
if {$tcl_platform(platform) != "macintosh"} {
    ::tycho::register mode "diffviewer" \
	    -command {::tycho::view EditDiff -toolbar 1} \
	    -viewclass ::tycho::EditDiff \
	    -label {Diff Viewer}  \
	    -category "tool" \
	    -underline 0
}

# Glimpse interface -- will not run on the Macintosh
if {$tcl_platform(platform) != "macintosh"} {
    ::tycho::register mode "glimpse" \
	    -command {set w [::tycho::autoName .glimpse]; \
	    ::tycho::Glimpse $w -geometry +0+0;\
	    wm deiconify $w} \
	    -label {Glimpse}  \
	    -category "tool" \
	    -underline 0
}

# Itcl class list and class diagram generator
::tycho::register mode "itclclasslist" \
	-command {::tycho::view ItclClassList -file {%s} -toolbar 1} \
	-viewclass ::tycho::ItclClassList \
	-label {Itcl Class List}  \
	-category "tool" \
	-underline 5

# Directed-acyclic graph viewer
::tycho::register mode "javaprofile" \
	-command {::tycho::view ProfileJava -file {%s} -toolbar 1} \
	-viewclass ::tycho::ProfileJava \
	-label {View Java Profile}  \
	-category "tool" \
	-underline 0
