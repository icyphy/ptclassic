# Tests for SDF Arithmetic stars
#
# @Author: Christopher
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
    set tmpfile [::tycho::tmpFileName SDFMpyCx]
    setstate Printa fileName $tmpfile
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
#### sdfTestArithmetic
# Test out one input arithmetic stars
#
proc sdfTestArithmetic {star} {
	reset __empty__
	domain SDF
	newuniverse sdfTestArithmetic SDF
	target loop-SDF
	star "$star.a" $star
        if [regexp {Fix$} $star] {
	    setstate "$star.a" ArrivingPrecision NO
	    setstate "$star.a" InputPrecision 15.1
	    setstate "$star.a" OutputPrecision 15.1
	}
	star Rampa Ramp

	set tmpfile [sdfSetupPrinter]

	connect "$star.a" output Printa input
	connect Rampa output "$star.a" input
	run 10 
	wrapup

	return [readTmpFile $tmpfile]
}


######################################################################
# Actual start of the tests.  We use one test per star so that
# the output is easier to read.


# Floating point one input stars

######################################################################
#### test SDFAdd
# 
test SDFAdd { SDFAdd} {
    sdfTestArithmetic Add
} {{0.0	
1.0	
2.0	
3.0	
4.0	
5.0	
6.0	
7.0	
8.0	
9.0	
}}

######################################################################
#### test SDFMpy
# 
test SDFMpy { SDFMpy} {
    sdfTestArithmetic Mpy
} {{0.0	
1.0	
2.0	
3.0	
4.0	
5.0	
6.0	
7.0	
8.0	
9.0	
}}

######################################################################
#### test SDFGain
# 
test SDFGain { SDFGain} {
    sdfTestArithmetic Gain
} {{0.0	
1.0	
2.0	
3.0	
4.0	
5.0	
6.0	
7.0	
8.0	
9.0	
}}

######################################################################
#### test SDFAverage
# 
test SDFMAverage { SDFAverage} {
    sdfTestArithmetic Average
} {{3.5	
11.5	
19.5	
27.5	
35.5	
43.5	
51.5	
59.5	
67.5	
75.5	
}}

# Cx one input stars

######################################################################
#### test SDFAddCx
# 
test SDFAddCx { SDFAddCx} {
    sdfTestArithmetic AddCx
} {{(0.0,0.0)	
(1.0,0.0)	
(2.0,0.0)	
(3.0,0.0)	
(4.0,0.0)	
(5.0,0.0)	
(6.0,0.0)	
(7.0,0.0)	
(8.0,0.0)	
(9.0,0.0)	
}}

######################################################################
#### test SDFMpyCx
# 
test SDFMpyCx { SDFMpyCx} {
    sdfTestArithmetic MpyCx
} {{(0.0,0.0)	
(1.0,0.0)	
(2.0,0.0)	
(3.0,0.0)	
(4.0,0.0)	
(5.0,0.0)	
(6.0,0.0)	
(7.0,0.0)	
(8.0,0.0)	
(9.0,0.0)	
}}

######################################################################
#### test SDFGainCx
# 
test SDFMpyCx { SDFGainCx} {
    sdfTestArithmetic GainCx
} {{(0.0,0.0)	
(1.0,0.0)	
(2.0,0.0)	
(3.0,0.0)	
(4.0,0.0)	
(5.0,0.0)	
(6.0,0.0)	
(7.0,0.0)	
(8.0,0.0)	
(9.0,0.0)	
}}

######################################################################
#### test SDFAverageCx
# 
test SDFMAverageCx { SDFAverageCx} {
    sdfTestArithmetic AverageCx
} {{(3.5,0.0)	
(11.5,0.0)	
(19.5,0.0)	
(27.5,0.0)	
(35.5,0.0)	
(43.5,0.0)	
(51.5,0.0)	
(59.5,0.0)	
(67.5,0.0)	
(75.5,0.0)	
}}

# Fix one input stars

######################################################################
#### test SDFAddFix
# 
test SDFAddFix { SDFAddFix} {
    sdfTestArithmetic AddFix
} {{0.0	
1.0	
2.0	
3.0	
4.0	
5.0	
6.0	
7.0	
8.0	
9.0	
}}

######################################################################
#### test SDFMpyFix
# 
test SDFMpyFix { SDFMpyFix} {
    sdfTestArithmetic MpyFix
} {{0.0	
1.0	
2.0	
3.0	
4.0	
5.0	
6.0	
7.0	
8.0	
9.0	
}}

######################################################################
#### test SDFGainFix
# 
test SDFMpyFix { SDFGainFix} {
    sdfTestArithmetic GainFix
} {{0.0	
1.0	
2.0	
3.0	
4.0	
5.0	
6.0	
7.0	
8.0	
9.0	
}}

# Int one input stars

######################################################################
#### test SDFAddInt
# 
test SDFAddInt { SDFAddInt} {
    sdfTestArithmetic AddInt
} {{0	
1	
2	
3	
4	
5	
6	
7	
8	
9	
}}

######################################################################
#### test SDFMpyInt
# 
test SDFMpyInt { SDFMpyInt} {
    sdfTestArithmetic MpyInt
} {{0	
1	
2	
3	
4	
5	
6	
7	
8	
9	
}}

######################################################################
#### test SDFGainInt
# 
test SDFMpyInt { SDFGainInt} {
    sdfTestArithmetic GainInt
} {{0	
1	
2	
3	
4	
5	
6	
7	
8	
9	
}}

######################################################################
#### test SDFDivByInt
# 
test SDFDivByInt { SDFDivByInt} {
    sdfTestArithmetic DivByInt
} {{0	
0	
1	
1	
2	
2	
3	
3	
4	
4	
}}

######################################################################
#### sdfTestArithmetic2input
# Test out two input arithmetic stars
#
proc sdfTestArithmetic2input {star} {
    reset __empty__
    domain SDF
    newuniverse SDFArithmeticInput=2 SDF
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

# Two input float stars

######################################################################
#### SDFAdd.input=2
# 
test SDFAdd2 { SDFAdd.input=2} {
    sdfTestArithmetic2input Add
} {{0.0	
2.0	
4.0	
6.0	
8.0	
10.0	
12.0	
14.0	
16.0	
18.0	
}}

######################################################################
#### SDFSub.input=2
# 
test SDFSub2 { SDFSub.input=2} {
    sdfTestArithmetic2input Sub
} {{0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
}}

######################################################################
#### SDFMpy.input=2
# 
test SDFMpy2 { SDFMpy.input=2} {
    sdfTestArithmetic2input Mpy
} {{0.0	
1.0	
4.0	
9.0	
16.0	
25.0	
36.0	
49.0	
64.0	
81.0	
}}

# Two input Cx stars

######################################################################
#### SDFAddCx.input=2
# 
test SDFAddCx2 { SDFAddCx.input=2} {
    sdfTestArithmetic2input AddCx
} {{(0.0,0.0)	
(2.0,0.0)	
(4.0,0.0)	
(6.0,0.0)	
(8.0,0.0)	
(10.0,0.0)	
(12.0,0.0)	
(14.0,0.0)	
(16.0,0.0)	
(18.0,0.0)	
}}

######################################################################
#### SDFSubCx.input=2
# 
test SDFSubCx2 { SDFSubCx.input=2} {
    sdfTestArithmetic2input SubCx
} {{(0.0,0.0)	
(0.0,0.0)	
(0.0,0.0)	
(0.0,0.0)	
(0.0,0.0)	
(0.0,0.0)	
(0.0,0.0)	
(0.0,0.0)	
(0.0,0.0)	
(0.0,0.0)	
}}

######################################################################
#### SDFMpyCx.input=2
# 
test SDFMpyCx2 { SDFMpyCx.input=2} {
    sdfTestArithmetic2input MpyCx
} {{(0.0,0.0)	
(1.0,0.0)	
(4.0,0.0)	
(9.0,0.0)	
(16.0,0.0)	
(25.0,0.0)	
(36.0,0.0)	
(49.0,0.0)	
(64.0,0.0)	
(81.0,0.0)	
}}

# Two input Fix stars

######################################################################
#### SDFAddFix.input=2
# 
test SDFAddFix2 { SDFAddFix.input=2} {
    sdfTestArithmetic2input AddFix
} {{0.0	
2.0	
4.0	
6.0	
8.0	
10.0	
12.0	
14.0	
16.0	
18.0	
}}


######################################################################
#### SDFSubFix.input=2
# 
test SDFSubFix2 { SDFSubFix.input=2} {
    sdfTestArithmetic2input SubFix
} {{0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
}}

######################################################################
#### SDFMpyFix.input=2
# 
test SDFMpyFix2 { SDFMpyFix.input=2} {
    sdfTestArithmetic2input MpyFix
} {{0.0	
1.0	
4.0	
9.0	
16.0	
25.0	
36.0	
49.0	
64.0	
81.0	
}}

# Two input Int stars

######################################################################
#### SDFAddInt.input=2
# 
test SDFAddInt2 { SDFAddInt.input=2} {
    sdfTestArithmetic2input AddInt
} {{0	
2	
4	
6	
8	
10	
12	
14	
16	
18	
}}

######################################################################
#### SDFSubInt.input=2
# 
test SDFSubInt2 { SDFSubInt.input=2} {
    sdfTestArithmetic2input SubInt
} {{0	
0	
0	
0	
0	
0	
0	
0	
0	
0	
}}

######################################################################
#### SDFMpyInt.input=2
# 
test SDFMpyInt2 { SDFMpyInt.input=2} {
    sdfTestArithmetic2input MpyInt
} {{0	
1	
4	
9	
16	
25	
36	
49	
64	
81	
}}

######################################################################

# Test out the other stars in the arithmetic palette

######################################################################
#### SDFIntegrator
# 
test SDFIntegrator { SDFIntegrator} {
    set star Integrator
    reset __empty__
    domain SDF
    newuniverse SDFArithmeticInput=2 SDF
    target loop-SDF
    star Rampa Ramp
    star Consta Const

    set tmpfile [sdfSetupPrinter]

    star "$star.a" $star
    setstate "$star.a" top 20.0
    connect "$star.a" output Printa input
    connect Rampa output "$star.a" data
    connect Consta output "$star.a" reset

    run 10 
    wrapup
    
    readTmpFile $tmpfile
} {{0.0	
1.0	
3.0	
6.0	
10.0	
15.0	
20.0	
20.0	
20.0	
20.0	
}}
