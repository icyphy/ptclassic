# list.itcl - Utility functions on lists. See also syntax.tcl.
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


##########################################################################
#
# Common list utility functions.
#
# Many of these are modelled after functions found in functional
# languages. There are a bunch of general utility functions, such
# as chopping lists up and combining in various ways, functions
# for set manipulation, and "higher-order" functions.  The
# script argument to the higher-order functions is a
# specially-constructed script as described for the apply{}
# proc -- I refer to these as "function-scripts."

##########################################################################
#### lchop list n
#
# Split a list into a list of sub-lists, each sub-list containing
# _n_ consecutive elements of the input list. If the length of
# the list is not evenly divided by _n_, then the final incomplete
# sublist is discarded. Example:
# <pre><tcl>
#     lchop {1 2 3 4 5 6 7} 3
# </tcl></pre>
#
proc lchop {list n} {
    set result {}

    while { [llength $list] >= $n } {
	lappend result [ltake $list $n]
	set list [ldrop $list $n]
    }
    return $result
}

##########################################################################
#### lcopy n item
#
# Make a list containing n copies of the specified item. Example:
# <pre><tcl>
#     lcopy 4 x
# </tcl></pre>
#
proc lcopy {n item} {
    set result {}
    while {$n > 0} {
	lappend result $item
	incr n -1
    }
    return $result
}

##########################################################################
#### ldelete list item
#
# Remove an item from a list. Examples:
# <pre><tcl>
#     ldelete {1 2 3 4} 3
# </tcl></pre>
# <pre><tcl>
#     ldelete {1 2 3 4} 5
# </tcl></pre>
#
proc ldelete {list item} {
    set i [lsearch -exact $list $item]

    if { $i != -1 } {
	return [lreplace $list $i $i]
    }

    return $list
}

##########################################################################
#### ldisjoint l1 l2
#
# Return true if l1 and l2 are disjoint -- that is, their
# intersection is null. Examples:
# <pre><tcl>
#     ldisjoint {1 2 3} {4 5 6}
# </tcl></pre>
# <pre><tcl>
#     ldisjoint {1 2 3} {1 3 5}
# </tcl></pre>
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
#### ldrop list n
#
# Drop _n_ list elements. <i>This proc will be deleted
# soon -- do not use.</i>
#
proc ldrop {list n} {
    return [lrange $list $n end]
}

##########################################################################
#### ldropUntil list item
#
# Drop list elements until the given value, _item_ is found. The
# returned list starts with _item_. If the item is not in the list,
# the empty list is returned.Example:
# <pre><tcl>
#    ldropUntil {1 2 3 4} 3
# </tcl></pre>
#
proc ldropUntil {list item} {
    set index [lsearch -exact $list $item]
    if { $index == -1 } {
	return {}
    } elseif { $index == 0 } {
	return $list
    } else {
	return [lrange $list $index end]
    }
}

##########################################################################
#### lfoldl init list script
#
# Fold a list from left to right. The first argument is the starting
# partial result, the second a list, and the third a two-argument
# function-script (see apply{}) to apply to each partial result
# and list element. Example:
# <pre><tcl>
#     lfoldl 1 {2 3 4} {expr %0 * %1}
# </tcl></pre>
#
proc lfoldl {init list script} {
    set result $init
    foreach x $list {
	set result [uplevel apply [list $script] [list $result] [list $x]]
    }
    return $result
}

##########################################################################
#### lfoldr init list script
#
# Fold a list from right to left. This is the same as lfoldl{}, except
# that the list is processed from right to left. If the function-script
# is associative, then the result will be the same.
#
proc lfoldr {init list script} {
    set result $init
    foreach x [lreverse $list] {
	set result [uplevel apply [list $script] [list $result] [list $x]]
    }
    return $result
}

##########################################################################
#### lgenerate init length script
#
# Generate a list by repeated applying a function to its previous
# partial result. The first argument is the starting partial
# result, the second the number of elements to produce, and
# the third a one-argument function-script that produce the nest partial
# result Example:
# <pre><tcl>
#     lgenerate 0 5 {expr %0 + 1}
# </tcl></pre>
#
proc lgenerate {init length script} {
    set result [list $init]
    set count 1
    while { $count < $length } {
	set init [uplevel apply [list $script] [list $init]]
	lappend result $init
	incr count
    }
    return $result
}

##########################################################################
#### lintersection l1 l2
#
# Return the intersection of two lists. Examples:
# <pre><tcl>
#     lintersection {1 2 3} {4 5 6}
# </tcl></pre>
# <pre><tcl>
#     lintersection {1 2 3} {1 3 5}
# </tcl></pre>
#
proc lintersection {l1 l2} {
    set result {}

    foreach i $l1 {
	if {[lsearch -exact $l2 $i] != -1} {
	    lappend result $i
	}
    }

    return $result
}

##########################################################################
#### lmap
#
# Apply a function-script point-wise across multiple lists. The
# _last_ argument is the function-script; all preceding arguments
# are lists. The function-script is applied to one element from
# each list to produce an element of the output list. The function-
# script is evaluated in the caller's context, so "free" variables
# are evaluated correctly. Examples:
# <pre><tcl>
#     set fred 4
#     lmap {1 2 3} {lambda x -> expr $x + 4}
# </tcl></pre>
# <pre><tcl>
#     lmap {1 2 3} {4 5 6} {lambda x y -> expr $x + $y}
# </tcl></pre>
#
# <b>Caveat</b>: The lists must all be the same length, or this function
# Will fail with a cryptic error message. This should be fixed.
#
proc lmap {args} {
    set result {}
    set script [lindex $args end]

    # Create the script to execute. For, say, two arg lists,
    # {1 2 3} and {4 5 6}, it will look (sort of) like this:
    #
    # foreach _0 {1 2 3} _1 {4 5 6} {
    #     lappend result [uplevel apply [list $script] $_0 $_1]
    # }
    set counts [interval 0 [expr [llength $args] - 2]]
    set runthis {foreach}
    foreach c $counts {
	lappend runthis _$c [lindex $args $c]
    }
    set runthismore [list eval uplevel apply [list [list $script]]]
    foreach c $counts {
	lappend runthismore "\[set _$c\]"
    }
    set runthismore [concat lappend result "\[$runthismore\]"]
    lappend runthis $runthismore

    # Now run it, and return the result
    eval $runthis
    return $result
}

##########################################################################
#### lmember list item
#
# Test whether an item is in a list. Examples:
# <pre><tcl>
#     lmember {1 2 3} 2
# </tcl></pre>
# <pre><tcl>
#     lmember {1 2 3} 4
# </tcl></pre>
#
proc lmember {list item} {
    return [expr [lsearch -exact $list $item] != -1]
}

##########################################################################
#### lnub list
#
# Remove duplicates from a list. Example:
# <pre><tcl>
#     lnub {1 2 3 2 1 2}
# </tcl></pre>
#
proc lnub {list} {
    set result {}
    foreach i $list {
	if { [lsearch -exact $result $i] == -1 } {
	    lappend result $i
	}
    }
    return $result
}

##########################################################################
#### lorder list order
#
# Order elements of a list in the same way as elements of another.
# Example:
# <pre><tcl>
#     lorder {5 2 8 6} {1 2 3 4 5 6 7 8 9}
# </tcl></pre>
#
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

## lorder_cp
#
# Utility function used by lorder
#
proc lorder_cp {a b} {
    return [expr [lindex $a 0] - [lindex $b 0]]
}

##########################################################################
#### lreject
#
# Filter a list based on a predicate. The output list contains
# elements of the input list for which the predicate fails. Example:
# <pre><tcl>
#     lreject {0 -3 5 -7 2 8} {lambda x -> expr $x >= 0}
# </tcl></pre>
#
proc lreject {list script} {
    set result {}

    foreach x $list {
	if { ! [uplevel apply [list $script] [list $x]] } {
	    lappend result $x
	}
    }
    return $result
}

##########################################################################
#### lreverse list
#
# Reverse a list. Example:
# <pre><tcl>
#     lreverse {1 2 3 4 5}
# </tcl></pre>
#
proc lreverse {list} {
    set result {}
    foreach i $list {
	set result [linsert $result 0 $i]
    }
    return $result
}

##########################################################################
#### lscanl init list script
#
# Scan across a list from left to right, producing accumulated results.
# This is similar to lfoldl{}, except that the partial results are produced
# into the output list. Example:
# <pre><tcl>
#     lscanl 1 {2 3 4} {expr %0 * %1}
# </tcl></pre>
#
proc lscanl {init list script} {
    set result [list $init]
    foreach x $list {
	set init [uplevel apply [list $script] [list $init] [list $x]]
	lappend result $init
    }
    return $result
}

##########################################################################
#### lscanr init list script
#
# Scan across a list from right to left. This is the same as lscanl{}, except
# that the list is processed from right to left. If the function-script
# is associative, then the result will be the same.
#
proc lscanr {init list script} {
    set result [list $init]
    foreach x [lreverse $list] {
	set init [uplevel apply [list $script] [list $init] [list $x]]
	lappend result $init
    }
    return [lreverse $result]
}

##########################################################################
#### lselect
#
# Filter a list based on a predicate. The output list contains
# elements of the input list for which the predicate is satisfied. Example:
# <pre><tcl>
#     lselect {0 -3 5 -7 2 8} {lambda x -> expr $x >= 0}
# </tcl></pre>
#
proc lselect {list script} {
    set result {}
    
    foreach x $list {
	if { [uplevel apply [list $script] [list $x]] } {
	    lappend result $x
	}
    }
    return $result
}

##########################################################################
#### lsetadd
#
# Add an element to a set (represented as a Tcl list). Examples:
# <pre><tcl>
#     lsetadd {1 2 3} 2
# </tcl></pre>
# <pre><tcl>
#     lsetadd {1 2 3} 5
# </tcl></pre>
#
proc lsetadd {list item} {
    if { [lsearch -exact $list $item] != -1 } {
	return $list
    } else {
	lappend list $item
	return $list
    }
}

##########################################################################
#### lsplit list n
#
# Split a list at the specified index. This is equivalent to
# performing a take{} and a drop{}. Example:
# <pre><tcl>
#     lsplit {1 2 3 4} 2
# </tcl></pre>
#
# Note: You cannot reliably use lsplit{} to split a list into
# its head and tail. *lsplit {1 2 3 4} 1* works, because the
# elements are not lists, but *lsplit {{1 2} {3 4}} 1* is not
# the same as *[list [lhead $l] [ltail $l]]* where
# *l = {{1 2} {3 4}}*.
#
proc lsplit {list n} {
    if { $n <= 0 } {
	return [list {} $list]
    } else {
	incr n -1
	return [list \
		[lrange $list 0 $n] \
		[lreplace $list 0 $n]]
    }
}

##########################################################################
#### lsubset l1 l2
#
# Return true if l1 is a subset of l2. Examples:
# <pre><tcl>
#     lsubset {3 2} {1 2 3 4}
# </tcl></pre>
# <pre><tcl>
#     lsubset {0 3 2} {1 2 3 4}
# </tcl></pre>
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

##########################################################################
#### subsets list
#
# Generate all non-empty subsets of a list. Subsets are not in
# any kind of order. Example:
# <pre><tcl>
#     subsets {1 2 3}
# </tcl></pre>
#
proc subsets {list} {
    set result {}

    if { [llength $list] == 1 } {
	return [lindex $list 0]
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

##########################################################################
#### lsubst list item value
#
# Replace an element of a list with another. If _list_ contains
# _item_, then the returned list has _value_ substituted for
# _value_. If not, then _list_ is returned unchanged.  Example:
# <pre><tcl>
#     lsubst {1 2 3} 2 z
# </tcl></pre>
# <pre><tcl>
#     lsubst {1 2 3} 4 z
# </tcl></pre>
#
proc lsubst {list item value} {
    set i [lsearch -exact $list $item]

    if { $i != -1 } {
	return [lreplace $list $i $i $value]
    }

    return $list
}

##########################################################################
#### ltake list n
#
# Take _n_ list elements. <i>This proc will be deleted
# soon -- do not use.</i>
#
proc ltake {list n} {
    return [lrange $list 0 [expr $n - 1]]
}

##########################################################################
#### ltranspose listlist
#
# Transpose a list of lists. All sub-lists must be the same length.
# Example:
# <pre><tcl>
#     ltranspose {{1 2 3} {5 6 7}}
# </tcl></pre>
#
# Caveat: this procedure may behave a little strangely if
# given a list with only one sub-list, or a list with no
# sub-lists (as in {{1 2 3}} or {1 2 3}).
#
proc ltranspose {listlist} {
    set result {}

    # Create the script to execute. For, say, the list
    # {{1 2 3} {4 5 6}}, it will look (sort of) like this:
    #
    # foreach _0 {1 2 3} _1 {4 5 6} {
    #     lappend result [list $_0 $_1]
    # }
    set counts [interval 0 [expr [llength $listlist] - 1]]
    set runthis {foreach}
    foreach c $counts {
	lappend runthis _$c [lindex $listlist $c]
    }
    set runthismore [list eval list]
    foreach c $counts {
	lappend runthismore "\[set _$c\]"
    }
    set runthismore [concat lappend result "\[$runthismore\]"]
    lappend runthis $runthismore

    # Now run it, and return the result
    eval $runthis
    return $result
}

##########################################################################
#### lunion l1 l2
#
# Return the union of two lists. If the two lists are not
# proper sets, the union is anyway. Example:
# <pre><tcl>
#     lunion {1 2 3} {3 4 5 4}
# </tcl></pre>
#
proc lunion {l1 l2} {
    return [lnub [concat $l1 $l2]]
}
