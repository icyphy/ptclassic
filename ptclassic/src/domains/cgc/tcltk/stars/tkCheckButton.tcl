# Tcl/Tk source for a check button.
# For use with the CGCTkCheckButton Star.
#
# Authors: Jose Luis Pino
# Version: @(#)tkCheckButton.tcl	1.1 2/15/96
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
# See the file $PTOLEMY/copyright for copyright notice,
# limitation of liability, and disclaimer of warranty provisions.
#
# See the documentation of the TclScript star for an explanation of
# how the Tcl/Ptolemy interface works.

proc ${uniqueSymbol}setupTcl {identifier value onVal offVal} {
    global uniqueSymbol

    set s .middle.check$uniqueSymbol

    frame $s -bd 3 -relief raised
    checkbutton $s.button -text $identifier -variable state$uniqueSymbol \
	    -command "${uniqueSymbol}setOutputs \[set state$uniqueSymbol\]" \
	    -onvalue $onVal -offvalue $offVal
    pack append $s $s.button {top fill}
    pack append .middle $s {top fill}
    if { $value == $onVal } {
	$s.button select
    }
}

proc ${uniqueSymbol}callTcl {} {
    global uniqueSymbol
    global $uniqueSymbol
    set value [set ${uniqueSymbol}(value)]
    set onVal [set ${uniqueSymbol}(onValue)]
    set offVal [set ${uniqueSymbol}(offValue)]
    if { $value == $onVal } {
	${uniqueSymbol}setOutputs $onVal
    } else {
	${uniqueSymbol}setOutputs $offVal
    }
}

${uniqueSymbol}setupTcl [set ${uniqueSymbol}(identifier)] \
	[set ${uniqueSymbol}(value)] \
	[set ${uniqueSymbol}(onValue)] \
	[set ${uniqueSymbol}(offValue)]





