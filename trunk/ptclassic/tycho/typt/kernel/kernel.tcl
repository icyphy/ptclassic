# Package load file for the tycho.typt.kernel package
#
# @Author: John Reekie
#
# @Version: $Id$
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

package provide tycho.typt.kernel 0.2.1

global env auto_path
set env(PTKERNEL_LIBRARY) [file dirname [info script]]
if { [lsearch -exact $auto_path $env(PTKERNEL_LIBRARY)] == -1 } {
    lappend auto_path $env(PTKERNEL_LIBRARY)
}

### PTOLEMY-SPECIFIC INITIALIZATION

global ::ptolemyfeature

# ptolemyfeature(ptolemy) is set to 1 if ptolemy is present
set ptolemyfeature(ptolemy) 0

# ptolemyfeature(ptolemyinstalled) is set if the file
# $ptolemy/copyright exists.
# This is not the same as having ptolemy present,
# which means the executing binary can execute ptolemy commands.
set ptolemyfeature(ptolemyinstalled) \
        [file exists [file join $ptolemy copyright]]

# ptolemyfeature(pitcl) is set to 1 if the ::pitcl commands are present,
# which indicates that we have access to the ptolemy kernel
# via ::pitcl
if {[uplevel #0 info namespace all pitcl] == "pitcl"} {
    set ptolemyfeature(pitcl) 1
    set ptolemyfeature(ptolemy) 1
} {
    set ptolemyfeature(pitcl) 0
}

# ptolemyfeature(ptcl) is set to 1 if the ptcl commands are present,
# which indicates that we have access to the ptolemy kernel
# via the older ptcl interface
if {[uplevel #0 info commands domains] !=  {}} {
    set ptolemyfeature(ptcl) 1
    set ptolemyfeature(ptolemy) 1
} {
    set ptolemyfeature(ptcl) 0
}

# ptolemyfeature(octtools) is set to 1 if the ptk commands are present,
# which indicates that we have access to the tcl/octtools commands.
if {[uplevel #0 info commands ptkOpenFacet] != {} && \
	[uplevel #0 info commands pvOpenWindow] != {}} {
    set ptolemyfeature(octtools) 1
    set ptolemyfeature(ptolemy) 1
} {
    set ptolemyfeature(octtools) 0
}

# Check to see if there is a configuration problem
if {$ptolemyfeature(ptolemy) == 1 && $ptolemyfeature(ptolemyinstalled) == 0} {
    puts stderr "Warning: This binary is ptolemy enabled, but we failed\nto\
	    detect a complete ptolemy installation because\n\
	    `[file join $ptolemy copyright]' is missing."
    set ptolemyfeature(ptolemyinstalled) 1
}


### CATEGORIES
global ::ptolemyfeature
if $ptolemyfeature(ptolemyinstalled) {
    ::tycho::register category new "ptolemy" -label "Ptolemy Tools" -underline 6
    ::tycho::register category open "ptolemy" -label "Open Ptolemy Tools"
}
