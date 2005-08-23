# Package load file for the Tycho slate demo package
#
# @Author: John Reekie
#
# @Version: @(#)test.tcl	1.2 07/29/98
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

package provide slate.test 1.0

# Just in case someone hasn't loaded the slate package
if [info exists ::TYCHO] {
    package require tycho.edit.slate
} else {
    package require slate
}

# If we are running in Tycho, load the tycho test package.
# Otherwise, just source a copy of the Tycho test defs file.
if [info exists ::TYCHO] {
    package require tycho.util.tytest
} else {
    source [file join $env(SLATE_LIBRARY) test defs2.tcl]
}

# We also have to load this helper file
source [file join $env(SLATE_LIBRARY) test defs.itcl]

# This is used to make the tests watchable
if { ! [::info exists SHORTTIME] } {
    set SHORTTIME 50
}
