# Copyright (c) 1990-1997 The Regents of the University of California.
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
#
# This file defines a set of generic math and list functions in Tcl.
# This file can be used by itself.
#
# As a part of ptcl, pigi, and tycho, these routines are useful in
# specifying state and parameter values.  The functions avoid using
# global variables.  For example, the Tcl code to compute the filter
# taps for an FIR filter whose impulse response is a sampled sinusoidal
# function specified as
#
# set taps {}
# for {set i 1} {$i <= 100} {incr i} {
#   lappend taps [expr cos(2*{PI}*($i-1)/100)]
# }
# join $taps
#
# becomes
#
# rangeApplyExpression {cos(2*{PI}*($i-1)/100)} 1 100
#
# Here, PI is a parameter of the Ptolemy system which is substituted
# before the Tcl command is passed to the Tcl interpreter.
#
# Author: Brian L. Evans
# @(#)mathexpr.tcl	1.13	01/22/97

# If add_to_help has not been defined as a procedure, then define a dummy one
#
# FIXME: Need to consolidate ptcl help functionality and Tycho
# automatic documentation generation.
#
if { [info commands add_to_help] == "" } {
  proc add_to_help {cmd argl desc} {}
}

#######################################################################
#### listApplyExpression
#
add_to_help listApplyExpression {<tclexpr> <list>} {
Produces a new list by applying each item in list to the tclexpr
by substituting for the variable i in tclexpr.  The tclexpr should
be in an unevaluated form, e.g. { cos($i) }.
}
#
proc listApplyExpression {tclexpr thelist} {
  foreach i $thelist {
    lappend newlist [expr $tclexpr]
  }
  return $newlist
}

#######################################################################
#### makeOrderedPairs
#
add_to_help makeOrderedPairs {<x-values> ?<y-values>?} {
Converts two lists, <x-values> and <y-values>, into ordered pairs; e.g.,
makeOrderedPairs "1 2 3" "3 2 1" returns "(1,3) (2,2) (3,1)".
If <y-values> is omitted, then <x-values> is returned.  This procedure
returns a string.
}
#
proc makeOrderedPairs {args} {
  set numargs [llength $args]
  if { $numargs == 2 } {
    set xvector [lindex $args 0]
    set yvector [lindex $args 1]
  } elseif { $numargs == 1 } {
    return [join $args]
  } else {
    set usagestr "makeOrderedPairs <x-values> ?y-values?"
    error "wrong # arguments: should be \"$usagestr\""
    return -code error
  }

  # Both x and y values were specified
  # Convert into ordered pairs: (x1, y1) (x2, y2) ...
  set numx [llength $xvector]
  set numy [llength $yvector]
  if { $numx == $numy } {
    set orderedpairs ""
    for {set i 0} {$i < $numx} {incr i} {
      set xvalue [lindex $xvector $i]
      set yvalue [lindex $yvector $i]
      lappend orderedpairs "($xvalue,$yvalue)"
    }
    return [join $orderedpairs]
  } else {
    set errmsg "# of x-values ($numx) different from # of y-values ($numy)"
    error "makeOrderedPairs: $errmsg"
    return -code error
  }
}

#######################################################################
#### max
#
add_to_help max {<x1> <x2> ...} {
Computes the maximum of the arguments according to the > operator,
which also compares strings.
}
#
proc max {a args} {
  set maxvalue $a
  foreach i $args {
    if { $i > $maxvalue } {
      set maxvalue $i
    }
  }
  return $maxvalue
}


#######################################################################
#### min
#
add_to_help min {<x1> <x2> ...} {
Computes the minimum of the arguments according to the < operator,
which also compares strings.
}
#
proc min {a args} {
  set minvalue $a
  foreach i $args {
    if { $i < $minvalue } {
      set minvalue $i
    }
  }
  return $minvalue
}


#######################################################################
#### range
#
add_to_help range {<min> <max> ?<increment>?} {
Produces an ordered list of numbers running from min to max at increments
of increment.  If the sequence of numbers will never reach max, then
the routine will return an empty list.
}
#
proc range {minindex maxindex {inc 1}} {
  if { [sign [expr "$maxindex - $minindex"]] == [sign $inc] } {
    for {set i $minindex} {$i <= $maxindex} {incr i $inc} {
      lappend rangelist $i
    }
  } else {
    set rangelist {}
  }
  return $rangelist
}


#######################################################################
#### rangeApplyExpression
#
add_to_help rangeApplyExpression {<tclexpr> <min> <max> ?<increment>?} {
Evaluate tclexpr at values of i running from min to max at increments
of increment to produce a list.  If the sequence of numbers will never
reach max, then the routine will return an empty list.  The tclexpr should
be in an unevaluated form, e.g. { cos($i) }. 
}
#
proc rangeApplyExpression {tclexpr minindex maxindex {inc 1}} {
  if { [sign [expr "$maxindex - $minindex"]] == [sign $inc] } {
    for {set i $minindex} {$i <= $maxindex} {incr i $inc} {
      lappend rangelist [expr $tclexpr]
    }
  } else {
    set rangelist {}
  }
  return $rangelist
}


#######################################################################
#### sign
#
add_to_help sign {<x>} {
Returns 1 if x > 0, -1 if x < 0, and 0 otherwise.
The comparison is based on the Tcl operators < and >, which also
compare strings.
}
#
proc sign {x} {
  if { $x < 0 } {
    set signum -1
  } elseif {$x > 0} {
    set signum 1
  } else {
    set signum 0
  }
  return $signum
}
