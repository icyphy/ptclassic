##########################################################################
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


##########################################################################
#### capitalize
#
# Capitalize a word -- first letter is uppercase; rest are lowercase.
#
proc ::tycho::capitalize {word} {
    return [string toupper \
	    [string index $word 0]][string tolower [string range $word 1 end]]
}

##########################################################################
#### stringSubtract
#
# Subtract one string from another. No change if no match.
# The -all option causes all such strings to be replaced.
#
# (Similar to regsub, but for literals.)
#
proc ::tycho::stringSubtract {args} {
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

##########################################################################
#### wordleft
#
# Return the index of the start of the word immediately to the
# left of the cursor. The behaviour is more useful than "string
# wordstart" for cursor movement operations.
#
proc ::tycho::wordleft {string index} {
    if { $index <= 0 } {
	return 0
    }

    # Move left one character
    incr index -1

    # Move left until past space
    while { [string match "\[ \t\n\]" [string index $string $index]] } {
	if { $index == 0 } {
	    return 0
	}
	incr index -1
    }

    # Move left until space is reached
    while { ! [string match "\[ \t\n\]" [string index $string $index]] } {
	if { $index == 0 } {
	    return 0
	}
	incr index -1
    }
    incr index 1

    return $index
}

##########################################################################
#### wordright
#
# Return the index of the start of the word immediately to the
# rightof the cursor. The behaviour is more useful than "string
# wordend" for cursor movement operations.
#
proc ::tycho::wordright {string index} {
    set length [string length $string]
    if { $index > $length } {
	return $length
    }

    # Move right until in space
    while { ! [string match "\[ \t\n\]" [string index $string $index]] } {
	if { $index == $length } {
	    return $length
	}
	incr index 1
    }

    # Move right until past space
    while { [string match "\[ \t\n\]" [string index $string $index]] } {
	if { $index == $length } {
	    return $length
	}
	incr index 1
    }

    return $index
}

