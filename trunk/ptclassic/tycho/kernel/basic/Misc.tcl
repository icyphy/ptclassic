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
# later elements of the list are just ignored. if the number of
# variable names is more than the length of the list, then an error
# will be generated.
#
# -- varnames: One or more names of variables.
# -- list: The list containing values to assign to the variables.
#
# FIXME: Use the Tcl7.5 foreach{} to implement.
#
proc assign {args} {
    set values [lindex $args end]            ;# llast
    foreach var [lreplace $args end end] {   ;# linit
	upvar $var v

	set v      [lindex   $values 0]      ;# lhead
	set values [lreplace $values 0 0]    ;# ltail
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

    set v [lindex   $l 0]      ;# lhead
    set l [lreplace $l 0 0]    ;# ltail
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


proc configopt {option listname} {

    upvar $listname l
    upvar $option   v

    set t [lsearch -exact $l -$option]
    if { $t != -1 } {
	uplevel configure -$option [lindex   $l [expr $t+1]]
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
	set ldash [lreplace $l 0 $t]
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


## getalloptions varname listname
#
# Get all options from an argument list into the specified
# variable. This assumes that all arguments from the first
# option onwards are options.
#
# This is very handy for processing non-option arguments in
# a uniform way: co-ordinates are the example for which this
# procedure was originally written.
#
# FIXME: This will incorrectly assume that a negative number starts
# the options list!!!
#
proc getalloptions {varname listname} {

    upvar $listname l
    upvar $varname  v

    set v {}
    set t [lsearch -glob $l -*]
    if { $t != -1 } {
	set v [lrange $l $t end]
	set l [lrange $l 0 [expr $t-1]]

	return 1
    }

    return 0
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
# It works as follows: for each variable name x and corresponding list,
# it creates a new variable and list v_x and l_x, where v_x is upvar'ed
# to the caller's variable x. It then builds a script that sets v_x
# to the head of l_x. Thus, the call
#
#     foreach* x {1 2 3} y {4 5 6} {puts "$x $y"}
#
# will build the script
#
#     set v_x [lindex $l_x $counter]; set v_y [lindex $l_y $counter];
#
# In the process, the length of the shortest list is counted. Then
# the generated script is executed to bind each argument variable
# in the caller's context to the correct element of the corresponding
# argument list, and the body executed in the caller's context.
#
# This is quite inefficient, of course, although it's not too bad
# compared with other things you need to do in Tk. For example, I timed
# the constant overhead (for one input list) at about the same as
# creating a 50 x 50 rectangle on the Tk canvas.
#
# FIXME: The functionalty of foreach*{} will be subsumed by foreach*
# in Tcl7.5. When that happens, remove this proc!
#
proc foreach* {args} {
    set v [readopt counter args]
    if {$v != ""} {
	upvar $v counter
    }

    set script {}
    set body  [lindex $args end]
    set outer [llength [lindex $args 1]]
    set inner 0

    while { [llength $args] > 1 } {
	set var [lindex $args 0]         ;# first  element
	upvar $var v_$var
	set   l_$var [lindex $args 1]    ;# second element
	set args [lreplace $args 0 1]    ;# rest of list

	set outer [min $outer [llength [set l_$var]]]
	lappend script "set v_$var \[lindex \$l_$var \$counter\]\;"
    }
    set script [eval concat $script]

    for { set counter 0 } { $counter < $outer } { incr counter } {
	eval $script
	uplevel $body
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
# FIXME: The functionalty of foreachpair{} will be subsumed by foreach*
# in Tcl7.5. When that happens, remove this proc!
#
proc foreachpair {x y l body} {
    upvar $x fst
    upvar $y snd

    while { [llength $l] >= 2 } {
	set fst [lindex $l 0]
	set snd [lindex $l 1]
	set l   [lreplace $l 0 1]

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
proc apply* {args} {
    set v [readopt counter args]
    if {$v != ""} {
	upvar $v counter
    }

    set script {}
    set body  [lindex $args end]
    set outer [llength [lindex $args 1]]
    set inner 0

    while { [llength $args] > 1 } {
	set var [lindex $args 0]         ;# first  element
	upvar $var v_$var
	set   l_$var [lindex $args 1]    ;# second element
	set args [lreplace $args 0 1]    ;# rest of list

	set outer [min $outer [llength [set l_$var]]]
	lappend script "set v_$var \[lindex \$l_$var \$counter\]\;"
    }
    set script [eval concat $script]

    set result {}
    for { set counter 0 } { $counter < $outer } { incr counter } {
	eval $script
	lappend result [uplevel $body]
    }

    return $result
}
