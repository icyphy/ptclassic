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


proc lhead {list} {
    return [lindex $list 0]
}

proc ltail {list} {
    return [lreplace $list 0 0]
}

proc linit {list} {
    return [ltake $list [expr [llength $list] - 1]]
}

proc llast {list} {
    return [lindex $list [expr [llength $list] - 1]]
}

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


#
# Take or drop list elements
#
proc ltake {list n} {
    return [lrange $list 0 [expr $n - 1]]
}

proc ldrop {list n} {
    return [lreplace $list 0 [expr $n-1]]
}

proc ldropUntil {list item} {
    set index [lsearch -exact $list $item]
    if { $index == -1 } {
	return {}
    } else {
	return [ldrop $list $index]
    }
}

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


#
# Return list of n integers in the range x to y
#
proc interval {x y} {
    set result {}

    while { $x <= $y } {
	lappend result $x
	incr x +1
    }

    return $result
}


#
# Return list of n numbers in the range x to y
#
proc range {n x y} {
    set result {}
    set i      0
    set delta  [expr (double($y) - $x) / ($n - 1)]

    while { $i < $n } {
	lappend result [expr $x + $i * $delta]

	incr i +1
    }

    return $result
}


#
# Return list of n numbers in the range x to y, but with
# half the interval before the first and last numbers.
#
# This is useful for spacing graphical elements "evenly" along
# a given distance.
#
proc spread {n x y} {
    set result {}
    set i      0
    set delta  [expr (double($y) - $x) / $n]

    set x [expr $x + $delta / 2]
    while { $i < $n } {
	lappend result [expr $x + $i * $delta]

	incr i +1
    }

    return $result
}



#
# List distributions: like in Backus' FP
#
proc ldistl {item list} {
    set result {}

    foreach i $list {
	lappend result [list $item $i]
    }

    return $result
}

proc ldistr {list item} {
    set result {}

    foreach i $list {
	lappend result [list $i $item]
    }

    return $result
}

#
# Test whether an element is in a list
#
proc lmember {list item} {
    return [expr [lsearch -exact $list $item] != -1]
}


#
# Order elements of a list in the same way as elements of another
#
proc cp {a b} {
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
    set nlist [lsort -command cp $nlist]
    foreach item $nlist {
	lappend list [lindex $item 1]
    }

    return $list
}


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

#
# Generate all non-empty subsets of a list
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


#
# Apply a function to every element of a list
#
proc lmap {list cmd} {
    set result {}

    foreach x $list {
	lappend result [$cmd $x]
    }
    return $result
}


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


#
# Difference of two lists: l1 - l2
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

#
# Is l1 a subset of l2?
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


#
# Are l1 and l2 disjoint?
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
