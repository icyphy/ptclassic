#  -*- mode: cts -*-
#  -*- class: ::tycho::PortedGraph -*-
#  -*- mtime: Tue May 19 23:19:34  1998 -*-

annotation layout -data {#  -*- mode: tcl -*-
#  -*- class: ::tycho::Layout -*-
#  -*- mtime: Tue May 19 23:19:34  1998 -*-

item vertex0 -slatetype IconFrame -anchor center -hsize 60 -text SquareWave -borderwidth 2 -color lightgreen -tags {vertex subsel} -font -adobe-helvetica-medium-r-normal-*-*-120-75-75-*-*-iso8859-* -label SquareWave -textanchor center -shape point -graphics {
        polygon 20 30 20 70 40 70 40 30 -fill orange -outline ""
        polygon 60 30 60 70 80 70 80 30 -fill orange -outline ""
        line 10 70 20 70 20 30 40 30 40 70 60 70 60 30 80 30 80 70 90 70 -width 1 -fill black
    } -coords {532.0 36.0} -relief raised -vsize 40 {
    item output -tags output -slatetype Terminal -direction w -style arrowout -coords {0 50}
}
item vertex2 -slatetype IconFrame -anchor center -hsize 60 -text Integrator -borderwidth 2 -color blue -tags {vertex subsel} -font -adobe-helvetica-medium-r-normal-*-*-120-75-75-*-*-iso8859-* -label Integrator -textanchor center -shape point -graphics {
         polygon 30 25 30 70 25 75 20 70 15 80 25 85 35 75 35 30  40 25 45 30 50 25 40 15 -fill black -outline yellow
         text 70 55 -text dx -fill yellow 
     } -coords {197.0 112.0} -relief raised -vsize 40 {
    item input -slatetype Terminal -tags input -style arrowtail -direction w -coords {0 50}
    item output -slatetype Terminal -tags output -style arrowout -direction e -coords {100 50}
}
item vertex3 -slatetype IconFrame -anchor center -hsize 60 -text Integrator -borderwidth 2 -color blue -tags {vertex subsel} -font -adobe-helvetica-medium-r-normal-*-*-120-75-75-*-*-iso8859-* -label Integrator -textanchor center -shape point -graphics {
         polygon 30 25 30 70 25 75 20 70 15 80 25 85 35 75 35 30  40 25 45 30 50 25 40 15 -fill black -outline yellow
         text 70 55 -text dx -fill yellow 
     } -coords {356.0 112.0} -relief raised -vsize 40 {
    item input -slatetype Terminal -tags input -style arrowtail -direction w -coords {0 50}
    item output -slatetype Terminal -tags output -style arrowout -direction e -coords {100 50}
}
item vertex4 -slatetype IconOval -anchor center -hsize 50 -text {} -fill white -tags {vertex subsel} -font -adobe-helvetica-medium-r-normal-*-*-120-75-75-*-*-iso8859-* -label Add -textanchor center -shape point -graphics {
         line 20 50 80 50 -width 4
         line 50 20 50 80 -width 4
    } -coords {76.0 112.0} -outline black -width 1 -vsize 50 {
    item input-0 -slatetype Terminal -tags input -style arrowtail -direction w -coords {0 50}
    item input-1 -slatetype Terminal -tags input -style arrowtail -direction n -coords {50 0}
    item input-2 -slatetype Terminal -tags input -style arrowtail -direction s -coords {50 100}
    item output -slatetype Terminal -tags output -style arrowout -direction e -coords {100 50}
}
item vertex6 -slatetype IconFrame -anchor center -hsize 60 -text Gain -borderwidth 2 -color lightsteelblue -tags {vertex subsel} -font -adobe-helvetica-medium-r-normal-*-*-120-75-75-*-*-iso8859-* -label Gain -textanchor center -shape point -graphics {
         polygon 70 20 70 80 30 50 -fill red -outline black
         line 10 50 30 50 -width 1 -fill black
         line 70 50 90 50 -width 1 -fill black
     } -coords {198.0 186.0} -relief raised -vsize 40 {
    item input -slatetype Terminal -tags input -style arrowtail -direction e -coords {100 50}
    item output -slatetype Terminal -tags output -style arrowout -direction w -coords {0 50}
}
item vertex8 -slatetype IconFrame -anchor center -hsize 60 -text Gain -borderwidth 2 -color lightsteelblue -tags {vertex subsel} -font -adobe-helvetica-medium-r-normal-*-*-120-75-75-*-*-iso8859-* -label Gain -textanchor center -shape point -graphics {
         polygon 70 20 70 80 30 50 -fill red -outline black
         line 10 50 30 50 -width 1 -fill black
         line 70 50 90 50 -width 1 -fill black
     } -coords {358.0 245.0} -relief raised -vsize 40 {
    item input -slatetype Terminal -tags input -style arrowtail -direction e -coords {100 50}
    item output -slatetype Terminal -tags output -style arrowout -direction w -coords {0 50}
}
item edge2 -toaspect terminal -slatetype Net -start e -tags edge -fill black -shape line -state 0 -coords {121.0 112.0 147.0 112.0} -smooth off -arrow none -beta 30 -fromaspect terminal -capstyle projecting -alpha 12 -end e -width 1 -joinstyle miter -autoshape 1
item edge3 -slatetype Net -toaspect w -start e -tags edge -fill black -shape line -state 0 -coords {247.0 112.0 265.0 112.0} -smooth off -arrow none -beta 30 -fromaspect terminal -capstyle projecting -alpha 12 -end e -width 1 -joinstyle miter -autoshape 1
item edge4 -slatetype Net -toaspect w -start e -tags edge -fill black -shape line -state 0 -coords {406.0 112.0 432.0 112.0} -smooth off -arrow none -beta 30 -fromaspect terminal -capstyle projecting -alpha 12 -end e -width 1 -joinstyle miter -autoshape 1
item junction0 -fill black -size 5 -tags junction -slatetype Junction -outline black -width 1 -shape point -coords {437.0 112.0}
item edge6 -slatetype Net -toaspect terminal -start s -tags edge -fill black -shape line -state 0 -coords {437.0 117.0 437.0 245.0 408.0 245.0} -smooth off -arrow none -beta 30 -fromaspect s -capstyle projecting -alpha 12 -end w -width 1 -joinstyle miter -autoshape 1
item junction1 -slatetype Junction -tags {subsel junction} -size 5 -fill black -width 1 -outline black -shape point -coords {270.0 112.0}
item edge7 -slatetype Net -toaspect terminal -start e -tags edge -fill black -shape line -state 0 -coords {275.0 112.0 306.0 112.0} -smooth off -arrow none -beta 30 -fromaspect e -capstyle projecting -alpha 12 -end e -width 1 -joinstyle miter -autoshape 1
item edge8 -slatetype Net -toaspect terminal -start s -tags edge -fill black -shape line -state 0 -coords {270.0 117.0 270.0 186.0 248.0 186.0} -smooth off -arrow none -beta 30 -fromaspect s -capstyle projecting -alpha 12 -end w -width 1 -joinstyle miter -autoshape 1
item edge9 -toaspect terminal -slatetype Net -start w -tags edge -fill black -shape line -state 0 -coords {148.0 186.0 76.0 186.0 76.0 157.0} -smooth off -arrow none -beta 30 -fromaspect terminal -capstyle projecting -alpha 12 -end n -width 1 -joinstyle miter -autoshape 1
item edge10 -toaspect terminal -slatetype Net -start w -tags edge -fill black -shape line -state 1 -coords {308.0 245.0 19.0 245.0 19.0 112.0 31.0 112.0} -smooth off -arrow none -beta 30 -fromaspect terminal -capstyle projecting -alpha 12 -end e -width 1 -joinstyle miter -autoshape 1
item vertex5 -slatetype IconFrame -anchor center -hsize 60 -text Gain -borderwidth 2 -color lightsteelblue -tags {vertex sel} -font -adobe-helvetica-medium-r-normal-*-*-120-75-75-*-*-iso8859-* -label Gain -textanchor center -shape point -graphics {
         polygon 70 20 70 80 30 50 -fill red -outline black
         line 10 50 30 50 -width 1 -fill black
         line 70 50 90 50 -width 1 -fill black
     } -coords {204.0 36.0} -relief raised -vsize 40 {
    item input -slatetype Terminal -tags input -style arrowtail -direction e -coords {100 50}
    item output -slatetype Terminal -tags output -style arrowout -direction w -coords {0 50}
}
item edge0 -toaspect terminal -slatetype Net -start w -tags edge -fill black -shape line -state 0 -coords {154.0 36.0 76.0 36.0 76.0 67.0} -smooth off -arrow none -beta 30 -fromaspect terminal -capstyle projecting -alpha 12 -end s -width 1 -joinstyle miter -autoshape 1
item edge1 -slatetype Net -toaspect e -start w -tags edge -fill black -shape line -state 0 -coords {482.0 36.0 405.0 36.0} -smooth off -arrow none -beta 30 -fromaspect terminal -capstyle projecting -alpha 12 -end w -width 1 -joinstyle miter -autoshape 1
item junction2 -slatetype Junction -tags junction -size 5 -fill black -width 1 -outline black -shape point -coords {400.0 36.0}
item edge11 -slatetype Net -toaspect terminal -start w -tags edge -fill black -shape line -state 0 -coords {395.0 36.0 254.0 36.0} -smooth off -arrow none -beta 30 -fromaspect w -capstyle projecting -alpha 12 -end w -width 1 -joinstyle miter -autoshape 1
item edge13 -slatetype Net -toaspect terminal -start e -tags edge -fill black -shape line -state 2 -coords {442.0 112.0 464.0 112.0 464.0 112.4 486.0 112.4} -smooth off -arrow none -beta 30 -fromaspect e -capstyle projecting -alpha 12 -end e -width 1 -joinstyle miter -autoshape 1
item vertex1 -slatetype IconFrame -anchor center -hsize 60 -text Plotter -borderwidth 2 -color lightblue -tags vertex -font -adobe-helvetica-medium-r-normal-*-*-120-75-75-*-*-iso8859-* -label Plotter_2 -textanchor center -shape point -graphics {
        polygon 20 20 20 80 80 80 80 20 -fill gray -outline black
        line 21 39 79 39 -fill green -stipple gray25
        line 21 59 79 59 -fill green -stipple gray25
        line 30 22 30 79 -fill green
        line 50 22 50 79 -fill green 
        line 70 23 70 79 -fill green
	line 21 79 40 40 70 60 79 31 -smooth true -fill red
    } -coords {536.0 106.0} -relief raised -vsize 40 {
    item input-0 -fill red -slatetype Terminal -tags input -style arrowtail -direction w -coords {0 33}
    item input-1 -fill red -slatetype Terminal -tags input -style arrowtail -direction w -coords {0 66}
}
item edge5 -slatetype Net -toaspect terminal -start s -tags edge -fill black -shape line -state 0 -coords {400.0 41.0 400.0 99.2 486.0 99.2} -smooth off -arrow none -beta 30 -fromaspect s -capstyle projecting -alpha 12 -end e -width 1 -joinstyle miter -autoshape 1
}
vertex vertex0 -maxValue 0.5 -label SquareWave -master edit/cts/stars.tim#source.squarewave -icon edit/cts/icons.tim#source.squarewave -minValue -0.5 -class SquareWave -frequency 0.25 {
    port output -datatype float
}
vertex vertex2 -initialState 0.0 -master edit/cts/stars.tim#dynamic.integral -label Integrator -icon edit/cts/icons.tim#dynamic.integral -class Integrator {
    port input -datatype float
    port output -datatype float
}
vertex vertex3 -initialState 0.0 -master edit/cts/stars.tim#dynamic.integral -label Integrator -icon edit/cts/icons.tim#dynamic.integral -class Integrator {
    port input -datatype float
    port output -datatype float
}
vertex vertex4 -master edit/cts/stars.tim#arithmetic.add_3 -label Add_3 -icon edit/cts/icons.tim#arithmetic.add_3 -class Add {
    port input -composite 1
    port output -composite 0
}
vertex vertex6 -gain -25 -label Gain -master edit/cts/stars.tim#arithmetic.gain -icon edit/cts/icons.tim#arithmetic.gain -class Gain {
    port input -composite 0
    port output -composite 0
}
vertex vertex8 -gain -2500 -label Gain -master edit/cts/stars.tim#arithmetic.gain -icon edit/cts/icons.tim#arithmetic.gain -class Gain {
    port input -composite 0
    port output -composite 0
}
vertex junction0
vertex junction1
vertex vertex5 -gain 1000 -master edit/cts/stars.tim#arithmetic.gain -label Gain -icon edit/cts/icons.tim#arithmetic.gain -class Gain {
    port input -composite 0
    port output -composite 0
}
vertex junction2
vertex vertex1 -YMax 1.0 -master edit/cts/stars.tim#sink.plot_2 -label Plotter -YMin -1.0 -icon edit/cts/icons.tim#sink.plot_2 -class Plot {
    port input -composite 1 -datatype float
}
edge edge2 -to {vertex2 input} -from {vertex4 output}
edge edge3 -to junction1 -from {vertex2 output}
edge edge4 -to junction0 -from {vertex3 output}
edge edge6 -to {vertex8 input} -from junction0
edge edge7 -from junction1 -to {vertex3 input}
edge edge8 -to {vertex6 input} -from junction1
edge edge9 -to {vertex4 input-2} -from {vertex6 output}
edge edge10 -to {vertex4 input-0} -from {vertex8 output}
edge edge0 -to {vertex4 input-1} -from {vertex5 output}
edge edge1 -to junction2 -from {vertex0 output}
edge edge11 -from junction2 -to {vertex5 input}
edge edge13 -to {vertex1 input-1} -from junction0
edge edge5 -to {vertex1 input-0} -from junction2
