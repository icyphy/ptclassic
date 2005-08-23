# Copyright (c) 1990-1996 The Regents of the University of California.
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
# Version: @(#)gantt.tcl	1.4	4/21/96
# Programmer: Brian Evans
#
# This Tcl/Tk script runs the ptkGantt.tcl script as a standalone
# application.

# Hide the default wish window
wm withdraw .

# These procedures have no meaning without pigi running
proc ptkClearHighlights {args} {} 
proc ptkHighlight {args} {} 

# Set ptolemy and PTOLEMY as global Tcl variables
set ptolemy $env(PTOLEMY)
set PTOLEMY $env(PTOLEMY)

source $PTOLEMY/lib/tcl/ptkColor.tcl
source $PTOLEMY/lib/tcl/ptkOptions.tcl
source $PTOLEMY/lib/tcl/message.tcl
source $PTOLEMY/lib/tcl/ptkGantt.tcl
source $PTOLEMY/lib/tcl/utilities.tcl

# Exit procedure
proc ptkGanttExit {} {
  destroy .
}

# Parse arguments
if { $argc != 1 } {
  puts stderr "Usage: gantt filename"
  exit 1
} else {
  ptkGanttDisplay unknown [lindex $argv 0] 1
}
