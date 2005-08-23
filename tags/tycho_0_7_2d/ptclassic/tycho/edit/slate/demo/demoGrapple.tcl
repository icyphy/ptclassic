# Demonstrate selection and grappling
#
# To run this demo, execute
#    ./demo Grapple
#
# @(#)demoGrapple.tcl	1.1 07/29/98
# Author: John Reekie

proc ::demo::demoGrapple {} {
    set slate [::demo::newslate "Selection and grappling demonstration"]

    # Use the tags draggable and selectable
    ::demo::fourItems $slate [list "draggable" "selectable"]

    # Set up selection and grappling
    ::demo::setupSelectAndGrapple $slate
}


proc ::demo::setupSelectAndGrapple {slate} {
    set follower [$slate interactor Follower]
    set selector [$slate interactor Selector]

    # Enable selection and drag-selection with button 1; shift-click toggles
    $selector bind selectable -button 1
    $selector bind selectable -button 1 -modifiers shift -toggle 1

    # When an item is selected, grapple it. When unselected, ungrapple it
    $selector configure \
	    -selectprefix "::demo::selectGrapple $slate" \
	    -deselectprefix "::demo::deselectGrapple $slate" \
	    -clearprefix "::demo::clearGrapple $slate"

    # In order for the above to work sensibly, turn off automatic
    # highlighting of selected items (we are doing it ourselves)
    $slate configure -highlightselection 0

    # Delegation with regular click
    $selector delegate $follower draggable -button 1 
    $selector delegate $follower editable -button 1 -mode {0 +}

    # Delegation with shift-click
    $selector delegate $follower draggable -button 1 -modifiers shift
}

# Procs for handling selection events
proc ::demo::selectGrapple {slate id} {
    $slate select add $id
    $slate grapple $id
}

proc ::demo::deselectGrapple {slate id} {
    $slate select remove $id
    $slate ungrapple $id
}

proc ::demo::clearGrapple {slate} {
    foreach id [$slate find withtag sel] {
	$slate ungrapple $id
    }
    $slate select clear
}

