#  -*- mode: cts -*-
#  -*- class: ::tycho::PortedGraph -*-
#  -*- mtime: Fri May 08 19:28:22 PDT 1998 -*-

annotation layout -data {#  -*- mode: tcl -*-
#  -*- class: ::tycho::Layout -*-
#  -*- mtime: Fri May 08 19:28:22 PDT 1998 -*-

item vertex0 -slatetype IconFrame -anchor center -hsize 60 -text Const -borderwidth 2 -color lightgreen -tags vertex -font -adobe-helvetica-medium-r-normal-*-*-120-75-75-*-*-iso8859-* -label Const -textanchor center -shape point -graphics {
        polygon 30 30 30 80 80 80 80 30 -fill orange -outline ""
        line 10 80 80 80 -width 1 -fill black
        line 30 80 30 30 -width 1 -fill black
        line 30 30 80 30 -width 1 -fill black
    } -coords {166.0 208.0} -relief raised -vsize 40 {
    item output -tags output -slatetype Terminal -direction w -style arrowout -coords {0 50}
}
item vertex2 -slatetype IconFrame -anchor center -hsize 60 -text Integrator -borderwidth 2 -color blue -tags vertex -font -adobe-helvetica-medium-r-normal-*-*-120-75-75-*-*-iso8859-* -label Integrator -textanchor center -shape point -graphics {
         polygon 30 25 30 70 25 75 20 70 15 80 25 85 35 75 35 30  40 25 45 30 50 25 40 15 -fill black -outline yellow
         text 70 55 -text dx -fill yellow 
     } -coords {425.0 98.0} -relief raised -vsize 40 {
    item input -slatetype Terminal -tags input -style arrowtail -direction w -coords {0 50}
    item output -slatetype Terminal -tags output -style arrowout -direction e -coords {100 50}
}
item vertex3 -slatetype IconFrame -anchor center -hsize 60 -text Integrator -borderwidth 2 -color blue -tags vertex -font -adobe-helvetica-medium-r-normal-*-*-120-75-75-*-*-iso8859-* -label Integrator -textanchor center -shape point -graphics {
         polygon 30 25 30 70 25 75 20 70 15 80 25 85 35 75 35 30  40 25 45 30 50 25 40 15 -fill black -outline yellow
         text 70 55 -text dx -fill yellow 
     } -coords {580.0 267.0} -relief raised -vsize 40 {
    item input -slatetype Terminal -tags input -style arrowtail -direction w -coords {0 50}
    item output -slatetype Terminal -tags output -style arrowout -direction e -coords {100 50}
}
item vertex4 -slatetype IconFrame -anchor center -hsize 60 -text Integrator -borderwidth 2 -color blue -tags vertex -font -adobe-helvetica-medium-r-normal-*-*-120-75-75-*-*-iso8859-* -label Integrator -textanchor center -shape point -graphics {
         polygon 30 25 30 70 25 75 20 70 15 80 25 85 35 75 35 30  40 25 45 30 50 25 40 15 -fill black -outline yellow
         text 70 55 -text dx -fill yellow 
     } -coords {406.0 441.0} -relief raised -vsize 40 {
    item input -slatetype Terminal -tags input -style arrowtail -direction w -coords {0 50}
    item output -slatetype Terminal -tags output -style arrowout -direction e -coords {100 50}
}
item vertex5 -slatetype IconOval -anchor center -hsize 50 -text {} -fill white -tags vertex -font -adobe-helvetica-medium-r-normal-*-*-120-75-75-*-*-iso8859-* -label Multiply -textanchor center -shape point -graphics {
        line 20 20 80 80 -width 4
        line 20 80 80 20 -width 4
    } -coords {254.0 292.0} -outline black -width 1 -vsize 50 {
    item input-0 -slatetype Terminal -tags input -style arrowtail -direction w -coords {0 50}
    item input-1 -slatetype Terminal -tags input -style arrowtail -direction n -coords {50 0}
    item output -slatetype Terminal -tags output -style arrowout -direction e -coords {100 50}
}
item vertex6 -slatetype IconOval -anchor center -hsize 50 -text {} -fill white -tags vertex -font -adobe-helvetica-medium-r-normal-*-*-120-75-75-*-*-iso8859-* -label Multiply -textanchor center -shape point -graphics {
        line 20 20 80 80 -width 4
        line 20 80 80 20 -width 4
    } -coords {87.0 451.0} -outline black -width 1 -vsize 50 {
    item input-0 -slatetype Terminal -tags input -style arrowtail -direction w -coords {0 50}
    item input-1 -slatetype Terminal -tags input -style arrowtail -direction n -coords {50 0}
    item output -slatetype Terminal -tags output -style arrowout -direction e -coords {100 50}
}
item vertex7 -slatetype IconFrame -anchor center -hsize 60 -text Gain -borderwidth 2 -color lightsteelblue -tags vertex -font -adobe-helvetica-medium-r-normal-*-*-120-75-75-*-*-iso8859-* -label Gain -textanchor center -shape point -graphics {
         polygon 70 20 70 80 30 50 -fill red -outline black
         line 10 50 30 50 -width 1 -fill black
         line 70 50 90 50 -width 1 -fill black
     } -coords {253.0 36.0} -relief raised -vsize 40 {
    item input -slatetype Terminal -tags input -style arrowtail -direction e -coords {100 50}
    item output -slatetype Terminal -tags output -style arrowout -direction w -coords {0 50}
}
item vertex8 -slatetype IconFrame -anchor center -hsize 60 -text Gain -borderwidth 2 -color lightsteelblue -tags vertex -font -adobe-helvetica-medium-r-normal-*-*-120-75-75-*-*-iso8859-* -label Gain -textanchor center -shape point -graphics {
         polygon 70 20 70 80 30 50 -fill red -outline black
         line 10 50 30 50 -width 1 -fill black
         line 70 50 90 50 -width 1 -fill black
     } -coords {578.0 441.0} -relief raised -vsize 40 {
    item input -tags input -slatetype Terminal -direction w -style arrowtail -coords {0 50}
    item output -tags output -slatetype Terminal -direction e -style arrowout -coords {100 50}
}
item vertex9 -slatetype IconFrame -anchor center -hsize 60 -text Gain -borderwidth 2 -color lightsteelblue -tags vertex -font -adobe-helvetica-medium-r-normal-*-*-120-75-75-*-*-iso8859-* -label Gain -textanchor center -shape point -graphics {
         polygon 70 20 70 80 30 50 -fill red -outline black
         line 10 50 30 50 -width 1 -fill black
         line 70 50 90 50 -width 1 -fill black
     } -coords {255.0 103.0} -relief raised -vsize 40 {
    item input -tags input -slatetype Terminal -direction w -style arrowtail -coords {0 50}
    item output -tags output -slatetype Terminal -direction e -style arrowout -coords {100 50}
}
item vertex10 -slatetype IconFrame -anchor center -hsize 60 -text Gain -borderwidth 2 -color lightsteelblue -tags {vertex sel} -font -adobe-helvetica-medium-r-normal-*-*-120-75-75-*-*-iso8859-* -label Gain -textanchor center -shape point -graphics {
         polygon 70 20 70 80 30 50 -fill red -outline black
         line 10 50 30 50 -width 1 -fill black
         line 70 50 90 50 -width 1 -fill black
     } -coords {501.0 190.0} -relief raised -vsize 40 {
    item input -slatetype Terminal -tags input -style arrowtail -direction e -coords {100 50}
    item output -slatetype Terminal -tags output -style arrowout -direction w -coords {0 50}
}
item vertex11 -slatetype IconOval -anchor center -hsize 50 -text {} -fill white -tags vertex -font -adobe-helvetica-medium-r-normal-*-*-120-75-75-*-*-iso8859-* -label Add -textanchor center -shape point -graphics {
        line 20 50 80 50 -width 4
        line 50 20 50 80 -width 4
    } -coords {126.0 104.0} -width 1 -outline black -vsize 50 {
    item input-0 -slatetype Terminal -tags input -style arrowtail -direction w -coords {0 50}
    item input-1 -slatetype Terminal -tags input -style arrowtail -direction n -coords {50 0}
    item output -slatetype Terminal -tags output -style arrowout -direction e -coords {100 50}
}
item vertex12 -slatetype IconOval -anchor center -hsize 50 -text {} -fill white -tags vertex -font -adobe-helvetica-medium-r-normal-*-*-120-75-75-*-*-iso8859-* -label Add -textanchor center -shape point -graphics {
        line 20 50 80 50 -width 4
        line 50 20 50 80 -width 4
    } -coords {102.0 290.0} -outline black -width 1 -vsize 50 {
    item input-0 -slatetype Terminal -tags input -style arrowtail -direction w -coords {0 50}
    item input-1 -slatetype Terminal -tags input -style arrowtail -direction n -coords {50 0}
    item output -slatetype Terminal -tags output -style arrowout -direction e -coords {100 50}
}
item vertex13 -slatetype IconOval -anchor center -hsize 50 -text {} -fill white -tags vertex -font -adobe-helvetica-medium-r-normal-*-*-120-75-75-*-*-iso8859-* -label Add -textanchor center -shape point -graphics {
        line 20 50 80 50 -width 4
        line 50 20 50 80 -width 4
    } -coords {387.0 304.0} -width 1 -outline black -vsize 50 {
    item input-0 -slatetype Terminal -tags input -style arrowtail -direction w -coords {0 50}
    item input-1 -slatetype Terminal -tags input -style arrowtail -direction n -coords {50 0}
    item output -slatetype Terminal -tags output -style arrowout -direction e -coords {100 50}
}
item edge0 -slatetype Net -toaspect s -start e -tags edge -fill black -shape line -state 0 -coords {475.0 98.0 621.0 98.0 621.0 60.0} -smooth off -arrow none -beta 30 -fromaspect terminal -capstyle projecting -alpha 12 -end n -width 1 -joinstyle miter -autoshape 1
item edge1 -toaspect terminal -slatetype Net -start w -tags edge -fill black -shape line -state 0 -coords {203.0 36.0 126.0 36.0 126.0 59.0} -smooth off -arrow none -beta 30 -fromaspect terminal -capstyle projecting -alpha 12 -end s -width 1 -joinstyle miter -autoshape 1
item edge2 -toaspect s -slatetype Net -start e -tags edge -fill black -shape line -state 0 -coords {630.0 267.0 659.0 267.0 659.0 218.0} -smooth off -arrow none -beta 30 -fromaspect terminal -capstyle projecting -alpha 12 -end n -width 1 -joinstyle miter -autoshape 1
item edge3 -toaspect terminal -slatetype Net -start e -tags edge -fill black -shape line -state 1 -coords {171.0 104.0 188.0 104.0 188.0 103.0 205.0 103.0} -smooth off -arrow none -beta 30 -fromaspect terminal -capstyle projecting -alpha 12 -end e -width 1 -joinstyle miter -autoshape 1
item edge4 -toaspect terminal -slatetype Net -start e -tags edge -fill black -shape line -state 1 -coords {305.0 103.0 340.0 103.0 340.0 98.0 375.0 98.0} -smooth off -arrow none -beta 30 -fromaspect terminal -capstyle projecting -alpha 12 -end e -width 1 -joinstyle miter -autoshape 1
item edge5 -toaspect terminal -slatetype Net -start w -tags edge -fill black -shape line -state 0 -coords {116.0 208.0 102.0 208.0 102.0 245.0} -smooth off -arrow none -beta 30 -fromaspect terminal -capstyle projecting -alpha 12 -end s -width 1 -joinstyle miter -autoshape 1
item edge6 -toaspect w -slatetype Net -start e -tags edge -fill black -shape line -state 0 -coords {456.0 441.0 483.0 441.0} -smooth off -arrow none -beta 30 -fromaspect terminal -capstyle projecting -alpha 12 -end e -width 1 -joinstyle miter -autoshape 1
item edge7 -toaspect terminal -slatetype Net -start e -tags edge -fill black -shape line -state 3 -coords {628.0 441.0 640.0 441.0 640.0 365.5 45.0 365.5 45.0 290.0 57.0 290.0} -smooth off -arrow none -beta 30 -fromaspect terminal -capstyle projecting -alpha 12 -end e -width 1 -joinstyle miter -autoshape 1
item edge8 -toaspect terminal -slatetype Net -start e -tags edge -fill black -shape line -state 2 -coords {147.0 290.0 178.0 290.0 178.0 292.0 209.0 292.0} -smooth off -arrow none -beta 30 -fromaspect terminal -capstyle projecting -alpha 12 -end e -width 1 -joinstyle miter -autoshape 1
item junction0 -slatetype Junction -tags junction -size 5 -fill black -width 1 -outline black -shape point -coords {621.0 55.0}
item edge10 -slatetype Net -toaspect terminal -start n -tags edge -fill black -shape line -state 0 -coords {621.0 50.0 621.0 36.0 303.0 36.0} -smooth off -arrow none -beta 30 -fromaspect n -capstyle projecting -alpha 12 -end w -width 1 -joinstyle miter -autoshape 1
item edge11 -slatetype Net -toaspect n -start e -tags edge -fill black -shape line -state 6 -coords {626.0 55.0 668.0 55.0 668.0 25.0 638.0 25.0 638.0 88.0} -smooth off -arrow none -beta 30 -fromaspect e -capstyle projecting -alpha 12 -end s -width 1 -joinstyle miter -autoshape 1
item junction1 -slatetype Junction -tags junction -size 5 -fill black -width 1 -outline black -shape point -coords {660.0 161.0}
item edge12 -slatetype Net -toaspect terminal -start n -tags edge -fill black -shape line -state 1 -coords {660.0 156.0 660.0 144.0 69.0 144.0 69.0 104.0 81.0 104.0} -smooth off -arrow none -beta 30 -fromaspect n -capstyle projecting -alpha 12 -end e -width 1 -joinstyle miter -autoshape 1
item edge13 -toaspect terminal -slatetype Net -start w -tags edge -fill black -shape line -state 2 -coords {655.0 161.0 603.0 161.0 603.0 190.0 551.0 190.0} -smooth off -arrow none -beta 30 -fromaspect w -capstyle projecting -alpha 12 -end w -width 1 -joinstyle miter -autoshape 1
item edge14 -toaspect terminal -slatetype Net -start e -tags edge -fill black -shape line -state 2 -coords {299.0 292.0 320.5 292.0 320.5 304.0 342.0 304.0} -smooth off -arrow none -beta 30 -fromaspect terminal -capstyle projecting -alpha 12 -end e -width 1 -joinstyle miter -autoshape 1
item edge15 -toaspect terminal -slatetype Net -start w -tags edge -fill black -shape line -state 0 -coords {451.0 190.0 387.0 190.0 387.0 259.0} -smooth off -arrow none -beta 30 -fromaspect terminal -capstyle projecting -alpha 12 -end s -width 1 -joinstyle miter -autoshape 1
item edge16 -toaspect terminal -slatetype Net -start e -tags edge -fill black -shape line -state 1 -coords {432.0 304.0 481.0 304.0 481.0 267.0 530.0 267.0} -smooth off -arrow none -beta 30 -fromaspect terminal -capstyle projecting -alpha 12 -end e -width 1 -joinstyle miter -autoshape 1
item junction2 -slatetype Junction -coords {659.0 213} -tags junction
item edge17 -slatetype Net -toaspect terminal -start n -tags edge -fill black -shape line -state 2 -coords {659.0 208.0 659.0 187.0 660.0 187.0 660.0 166.0} -smooth off -arrow none -beta 30 -fromaspect n -capstyle projecting -alpha 12 -end n -width 1 -joinstyle miter -autoshape 1
item edge19 -slatetype Net -toaspect terminal -start e -tags edge -fill black -shape line -state 5 -coords {664.0 213.0 676.0 213.0 676.0 394.0 87.0 394.0 87.0 406.0} -smooth off -arrow none -beta 30 -fromaspect e -capstyle projecting -alpha 12 -end s -width 1 -joinstyle miter -autoshape 1
item edge20 -slatetype Net -toaspect terminal -start w -tags edge -fill black -shape line -state 0 -coords {616.0 55.0 30.0 55.0 30.0 451.0 42.0 451.0} -smooth off -arrow none -beta 30 -fromaspect w -capstyle projecting -alpha 12 -end e -width 1 -joinstyle miter -autoshape 1
item vertex14 -slatetype IconFrame -anchor center -hsize 60 -text Gain -borderwidth 2 -color lightsteelblue -tags vertex -font -adobe-helvetica-medium-r-normal-*-*-120-75-75-*-*-iso8859-* -label Gain -textanchor center -shape point -graphics {
         polygon 70 20 70 80 30 50 -fill red -outline black
         line 10 50 30 50 -width 1 -fill black
         line 70 50 90 50 -width 1 -fill black
     } -coords {431.0 537.0} -relief raised -vsize 40 {
    item input -slatetype Terminal -tags input -style arrowtail -direction e -coords {100 50}
    item output -slatetype Terminal -tags output -style arrowout -direction w -coords {0 50}
}
item vertex15 -slatetype IconOval -anchor center -hsize 50 -text {} -fill white -tags vertex -font -adobe-helvetica-medium-r-normal-*-*-120-75-75-*-*-iso8859-* -label Add -textanchor center -shape point -graphics {
        line 20 50 80 50 -width 4
        line 50 20 50 80 -width 4
    } -coords {209.0 494.0} -width 1 -outline black -vsize 50 {
    item input-0 -slatetype Terminal -tags input -style arrowtail -direction w -coords {0 50}
    item input-1 -slatetype Terminal -tags input -style arrowtail -direction n -coords {50 0}
    item output -slatetype Terminal -tags output -style arrowout -direction e -coords {100 50}
}
item junction3 -slatetype Junction -coords {488 441.0} -tags junction
item edge21 -slatetype Net -coords {493.0 441.0 528.0 441.0} -start e -end e -fromaspect e -tags edge
item edge22 -toaspect terminal -slatetype Net -start s -tags edge -fill black -shape line -state 1 -coords {488.0 446.0 488.0 458.0 493.0 458.0 493.0 537.0 481.0 537.0} -smooth off -arrow none -beta 30 -fromaspect s -capstyle projecting -alpha 12 -end w -width 1 -joinstyle miter -autoshape 1
item edge23 -toaspect terminal -slatetype Net -start w -tags edge -fill black -shape line -state 1 -coords {381.0 537.0 152.0 537.0 152.0 494.0 164.0 494.0} -smooth off -arrow none -beta 30 -fromaspect terminal -capstyle projecting -alpha 12 -end e -width 1 -joinstyle miter -autoshape 1
item edge24 -toaspect terminal -slatetype Net -start e -tags edge -fill black -shape line -state 1 -coords {132.0 451.0 144.0 451.0 144.0 437.0 209.0 437.0 209.0 449.0} -smooth off -arrow none -beta 30 -fromaspect terminal -capstyle projecting -alpha 12 -end s -width 1 -joinstyle miter -autoshape 1
item edge25 -toaspect terminal -slatetype Net -start e -tags edge -fill black -shape line -state 1 -coords {254.0 494.0 305.0 494.0 305.0 441.0 356.0 441.0} -smooth off -arrow none -beta 30 -fromaspect terminal -capstyle projecting -alpha 12 -end e -width 1 -joinstyle miter -autoshape 1
item edge27 -slatetype Net -toaspect terminal -start w -tags edge -fill black -shape line -state 2 -coords {654.0 213.0 642.0 213.0 642.0 287.2 801.0 287.2} -smooth off -arrow none -beta 30 -fromaspect w -capstyle projecting -alpha 12 -end e -width 1 -joinstyle miter -autoshape 1
item vertex1 -slatetype IconFrame -anchor center -hsize 60 -text 2D-Plotter -borderwidth 2 -color lightblue -tags vertex -font -adobe-helvetica-medium-r-normal-*-*-120-75-75-*-*-iso8859-* -label 2D-Plotter -textanchor center -shape point -graphics {
        polygon 20 20 20 80 80 80 80 20 -fill gray -outline black
        line 21 39 79 39 -fill green -stipple gray25
        line 21 59 79 59 -fill green -stipple gray25
        line 30 22 30 79 -fill green
        line 50 22 50 79 -fill green 
        line 70 23 70 79 -fill green
	oval 1c 1c 2c 2c -outline red

    } -coords {851.0 294.0} -relief raised -vsize 40 {
    item inputX -fill red -slatetype Terminal -tags input -style arrowtail -direction w -coords {0 33}
    item inputY -fill red -slatetype Terminal -tags input -style arrowtail -direction w -coords {0 66}
}
item junction4 -slatetype Junction -coords {638.0 93} -tags junction
item edge9 -slatetype Net -coords {638.0 98.0 254.0 247.0} -start s -end s -fromaspect s -tags edge
item edge18 -toaspect terminal -slatetype Net -start e -tags edge -fill black -shape line -state 2 -coords {643.0 93.0 722.0 93.0 722.0 300.4 801.0 300.4} -smooth off -arrow none -beta 30 -fromaspect e -capstyle projecting -alpha 12 -end e -width 1 -joinstyle miter -autoshape 1
}
vertex vertex0 -label Const -master edit/cts/stars.tim#source.const -icon edit/cts/icons.tim#source.const -value 25 -class Const {
    port output -datatype float
}
vertex vertex2 -initialState 1.0 -label Integrator -master edit/cts/stars.tim#dynamic.integral -icon edit/cts/icons.tim#dynamic.integral -class Integrator {
    port input -datatype float
    port output -datatype float
}
vertex vertex3 -initialState 1.0 -label Integrator -master edit/cts/stars.tim#dynamic.integral -icon edit/cts/icons.tim#dynamic.integral -class Integrator {
    port input -datatype float
    port output -datatype float
}
vertex vertex4 -initialState 1.0 -label Integrator -master edit/cts/stars.tim#dynamic.integral -icon edit/cts/icons.tim#dynamic.integral -class Integrator {
    port input -datatype float
    port output -datatype float
}
vertex vertex5 -master edit/cts/stars.tim#arithmetic.multiply -label Multiply -icon edit/cts/icons.tim#arithmetic.multiply -class Multiply {
    port input -composite 1
    port output -composite 0
}
vertex vertex6 -master edit/cts/stars.tim#arithmetic.multiply -label Multiply -icon edit/cts/icons.tim#arithmetic.multiply -class Multiply {
    port input -composite 1
    port output -composite 0
}
vertex vertex7 -gain -1.0 -label Gain -master edit/cts/stars.tim#arithmetic.gain -icon edit/cts/icons.tim#arithmetic.gain -class Gain {
    port input -composite 0
    port output -composite 0
}
vertex vertex8 -gain -1.0 -label Gain -master edit/cts/stars.tim#arithmetic.gain -icon edit/cts/icons.tim#arithmetic.gain -class Gain {
    port input -composite 0
    port output -composite 0
}
vertex vertex9 -gain 10 -label Gain -master edit/cts/stars.tim#arithmetic.gain -icon edit/cts/icons.tim#arithmetic.gain -class Gain {
    port input -composite 0
    port output -composite 0
}
vertex vertex10 -gain -1.0 -label Gain -master edit/cts/stars.tim#arithmetic.gain -icon edit/cts/icons.tim#arithmetic.gain -class Gain {
    port input -composite 0
    port output -composite 0
}
vertex vertex11 -master edit/cts/stars.tim#arithmetic.add -label Add_2 -icon edit/cts/icons.tim#arithmetic.add -class Add {
    port input -composite 1
    port output -composite 0
}
vertex vertex12 -master edit/cts/stars.tim#arithmetic.add -label Add_2 -icon edit/cts/icons.tim#arithmetic.add -class Add {
    port input -composite 1
    port output -composite 0
}
vertex vertex13 -master edit/cts/stars.tim#arithmetic.add -label Add_2 -icon edit/cts/icons.tim#arithmetic.add -class Add {
    port input -composite 1
    port output -composite 0
}
vertex junction0
vertex junction1
vertex junction2
vertex vertex14 -gain -2.0 -master edit/cts/stars.tim#arithmetic.gain -label Gain -icon edit/cts/icons.tim#arithmetic.gain -class Gain {
    port input -composite 0
    port output -composite 0
}
vertex vertex15 -master edit/cts/stars.tim#arithmetic.add -label Add_2 -icon edit/cts/icons.tim#arithmetic.add -class Add {
    port input -composite 1
    port output -composite 0
}
vertex junction3
vertex vertex1 -YMax 30.0 -master edit/cts/stars.tim#sink.xyplot -XMin -20.0 -label 2D-Plotter -YMin -30.0 -icon edit/cts/icons.tim#sink.xyplot -XMax 25.0 -class XYPlot {
    port inputX -datatype float
    port inputY -datatype float
}
vertex junction4
edge edge0 -to junction0 -from {vertex2 output}
edge edge1 -to {vertex11 input-1} -from {vertex7 output}
edge edge2 -to junction2 -from {vertex3 output}
edge edge3 -to {vertex9 input} -from {vertex11 output}
edge edge4 -to {vertex2 input} -from {vertex9 output}
edge edge5 -to {vertex12 input-1} -from {vertex0 output}
edge edge6 -to junction3 -from {vertex4 output}
edge edge7 -to {vertex12 input-0} -from {vertex8 output}
edge edge8 -to {vertex5 input-0} -from {vertex12 output}
edge edge10 -from junction0 -to {vertex7 input}
edge edge11 -to junction4 -from junction0
edge edge12 -from junction1 -to {vertex11 input-0}
edge edge13 -to {vertex10 input} -from junction1
edge edge14 -to {vertex13 input-0} -from {vertex5 output}
edge edge15 -to {vertex13 input-1} -from {vertex10 output}
edge edge16 -to {vertex3 input} -from {vertex13 output}
edge edge17 -from junction2 -to junction1
edge edge19 -to {vertex6 input-1} -from junction2
edge edge20 -to {vertex6 input-0} -from junction0
edge edge21 -from junction3 -to {vertex8 input}
edge edge22 -to {vertex14 input} -from junction3
edge edge23 -to {vertex15 input-0} -from {vertex14 output}
edge edge24 -to {vertex15 input-1} -from {vertex6 output}
edge edge25 -to {vertex4 input} -from {vertex15 output}
edge edge27 -to {vertex1 inputX} -from junction2
edge edge9 -from junction4 -to {vertex5 input-1}
edge edge18 -to {vertex1 inputY} -from junction4
