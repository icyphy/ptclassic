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

set s $ptkControlPanel.digiWatch

if {![winfo exists $s]} {
    toplevel $s
    wm title $s "Digital Watch"
    wm iconname $s "Digital Watch"

    frame $s.left
    pack $s.left   -side left -fill both -expand 1
    frame $s.middle
    pack $s.middle -side left -fill both -expand 1
    frame $s.right
    pack $s.right  -side left -fill both -expand 1

}  else {
    wm deiconify $s
    raise $s
}

set lb $s.left.buttons
set rb $s.right.buttons
catch {destroy $lb}
catch {destroy $rb}

frame $lb
pack $lb -fill both -expand 1
frame $rb
pack $rb -fill both -expand 1

button $lb.upper -text "update" -bd 3 -command "setOutput_$starID 1 1"
button $lb.lower -text " mode " -bd 3 -command "setOutput_$starID 2 1"
pack $lb.upper -side top
pack $lb.lower -side bottom

button $rb.upper -text "select" -bd 3 -command "setOutput_$starID 3 1"
button $rb.lower -text "adjust" -bd 3 -command "setOutput_$starID 4 1"
pack $rb.upper -side top
pack $rb.lower -side bottom

tkwait visibility $lb
tkwait visibility $rb


