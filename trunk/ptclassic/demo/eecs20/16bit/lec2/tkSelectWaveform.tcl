# Tcl/Tk source for a panel of buttons
#
# Authors: Edward A. Lee and Wan-Teh Chang
# Version: %W%	%G%
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
set s .buttonpad_$starID
catch {destroy $s}
toplevel $s
wm title $s "Selector"
wm iconname $s "Selector"

pack [frame $s.b -bd 10]

# Create the individual selector buttons.
set cnt 0
foreach button {triangle sawtooth rectangular} {
    set but $s.b.$button
    radiobutton $but -text $button -variable selector -value $cnt
    pack $but -fill x -expand yes -anchor w
    incr cnt
}

set selector 0

proc goTcl_$starID {starID} {
    global selector
    setOutputs_$starID $selector
}

# Remove the buttons
proc destructorTcl_$starID {starID} "
    destroy $s
"

unset s
