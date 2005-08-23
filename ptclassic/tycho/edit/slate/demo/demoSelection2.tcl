# Illustrate selecting and editing items, version 2.
#
# To run this demo, execute
#    ./demo Selection2
#
# @(#)demoSelection2.tcl	1.1 07/29/98
# Author: John Reekie

proc ::demo::demoSelection2 {} {

    set slate [::demo::newslate "Selection and editing, version 2"]
    ::demo::selectionItems $slate

    # CReate interactors
    set follower [$slate interactor Follower]
    set lineeditor [$slate interactor TextItemEdit]
    set selector [$slate interactor Selector]

    # Enable selection and drag-selection with button 1; shift-click toggles
    $selector bind selectable -button 1
    $selector bind selectable -button 1 \
	    -modifiers shift -toggle 1
    # Delegation with regular click
    $selector delegate $follower draggable -button 1 
    $selector delegate $follower editable -button 1 -mode {1 +}
    $selector delegate $lineeditor editable -button 1 -mode 0

    # Delegation with shift-click
    $selector delegate $follower draggable -button 1 \
	    -modifiers shift
    $selector delegate $follower editable -button 1 -mode {1 +} \
	    -modifiers shift
    $selector delegate $lineeditor editable -button 1 -mode 0 \
	    -modifiers shift

    # Enable selection and drag-selection with control-button 1;
    # shift-control-click toggles
    $selector bind selectable -button 1 \
	    -modifiers control$selector bind selectable -button 1 \
	    -modifiers {shift control} -toggle 1

    # Delegation with control-click
    $selector delegate $follower draggable -button 1 \
	    -modifiers control
    $selector delegate $follower editable -button 1 \
	    -modifiers control

    # Delegation with shift-control-click
    $selector delegate $follower draggable -button 1 \
	    -modifiers {shift control} -toggle 1
    $selector delegate $follower editable -button 1 \
	    -modifiers {shift control} -toggle 1
}
