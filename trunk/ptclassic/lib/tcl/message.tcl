# Message routines for use with Ptolemy
# $Id$
# Author: Edward A. Lee
#
# Copyright (c) 1990-1993 The Regents of the University of California.
# All rights reserved.
# 
# Permission is hereby granted, without written agreement and without
# license or royalty fees, to use, copy, modify, and distribute this
# software and its documentation for any purpose, provided that the above
# copyright notice and the following two paragraphs appear in all copies
# of this software.
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
#                                                         COPYRIGHTENDKEY


###################################################################
# Global variables
#
# the global variable ptolemy is assumed to point to the home directory
# of the Ptolemy installation

# used to collect profile information before displaying:
set ptkProfileString ""

# flag whether Tcl errors should be poped up in windows
set REPORT_TCL_ERRORS 1

# increment on each use to get unique window names
set unique 0


###################################################################
# procedure to issue an error message from any internal tk error
#
proc tkerror message {
    ptkImportantMessage .error "Background error in Tk"
    global REPORT_TCL_ERRORS
    if {$REPORT_TCL_ERRORS == 1} {ptkImportantMessage .error $message}
}


###################################################################
# Procedure to open a message window and grab the focus.
# The user cannot do anything until the window is dismissed.
#
proc ptkImportantMessage {w text} {
    catch {destroy $w}
    toplevel $w
    wm title $w "Ptolemy Message"
    wm iconname $w "Ptolemy Message"

    button $w.ok -text "OK <Return>" -command "destroy $w"
    message $w.msg -font -Adobe-times-medium-r-normal--*-180* -width 25c \
            -text $text -justify center
    pack append $w $w.msg {top fill expand} $w.ok {top fill expand}

    wm geometry $w +200+200
    tkwait visibility $w
    focus $w
    bind $w <Key> "destroy $w"
    bind $w <Button> "destroy $w"
    bind $w.msg <Button> "destroy $w"
    grab $w
    tkwait window $w
}

###################################################################
# Open a message window with the Ptolemy bitmap and grab the focus.
# The user cannot do anything until the window is dismissed.
#
proc ptkStartupMessage {pigiVersion pigiFilename} {
    set w .ptkStartupMessage
    catch {destroy $w}
    toplevel $w
    wm title $w "Ptolemy Message"
    wm iconname $w "Ptolemy Message"

    button $w.ok -text "OK <Return>" -command "destroy $w"
    frame $w.f -relief raised -bd 5
    frame $w.f.msg
    message $w.f.msg.msg1 -font -Adobe-Helvetica-Bold-R-Normal-*-240-* \
	-justify center -foreground firebrick -width 20c -text { \
Ptolemy Interactive Graphical Interface }
    append text $pigiVersion {
} {
Ptolemy executable you are running:
} $pigiFilename {

Copyright (c) 1990-1993 Regents of the University of California
- All rights reserved -
For copyright notice, limitation of liability,
and disclaimer of warranty provisions, push the button below. }
    message $w.f.msg.msg2 -font -Adobe-times-medium-r-normal--*-180* \
	-justify center -text $text -width 20c
    button $w.f.msg.copyright -command "ptkDisplayCopyright" \
	-text {more information} \
	-background burlywood1 -font -Adobe-times-medium-r-normal--*-180*

    pack append $w.f.msg $w.f.msg.msg1 {top fillx} $w.f.msg.msg2 {top} \
	$w.f.msg.copyright {top fillx}

    canvas $w.f.bm -width 6c -height 7.5c
    global ptolemy
    $w.f.bm create bitmap 3c 3.75c -bitmap @$ptolemy/tcl/lib/Ptolemy.xbm
    pack append $w.f $w.f.bm {left} $w.f.msg {right fill expand}
    pack append $w $w.f {top fill expand} $w.ok {top fill expand}

    wm geometry $w +200+300
    tkwait visibility $w
    focus $w
    bind $w <Button> "destroy $w"
    bind $w.f.msg <Button> "destroy $w"
    bind $w.f.msg.msg1 <Button> "destroy $w"
    bind $w.f.msg.msg2 <Button> "destroy $w"
    bind $w.f.bm <Button> "destroy $w"
    bind $w <Key> "destroy $w"
    bind $w.f <Key> "destroy $w"
    grab -global $w
    tkwait window $w
}

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
    set unique [expr $unique+1]

    catch {destroy $w}
    toplevel $w
    wm title $w "Profile"
    wm iconname $w "Profile"

    button $w.ok -text "OK <Return>" -command "destroy $w"
    message $w.msg -font -Adobe-times-medium-r-normal--*-180* -width 25c \
            -text $ptkProfileString -justify left
    pack append $w $w.msg {top fill expand} $w.ok {top fill expand}

    wm geometry $w +200+200
    tkwait visibility $w
    focus $w
    bind $w <Return> "destroy $w"
    bind $w <Button> "destroy $w"
    bind $w.msg <Button> "destroy $w"
}
