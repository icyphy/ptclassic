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

} {}


######################################################################
#### test SDFIIRFix
# 
test SDFIIRFix { SDFIIRFix} {

} {}

######################################################################
#### test SDFLMSCx
# 
test SDFLMSCx { SDFLMSCx} {

} {}
