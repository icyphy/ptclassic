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
#
# This is the startup file for ptcl.  It is run by the ptcl binary
# after initializing all the ptolemy extensions to tcl (primarily
# the PTcl class's commands), but before any user's rc file is
# run, and before any command line options like -f have been processed.
#
# We must find and load the tcl libraries, and then do any
# argument processing we want.
#
# Author: Kennard White
# $Id$
#

proc ptcl_init_env {} {
    global env tcl_prompt1 tcl_prompt2

    if { ![info exist env(PTOLEMY)] } {
	set env(PTOLEMY) [glob ~ptolemy]
    }
    if { ![info exist env(TCL_LIBRARY)] } {
	#set env(TCL_LIBRARY) $env(PTOLEMY)/tcl/tcl[info tclversion]/lib
	set env(TCL_LIBRARY) $env(PTOLEMY)/tcltk/tcl/lib/tcl
    }
    uplevel #0 source [info library]/init.tcl

    set tcl_prompt1 "puts -nonewline stdout {ptcl> }"
    set tcl_prompt2 "puts -nonewline stdout {ptcl? }"
    
}

ptcl_init_env

# Load the help system for ptcl
source $env(PTOLEMY)/lib/tcl/ptcl_help.tcl

# Math extensions for parameter parsing
source $env(PTOLEMY)/lib/tcl/mathexpr.tcl
