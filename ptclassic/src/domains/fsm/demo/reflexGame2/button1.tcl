# Tcl/Tk source for a panel of buttons
#
# Authors: Bilung Lee
# Version: %W% %G%
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

set s $ptkControlPanel.reflexGame

if {![winfo exists $s]} {
    toplevel $s
    wm title $s "Reflex Game"
    wm iconname $s "reflexGame"
} else {
    wm deiconify $s
    raise $s
}

set button1 $s.button1
catch {destroy $button1}

frame $button1 -relief groove -bd 3
pack $button1 -padx 3 -pady 3 -expand 1 -fill both

label $button1.label -text "Player1:"
pack $button1.label -side top
button $button1.coin -text coin -bd 4 -padx 3 -pady 3 -width 5\
	-command "setOutput_$starID 1 1"
button $button1.go -text go -bd 4 -padx 3 -pady 3  -width 5\
	-command "setOutput_$starID 2 1"
pack $button1.coin $button1.go -side left -padx 15 -pady 3 

tkwait visibility $button1

