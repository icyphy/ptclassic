# Tests for SDF Contrib stars
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


######################################################################
# Actual start of the tests.  We use one test per star so that
# the output is easier to read.


######################################################################
#### test SDFBeamAbs
#
test SDFBeamAbs {SDFBeamAbs} {
    set star BeamAbs

    reset __empty__
    domain SDF
    newuniverse ${star}Test SDF
    target loop-SDF

    sdfTestRampCx RectToCxA
    set tmpfile [sdfSetupPrinter]
    star "$star.a" $star

    connect "$star.a" output Printa input
    connect RectToCxA output "$star.a" steering
    setstate "$star.a" AngRes 5
    run 3
    wrapup
    readTmpFile $tmpfile

} {{0.0	
0.0	
0.0	
0.0	
0.0	
1.11803398874989	
1.11803398874989	
1.11803398874989	
1.11803398874989	
1.11803398874989	
2.23606797749979	
2.23606797749979	
2.23606797749979	
2.23606797749979	
2.23606797749979	
}}

######################################################################
#### test SDFConstCx1ofN
# 
test SDFConstCx1ofN {SDFConstCx1ofN} {
    set star ConstCx1ofN

    reset __empty__
    domain SDF
    newuniverse ${star}Test SDF
    target loop-SDF

    set tmpfile [sdfSetupPrinter]

    star "$star.a" $star
    connect "$star.a" output Printa input
    setstate "$star.a" ComplexValues "(1.1,2.2) (3.3,4.4)"
    setstate "$star.a" ValueIndex 2
    run 2
    wrapup
    readTmpFile $tmpfile
}  {{(3.3,4.4)	
(3.3,4.4)	
}}

######################################################################
#### test SDFRLSCx

test SDFRLSCx { SDFRLSCx} {
    set star RLSCx

    reset __empty__
    domain SDF
    newuniverse ${star}Test SDF
    target loop-SDF

    sdfTestRampCx RectToCxA
    sdfTestRampCx RectToCxB
    set tapfile [::tycho::tmpFileName RLSCx]
    set tmpfile [sdfSetupPrinter]
    star "$star.a" $star
    connect  "$star.a" output Printa input
    connect RectToCxA output "$star.a" input
    connect RectToCxB output "$star.a" error
    setstate "$star.a" SaveTapsFile $tapfile
    run 16
    wrapup
    list [readTmpFile $tmpfile] [readTmpFile $tapfile]
} {{{(0.0,0.0)	
(0.0,0.0)	
(3.72781771388895,1.86390885694447)	
(11.9713167161342,5.98565835806709)	
(24.5968645608257,12.2984322804128)	
(42.4760292892306,21.2380146446153)	
(66.4879898335967,33.2439949167983)	
(93.7656881630318,46.8828440815159)	
(123.219780550127,61.6098902750634)	
(154.31809763061,77.1590488153048)	
(186.745274181424,93.3726370907118)	
(220.292658489675,110.146329244837)	
(254.812022180775,127.406011090387)	
(290.192675573729,145.096337786865)	
(326.348847233177,163.174423616589)	
(363.212151111664,181.606075555832)	
}} {{(2.20039 ,-0)
(2.58438 ,-0)
(1.22381 ,-0)
(-5.7703 ,-0)
(31.5781 ,-0)
}}}

######################################################################
#### test SDFRLSCxTO
test SDFRLSCxTO { SDFRLSCxTO} {
    set star RLSCxTO

    reset __empty__
    domain SDF
    newuniverse ${star}Test SDF
    target loop-SDF

    sdfTestRampCx RectToCxA
    sdfTestRampCx RectToCxB
    set tapfile [::tycho::tmpFileName RLSCx]
    set tmpfile [sdfSetupPrinter]
    set tmpfile2 [sdfSetupPrinter Printb]
    star "$star.a" $star
    connect  "$star.a" output Printa input
    connect  "$star.a" TO Printb input
    connect RectToCxA output "$star.a" input
    connect RectToCxB output "$star.a" error
    setstate "$star.a" SaveTapsFile $tapfile
    run 16
    wrapup
    list [readTmpFile $tmpfile] [readTmpFile $tmpfile2] [readTmpFile $tapfile]
} {{{(0.0,0.0)	
(0.0,0.0)	
(3.72781771388895,1.86390885694447)	
(11.9713167161342,5.98565835806709)	
(24.5968645608257,12.2984322804128)	
(42.4760292892306,21.2380146446153)	
(66.4879898335967,33.2439949167983)	
(93.7656881630318,46.8828440815159)	
(123.219780550127,61.6098902750634)	
(154.31809763061,77.1590488153048)	
(186.745274181424,93.3726370907118)	
(220.292658489675,110.146329244837)	
(254.812022180775,127.406011090387)	
(290.192675573729,145.096337786865)	
(326.348847233177,163.174423616589)	
(363.212151111664,181.606075555832)	
}} {{(0.0,0.0)	
(0.0,0.0)	
(1.0,0.0)	
(0.0,0.0)	
(0.0,0.0)	
(2.16790862838969,-0.0)	
(2.23379541548255,-0.0)	
(1.0,-0.0)	
(0.0,-0.0)	
(0.0,-0.0)	
(2.02252562598673,-0.0)	
(2.67501224103193,-0.0)	
(4.01898712752365,-0.0)	
(3.47244868322755,-0.0)	
(11.7095809184324,-0.0)	
(2.14502800165847,-0.0)	
}} {{(2.20039 ,-0)
(2.58438 ,-0)
(1.22381 ,-0)
(-5.7703 ,-0)
(31.5781 ,-0)
}}}
