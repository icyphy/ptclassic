# testtask.tcl
#
# File to demonstrate the Tycho scheduler with a C "task"
# and a Tcl "task" interleaved in execution.
#
# $Id$
#

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
    ::tycho::Scheduler::start ctask {
	::tycho::testtask setup ccounter
    } {
	::tycho::testtask execute
    } {
        ::tycho::testtask wrapup
    } {}
}

# This button pauses the task
button .t.a.pause  -text Pause  -command {
    ::tycho::Scheduler::suspend ctask
}

# This button resumes the task
button .t.a.resume  -text Resume  -command {
    ::tycho::Scheduler::resume ctask
}

# This button kills the task
button .t.a.stop  -text Stop -command {
    ::tycho::Scheduler::kill ctask
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
    ::tycho::Scheduler::start tcltask {
	set tclcounter 0
    } {
	::tycho::timer start
	while { ! [::tycho::timer elapsed] } {
	incr tclcounter
	}
	expr 1 ;# 1 means that we haven't terminated
    } {} {}
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
    catch {::tycho::Scheduler::kill ctask}
    catch {::tycho::Scheduler::kill bar}
    destroy .t
}
pack .t.close


wm deiconify .t
