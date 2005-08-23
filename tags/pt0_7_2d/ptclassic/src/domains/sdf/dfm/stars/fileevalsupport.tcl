# Support for the FileEval star
#
# Author: Edward A. Lee
# Version: @(#)fileevalsupport.tcl	1.2	1/21/96
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

#### fileeval_evalneeded
# Given a starID, return 1 if evaluation is needed, 0 otherwise.
# Evaluation is needed if:
#
#   (1) There are files in the inputs and these are newer than the outputs.
#   (2) The conditional parameter is NO.
#   (3) The (edited) command is different from the previous invocation.
#
# As a side effect, this procedure sets a Tcl variable
# $starID(editedcommand) equal to an edited version of the command
# parameter.  The editing done is:
#
#   (1) "input#i" is replaced by whatever is returned by the print
#       method of the input particle at the i-th input.
#   (2) "output#i" is replaced by the filename of the i-th output.
#
# It is assumed before entry that the variable $starID(outputfilenames)
# contains a list of output file names.
#
proc fileeval_evalneeded {starID} {
    upvar #0 $starID params

    set newcommand $params(command)

    set input_mtime 0
    set num 1
    foreach input [grabInputs_$starID] {
	if [file readable $input] {
	    set input [glob [ptkExpandEnvVar $input]]
	    set mt [file mtime $input]
	    if {$mt > $input_mtime} {
		set input_mtime $mt
	    }
	}
	# A dummy packet input (probably due to a delay) is
	# identified by having a name containing the string "DUMMY".
	# We replace it with the special string "/dev/null".
	if {[string first DUMMY $input] >= 0} {
	    set input {/dev/null}
	}
	regsub -all input#$num $newcommand $input newcommand
	incr num
    }
    set output_mtime 0
    set all_outputs_present 1
    set num 1
    foreach file $params(outputfilenames) {
	if [file exists $file] {
	    if [file writable $file] {
		regsub -all output#$num $newcommand \
			[glob [ptkExpandEnvVar $file]] newcommand
	    } {
		error "FileEval: output file \"$file\" is not writable"
	    }
	    set mt [file mtime $file]
	    if {$mt < $output_mtime || $output_mtime == 0 } {
		set output_mtime $mt
	    }
	} {
	    set dir [file dirname $file]
	    if {[file isdirectory $dir] && [file writable $dir]} {
		set file "[glob [ptkExpandEnvVar $dir]]/[file tail $file]"
		regsub -all output#$num $newcommand $file newcommand
		set all_outputs_present 0
	    } {
		error "FileEval: output directory \"$dir\" is not writable"
	    }
	}
	incr num
    }
    # If there are any references to inputs or outputs left, then
    # they must be out of range.
    if [regexp {(input#)|(output#)[0-9]+} $newcommand match] {
	error "Reference \"$match\" in command is out of range"
    }

    set params(editedcommand) $newcommand

    # Check here to see whether the command is the same as
    # on the previous invocation, if such information is available.
    # NOTE: The following code really depends on ideosyncracies in pigi,
    # and will have to be rewritten for Tycho.
    global ptkOctHandles
    if {![info exists ptkOctHandles([curuniverse])]} {
	error "Internal error: Can't find oct handle for top-level system"
    }
    set toplevelOctHandle $ptkOctHandles([curuniverse])
    set previouscmd [ptkGetStringProp $toplevelOctHandle \
	    "command $params(fullName)"]
    set commandchanged 1
    if {[string compare $previouscmd $newcommand] == 0} {
	set commandchanged 0
    }

    # FIXME: In derived classes, will it be possible to use this
    # mechanism to trigger execution if parameters have changed?

    # FIXME: We may want to substitute parameter values into the
    # command here also.  Note that this will happen automatically
    # for galaxy parameters, but star parameters may be more useful.
    # Note that the parameter values are in the array $starID.

    if {!$all_outputs_present || \
	    $input_mtime > $output_mtime || \
	    $params(conditional) == 0 || \
	    $commandchanged } {
	ptkSetStringProp $toplevelOctHandle "command $params(fullName)" \
		$newcommand
	return 1
    } {
	return 0
    }
}
