#
# mapping.tcl
#
# Utility functions for mapping between number ranges, scaling
# intervals, and so on.
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


## spread n x y [-indented]
#
# Return list of n numbers in the range x to y. The -indented
# option causes the first and list numbers to be more or less
# the end points by half the interval between the reset of the
# elements. For example,
#
#     spread 4 1 5
#
# returns {1.0 2.33333 3.66666 4.99999}, and
#
#     spread 4 1 5 -indented
#
# returns {1.5 2.5 3.5 4.5}. Note that there may be some rounding
# errors, as in the first example above.
#
proc spread {n x y args} {
    getflag indented args

    set result {}
    set i      0

    if { $indented } {
	set delta  [expr (double($y) - $x) / $n]
	set x [expr $x + $delta / 2]
    } else {
	set delta  [expr (double($y) - $x) / ($n - 1)]
    }

    while { $i < $n } {
	lappend result [expr $x + $i * $delta]
	incr i +1
    }

    return $result
}


#
# Given a number, round up to the nearest power of ten
# times 1, 2, or 5.
#
proc axisRoundUp {x} {
    set exp [expr floor (log10($x))]
    set x [expr $x * pow(10, -$exp)]

    if {$x > 5.0} {
	set x 10.0
    } elseif {$x > 2.0} {
	set x 5.0
    } elseif {$x > 1.0 } {
	set x 2.0
    }

    set x [expr $x * pow(10,$exp)]
    return $x
}


#
# Given a number, round up or down to the nearest power of two.
#
proc roundUpTwo {x} {
    set exp [expr ceil (log($x)/log(2))]
    set x   [expr pow(2,$exp)]
}

proc roundDownTwo {x} {
    set exp [expr floor (log($x)/log(2))]
    set x   [expr pow(2,$exp)]
}


#
# Given a range, space, field width, and padding, figure out how
# the field increment so they will fit.
#
proc axisIncrement {low high space width padding} {
    set maxnum   [expr double($space) / ($width+$padding)]
    set estimate [expr (double($high) - $low) / ($maxnum)]
    set estimate [axisRoundUp $estimate]

    return $estimate
}


#
# Given a range and an increment, return the list of numbers
# within that range and on that increment.
#
proc rangeValues {low high inc} {
    set result {}
    set n      1

    set val [roundUpTo $low $inc]
    while {$val <= $high} {
	lappend result $val
	set val [expr $val + $n * $inc]
    }

    return $result
}


#
# Given two ranges and a list of numbers in the first range,
# produce the mapping of that list to the second range.
#
proc mapRange {low high values lowdash highdash} {
    set result {}

    set scale [expr (double($highdash) - $lowdash) / ($high - $low)]
    foreach n $values {
	lappend result [expr $lowdash + ($n-$low) * $scale]
    }

    return $result
}


#
# Given two ranges and a number in the first range,
# produce the mapping of that number to the second range.
#
proc mapValue {low high value lowdash highdash} {
    set scale [expr (double($highdash) - $lowdash) / ($high - $low)]
    return [expr $lowdash + ($value-$low) * $scale]
}


#
# Given two numbers, round the first up or down to the
# nearest multiple of the second.
#
proc roundDownTo {x i} {
    return [expr $i * (floor(double($x)/$i))]
}

proc roundUpTo {x i} {
    return [expr $i * (ceil(double($x)/$i))]
}


