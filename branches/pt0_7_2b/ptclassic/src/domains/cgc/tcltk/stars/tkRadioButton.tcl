# Tcl/Tk source for a telephone keypad, to be used with the TkSource star
# For use with a two-output, zero-input TclScript star.
#
# Authors: Jose Luis Pino
# Version: $Id$
#
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
# See the file $PTOLEMY/copyright for copyright notice,
# limitation of liability, and disclaimer of warranty provisions.
#
# See the documentation of the TclScript star for an explanation of
# how the Tcl/Ptolemy interface works.

set s .middle.radio$uniqueSymbol

frame $s -bd 3 -relief raised
pack append .middle $s {top fill}
label $s.lbl -text "WOW"
pack append $s $s.lbl {top fill}

set pairs "{One 1.0} {Two 2.0}"

foreach pair $pairs {
    set pairlbl [lindex $pair 0]
    radiobutton $s."p_$pairlbl" -text $pairlbl -val [lindex $pair 1] \
	    -var $s -com "${uniqueSymbol}setOutputs [lindex $pair 1]"
    pack append $s $s."p_$pairlbl" { top fill expand }
}

proc ${uniqueSymbol}callTcl {} {
    global uniqueSymbol
    global pairs
    ${uniqueSymbol}setOutputs [lindex [lindex $pairs 1] 1]
}
