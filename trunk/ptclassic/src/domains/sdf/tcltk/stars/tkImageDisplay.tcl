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
#
# This is the Tcl/Tk script used as the default for the SDFTkImageDisplay
# star.  It creates a Tcl/Tk name for the image as ptkImageData_$starID.
# It also assumes that "ptimage" has been installed as an Tk image type
# so that the Tcl/Tk command "image create ptimage" works.
#
# Author: Mei Xiao and Brian L. Evans
# Version: $Id$

# Define the window to hold the image
set s $ptkControlPanel.image_$starID

# If a window with the right name already exists, we assume it was
# created by a previous run of the very same star, and hence can be
# used for this new run.  Some trickiness occurs, however, because
# parameter values may have changed, including the number of inputs.

if {[winfo exists $s]} {
    set window_previously_existed 1
} {
    set window_previously_existed 0
}

if {!$window_previously_existed} {
    toplevel $s
    wm title $s "Tk Image Display"
    wm iconname $s "PtTkImage"
    frame $s.f
    message $s.msg -width 12c
    pack append $s $s.msg {top expand} $s.f top

    proc ptkImageName {starID} {
	return ptkImageData_$starID
    }

    proc ptkImageWindow {dispWin imageName} {
	set dispB $dispWin.dispButton
	set saveB $dispWin.saveButton
	set dismissB $dispWin.dismissButton
	button $dispB -image $imageName
	button $saveB -text Save \
		-command "destroy $saveButton; ptkImageShow $dispWin"
	button $dismissB -text "DISMISS" \
		-command "ptkStop [curuniverse]; destroy $s"
	pack $dispB -side top -fill both
	pack $saveB -side top -fill x 
	pack $dismissB -side bottom -fill x 
    }

    proc ptkImageShow {dispWin} {
	global ptkImage_FileName
	frame $dispWin.fm
	label $dispWin.fm.l -text "File Name:"
	entry $dispWin.fm.e -width 20 -relief sunken -bd 2 \
		-textvariable ptkImage_FileName 
	button $dispWin.fm.ok -text "OK" \
		-command "destroy $dispWin.fm; ptkImageSave $dispWin"
	pack $dispWin.fm -side top -fill x 
	pack $dispWin.fm.l $dispWin.fm.e $dispWin.fm.ok -side left 
	pack $dispWin.fm.ok -side left -expand y -fill x
    }

    proc ptkImageSave {dispWin} {
	global ptkImage_FileName
	$dispWin write $ptkImage_FileName 
	button $dispWin.saveButton -text Save \
		-command "destroy $dispWin.saveButton; ptkImageShow $dispWin"
	pack $dispWin.saveButton -side top -fill x 
    }

    proc goTcl_$starID { starID } {
	set s $ptkControlPanel.image_$starID
	set i [ptkImageName $starID]
	$s.dispButton configure -image $i
    }

    # Allocate space for an image
    set i [ptkImageName $starID]
    if { ! [info exists $i] } {
      image create ptimage $i
    }

    ptkImageWindow $s $i
}
