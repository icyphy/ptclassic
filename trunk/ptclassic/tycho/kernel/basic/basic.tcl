# Package load file for the tycho.kernel.basic package
#
# @Author: John Reekie
#
# @Version: @(#)basic.tcl	1.6 05/08/98
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

package provide tycho.kernel.basic 2.0
global env auto_path
set env(BASIC_LIBRARY) [file dirname [info script]]
if { [lsearch -exact $auto_path $env(BASIC_LIBRARY)] == -1 } {
    lappend auto_path $env(BASIC_LIBRARY)
}


# The basic package is loaded by every program that uses
# tycho. Set up some "basic" stuff:

# Set global variables used by Tycho packages
# FIXME: why is this here?
if [info exists env(TYCHO)] {
    set TYCHO $env(TYCHO)
    set tycho $env(TYCHO)
}

# Create the ::tycho namespace
namespaceEval ::tycho
