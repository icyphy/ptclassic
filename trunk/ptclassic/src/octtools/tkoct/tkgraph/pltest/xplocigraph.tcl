proc xplocigraph.tcl { } { }
#
# Encapsulation window.  The {w} passed in should be of class
# XPPlaneGraph for this to work.
#
proc xpLociPlaneGraph { w } {
    upvar #0 $w.draw pvars
    set pvars(graph) $w
    set pvars(conj) 0

    menubutton $w.title -text "Complex Plane" -menu $w.title.m
    pack append $w $w.title { top filly }
    set dd $w.title.m
    menu $dd
    $dd add command -label "Update" -com "$w.draw axisxf"

    frame $w.loc
    pack append $w.loc [label $w.loc.xy] {left expand frame w}
    place [label $w.loc.state] -relx .5 -anchor n
    pack append $w.loc [label $w.loc.mp] {right expand frame e}
    pack append $w $w.loc {top fill}
    set pvars(locwin) $w.loc
    set pvars(locfmt) "%.3E"

    xpaxis $w.lAxis -side right -flip true
    pack append $w $w.lAxis {left filly}

    xpaxis $w.bAxis -side top
    pack append $w $w.bAxis {bottom fillx}

    xpplanewidget $w.draw -xaxis $w.bAxis -yaxis $w.lAxis -datasrc loci
    pack append $w $w.draw {top expand fill}

    $w.lAxis conf -dataWdg $w.draw -command "$w.draw axisxf"
    $w.bAxis conf -dataWdg $w.draw -command "$w.draw axisxf"
    xpAxis.AspectOn $w.lAxis $w.bAxis

    rubber $w.draw.rubber $w.draw -command "xpPlane.rubberFunc $w.draw"

    frame $w.k; pack before $w.lAxis $w.k { bottom fillx }
    label $w.k.lbl -text "K:"
    pack append $w.k $w.k.lbl { left }
    dscale $w.k.ds -orient horiz -command "$w.draw locusmark"
    pack append $w.k $w.k.ds { left expand fill }
}
