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

set buttons $s.buttons
catch {destroy $buttons}

frame $buttons -relief groove -bd 3
pack $buttons -padx 3 -pady 3 -expand 1 -fill both

label $buttons.label -text "You can push:"
pack $buttons.label -side top
button $buttons.coin -text Coin -bd 4 -padx 3 -pady 3\
	-command "setOutput_$starID 1 1"
button $buttons.ready -text Ready -bd 4 -padx 3 -pady 3\
	-command "setOutput_$starID 2 1"
button $buttons.stop -text Stop -bd 4 -padx 3 -pady 3\
	-command "setOutput_$starID 3 1"
pack $buttons.coin $buttons.ready $buttons.stop -side left\
	-expand 1 -fill both -padx 3 -pady 3 

tkwait visibility $buttons

