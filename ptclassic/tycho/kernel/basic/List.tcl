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
# languages.
#


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


## llast, linit list
#
# Take the last element of a list, end everything but the last
# element.
#
# Note: linit{} requires that the list contain at least one
# element. Use lnull{} to check this before calling if you're
# not sure that this is the case. _This is a deliberate choice,
# not a bug._
#
proc llast {list} {
    return [lindex $list end]
}

proc linit {list} {
    return [lreplace $list end end]
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


## ltake, ldrop list n
#
# Take or drop list elements.
#
proc ltake {list n} {
    return [lrange $list 0 [expr $n - 1]]
}

proc ldrop {list n} {
    return [lrange $list $n end]
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


## lreverse list
#
# Reverse a list.
#
proc lreverse {list} {
    set result {}
    foreach i $list {
	set result [concat [list $i] $result]
    }
    return $result
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
# The test for odd-length list is an optimization: it allows the
# loop body to be coded in a slightly faster manner.
#
proc lpairs {list} {
    if { [expr [llength $list]] % 2 != 0 } {
	set list [lreplace $list end end]
    }

    set result {}
    while { $list != "" } {
	lappend result [list [lindex $list 0] [lindex $list 1]]
	set list [lreplace $list 0 1]
    }
    return $result
}


## zip list list [{list}]
#
# ``Zip'' two or more lists into a list of sub-lists. If the lists
# are not all the same length, then the returned list is only as
# long as the shortest list. For example,
#
#     zip {1 2 3 4} {5 6 7}
#
# returns {{1 5} {2 6} {3 7}}.
#
# Note that zip is almost its own inverse:
#
#     zip {1 5} {2 6} {3 7}
#
# returns {{1 2 3} {5 6 7}}. Use ltranspose{} for proper inverses.
#
proc zip {l1 l2 args} {

    set result {}
    while { $l1 != "" && $l2 != "" } {
	lappend result [list [lindex $l1 0] [lindex $l2 0]]
	
	set l1 [lreplace $l1 0 0]
	set l2 [lreplace $l2 0 0]
    }

    while { $args != "" } {
	behead list args

	set temp {}
	while { $result != "" && $list != "" } {
	    lappend temp [concat [lindex $result 0] [list [lindex $list 0]]]

	    set result [lreplace $result 0 0]
	    set list   [lreplace $list   0 0]
	}
	set result $temp
    }
    return $result
}


## ltranspose listlist
#
# Transpose a list of lists. If each sub-list is not the same length,
# then the result list is only as long as the shortest sub-list.
#
#     ltranspose {{1 2 3 4} {5 6 7}}
#
# returns {{1 5} {2 6} {3 7}}.
#
# Caveats: this procedure may behave a little strangely if
# given a list with only one sub-list, or a list with no
# sub-lists (as in {{1 2 3}} or {1 2 3}).
#
proc ltranspose {listlist} {

    set result [lcopy [llength [lindex $listlist 0]] {}]

    while { $listlist != "" } {
	behead list listlist

	set temp {}
	while { $result != "" && $list != "" } {
	    lappend temp [concat [lindex $result 0] [list [lindex $list 0]]]

	    set result [lreplace $result 0 0]
	    set list   [lreplace $list   0 0]
	}
	set result $temp
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


##########################################################################
#
# Sorting and set-related functions.
#
# These functions are more complex list operations, such as functions
# to sort lists, and functions for using lists as sets.
#


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
	if { [lsearch -exact $result $i] == -1 } {
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


## lsubtract l2
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



## lunion l1 l2
#
# Return the union of two lists. If the two lists are not
# proper sets, the union is anyway.
#
proc lunion {l1 l2} {
    return [lnub [concat $l1 $l2]]
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
# Association list functions.
#
# An association list is normally (in say Lisp and functional languages)
# as a list of pairs. Because this is Tcl, however, I'm implementing
# them as a list of lists, where the first element of each list is
# the element name and the rest of them are the element value. (In
# other words, if there is more than one, the value is a list.)
#


## assocSplit
#
# ``Unzip'' a list of lists into an association list. This is used to
# make association lists. For example,
#
#     assocSplit {{a 1} {b 2 3}}
#
# returns {{a 1} {b {2 3}}. Note that if the list is already an
# association list, this function has no effect.
#
proc assocSplit {list} {
    apply sublist $list {
	behead item sublist
	list $item $sublist
    }
}


## assocAppend listname name value
#
# Add a new value to the association list. This is a ``destructive''
# update, like lappend. This function is straightforward, and this
# and the other assocation list functions guarantee that
#
#     lappend list {fred 42}
#
# will have the same effect as
#
#     assocAppend list fred 42
#
proc assocAppend {list name value} {
    upvar $list l
    lappend l [list $name $value]
}


## assocReplace listname name value
#
# Replace a value in an association list. This is a ``destructive''
# update. If no value with the given name exists, then it is added.
#
proc assocReplace {list name value} {
    upvar $list l

    if { $l != "" } {
	assign names values [ltranspose $l]
	set index [lsearch -exact $names $name]
	if { $index != -1 } {
	    set l [lreplace $l $index $index [list $name $value]]
	} else {
	    lappend l [list $name $value]
	}
    } else {
	lappend l [list $name $value]
    }
}


## assocRemove listname name
#
# Remove the value with the specified name from the association
# list. This is a destructive delete: that is, the list is modified
# ``in place.''
#
# (Is it such a good idea to do a destructive delete?)
#
proc assocRemove {listname name} {
    upvar $listname l

    assign names values [ltranspose $l]
    set index [lsearch -exact $names $name]
    if { $index != -1 } {
	set l [lreplace $l $index $index]
    }
}


## assocLookup list name
#
# Return the value associated with the given name. Returns the
# null string if there is none.
#
# Note: Unlike assocAppend{} and assocRemove{}, assocLookup{} expects
# the list itself as an argument, not its name.
#
# Known bugs: Well, not really a bug, but the fact that this function
# expects a list value whereas all the other association list functions
# expect a list name is confusing. But then again, this kind of
# inconsistency is nothing new to Tcl ...
#
proc assocLookup {list name} {
    assign names values [ltranspose $list]
    set index [lsearch -exact $names $name]
    if { $index != -1 } {
	return [lindex $values $index]
    } else {
	return ""
    }
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


