# Tcl/Tk source for a bar meter display
#
# Author: Edward A. Lee
# Version: $Id$
#
# Copyright (c) 1993 The Regents of the University of California.
# All rights reserved.
# See the file $PTOLEMY/copyright for copyright notice,
# limitation of liability, and disclaimer of warranty provisions.
#
# When this file is sourced, it is assumed that the following global
# variables have been set:
#	uniqueSymbol
#	numInputs
#	label
#	low
#	high
# where the last three are given values corresponding to parameter values.

set s .run_[curuniverse].${uniqueSymbol}meter

# If a window with the right name already exists, we assume it was
# created by a previous run of the very same star, and hence can be
# used for this new run.

if {![winfo exists $s]} {
    toplevel $s
    wm title $s "Bar Meters"
    wm iconname $s "Bar Meters"

    frame $s.f
    message $s.msg -font -Adobe-times-medium-r-normal--*-180* -width 12c \
	-text $label
    for {set i 0} {$i < $numInputs} {incr i} {
    	ptkMakeMeter $s.f m$i "" $low $high
    }
    button $s.ok -text "DISMISS" -command "destroy $s"
    pack append $s $s.msg {top expand} $s.f top $s.ok {top fillx}

    # In the following procedure, all variables are passed, so
    # there is no need to evaluat any values when the file is sourced.
    proc tkMeterSetValues {uniqueSymbol numInputs} {
        set s .run_[curuniverse].${uniqueSymbol}meter
        set c $s.f
        set inputVals [${uniqueSymbol}grabInputs]
	for {set i 0} {$i < $numInputs} {incr i} {
            set in [lindex $inputVals $i]
            ptkSetMeter $s.f m$i $in
	}
        update
    }

    # In the following definition, the value of uniqueSymbol and
    # numInputs is evaluated when the file is sourced.
    proc ${uniqueSymbol}callTcl {} "
        tkMeterSetValues $uniqueSymbol $numInputs
    "
}
