# Tests for SDF Control stars
#
# @Author: Christopher Hylands
#
# @Version: @(#)testSDFControl.tcl	1.2 03/28/98
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

######################################################################
#### SDFDistributor
#
test SDFDistributor-1.1 {SDFDistributor with Complex inputs} {
    set star Distributor
    sdfInitUniverse $star

    sdfTestRampCx RectToCxA
    set tmpfile [sdfSetupPrinter]
    star "$star.a" $star
    numports "$star.a" output 3
    busconnect  "$star.a" output Printa input 3
    connect RectToCxA output "$star.a" input
    setstate "$star.a" blockSize 5
    run 4
    wrapup
    list [readTmpFile $tmpfile]
} {{{(0.0,0.0)	(5.0,2.5)	(10.0,5.0)	
(1.0,0.5)	(6.0,3.0)	(11.0,5.5)	
(2.0,1.0)	(7.0,3.5)	(12.0,6.0)	
(3.0,1.5)	(8.0,4.0)	(13.0,6.5)	
(4.0,2.0)	(9.0,4.5)	(14.0,7.0)	
(15.0,7.5)	(20.0,10.0)	(25.0,12.5)	
(16.0,8.0)	(21.0,10.5)	(26.0,13.0)	
(17.0,8.5)	(22.0,11.0)	(27.0,13.5)	
(18.0,9.0)	(23.0,11.5)	(28.0,14.0)	
(19.0,9.5)	(24.0,12.0)	(29.0,14.5)	
(30.0,15.0)	(35.0,17.5)	(40.0,20.0)	
(31.0,15.5)	(36.0,18.0)	(41.0,20.5)	
(32.0,16.0)	(37.0,18.5)	(42.0,21.0)	
(33.0,16.5)	(38.0,19.0)	(43.0,21.5)	
(34.0,17.0)	(39.0,19.5)	(44.0,22.0)	
(45.0,22.5)	(50.0,25.0)	(55.0,27.5)	
(46.0,23.0)	(51.0,25.5)	(56.0,28.0)	
(47.0,23.5)	(52.0,26.0)	(57.0,28.5)	
(48.0,24.0)	(53.0,26.5)	(58.0,29.0)	
(49.0,24.5)	(54.0,27.0)	(59.0,29.5)	
}}}
