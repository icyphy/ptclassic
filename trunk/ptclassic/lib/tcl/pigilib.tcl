# Master initialization file for the tcl/tk Ptolemy interface
# Author: Edward A. Lee
# Version: $Id$
#
# This is based on ptkInit.tcl
#
# Copyright (c) 1990-%Q% The Regents of the University of California.
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


# Source the users ~/.ptkrc file if it is present
# Interesting variables to set:
# If gSlowX11Link is set, do not show the mr ptolemy bitmap
# ptkVerboseErrors controls whether we get a stack trace
#
# sun4 says that 'file exists ""' returns 1, rather than 0
if {[glob -nocomplain ~/.ptkrc ] != "" && \
	[file exists [glob -nocomplain ~/.ptkrc ]]} {
    source ~/.ptkrc
}

proc pigilib_init_env {} {
    global env tcl_prompt1 tcl_prompt2 tk_library tk_version
    global ptolemy PTOLEMY

    if { ![info exist env(PTOLEMY)] } {
	set env(PTOLEMY) [glob ~ptolemy]
    }
    set ptolemy $env(PTOLEMY)
    set PTOLEMY $env(PTOLEMY)
    if {$tk_version >= 4.1 } {
	set env(TCL_LIBRARY) $PTOLEMY/tcltk/itcl/lib/itcl/tcl
	set env(TK_LIBRARY) $PTOLEMY/tcltk/itcl/lib/itcl/tk
	set tk_library $env(TK_LIBRARY)
	uplevel #0 {
	    source $PTOLEMY/tcltk/itcl/lib/itcl/tcl/init.tcl
	    source $PTOLEMY/tcltk/itcl/lib/itcl/tk/tk.tcl
	    source $PTOLEMY/tcltk/itcl/lib/itcl/itcl/itcl.tcl
	    source $PTOLEMY/tcltk/itcl/lib/itcl/itk/itk.tcl
	    source $PTOLEMY/tcltk/itcl/lib/itcl/iwidgets/iwidgets.tcl
	}
    } else {
	#set env(TCL_LIBRARY) $env(PTOLEMY)/tcl/tcl[info tclversion]/lib
	#set env(TK_LIBRARY) $env(PTOLEMY)/tcl/tk$tk_version/lib
	set env(TCL_LIBRARY) $env(PTOLEMY)/tcltk/tcl/lib/tcl
	set env(TK_LIBRARY) $env(PTOLEMY)/tcltk/tk/lib/tk
	set tk_library $env(TK_LIBRARY)
	#uplevel #0 source $env(PTOLEMY)/tcl/tcl[info tclversion]/lib/init.tcl
	#uplevel #0 source $env(PTOLEMY)/tcl/tk$tk_version/lib/tk.tcl
	uplevel #0 source $env(PTOLEMY)/tcltk/tcl/lib/tcl/init.tcl
	uplevel #0 source $env(PTOLEMY)/tcltk/tk/lib/tk/tk.tcl
    }

    set tcl_prompt1 "puts -nonewline stdout {pigi> }"
    set tcl_prompt2 "puts -nonewline stdout {pigi? }"

    global gSlowX11Link
    if {! [info exists gSlowX11Link] } {
	set gSlowX11Link 0
    }

}

pigilib_init_env

# Load the help system for ptcl
source $ptolemy/lib/tcl/ptcl_help.tcl

# A procedure to return the rgb value of all color names used in Pigi.
source $ptolemy/lib/tcl/ptkColor.tcl

# Set the Defaults for fonts, colors, etc. for the Pigi Windows.
source $ptolemy/lib/tcl/ptkOptions.tcl

# Other Pigi procedures
# source $ptolemy/lib/tcl/message.tcl

# FIXME: Remove when no longer needed
set unique 1

source $ptolemy/lib/tcl/utilities.tcl

# Always start tycho
ptkStartTycho

source $ptolemy/lib/tcl/ptkBind.tcl
source $ptolemy/lib/tcl/ptkControl.tcl
source $ptolemy/lib/tcl/ptkRunAllDemos.tcl
source $ptolemy/lib/tcl/ptkParams.tcl
source $ptolemy/lib/tcl/ptkBarGraph.tcl
source $ptolemy/lib/tcl/ptkPrfacet.tcl

# Math extensions for parameter parsing
source $ptolemy/lib/tcl/mathexpr.tcl

# Gantt chart support
source $ptolemy/lib/tcl/ptkGantt.tcl
