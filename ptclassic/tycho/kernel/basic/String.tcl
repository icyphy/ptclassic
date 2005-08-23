##########################################################################
#
# Author:  H. John Reekie
#
# Version: @(#)String.tcl	1.10    11/10/97
#
# Copyright (c) 1996-1998 The Regents of the University of California.
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
#### breakLines
#
# Break a string into lines. The first argument is the string to
# break. Optional arguments are:
# <dl>
# <dt><b>-length</b>
# <dd>The maximum line length. The default is 72.
#
# <dt><b>-substring</b>
# <dd>The substring to match to decide where to break lines.
# The default is a single space. The substring will be included
# in the line following the break, except that left-most white-space
# will be trimmed.
#
# <dt><b>-prefix</b>
# <dd>A prefix to insert at the start of each line. The default is null.
# 
# <dt><b>-indentprefix</b>
# <dd>A string to insert at the second and subsequent lines. The
# default is a string of eight spaces.
# 
# <dt><b>-continuation</b>
# <dd>A string to append at the end of broken lines. The default
# is a space followed by a backslash.
#
# </dl> 
#
proc ::tycho::breakLines {string args} {
    array set opts {
        -length 72
        -substring " "
        -prefix ""
        -indentprefix "        "
        -continuation " \\"
    }
    array set opts $args
    set prefix $opts(-prefix)
    set indentlength [string length $prefix]
    set stringlength [expr [string length $string] + $indentlength]
    set result ""
    set doneone 0
    set donereturn 0

    while { $stringlength > $opts(-length) } {
        # Strip leading whitespace (not newlines)
        if [regexp "(^\[ \t\]+)" $string init] {
            set t [string length $init]
            set string [string range $string $t end]
            incr stringlength -$t
        }
        set temp [string range $string 0 [expr $opts(-length) - $indentlength]]
        set index [string first "\n" $temp]
        if { $index != -1 } {
           # Got a carriage return
           if !$doneone {
               append result $prefix \
                       [string trimleft [string range $string 0 $index]]
           } else {
               if !$donereturn {
                   append result \
                           $opts(-continuation) \
                           "\n"
               }
               append result \
                       $prefix \
                       [string trimleft [string range $string 0 $index]]
           }
           set string [string range $string [expr $index+1] end]
           incr stringlength -$index
           incr stringlength -1
           set donereturn 1
           continue
       }
       set index [string last $opts(-substring) $temp]
       if { $index == -1 } {
	   # Didn't find a break-point: try searching forwards
	   set index [string first $opts(-substring) $string]
       }
       if { $index != -1 } {
          # Found a break-point
           if !$doneone {
               append result \
                       $prefix \
                       [string trimleft \
                       [string range $string 0 [expr $index-1]] " "]
               append prefix $opts(-indentprefix)
               incr indentlength [string length $opts(-indentprefix)]
               incr stringlength [string length $opts(-indentprefix)]
               set doneone 1
           } else {
               if !$donereturn {
                   append result \
                            $opts(-continuation) \
                            "\n"
                }
                append result \
                        $prefix \
                        [string trimleft \
                        [string range $string 0 [expr $index-1]] " "]
            }
            set string [string range $string $index end]
            incr stringlength -$index
            set donereturn 0
            continue
        }
        # Cannot find a break-point, so exit
        break
    }
    # Deal with the remaining text
    if !$doneone {
        append result \
                $prefix \
                [string trimleft $string]
    } else {
        append result \
                $opts(-continuation) \
                "\n" \
                $prefix \
                [string trimleft $string]
    }
    return $result
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
proc ::tycho::wordleft {string index {skip " \[\t\n\]"}} {
    set length [string length $string]
    if { $index <= 0 } {
	return 0
    } elseif {$index > $length} {
        set index $length
    }

    # only interested in stuff to the left
    set string [string range $string 0 $index]
  
    # skip non-word characters
    set buffer [string trimright $string $skip]

    if {$buffer == {}} {
        return 0
    }

    # skip alphanumeric characters
    set buffer [string trimright $buffer \
            "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"]

    return [string length $buffer]
}

##########################################################################
#### wordright
#
# Return the index after the end of the word immediately to the
# right of the cursor. The behaviour is more useful than "string
# wordend" for cursor movement operations.
#
proc ::tycho::wordright {string index {skip " \[\t\n\]"}} {
    set length [string length $string]
    if {$index >= $length} {
	return $length
    } elseif {$index <= 0} {
        set index 0
    }

    # only interested in stuff to the right
    set string [string range $string $index end]

    # skip non-word characters
    set buffer [string trimleft $string $skip]

    if {$buffer == {}} {
        return $length
    }

    # skip alphanumeric characters 
    set buffer [string trimleft $buffer \
            "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"]

    return [expr $length - [string length $buffer]]
}

