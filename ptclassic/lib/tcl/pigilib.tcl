# Master initialization file for the tcl/tk Ptolemy interface
# Author: Edward A. Lee
# Version: @(#)ptkInit.tcl	1.8	10/21/93
#
# Copyright (c) 1990-1993 The Regents of the University of California.
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


proc pigilib_init_env {} {
    global env tcl_prompt1 tcl_prompt2 tk_library tk_version

    if { ![info exist env(PTOLEMY)] } {
	set env(PTOLEMY) [glob ~ptolemy]
    }
    if { ![info exist env(TCL_LIBRARY)] } {
	set env(TCL_LIBRARY) $env(PTOLEMY)/tcl/tcl[info tclversion]/lib
    }
    if { ![info exist env(TK_LIBRARY)] } {
	set env(TK_LIBRARY) $env(PTOLEMY)/tcl/tk${tk_version}/lib
    }
    set tk_library $env(TK_LIBRARY)
    uplevel #0 source [info library]/init.tcl
    uplevel #0 source $tk_library/tk.tcl

    set tcl_prompt1 "puts -nonewline stdout {pigi> }"
    set tcl_prompt2 "puts -nonewline stdout {pigi? }"
}

pigilib_init_env

source $ptolemy/lib/tcl/message.tcl
source $ptolemy/lib/tcl/utilities.tcl
source $ptolemy/lib/tcl/dialog.tcl
source $ptolemy/lib/tcl/ptkBind.tcl
source $ptolemy/lib/tcl/ptkControl.tcl
source $ptolemy/lib/tcl/ptkParams.tcl
source $ptolemy/lib/tcl/ptkBarGraph.tcl
