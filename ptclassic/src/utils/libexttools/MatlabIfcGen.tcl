# -*- tcl -*-
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
#                                               PT_COPYRIGHT_VERSION_2
#                                               COPYRIGHTENDKEY
#
# MatlabIfcGen: This script generates much of the Matlab Interface
# code.  It reads the function declarations from MatlabIfcGen.dat,
# and it writes out versions of them to four different files.
#
# The function declarations should be in this form:
# return type
# name of function
# (argument list);
#
# Blank lines are ignored.  The number of non-blank lines should be a
# multiple of 3.  For example, if the interface were
#
# int foo ();
# void bar (int);
#
# then MatlabIfcGen.dat would contain:
#
# int
# foo
# ();
#
# void
# bar
# (int);
#
# Author: Guy Maor <maor@ece.utexas.edu>
# Date: 7/24/97
# Version: $Id$


# read the next function, skipping blank lines
proc nextFunction {datafile ret name parms} {
    upvar $datafile f $ret r $name n $parms p
    set i -1
    while { [gets $f d([incr i])] != -1 } {
	if { [set $d($i) [string trim $d($i)]] == "" } { incr i -1 }
	if {$i == 2} {
	    set r $d(0)
	    set n $d(1)
	    set p [string trimright $d(2) {;}]
	    break
	}
    }
    expr { ! [eof $f] }
}


proc generateAuto {} {
    set f  [open MatlabIfcGen.dat]
    set m1 [open MatlabIfc.h.Auto w]
    set m2 [open MatlabIfc.cc.Auto1 w]
    set m3 [open MatlabIfc.cc.Auto2 w]
    set m4 [open MatlabHook.cc.Auto w]

    while { [nextFunction f ret name parms] } {

	# to MatlabIfc.h: static pointer declaration, dummy function definition
	set rtn0 [expr {$ret == "void" ? "" : "return 0;"}]
	puts $m1 "static $ret (*$name)$parms;"
	puts $m1 "static $ret dum$name$parms { MatlabEngineError(); $rtn0 }"
	
	# to MatlabIfc.cc: static pointer definition
	puts $m2 "$ret (*MatlabIfc :: $name)$parms;"
	
	# to MatlabIfc.cc ctor: assign pointer to dummy
	puts $m3 "$name = dum$name;"

	# to MatlabHook.cc: assign pointer to matlab function
	puts $m4 "MatlabIfc :: $name = $name;";
    }
}


if {$tcl_interactive != 1} {
    generateAuto
}
