# Tycho test suite definitions
#
# @Authors: Christopher Hylands
#
# @Version: $Id$
#
# @Copyright (c) 1996 The Regents of the University of California.
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


# This file contains support code for the Tcl test suite.  It is
# normally sourced by the individual files in the test suite before
# they run their tests.  This improved approach to testing was designed
# and initially implemented by Mary Ann May-Pumphrey of Sun Microsystems.
#
# Copyright (c) 1990-1994 The Regents of the University of California.
# Copyright (c) 1994 Sun Microsystems, Inc.
#
# ========================================================================
# >>>>>>>>>>>>>>>> INCLUDES MODIFICATIONS FOR [incr Tcl] <<<<<<<<<<<<<<<<<
#
#  AUTHOR:  Michael J. McLennan       Phone: (610)712-2842
#           AT&T Bell Laboratories   E-mail: michael.mclennan@att.com
#     RCS:  $Id$
# ========================================================================
#             Copyright (c) 1993-1995  AT&T Bell Laboratories
# ------------------------------------------------------------------------
#
# See the file "license.terms" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#


if [info exist env(PTOLEMY)] {
    set TYCHO $env(PTOLEMY)/tycho
}

if [info exist env(TYCHO)] {
    set TYCHO $env(TYCHO)
}


if ![info exists FAILED] {
    set FAILED 0
}
if ![info exists PASSED] {
    set PASSED 0
}
if ![info exists VERBOSE] {
    set VERBOSE 0
}
if ![info exists TESTS] {
    set TESTS {}
}

proc print_verbose {test_name test_description contents_of_test code answer} {
    global FAILED
    global errorInfo 
    puts "\n"
    puts "==== $test_name $test_description"
    puts "==== Contents of test case:"
    puts "$contents_of_test"
    if {$code != 0} {
	incr FAILED
        if {$code == 1} {
            puts "==== Test generated error:"
	    puts "$errorInfo"
        } elseif {$code == 2} {
            puts "==== Test generated return exception;  result was:"
            puts $answer
        } elseif {$code == 3} {
            puts "==== Test generated break exception"
        } elseif {$code == 4} {
            puts "==== Test generated continue exception"
        } else {
            puts "==== Test generated exception $code;  message was:"
            puts $answer
        }
    } else {
        puts "==== Result was:"
        puts "$answer"
    }
}

proc test {test_name test_description contents_of_test passing_results} {
    global VERBOSE
    global TESTS PASSED FAILED
    if {[string compare $TESTS ""] != 0} then {
        set ok 0
        foreach test $TESTS {
            if [string match $test $test_name] then {
                set ok 1
                break
            }
        }
        if !$ok then return
    }
    set code [catch {uplevel $contents_of_test} answer]
    if {$code != 0} {
        print_verbose $test_name $test_description $contents_of_test \
                $code $answer
    } elseif {[string compare $answer $passing_results] == 0} then { 
        if $VERBOSE then {
            print_verbose $test_name $test_description $contents_of_test \
                    $code $answer
            puts "++++ $test_name PASSED"

        }
	incr PASSED
    } else {
        print_verbose $test_name $test_description $contents_of_test $code \
                $answer 
        puts "---- Result should have been:"
        puts "$passing_results"
        puts "---- $test_name FAILED" 
	incr FAILED
    }
}

proc dotests {file args} {
    global TESTS PASSED FAILED
    set savedTests $TESTS
    set TESTS $args
    source $file
    set TESTS $savedTests
    doneTests
}

# Below here we have Tycho Specific extensions

############################################################################
#### removeobj
# This procedure removes an object if it exists.
#
proc removeobj {name} {
    if {[info object $name] != ""} {
	delete object $name
    }
}

# How long windows are kept around, in milliseconds
set duration 4000
set longDuration 8000

############################################################################
#### sleep
# sleep for 'seconds'.
#
proc sleep {seconds} {
    puts -nonewline "sleeping $seconds seconds: "
    set endtime [expr [clock seconds] + $seconds]
    while {[clock seconds] < $endtime} {
	puts -nonewline "."
	update
    }
}
############################################################################
#### openAllFiles 
# Open up the files that are passed in as arguments, then destroy
# the windows after a short wait
#
proc openAllFiles {args} {
    global VERBOSE
    global duration
    foreach testfile $args {
	if {$VERBOSE == 1} {
	    puts "$testfile"
	}
	if [ file exists [::tycho::expandPath $testfile]] {
	    set win [::tycho::File::openContext $testfile]
	    #	wm deiconify $win
	    puts "testDefs.tcl: openAllFiles{}: win = $win"
	    if [catch {$win displayer windowName}] {
		after [expr {2 * $duration}] removeobj $win
	    } else {
		after [expr {2 * $duration}] removeobj \
			[$win displayer windowName]
	    }
	    update
	}
    }

}

############################################################################
#### doneTests
# Call this at the bottom of each test file
# If reallyExit exists and is not set to 1, then don't exist
#
proc doneTests {args} {
    global PASSED FAILED duration reallyExit
    puts "Total: [expr $PASSED + $FAILED] (Passed: $PASSED Failed: $FAILED)"
    flush stderr
    update
    if {![info exists reallyExit] || $reallyExit == 1} {
	after [expr {2 * $duration}] ::tycho::TopLevel::exitProgram
    }
}
