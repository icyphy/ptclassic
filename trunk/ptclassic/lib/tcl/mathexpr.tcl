# Copyright (c) 1990-%Q% The Regents of the University of California.
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
# This file defines several math and list functions useful in specifying
# state and parameter values for ptcl, pigi, and tycho.  The functions
# help avoid the use of global variables.  For example, the Tcl code
# to compute the filter taps for an FIR filter whose impulse response of
# a sinusoidal function is
#
# set taps {}
# for {set i 1} {$i <= 100} {incr i} {
#   lappend taps [expr cos(2*{PI}*($i-1)/100)]
# }
# join $taps
#
# becomes
#
# sampleExpr {cos(2*{PI}*($i-1)/100)} 1 100
#
# Here, PI is a parameter of the Ptolemy system which is substituted
# before the Tcl command is passed to the Tcl interpreter.
#
# Author: Brian L. Evans
# $Id$

# If add_to_help has not been defined as a procedure, then define a dummy one
if { [info commands add_to_help] == "" } {
  proc add_to_help {cmd argl desc} {}
}

# Function: listSampleExpr

add_to_help listSampleExpr {<tclexpr> <list>} {
Produces a new list by applying each item in list to the tclexpr
by substituting for the variable i in tclexpr.  The tclexpr should
be in an unevaluated form, e.g. { cos($i) }.
}

proc listSampleExpr {tclexpr thelist} {
  foreach i $thelist {
    lappend newlist [expr $tclexpr]
  }
  return $newlist
}

# Function: max

add_to_help max {<x1> <x2> ...} {
Computes the maximum of the arguments according to the > operator.
}

proc max {a args} {
  set maxvalue $a
  foreach i $args {
    if { $i > $maxvalue } {
      set maxvalue $i
    }
  }
  return $maxvalue
}

# Function: min

add_to_help min {<x1> <x2> ...} {
Computes the minimum of the arguments according to the < operator,
which also compares strings.
}

proc min {a args} {
  set minvalue $a
  foreach i $args {
    if { $i < $minvalue } {
      set minvalue $i
    }
  }
  return $minvalue
}

# Function: range

add_to_help range {<min> <max> ?<increment>?} {
Produces an ordered list of numbers running from min to max at increments
of increment.  If the sequence of numbers will never reach max, then
the routine will return an empty list.
}

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

# Function: sampleExpr

add_to_help sampleExpr {<tclexpr> <min> <max> ?<increment>?} {
Evaluate tclexpr at values of i running from min to max at increments
of increment to produce a list.  If the sequence of numbers will never
reach max, then the routine will return an empty list.  The tclexpr should
be in an unevaluated form, e.g. { cos($i) }. 
}

proc sampleExpr {tclexpr minindex maxindex {inc 1}} {
  if { [sign [expr "$maxindex - $minindex"]] == [sign $inc] } {
    for {set i $minindex} {$i <= $maxindex} {incr i $inc} {
      lappend rangelist [expr $tclexpr]
    }
  } else {
    set rangelist {}
  }
  return $rangelist
}

# Function: sign

add_to_help sign {<x>} {
Returns 1 if x > 0, -1 if x < 0, and 0 otherwise.
The comparison is based on the Tcl operators < and >, which also
compare strings.
}

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
