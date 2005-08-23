# Message routines for use with Ptolemy
# @(#)message.tcl	1.38	10/29/96
# Author: Edward A. Lee
#
# Copyright (c) 1990-1997 The Regents of the University of California.
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


###################################################################
# Global variables
#
# the global variable ptolemy is assumed to point to the home directory
# of the Ptolemy installation

# used to collect profile information before displaying:
set ptkProfileString ""

# increment on each use to get unique window names
set unique 0

###################################################################
# Destroy a window if it exists
proc ptkSafeDestroy {win} {if {[winfo exists $win]} {destroy $win}}

###################################################################
# Open a message window with the Ptolemy copyright notice
#
proc ptkDisplayCopyright {} {
    global ptolemy
    set fileID [open $ptolemy/copyright r]
    set text [read $fileID]
    ptkImportantMessage .copyright $text
    close $fileID
}

###################################################################
# Display the contents of the global string ptkDisplayProfile
#
proc ptkDisplayProfile {} {
    global ptkProfileString

    global unique
    set w .profileWindow$unique
    incr unique

    ptkSafeDestroy $w
    toplevel $w
    wm title $w "Profile"
    wm iconname $w "Profile"

    button $w.ok -text "OK <Return>" -command "ptkSafeDestroy $w"
    message $w.msg -width 25c -text $ptkProfileString -justify left
    pack append $w $w.msg {top fill expand} $w.ok {top fill expand}

    wm geometry $w +200+200
    tkwait visibility $w
    bind $w <Return> "ptkSafeDestroy $w"
    bind $w <Button> "ptkSafeDestroy $w"
}
