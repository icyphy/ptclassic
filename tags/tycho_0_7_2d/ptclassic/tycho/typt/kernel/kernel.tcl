# Package load file for the tycho.typt.kernel package
#
# @Authors: John Reekie, Christopher Hylands
#
# @Version: @(#)kernel.tcl	1.5 07/21/98
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

if [info exist env(TYCHO)] {
    set tycho $env(TYCHO)
    set TYCHO $env(TYCHO)
}

if [info exist env(PTOLEMY)] {
    set ptolemy $env(PTOLEMY)
    set PTOLEMY $env(PTOLEMY)
    if {![info exists tycho]} {
	set tycho [file join $ptolemy tycho]
	set TYCHO [file join $ptolemy tycho]
    }
}

if {![info exists tycho]} {
    # Neither environment variable is set.
    # See whether there is a ~ptolemy or ~ptdesign user, in that order,
    # that has tycho installed.
    if [file exists [glob [file join ~ptolemy tycho]]] {
	set tycho [glob [file join ~ptolemy tycho]]
	set TYCHO $tycho
    } {
	if [file exists [glob [file join ~ptdesign tycho]]] {
	    set tycho [glob [file join ~ptdesign tycho]]
	    set TYCHO $tycho
	}
    }
}

if {![info exists tycho] || ![file exists $tycho]} {
    # All of the above failed, give up
    error {To run tycho, set your environment variable TYCHO to the tycho home}
}

# If the ptolemy variables are not yet set, set them relative to tycho.
if {![info exists ptolemy]} {
    set ptolemy [file join $tycho ..]
    set PTOLEMY $ptolemy
}


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
# This test was:
#   if {[uplevel #0 info namespace all pitcl] == "pitcl"}
#
if { [lsearch -exact [namespace children ::] ::pitcl] >= 0 } {
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

if { $ptolemyfeature(ptolemy) == 1 } {
    # pigi_version and pigi_filename are set in pigilib/xfunctions.c
    ::tycho::register help aboutptolemy \
	    -label "About Ptolemy" \
	    -underline 0 \
	    -command {ptkStartupMessageAlways $pigi_version $pigi_filename}

    ::tycho::register help ptolemyhomepage \
	    -label "Ptolemy Homepage" \
	    -underline 0 \
	    -command {::tycho::File::openContext \
	    [file join $PTOLEMY doc html index.html]}
}

### CATEGORIES
global ::ptolemyfeature
if $ptolemyfeature(ptolemyinstalled) {
    ::tycho::register category new "ptolemy" -label "Ptolemy Tools" -underline 6
    ::tycho::register category open "ptolemy" -label "Open Ptolemy Tools"
}
