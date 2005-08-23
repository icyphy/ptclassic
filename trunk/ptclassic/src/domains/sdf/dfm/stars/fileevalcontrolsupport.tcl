# Support for the FileEval star
#
# Author: Edward A. Lee
# Version: @(#)fileevalcontrolsupport.tcl	1.2	1/21/96
#
# Copyright (c) 1990-1996 The Regents of the University of California.
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
# See the file $PTOLEMY/copyright for copyright notice,
# limitation of liability, and disclaimer of warranty provisions.
#

#### fileevalcontrol_editcommand
# Given a starID, modify the "${starID}(editedcommand)" string to
# replace references to "intin#i" with the current value of the
# corresponding integer input.  Also, references to "intout#i" are
# replaced with the string "${starID}(intout#i)".  This string
# defines a variable which, if set by the command, will determine
# the value of the integer outputs.
#
proc fileevalcontrol_editcommand {starID} {
    upvar #0 $starID params

    if [info exists params(editedcommand)] {
	set newcommand $params(editedcommand)
    } {
	set newcommand $params(command)
    }

    set num 1
    if [info exists params(intin)] {
	foreach input $params(intin) {
	    regsub -all intin#$num $newcommand $input newcommand
	    incr num
	}
    }

    for {set num 1} {$num <= $params(numintout)} {incr num} {
	regsub -all intout#$num $newcommand "$starID\(intout_$num\)" \
		newcommand
	incr num
    }

    # If there are any references to inputs or outputs left, then
    # they must be out of range.
    if [regexp {(intin#)|(intout#)[0-9]+} $newcommand match] {
	error "Reference \"$match\" in command is out of range"
    }

    set params(editedcommand) $newcommand
}

#### fileevalcontrol_getintout
# If ${starID}(intout#$num) has been set, where $num is the value of
# the second argument, then return its value.  Otherwise, return 0.
#
proc fileevalcontrol_getintout {starID num} {

    global $starID

    if [info exists ${starID}(intout_$num)] {
	return [set ${starID}(intout_$num)]
    } {
	return 0
    }
}
