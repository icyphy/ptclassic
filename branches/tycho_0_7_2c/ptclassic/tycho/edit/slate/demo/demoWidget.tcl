# Demonstrate the principles of constructing a custom widget
#
# To run this demo, execute
#    ./demo Widget
#
# @(#)demoWidget.tcl	1.1 07/29/98
# Author: John Reekie

proc ::demo::demoWidget {} {
    set slate [::demo::newslate "A pretend widget" \
	    -height 180 -width 100]

    # Create the display elements
    set value [$slate create text 50 20 -text 0 -anchor s -fill blue]
    set trough [$slate create Frame 48 23 52 143 -color darkgrey \
	    -borderwidth 2 -relief sunken]
    set bar [$slate create Frame 40 132 60 142 \
	    -color darkseagreen -borderwidth 3]
    set label [$slate create text 50 150 -text "Fred" \
                    -anchor n -justify center]

    # The bounder moves the bar along the trough
    set bounder [$slate interactor Bounder \
	    -dragcommand "updateWidget $slate $bar $value" \
	    -constrain y -bounds {0 24 0 142}]	
    $bounder bind $bar -button 1

    # The stepper quantizes movement to increments of 0.5
    set stepper [$slate interactor Stepper -stepsize [expr 108.0/22]]
    $bounder cascade $stepper

    # For fun, add a "real" Slider
    # set slider [::tycho::Slider [winfo parent $slate].slider]
    # pack $slider -side bottom
}

proc updateWidget {slate bar value args} {
    set position [expr [lindex [$slate coords $bar] 1] + 5]
    set x [expr (137.0-$position)/108.0 * 11.0]
    $slate itemconfigure $value -text [format %.1f $x]
}
