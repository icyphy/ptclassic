#  -*- mode: cts -*-
#  -*- class: ::tycho::PortedGraph -*-
#  -*- mtime: Sun Sep 13 22:38:48 Pacific Daylight Time 1998 -*-

edge edge0 -to junction2 -from {vertex0 output}
edge edge1 -to {vertex2 input-1} -from {vertex5 output}
edge edge2 -to {vertex3 input} -from {vertex2 output}
edge edge3 -to junction0 -from {vertex3 output}
edge edge4 -to junction1 -from {vertex4 output}
edge edge5 -to {vertex1 input-2} -from {vertex9 output}
edge edge6 -from junction0 -to {vertex4 input}
edge edge7 -to {vertex8 input} -from junction0
edge edge8 -to {vertex2 input-2} -from {vertex8 output}
edge edge9 -from junction1 -to {vertex9 input}
edge edge10 -to {vertex7 input} -from junction1
edge edge11 -to {vertex2 input-0} -from {vertex7 output}
edge edge12 -from junction2 -to {vertex5 input}
edge edge13 -to {vertex1 input-0} -from junction2
edge edge14 -to {vertex1 input-1} -from junction1
vertex vertex0 -maxValue 5 -label SquareWave -master edit/cts/stars.tim#source.squarewave -icon edit/cts/icons.tim#source.squarewave -minValue -5 -class SquareWave -frequency 0.5 {
port output -datatype float
}
vertex vertex2 -master edit/cts/stars.tim#arithmetic.add_3 -label Add_3 -icon edit/cts/icons.tim#arithmetic.add_3 -class Add {
port input -composite 1
port output -composite 0
}
vertex vertex3 -initialState 0.0 -master edit/cts/stars.tim#dynamic.integral -label Integrator -icon edit/cts/icons.tim#dynamic.integral -class Integrator {
port input -datatype float
port output -datatype float
}
vertex vertex4 -initialState 0.0 -master edit/cts/stars.tim#dynamic.integral -label Integrator -icon edit/cts/icons.tim#dynamic.integral -class Integrator {
port input -datatype float
port output -datatype float
}
vertex vertex5 -gain 1000 -label Gain -master edit/cts/stars.tim#arithmetic.gain -icon edit/cts/icons.tim#arithmetic.gain -class Gain {
port input -composite 0
port output -composite 0
}
vertex vertex7 -gain -2500 -label Gain -master edit/cts/stars.tim#arithmetic.gain -icon edit/cts/icons.tim#arithmetic.gain -class Gain {
port input -composite 0
port output -composite 0
}
vertex vertex8 -gain -25 -label Gain -master edit/cts/stars.tim#arithmetic.gain -icon edit/cts/icons.tim#arithmetic.gain -class Gain {
port input -composite 0
port output -composite 0
}
vertex vertex9 -master edit/cts/stars.tim#function.lookup -domain {-5 -4 -3 -2 -1 0 1 2 3 4 5} -label LookupTable -range {-0.999 -0.9993 -0.9951 -0.9640 -0.7616 0 0.7616 0.9640 0.9951 0.9993 0.9999} -icon edit/cts/icons.tim#function.lookup -class LookupTable {
port input -datatype float
port output -datatype float
}
vertex junction0
vertex junction1
vertex junction2
vertex vertex1 -YMax 5 -master edit/cts/stars.tim#sink.plot_3 -label Plotter_3 -YMin -5 -icon edit/cts/icons.tim#sink.plot_3 -class Plot {
port input -composite 1 -datatype float
}
annotation layout -data {#  -*- mode: tcl -*-
#  -*- class: ::tycho::Layout -*-
#  -*- mtime: Sun Sep 13 22:38:48 Pacific Daylight Time 1998 -*-

item vertex0 -slatetype IconFrame -anchor center -hsize 60 -text SquareWave -borderwidth 2 -color lightgreen -tags vertex -font -adobe-helvetica-medium-r-normal-*-*-120-75-75-*-*-iso8859-* -label SquareWave -textanchor center -shape point -graphics {
        polygon 20 30 20 70 40 70 40 30 -fill orange -outline ""
        polygon 60 30 60 70 80 70 80 30 -fill orange -outline ""
        line 10 70 20 70 20 30 40 30 40 70 60 70 60 30 80 30 80 70 90 70 -width 1 -fill black
    } -coords {479.0 54.0} -relief raised -vsize 40 {
item output -tags output -slatetype Terminal -direction w -style arrowout -coords {0 50}
}
item vertex2 -slatetype IconOval -anchor center -hsize 50 -text {} -fill white -tags vertex -font -adobe-helvetica-medium-r-normal-*-*-120-75-75-*-*-iso8859-* -label Add -textanchor center -shape point -graphics {
         line 20 50 80 50 -width 4
         line 50 20 50 80 -width 4
    } -coords {65.0 170.0} -outline black -width 1 -vsize 50 {
item input-0 -slatetype Terminal -tags input -style arrowtail -direction w -coords {0 50}
item input-1 -slatetype Terminal -tags input -style arrowtail -direction n -coords {50 0}
item input-2 -slatetype Terminal -tags input -style arrowtail -direction s -coords {50 100}
item output -slatetype Terminal -tags output -style arrowout -direction e -coords {100 50}
}
item vertex3 -slatetype IconFrame -anchor center -hsize 60 -text Integrator -borderwidth 2 -color blue -tags vertex -font -adobe-helvetica-medium-r-normal-*-*-120-75-75-*-*-iso8859-* -label Integrator -textanchor center -shape point -graphics {
         polygon 30 25 30 70 25 75 20 70 15 80 25 85 35 75 35 30  40 25 45 30 50 25 40 15 -fill black -outline yellow
         text 70 55 -text dx -fill yellow 
     } -coords {179.0 170.0} -relief raised -vsize 40 {
item input -slatetype Terminal -tags input -style arrowtail -direction w -coords {0 50}
item output -slatetype Terminal -tags output -style arrowout -direction e -coords {100 50}
}
item vertex4 -slatetype IconFrame -anchor center -hsize 60 -text Integrator -borderwidth 2 -color blue -tags vertex -font -adobe-helvetica-medium-r-normal-*-*-120-75-75-*-*-iso8859-* -label Integrator -textanchor center -shape point -graphics {
         polygon 30 25 30 70 25 75 20 70 15 80 25 85 35 75 35 30  40 25 45 30 50 25 40 15 -fill black -outline yellow
         text 70 55 -text dx -fill yellow 
     } -coords {326.0 170.0} -relief raised -vsize 40 {
item input -slatetype Terminal -tags input -style arrowtail -direction w -coords {0 50}
item output -slatetype Terminal -tags output -style arrowout -direction e -coords {100 50}
}
item vertex5 -slatetype IconFrame -anchor center -hsize 60 -text Gain -borderwidth 2 -color lightsteelblue -tags vertex -font -adobe-helvetica-medium-r-normal-*-*-120-75-75-*-*-iso8859-* -label Gain -textanchor center -shape point -graphics {
         polygon 70 20 70 80 30 50 -fill red -outline black
         line 10 50 30 50 -width 1 -fill black
         line 70 50 90 50 -width 1 -fill black
     } -coords {226.0 54.0} -relief raised -vsize 40 {
item input -slatetype Terminal -tags input -style arrowtail -direction e -coords {100 50}
item output -slatetype Terminal -tags output -style arrowout -direction w -coords {0 50}
}
item vertex7 -slatetype IconFrame -anchor center -hsize 60 -text Gain -borderwidth 2 -color lightsteelblue -tags vertex -font -adobe-helvetica-medium-r-normal-*-*-120-75-75-*-*-iso8859-* -label Gain -textanchor center -shape point -graphics {
         polygon 70 20 70 80 30 50 -fill red -outline black
         line 10 50 30 50 -width 1 -fill black
         line 70 50 90 50 -width 1 -fill black
     } -coords {306.0 325.0} -relief raised -vsize 40 {
item input -slatetype Terminal -tags input -style arrowtail -direction e -coords {100 50}
item output -slatetype Terminal -tags output -style arrowout -direction w -coords {0 50}
}
item vertex8 -slatetype IconFrame -anchor center -hsize 60 -text Gain -borderwidth 2 -color lightsteelblue -tags vertex -font -adobe-helvetica-medium-r-normal-*-*-120-75-75-*-*-iso8859-* -label Gain -textanchor center -shape point -graphics {
         polygon 70 20 70 80 30 50 -fill red -outline black
         line 10 50 30 50 -width 1 -fill black
         line 70 50 90 50 -width 1 -fill black
     } -coords {176.0 262.0} -relief raised -vsize 40 {
item input -slatetype Terminal -tags input -style arrowtail -direction e -coords {100 50}
item output -slatetype Terminal -tags output -style arrowout -direction w -coords {0 50}
}
item edge0 -toaspect e -slatetype Net -start w -tags edge -fill black -shape line -state 0 -coords {429.0 54.0 402.0 54.0} -smooth off -arrow none -beta 30 -fromaspect terminal -capstyle projecting -alpha 12 -end w -width 1 -joinstyle miter -autoshape 1
item edge1 -toaspect terminal -slatetype Net -start w -tags edge -fill black -shape line -state 0 -coords {176.0 54.0 65.0 54.0 65.0 125.0} -smooth off -arrow none -beta 30 -fromaspect terminal -capstyle projecting -alpha 12 -end s -width 1 -joinstyle miter -autoshape 1
item edge2 -toaspect terminal -slatetype Net -start e -tags edge -fill black -shape line -state 0 -coords {110.0 170.0 129.0 170.0} -smooth off -arrow none -beta 30 -fromaspect terminal -capstyle projecting -alpha 12 -end e -width 1 -joinstyle miter -autoshape 1
item edge3 -slatetype Net -toaspect w -start e -tags edge -fill black -shape line -state 0 -coords {229.0 170.0 249.0 170.0} -smooth off -arrow none -beta 30 -fromaspect terminal -capstyle projecting -alpha 12 -end e -width 1 -joinstyle miter -autoshape 1
item vertex9 -slatetype IconFrame -anchor center -hsize 60 -text LookupTable -borderwidth 2 -color darkgreen -tags vertex -font -adobe-helvetica-medium-r-normal-*-*-120-75-75-*-*-iso8859-* -label LookupTable -textanchor center -shape point -graphics {
         polygon  30 10 70 10 70 85 30 85 -fill lightsteelblue -outline black
         line 30 25 70 25 -width 1 -fill black
         line 30 40 70 40 -width 1 -fill black
         line 30 55 70 55 -width 1 -fill black
         line 30 70 70 70 -width 1 -fill black
         line 50 10 50 85 -width 1 -fill black
     } -coords {476.0 170.0} -relief raised -vsize 40 {
item input -slatetype Terminal -tags input -style arrowtail -direction w -coords {0 50}
item output -slatetype Terminal -tags output -style arrowout -direction e -coords {100 50}
}
item edge4 -slatetype Net -toaspect w -start e -tags edge -fill black -shape line -state 0 -coords {376.0 170.0 392.0 170.0} -smooth off -arrow none -beta 30 -fromaspect terminal -capstyle projecting -alpha 12 -end e -width 1 -joinstyle miter -autoshape 1
item edge5 -toaspect terminal -slatetype Net -start e -tofeature terminal -tags edge -fill black -fromfeature terminal -shape line -state 1 -coords {526.0 170.0 551.0 170.0 551.0 133.0 576.0 133.0} -smooth off -arrow none -beta 30 -fromaspect terminal -capstyle projecting -alpha 12 -end e -width 1 -joinstyle miter -autoshape 1
item junction0 -slatetype Junction -tags junction -size 5 -fill black -width 1 -outline black -shape point -coords {254.0 170.0}
item edge6 -slatetype Net -toaspect terminal -start e -tags edge -fill black -shape line -state 0 -coords {259.0 170.0 276.0 170.0} -smooth off -arrow none -beta 30 -fromaspect e -capstyle projecting -alpha 12 -end e -width 1 -joinstyle miter -autoshape 1
item edge7 -toaspect terminal -slatetype Net -start s -tags edge -fill black -shape line -state 0 -coords {254.0 175.0 254.0 262.0 226.0 262.0} -smooth off -arrow none -beta 30 -fromaspect s -capstyle projecting -alpha 12 -end w -width 1 -joinstyle miter -autoshape 1
item edge8 -toaspect terminal -slatetype Net -start w -tags edge -fill black -shape line -state 0 -coords {126.0 262.0 65.0 262.0 65.0 215.0} -smooth off -arrow none -beta 30 -fromaspect terminal -capstyle projecting -alpha 12 -end n -width 1 -joinstyle miter -autoshape 1
item junction1 -slatetype Junction -tags junction -size 5 -fill black -width 1 -outline black -shape point -coords {397.0 170.0}
item edge9 -slatetype Net -toaspect terminal -start e -tofeature terminal -tags edge -fill black -fromfeature terminal -shape line -state 0 -coords {402.0 170.0 426.0 170.0} -smooth off -arrow none -beta 30 -fromaspect e -capstyle projecting -alpha 12 -end e -width 1 -joinstyle miter -autoshape 1
item edge10 -toaspect terminal -slatetype Net -start s -tags edge -fill black -shape line -state 0 -coords {397.0 175.0 397.0 325.0 356.0 325.0} -smooth off -arrow none -beta 30 -fromaspect s -capstyle projecting -alpha 12 -end w -width 1 -joinstyle miter -autoshape 1
item edge11 -toaspect terminal -slatetype Net -start w -tags edge -fill black -shape line -state 1 -coords {256.0 325.0 8.0 325.0 8.0 170.0 20.0 170.0} -smooth off -arrow none -beta 30 -fromaspect terminal -capstyle projecting -alpha 12 -end e -width 1 -joinstyle miter -autoshape 1
item junction2 -fill black -size 5 -tags junction -slatetype Junction -outline black -width 1 -shape point -coords {397.0 54.0}
item edge12 -slatetype Net -toaspect terminal -start w -tags edge -fill black -shape line -state 0 -coords {392.0 54.0 276.0 54.0} -smooth off -arrow none -beta 30 -fromaspect w -capstyle projecting -alpha 12 -end w -width 1 -joinstyle miter -autoshape 1
item edge13 -slatetype Net -toaspect terminal -start s -tags edge -fill black -shape line -state 0 -coords {397.0 59.0 397.0 113.0 576.0 113.0} -smooth off -arrow none -beta 30 -fromaspect s -capstyle projecting -alpha 12 -end e -width 1 -joinstyle miter -autoshape 1
item vertex1 -slatetype IconFrame -anchor center -hsize 60 -text Plotter -borderwidth 2 -color lightblue -tags vertex -font -adobe-helvetica-medium-r-normal-*-*-120-75-75-*-*-iso8859-* -label Plotter_3 -textanchor center -shape point -graphics {
        polygon 20 20 20 80 80 80 80 20 -fill gray -outline black
        line 21 39 79 39 -fill green -stipple gray25
        line 21 59 79 59 -fill green -stipple gray25
        line 30 22 30 79 -fill green
        line 50 22 50 79 -fill green 
        line 70 23 70 79 -fill green
	line 21 79 40 40 70 60 79 31 -smooth true -fill red
    } -coords {626.0 123.0} -relief raised -vsize 40 {
item input-0 -fill red -slatetype Terminal -tags input -style arrowtail -direction w -coords {0 25}
item input-1 -fill red -slatetype Terminal -tags input -style arrowtail -direction w -coords {0 50}
item input-2 -fill red -slatetype Terminal -tags input -style arrowtail -direction w -coords {0 75}
}
item edge14 -slatetype Net -toaspect terminal -start n -tags edge -fill black -shape line -state 0 -coords {397.0 165.0 397.0 123.0 576.0 123.0} -smooth off -arrow none -beta 30 -fromaspect n -capstyle projecting -alpha 12 -end e -width 1 -joinstyle miter -autoshape 1
}
