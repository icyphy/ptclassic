#
# string.tcl
#
# Utility functions on strings
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


#
# Join arguments into a string without intervening spaces
#
proc butt {args} {
    return [join $args ""]
}


#
# Generate a string of n spaces
#
proc spaces {n} {
    return [butt [lcopy $n " "]]
}


#
# Strip an itcl namespace path of all by the last name
#
proc nsLastField {nsp} {
   set index [string last "::" $nsp]
   if {$index == -1} { 
       return $nsp
   } else {
       return [string range $nsp [expr $index+2] end]
   }
}


#
# Subtract one string from another. No change if no match.
# The -all option causes all such strings to be replaced.
#
# (Similar to regsub, but for literals.)
#
proc ssubtract {args} {
    getflag all args
    assign a b $args

    set index [string first $b $a]
    if {$index == -1} {
	return $a
    } else {
	set result [string range $a 0 [expr $index-1]]
	append result [string range $a [expr $index+[string length $b]] end]

	if { $all } {
	    return [ssubtract -all $result $b]
	} else {
	    return $result
	}
    }
}
