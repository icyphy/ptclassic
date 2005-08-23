# Demonstrate how binding works in a hierarchy
#
# To run this demo, execute
#    ./demo Binding
#
# @(#)demoBinding.tcl	1.1 07/29/98
# Author: John Reekie

proc ::demo::demoBinding {} {
    set slate [::demo::newslate "A binding demonstration"]

    # Create an item
    set box [$slate create LabeledRect 20 20 100 80 -fill yellow \
	    -outline red -width 1 -textanchor nw -text "Mouse me!" \
	    -graphics {
	Frame 10 10 50 50 -color green
	Frame 50 50 90 90 -color red
    } -tags draggable]
	
    # Set up bindings for moving the item
    $slate bind draggable <Button-1> "set _x %x; set _y %y"
    $slate bind draggable <B1-Motion> \
	    "$slate move \[$slate find withtag current\] \
	    \[expr %x-\$_x\] \[expr %y - \$_y\]; \
	    set _x %x; set _y %y"
    $slate bind draggable <ButtonRelease-1> "puts Done!"
	
    # Create another item
    set newbox [$slate create LabeledRect 100 100 180 160 -fill pink \
		-outline blue -width 1 -textanchor se -text "Me too!" \
		-graphics {
	    rectangle 0 0 0 0 -outline ""
    } -tags draggable]

    # Create active children
    set one [$slate createrootchild $newbox Frame 110 110 140 130 -color green]
    set two [$slate createrootchild $newbox Frame 140 130 170 150 -color red]

    # Make bindings for them
    $slate bind $one <Button-1> "toggleItem $slate $one"
    $slate bind $two <Button-1> "toggleItem $slate $two"
}

# Here's a proc to make the children do something usefule
proc toggleItem {slate item} {
    if { [$slate itemcget $item -relief] == "raised" } {
	$slate itemconfigure $item -relief sunken
    } else {
	$slate itemconfigure $item -relief raised
    }
}
