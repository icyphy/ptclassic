# Package load file for the Tycho slate package
#
# @Author: John Reekie
#
# @Version: @(#)slate.tcl	1.6 07/29/98
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

# Prepare according to whether we are running Tycho, itcl or neither
if [info exists ::TYCHO] {
    package require tycho.kernel.basic
    package provide tycho.edit.slate 2.0
} else {
    package provide slate 3.0
    if ![info exists itcl::version] {
	package require obstcl
	obstcl::itclCompatibility
    }
}

# Make sure the tycho namespace exists
namespace eval ::tycho {}

# Set up auto-loading
global env auto_path
set env(SLATE_LIBRARY) [file dirname [info script]]
if { [lsearch -exact $auto_path $env(SLATE_LIBRARY)] == -1 } {
    lappend auto_path $env(SLATE_LIBRARY)
}

# Load the compatibility procs if not running in Tycho
if ![info exists ::TYCHO] {
    source [file join $env(SLATE_LIBRARY) compat/procs.tcl]
    source [file join $env(SLATE_LIBRARY) compat/math.tcl]
    source [file join $env(SLATE_LIBRARY) compat/list.tcl]
    source [file join $env(SLATE_LIBRARY) compat/classes.tcl]
}

# Create a global "slate" procedure if not running in Tycho
if ![info exists ::TYCHO] {
    uplevel #0 {
	proc slate {name args} {
	    eval ::tycho::slate $name $args
	}
    }
}
