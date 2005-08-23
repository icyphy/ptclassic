# Tests for SDF Conversion stars
#
# @Author: Christopher Hylands
#
# @Version: @(#)testSDFConv.tcl	1.6 03/28/98
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
#### test SDFCxToFix
# 
test SDFCxToFix { SDFCxToFix} {
    sdfTest1In1Out CxToFix
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
#### test SDFCxToFix
#
test SDFCxToFix-2 { SDFCxToFix: Connect to a fork} {
    sdfTest1In1OutFork CxToFix
} {{{0.0	
1.0	
2.0	
3.0	
4.0	
}} {{0.0	
1.0	
2.0	
3.0	
4.0	
}}}

######################################################################
#### test SDFCxToFloat
# 
test SDFCxToFloat { SDFCxToFloat} {
    sdfTest1In1Out CxToFloat
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
#### test SDFCxToFloat
#
test SDFCxToFloat-2 { SDFCxToFloat: Connect to a fork} {
    sdfTest1In1OutFork CxToFloat
} {{{0.0	
1.0	
2.0	
3.0	
4.0	
}} {{0.0	
1.0	
2.0	
3.0	
4.0	
}}}

######################################################################
#### test SDFCxToInt
# 
test SDFCxToInt { SDFCxToInt } {
    sdfTest1In1Out CxToInt 1.4
} {{0	
1	
3	
4	
6	
7	
8	
10	
11	
13	
}}

######################################################################
#### test SDFCxToInt
#
test SDFCxToInt-2 { SDFCxToInt: Connect to a fork} {
    sdfTest1In1OutFork CxToInt
} {{{0	
1	
2	
3	
4	
}} {{0	
1	
2	
3	
4	
}}}

######################################################################
#### test SDFCxToRect-1
# 
test SDFCxToRect-1 { SDFCxToRect} {
    set star CxToRect
    reset __empty__
    domain SDF
    newuniverse SDFCxToRect SDF
    target loop-SDF

    star Rampa Ramp
    set tmpfile [sdfSetupPrinter]
    set tmpfile2 [sdfSetupPrinter Printb]
    star "$star.a" $star
    connect "$star.a" real Printa input
    connect "$star.a" imag Printb input
    connect Rampa output "$star.a" input

    run 10 
    wrapup
    
    list [readTmpFile $tmpfile] [readTmpFile $tmpfile2]
} {{{0.0	
1.0	
2.0	
3.0	
4.0	
5.0	
6.0	
7.0	
8.0	
9.0	
}} {{0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
}}}

######################################################################
#### test SDFCxToRect-2
# 
test SDFCxToRect-2 { SDFCxToRect -> SDFRectToCx} {
    reset __empty__
    domain SDF
    newuniverse SDFCxToRect SDF
    target loop-SDF

    star Rampa Ramp
    star CxToRect.a CxToRect
    star RectToCx.a RectToCx
    set tmpfile [sdfSetupPrinter]
    connect RectToCx.a output Printa input
    connect CxToRect.a imag RectToCx.a imag
    connect CxToRect.a real RectToCx.a real
    connect Rampa output CxToRect.a input

    run 10 
    wrapup
    
    list [readTmpFile $tmpfile]
} {{{(0.0,0.0)	
(1.0,0.0)	
(2.0,0.0)	
(3.0,0.0)	
(4.0,0.0)	
(5.0,0.0)	
(6.0,0.0)	
(7.0,0.0)	
(8.0,0.0)	
(9.0,0.0)	
}}}

######################################################################
#### test SDFFixToCx
# 
test SDFFixToCx { SDFFixToCx} {
    puts "SDFFixToCx test: Expect 3 out of 10 overflow messages"
    sdfTest1In1Out FixToCx 0.25 "Fix"
} {{(0.0,0.0)	
(0.25,0.0)	
(0.5,0.0)	
(0.75,0.0)	
(1.0,0.0)	
(1.25,0.0)	
(1.5,0.0)	
(1.75,0.0)	
(1.99993896484375,0.0)	
(1.99993896484375,0.0)	
}}

######################################################################
#### test SDFFixToCx
#
test SDFFixToCx-2 { SDFFixToCx: Connect to a fork} {
    sdfTest1In1OutFork FixToCx
} {{{(0.0,0.0)	
(1.0,0.0)	
(2.0,0.0)	
(3.0,0.0)	
(4.0,0.0)	
}} {{(0.0,0.0)	
(1.0,0.0)	
(2.0,0.0)	
(3.0,0.0)	
(4.0,0.0)	
}}}

######################################################################
#### test SDFFixToFloat
# 
test SDFFixToFloat { SDFFixToFloat} {
    puts "SDFFixToFloat test: Expect 1 out of 10 overflow messages"
    sdfTest1In1Out FixToFloat 0.11 "Fix"
} {{0.0	
0.1099853515625	
0.219970703125	
0.3299560546875	
0.43994140625	
0.5499267578125	
0.659912109375	
0.7698974609375	
0.8798828125	
0.9898681640625	
}}

######################################################################
#### test SDFFixToFloat
#
test SDFFixToFloat-2 { SDFFixToFloat: Connect to a fork} {
    sdfTest1In1OutFork FixToFloat
} {{{0.0	
1.0	
2.0	
3.0	
4.0	
}} {{0.0	
1.0	
2.0	
3.0	
4.0	
}}}

######################################################################
#### test SDFFixToInt
# 
test SDFFixToInt { SDFFixToInt} {
    sdfTest1In1Out FixToInt 0.2  "Fix"
} {{0	
0	
0	
0	
0	
1	
1	
1	
1	
1	
}}

######################################################################
#### test SDFFloatToCx
# 
test SDFFloatToCx-1 { SDFFloatToCx: integer steps} {
    sdfTest1In1Out FloatToCx
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
#### test SDFFloatToCx
#
test SDFFloatToCx-2 { SDFFloatToCx: Connect to a fork} {
    sdfTest1In1OutFork FloatToCx
} {{{(0.0,0.0)	
(1.0,0.0)	
(2.0,0.0)	
(3.0,0.0)	
(4.0,0.0)	
}} {{(0.0,0.0)	
(1.0,0.0)	
(2.0,0.0)	
(3.0,0.0)	
(4.0,0.0)	
}}}

######################################################################
#### test SDFFloatToCx
# 
test SDFFloatToCx-3 { SDFFloatToCx: fractional steps} {
    sdfTest1In1Out FloatToCx  0.015625
} {{(0.0,0.0)	
(0.015625,0.0)	
(0.03125,0.0)	
(0.046875,0.0)	
(0.0625,0.0)	
(0.078125,0.0)	
(0.09375,0.0)	
(0.109375,0.0)	
(0.125,0.0)	
(0.140625,0.0)	
}}


######################################################################
#### Test SDFFloatToFix
# 
test SDFFloatToFix { SDFFloatToFix} {
    sdfTest1In1Out FloatToFix
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
#### test SDFFloatToFix
#
test SDFFloatToFix-2 { SDFFloatToFix: Connect to a fork} {
    sdfTest1In1OutFork FloatToFix
} {{{0.0	
1.0	
2.0	
3.0	
4.0	
}} {{0.0	
1.0	
2.0	
3.0	
4.0	
}}}

######################################################################
#### test SDFFloatToInt
# 
test SDFFloatToInt { SDFFloatToInt} {
    sdfTest1In1Out FloatToInt -1.1
} {{0	
-1	
-2	
-3	
-4	
-5	
-7	
-8	
-9	
-10	
}}

######################################################################
#### test SDFFloatToInt
#
test SDFFloatToInt-2 { SDFFloatToInt: Connect to a fork} {
    sdfTest1In1OutFork FloatToInt
} {{{0	
1	
2	
3	
4	
}} {{0	
1	
2	
3	
4	
}}}

######################################################################
#### test SDFIntToBits
# 
test SDFIntToBits { SDFIntToBits} {
    sdfTest1In1Out IntToBits
} {{0	
0	
0	
0	
0	
0	
0	
1	
0	
0	
1	
0	
0	
0	
1	
1	
0	
1	
0	
0	
0	
1	
0	
1	
0	
1	
1	
0	
0	
1	
1	
1	
1	
0	
0	
0	
1	
0	
0	
1	
}}

######################################################################
#### test SDFIntToCx
# 
test SDFIntToCx { SDFIntToCx} {
    puts "SDFIntToCx test: Expect 8 out of 10 overflow messages"
    sdfTest1In1Out IntToCx
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
#### test SDFIntToCx
#
test SDFIntToCx-2 { SDFIntToCx: Connect to a fork} {
    sdfTest1In1OutFork IntToCx
} {{{(0.0,0.0)	
(1.0,0.0)	
(2.0,0.0)	
(3.0,0.0)	
(4.0,0.0)	
}} {{(0.0,0.0)	
(1.0,0.0)	
(2.0,0.0)	
(3.0,0.0)	
(4.0,0.0)	
}}}

######################################################################
#### test SDFIntToFix
# 
test SDFIntToFix { SDFIntToFix} {
    sdfTest1In1Out IntToFix [expr pow(2,13)]
} {{0.0	
8192.0	
16383.5	
16383.5	
16383.5	
16383.5	
16383.5	
16383.5	
16383.5	
16383.5	
}}

######################################################################
#### test SDFIntToFix
#
test SDFIntToFix-2 { SDFIntToFix: Connect to a fork} {
    sdfTest1In1OutFork IntToFix
} {{{0.0	
1.0	
2.0	
3.0	
4.0	
}} {{0.0	
1.0	
2.0	
3.0	
4.0	
}}}

######################################################################
#### test SDFIntToFloat
# 
test SDFIntToFloat { SDFIntToFloat} {
    sdfTest1In1Out IntToFloat
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
#### test SDFIntToFloat
#
test SDFIntToFloat-2 { SDFIntToFloat: Connect to a fork} {
    sdfTest1In1OutFork IntToFloat
} {{{0.0	
1.0	
2.0	
3.0	
4.0	
}} {{0.0	
1.0	
2.0	
3.0	
4.0	
}}}
