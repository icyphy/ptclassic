#  Tcl interface for creating a picture display utility
# 
#  Author: Luis Gutierrez
#  @(#)ptkPicture.tcl	1.1 06/25/97
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

proc ptkCreatePicture { w height width palette title geo univ } {
    catch {destroy $w }
    toplevel $w
    wm minsize $w 300 300
    wm title $w $title
    wm iconname $w "TkPicture"
    wm geometry $w $geo
    scale $w.scalex -from -3 -to 3 -variable fx$w -orient horizontal \
	-tickinterval 1 -command scalex$w  -showvalue 0 \
	-relief groove
    scale $w.scaley -from 3 -to -3 -variable fy$w -orient vertical \
	-tickinterval 1 -command scaley$w  -showvalue 0 \
	-relief groove
    pack [button $w.quit -text DISMISS\
	    -command "ptkStop $univ; image delete $w.pic; \
	              unset fy$w fx$w; destroy $w"] \
	    -side bottom -fill x
    pack $w.scalex -fill x
    pack $w.scaley -fill y -side left
    pack [canvas $w.c ] -expand true
    image create photo $w.pic -palette $palette 
    $w.c create image 0 0 -image $w.pic -anchor nw
}
    
