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




## lchop list n
#
# Split a list into a list of sub-lists, each sub-list containing
# _n_ consecutive elements of the input list. If the length of
# the list is not evenly divded by _n_, then the final incomplete
# sublist is discarded. For example,
#
#     lchop {1 2 3 4 5 6 7} 3
#
# returns {{1 2 3} {4 5 6}}.
#
proc lchop {list n} {
    set result {}

    while { [llength $list] >= $n } {
	lappend result [ltake $list $n]
	set list [ldrop $list $n]
    }
    return $result
}


## lcopy n item
#
# Make a list containing n copies of the specified item
#
proc lcopy {n item} {
    set result {}
    while {$n > 0} {
	lappend result $item
	incr n -1
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


## ldrop list n
#
# Drop _n_ list elements.
#
proc ldrop {list n} {
    return [lrange $list $n end]
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
    } elseif { $index == 0 } {
	return $list
    } else {
	return [lrange $list $index end]
    }
}


## lequal list1 list2
#
# Test if two lists are equal. This tests individual elements
# of the list, rather than just testing the two strings representing
# the lists. To see what I mean, note that
#
#     puts [expr {{1 2} == { 1 2}}]
#
# returns 0, whereas
#
#     lequal {1 2} { 1 2}
#
# returns 1.
#
proc lequal {list1 list2} {
    if { [llength $list1] != [llength $list2] } {
	return 0
    }

    set i 0
    foreach x $list1 {
	if { $x != [lindex $list2 $i] } {
	    return 0
	}

	incr i
    }
    return 1
}


## lfoldl init list script
#
# Fold a list from left to right.
#
proc lfoldl {init list script} {
    set result $init
    foreach x $list {
	set result [uplevel apply [list $script] [list $result] [list $x]]
    }
    return $result
}


## lfoldr init list script
#
# Fold a list from right to left.
#
proc lfoldr {init list script} {
    set result $init
    foreach x [lreverse $list] {
	set result [uplevel apply [list $script] [list $result] [list $x]]
    }
    return $result
}


## lgenerate init length script
#
# Generate a list by repeated applying a function to its previous
# result.
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


## lhead list
#
# Return the first item in the list, or the item itself if it is not a 
# list.
# 
# Return: Return the first item in the list, or the item itself if it is not a 
# list.
#
proc lhead {list} {
    return [lindex $list 0]
}


## linit list
#
# Take everything but the last element of a list.
# Note: linit{} requires that the list contain at least one
# element. Use lnull{} to check this before calling if you're
# not sure that this is the case. _This is a deliberate choice,
# not a bug._
#
proc linit {list} {
    return [lreplace $list end end]
}


## lintersection l1 l2
#
# Return the intersection of two lists.
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


## llast list
#
# Take the last element of a list.
#
proc llast {list} {
    return [lindex $list end]
}


## lmap
#
# Apply a function-script to every element of a list. The function-
# script is evaluated in the caller's context, so "free" variables
# are evaluated correctly.
#
# OBSOLETE: The multi-argument lmap is nearly as fast anyway.
#
# proc lmap {list script} {
#     set result {}
#
#     foreach x $list {
# 	lappend result [uplevel apply [list $script] [list $x]]
#     }
#     return $result
# }


## lmap
#
# Apply a function-script to every element of a list. The function-
# script is evaluated in the caller's context, so "free" variables
# are evaluated correctly.
#
# This code is a nigh-mare, and there _must_ be a simpler way
# of doing it. Tcl's schizophrenic treatment of lists, strings,
# and commands has almost made ME schizoid, so let's just be
# glad the damn thing works! (If you know a better way, please
# tell me!)
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


## lmember list item
#
# Test whether an item is in a list
#
proc lmember {list item} {
    return [expr [lsearch -exact $list $item] != -1]
}


## lnub list
#
# Remove duplicates from a list
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


## lorder list order
#
# Order elements of a list in the same way as elements of another.
# For example:
#
#     lorder {5 2 8 6} {1 2 3 4 5 6 7 8 9}
#
# returns {2 5 6 8}.
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


## lpairs list
#
# Split a list into a list of pairs, each pair containing two
# consecutive elements of the input list. If the list contains
# an odd number of elements, the last element is discarded.
# For example,
#
#     lpairs {1 2 3 4 5 6 7}
#
# returns {{1 2} {3 4} {5 6}}.
#
# The test for odd-length list ensures that a final odd element
# is not included in the result.
#
proc lpairs {list} {
    if { [expr [llength $list]] % 2 != 0 } {
	set list [lreplace $list end end]
    }

    set result {}
    foreach {a b} $list {
	lappend result [list $a $b]
    }
    return $result
}
    

## lreduce
#
# Reduce or ``fold'' a list into a single value. The list must
# contain at least one element.
#
proc lreduce {list script} {
    set result [lhead $list]
    set list   [ltail $list]

    while { ! [lnull $list] } {
	set result [uplevel apply [list $script] [list $result] \
		[list [list [lhead $list]]]]
	set list [ltail $list]
    }
    return $result
}


## lreject
#
# Filter a list based on a predicate. The output list contains
# elements of the input list for which the predicate fails.
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


## lreverse list
#
# Reverse a list.
#
proc lreverse {list} {
    set result {}
    foreach i $list {
	set result [linsert $result 0 $i]
    }
    return $result
}


## lscanl init list script
#
# Scan across a list from left to right, producing accumulated results.
#
proc lscanl {init list script} {
    set result [list $init]
    foreach x $list {
	set init [uplevel apply [list $script] [list $init] [list $x]]
	lappend result $init
    }
    return $result
}


## lscanr init list script
#
# Scan across a list from right to left.
#
proc lscanr {init list script} {
    set result [list $init]
    foreach x [lreverse $list] {
	set init [uplevel apply [list $script] [list $init] [list $x]]
	lappend result $init
    }
    return [lreverse $result]
}


## lselect
#
# Filter a list based on a predicate. The output list contains
# elements of the input list for which the predicate is satisfied.
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


## lsplit list n
#
# Split a list at the specified index. This is equivalent to
# performing a take{} and a drop{}. For example,
#
#     lsplit {1 2 3 4} 2
#
# returns {{1 2} {3 4}}.
#
# Note: You cannot reliably use lsplit{} to split a list into
# its head and tail. *lsplit {1 2 3 4} 1* works, because the
# elements are not lists, but *lsplit {{1 2} {3 4}} 1* is not
# the same as *[list [lhead $l] [ltail $l]]* where
# l = {{1 2} {3 4}}. Use behead{}.
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


## lsubsets list
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


## lsubtract l1 l2
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


## ltail list
#
# Return a list composed of all elements in the supplied list, except the 
# first item. 
# 
# Return: Return a list composed of all elements in the supplied list, 
# except the first item.       
#
# Note: ltail{} requires that the list contain at least one
# element. Use lnull{} to check this before calling if you're
# not sure that this is the case. _This is a deliberate choice,
# not a bug_.
#
proc ltail {list} {
    return [lreplace $list 0 0]
}


## ltake list n
#
# Take _n_ list elements.
#
proc ltake {list n} {
    return [lrange $list 0 [expr $n - 1]]
}


## ltranspose listlist
#
# Transpose a list of lists. All sub-lists must be the same length.
#
#     ltranspose {{1 2 3} {5 6 7}}
#
# returns {{1 5} {2 6} {3 7}}.
#
# Caveats: this procedure may behave a little strangely if
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


## lunion l1 l2
#
# Return the union of two lists. If the two lists are not
# proper sets, the union is anyway.
#
proc lunion {l1 l2} {
    return [lnub [concat $l1 $l2]]
}


