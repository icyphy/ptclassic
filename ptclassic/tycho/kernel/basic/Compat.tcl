# Procedures needed for compatibility between Tcl versions
#
# @Author: John Reekie
#
# @Version: $Id$
#
# @Copyright (c) 1995-1998 The Regents of the University of California.
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
#######################################################################

##########################################################################
#### namespaceEval
# Evaluate the argument in the given namespace. If there is no
# argument, make sure that the namespace exists. This is needed
# for backward compatibility with Itcl2.2.
#
proc namespaceEval {ns {script {}}} {
    global tcl_version
    if { $tcl_version >= 8.0 } {
	uplevel namespace eval $ns [list $script]
    } else {
	uplevel namespace $ns [list $script]
    }
}

##########################################################################
#### winfoMegawidget
# 
# Use this instead of "winfo megawidget."
#
proc winfoMegawidget {w} {
    global tcl_version
    if { $tcl_version >= 8.0 } {
	while {$w != ""} {
	    if {![catch {$w isa Object}]} {
		return $w
	    } else {
		set w [winfo parent $w]
	    }
	}
	return $w
    } else {
	winfo megawidget $w
    }
}
