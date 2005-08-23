# Initialization file for slate demos
#
# @Copyright (c) 1996- The Regents of the University of California.
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
# @(#)init.tcl	1.1 05/06/98

# Perform start-up stuff in case this is being executed directly
# Try SLATE_LIBRARY
if [info exists env(SLATE_LIBRARY)] {
    if { [lsearch -exact $auto_path $env(SLATE_LIBRARY)] }  {
	lappend auto_path $env(SLATE_LIBRARY)
    }
}

# Try TCL_PACKAGES
if [info exists env(TCL_PACKAGES)] {
    if { [lsearch -exact $auto_path $env(TCL_PACKAGES)] == -1 }  {
	lappend auto_path $env(TCL_PACKAGES)
    }
}

# Try TYCHO, or PTOLEMY if there is not TYCHO
if ![info exists env(TYCHO)] {
    if [info exists env(PTOLEMY)] {
	# TYCHO is not set, but PTOLEMY is.
	if [file exists [file join $env(PTOLEMY) tycho]] {
	    set env(TYCHO) [file join $env(PTOLEMY) tycho]
	}
    } 
}
if [info exists env(TYCHO)] {
    if { [lsearch -exact $auto_path $env(TYCHO)] == -1 }  {
	lappend auto_path $env(TYCHO)
    }
}

# Try loading the package. If it doesn't work, print a helpful
# error message and exit
if [catch {package require tycho.edit.slate} err ] {
    puts "Cannot load \"tycho.edit.slate\"\
	    Error was:\n--start--\n$errorInfo\n--end--\n\
	    The auto_path variable lists the directories that were searched.\n\
	    auto_path = $auto_path\n\
	    You can:\n\
	    1. Install tycho.edit.slate in a subdirectory of your \
	    Tcl library\n\
	    2. Install tycho.edit.slate in the directory given by the \
	    TCL_PACKAGES variable\n\
	    3. Set the SLATE_LIBRARY variable to point to the \
	    slate directory\n\
	    4. Set the TYCHO variable to point to your \
	    Tycho installation\n\
	    5. Set the PTOLEMY variable to point to your \
	    Ptolemy installation\n"
    exit
}

# Put the demo procedures in the path
uplevel #0 {
    lappend auto_path [file join $env(SLATE_LIBRARY) demo]
}

# Set a flag so we know we have been initialised
set tychoSlateDemosLoaded 1

# All demo procs have their own namespace
namespaceEval ::tycho::demo

# We don't want to see the . top-level (do we?)
wm iconify .
