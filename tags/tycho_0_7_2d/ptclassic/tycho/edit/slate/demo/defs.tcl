# Utility procedures for Slate demos
#
# @Copyright (c) 1996- The Regents of the University of California.
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
# @(#)defs.tcl	1.1 07/29/98

proc ::demo::newslate {title args} {
    # If running TYCHO, use the Tycho gui package
    if { [::info exists ::TYCHO] } {
	set win [uplevel #0 ::tycho::Displayer [::tycho::autoName .demo]]
	$win configure -title $title
    } else {
	# Otherwide just create a plain Tk toplevel
	# FIXME: Add a close button
	set win [uplevel #0 toplevel [::tycho::autoName .demo]]
	wm title $win $title
    }
    # Create the slate and pack it
    uplevel #0 ::tycho::slate $win.slate $args
    pack $win.slate -fill both -expand on

    # Create a quit button if not in Tycho
    if ![::info exists ::TYCHO] {
	uplevel #0 frame $win.bar
	uplevel #0 button $win.bar.quit -text Quit -command exit
	pack $win.bar.quit -side right -padx 2 -pady 2
	pack $win.bar -side bottom -fill x -expand on
    }

    # Always create a follower and make items tagged "moveable"
    # respond to the mouse
    set follower [$win.slate interactor Follower]
    $follower bind moveable -button 1

    # Display the window and return
    if { [package provide tycho.kernel.gui] != "" } {
	$win centerOnScreen
    } else {
	;# already displayed
    }
    return $win.slate
}
