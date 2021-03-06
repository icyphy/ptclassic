#  -*- mode: cts -*-
#  -*- class: ::tycho::PortedGraph -*-
#  -*- mtime: Wed May 20 18:21:58 PDT 1998 -*-

annotation layout -data {#  -*- mode: tcl -*-
#  -*- class: ::tycho::Layout -*-
#  -*- mtime: Wed May 20 18:21:58 PDT 1998 -*-

item vertex0 -slatetype IconFrame -anchor center -hsize 60 -text Const -borderwidth 2 -color lightgreen -tags {vertex sel} -font -adobe-helvetica-medium-r-normal-*-*-120-75-75-*-*-iso8859-* -label Const -textanchor center -shape point -graphics {
        polygon 30 30 30 80 80 80 80 30 -fill orange -outline ""
        line 10 80 80 80 -width 1 -fill black
        line 30 80 30 30 -width 1 -fill black
        line 30 30 80 30 -width 1 -fill black
    } -coords {39.0 240.0} -relief raised -vsize 40 {
    item output -tags output -slatetype Terminal -direction e -style arrowout -coords {100 50}
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
     } -coords {460.0 304.0} -relief raised -vsize 40 {
    item input -slatetype Terminal -tags input -style arrowtail -direction w -coords {0 50}
    item output -slatetype Terminal -tags output -style arrowout -direction e -coords {100 50}
}
item vertex4 -slatetype IconFrame -anchor center -hsize 60 -text Integrator -borderwidth 2 -color blue -tags vertex -font -adobe-helvetica-medium-r-normal-*-*-120-75-75-*-*-iso8859-* -label Integrator -textanchor center -shape point -graphics {
         polygon 30 25 30 70 25 75 20 70 15 80 25 85 35 75 35 30  40 25 45 30 50 25 40 15 -fill black -outline yellow
         text 70 55 -text dx -fill yellow 
     } -coords {290.0 458.0} -relief raised -vsize 40 {
    item input -slatetype Terminal -tags input -style arrowtail -direction w -coords {0 50}
    item output -slatetype Terminal -tags output -style arrowout -direction e -coords {100 50}
}
item vertex5 -slatetype IconOval -anchor center -hsize 50 -text {} -fill white -tags vertex -font -adobe-helvetica-medium-r-normal-*-*-120-75-75-*-*-iso8859-* -label Multiply -textanchor center -shape point -graphics {
        line 20 20 80 80 -width 4
        line 20 80 80 20 -width 4
    } -coords {221.0 304.0} -outline black -width 1 -vsize 50 {
    item input-0 -slatetype Terminal -tags input -style arrowtail -direction w -coords {0 50}
    item input-1 -slatetype Terminal -tags input -style arrowtail -direction n -coords {50 0}
    item output -slatetype Terminal -tags output -style arrowout -direction e -coords {100 50}
}
item vertex6 -slatetype IconOval -anchor center -hsize 50 -text {} -fill white -tags vertex -font -adobe-helvetica-medium-r-normal-*-*-120-75-75-*-*-iso8859-* -label Multiply -textanchor center -shape point -graphics {
        line 20 20 80 80 -width 4
        line 20 80 80 20 -width 4
    } -coords {509.0 371.0} -outline black -width 1 -vsize 50 {
    item input-0 -tags input -slatetype Terminal -direction e -style arrowtail -coords {100 50}
    item input-1 -tags input -slatetype Terminal -direction n -style arrowtail -coords {50 0}
    item output -tags output -slatetype Terminal -direction w -style arrowout -coords {0 50}
}
item vertex7 -slatetype IconFrame -anchor center -hsize 60 -text Gain -borderwidth 2 -color lightsteelblue -tags vertex -font -adobe-helvetica-medium-r-normal-*-*-120-75-75-*-*-iso8859-* -label Gain -textanchor center -shape point -graphics {
         polygon 70 20 70 80 30 50 -fill red -outline black
         line 10 50 30 50 -width 1 -fill black
         line 70 50 90 50 -width 1 -fill black
     } -coords {255.0 21.0} -relief raised -vsize 40 {
    item input -slatetype Terminal -tags input -style arrowtail -direction e -coords {100 50}
    item output -slatetype Terminal -tags output -style arrowout -direction w -coords {0 50}
}
item vertex10 -slatetype IconFrame -anchor center -hsize 60 -text Gain -borderwidth 2 -color lightsteelblue -tags vertex -font -adobe-helvetica-medium-r-normal-*-*-120-75-75-*-*-iso8859-* -label Gain -textanchor center -shape point -graphics {
         polygon 70 20 70 80 30 50 -fill red -outline black
         line 10 50 30 50 -width 1 -fill black
         line 70 50 90 50 -width 1 -fill black
     } -coords {407.0 215.0} -relief raised -vsize 40 {
    item input -slatetype Terminal -tags input -style arrowtail -direction e -coords {100 50}
    item output -slatetype Terminal -tags output -style arrowout -direction w -coords {0 50}
}
item vertex11 -slatetype IconOval -anchor center -hsize 50 -text {} -fill white -tags vertex -font -adobe-helvetica-medium-r-normal-*-*-120-75-75-*-*-iso8859-* -label Add -textanchor center -shape point -graphics {
        line 20 50 80 50 -width 4
        line 50 20 50 80 -width 4
    } -coords {127.0 98.0} -outline black -width 1 -vsize 50 {
    item input-0 -slatetype Terminal -tags input -style arrowtail -direction w -coords {0 50}
    item input-1 -slatetype Terminal -tags input -style arrowtail -direction n -coords {50 0}
    item output -slatetype Terminal -tags output -style arrowout -direction e -coords {100 50}
}
item vertex12 -slatetype IconOval -anchor center -hsize 50 -text {} -fill white -tags vertex -font -adobe-helvetica-medium-r-normal-*-*-120-75-75-*-*-iso8859-* -label Add -textanchor center -shape point -graphics {
        line 20 50 80 50 -width 4
        line 50 20 50 80 -width 4
    } -coords {106.0 304.0} -width 1 -outline black -vsize 50 {
    item input-0 -slatetype Terminal -tags input -style arrowtail -direction w -coords {0 50}
    item input-1 -slatetype Terminal -tags input -style arrowtail -direction n -coords {50 0}
    item output -slatetype Terminal -tags output -style arrowout -direction e -coords {100 50}
}
item vertex13 -slatetype IconOval -anchor center -hsize 50 -text {} -fill white -tags vertex -font -adobe-helvetica-medium-r-normal-*-*-120-75-75-*-*-iso8859-* -label Add -textanchor center -shape point -graphics {
        line 20 50 80 50 -width 4
        line 50 20 50 80 -width 4
    } -coords {336.0 304.0} -width 1 -outline black -vsize 50 {
    item input-0 -slatetype Terminal -tags input -style arrowtail -direction w -coords {0 50}
    item input-1 -slatetype Terminal -tags input -style arrowtail -direction n -coords {50 0}
    item output -slatetype Terminal -tags output -style arrowout -direction e -coords {100 50}
}
item edge0 -slatetype Net -toaspect s -start e -tags edge -fill black -shape line -state 0 -coords {475.0 98.0 488.0 98.0 488.0 72.0} -smooth off -arrow none -beta 30 -fromaspect terminal -capstyle projecting -alpha 12 -end n -width 1 -joinstyle miter -autoshape 1
item edge1 -toaspect terminal -slatetype Net -start w -tags edge -fill black -shape line -state 0 -coords {205.0 21.0 127.0 21.0 127.0 53.0} -smooth off -arrow none -beta 30 -fromaspect terminal -capstyle projecting -alpha 12 -end s -width 1 -joinstyle miter -autoshape 1
item edge3 -toaspect terminal -slatetype Net -start e -tags edge -fill black -shape line -state 0 -coords {172.0 98.0 208.0 98.0} -smooth off -arrow none -beta 30 -fromaspect terminal -capstyle projecting -alpha 12 -end e -width 1 -joinstyle miter -autoshape 1
item edge4 -toaspect terminal -slatetype Net -start e -tags edge -fill black -shape line -state 0 -coords {308.0 98.0 375.0 98.0} -smooth off -arrow none -beta 30 -fromaspect terminal -capstyle projecting -alpha 12 -end e -width 1 -joinstyle miter -autoshape 1
item edge5 -toaspect terminal -slatetype Net -start e -tags edge -fill black -shape line -state 0 -coords {89.0 240.0 106.0 240.0 106.0 259.0} -smooth off -arrow none -beta 30 -fromaspect terminal -capstyle projecting -alpha 12 -end s -width 1 -joinstyle miter -autoshape 1
item edge6 -toaspect w -slatetype Net -start e -tags edge -fill black -shape line -state 0 -coords {340.0 458.0 379.0 458.0} -smooth off -arrow none -beta 30 -fromaspect terminal -capstyle projecting -alpha 12 -end e -width 1 -joinstyle miter -autoshape 1
item edge8 -toaspect terminal -slatetype Net -start e -tags edge -fill black -shape line -state 0 -coords {151.0 304.0 176.0 304.0} -smooth off -arrow none -beta 30 -fromaspect terminal -capstyle projecting -alpha 12 -end e -width 1 -joinstyle miter -autoshape 1
item junction0 -slatetype Junction -tags junction -size 5 -fill black -width 1 -outline black -shape point -coords {488.0 67.0}
item edge10 -slatetype Net -toaspect terminal -start n -tags edge -fill black -shape line -state 0 -coords {488.0 62.0 488.0 21.0 305.0 21.0} -smooth off -arrow none -beta 30 -fromaspect n -capstyle projecting -alpha 12 -end w -width 1 -joinstyle miter -autoshape 1
item junction1 -slatetype Junction -tags junction -size 5 -fill black -width 1 -outline black -shape point -coords {566.0 215.0}
item edge12 -slatetype Net -toaspect e -start n -tags edge -fill black -shape line -state 0 -coords {566.0 210.0 566.0 182.0 255.0 182.0} -smooth off -arrow none -beta 30 -fromaspect n -capstyle projecting -alpha 12 -end w -width 1 -joinstyle miter -autoshape 1
item edge13 -toaspect terminal -slatetype Net -start w -tags edge -fill black -shape line -state 0 -coords {561.0 215.0 457.0 215.0} -smooth off -arrow none -beta 30 -fromaspect w -capstyle projecting -alpha 12 -end w -width 1 -joinstyle miter -autoshape 1
item edge14 -toaspect terminal -slatetype Net -start e -tags edge -fill black -shape line -state 0 -coords {266.0 304.0 291.0 304.0} -smooth off -arrow none -beta 30 -fromaspect terminal -capstyle projecting -alpha 12 -end e -width 1 -joinstyle miter -autoshape 1
item edge15 -toaspect terminal -slatetype Net -start w -tags edge -fill black -shape line -state 0 -coords {357.0 215.0 336.0 215.0 336.0 259.0} -smooth off -arrow none -beta 30 -fromaspect terminal -capstyle projecting -alpha 12 -end s -width 1 -joinstyle miter -autoshape 1
item edge16 -toaspect terminal -slatetype Net -start e -tags edge -fill black -shape line -state 0 -coords {381.0 304.0 410.0 304.0} -smooth off -arrow none -beta 30 -fromaspect terminal -capstyle projecting -alpha 12 -end e -width 1 -joinstyle miter -autoshape 1
item edge17 -slatetype Net -toaspect w -start e -tags edge -fill black -shape line -state 0 -coords {510.0 304.0 561.0 304.0} -smooth off -arrow none -beta 30 -fromaspect terminal -capstyle projecting -alpha 12 -end e -width 1 -joinstyle miter -autoshape 1
item vertex14 -slatetype IconFrame -anchor center -hsize 60 -text Gain -borderwidth 2 -color lightsteelblue -tags vertex -font -adobe-helvetica-medium-r-normal-*-*-120-75-75-*-*-iso8859-* -label Gain -textanchor center -shape point -graphics {
         polygon 70 20 70 80 30 50 -fill red -outline black
         line 10 50 30 50 -width 1 -fill black
         line 70 50 90 50 -width 1 -fill black
     } -coords {293.0 401.0} -relief raised -vsize 40 {
    item input -slatetype Terminal -tags input -style arrowtail -direction e -coords {100 50}
    item output -slatetype Terminal -tags output -style arrowout -direction w -coords {0 50}
}
item vertex15 -slatetype IconOval -anchor center -hsize 50 -text {} -fill white -tags vertex -font -adobe-helvetica-medium-r-normal-*-*-120-75-75-*-*-iso8859-* -label Add -textanchor center -shape point -graphics {
        line 20 50 80 50 -width 4
        line 50 20 50 80 -width 4
    } -coords {129.0 458.0} -outline black -width 1 -vsize 50 {
    item input-0 -slatetype Terminal -tags input -style arrowtail -direction w -coords {0 50}
    item input-1 -slatetype Terminal -tags input -style arrowtail -direction n -coords {50 0}
    item output -slatetype Terminal -tags output -style arrowout -direction e -coords {100 50}
}
item junction3 -slatetype Junction -tags junction -size 5 -fill black -width 1 -outline black -shape point -coords {384.0 458.0}
item edge21 -slatetype Net -toaspect terminal -start s -tags edge -fill black -shape line -state 0 -coords {384.0 463.0 384.0 521.0 288.0 521.0} -smooth off -arrow none -beta 30 -fromaspect s -capstyle projecting -alpha 12 -end w -width 1 -joinstyle miter -autoshape 1
item edge22 -toaspect terminal -slatetype Net -start n -tags edge -fill black -shape line -state 0 -coords {384.0 453.0 384.0 401.0 343.0 401.0} -smooth off -arrow none -beta 30 -fromaspect n -capstyle projecting -alpha 12 -end w -width 1 -joinstyle miter -autoshape 1
item edge23 -toaspect terminal -slatetype Net -start w -tags edge -fill black -shape line -state 1 -coords {243.0 401.0 231.0 401.0 231.0 401.0 129.0 401.0 129.0 413.0} -smooth off -arrow none -beta 30 -fromaspect terminal -capstyle projecting -alpha 12 -end s -width 1 -joinstyle miter -autoshape 1
item edge24 -toaspect terminal -slatetype Net -start w -tags edge -fill black -shape line -state 0 -coords {464.0 371.0 72.0 371.0 72.0 458.0 84.0 458.0} -smooth off -arrow none -beta 30 -fromaspect terminal -capstyle projecting -alpha 12 -end e -width 1 -joinstyle miter -autoshape 1
item edge25 -toaspect terminal -slatetype Net -start e -tags edge -fill black -shape line -state 0 -coords {174.0 458.0 240.0 458.0} -smooth off -arrow none -beta 30 -fromaspect terminal -capstyle projecting -alpha 12 -end e -width 1 -joinstyle miter -autoshape 1
item vertex1 -slatetype IconFrame -anchor center -hsize 60 -text 2D-Plotter -borderwidth 2 -color lightblue -tags vertex -font -adobe-helvetica-medium-r-normal-*-*-120-75-75-*-*-iso8859-* -label 2D-Plotter -textanchor center -shape point -graphics {
        polygon 20 20 20 80 80 80 80 20 -fill gray -outline black
        line 21 39 79 39 -fill green -stipple gray25
        line 21 59 79 59 -fill green -stipple gray25
        line 30 22 30 79 -fill green
        line 50 22 50 79 -fill green 
        line 70 23 70 79 -fill green
	oval 1c 1c 2c 2c -outline red

    } -coords {666.0 147.0} -relief raised -vsize 40 {
    item inputX -fill red -slatetype Terminal -tags input -style arrowtail -direction w -coords {0 33}
    item inputY -fill red -slatetype Terminal -tags input -style arrowtail -direction w -coords {0 66}
}
item edge9 -slatetype Net -toaspect n -start e -tags edge -fill black -shape line -state 0 -coords {493.0 67.0 509.0 67.0 509.0 148.0} -smooth off -arrow none -beta 30 -fromaspect e -capstyle projecting -alpha 12 -end s -width 1 -joinstyle miter -autoshape 1
item junction4 -slatetype Junction -tags junction -size 5 -fill black -width 1 -outline black -shape point -coords {509.0 153.0}
item edge11 -slatetype Net -toaspect n -start s -tags edge -fill black -shape line -state 0 -coords {509.0 158.0 509.0 241.5} -smooth off -arrow none -beta 30 -fromaspect s -capstyle projecting -alpha 12 -end s -width 1 -joinstyle miter -autoshape 1
item edge26 -slatetype Net -toaspect terminal -start e -tags edge -fill black -shape line -state 2 -coords {514.0 153.0 565.0 153.0 565.0 153.4 616.0 153.4} -smooth off -arrow none -beta 30 -fromaspect e -capstyle projecting -alpha 12 -end e -width 1 -joinstyle miter -autoshape 1
item vertex16 -slatetype IconFrame -anchor center -hsize 60 -text Gain -borderwidth 2 -color lightsteelblue -tags vertex -font -adobe-helvetica-medium-r-normal-*-*-120-75-75-*-*-iso8859-* -label Gain -textanchor center -shape point -graphics {
         polygon 70 20 70 80 30 50 -fill red -outline black
         line 10 50 30 50 -width 1 -fill black
         line 70 50 90 50 -width 1 -fill black
     } -coords {238.0 521.0} -relief raised -vsize 40 {
    item input -slatetype Terminal -tags input -style arrowtail -direction e -coords {100 50}
    item output -slatetype Terminal -tags output -style arrowout -direction w -coords {0 50}
}
item edge7 -toaspect terminal -slatetype Net -start w -tags edge -fill black -shape line -state 1 -coords {188.0 521.0 49.0 521.0 49.0 304.0 61.0 304.0} -smooth off -arrow none -beta 30 -fromaspect terminal -capstyle projecting -alpha 12 -end e -width 1 -joinstyle miter -autoshape 1
item vertex8 -slatetype IconFrame -anchor center -hsize 60 -text Gain -borderwidth 2 -color lightsteelblue -tags vertex -font -adobe-helvetica-medium-r-normal-*-*-120-75-75-*-*-iso8859-* -label Gain -textanchor center -shape point -graphics {
         polygon 30 20 30 80 70 50 -fill red -outline black
         line 10 50 30 50 -width 1 -fill black
         line 70 50 90 50 -width 1 -fill black
     } -coords {258.0 98.0} -relief raised -vsize 40 {
    item input -slatetype Terminal -tags input -style arrowtail -direction w -coords {0 50}
    item output -slatetype Terminal -tags output -style arrowout -direction e -coords {100 50}
}
item junction7 -fill black -size 5 -tags junction -slatetype Junction -outline black -width 1 -shape point -coords {509.0 246.5}
item edge31 -slatetype Net -toaspect terminal -start w -tags edge -fill black -shape line -state 0 -coords {504.0 246.5 221.0 246.5 221.0 259.0} -smooth off -arrow none -beta 30 -fromaspect w -capstyle projecting -alpha 12 -end s -width 1 -joinstyle miter -autoshape 1
item edge32 -toaspect terminal -slatetype Net -start s -tags edge -fill black -shape line -state 0 -coords {509.0 251.5 509.0 326.0} -smooth off -arrow none -beta 30 -fromaspect s -capstyle projecting -alpha 12 -end s -width 1 -joinstyle miter -autoshape 1
item junction9 -fill black -size 5 -tags junction -slatetype Junction -outline black -width 1 -shape point -coords {566.0 304.0}
item edge35 -slatetype Net -toaspect s -start n -tags edge -fill black -shape line -state 0 -coords {566.0 299.0 566.0 220.0} -smooth off -arrow none -beta 30 -fromaspect n -capstyle projecting -alpha 12 -end n -width 1 -joinstyle miter -autoshape 1
item edge36 -toaspect terminal -slatetype Net -start s -tags edge -fill black -shape line -state 0 -coords {566.0 309.0 566.0 371.0 554.0 371.0} -smooth off -arrow none -beta 30 -fromaspect s -capstyle projecting -alpha 12 -end w -width 1 -joinstyle miter -autoshape 1
item edge37 -slatetype Net -toaspect terminal -start e -tags edge -fill black -shape line -state 1 -coords {571.0 215.0 593.5 215.0 593.5 140.2 616.0 140.2} -smooth off -arrow none -beta 30 -fromaspect e -capstyle projecting -alpha 12 -end e -width 1 -joinstyle miter -autoshape 1
item junction10 -fill black -size 5 -tags {junction sel} -slatetype Junction -outline black -width 1 -shape point -coords {250.0 182.0}
item edge38 -slatetype Net -toaspect terminal -start w -tags edge -fill black -shape line -state 1 -coords {245.0 182.0 70.0 182.0 70.0 98.0 82.0 98.0} -smooth off -arrow none -beta 30 -fromaspect w -capstyle projecting -alpha 12 -end e -width 1 -joinstyle miter -autoshape 1
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
vertex vertex16 -gain -1.0 -label Gain -master edit/cts/stars.tim#arithmetic.gain -icon edit/cts/icons.tim#arithmetic.gain -class Gain {
    port input -composite 0
    port output -composite 0
}
vertex vertex8 -gain 10 -label Gain_Right -master edit/cts/stars.tim#arithmetic.gain_right -icon edit/cts/icons.tim#arithmetic.gain_right -class Gain {
    port input -composite 0
    port output -composite 0
}
vertex junction7
vertex junction9
vertex junction10
edge edge0 -to junction0 -from {vertex2 output}
edge edge1 -to {vertex11 input-1} -from {vertex7 output}
edge edge3 -to {vertex8 input} -from {vertex11 output}
edge edge4 -to {vertex2 input} -from {vertex8 output}
edge edge5 -to {vertex12 input-1} -from {vertex0 output}
edge edge6 -to junction3 -from {vertex4 output}
edge edge8 -to {vertex5 input-0} -from {vertex12 output}
edge edge10 -from junction0 -to {vertex7 input}
edge edge12 -to junction10 -from junction1
edge edge13 -to {vertex10 input} -from junction1
edge edge14 -to {vertex13 input-0} -from {vertex5 output}
edge edge15 -to {vertex13 input-1} -from {vertex10 output}
edge edge16 -to {vertex3 input} -from {vertex13 output}
edge edge17 -to junction9 -from {vertex3 output}
edge edge21 -to {vertex16 input} -from junction3
edge edge22 -to {vertex14 input} -from junction3
edge edge23 -to {vertex15 input-1} -from {vertex14 output}
edge edge24 -to {vertex15 input-0} -from {vertex6 output}
edge edge25 -to {vertex4 input} -from {vertex15 output}
edge edge9 -to junction4 -from junction0
edge edge11 -to junction7 -from junction4
edge edge26 -to {vertex1 inputY} -from junction4
edge edge7 -to {vertex12 input-0} -from {vertex16 output}
edge edge31 -from junction7 -to {vertex5 input-1}
edge edge32 -to {vertex6 input-1} -from junction7
edge edge35 -to junction1 -from junction9
edge edge36 -to {vertex6 input-0} -from junction9
edge edge37 -to {vertex1 inputX} -from junction1
edge edge38 -from junction10 -to {vertex11 input-0}
