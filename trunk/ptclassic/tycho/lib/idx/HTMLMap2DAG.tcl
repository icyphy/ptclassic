# Convert htmlchek MAP files to DAGs
#
# @Author: Christopher Hylands
#
# @Version: $Id$
#
# @Copyright (c) 1996-%Q% The Regents of the University of California.
# All rights reserved.
#
# Permission is hereby granted, without written agreement and without
# license or royalty fees, to use, copy, modify, and distribute this
# software and its documentation for any purpose, provided that the above
# copyright notice and the following two paragraphs appear in all copies
# of this software.
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
#                                        PT_COPYRIGHT_VERSION_2
#                                        COPYRIGHTENDKEY
#######################################################################

#####################################################################
#### HTMLMap2DAG
# Convert a htmlchek .MAP file to a DAG
#
proc HTMLMap2DAG {mapfile} {
    global TYCHO
    if ![file readable $mapfile] {
        error "$mapfile does not exist or is not readable"
    }
    set mapfd [open $mapfile r]


    set web [::tycho::autoName web]
    ::tycho::Forest $web

    while {![eof $mapfd]} {
        set line [gets $mapfd]
        if {[string length $line] > 0} {
            if [regexp {[ \t]*File (.*) references:$} $line dummy srcfile] {
                 if ![info exists sawNode($srcfile)] {
                    set sawNode($srcfile) 1
                    # Strip out $TYCHO
                    set label $srcfile
                    regexp "$TYCHO/(.*)" $srcfile dummy label
                    $web add $srcfile [list label $label]
                }
            } else {
		set dstfile [string trim $line]
                if ![info exists sawNode($dstfile)] {
                    set sawNode($dstfile) 1
                    # Strip out $TYCHO
                    set label $dstfile
                    regexp "$TYCHO/(.*)" $dstfile dummy label

                    $web add $dstfile \
                            [list label $label \
                            link [list $dstfile {}]] $srcfile
                }
            }
        }
    }
    close $mapfd
    ::tycho::view EditForest -graph $web

    
}

HTMLMap2DAG $TYCHO/htmlchekout.MAP
