# Common procs for testing ACS
#
# @Author: Christopher Hylands
#
# @Version: @(#)testACSDefs.tcl	1.1 03/28/98
#
# @Copyright (c) 1998 The Regents of the University of California.
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
set TYCHO [file join $env(PTOLEMY) tycho]
set env(TYCHO) $TYCHO

lappend auto_path $TYCHO
package require tycho.kernel.basic
package require tycho.util.tytest

# Uncomment this to get a full report, or set in your Tcl shell window.
# set VERBOSE 1

# Define a few helper functions

######################################################################
#### acsSetupPrinter
# Create a printer star that will print to a tmp file.
# Return the name of the tmp file
#
proc acsSetupPrinter { {starName Printa}} {
    star $starName Printer
    set tmpfile [::tycho::tmpFileName ACSArith]
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
#### acsInitUniverse
# Initialize a universe by resetting, setting the name and the target.
# This would be a good place to test schedulers.
#
proc acsInitUniverse {{name {ACS_Init_Universe}} {target loop-ACS}} {
    reset __empty__
    domain ACS
    newuniverse ${name}Test ACS
    seed 100
    target default-ACS
    #target loop-DE
    #targetparam loopScheduler "DEF #choices: DEF, CLUST,ACYLOOP"
}

