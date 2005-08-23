# Demonstrate animation
#
# To run this demo, execute
#    ./demo Animator
#
# @(#)demoAnimator.tcl	1.1 07/29/98
# Author: John Reekie

proc ::demo::demoAnimator {} {
    set s [::demo::newslate "Animation demonstration" \
	    -height 400 -width 500]

    set f [$s create Frame 50 50 100 100 -color green]
    set a [$s create Solid 150.0 60.0 182.0 60.0 \
	    182.0 44.0 214.0 76.0 182.0 108.0 182.0 \
	    92.0 150.0 92.0 150.0 60.0 \
	    -color red]
    update
    after 1000
    $s animate move $f 100 100
    $s animate move $a 50 50 -duration 500
    $s animate wait
    $s animate reshape $f 100 100 -features se -interval 200 
    $s animate wait
    $s animate reshape $f 100 100 -features nw -duration 2000
    $s animate reshape $a 100 0 \
	    -features {vertex-1 vertex-2 vertex-3 vertex-4 vertex-5} \
	    -duration 2000
    $s animate wait
    $s animate move $f -200 -200 -ghost 1
    $s animate reshape $a 100 0 \
	    -features {vertex-0 vertex-6}
}
