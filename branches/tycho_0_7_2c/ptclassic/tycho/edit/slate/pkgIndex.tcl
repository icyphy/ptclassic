# Package index for the Tycho tycho.edit.slate package
#
# @Author: John Reekie
#
# @Version: @(#)pkgIndex.tcl	1.5 07/29/98
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

# Package name depends on whether we are running Tycho
if [info exists ::TYCHO] {
    package ifneeded tycho.edit.slate 3.0 \
	    [list source [file join $dir slate.tcl]]
} else {
    package ifneeded slate 3.0 \
	    [list source [file join $dir slate.tcl]]
}

# Make sure that the demo package is known about
if [file exists [file join $dir demo pkgIndex.tcl]] {
    set olddir $dir
    set dir [file join $dir demo] ;# $dir is needed by the sourced file
    source [file join $dir pkgIndex.tcl]
    set dir $olddir
}

# Make sure that the test package is known about
if [file exists [file join $dir test pkgIndex.tcl]] {
    set olddir $dir
    set dir [file join $dir test] ;# $dir is needed by the sourced file
    source [file join $dir pkgIndex.tcl]
    set dir $olddir
}
