# Create reshapable LabeledLine items.
#
# To run this demo, execute
#    ./demo LabeledLine
#
# @(#)demoLabeledLine.tcl	1.1 07/29/98
# Author: John Reekie

proc ::demo::demoLabeledLine {} {
    set slate [::demo::newslate "LabeledLine demo"]
    set win [winfo toplevel $slate]

    # A line with an even number of segments
    set leven [$slate create LabeledLine 50 50 110 100 110 200 \
	    -arrow last -text Foo -tags line]

    # A line with an odd number of segments
    set lodd [$slate create LabeledLine 210 50 260 100 200 145 240 180 \
	    -arrow last -text Bar -tags line]

    # Make them reshapable
    $slate grapple $leven
    $slate grapple $lodd

    # Radiobuttons to choose line style
    set frame [frame $win.f]
    set ::demo::_smooth_ off
    radiobutton $frame.off -text Regular -variable ::demo::_smooth_ -value off \
	    -command "::demo::lineSmooth $slate off"
    radiobutton $frame.on -text Smooth -variable ::demo::_smooth_ -value on \
	    -command "::demo::lineSmooth $slate on"
    grid $frame.off $frame.on
    pack $frame -side right

}

proc ::demo::lineSmooth {slate smooth} {
    foreach id [$slate find withtag line] {
	$slate itemconfigure $id -smooth $smooth
    }
}
