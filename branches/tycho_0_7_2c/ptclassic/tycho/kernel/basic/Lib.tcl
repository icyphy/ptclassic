# Master initialization file for the Tycho tcl/tk Ptolemy interface
# Author: Edward A. Lee
# Version: @(#)Lib.tcl	1.8	9/15/95
#
# Copyright (c) 1990-1995 The Regents of the University of California.
# All rights reserved.
#
# Permission is hereby granted, without written agreement and without
# license or royalty fees, to use, copy, modify, and distribute this
# software and its documentation for any purpose, provided that the above
# copyright notice and the following two paragraphs appear in all copies
# of this software.
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
#                                                         COPYRIGHTENDKEY


########################################################################
# Initialize environment variables.
# Note that this is written so that it will work even if it is not
# executed at global level.
#
global env tk_library tk_version
global ptolemy PTOLEMY

if { ![info exist env(PTOLEMY)] } {
    set env(PTOLEMY) [glob ~ptolemy]
}
set ptolemy $env(PTOLEMY)
set PTOLEMY $env(PTOLEMY)

set env(TCL_LIBRARY) $PTOLEMY/tcltk/itcl-2.0b2/lib/tcl7.4
set env(TK_LIBRARY) $PTOLEMY/tcltk/itcl-2.0b2/lib/tk4.0
set tk_library $env(TK_LIBRARY)
uplevel #0 source $PTOLEMY/tcltk/itcl-2.0b2/lib/tcl7.4/init.tcl
uplevel #0 source $PTOLEMY/tcltk/itcl-2.0b2/lib/tk4.0/tk.tcl
uplevel #0 source $PTOLEMY/tcltk/itcl-2.0b2/lib/itcl2.0/init.itcl
uplevel #0 source $PTOLEMY/tcltk/itcl-2.0b2/lib/itk2.0/init.itk

########################################################################
# auto-loading
# Set up the directories to be searched in order of priority.
#
global auto_path
set auto_path [linsert $auto_path 0 \
	$ptolemy/tycho/kernel_itk \
	$ptolemy/tcltk/itcl-2.0b2/lib \
	$ptolemy/tcltk/itcl-2.0b2/lib/iwidgets2.0
]
