##########################################################################
#
# Author:  H. John Reekie
#
# Version: $Id$
#
# Copyright (c) 1995-%Q% The Regents of the University of California.
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
proc ::tycho::ldelete {list item} {
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
proc ::tycho::ldisjoint {l1 l2} {
    set result {}

    foreach i $l1 {
	if {[lsearch -exact $l2 $i] != -1} {
	    return 0
	}
    }

    return 1
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
proc ::tycho::lintersection {l1 l2} {
    set result {}

    foreach i $l1 {
	if {[lsearch -exact $l2 $i] != -1} {
	    lappend result $i
	}
    }

    return $result
}

#######################################################################
#### linterval
#
# Return list of integers in the range _x_ to _y_. For example,
# <pre>
#    linterval 2 5
# </pre>
# returns <code>{2 3 4 5}</code>.
#
proc ::tycho::linterval {x y} {
    set result {}

    while { $x <= $y } {
	lappend result $x
	incr x +1
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
proc ::tycho::lmap {args} {
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
proc ::tycho::lmember {list item} {
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
# The implementation uses an array to remove duplicates, and
# is faster than a list-based implementation for all length lists.
#
proc ::tycho::lnub {list} {
    # Create an array indexed by elements of the list
    set l [llength $list]
    if { $l & 1 } {
	# The list is odd length
	array set temp [lreplace $list 0 0]
	lappend list 0
	array set temp $list
    } else {
	# The list is even length
	array set temp $list
	lappend list 0
	array set temp [lreplace $list 0 0]
    }

    # The result is the indexes of the array
    return [array names temp]
}

##########################################################################
#### lreverse list
#
# Reverse a list. Example:
# <pre><tcl>
#     lreverse {1 2 3 4 5}
# </tcl></pre>
#
proc ::tycho::lreverse {list} {
    set result {}
    foreach i $list {
	set result [linsert $result 0 $i]
    }
    return $result
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
proc ::tycho::lsubset {l1 l2} {
    set result {}

    foreach i $l1 {
	if {[lsearch -exact $l2 $i] == -1} {
	    return 0
	}
    }

    return 1
}

##########################################################################
#### lsubtract l1 l2
#
# Return the difference of two lists: _l1_ - _l2_. Example:
# <pre><tcl>
#     lsubtract {1 2 3 4 5} {2 4 6}
# </tcl></pre>
#
# This procedure is implemented using arrays, and on 100-elements lists
# is nearly three times faster than a simpler implementation using
# lists. However, on short lists (a few elements) it is slower, so
# the list version is still available as ldiff{}.
#
proc ::tycho::lsubtract {l1 l2} {
    # Optimize if they're the same
    if { $l1 == $l2 } {
	return ""
    }

    # Create an array indexed by elements of the first list
    set l [llength $l1]
    if { $l & 1 } {
	# The list is odd length
	array set temp [lreplace $l1 0 0]
	lappend l1 0
	array set temp $l1
    } else {
	# The list is even length
	array set temp $l1
	lappend l1 0
	array set temp [lreplace $l1 0 0]
    }

    # Remove each element in l2 from the array
    foreach i $l2 {
	catch {unset temp($i)}
    }

    # What's left is the result
    return [array names temp]
}

##########################################################################
#### ltake list n
#
# Take _n_ list elements. <i>This proc ::tycho::will be deleted
# soon -- do not use.</i>
#
proc ::tycho::ltake {list n} {
    return [lrange $list 0 [expr $n - 1]]
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
proc ::tycho::lunion {l1 l2} {
    return [lnub [concat $l1 $l2]]
}
