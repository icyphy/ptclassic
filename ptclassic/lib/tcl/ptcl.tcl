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
	set env(TCL_LIBRARY) $env(PTOLEMY)/tcl/tcl[info tclversion]/lib
    }
    uplevel #0 source [info library]/init.tcl

    set tcl_prompt1 "puts -nonewline stdout {ptcl> }"
    set tcl_prompt2 "puts -nonewline stdout {ptcl? }"
    
}

ptcl_init_env

# Load the help system for ptcl
source $env(PTOLEMY)/lib/tcl/ptcl_help.tcl
