# Message routines for use with Ptolemy
# $Id$
# Author: Edward A. Lee
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


###################################################################
# Global variables
#
# the global variable ptolemy is assumed to point to the home directory
# of the Ptolemy installation

# used to collect profile information before displaying:
set ptkProfileString ""

# increment on each use to get unique window names
set unique 0

# flag whether or not to report tcl error messages.
# This should be made a settable option.
set ptkVerboseErrors 0


###################################################################
# Destroy a window if it exists
proc ptkSafeDestroy {win} {if {[winfo exists $win]} {destroy $win}}

###################################################################
# procedure to issue an error message from any internal tk error
#
proc tkerror message {
    set flag 1
    if {$message != ""} {
	ptkImportantMessage .error $message
	set flag 0
    }
    global ptkVerboseErrors
    if {$ptkVerboseErrors} {
        global errorInfo
        if {$errorInfo != ""} {
	    ptkImportantMessage .error $errorInfo
	    set flag 0
        }
    }
    # If no other messages are available, issue a generic one
    if {$flag} {ptkImportantMessage .error "Tcl reports an error"}
    # In case a run is active, request a halt
    halt
}

###################################################################
# Procedure to open a message window and grab the focus.
# The user cannot do anything until the window is dismissed.
#
proc ptkImportantMessage {w text} {
    ptkSafeDestroy $w
    toplevel $w
    wm title $w "Ptolemy Message"
    wm iconname $w "Ptolemy Message"

    button $w.ok -text "OK <Return>" -command "ptkSafeDestroy $w"
    message $w.msg -width 25c -text $text -justify left
    pack append $w $w.msg {top fill expand} $w.ok {top fill expand}

    wm geometry $w +200+200
    tkwait visibility $w
    bind $w <Key> "ptkSafeDestroy $w"
    bind $w <ButtonPress> "ptkSafeDestroy $w"
    bind $w.msg <Button> "ptkSafeDestroy $w"
    set prevFocus [focus]
    focus $w
    grab $w
    tkwait window $w
    focus $prevFocus
}

###################################################################
# Procedure to open a message window and leave it open until dismissed
#
set ptkMessageWindowNum 0

proc ptkMessage {text} {
    # generate unique window name
    global ptkMessageWindowNum
    set w .message$ptkMessageWindowNum
    incr ptkMessageWindowNum
    ptkInform $w $text
}

###################################################################
# Inform the user of something in a window with the specified name
#
proc ptkInform {w text} {
    ptkSafeDestroy $w
    toplevel $w
    wm title $w "Ptolemy Message"
    wm iconname $w "Ptolemy Message"

    button $w.ok -text "OK <Return>" -command "ptkSafeDestroy $w"
    message $w.msg -width 25c -text $text -justify left
    pack append $w $w.msg {top fill expand} $w.ok {top fill expand}

    wm geometry $w +200+200
    tkwait visibility $w
    bind $w <Key> "ptkSafeDestroy $w"
    bind $w <ButtonPress> "ptkSafeDestroy $w"
    bind $w.msg <Button> "ptkSafeDestroy $w"
}


###################################################################
# Procedure to display whatever is in the global variable errorInfo
#
proc ptkDisplayErrorInfo {} {
    global errorInfo
    ptkImportantMessage .error $errorInfo
}

###################################################################
# Open a message window with the Ptolemy bitmap.
# This is actually the main window for the application.
# It gets iconified, not destroyed, when dismissed.
# Only on the first call does it configure the window
# with version information.
#
set ptkStartupMessageCreated 0

proc ptkStartupMessage {pigiVersion pigiFilename} {
    global ptkStartupMessageCreated
    if {$ptkStartupMessageCreated} {
	wm deiconify .
	return
    }
    set ptkStartupMessageCreated 1
    wm title . "Ptolemy Welcome Window"
    wm iconname . "Ptolemy Welcome Window"

    pack [button .mainok -text "OK <Return>" -command "wm iconify ."] \
	-side bottom -fill x -expand 1
    pack [frame .version -relief raised -bd 5] \
	-side top -fill both -expand 1
    pack [frame .version.msg] \
	-side right -fill both -expand 1

    message .version.msg.msg1 -font [option get . bigfont Pigi] \
	-font [option get . mediumfont Pigi] \
	-justify center -foreground [ptkColor firebrick] -width 20c -text { \
Ptolemy Interactive Graphical Interface }

    append text $pigiVersion {
} {
Ptolemy executable you are running:
} $pigiFilename "

Copyright \251 1990-1995 Regents of the University of California
- All rights reserved -
For copyright notice, limitation of liability,
and disclaimer of warranty provisions, push the button below. "

    message .version.msg.msg2 -justify center -text $text -width 20c \
	-font [option get . mediumfont Pigi]

    pack .version.msg.msg1 .version.msg.msg2 -fill x

    pack [frame .version.msg.at -class Attention] -side bottom -fill x

    # We are using the old tk3.x colors, rather than grey85
    button .version.msg.at.copyright -command "ptkDisplayCopyright" \
	-text {more information} \
        -background [ptkColor burlywood1] \
        -activebackground [ptkColor bisque2] \
	-font [option get . mediumfont Pigi]

    pack .version.msg.at.copyright -side bottom -fill x

    global ptolemy
    image create photo ptolemygif -palette 4/4/3  \
	-file $ptolemy/lib/tcl/ptolemyWelcome.gif
    set height [image height ptolemygif]
    set width [image width ptolemygif]
    pack [canvas .version.gif -height $height -width $width -bg \
	[ptkColor bisque] ] -side left
    .version.gif create image [expr $width/2] [expr $height/2] \
	-image ptolemygif

    wm geometry . +200+300
    tkwait visibility .
    bind . <ButtonRelease> "wm iconify ."
    bind .version.msg <Button> "wm iconify ."
    bind .version.msg.msg1 <Button> "wm iconify ."
    bind .version.msg.msg2 <Button> "wm iconify ."
    bind .version.bm <Button> "wm iconify ."
    bind . <Key> "wm iconify ."
    bind .version <Key> "wm iconify ."
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
    focus $w
    bind $w <Return> "ptkSafeDestroy $w"
    bind $w <Button> "ptkSafeDestroy $w"
    bind $w.msg <Button> "ptkSafeDestroy $w"
}
