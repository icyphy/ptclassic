# Utility functions for mapping between number ranges, scaling
# intervals, and so on.
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


#######################################################################
#### axisIncrement
#
# Given a range, space, field width, and padding, figure out how
# the field increment so they will fit.
#
proc ::tycho::axisIncrement {low high space width padding} {
    set maxnum   [expr double($space) / ($width+$padding)]
    set estimate [expr (double($high) - $low) / ($maxnum)]
    set estimate [axisRoundUp $estimate]

    return $estimate
}

#######################################################################
#### axisRoundUp
#
# Given a number, round up to the nearest power of ten
# times 1, 2, or 5.
#
# Note: The argument must be positive.
#
proc ::tycho::axisRoundUp {x} {
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

#######################################################################
#### mapRange
#
# Given two ranges and a list of numbers in the first range,
# produce the mapping of that list to the second range.
#
proc ::tycho::mapRange {low high values lowdash highdash} {
    set result {}

    set scale [expr (double($highdash) - $lowdash) / ($high - $low)]
    foreach n $values {
	lappend result [expr $lowdash + ($n-$low) * $scale]
    }

    return $result
}

#######################################################################
#### mapValue
#
# Given two ranges and a number in the first range,
# produce the mapping of that number to the second range.
#
proc ::tycho::mapValue {low high value lowdash highdash} {
    set scale [expr (double($highdash) - $lowdash) / ($high - $low)]
    return [expr $lowdash + ($value-$low) * $scale]
}

##########################################################################
##### max
# Return the largest of two numbers.
#
proc ::tycho::max {x y} {
    if { $x > $y } {
	set x
    } else {
	set y
    }
}

##########################################################################
##### maximum
# Return the largest of a list of numbers.
#
proc ::tycho::maximum {args} {
    lindex [lsort -real $args] end
}

##########################################################################
##### min 
# Return the smallest of two numbers.
#
proc ::tycho::min {x y} {
    if { $x < $y } {
	set x
    } else {
	set y
    }
}

##########################################################################
##### minimum
# Return the smallest of a list of numbers.
#
proc ::tycho::minimum {args} {
    lindex [lsort -real $args] 0
}

#######################################################################
#### rangeValues
#
# Given a range and an increment, return the list of numbers
# within that range and on that increment.
#
proc ::tycho::rangeValues {low high inc} {
    set result {}
    set n      1

    set val [roundUpTo $low $inc]
    while {$val <= $high} {
	lappend result $val
	set val [expr $val + $n * $inc]
    }

    return $result
}

#######################################################################
#### roundDownTwo
#
# Given a number, round down to the nearest power of two.
#
proc ::tycho::roundDownTwo {x} {
    set exp [expr floor (log($x)/log(2))]
    set x   [expr pow(2,$exp)]
    return $x
}

#######################################################################
#### roundDownTo
#
# Given two numbers, round down the first to the nearest multiple of
# the second.
#
proc ::tycho::roundDownTo {x i} {
    return [expr $i * int(floor(double($x)/$i))]
}

#######################################################################
#### roundTo
#
# Given two numbers, round the first to the nearest multiple of
# the second.
#
proc ::tycho::roundTo {x i} {
    return [expr $i * round(double($x)/$i)]
}

#######################################################################
#### roundUpTo
#
# Given two numbers, round up the first to the nearest multiple of
# the second.
#
proc ::tycho::roundUpTo {x i} {
    return [expr $i * int(ceil(double($x)/$i))]
}

#######################################################################
#### roundUpTwo
#
# Given a number, round up to the nearest power of two.
# The argument must be positive.
#
proc ::tycho::roundUpTwo {x} {
    set exp [expr ceil (log($x)/log(2))]
    set x   [expr pow(2,$exp)]
    return $x
}

#######################################################################
#### spread
#
# Return list of n numbers in the range x to y. The -indented
# option causes the first and list numbers to be more or less
# the end points by half the interval between the reset of the
# elements. For example,
# <pre>
#     spread 4 1 5
# </pre>
# returns <code>{1.0 2.33333 3.66666 4.99999}</code>, and
# <pre>
#     spread 4 1 5 -indented
# </pre>
# returns <code>{1.5 2.5 3.5 4.5}</code>. Note that there may
# be some rounding errors, as in the first example above.
#
proc ::tycho::spread {n x y args} {
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
