# Tests for SDF Matrix Stars
#
# @Author: Christopher Hylands
#
# @Version: @(#)testSDFMatrix.tcl	1.3 03/28/98
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

# Alphabetical by starname

proc _testSDFPack { {packstar {Pack}} {unpkstar {UnPk}} {convstar {Gain}}} {
    reset __empty__
    domain SDF
    newuniverse ${packstar}Test SDF
    target loop-SDF


    star WaveForm1 WaveForm
	setstate WaveForm1 value {1.5 -1.5 2.2 -2.2}
	setstate WaveForm1 haltAtEnd NO
	setstate WaveForm1 periodic YES
	setstate WaveForm1 period 0

    star Conv1 $convstar
    if { "$convstar" == "FloatToFix" } {
	setstate Conv1 OverflowHandler saturate
	setstate Conv1 ReportOverflow YES
	setstate Conv1 OutputPrecision 5.14
    }
    star Pack1 $packstar
        setstate Pack1 numRows 2
        setstate Pack1 numCols 2
    star UnPk1 $unpkstar
        setstate UnPk1 numRows 2
        setstate UnPk1 numCols 2
    set tmpfile [sdfSetupPrinter Printer1]

    connect WaveForm1 output Conv1 input
    connect Conv1 output Pack1 input	
    connect Pack1 output UnPk1 input
    connect UnPk1 output Printer1 input
    run 1
    wrapup
    regsub -all {	*} [readTmpFile $tmpfile] {} a
	list $a
}

######################################################################
#### test SDFPack_M
# 
test SDFPack_M { SDFPack_M - Similar to part of MatrixTest3} {
    _testSDFPack Pack_M UnPk_M Gain
} {{{1.5
-1.5
2.2
-2.2
}}}

######################################################################
#### test SDFPackCx_M
# 
test SDFPackCx_M { SDFPackCx_M - Similar to part of MatrixTest3} {
    _testSDFPack PackCx_M UnPkCx_M FloatToCx
} {{{(1.5,0.0)
(-1.5,0.0)
(2.2,0.0)
(-2.2,0.0)
}}}

######################################################################
#### test SDFPackFix_M
# 
test SDFPackFix_M { SDFPackFix_M - Similar to part of MatrixTest3} {
    _testSDFPack PackFix_M UnPkFix_M FloatToFix
} {{{1.5
-1.5
2.20001220703125
-2.20001220703125
}}}

######################################################################
#### test SDFPackInt_M
# 
test SDFPackInt_M { SDFPackInt_M - Similar to part of MatrixTest3} {
    _testSDFPack PackInt_M UnPkInt_M FloatToInt 
} {{{2
-1
2
-2
}}}
