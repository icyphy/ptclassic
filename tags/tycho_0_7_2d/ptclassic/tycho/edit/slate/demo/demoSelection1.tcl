# Illustrate selecting and editing items, version 1.
#
# To run this demo, execute
#    ./demo Selection1
#
# @(#)demoSelection1.tcl	1.1 07/29/98
# Author: John Reekie

proc ::demo::demoSelection1 {} {

    set slate [::demo::newslate "Selection and editing, version 1"]
    ::demo::selectionItems $slate

    set follower [$slate interactor Follower]
    set lineeditor [$slate interactor TextItemEdit]
    set selector [$slate interactor Selector]

    # Enable selection and drag-selection with button 1; shift-click toggles
    $selector bind selectable -button 1
    $selector bind selectable -button 1 \
	    -modifiers shift -toggle 1

    # Delegation with regular click
    $selector delegate $follower draggable -button 1 
    $selector delegate $follower editable -button 1 -mode {0 +}
    $selector delegate $lineeditor editable -button 1 -mode 1

    # Delegation with shift-click
    $selector delegate $follower draggable -button 1 \
	    -modifiers shift
    $selector delegate $follower editable -button 1 -mode {0 +} \
	    -modifiers shift

    # This is not needed!!!
    # $selector delegate $lineeditor editable -button 1 -mode 1 \
    #	    -modifiers shift
}


# Code to create items for selection demos
proc ::demo::selectionItems {slate} {
    # To illustrate, here are some items. The green frame and the labeled
    # rectangle can be moved; the arrow can be selected but not moved, and
    # the plain text item can be selected and edited but not moved.
    $slate create Frame 120 120 150 140 -borderwidth 4 -color green \
	    -tags {draggable selectable}
    $slate create Solid 100 20 120 20 120 10 140 30 120 50 120 40 100 40 \
	    -tags selectable
    $slate create text 50 50 -text {Fred said hello} -anchor w \
	    -tags {selectable editable}
    $slate create text 150 200 -text {Over to you, Spencer!} \
	    -tags {selectable editable}
    $slate create LabeledRect 170 65 239 105 -fill blue -outline red \
	    -anchor nw -text Foo \
	    -graphics "line 0 0 100 100; line 0 100 100 0" \
	    -tags {draggable selectable}
}
