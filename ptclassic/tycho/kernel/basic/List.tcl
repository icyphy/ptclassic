#
# list.itcl
#
# Utility functions on lists. See also syntax.tcl.
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


## lhead, ltail list
#
# Take the head and tail of a list.
#
proc lhead {list} {
    return [lindex $list 0]
}

proc ltail {list} {
    return [lreplace $list 0 0]
}


## linit, llast list
#
# Take the last element of a list, end everything but the last
# element.
#
proc linit {list} {
    return [ltake $list [expr [llength $list] - 1]]
}

proc llast {list} {
    return [lindex $list end]
}


## lnull list
#
# Test for a null list (or element). Written the way it is
# because a) `==' cannot be used if the list starts with a number and b
# llength is not so good because it traverses the whole list.
#
# The second case checks for the list being null but indicated
# by empty braces. I'm confused as to why I need this...
#
proc lnull {list} {
    return [expr (! [string match "?*" $list]) \
	         || [string match "{}" $list]]
}


## ltake, ldrop list n
#
# Take or drop list elements.
#
proc ltake {list n} {
    return [lrange $list 0 [expr $n - 1]]
}

proc ldrop {list n} {
    if { $n == 0 } {
	return $list
    } else {
	return [lreplace $list 0 [expr $n-1]]
    }
}


## ldropUntil list item
#
# Drop list elements until the given value, _item_ is found. The
# returned list starts with _item_. For example,
#
#    ldropUntil {1 2 3 4} 3
#
# returns {3 4}. If the item is not in the list, the empty
# list is returned.
#
proc ldropUntil {list item} {
    set index [lsearch -exact $list $item]
    if { $index == -1 } {
	return {}
    } else {
	return [ldrop $list $index]
    }
}


## lcopy n item
#
# Make a list containing n copies of the specified item
#
proc lcopy {n item} {
    set result {}
    loop $n {
	lappend result $item
    }
    return $result
}


## interval x y
#
# Return list of integers in the range _x_ to _y_. For example,
#
#    interval 2 5
#
# returns {2 3 4 5}.
#
proc interval {x y} {
    set result {}

    while { $x <= $y } {
	lappend result $x
	incr x +1
    }

    return $result
}


## ldistl item list
#
# A left list distribution, as in Backus' FP. Note carefully
# the order of arguments. Example:
#
#     ldistl 1 {2 3 4}
#
# returns {{1 2} {1 3} {1 4}}.
#
proc ldistl {item list} {
    set result {}

    foreach i $list {
	lappend result [list $item $i]
    }

    return $result
}


## ldistr list item
#
# A right list distribution, as in Backus' FP. Note carefully
# the order of arguments. Example:
#
#     ldistr {1 2 3} 4
#
# returns {{1 4} {2 4} {3 4}}.
#
proc ldistr {list item} {
    set result {}

    foreach i $list {
	lappend result [list $i $item]
    }

    return $result
}


## lmember list item
#
# Test whether an item is in a list
#
proc lmember {list item} {
    return [expr [lsearch -exact $list $item] != -1]
}


## lorder list order
#
# Order elements of a list in the same way as elements of another.
# For example:
#
#     lorder {5 2 8 6} {1 2 3 4 5 6 7 8 9}
#
# returns {2 5 6 8}.
#
proc lorder_cp {a b} {
    return [expr [lindex $a 0] - [lindex $b 0]]
}

proc lorder {list order} {
    set nlist {}

    foreach item $list {
	set i [lsearch -exact $order $item]
	if {$i == -1} {
	    set i [llength $order]
	}
	lappend nlist [list $i $item]
    }

    set list {}
    set nlist [lsort -command lorder_cp $nlist]
    foreach item $nlist {
	lappend list [lindex $item 1]
    }

    return $list
}


## lnub list
#
# Remove duplicates from a list
#
proc lnub {list} {
    set result {}
    foreach i $list {
	set list [ltail $list]
	if { ! [lmember $list $i] } {
	    lappend result $i
	}
    }
    return $result
}


## subsets list
#
# Generate all non-empty subsets of a list. Subsets are not in
# any kind of order. For example,
#
#     subsets {1 2 3}
#
# generate {{1 2 3} {1 2} {1 3} 1 {2 3} 2 3}.
#
proc subsets {list} {
    set result {}

    if { [llength $list] == 1 } {
	return [lhead $list]
    }

    foreach i $list {
	set list [ldelete $list $i]
	foreach l [subsets $list] {
	    lappend result [concat $i $l]
	}
	lappend result $i
    }

    return $result
}


## ldelete list item
#
# Remove an item from a list
#
proc ldelete {list item} {
    set i [lsearch -exact $list $item]

    if { $i != -1 } {
	return [lreplace $list $i $i]
    }

    return $list
}


## lsubst list item value
#
# Replace an element of a list with another. If _list_ contains
# _item_, then the returned list has _value_ substituted for
# _value_. If not, then _list_ is returned unchanged.
#
proc lsubst {list item value} {
    set i [lsearch -exact $list $item]

    if { $i != -1 } {
	return [lreplace $list $i $i $value]
    }

    return $list
}


## ldifference l1 l2
#
# Return the difference of two lists: l1 - l2
#
proc lsubtract {l1 l2} {
    set result {}

    if { $l1 == $l2 } {
	return ""
    }

    foreach i $l1 {
	if {[lsearch -exact $l2 $i] == -1} {
	    lappend result $i
	}
    }

    return $result
}

## lsubset l1 l2
#
# Return true if l1 is a subset of l2.
#
proc lsubset {l1 l2} {
    set result {}

    foreach i $l1 {
	if {[lsearch -exact $l2 $i] == -1} {
	    return 0
	}
    }

    return 1
}


## ldisjoint l1 l2
#
# Return true if l1 and l2 are disjoint -- that is, their
# intersection is null.
#
proc ldisjoint {l1 l2} {
    set result {}

    foreach i $l1 {
	if {[lsearch -exact $l2 $i] != -1} {
	    return 0
	}
    }

    return 1
}




##########################################################################
#
# ``Higher-order'' functions on lists.
#
# A miscelleous set of functions that sort-of mimics some of the common
# higher-order functions found in functional languages.
#
# The ``function'' arguments to these functions can either be an
# itcl command name, or a string of the command name and an initial
# set of arguments.
#
# In all of these, the list argument comes _list_, rather than first
# as is common in itcl procs. This is because it's less confusing
# when writing command prefixes: because the list element comes at the
# end of the string, it's better that the list in the call to the
# higher-order function come at the end of the argument list.
#
#


## lmap
#
# Apply a function to every element of a list
#
proc lmap {cmd list} {
    set result {}

    foreach x $list {
	lappend result [eval $cmd [list $x]]
    }
    return $result
}


## lselect
#
# Filter a list based on a predicate. The output list contains
# elements of the input list for which the predicate is satisfied.
#
proc lselect {cmd list} {
    set result {}
    
    foreach x $list {
	if { [eval $cmd [list $x]] } {
	    lappend result $x
	}
    }
    return $result
}


## lreject
#
# Filter a list based on a predicate. The output list contains
# elements of the input list for which the predicate fails.
#
proc lreject {cmd list} {
    set result {}

    foreach x $list {
	if { ! [eval $cmd [list $x]] } {
	    lappend result $x
	}
    }
    return $result
}


## lreduce cmd list
#
# Reduce or ``fold'' a list into a single value. The list must
# contain at least one element.
#
proc lreduce {cmd list} {
    set result [lhead $list]
    set list   [ltail $list]

    while { ! [lnull $list] } {
	set result [eval $cmd $result [list [lhead $list]]]
	set list   [ltail $list]
    }
    return $result
}


