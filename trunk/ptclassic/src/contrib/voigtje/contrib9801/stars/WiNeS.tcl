# WiNeS.tcl
#
# @(#)WiNeS.tcl	1.2 06/24/97
# author: J.Voigt
#
# copyright (c) Dresden University of Technology
# Mobile Communications Systems
#
# This file has to be used with DEWiNeS_Tcl_animation.pl.
#
# Contains data for an environment which is created during setup as well as
# procedures for the animation.
#
# Make sure that the files bs.xbm and ms.xbm are in the right place !!!


set s $ptkControlPanel.field_$starID


if {![winfo exists $s] } {
    catch {destroy $s}
    toplevel $s
    wm title $s "WiNeS-Animation"
    wm iconname $s "WiNeS-Animation"
    frame $s.f -bd 10
    canvas $s.f.pad -relief sunken -height 600 -width 1000
    button $s.f.ok -text "DISMISS" -command "ptkStop [curuniverse]; destroy $s"
    pack append $s.f $s.f.pad top $s.f.ok {top fillx}
    pack append $s $s.f top

    $s.f.pad create polygon \
            50 50 \
            50 100 \
            60 100\
            60 60\
            112 60 \
            112 50 \
            50 50
    $s.f.pad create line \
            50 100 \
            50 136
    $s.f.pad create polygon \
            50 136 \
            50 163 \
            26 163 \
            26 186 \
            36 186 \
            36 173 \
            189 173 \
            189 165 \
            70 165 \
            70 153 \
            60 153 \
            60 136 \
            50 136
    $s.f.pad create line \
            26 183 \
            26 204
    $s.f.pad create polygon \
            26 204 \
            26 211 \
            36 211 \
            36 204 \
            36 204
    $s.f.pad create line \
            26 211 \
            26 259
    $s.f.pad create polygon \
            26 259 \
            26 266 \
            36 266 \
            36 259 \
            26 259
    $s.f.pad create line \
            26 266 \
            26 287  
      $s.f.pad create polygon \
            26 287 \
            26 303 \
            50 303 \
            50 341 \
            60 341 \
            60 309 \
            70 309 \
            70 301 \
            163 301 \
            163 341 \
            171 341 \
            171 301 \
            178 301 \
            178 293 \
            36 293 \
            36 287 \
            26 287
    $s.f.pad create line \
            50 341 \
            50 373
    $s.f.pad create polygon \
            50 373 \
            50 401 \
            60 401 \
            60 373 \
            50 373
    $s.f.pad create line \
            50 401 \
            50 437
    $s.f.pad create polygon \
            50 437 \
            50 465 \
            60 465 \
            60 437 \
            50 437
    $s.f.pad create line \
            50 465 \
            50 501
    $s.f.pad create polygon \
            50 501 \
            50 529 \
            60 529 \
            60 501 \
            50 501
$s.f.pad create line \
        112 50 \
        155 50
$s.f.pad create polygon \
        155 50 \
        176 50 \
        176 60 \
        155 60 \
        155 50
$s.f.pad create line \
        176 50 \
        212 50
$s.f.pad create polygon \
        212 50 \
        240 50 \
        240 60 \
        212 60 \
        212 50
$s.f.pad create line \
        240 50 \
        276 50
$s.f.pad create polygon \
        276 50 \
        304 50 \
        304 60 \
        276 60 \
        276 50
$s.f.pad create line \
        304 50 \
        340 50
$s.f.pad create polygon \
        340 50 \
        368 50 \
        368 60 \
        340 60 \
        340 50
$s.f.pad create line \
        368 50 \
        404 50
$s.f.pad create polygon \
        404 50 \
        432 50 \
        432 60 \
        404 60 \
        404 50
$s.f.pad create line \
        432 50 \
        468 50
$s.f.pad create polygon \
        468 50 \
        496 50 \
        496 60 \
        468 60 \
        468 50
$s.f.pad create line \
        496 50 \
        532 50
$s.f.pad create polygon \
        532 50 \
        560 50 \
        560 60 \
        532 60 \
        532 50
$s.f.pad create line \
        560 50 \
        596 50
$s.f.pad create polygon \
        596 50 \
        624 50 \
        624 60 \
        596 60 \
        596 50
$s.f.pad create line \
        624 50 \
        660 50
$s.f.pad create polygon \
        660 50 \
        688 50 \
        688 60 \
        660 60 \
        660 50
$s.f.pad create line \
        688 50 \
        724 50
$s.f.pad create polygon \
        724 50 \
        752 50 \
        752 60 \
        724 60 \
        724 50
$s.f.pad create line \
        752 50 \
        788 50
$s.f.pad create polygon \
        788 50 \
        816 50 \
        816 60 \
        788 60 \
        788 50
$s.f.pad create line \
        816 50 \
        852 50
$s.f.pad create polygon \
        852 50 \
        880 50 \
        880 60 \
        852 60 \
        852 50
$s.f.pad create polygon \
        163 60 \
        163 125 \
        165 125 \
        165 60 \
        163 60
$s.f.pad create polygon \
        163 145 \
        163 165 \
        165 165 \
        165 145 \
        163 145
$s.f.pad create polygon \
        287 60 \
        287 125 \
        293 125 \
        293 60 \
        287 60
$s.f.pad create polygon \
        287 145 \
        287 165 \
        293 165 \
        293 145 \
        287 145
$s.f.pad create polygon \
        349 60 \
        349 165 \
        355 165 \
        355 60 \
        349 60
$s.f.pad create polygon \
        415 60 \
        415 165 \
        421 165 \
        421 60 \
        415 60
$s.f.pad create polygon \
        479 60 \
        479 165 \
        485 165 \
        485 60 \
        479 60
$s.f.pad create polygon \
        609 60 \
        609 165 \
        611 165 \
        611 60 \
        609 60
$s.f.pad create polygon \
        209 165 \
        209 173 \
        302 173 \
        302 181 \
        310 181 \
        310 173 \
        319 173 \
        319 165 \
        209 165
$s.f.pad create polygon \
        339 165 \
        339 173 \
        376 173 \
        376 165 \
        339 165
$s.f.pad create polygon \
        396 165 \
        396 173 \
        442 173 \
        442 165 \
        396 165
$s.f.pad create polygon \
        462 165 \
        462 173 \
        490 173 \
        490 165 \
        462 165
$s.f.pad create polygon \
        479 173 \
        479 181 \
        485 181 \
        485 173 \
        479 173
$s.f.pad create polygon \
        490 168 \
        490 170 \
        510 170 \
        510 168 \
        490 168
$s.f.pad create polygon \
        530 168 \
        530 170 \
        605 170 \
        605 168 \
        530 168
$s.f.pad create polygon \
        605 165 \
        605 173 \
        613 173 \
        613 165 \
        605 165
$s.f.pad create polygon \
        733 165 \
        733 173 \
        741 173 \
        741 165 \
        733 165
$s.f.pad create polygon \
        870 60 \
        870 341 \
        878 341 \
        878 60 \
        870 60
$s.f.pad create polygon \
        870 165 \
        870 173 \
        862 173 \
        862 165 \
        870 165
$s.f.pad create polygon \
        870 228 \
        870 236 \
        862 236 \
        862 228 \
        870 228
$s.f.pad create polygon \
        350 351 \
        368 351 \
        368 341 \
        350 341 \
        350 351
$s.f.pad create line \
        368 351 \
        404 351
$s.f.pad create polygon \
        404 351 \
        432 351 \
        432 341 \
        404 341 \
        404 351
$s.f.pad create line \
        432 351 \
        468 351
$s.f.pad create polygon \
        468 351 \
        496 351 \
        496 341 \
        468 341 \
        468 351
$s.f.pad create line \
        496 351 \
        532 351
$s.f.pad create polygon \
        532 351 \
        560 351\
        560 341 \
        532 341 \
        532 351
$s.f.pad create line \
        560 351 \
        596 351
$s.f.pad create polygon \
        596 351 \
        624 351 \
        624 341 \
        596 341 \
        596 351
$s.f.pad create line \
        624 351\
        660 351
$s.f.pad create polygon \
        660 351 \
        688 351 \
        688 341 \
        660 341 \
        660 351
$s.f.pad create line \
        688 351 \
        724 351
$s.f.pad create polygon \
        724 351 \
        752 351 \
        752 341 \
        724 341 \
        724 351
$s.f.pad create line \
        752 351 \
        788 351
$s.f.pad create polygon \
        788 351 \
        816 351 \
        816 341 \
        788 341 \
        788 351
$s.f.pad create line \
        816 351 \
        852 351
$s.f.pad create polygon \
        852 351 \
        880 351 \
        880 341 \
        852 341 \
        852 351
$s.f.pad create polygon \
        302 220 \
        302 238 \
        310 238 \
        310 236 \
        375 236 \
        375 228 \
        310 228 \
        310 220 \
        302 220
$s.f.pad create polygon \
        396 228 \
        396 236 \
        490 236 \
        490 228 \
        396 228
$s.f.pad create polygon \
        479 220 \
        479 228 \
        485 228 \
        485 220 \
        379 220
$s.f.pad create polygon \
        490 231 \
        490 233 \
        510 233 \
        510 231 \
        490 231
$s.f.pad create polygon \
        530 231 \
        530 233 \
        605 233 \
        605 231 \
        530 231
$s.f.pad create polygon \
        605 228 \
        605 236 \
        613 236 \
        613 228 \
        605 228
$s.f.pad create polygon \
        733 228 \
        733 236 \
        741 236 \
        741 228 \
        733 228
$s.f.pad create polygon \
        479 236 \
        479 341 \
        485 341 \
        485 236 \
        479 236
$s.f.pad create polygon \
        609 236 \
        609 341 \
        611 341 \
        611 236 \
        609 236
$s.f.pad create polygon \
        340 236 \
        340 401 \
        350 401 \
        350 236 \
        340 236
$s.f.pad create line \
        350 401 \
        350 437
$s.f.pad create polygon \
        340 437 \
        340 465 \
        350 465 \
        350 437 \
        340 437
$s.f.pad create line \
        350 465 \
        350 501
$s.f.pad create polygon \
        340 501 \
        340 529 \
        350 529 \
        350 501 \
        340 501
$s.f.pad create polygon \
        163 361 \
        163 401 \
        171 401 \
        171 361 \
        163 361
$s.f.pad create polygon \
        163 421 \
        171 421 \
        171 469 \
        163 469 \
        163 421
$s.f.pad create polygon \
        163 489 \
        171 489 \
        171 529 \
        163 529 \
        163 489 
$s.f.pad create polygon \
        60 386 \
        163 386 \
        163 388 \
        60 388 \
        60 386
$s.f.pad create polygon \
        60 448 \
        163 448 \
        163 454 \
        60 454 \
        60 448
$s.f.pad create polygon \
        60 512 \
        163 512 \
        163 518 \
        60 518 \
        60 512
$s.f.pad create polygon \
        220 293 \
        220 301 \
        310 301 \
        310 264 \
        302 264 \
        302 293 \
        220 293
$s.f.pad create polygon \
        310 273 \
        310 279 \
        340 279 \
        340 273 \
        310 273 
$s.f.pad create polygon \
        227 301 \
        227 401 \
        235 401 \
        235 301 \
        227 301
$s.f.pad create polygon \
        227 421 \
        227 469 \
        235 469 \
        235 421 \
        227 421
$s.f.pad create polygon \
        227 489 \
        227 529 \
        235 529 \
        235 489 \
        227 489
$s.f.pad create polygon \
        235 448 \
        340 448 \
        340 454 \
        235 454 \
        235 448
$s.f.pad create polygon \
        235 512 \
        340 512 \
        340 518 \
        235 518 \
        235 512
}
# 103

#$s.f.pad create text 900 400 -text connection
#$s.f.pad create text 900 430 -text CIR<40dB
#$s.f.pad create text 900 460 -text CIR<20dB
#$s.f.pad create text 900 490 -text CIR<10dB
#$s.f.pad create text 900 520 -text Stoerer=Basisstation
#s.f.pad create text 900 550 -text Stoerer=Mobilstation
#$s.f.pad create text 480 400 -text Interferenzanalyse/Uplink -justify left
#$s.f.pad create text 480 410 -text -------------------- -justify left
#$s.f.pad create text 400 450 -text die -justify left 
#$s.f.pad create text 455 450 -text Interferenzen -justify left 
#$s.f.pad create text 525 450 -text werden -justify left 
#$s.f.pad create text 575 450 -text jeweils -justify left 
#$s.f.pad create text 400 480 -text an -justify left 
#$s.f.pad create text 425 480 -text der -justify left 
#$s.f.pad create text 480 480 -text Basisstation -justify left 
#$s.f.pad create text 550 480 -text gemessen -justify left


#$s.f.pad create line 700 400 800 400 -fill yellow -arrow both
#$s.f.pad create line 700 430 800 430 -fill red -width 1
#$s.f.pad create line 700 460 800 460 -fill red -width 2
#$s.f.pad create line 700 490 800 490 -fill red -width 3
#$s.f.pad create line 700 520 800 520 -fill red -width 2
#$s.f.pad create line 700 550 800 550 -fill orange -width 3



# base stations
image create bitmap base \
	-file "$env(PTOLEMY)/src/domains/de/contrib/stars/bs.xbm" \
	-foreground blue

set inputvals [grabSetup_$starID]
    set test [lindex $inputvals 0]

#  in der Reihenfolge der Zellennummern
    set xbs_0 736
    set ybs_0 200
    set xbs_1 547
    set ybs_1 112
    set xbs_2 547
    set ybs_2 288
    set xbs_4 450
    set ybs_4 112
    set xbs_6 385
    set ybs_6 112
    set xbs_7 321
    set ybs_7 112
    set xbs_9 110
    set ybs_9 112
    set xbs_11 112
    set ybs_11 341
    set xbs_13 288
    set ybs_13 374
    set xbs_14 112
    set ybs_14 418
    set xbs_15 112
    set ybs_15 483
    set xbs_16 288
    set ybs_16 483

# a) gerichtet
if { $test == 1} {
    set xbs_3 605
    set ybs_3 200
    set xbs_5 358
    set ybs_5 330
    set xbs_8 170
    set ybs_8 157
    set xbs_10  38
    set ybs_10 175
    set xbs_12 200
    set ybs_12 304
} 
if { $test == 0 } {
# b) omnidirektional
    set xbs_3 460
    set ybs_3 200
    set xbs_5 414
    set ybs_5 288
    set xbs_8 225
    set ybs_8 112
    set xbs_10 169
    set ybs_10 233
    set xbs_12 200
    set ybs_12 416
}
 $s.f.pad create image $xbs_0 $ybs_0 -image base 
 $s.f.pad create image $xbs_1 $ybs_1 -image base 
 $s.f.pad create image $xbs_2 $ybs_2 -image base 
 $s.f.pad create image $xbs_3 $ybs_3 -image base 
 $s.f.pad create image $xbs_4 $ybs_4 -image base 
 $s.f.pad create image $xbs_5 $ybs_5 -image base 
 $s.f.pad create image $xbs_6 $ybs_6 -image base 
 $s.f.pad create image $xbs_7 $ybs_7 -image base 
 $s.f.pad create image $xbs_8 $ybs_8 -image base 
 $s.f.pad create image $xbs_9 $ybs_9 -image base 
 $s.f.pad create image $xbs_10 $ybs_10 -image base 
 $s.f.pad create image $xbs_11 $ybs_11 -image base 
 $s.f.pad create image $xbs_12 $ybs_12 -image base 
 $s.f.pad create image $xbs_13 $ybs_13 -image base 
 $s.f.pad create image $xbs_14 $ybs_14 -image base 
 $s.f.pad create image $xbs_15 $ybs_15 -image base 
 $s.f.pad create image $xbs_16 $ybs_16 -image base   
#120
set counter 0

proc new_$starID {starID} {

    global ptkControlPanel
    global counter
    global xbs_0
    global ybs_0 
    global xbs_1 
    global ybs_1 
    global xbs_2 
    global ybs_2 
    global xbs_3 
    global ybs_3 
    global xbs_4 
    global ybs_4 
    global xbs_5 
    global ybs_5 
    global xbs_6 
    global ybs_6 
    global xbs_7 
    global ybs_7 
    global xbs_8 
    global ybs_8 
    global xbs_9 
    global ybs_9 
    global xbs_10 
    global ybs_10 
    global xbs_11 
    global ybs_11 
    global xbs_12 
    global ybs_12 
    global xbs_13 
    global ybs_13 
    global xbs_14 
    global ybs_14 
    global xbs_15 
    global ybs_15 
    global xbs_16 
    global ybs_16 

    
    set s $ptkControlPanel.field_$starID
   
    set inputvals [grabInputsNew_$starID]
    set xin [lindex $inputvals 0]
    set yin [lindex $inputvals 1]
    set cellnum [lindex $inputvals 2]

    #Umrechnung auf Tcl_Koordinaten
    set xin [expr $xin - 500]
    set yin [expr 2500 - $yin]
    set xin [expr $xin / 5]
    set yin [expr $yin / 5]
    set xin [expr $xin + 50]
    set yin [expr $yin + 50]

    switch $cellnum {
	0 {
	    set xbs $xbs_0
	    set ybs $ybs_0
	}
	1 {
	    set xbs $xbs_1
	    set ybs $ybs_1
	}
	2 {
	    set xbs $xbs_2
	    set ybs $ybs_2
	}
	3 {
	    set xbs $xbs_3
	    set ybs $ybs_3
	}
	4 {
	    set xbs $xbs_4
	    set ybs $ybs_4
	}
	5 {
	    set xbs $xbs_5
	    set ybs $ybs_5
	}
	6 {
	    set xbs $xbs_6
	    set ybs $ybs_6
	}
	7 {
	    set xbs $xbs_7
	    set ybs $ybs_7
	}
	8 {
	    set xbs $xbs_8
	    set ybs $ybs_8
	}
	9 {
	    set xbs $xbs_9
	    set ybs $ybs_9
	}
	10 {
	    set xbs $xbs_10
	    set ybs $ybs_10
	}
	11 {
	    set xbs $xbs_11
	    set ybs $ybs_11
	}
	12 {
	    set xbs $xbs_12
	    set ybs $ybs_12
	}
	13 {
	    set xbs $xbs_13
	    set ybs $ybs_13
	}
	14 {
	    set xbs $xbs_14
	    set ybs $ybs_14
	}
	15 {
	    set xbs $xbs_15
	    set ybs $ybs_15
	}
	16 {
	    set xbs $xbs_16
	    set ybs $ybs_16
	}
    }
    
    global env
    image create bitmap handy \
	    -file "$env(PTOLEMY)/src/domains/de/contrib/stars/ms.xbm" \
	    -foreground black
    incr counter
    $s.f.pad create image $xin $yin -image handy -tag telefon_$counter 
    $s.f.pad create line $xbs $ybs $xin $yin -arrow both -fill yellow -tag pfeil_$counter
 
   #  $s.f.pad scale [expr 120 + $counter] $xin $yin 2.0 0.1
   
}

proc loeschen_$starID {starID} {
    global ptkControlPanel

    set s $ptkControlPanel.field_$starID
    set inputvals [grabInputsDel_$starID]
    set number [lindex $inputvals 0]

    $s.f.pad delete telefon_$number 
    $s.f.pad delete pfeil_$number
    $s.f.pad delete int_$number
    $s.f.pad delete ibs_$number
}

proc interferenz_$starID {starID} {
    global ptkControlPanel
    global xbs_0
    global ybs_0 
    global xbs_1 
    global ybs_1 
    global xbs_2 
    global ybs_2 
    global xbs_3 
    global ybs_3 
    global xbs_4 
    global ybs_4 
    global xbs_5 
    global ybs_5 
    global xbs_6 
    global ybs_6 
    global xbs_7 
    global ybs_7 
    global xbs_8 
    global ybs_8 
    global xbs_9 
    global ybs_9 
    global xbs_10 
    global ybs_10 
    global xbs_11 
    global ybs_11 
    global xbs_12 
    global ybs_12 
    global xbs_13 
    global ybs_13 
    global xbs_14 
    global ybs_14 
    global xbs_15 
    global ybs_15 
    global xbs_16 
    global ybs_16

    set s $ptkControlPanel.field_$starID
    set inputvals [grabInterf_$starID]
    set x [lindex $inputvals 0]
    set x1 [lindex $inputvals 1]
    set y1 [lindex $inputvals 2]
    set h1 [lindex $inputvals 3]
    set h2 [lindex $inputvals 4]
    set ss [lindex $inputvals 5]

    #Umrechnung auf Tcl_Koordinaten
    set x1 [expr $x1 - 500]
    set y1 [expr 2500 - $y1]
    set x1 [expr $x1 / 5]
    set y1 [expr $y1 / 5]
    set x1 [expr $x1 + 50]
    set y1 [expr $y1 + 50]
 
switch $x {
	0 {
	    set xbs $xbs_0
	    set ybs $ybs_0
	}
	1 {
	    set xbs $xbs_1
	    set ybs $ybs_1
	}
	2 {
	    set xbs $xbs_2
	    set ybs $ybs_2
	}
	3 {
	    set xbs $xbs_3
	    set ybs $ybs_3
	}
	4 {
	    set xbs $xbs_4
	    set ybs $ybs_4
	}
	5 {
	    set xbs $xbs_5
	    set ybs $ybs_5
	}
	6 {
	    set xbs $xbs_6
	    set ybs $ybs_6
	}
	7 {
	    set xbs $xbs_7
	    set ybs $ybs_7
	}
	8 {
	    set xbs $xbs_8
	    set ybs $ybs_8
	}
	9 {
	    set xbs $xbs_9
	    set ybs $ybs_9
	}
	10 {
	    set xbs $xbs_10
	    set ybs $ybs_10
	}
	11 {
	    set xbs $xbs_11
	    set ybs $ybs_11
	}
	12 {
	    set xbs $xbs_12
	    set ybs $ybs_12
	}
	13 {
	    set xbs $xbs_13
	    set ybs $ybs_13
	}
	14 {
	    set xbs $xbs_14
	    set ybs $ybs_14
	}
	15 {
	    set xbs $xbs_15
	    set ybs $ybs_15
	}
	16 {
	    set xbs $xbs_16
	    set ybs $ybs_16
	}
    }
  $s.f.pad create line $x1 $y1 $xbs $ybs -arrow last -fill orange -tag int_$h1 -width $ss
  $s.f.pad addtag int_$h2 withtag int_$h1

}

proc interferenz_bs_$starID {starID} {
    global ptkControlPanel
    global xbs_0
    global ybs_0 
    global xbs_1 
    global ybs_1 
    global xbs_2 
    global ybs_2 
    global xbs_3 
    global ybs_3 
    global xbs_4 
    global ybs_4 
    global xbs_5 
    global ybs_5 
    global xbs_6 
    global ybs_6 
    global xbs_7 
    global ybs_7 
    global xbs_8 
    global ybs_8 
    global xbs_9 
    global ybs_9 
    global xbs_10 
    global ybs_10 
    global xbs_11 
    global ybs_11 
    global xbs_12 
    global ybs_12 
    global xbs_13 
    global ybs_13 
    global xbs_14 
    global ybs_14 
    global xbs_15 
    global ybs_15 
    global xbs_16 
    global ybs_16 


    set s $ptkControlPanel.field_$starID
    set inputvals [grabInterf_bs_$starID]
    set x [lindex $inputvals 0]
    set ms [lindex $inputvals 1]
    set bs [lindex $inputvals 2]
    set ss [lindex $inputvals 3]
   
    
    switch $bs {
	0 {
	    set xbs $xbs_0
	    set ybs $ybs_0
	}
	1 {
	    set xbs $xbs_1
	    set ybs $ybs_1
	}
	2 {
	    set xbs $xbs_2
	    set ybs $ybs_2
	}
	3 {
	    set xbs $xbs_3
	    set ybs $ybs_3
	}
	4 {
	    set xbs $xbs_4
	    set ybs $ybs_4
	}
	5 {
	    set xbs $xbs_5
	    set ybs $ybs_5
	}
	6 {
	    set xbs $xbs_6
	    set ybs $ybs_6
	}
	7 {
	    set xbs $xbs_7
	    set ybs $ybs_7
	}
	8 {
	    set xbs $xbs_8
	    set ybs $ybs_8
	}
	9 {
	    set xbs $xbs_9
	    set ybs $ybs_9
	}
	10 {
	    set xbs $xbs_10
	    set ybs $ybs_10
	}
	11 {
	    set xbs $xbs_11
	    set ybs $ybs_11
	}
	12 {
	    set xbs $xbs_12
	    set ybs $ybs_12
	}
	13 {
	    set xbs $xbs_13
	    set ybs $ybs_13
	}
	14 {
	    set xbs $xbs_14
	    set ybs $ybs_14
	}
	15 {
	    set xbs $xbs_15
	    set ybs $ybs_15
	}
	16 {
	    set xbs $xbs_16
	    set ybs $ybs_16
	}
    }

    switch $x {
	0 {
	    set xs $xbs_0
	    set ys $ybs_0
	}
	1 {
	    set xs $xbs_1
	    set ys $ybs_1
	}
	2 {
	    set xs $xbs_2
	    set ys $ybs_2
	}
	3 {
	    set xs $xbs_3
	    set ys $ybs_3
	}
	4 {
	    set xs $xbs_4
	    set ys $ybs_4
	}
	5 {
	    set xs $xbs_5
	    set ys $ybs_5
	}
	6 {
	    set xs $xbs_6
	    set ys $ybs_6
	}
	7 {
	    set xs $xbs_7
	    set ys $ybs_7
	}
	8 {
	    set xs $xbs_8
	    set ys $ybs_8
	}
	9 {
	    set xs $xbs_9
	    set ys $ybs_9
	}
	10 {
	    set xs $xbs_10
	    set ys $ybs_10
	}
	11 {
	    set xs $xbs_11
	    set ys $ybs_11
	}
	12 {
	    set xs $xbs_12
	    set ys $ybs_12
	}
	13 {
	    set xs $xbs_13
	    set ys $ybs_13
	}
	14 {
	    set xs $xbs_14
	    set ys $ybs_14
	}
	15 {
	    set xs $xbs_15
	    set ys $ybs_15
	}
	16 {
	    set xs $xbs_16
	    set ys $ybs_16
	}
    }

  $s.f.pad create line $xs $ys $xbs $ybs -arrow first -fill red -tag ibs_$ms -width $ss
  
}
unset s
