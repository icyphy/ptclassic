# Compatbility procs for running Slate stand-alone.
# If you are a Ptolemy developer and find a bug in here,
# edit the proc in $TYCHO/basic/kernel and copy it to
# this file.
#
# @Version: @(#)procs.tcl	1.1 07/29/98
# @Author: H. John Reekie
#
# @Copyright (c) 1996- The Regents of the University of California.
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
##########################################################################

##########################################################################
#### apply script arg ?arg?
#
# "Apply" a script to some arguments. This is a clumsy imitation
# of higher-order functions, but is very handy for building
# callback scripts and the like.
#
# The script argument can have one of two formats. If it looks
# like this:
# <pre>
#     { .... %0 ..... %1 .... }
# </pre>
# then each _arg_ is substituted for the corresponding place-holder
# and the result evaluated IN THE CALLER'S CONTEXT. If it looks
# like this:
# <pre>
#     { lambda x y -> ... $x ..... $y .... }
# </pre>
# then the names between the "lambda" and the "->" are used to
# substitute argument values into the "body" of the script (after
# the "->". For example, executing
# <pre>
#     apply { lambda x y -> expr $x / $y } 6 3
# </pre>
# will return 2. If there are not enough arguments, then the script
# will _not_ be evaluated, but will be returned as-is. "Currying!"
# (This is not the case for the simpler %0-style format -- in that
# case, the script is evaluated regardless.)
#
# NOTE: Nested scripts do not work reliably (because the substitution
# is simple-minded, and does not understand scope)
#.
proc ::tycho::apply {script args} {
    applylkernel $script $args
}

##########################################################################
#### applyl script {arg ?arg?}
#
# "Apply" a script to a list of arguments. This is the same as
# apply{}, except that the arguments are in a list -- this is more
# convenient when working with a list of optional arguments and so on.
#
proc ::tycho::applyl {script actuals} {
    applylkernel $script $actuals
}

##########################################################################
#### applylkernel script {arg ?arg?}
#
# This is the "kernel" for apply and applyl. It's necessary to
# ensure that the "uplevel" evaluation works correctly.
#
proc ::tycho::applylkernel {script actuals} {
    if { [lindex $script 0] == "lambda" } {
	# This is the more complicated case. Get the names of the formals
	# and the body of the script.
	set arrow [lsearch -exact $script {->}]
	set formals [lrange $script 1 [expr $arrow - 1]]
	set script  [lrange $script [expr $arrow + 1]  end]

	# If the script is "fully applied," then substitute all
	# formals and evaluate it (ignore extra args, if any)
	if { [llength $actuals] >= [llength $formals] } {
	    foreach formal $formals actual $actuals {
		regsub -all \\\$$formal $script [list $actual] script
	    }
	    return [uplevel 2 $script]
	}

	# Otherwise, the number of actuals is less than the number
	# of formals: substitute as many as there are and return the
	# a new script with remaining arguments.
	foreach formal [ltake $formals [llength $actuals]] \
		actual $actuals {
	    regsub -all \\\$$formal $script [list $actual] script
	}
	return "lambda [ldrop $formals [llength $actuals]] -> $script"
    } else {
	# This is the simpler case. Just substitute each argument
	# for the corresponding %n symbol.
	foreach actual $actuals n [::tycho::linterval \
		0 [expr [llength $actuals] - 1]] {
	    regsub -all %$n $script [list $actual] script
	}
	# .. and evaluate
	return [uplevel 2 $script]
    }
}

##########################################################################
#### assign {varnames}+ list
#
# Assign elements of a list to multiple variables. If the number
# of variable names is less then the length of the list, then
# later elements of the list are just ignored. If the number of
# variable names is more than the length of the list, then the
# remaining variables are set to null.
#
# In really critical situations, you can use the obscure Tcl (7.5)
# syntax:
# <pre>
#    foreach {x y z} $list {}
# </pre>
# instead of:
# <pre>
#    assign x y z $list
# </pre>
proc ::tycho::assign {args} {
    foreach var [lreplace $args end end] val [lindex $args end] {
	upvar $var v
	set v $val
    }
}

##########################################################################
#### autoName 
# Return a name constructed by augmenting the provided
# stem with a number to guarantee that the name is unique. A global
# (within the tycho namespace) array autoNames is used to keep track of
# the numbers used for each stem. This procedure should be used for
# window classes instead of the #auto facility in itcl to assign to the
# class valid names for windows. Window names must begin with a period.
# By convention, for a class named "Class", we would use the stem
# ".class". Thus, the single argument should be ".class".
# Note that if you invoke this outside the namespace "tycho", you
# must call it "::tycho::autoName".
#
proc ::tycho::autoName {stem} {
    global ::autoNames
    if {[info exists autoNames] && [info exists autoNames($stem)]} {
        incr autoNames($stem)
    } else {
        set autoNames($stem) 0
    }
    return "$stem$autoNames($stem)"
}

##########################################################################
#### capitalize
#
# Capitalize a word -- first letter is uppercase; rest are lowercase.
#
proc ::tycho::capitalize {word} {
    return [string toupper \
	    [string index $word 0]][string tolower [string range $word 1 end]]
}

##########################################################################
#### readoption option listname
#
# Like readopt{}, but returns all arguments
# following the option flag up to the next option flag.
#
# <b>Deprecated -- do not use.</b>
#
proc ::tycho::readoption {option listname} {

    upvar $listname l

    set t [lsearch -exact $l -$option]
    if { $t != -1 } {
	set ldash [lreplace $l 0 $t]
	set tdash [lsearch -glob $ldash "-*"]

	if { $tdash == -1 } {
	    set tdash [llength $ldash]
	}

	set v [lrange   $ldash 0  [expr $tdash-1]]
	set l [lreplace $l     $t [expr $t+$tdash]]

	return $v
    }

    return ""
}

##########################################################################
#### safeUpdate
# Calling update or even "update idletasks" within a method of any Itcl
# object is very dangerous. The itcl object may be deleted during the
# update, so when the update returns, you will be trying to execute a
# method for an object that no longer exists. If you really have to
# call update inside a method, call this procedure instead, passing it the
# name of the object you are calling from ($this). If after the call to
# update the object no longer exists, then a silent error is triggered.
# This unwinds the calling stack to the top level, preventing this
# procedure from returning.  Note that if this is inside a "catch", then
# all bets are off.  You could get a core dump if after returning from
# the catch the object no longer exists.  The object name given in the
# argument must be visible at the global scope.
#
proc ::tycho::safeUpdate {obj} {
    update
    if {$obj == {}} {return}
    # NOTE: Formerly used
    #   if {[uplevel #0 info objects $obj] == {} && ![winfo exists $obj]}
    if {[info commands $obj] == {}} {
	# If you are having trouble with safeUpdate, uncomment the line below
        # error "::tycho::safeUpdate: command aborted: $obj does not exist"
        ::tycho::silentError
    }
    return 0
}

