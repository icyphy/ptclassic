# Common procs for testing DE
#
# @Author: Christopher Hylands
#
# @Version: @(#)testDEDefs.tcl	1.1 12/09/97
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

set TYCHO [file join $env(PTOLEMY) tycho]

# Load up the test definitions.
if {[string compare test [info procs test]] == 1} then { 
    source [file join $TYCHO kernel test testDefs.tcl]
} {}

# Uncomment this to get a full report, or set in your Tcl shell window.
# set VERBOSE 1

# If a file contains non-graphical tests, then it should be named .tcl
# If a file contains graphical tests, then it should be called .itcl
# 
# It would be nice if the tests would work in a vanilla itkwish binary.
# Check for necessary classes and adjust the auto_path accordingly.
#


if {[info command ::tycho::tmpFileName] == {} } {
    uplevel #0 {
	set ::auto_path [linsert $auto_path 0 [file join $TYCHO kernel]] 
    }
}

# Define a few helper functions

######################################################################
#### deSetupPrinter
# Create a printer star that will print to a tmp file.
# Return the name of the tmp file
#
proc deSetupPrinter { {starName Printa}} {
    star $starName Printer
    set tmpfile [::tycho::tmpFileName DEArith]
    setstate $starName fileName $tmpfile
    return $tmpfile
}

######################################################################
#### readTmpFile
# read a file, remove the file, return the value as a list.
#
proc readTmpFile {tmpfile} {
    set fd [open $tmpfile r]
    set retval [read $fd]
    close $fd
    file delete -force $tmpfile
    list $retval
}

######################################################################
#### deInitUniverse
# Initialize a universe by resetting, setting the name and the target.
# This would be a good place to test schedulers.
#
proc deInitUniverse {{name {DE_Init_Universe}} {target loop-DE}} {
    reset __empty__
    domain DE
    newuniverse ${name}Test DE
    seed 100
    target default-DE
    #target loop-DE
    #targetparam loopScheduler "DEF #choices: DEF, CLUST,ACYLOOP"
}

