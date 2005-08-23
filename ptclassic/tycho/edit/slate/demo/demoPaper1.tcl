# Create some sample items for the tcltk-98 paper
#
# To run this demo, execute
#    ./demo Paper1
#
# @(#)demoPaper1.tcl	1.1 07/29/98
# Author: John Reekie

proc ::demo::demoPaper1 {} {
    set slate [::demo::newslate "Samples for Tcl/Tk paper -- select and drag"]

    ::demo::fourItems $slate "moveable"
}

proc ::demo::fourItems {slate {tags {}}} {
        set f [$slate create Frame 50 50 100 100 -color green -relief ridge \
	    -borderwidth 4 -tags $tags]

    set s [$slate create Solid 220.0 60.0 252.0 60.0 252.0 44.0 \
	    284.0 76.0 252.0 108.0 252.0 92.0 220.0 92.0 220.0 60.0 \
	    -color green -borderwidth 4 -tags $tags]

    set r [$slate create LabeledRect 50 150 100 200 -fill green2  \
	    -outline red -width 3 \
	    -textanchor nw -text "Foo!" \
	    -graphics "line 0 0 100 100; line 0 100 100 0" \
	    -tags $tags]

    set l [$slate create SmartLine 220 150 260 200 \
	    -start s -end w -width 3 -alpha 20 -arrow last \
	    -tags $tags]
}
