# Generate documentation for the pitcl interface.
#
# @Author: Edward A. Lee
#
# @Version: $Id$
#
# @Copyright (c) 1997 The Regents of the University of California.
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

# This is a very specialized script designed just to generate documentation
# from PITcl.cc for the Itcl commands defined in the ::pitcl namespace.

proc convertline {line} {
    # Remove comment characters
    regsub {^//$} $line {} line
    regsub {^// } $line {} line
    if {$line == {}} {
        return {<p>}
    } {
        if [string match {Usage:*} $line] {
            regsub -all {<} $line {<i>} line
            regsub -all {([^<][^i])>} $line {\1</i>} line
            regsub {Usage:} $line {<b>Usage:</b>} line
        }
        regsub -all {\*([^* ]+)\*} $line {<code>\1</code>} line
        regsub -all {(^| )_([^_ ]+)_( |$)} $line {<i>\2</i>} line
        return $line
    }
}

cd [file join $PTOLEMY src pitcl]
set ifd [open PITcl.cc]

# Place the output in the doc/codeDoc directory.
# Create the directory if it does not exist
if { [file  isdirectory [file join doc codeDoc]] != 1 } {
    file mkdir [file join doc codeDoc]
}

set ofd [open [file join doc codeDoc pitcl.html] w]

puts $ofd {<!-- Automatically generated from PITcl.cc -->}
puts $ofd {<html>}
puts $ofd {<head>}
puts $ofd {<title>pitcl interface to Ptolemy</title>}
puts $ofd {</head>}
puts $ofd {<body bgcolor="#faf0e6">}
puts $ofd {<h1>Pitcl interface to Ptolemy</h1>}
puts $ofd {<a name="pitcl interface to Ptolemy">}

set mode ignoretop
while {[gets $ifd line] >= 0} {
    switch $mode {
        ignoretop {
            if [string match {// NOTE: DO NOT REMOVE THIS LINE.  START OF OVERVIEW.} $line] {
                set mode overview
            }
        }
        overview {
            if [string match {// NOTE: DO NOT REMOVE THIS LINE.  END OF OVERVIEW.} $line] {
                set mode ignoremiddle
            } {
                puts $ofd [convertline $line]
            }
        }
        ignoremiddle {
            if [string match {// NOTE: DO NOT REMOVE THIS LINE.  START OF DOCUMENTATION SOURCE.} $line] {
                set mode docscan
            }
        }
        docscan {
            set mmode ignore
            while {[gets $ifd line] >= 0} {
                if [string match {// NOTE: DO NOT REMOVE THIS LINE.  END OF DOCUMENTATION SOURCE.} $line] {
                    set mode ignoreend
                    break
                }
                switch $mmode {
                    ignore {
                        if [string match {//// *} $line] {
                            set procname {}
                            scan $line {//// %s} procname
                            puts $ofd "<a name=\"$procname pitcl command\">"
                            puts $ofd "<h2>$procname</h2>"
                            puts $ofd "</a>"
                            set mmode procdoc
                        }
                    }
                    procdoc {
                        if [string match {//*} $line] {
                            puts $ofd [convertline $line]
                        } {
                            set mmode ignore
                        }
                    }
                }
            }
        }
    }
}


puts $ofd {</body>}
puts $ofd {</html>}
close $ofd
close $ifd
