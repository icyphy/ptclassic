#
# syntax.tcl
#
# Functions for defining new "syntax" in tcl. Option reading,
# new versions of foreach and similar functions, and so on.
#

##########################################################################
#
# Author:  H. John Reekie
#
# Version: $Id$
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
##########################################################################


#
# FIXME: add correct handling of return in foreach* etc
#
 
## assign {varnames}+ list
#
# Assign elements of a list to multiple variables. If the number
# of variable names is less then the length of the list, then
# later elements pf the list are just ignored. Of the number of
# variable names is more than the length of the list, then only
# as many variables as there list elements are set. (Actually, I
# wonder if this shouldn't flag an error: is there any reason why
# one would ever want to do this?)
#
# -- varnames: One or more names of variables.
# -- list: The list containing values to assign to the variables.
#
proc assign {args} {
    foreach* var [linit $args] val [llast $args] {
	upvar $var v
	set v $val
    }
}


## behead varname listname
#
# Assign the first element of a list to the specified variable,
# and remove the head from that list. Note that the list is
# passed by _name_, so that it is destructively altered in place.
# For example, suppose that fred equal {1 2 3 4}. Then
#
#     behead x fred
#
# (NB, no $ in front of _fred_) sets _x_ to 1, and _fred_ to {2 3 4}.
# This is particularly useful for extracting optional arguments, 
# as in:
#
#     if { $args != "" } {
#         behead thing args
#     }
#     if { $args != "" } {
#         behead thang args
#     }
#     process thing thang
#
# -- varname: The name of the variable to assign the head of the
#    list to.
#
# -- list: The _name_ of the list to modify.
#
proc behead {varname listname} {
    upvar $varname  v
    upvar $listname l

    set v [lhead $l]
    set l [ltail $l]
}


#
# assert
#
# Print an error if the argument expression fails.
#
# This used to check for a global variable called ASSERTIONS.
# Since, however, tcl has no macro facility, I decided it would
# be better to always print the error. To remove assertions after
# debugging, you have to comment out the call in the relevant source
# files.
#
# Hey, it's not my fault, _I_ didn't design the language!
#
# FIXME: This doesn't work outside tycho!
#
proc assert {expr args} {
    if { ! [uplevel "expr $expr"] } {
	if { $args != "" } {
	    error $args
	} else {
	    error "Assertion failed: $expr"
	}
    }
}


#
# getopt
#
# A handy thang for reading option arguments fram an argument
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
#
#      getopt fred args  ;# NOTE args NOT $args!
#
# the variable fred is set to "nerks" and args is set to "1 2 3 4".
#
# FIXME: Make listname an optional argument. By default, this proc
# should use "args."
#
proc getopt {option listname} {

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


#
# readopt
#
# Read an option argument, like getopt{}, but instead of setting
# a variable return the read value of the option.
#
# The argument list is modified as for getopt{}.
#
# Example:
#
#    set thing [readopt fred args]
#
# Note that readopt{} does not make getopt{} redundant, since getopt{]
# does not change the option variable if the option is not present.
#
# FIXME: Make listname an optional argument. By default, this proc
# should use "args."
#
proc readopt {option listname} {

    upvar $listname l

    set t [lsearch -exact $l -$option]
    if { $t != -1 } {
	set v [lindex   $l [expr $t+1]]
	set l [lreplace $l $t [expr $t+1]]

	return $v
    }
    return ""
}


#
# getoption
#
# Like getopt{}, but sets the option variable to all arguments
# following the option flag up to the next option flag.
#
# FIXME: Make listname an optional argument. By default, this proc
# should use "args."
#
proc getoption {option listname} {

    upvar $listname l
    upvar $option   v

    set t [lsearch -exact $l -$option]
    if { $t != -1 } {
	set ldash [ldrop $l [expr $t+1]]
	set tdash [lsearch -regexp $ldash {-[^0-9]}]

	if { $tdash == -1 } {
	    set tdash [llength $ldash]
	}

	set v [lrange   $ldash 0  [expr $tdash-1]]
	set l [lreplace $l     $t [expr $t+$tdash]]

	return 1
    }

    return 0
}


#
# readoption
#
# Like readopt{}, but returns all arguments
# following the option flag up to the next option flag.
#
# FIXME: Make listname an optional argument. By default, this proc
# should use "args."
#
proc readoption {option listname} {

    upvar $listname l

    set t [lsearch -exact $l -$option]
    if { $t != -1 } {
	set ldash [ldrop $l [expr $t+1]]
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


#
# getflag
#
# Like getopt{}, but set the option variable to 1 if the
# option flag is there, else 0. Delete the option flag 
# from the argument list.
#
# FIXME: Make listname an optional argument. By default, this proc
# should use "args."
#
proc getflag {option listname} {

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


#
# makeflag
#
# More-or-less the reverse of getopt: given the name of a boolean value,
# return a string that is either empty or contains an option. For example,
#
#     makeflag fred
#
# returns "-fred" if $fred is true in the calling environment, and
#
# returns "" if $fred is false in the calling environment.
#
proc makeflag {option} {
    if {[uplevel [list set $option]]} {
	return "-$option"
    } else {
	return ""
    }
}



#
# makeopt
#
# Given the name of a variable, return a string that is either
# empty or contains an option. For example,
#
#     makeopt fred
#
# returns "-fred 123" if $fred is equal to 123 in the calling
# environment, and
#
# returns "" if $fred is "" in the calling environment.
#
proc makeopt {option} {
    set value [uplevel [list set $option]]
    if { $value != "" } {
	return "-$option $value"
    } else {
	return ""
    }
}


#
# setquery
#
# A useful procedure for set-query methods. If the argument 'list'
# is not empty, return the variable 'name'; if it is, set the
# variable (return its value), and optionally execute a script.
#
# Use like this:
#
#   method rate {args} {
#       setquery rate args {
#           do_something
#       }
#   }
#
# WARNING: This function is probably bogus. DO NOT USE.
#
#proc setquery {name arg args} {
#    upvar $name v
#
#    if { $arg == "" } {
#	return $v
#    } else {
#	set v $arg
#	if { $args != "" } {
#	    uplevel [lindex $args 0]
#	}
#    }
#}


#
# loop
#
# Loop $n times. Called as "loop n body." The -counter option
# introduces the name of a variable that ranges from 0 to $n-1.
#
# "body" cannot contain return commands. This could be fixed
# if it ever becomes a problem --- see pg 123 of Ousterhout's
# book.
#
proc loop {args} {
    set v [readopt counter args]
    if {$v != ""} {
	upvar $v counter
    }

    set n    [lindex $args 0]
    set body [lindex $args 1]
    for {set counter 0} {$counter < $n} {incr counter} {
	uplevel $body
    }
}


#
# foreach*
#
# Multi-argument version of foreach. Stops when any one of the
# argument lists runs out of elements.
#
# As for loop{},. "body" cannot contain return commands. As
# for loop{}, this could be fixed later if necessary.
#
# Also as for loop{}, the "-counter" option introduces the name
# of a variable that counts from 0 to n-1, where n is the length
# of the shortest argument list.
#
# How does it work? I wish you hadn't asked.... For each variable
# name, it creates a local variable called i0, i1 etc, and binds
# the local variable to the passed variable with upvar{}. For each
# argument list, it creates local variable called l0, l1 etc.
#
# Then, in a frenzy of list heading and tailing, it sets each i(n)
# to the head element of the corresponding l(n), and evaluates the
# body in the caller's context (so that variable names passed to
# this procedure reference the appropriate i(n)). It takes the tail
# of the lists and, provided none are empty, does the same thing again.
#
# This is very inefficient, of course, and should be one of the
# first things recoded in C when things need speeding up.
#
# FIXME: This procedure ignores the last element if it is an empty
# list. Depending on how you look at it, the problem is either in the
# way that Tcl handles lists, which is totally brain-dead, or in the
# definition of lnull{}. A work-around
# is to ensure that, if the last
# element is an empty list, that it is followed by a space before
# the terminating brace. Really, this should be fixed properly.
#
proc foreach* {args} {
    set c 0

    set v [readopt counter args]
    if {$v != ""} {
	upvar $v counter
    }

    while {[llength $args] > 2} {
	upvar   [lindex $args 0] i$c
	set l$c [lindex $args 1]

	set args [ldrop $args 2]
	incr c
    }

    if {[llength $args] != 1} {
	puts "Wrong number of args to foreach*"
	return
    }

    set body [lindex $args 0]

    set done    0
    set counter 0
    while { ! $done } {
	for {set i 0} {$i < $c} {incr i} {
	    set i$i [lindex [set l$i] 0]
	}

	uplevel $body

	for {set i 0} {$i < $c} {incr i} {
	    set l$i [ltail [set l$i]]
	    if {[lnull [set l$i]]} {
		set done 1
	    }
	}

	incr counter
    }
}


#
# foreachpair
#
# Like foreach{}, but takes _two_ variable names, and assigns them to
# consecutive elements of the argument list.
#
#   foreachpair x y {1 2 3 4} {
#        puts [expr $x + $y]
#   }
#
# prints:
#
#   3
#   7
#
proc foreachpair {x y l body} {
    upvar $x fst
    upvar $y snd

    while { [llength $l] >= 2 } {
	assign fst snd $l
	set l [ldrop $l 2]

	uplevel $body
    }
}


#
# apply
#
# Similar to foreach{}, but returns a list containing the
# result of each application of "body." Body can either be
# an expression, or a series of statements with the final one
# an expression. For example:
#
#   apply x {1 2 3} {
#        set q [expr $x * $x]
#        expr $q * $q  
#   }
#
# In any other language, I'd call this "map."
#
# If you want a counter as well, use apply*.
#
# Don't put a return the body---it doesn't do anything
# sensible. If you want to have the body produce a value
# conditionally you can do this:
#
#    apply x {1 2 3} {
#       if {$x > 2} {expr $x} else {expr $x * $x}
#    }
#
# See also: lmap
#
proc apply {v l body} {

    set result ""

    upvar $v x
    foreach x $l {
	lappend result [uplevel $body]
    }

    return $result
}


#
# filter
#
# Similar to apply{}, but returns a list containing the elements
# of the list for which the body evaluates to non-zero.
#
# See also: lselect, lreject
#
proc filter {v l body} {

    set result ""

    upvar $v x
    foreach x $l {
	if {[uplevel $body]} {
	    lappend result $x
	}
    }

    return $result
}


#
# apply*
#
# Like apply{}, but takes multiple variable-arglist pairs,
# and will also accept the -counter option. It's almost identical
# to foreach*{} above.
#
# For example, to point-wise multiply two lists and pair (in a
# 2-list) each result with its position in the list, do:
#
#    apply* x {1 2 3} y {4 5 6} -counter i {
#        list $i [expr $x * $y]
#    }
#
# FIXME: apply*{} has the same bug with an empty list element at the end
# of the list as foreach*{}.
#
proc apply* {args} {

    set c      0
    set result ""

    set v [readopt counter args]
    if {$v != ""} {
	upvar $v counter
    }

    while {[llength $args] > 2} {
	upvar   [lindex $args 0] i$c
	set l$c [lindex $args 1]

	set args [ldrop $args 2]
	incr c
    }

    if {[llength $args] != 1} {
	puts "Wrong number of args to apply*"
	return
    }

    set body [lindex $args 0]

    set done    0
    set counter 0
    while { ! $done } {
	for {set i 0} {$i < $c} {incr i} {
	    set i$i [lindex [set l$i] 0]
	}

	lappend result [uplevel $body]

	for {set i 0} {$i < $c} {incr i} {
	    set l$i [ltail [set l$i]]
	    if {[lnull [set l$i]]} {
		set done 1
	    }
	}
	
	incr counter
    }

    return $result
}

