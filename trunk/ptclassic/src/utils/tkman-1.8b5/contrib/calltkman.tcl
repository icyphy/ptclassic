# Adding the code for callTkMan to your .tkman file causes tkman to
# bring up a second window labeled "callTkMan".  The intent of this
# window is to provide a convenient mechanism for kicking off tkman
# searches without keeping the entire tkman window open on your screen.
# The callTkMan window has 2 buttons and an entry box.  The first button
# labeled "Raise tkman" simply deiconifies the main tkman window if it is
# iconified and raises it.  The second button labeled "Use current
# selection" will deiconify and raise the tkman window and then run a
# search for the current selection as if you had typed it into the entry
# box at the top of the tkman window.  The entry box is labeled "Enter
# topic:" and allows you to type a topic into this entry box and when
# you hit return it will deiconify and raise the tkman window and then
# act as if you had typed it into the entry box at the top of the tkman
# window.  I routinely close my tkman window to save screen real estate
# but always keep the callTkMan window in an unobscured position on the
# screen so I can quickly kick off a man search.

# There are several options in installing this code.  First, an individual
# may append this file to then end of the ~/.tkman file.  Second, the
# code may be added to the file tkman.tcl before the `make install'
# thus creating the callTkMan window for everyone who shares the executable.
# Finally, one can delete the last line of the file--the invocation line
# "callTkManInit"--and append the remaining code to tkman.tcl before the 
# `make'. This final option doesn't show the callTkMan window by default,
# but rather makes it available to interested parties via a single
# addition to the ~/.tkman file, namely the addition of a single line
# that reads "callTkManInit"

# questions to  Neil.Smithline@eng.sun.com


proc callTkManInit {} {
    set w .callTkMan

    toplevel $w

    button $w.raise -text {Raise tkman} -command manRaise
    button $w.cur -text {Use current selection} -command manCurSel
    label $w.label -text {Enter topic: }
    entry $w.entry -text {Use current selection} \
	-textvariable mantextsel -relief sunken
    bind $w.entry <KeyPress-Return> manEntry
    frame $w.frame -relief raised -borderwidth 2
    
    pack $w.raise $w.cur -side top -expand 1 -fill x
    pack $w.label -side left -in $w.frame
    pack $w.entry -side left -expand 1 -fill x -in $w.frame
    pack $w.frame -side top -expand 1 -fill x
    raise $w.label
    raise $w.entry
    wm maxsize $w 800 800


    $w configure
}

proc manRaise {} {
	raise .man
	wm deiconify .man
}

proc manCurSel {} {
	doMan [selection get]
}

proc manEntry {} {
	global mantextsel
	doMan $mantextsel
}


proc doMan {topic} {
	manRaise
	manShowMan $topic
}

callTkManInit
