# test.tcl
#
# File to demonstrate the Tycho/C scheduler
#
# $Id$
#

FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME 

# Load the Tycho scheduler interface
::tycho::loadIfNotPresent ::tycho::scheduler tysched

# Initialize a Tk counter
set counter 0

# Create a label which prints the counter value
catch {destroy .t}
toplevel .t
label .t.l -textvariable counter -width 8 \
        -font [::tycho::font Helvetica 24]
frame .t.f

# This button starts the event loop and the C counter.
# Use a long period just to demonstrate that events are
# only processed at the correct interval
button .t.start -text Start -command {
    ::tycho::eventLoop start 500
    ::tycho::eventTest start counter
}

# This button stops the C counter and then the event loop
button .t.stop  -text Stop  -command {
    ::tycho::eventTest stop
    ::tycho::eventLoop stop
}

# It's handy to have a button to close the window
button .t.close  -text Close -command {
    ::tycho::eventTest stop
    ::tycho::eventLoop stop
    destroy .t
}

pack .t.l -fill x -expand on
pack .t.f -fill x -expand on
pack .t.start -in .t.f -side left
pack .t.stop -in .t.f -side left
pack .t.close -in .t.f -side left

wm deiconify .t
