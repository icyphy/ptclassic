# Tcl/Tk source used as the default for the TclScript star
# Demonstration script for use with a one-output, zero-input TclScript star.
#
# Author: Edward A. Lee
# Version: $Id$
#
# Copyright (c) 1993 The Regents of the University of California.
# All rights reserved.
# See the file $PTOLEMY/copyright for copyright notice,
# limitation of liability, and disclaimer of warranty provisions.
#
# The TclScript star guarantees that prior to reading this it will have
# set the tcl variable named "uniqueSymbol" to some unique string.
# If this file is used more than once by more than one instance of the
# TkScript star, then each instance will use a different string for
# uniqueSymbol.  So this symbol can be used to create unique names
# for windows.
#
# The following functions are defined by the star and registered as
# Tcl procedures before this script is executed:
# 	${uniqueSymbol}setOutputs
# 	${uniqueSymbol}grabInputs
# The first is used by the script to define the value of the outputs
# of the star.  The second is used to read the values of the inputs.
# The script must also define a procedure:
#	${uniqueSymbol}callTcl
# If the star is set for synchronous operation with Tcl, this procedure
# would normally read the inputs (if any) using ${uniqueSymbol}grab_inputs
# and set the outputs (if any) using ${uniqueSymbol}setOutputs.
# It will be called each time the star fires.
# If the star is set for asynchronous operation, then the procedure
# will be called only once before the main loop begins.
# This default script can be used either way, but it creates much
# less of a drain on the computation time if the star is set to
# operate asynchronously.

set s .cp_middle.button$uniqueSymbol
button $s -text "PUSH ME" -fg tan4
pack append .cp_middle $s {top fillx}
bind $s <ButtonPress-1> \
	"${uniqueSymbol}setOutputs 1.0; $s configure -relief sunken"
bind $s <ButtonRelease-1> \
	"${uniqueSymbol}setOutputs 0.0; $s configure -relief raised"
proc ${uniqueSymbol}callTcl {} {}
