# Tests for SDF Arithmetic stars
#
# @Author: Christopher
#
# @Version: @(#)testSDFArith.tcl	1.4 03/28/98
#
# @Copyright (c) 1997-1998 The Regents of the University of California.
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
    source [file join $env(PTOLEMY) src domains sdf test testSDFDefs.tcl]
} {}


######################################################################
# Actual start of the tests.  We use one test per star so that
# the output is easier to read.


# Floating point one input stars

######################################################################
#### test SDFAdd
# 
test SDFAdd { SDFAdd} {
    sdfTest1In1Out Add
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
    sdfTest1In1Out Mpy
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
    sdfTest1In1Out Gain
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
    sdfTest1In1Out Average
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
    sdfTest1In1Out AddCx
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
    sdfTest1In1Out MpyCx
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
test SDFGainCx { SDFGainCx} {
    sdfTest1In1Out GainCx
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
    sdfTest1In1Out AverageCx
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

######################################################################
#### test SDFAverageCx
# 
test SDFMAverageCx { SDFAverageCx, Complex input} {
    sdfTest1In1Out AverageCx 1.1 Complex
} {{(3.85,1.75)	
(12.65,5.75)	
(21.45,9.75)	
(30.25,13.75)	
(39.05,17.75)	
(47.85,21.75)	
(56.65,25.75)	
(65.45,29.75)	
(74.25,33.75)	
(83.05,37.75)	
}}

# Fix one input stars

######################################################################
#### test SDFAddFix
# 
test SDFAddFix { SDFAddFix} {
    sdfTest1In1Out AddFix
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
    sdfTest1In1Out MpyFix
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
test SDFGainFix { SDFGainFix} {
    sdfTest1In1Out GainFix
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
    sdfTest1In1Out AddInt
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
    sdfTest1In1Out MpyInt
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
test SDFGainInt { SDFGainInt} {
    sdfTest1In1Out GainInt
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
    sdfTest1In1Out DivByInt
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
#### SDFAddCx.input=2
# 
test SDFAddCx3 { SDFAddCx.input=2, Complex inputs} {
    sdfTestArithmetic2input AddCx Complex
} {{(0.0,0.0)	
(3.0,0.75)	
(6.0,1.5)	
(9.0,2.25)	
(12.0,3.0)	
(15.0,3.75)	
(18.0,4.5)	
(21.0,5.25)	
(24.0,6.0)	
(27.0,6.75)	
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
#### SDFSubCx.input=2
# 
test SDFSubCx3 { SDFSubCx.input=2, Complex inputs} {
    sdfTestArithmetic2input SubCx Complex
} {{(0.0,0.0)	
(-1.0,0.25)	
(-2.0,0.5)	
(-3.0,0.75)	
(-4.0,1.0)	
(-5.0,1.25)	
(-6.0,1.5)	
(-7.0,1.75)	
(-8.0,2.0)	
(-9.0,2.25)	
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

######################################################################
#### SDFMpyCx.input=2
# 
test SDFMpyCx2 { SDFMpyCx.input=2, Complex input} {
    sdfTestArithmetic2input MpyCx Complex
} {{(0.0,0.0)	
(1.875,1.25)	
(7.5,5.0)	
(16.875,11.25)	
(30.0,20.0)	
(46.875,31.25)	
(67.5,45.0)	
(91.875,61.25)	
(120.0,80.0)	
(151.875,101.25)	
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
#### SDFSubFix.input=2
# 
test SDFSubFix3 { SDFSubFix.input=2, Fix inputs} {
    sdfTestArithmetic2input SubFix Fix
} {{0.0	
0.0	
0.0	
0.5	
1.0	
1.0	
1.0	
1.5	
1.5	
1.5	
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

######################################################################
#### SDFMpyFix.input=2
# 
test SDFMpyFix3 { SDFMpyFix.input=2, Fix input} {
    sdfTestArithmetic2input MpyFix Fix
} {{0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
-0.5	
-0.5	
-0.5	
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
