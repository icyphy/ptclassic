# @Copyright (c) 1997 The Regents of the University of California.
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
# testtask.tcl
#
# File to demonstrate the Tycho scheduler with a C "task"
# and a Tcl "task" interleaved in execution.
#
# $Id$
#

# Load the package
::tycho::loadIfNotPresent ::tycho::timer tytimer
::tycho::loadIfNotPresent ::tycho::testtask tytest

# Initialize two counters
set ccounter 0
set tclcounter 0

catch {destroy .t}
toplevel .t
frame .t.a
frame .t.b
pack .t.a
pack .t.b

# Create a label which prints the counter value
label .t.a.l -textvariable ccounter -width 8 \
        -font [::tycho::font Helvetica 24]
frame .t.a.buttons

# This button adds a task to the scheduler.
button .t.a.start -text Start -command {
    ::tycho::Scheduler::startC ::tycho::testtask ccounter
}

# This button pauses the task
button .t.a.pause  -text Pause  -command {
    ::tycho::Scheduler::suspend testtask
}

# This button resumes the task
button .t.a.resume  -text Resume  -command {
    ::tycho::Scheduler::resume testtask
}

# This button kills the task
button .t.a.stop  -text Stop -command {
    ::tycho::Scheduler::kill testtask
}

pack .t.a.l -fill x -expand on
pack .t.a.buttons -fill x -expand on
pack .t.a.start -in .t.a.buttons -side left
pack .t.a.pause -in .t.a.buttons -side left
pack .t.a.resume -in .t.a.buttons -side left
pack .t.a.stop -in .t.a.buttons -side left


# Create a label which prints the counter value
label .t.b.l -textvariable tclcounter -width 8 \
        -font [::tycho::font Helvetica 24]
frame .t.b.buttons

# This button adds a task to the scheduler.
button .t.b.start -text Start -command {
    set tclcounter 0
    ::tycho::Scheduler::startTcl tcltask {
        incr tclcounter
        expr 1 ;# 1 means that we haven't terminated
    }
}

# This button pauses the task
button .t.b.pause  -text Pause  -command {
    ::tycho::Scheduler::suspend tcltask
}

# This button resumes the task
button .t.b.resume  -text Resume  -command {
    ::tycho::Scheduler::resume tcltask
}

# This button kills the task
button .t.b.stop  -text Stop -command {
    ::tycho::Scheduler::kill tcltask
}

pack .t.b.l -fill x -expand on
pack .t.b.buttons -fill x -expand on
pack .t.b.start -in .t.b.buttons -side left
pack .t.b.pause -in .t.b.buttons -side left
pack .t.b.resume -in .t.b.buttons -side left
pack .t.b.stop -in .t.b.buttons -side left


# It's handy to have a button to close the window
button .t.close  -text Close -command {
    catch {::tycho::Scheduler::kill testtask}
    catch {::tycho::Scheduler::kill tcltask}
    destroy .t
}
pack .t.close


wm deiconify .t
