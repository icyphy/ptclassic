# Tcl/Tk source for a radio button widget.
# For use with the CGCTkRadioButton Star.
#
# Authors: Jose Luis Pino
# Version: @(#)tkRadioButton.tcl	1.5 2/28/96
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

proc ${uniqueSymbol}setupTcl {identifier initialChoice pairs} {
    global uniqueSymbol

    set s .middle.radio$uniqueSymbol

    frame $s -bd 3 -relief raised
    label $s.lbl -text $identifier
    pack append $s $s.lbl {top fill}

    frame $s.buttons

    foreach pair $pairs {
	set pairlbl [lindex $pair 0]
	set pairValue [lindex $pair 1]
	radiobutton $s.buttons."p_$pairlbl" -text $pairlbl -val \
		$pairValue -var $s \
		-com "${uniqueSymbol}setOutputs $pairValue"
	pack $s.buttons $s.buttons."p_$pairlbl" -side left
    }
    pack append $s $s.buttons {top fill}
    pack append .middle $s {top fill}

    $s.buttons."p_$initialChoice" select
}

proc ${uniqueSymbol}callTcl {} {
    global uniqueSymbol
    global $uniqueSymbol
    set initialChoice [set ${uniqueSymbol}(initialChoice)]
    set pairs [set ${uniqueSymbol}(pairs)]
    foreach pair $pairs {
	if { [lindex $pair 0] == $initialChoice } {
	    ${uniqueSymbol}setOutputs [lindex $pair 1]
	    break
	}
    }
}

${uniqueSymbol}setupTcl [set ${uniqueSymbol}(identifier)] \
	[set ${uniqueSymbol}(initialChoice)] \
	[set ${uniqueSymbol}(pairs)]





