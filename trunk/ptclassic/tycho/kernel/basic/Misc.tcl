##########################################################################
#
# Author:  H. John Reekie
#
# Version: $Id$
#
# Copyright (c) 1996-%Q% The Regents of the University of California.
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
#### abstractMethod
#
# Complain about an abstract method being called.
#
proc ::tycho::abstractMethod {method} {
    error "Method $method of class [uplevel {info context}] is abstract."
}

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
#### getalloptions varname listname
#
# Get all options from an argument list into the specified
# variable. This assumes that all arguments from the first
# option onwards are options.
#
# This is very handy for processing non-option arguments in
# a uniform way: co-ordinates are the example for which this
# procedure was originally written.
#
proc ::tycho::getalloptions {varname listname} {

    upvar $listname l
    upvar $varname  v

    set v {}
    set t [lsearch -glob $l {-[a-z]*}]
    if { $t != -1 } {
	set v [lrange $l $t end]
	set l [lrange $l 0 [expr $t-1]]

	return 1
    }

    return 0
}

##########################################################################
#### getopt option listname
#
# A handy proc ::tycho::for reading option arguments fram an argument
# list. Sort of like configuration options but more controlled.
#
# The first argument is the name of the option, the second the NAME
# of the argument list. A variable with the option name is set if
# the option name, preceded by a dash, is found in the argument list.
# The value set is the one following the option name. The variable
# is _unchanged_ if the option is not present.
#
# Both the option string and the value are deleted from the argument
# list. Returns 1 if the option was found.
#
# For example, if args contains "1 2 -fred nerks 3 4", then after
# the call
# <pre>
#      getopt fred args  ;# NOTE args NOT $args!
# </pre>
# the variable fred is set to "nerks" and args is set to "1 2 3 4".
#
# FIXME: Make listname an optional argument. By default, this proc
# should use "args."
#
proc ::tycho::getopt {option listname} {

    upvar $listname l
    upvar $option   v

    set t [lsearch -exact $l -$option]
    if { $t != -1 } {
	set v [lindex   $l [expr $t+1]]
	set l [lreplace $l $t [expr $t+1]]

	return 1
    }

    return 0
}

##########################################################################
#### getflag option listname
#
# Like getopt{}, but set the option variable to 1 if the
# option flag is there, else 0. Delete the option flag 
# from the argument list.
#
# FIXME: Make listname an optional argument. By default, this proc
# should use "args."
#
proc ::tycho::getflag {option listname} {

    upvar $listname l
    upvar $option   v

    set t [lsearch -exact $l -$option]
    if { $t != -1 } {
	set v 1
	set l [lreplace $l $t $t]

	return 1
    } else {
	set v 0

	return 0
    }
}

##########################################################################
#### readopt option listname
#
# Read an option argument, like getopt{}, but instead of setting
# a variable return the read value of the option.
#
# The argument list is modified as for getopt{}.
#
# Example:
# <pre>
#    set thing [readopt fred args]
# </pre>
# Note that readopt{} does not make getopt{} redundant, since getopt{]
# does not change the option variable if the option is not present.
#
# FIXME: Make listname an optional argument. By default, this proc
# should use "args."
#
proc ::tycho::readopt {option listname} {

    upvar $listname l

    set t [lsearch -exact $l -$option]
    if { $t != -1 } {
	set v [lindex   $l [expr $t+1]]
	set l [lreplace $l $t [expr $t+1]]

	return $v
    }
    return ""
}

##########################################################################
#### readoption option listname
#
# Like readopt{}, but returns all arguments
# following the option flag up to the next option flag.
#
# FIXME: Make listname an optional argument. By default, this proc
# should use "args."
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
