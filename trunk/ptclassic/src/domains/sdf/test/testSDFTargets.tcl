# Tests for SDF Schedulers stars
#
# @Author: Christopher Hylands
#
# @Version: @(#)testSDFTargets.tcl	1.4 03/28/98
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

proc sdfTestScheduler {{target loop-SDF} {targetparams {}}} {
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
    run 5
    wrapup
    return [list [readTmpFile $tmpfile] [schedule]]
} 
######################################################################
#### test UpSample
#
test UpSample-loop-SDF  {Run the loop-SDF scheduler on a UpSample universe} {
    sdfTestScheduler loop-SDF
} {{{0.0	
0.0	
0.0	
0.0	
1.0	
0.0	
0.0	
2.0	
0.0	
0.0	
3.0	
0.0	
0.0	
4.0	
0.0	
}} \{\n\ \ \{\ scheduler\ \"Bhattacharyya's\ Loop\ SDF\ Scheduler\"\ \}\n\{\ fire\ UpSampleTest.Rampa\ \}\n\{\ fire\ UpSampleTest.UpSample.a\ \}\n\{\ repeat\ 3\ \{\n\ \ \{\ fire\ UpSampleTest.Printa\ \}\n\}\n\}\n}

######################################################################
#### test UpSample
#
test UpSample-default-SDF-1 \
	{Run the default-SDF scheduler with DEF loopScheduler} {
    sdfTestScheduler default-SDF {targetparam loopScheduler DEF}

} {{{0.0	
0.0	
0.0	
0.0	
1.0	
0.0	
0.0	
2.0	
0.0	
0.0	
3.0	
0.0	
0.0	
4.0	
0.0	
}} {{
  { scheduler "Simple SDF Scheduler" }
  { fire UpSampleTest.Rampa }
  { fire UpSampleTest.UpSample.a }
  { fire UpSampleTest.Printa }
  { fire UpSampleTest.Printa }
  { fire UpSampleTest.Printa }
}
}}

######################################################################
#### test UpSample
#
test UpSample-default-SDF-2 \
	{Run the default-SDF scheduler with CLUST loopScheduler} {
    sdfTestScheduler default-SDF {targetparam loopScheduler CLUST}
} {{{0.0	
0.0	
0.0	
0.0	
1.0	
0.0	
0.0	
2.0	
0.0	
0.0	
3.0	
0.0	
0.0	
4.0	
0.0	
}} \{\n\{\ scheduler\ \"Buck\ and\ Bhattacharyya's\ SDF\ Loop\ Scheduler\"\ \}\n\{\ fire\ UpSampleTest.Rampa\ \}\n\{\ fire\ UpSampleTest.UpSample.a\ \}\n\{\ repeat\ 3\ \{\n\ \ \{\ fire\ UpSampleTest.Printa\ \}\n\}\n\}\n}


######################################################################
#### test UpSample
#
test UpSample-default-SDF-4 \
	{Run the default-SDF scheduler with ACYLOOP loopScheduler} {
    sdfTestScheduler default-SDF {targetparam loopScheduler ACYLOOP}
} {{{0.0	
0.0	
0.0	
0.0	
1.0	
0.0	
0.0	
2.0	
0.0	
0.0	
3.0	
0.0	
0.0	
4.0	
0.0	
}} \{\n\{\ scheduler\ \"Murthy\ and\ Bhattacharyya's\ SDF\ Loop\ scheduler\"\ \}\n\{\ fire\ Rampa\ \}\n\{\ fire\ UpSample.a\ \}\n\{\ repeat\ 3\ \{\n\ \ \{\ fire\ Printa\ \}\n\}\n\}\n}


######################################################################
#### test default-SDF
#
test UpSample-default-SDF-5 \
	{Run the default-SDF scheduler with SJS loopScheduler} {
    sdfTestScheduler default-SDF {targetparam loopScheduler SJS}
} {} {KNOWN_FAILED}

######################################################################
#### test compile-SDF
#
test UpSample-compile-SDF-1 \
	{Run the compile-SDF scheduler with DEF loopScheduler} {
    sdfTestScheduler compile-SDF {targetparam "Looping Level" DEF}
} {{{0.0	
0.0	
0.0	
0.0	
1.0	
0.0	
0.0	
2.0	
0.0	
0.0	
3.0	
0.0	
0.0	
4.0	
0.0	
}} {{
  { scheduler "Simple SDF Scheduler" }
  { fire UpSampleTest.Rampa }
  { fire UpSampleTest.UpSample.a }
  { fire UpSampleTest.Printa }
  { fire UpSampleTest.Printa }
  { fire UpSampleTest.Printa }
}
}}

######################################################################
#### test default-DDF
#
test UpSample-default-DDF-1 {Run the default-DDF Scheduler} {
    sdfTestScheduler default-DDF
} {{{0.0	
0.0	
0.0	
}} {{ { scheduler "Dynamic Dataflow Scheduler" } }}}

######################################################################
#### test default-BDF
#
test UpSample-default-BDF-1 {Run the default-BDF Scheduler} {
    sdfTestScheduler default-BDF
} {{{0.0	
0.0	
0.0	
0.0	
1.0	
0.0	
0.0	
2.0	
0.0	
0.0	
3.0	
0.0	
0.0	
4.0	
0.0	
}} {{
  { scheduler "Buck's Boolean Dataflow Scheduler" }
    { fire UpSampleTest.Rampa }
    { fire UpSampleTest.UpSample.a }
    { repeat 3 {
    { fire UpSampleTest.Printa }
  } }
}
}}
