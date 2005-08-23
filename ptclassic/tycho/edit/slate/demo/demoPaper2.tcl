# Create some more sample items for the tcltk-98 paper
#
# To run this demo, execute
#    ./demo Paper2
#
# @(#)demoPaper2.tcl	1.1 07/29/98
# Author: John Reekie

proc ::demo::demoPaper2 {} {
    # Start here
    set slate [::demo::newslate "More samples for Tcl/Tk paper"]

    set citem [$slate create ComplexItem 50 50 100 100 \
	     -tags moveable]

    #$slate createchild $citem rectangle 50 50 100 100 -outline grey
    $slate createchild $citem line 50 50 100 100
    $slate createchild $citem line 50 100 100 50
    $slate createchild $citem oval 60 60 90 90 -fill green

    set citem2 [$slate create ComplexItem 50 50 100 100 \
	     -tags moveable]
    
    #$slate createchild $citem2 rectangle 50 50 100 100 -outline grey
    $slate createchild $citem2 line 50 50 100 100
    $slate createchild $citem2 line 50 100 100 50
    $slate createchild $citem2 oval 60 60 90 90 -fill green

    $slate move $citem2 100 0
    $slate scale $citem2 150 50 0.5 1.5
    
    set citem3 [$slate create ComplexItem 50 50 100 100 \
	     -tags moveable]

    #$slate createchild $citem3 rectangle 50 50 100 100 -outline grey
    $slate createchild $citem3 line 50 50 100 100
    $slate createchild $citem3 line 50 100 100 50
    $slate createchild $citem3 oval 60 60 90 90 -fill green

    $slate move $citem3 100 0
    $slate scale $citem3 150 50 0.5 1.5
    $slate move $citem3 80 0

    $slate grapple $citem3
}
