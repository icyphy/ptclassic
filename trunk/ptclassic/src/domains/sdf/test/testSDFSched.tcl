# Tests for SDF Schedulers stars
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

proc sdfTestScheduler {{target loop-SDF} {targetparams {}} {
    set star UpSample

    sdfInitUniverse $star

    star Rampa Ramp
    star "$star.a" $star
    set tmpfile [sdfSetupPrinter]

    target $target
    if { $targetparams != {} } {
	eval $targetparams
    }

    setstate "$star.a" factor 3
    setstate "$star.a" phase 1
    setstate "$star.a" fill 0.0

    connect Rampa output "$star.a" input
    connect "$star.a" output Printa input
    run 10
    wrapup
    return [readTmpFile $tmpfile]
}
######################################################################
#### test UpSample
#
test SDFUpSample {based on $PTOLEMY/src/domains/cgc/demo/upsample} {
    sdfTestScheduler
} {}

