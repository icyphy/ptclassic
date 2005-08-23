# Create a draggable SmartLine item.
# It would be easier to use grapple{}, but this looks neater!
#
# To run this demo, execute
#    ./demo SmartLine
#
# @(#)demoSmartLine.tcl	1.1 07/29/98
# Author: John Reekie

proc ::demo::demoSmartLine {} {
    set slate [::demo::newslate "SmartLine demo"]
    set win [winfo toplevel $slate]
    
    set follower [$slate interactor Follower]
    set line [$slate create SmartLine 150 120 200 200 -arrow last]
    set grapple [$slate create Solid 200 200 210 190 220 200 210 210 \
	    -color green]

    $follower bind $grapple
    $follower configure \
	    -clickprefix "::demo::reshapeLine click $slate $line $grapple" \
	    -dragprefix "::demo::reshapeLine drag $slate $line $grapple" \
	    -releaseprefix "::demo::reshapeLine release $slate $line $grapple"

    set frame [frame $win.f]
    set ::demo::_dir_ e
    radiobutton $frame.north -text North -variable ::demo::_dir_ -value n \
	    -command "::demo::reshapeEnd $slate $line $grapple 3 n"
    radiobutton $frame.south -text South -variable ::demo::_dir_ -value s \
	    -command "::demo::reshapeEnd $slate $line $grapple 1 s"
    radiobutton $frame.east -text East -variable ::demo::_dir_ -value e \
	    -command "::demo::reshapeEnd $slate $line $grapple 0 e"
    radiobutton $frame.west -text West -variable ::demo::_dir_ -value w \
	    -command "::demo::reshapeEnd $slate $line $grapple 2 w"
    grid $frame.north $frame.south $frame.east $frame.west
    pack $frame -side right
}

proc ::demo::reshapeLine {mode slate line grapple id {x {}} {y {}} args} {
    switch -exact $mode {
	"click" {
	    $slate moveclick $grapple $x $y
	    $slate reshapeclick $line $x $y -features end
	}
	"drag" {
	    $slate movedrag $grapple $x $y
	    $slate reshapedrag $line $x $y
	}
	"release" {
	    $slate moverelease $grapple
	    $slate reshaperelease $line
	}
    }
}

proc ::demo::reshapeEnd {slate line grapple vertex dir} {
    eval $slate reshapeto $line [$slate feature $grapple vertex-$vertex] end
    $slate itemconfigure $line -end $dir
}

