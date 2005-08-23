# Common procs for testing SDF
#
# @Author: Christopher Hylands
#
# @Version: $Id$
#
# @Copyright (c) 1997 The Regents of the University of California.
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
#######################################################################

# Tycho test bed, see $TYCHO/doc/coding/testing.html for more information.

set TYCHO [file join $env(PTOLEMY) tycho]

# Load up the test definitions.
if {[string compare test [info procs test]] == 1} then { 
    source [file join $TYCHO kernel test testDefs.tcl]
} {}

# Uncomment this to get a full report, or set in your Tcl shell window.
# set VERBOSE 1

# If a file contains non-graphical tests, then it should be named .tcl
# If a file contains graphical tests, then it should be called .itcl
# 
# It would be nice if the tests would work in a vanilla itkwish binary.
# Check for necessary classes and adjust the auto_path accordingly.
#


if {[info command ::tycho::tmpFileName] == {} } {
    uplevel #0 {
	set ::auto_path [linsert $auto_path 0 [file join $TYCHO kernel]] 
    }
}

# Define a few helper functions

######################################################################
#### sdfSetupPrinter
# Create a printer star that will print to a tmp file.
# Return the name of the tmp file
#
proc sdfSetupPrinter { {starName Printa}} {
    star $starName Printer
    set tmpfile [::tycho::tmpFileName SDFArith]
    setstate $starName fileName $tmpfile
    return $tmpfile
}

######################################################################
#### readTmpFile
# read a file, remove the file, return the value as a list.
#
proc readTmpFile {tmpfile} {
    set fd [open $tmpfile r]
    set retval [read $fd]
    close $fd
    file delete -force $tmpfile
    list $retval
}

######################################################################
#### sdfTest1In1Out
# Test out one input one output star
#
proc sdfTest1In1Out {star {rampStep 1.0} {rampType "Float"} } {
	reset __empty__
	domain SDF
    newuniverse ${star}Test SDF
	target loop-SDF
	star "$star.a" $star
        if [regexp {Fix$} $star] {
	    catch {
		setstate "$star.a" ArrivingPrecision NO
		setstate "$star.a" InputPrecision 15.1
	    }
	    setstate "$star.a" OutputPrecision 15.1
	}
	switch $rampType {
	    Float {
		star Rampa Ramp
	    }
	    Fix {
		star Rampa RampFix
	    }
	    Int {
		star Rampa RampFloat
	    }
	    default {
		error "rampType `$rampType' not supported"
	    }
	}

	setstate Rampa step $rampStep

	set tmpfile [sdfSetupPrinter]

	connect "$star.a" output Printa input
	connect Rampa output "$star.a" input
	run 10 
	wrapup

	return [readTmpFile $tmpfile]
}

######################################################################
#### sdfTestArithmetic2input
# Test out two input arithmetic stars
#
proc sdfTestArithmetic2input {star} {
    reset __empty__
    domain SDF
    newuniverse ${star}Test SDF
    target loop-SDF
    star Rampa Ramp
    star Rampb Ramp
    set tmpfile [sdfSetupPrinter]

    if [regexp {^Sub} $star] {
	star "$star.a" $star
	connect "$star.a" output Printa input
	connect Rampa output "$star.a" pos
	connect Rampb output "$star.a" neg
        if [regexp {Fix$} $star] {
	    setstate "$star.a" ArrivingPrecision NO
	    setstate "$star.a" InputPrecision 15.1
	    setstate "$star.a" OutputPrecision 15.1
	}

    } else {
	star "$star.input=21" $star
	numports "$star.input=21" input 2
	connect "$star.input=21" output Printa input
	connect Rampa output "$star.input=21" "input#1"
	connect Rampb output "$star.input=21" "input#2"
        if [regexp {Fix$} $star] {
	    setstate "$star.input=21" ArrivingPrecision NO
	    setstate "$star.input=21" InputPrecision 15.1
	    setstate "$star.input=21" OutputPrecision 15.1
	}
    }
    run 10 
    wrapup
    
    return [readTmpFile $tmpfile]
}

######################################################################
#### sdfTestRampCx
# Create a pseudo ramp that outputs a complex
#
proc sdfTestRampCx { {starName RectToCx.a} {rampAstep 1} {rampBstep 0.5}} {
    star Rampa$starName Ramp
    star Rampb$starName Ramp
    star $starName RectToCx
    
    setstate Rampa$starName step $rampAstep
    setstate Rampb$starName step $rampBstep

    connect Rampa$starName output $starName real
    connect Rampb$starName output $starName imag

}
