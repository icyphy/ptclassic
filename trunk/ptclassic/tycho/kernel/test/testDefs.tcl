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
    puts stdout "\n"
    puts stdout "==== $test_name $test_description"
    puts stdout "==== Contents of test case:"
    puts stdout "$contents_of_test"
    if {$code != 0} {
        if {$code == 1} {
            puts stdout "==== Test generated error:"
            puts stdout $answer
        } elseif {$code == 2} {
            puts stdout "==== Test generated return exception;  result was:"
            puts stdout $answer
        } elseif {$code == 3} {
            puts stdout "==== Test generated break exception"
        } elseif {$code == 4} {
            puts stdout "==== Test generated continue exception"
        } else {
            puts stdout "==== Test generated exception $code;  message was:"
            puts stdout $answer
        }
    } else {
        puts stdout "==== Result was:"
        puts stdout "$answer"
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
            puts stdout "++++ $test_name PASSED"
	    incr PASSED
        }
    } else {
        print_verbose $test_name $test_description $contents_of_test $code \
                $answer 
        puts stdout "---- Result should have been:"
        puts stdout "$passing_results"
        puts stdout "---- $test_name FAILED" 
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
#### doneTests
# Call this at the bottom of each test file
#
proc doneTests {args} {
    global PASSED FAILED
    puts "Total: [expr $PASSED + $FAILED] Passed: $PASSED Failed: $FAILED"
    flush stdout
}

 
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
