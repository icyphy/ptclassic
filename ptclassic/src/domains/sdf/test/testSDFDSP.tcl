# Tests for SDF DSP stars
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

# Load up the test definitions.
if {[string compare test [info procs test]] == 1} then { 
    source [file join $env(PTOLEMY) src domains sdf test testSDFDefs.tcl]
} {}

# Alphabetical by starname

######################################################################
#### test SDFConvolCx
# 
test SDFConvolCx { SDFConvolCx} {
    set star ConvolCx

    reset __empty__
    domain SDF
    newuniverse ${star}Test SDF
    target loop-SDF

    sdfTestRampCx RectToCxA
    sdfTestRampCx RectToCxB
    set tmpfile [sdfSetupPrinter]

    star "$star.a" $star
    connect "$star.a" out Printa input
    connect RectToCxA output "$star.a" inA
    connect RectToCxB output "$star.a" inB
    setstate "$star.a" truncationDepth 4
    run 10 
    wrapup
    readTmpFile $tmpfile
} {{(0.0,0.0)	
(0.0,0.0)	
(1.25,0.0)	
(5.0,0.0)	
(32.5,0.0)	
(65.0,0.0)	
(102.5,0.0)	
(145.0,0.0)	
(212.5,0.0)	
(285.0,0.0)	
}}

######################################################################
#### test SDFDTFT
# 
test SDFDTFT { SDFDTFT} {
    set star DTFT

    reset __empty__
    domain SDF
    newuniverse ${star}Test SDF
    target loop-SDF

    sdfTestRampCx RectToCxA
    set tmpfile [sdfSetupPrinter]
    star Consta Const 
    star "$star.a" $star

    connect "$star.a" dtft Printa input
    connect RectToCxA output "$star.a" signal
    connect Consta output "$star.a" omega
    setstate "$star.a" length 2
    setstate "$star.a" numberOfSamples 4
    run 16
    wrapup
    readTmpFile $tmpfile
} {{(1.0,0.5)	
(1.0,0.5)	
(1.0,0.5)	
(1.0,0.5)	
(5.0,2.5)	
(5.0,2.5)	
(5.0,2.5)	
(5.0,2.5)	
(9.0,4.5)	
(9.0,4.5)	
(9.0,4.5)	
(9.0,4.5)	
(13.0,6.5)	
(13.0,6.5)	
(13.0,6.5)	
(13.0,6.5)	
(17.0,8.5)	
(17.0,8.5)	
(17.0,8.5)	
(17.0,8.5)	
(21.0,10.5)	
(21.0,10.5)	
(21.0,10.5)	
(21.0,10.5)	
(25.0,12.5)	
(25.0,12.5)	
(25.0,12.5)	
(25.0,12.5)	
(29.0,14.5)	
(29.0,14.5)	
(29.0,14.5)	
(29.0,14.5)	
(33.0,16.5)	
(33.0,16.5)	
(33.0,16.5)	
(33.0,16.5)	
(37.0,18.5)	
(37.0,18.5)	
(37.0,18.5)	
(37.0,18.5)	
(41.0,20.5)	
(41.0,20.5)	
(41.0,20.5)	
(41.0,20.5)	
(45.0,22.5)	
(45.0,22.5)	
(45.0,22.5)	
(45.0,22.5)	
(49.0,24.5)	
(49.0,24.5)	
(49.0,24.5)	
(49.0,24.5)	
(53.0,26.5)	
(53.0,26.5)	
(53.0,26.5)	
(53.0,26.5)	
(57.0,28.5)	
(57.0,28.5)	
(57.0,28.5)	
(57.0,28.5)	
(61.0,30.5)	
(61.0,30.5)	
(61.0,30.5)	
(61.0,30.5)	
}}


######################################################################
#### test SDFFIRCx
# 
test SDFFIRCx { SDFFIRCx} {
    set star FIRCx

    reset __empty__
    domain SDF
    newuniverse ${star}Test SDF
    target loop-SDF

    sdfTestRampCx RectToCxA
    set tmpfile [sdfSetupPrinter]
    star "$star.a" $star

    connect "$star.a" signalOut Printa input
    connect RectToCxA output "$star.a" signalIn
    run 8
    wrapup
    readTmpFile $tmpfile

} {{(0.0,0.0)	
(-0.040609,-0.0203045)	
(-0.082846,-0.041423)	
(0.053447,0.0267235)	
(0.56639,0.283195)	
(1.455983,0.7279915)	
(2.524106,1.262053)	
(3.590601,1.7953005)	
}}

######################################################################
#### test SDFFIRFix
# 
test SDFFIRFix { SDFFIRFix} {
    set star FIRFix

    reset __empty__
    domain SDF
    newuniverse ${star}Test SDF
    target loop-SDF

    star Rampa RampFix
    setstate Rampa OutputPrecision 5.13
    set tmpfile [sdfSetupPrinter]
    star "$star.a" $star

    connect "$star.a" signalOut Printa input
    connect Rampa output "$star.a" signalIn
    run 8
    wrapup
    readTmpFile $tmpfile
} {{0.0	
-0.04058837890625	
-0.08282470703125	
0.053466796875	
0.56640625	
1.45599365234375	
2.52410888671875	
3.590576171875	
}}

######################################################################
#### test SDFFFTCx
# 
test SDFFFTCx { SDFFFTCx} {
    set star FFTCx

    reset __empty__
    domain SDF
    newuniverse ${star}Test SDF
    target loop-SDF

    sdfTestRampCx RectToCxA
    set tmpfile [sdfSetupPrinter]
    star "$star.a" $star

    connect "$star.a" output Printa input
    connect RectToCxA output "$star.a" input
    setstate "$star.a" order 2
    setstate "$star.a" size 4
    run 16
    wrapup
    readTmpFile $tmpfile
} {{(6.0,3.0)	
(-3.0,1.0)	
(-2.0,-1.0)	
(-1.0,-3.0)	
(22.0,11.0)	
(-3.0,1.0)	
(-2.0,-1.0)	
(-1.0,-3.0)	
(38.0,19.0)	
(-3.0,1.0)	
(-2.0,-1.0)	
(-1.0,-3.0)	
(54.0,27.0)	
(-3.0,1.0)	
(-2.0,-1.0)	
(-1.0,-3.0)	
(70.0,35.0)	
(-3.0,1.0)	
(-2.0,-1.0)	
(-1.0,-3.0)	
(86.0,43.0)	
(-3.0,1.0)	
(-2.0,-1.0)	
(-1.0,-3.0)	
(102.0,51.0)	
(-3.0,1.0)	
(-2.0,-1.0)	
(-1.0,-3.0)	
(118.0,59.0)	
(-3.0,1.0)	
(-2.0,-1.0)	
(-1.0,-3.0)	
(134.0,67.0)	
(-3.0,1.0)	
(-2.0,-1.0)	
(-1.0,-3.0)	
(150.0,75.0)	
(-3.0,1.0)	
(-2.0,-1.0)	
(-1.0,-3.0)	
(166.0,83.0)	
(-3.0,1.0)	
(-2.0,-1.0)	
(-1.0,-3.0)	
(182.0,91.0)	
(-3.0,1.0)	
(-2.0,-1.0)	
(-1.0,-3.0)	
(198.0,99.0)	
(-3.0,1.0)	
(-2.0,-1.0)	
(-1.0,-3.0)	
(214.0,107.0)	
(-3.0,1.0)	
(-2.0,-1.0)	
(-1.0,-3.0)	
(230.0,115.0)	
(-3.0,1.0)	
(-2.0,-1.0)	
(-1.0,-3.0)	
(246.0,123.0)	
(-3.0,1.0)	
(-2.0,-1.0)	
(-1.0,-3.0)	
}}


######################################################################
#### test SDFIIRFix
# 
test SDFIIRFix { SDFIIRFix} {
    puts "SDFIIRFix test: expect 4 of 48 fixed-point overflow warnings"
    set star IIRFix

    reset __empty__
    domain SDF
    newuniverse ${star}Test SDF
    target loop-SDF

    star Rampa RampFix
    setstate Rampa OutputPrecision 5.13
    set tmpfile [sdfSetupPrinter]
    star "$star.a" $star

    connect "$star.a" signalOut Printa input
    connect Rampa output "$star.a" signalIn
    run 8
    wrapup
    readTmpFile $tmpfile
} {{0.0	
0.5	
1.0	
1.45001220703125	
1.637451171875	
1.6949462890625	
1.699951171875	
1.699951171875	
}}

######################################################################
#### test SDFLMSCx
# 
test SDFLMSCx { SDFLMSCx} {
    set star LMSCx

    reset __empty__
    domain SDF
    newuniverse ${star}Test SDF
    target loop-SDF

    sdfTestRampCx RectToCxA
    sdfTestRampCx RectToCxB
    set tapfile [::tycho::tmpFileName LMSCx]
    set tmpfile [sdfSetupPrinter]
    star "$star.a" $star
    connect  "$star.a" signalOut Printa input
    connect RectToCxA output "$star.a" signalIn
    connect RectToCxB output "$star.a" error
    setstate "$star.a" saveTapsFile $tapfile
    run 16
    wrapup
    list [readTmpFile $tmpfile] [readTmpFile $tapfile]
} {{{(0.0,0.0)	
(-0.040609,-0.0203045)	
(-0.032846,-0.016423)	
(0.428447,0.2142235)	
(2.07889,1.039445)	
(5.905983,2.9529915)	
(13.286606,6.643303)	
(26.340601,13.1703005)	
(48.191487,24.0957435)	
(83.042373,41.5211865)	
(135.943259,67.9716295)	
(212.744145,106.3720725)	
(320.095031,160.0475155)	
(465.445917,232.7229585)	
(657.046803,328.5234015)	
(903.947689,451.9738445)	
}} {{(13.9594 ,0)
(12.5109 ,0)
(11.2285 ,0)
(10.0016 ,0)
(8.62665 ,0)
(7.11603 ,0)
(5.69837 ,0)
(4.50939 ,0)
}}}
