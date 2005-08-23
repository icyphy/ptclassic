# Tests for SDF Matrix Stars
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
#### test SDFUnPkCx_M
# 
test SDFUnPkCx_M { SDFUnPkCx_M - Similar to part of MatrixTest3} {
    set star SDFUnPkCx_M
    reset __empty__
    domain SDF
    newuniverse ${star}Test SDF
    target loop-SDF


    star WaveForm1 WaveFormCx
	setstate WaveForm1 value {(1.5,0) (-1.5,0) (2.2,0) (-2.2,0)}
	setstate WaveForm1 haltAtEnd NO
	setstate WaveForm1 periodic YES

    star PackCx_M1 PackCx_M
        setstate PackCx_M1 numRows 2
        setstate PackCx_M1 numCols 2
    star UnPkCx_M1 UnPkCx_M
        setstate UnPkCx_M1 numRows 2
        setstate UnPkCx_M1 numCols 2
    set tmpfile [sdfSetupPrinter Printer1]

    connect WaveForm1 output PackCx_M1 input
    connect PackCx_M1 output UnPkCx_M1 input
    connect UnPkCx_M1 output Printer1 input
    run 1
    wrapup
    regsub -all {	*} [readTmpFile $tmpfile] {} a
	list $a
} {{{(1.5,0.0)
(-1.5,0.0)
(2.2,0.0)
(-2.2,0.0)
}}}
