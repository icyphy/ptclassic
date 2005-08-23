# Tcl/Tk source to demonstrate the use of the TclSource star
#
# Author: Edward A. Lee
# Version: %W%   %G%
#
# Copyright (c) 1993 The Regents of the University of California.
# All rights reserved.
# See the file $PTOLEMY/copyright for copyright notice,
# limitation of liability, and disclaimer of warranty provisions.
#
# This script emits a DE events once per second

# This procedure writes to the single output of the star.
# The time stamp of the output event will be whatever the
# current time stamp is when this is invoked.
# The first output will always have time stamp zero.
# The next invocation is scheduled to occur after 100 milliseconds.
# If the script is used in stars with inputs, then outputs will also
# be triggered when inputs arrive, and at multiples of 100 millisenconds
# thereafter.  If inputs keep arriving, then a large number of interleaved
# output streams with approximate period (in real time, not simulated time)
# will occur.
#
global tclSourceDemoUniv
set tclSourceDemoUniv [curuniverse]

proc goTcl_$starID {starID} {
    global ptkRunFlag tclSourceDemoUniv
    if {![info exists ptkRunFlag($tclSourceDemoUniv)]} {return}
    if {$ptkRunFlag($tclSourceDemoUniv) == {ACTIVE}} {
	setOutputs_$starID 1.0
	after 100 "goTcl_$starID $starID"
    } {
	if {$ptkRunFlag($tclSourceDemoUniv) == {PAUSED}} {
	    after 100 "goTcl_$starID $starID"
	}
    }
}

# call the method to get things started.
goTcl_$starID $starID