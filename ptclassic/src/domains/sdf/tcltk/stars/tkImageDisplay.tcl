#
# This is the Tcl/Tk script used as the default for the SDFTkImgDisplay star.
#
# Author: Mei Xiao
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

proc goTcl_$dispStarID {dispStarID frameID} {
    
    global ptkControlPanel
    set dispWin $ptkControlPanel.img$dispStarID$frameID

    if {![winfo exists $dispWin]} {

        catch { destroy $dispWin }
        toplevel $dispWin

        set dispB $dispWin.dispButton
	set saveB $dispWin.saveButton
	set dismissB $dispWin.dismissButton
        button $dispB -image Img_$dispStarID$frameID 
        button $saveB -text Save -command \
			      [list ShowEntry $saveB $dispStarID $frameID] 
        button $dismissB -text Dismiss -command \
				 [list DestroyWin $dispStarID $frameID] 

        pack $dispB -side top -fill both
        pack $saveB -side top -fill x 
        pack $dismissB -side bottom -fill x 

	set winTitle [format "%s%s" $dispStarID $frameID]
	wm title $dispWin $winTitle
	wm iconname $dispWin $winTitle 
    }
}

proc ShowEntry {sb dispStarID frameID} {

    global ptkControlPanel 
    global foolfname
    set dispWin $ptkControlPanel.img$dispStarID$frameID
    destroy $sb
    frame $dispWin.fm
    label $dispWin.fm.l -text "File Name:"
    entry $dispWin.fm.e -width 20 -relief sunken -bd 2 \
			               -textvariable foolfname 
    button $dispWin.fm.ok -text Ok -command \
			[list SaveImage $dispWin.fm $dispStarID $frameID] 
    pack $dispWin.fm -side top -fill x 
    pack $dispWin.fm.l $dispWin.fm.e $dispWin.fm.ok -side left 
    pack $dispWin.fm.ok -side left -expand y -fill x
}

proc SaveImage {fm dispStarID frameID} {

    global ptkControlPanel 
    global foolfname
    destroy $fm
    Img_$dispStarID$frameID write $foolfname 
    set dispWin $ptkControlPanel.img$dispStarID$frameID
    button $dispWin.saveButton -text Save -command \
		      [list ShowEntry $dispWin.saveButton $dispStarID $frameID]
    pack $dispWin.saveButton -side top -fill x 
}

proc DestroyWin {dispStarID frameID} {

    global ptkControlPanel
    global foolfname
    set foolfname ""
    destroy $ptkControlPanel.img$dispStarID$frameID
}
