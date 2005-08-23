# Tcl/Tk source for the displays of the Digital Watch.
#
# Author: Bilung Lee
# Version: %W%   %G%
#
# Copyright (c) 1990-%Q% The Regents of the University of California.
# All rights reserved.
# 
# Permission is hereby granted, without written agreement and without
# license or royalty fees, to use, copy, modify, and distribute this
# software and its documentation for any purpose, provided that the
# above copyright notice and the following two paragraphs appear in all
# copies of this software.
# 
# IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY
# FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
# ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
# THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
# 
# THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
# INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
# PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
# CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
# ENHANCEMENTS, OR MODIFICATIONS.
# 
# 						PT_COPYRIGHT_VERSION_2
# 						COPYRIGHTENDKEY
# See the file $PTOLEMY/copyright for copyright notice,
# limitation of liability, and disclaimer of warranty provisions.
#

set s $ptkControlPanel.digiWatch

if {![winfo exists $s]} {
    toplevel $s
    wm title $s "Digital Watch"
    wm iconname $s "Digital Watch"

    frame $s.left
    pack $s.left   -side left -fill both -expand 1
    frame $s.middle
    pack $s.middle -side left -fill both -expand 1
    frame $s.right
    pack $s.right  -side left -fill both -expand 1

}  else {
    wm deiconify $s
    raise $s
}

set c $s.middle.canvas
catch {destroy $c}
canvas $c -relief groove -bd 4 -height 3.5c -width 8c -background grey
pack $c

proc drawOneDigit {win tag num color} {
    if {$tag == "hr1"} {
	set x 48; set y 55; set w 3; set l 18; set d 2
	if {$num == 0} {set num -1}
    } elseif {$tag == "hr0"} {
	set x 90; set y 55; set w 3; set l 18; set d 2
    } elseif {$tag == "min1"} {
	set x 150; set y 55; set w 3; set l 18; set d 2
    } elseif {$tag == "min0"} {
	set x 192; set y 55; set w 3; set l 18; set d 2
    } elseif {$tag == "sec1"} {
	set x 235; set y 80; set w 2; set l 10; set d 1
    } elseif {$tag == "sec0"} {
	set x 260; set y 80; set w 2; set l 10; set d 1
    } else {
	error "Unrecognized tag type for drawOneDigit proc!"
    }


    if {$num==0 || $num==2 || $num==3 || $num==5 || $num==6\
	    || $num==7 || $num==8 || $num==9 } {
	$win create polygon $x $y [expr $x+$w] [expr $y-$w]\
		[expr $x+$w+$l] [expr $y-$w] [expr $x+$l+2*$w] $y\
		[expr $x+$w+$l] [expr $y+$w] [expr $x+$w] [expr $y+$w]\
		-tags $tag -fill $color
    }
    if {$num==2 || $num==3 || $num==4 || $num==5 || $num==6 || $num==8\
	    || $num==9 } {
	$win create polygon $x [expr $y+2*$d+$l+2*$w]\
		[expr $x+$w] [expr $y+2*$d+$l+$w]\
		[expr $x+$w+$l] [expr $y+2*$d+$l+$w]\
		[expr $x+2*$w+$l] [expr $y+2*$d+$l+2*$w]\
		[expr $x+$w+$l] [expr $y+2*$d+$l+3*$w]\
		[expr $x+$w] [expr $y+2*$d+$l+3*$w] -tags $tag -fill $color
    }
    if {$num==0 || $num==2 || $num==3 || $num==5 || $num==6 || $num==8\
	    || $num==9 } {
	$win create polygon $x [expr $y+4*$d+2*$l+4*$w]\
		[expr $x+$w] [expr $y+4*$d+2*$l+3*$w]\
		[expr $x+$w+$l] [expr $y+4*$d+2*$l+3*$w]\
		[expr $x+2*$w+$l] [expr $y+4*$d+2*$l+4*$w]\
		[expr $x+$w+$l] [expr $y+4*$d+2*$l+5*$w]\
		[expr $x+$w] [expr $y+4*$d+2*$l+5*$w] -tags $tag -fill $color
    }
    if {$num==0 || $num==4 || $num==5 || $num==6 || $num==8 || $num==9 } {
	$win create polygon $x [expr $y+$d] [expr $x+$w] [expr $y+$d+$w]\
		[expr $x+$w] [expr $y+$d+$w+$l]	$x [expr $y+$d+2*$w+$l]\
		[expr $x-$w] [expr $y+$d+$w+$l]	[expr $x-$w] [expr $y+$d+$w]\
		-tags $tag -fill $color
    }
    if {$num==0 || $num==1 || $num==2 || $num==3 || $num==4 || $num==7\
	    || $num==8 || $num==9 } {
	$win create polygon [expr $x+$l+2*$w] [expr $y+$d]\
		[expr $x+$l+3*$w] [expr $y+$d+$w]\
		[expr $x+$l+3*$w] [expr $y+$d+$w+$l]\
		[expr $x+$l+2*$w] [expr $y+$d+2*$w+$l]\
		[expr $x+$l+$w] [expr $y+$d+$w+$l]\
		[expr $x+$l+$w] [expr $y+$d+$w] -tags $tag -fill $color
    }
    if {$num==0 || $num==2 || $num==6 || $num==8} {
	$win create polygon $x [expr $y+3*$d+$l+2*$w]\
	    [expr $x+$w] [expr $y+3*$d+$l+3*$w]\
	    [expr $x+$w] [expr $y+3*$d+2*$l+3*$w]\
	    $x [expr $y+3*$d+2*$l+4*$w] [expr $x-$w] [expr $y+3*$d+2*$l+3*$w]\
	    [expr $x-$w] [expr $y+3*$d+$l+3*$w] -tags $tag -fill $color
    }
    if {$num==0 || $num==1 || $num==3 || $num==4 || $num==5 || $num==6\
	    || $num==7 || $num==8 || $num==9 } {
	$win create polygon [expr $x+$l+2*$w] [expr $y+3*$d+$l+2*$w]\
		[expr $x+$l+3*$w] [expr $y+3*$d+$l+3*$w]\
		[expr $x+$l+3*$w] [expr $y+3*$d+2*$l+3*$w]\
		[expr $x+$l+2*$w] [expr $y+3*$d+2*$l+4*$w]\
		[expr $x+$l+$w] [expr $y+3*$d+2*$l+3*$w]\
		[expr $x+$l+$w] [expr $y+3*$d+$l+3*$w] -tags $tag -fill $color
    }

#    return [list $row1 $row2 $row3 $column1 $column2 $column3 $column4]
}

proc drawColon {win tag} {
    if {$tag == "colon0"} {
	set x 128; set y 65
    } else {
	error "Unrecognized tag type for drawColon proc!"
    }
    
    $win create rectangle $x $y [expr $x+6] [expr $y+6] -fill black -tags $tag
    $win create rectangle $x [expr $y+30] [expr $x+6] [expr $y+36]\
	    -fill black -tags $tag
}

# ---------- Global variables ---------
set dispMd {Time Alarm}
set timeUpdMd {Second Minute Hour Day}
set alarmUpdMd {Minute Hour}
set alarmMd {Off Alarm Both Chime}

set curDispMd Time
set curUpdOnOff 0
set curTimeUpdMd Second
set curAlarmUpdMd Minute
set curAlarmMd Off

set curLightSwitch Off

set beeping 0
set beepCount 0
set hasChimed 0
set 24HMd 1

set displayColor(Second) black
set displayColor(Minute) black
set displayColor(Hour)   black
set displayColor(Day)    black

set dayOfWeek {Sun Mon Tue Wed Thu Fri Sat}
# --------------------------------------

# ---------- Setup display ---------
set 24HrText [$c create text 0.25c 3.0c -anchor w -text 24H]
set dayText  [$c create text 6.5c 0.6c -anchor w -text Sun\
	-fill $displayColor(Day)]
set alarmText [$c create text 0.25c 0.4c -anchor w -text ""]
set chimeText [$c create text 0.25c 0.8c -anchor w -text ""]

drawOneDigit $c hr1  8 $displayColor(Second)
drawOneDigit $c hr0  8 $displayColor(Second)
drawColon $c colon0
drawOneDigit $c min1 8 $displayColor(Minute)
drawOneDigit $c min0 8 $displayColor(Minute)
drawOneDigit $c sec1 8 $displayColor(Second)
drawOneDigit $c sec0 8 $displayColor(Second)
# --------------------------------------

proc goTcl_$starID {starID} "
    processInputs_$starID $starID $c
"

proc destructorTcl_$starID {starID} "
    # Unset all global variables
    global 24HrText dayText alarmText chimeText
    global dispMd timeUpdMd alarmUpdMd alarmMd
    global curDispMd curUpdOnOff curTimeUpdMd curAlarmUpdMd curAlarmMd
    global curLightSwitch
    global beeping beepCount hasChimed 24HMd
    global displayColor dayOfWeek
    unset 24HrText dayText alarmText chimeText
    unset dispMd timeUpdMd alarmUpdMd alarmMd
    unset curDispMd curUpdOnOff curTimeUpdMd curAlarmUpdMd curAlarmMd
    unset curLightSwitch
    unset beeping beepCount hasChimed 24HMd
    unset displayColor dayOfWeek    
"

proc processInputs_$starID {starID w} {
    # Load global variables
    global 24HrText dayText alarmText chimeText
    global dispMd timeUpdMd alarmUpdMd alarmMd
    global curDispMd curUpdOnOff curTimeUpdMd curAlarmUpdMd curAlarmMd
    global curLightSwitch
    global beeping beepCount hasChimed 24HMd
    global displayColor dayOfWeek

    # We only care about whether an event occurred at each input,
    # not about its value.
    set inputVals [grabInputs_$starID]

    # For clarity, break up the inputs by name
    set time              [lindex $inputVals 0]
    set second            [lindex $inputVals 1]
    set minute            [lindex $inputVals 2]
    set hour              [lindex $inputVals 3]
    set day               [lindex $inputVals 4]
    set alarmMin          [lindex $inputVals 5]
    set alarmHr           [lindex $inputVals 6]
    set chime             [lindex $inputVals 7]
    set beep_on           [lindex $inputVals 8]
    set toggle24HMd       [lindex $inputVals 9]
    set toggleDispMd      [lindex $inputVals 10]
    set toggleUpdOnOff    [lindex $inputVals 11]
    set toggleUpdMd       [lindex $inputVals 12]
    set toggleAlarmMd     [lindex $inputVals 13]
    set toggleLightSwitch [lindex $inputVals 14]
    set beep_off          [lindex $inputVals 15]

    if {$toggle24HMd} {
	set 24HMd [expr !$24HMd]
	if {$24HMd} {$w itemconfigure $24HrText -text 24H}
    } elseif {$toggleDispMd} {
	set nextIndx [expr ([lsearch $dispMd $curDispMd]+1)%[llength $dispMd]]
	set curDispMd [lindex $dispMd $nextIndx]
	if {$curDispMd=="Time"} {
	    drawColon $w colon0
	} elseif {$curDispMd=="Alarm"} {
	    $w delete colon0 sec0 sec1
	    $w itemconfigure $dayText -text ""
	}
    } elseif {$toggleUpdOnOff} {
	set curUpdOnOff [expr !$curUpdOnOff]
	if {$curUpdOnOff} {
	    if {$curDispMd == "Time"} {
		set curTimeUpdMd  [lindex $timeUpdMd 0]
		set displayColor(Second) red
	    } elseif {$curDispMd == "Alarm"} {
		set curAlarmUpdMd [lindex $alarmUpdMd 0]
		set displayColor(Minute) red
	    }
	} else {
	    set displayColor(Second) black
	    set displayColor(Minute) black
	    set displayColor(Hour)   black
	    set displayColor(Day)    black
	}
    } elseif {$toggleUpdMd} {
	if {$curDispMd=="Time"} {
	    set nextIndx [expr ([lsearch $timeUpdMd $curTimeUpdMd]+1)\
		    %[llength $timeUpdMd]]
	    set curTimeUpdMd [lindex $timeUpdMd $nextIndx]
	    if {$curTimeUpdMd == "Second"} {
		set displayColor(Second) red
		set displayColor(Day)    black
	    } elseif {$curTimeUpdMd == "Minute"} {
		set displayColor(Second) black
		set displayColor(Minute) red
	    } elseif {$curTimeUpdMd == "Hour"} {
		set displayColor(Minute) black
		set displayColor(Hour)   red
	    } elseif {$curTimeUpdMd == "Day"} {
		set displayColor(Hour)   black
		set displayColor(Day)    red
	    }

	} elseif {$curDispMd=="Alarm"} {
	    set nextIndx [expr ([lsearch $alarmUpdMd $curAlarmUpdMd]+1)\
		    %[llength $alarmUpdMd]]
	    set curAlarmUpdMd [lindex $alarmUpdMd $nextIndx]
	    if {$curAlarmUpdMd == "Minute"} {
		set displayColor(Minute) red
		set displayColor(Hour)   black
	    } elseif {$curAlarmUpdMd == "Hour"} {
		set displayColor(Minute) black
		set displayColor(Hour)   red
	    }

	}
    } elseif {$toggleAlarmMd} {
	set nextIndx [expr ([lsearch $alarmMd $curAlarmMd]+1)\
		%[llength $alarmMd]]
	set curAlarmMd [lindex $alarmMd $nextIndx]
	if {$curAlarmMd == "Off"} {
	    $w itemconfigure $chimeText -text ""
	} elseif {$curAlarmMd == "Alarm"} {
	    $w itemconfigure $alarmText -text "Alarm On"
	} elseif {$curAlarmMd == "Both"} {
	    $w itemconfigure $chimeText -text "Chime On"
	} elseif {$curAlarmMd == "Chime"} {
	    $w itemconfigure $alarmText -text ""
	} else {
	    error "Unrecognized alarm mode!"
	}
    }

    # Refresh the display screen
    if {$curDispMd=="Time"} {
        set sec1 [expr int($second/10)]
	set sec0 [expr $second-$sec1*10]
        set min1 [expr int($minute/10)]
	set min0 [expr $minute-$min1*10]
	if {!$24HMd} {
	    if {$hour >= 12} {
		$w itemconfigure $24HrText -text PM
		set hour [expr $hour-12]
	    } else {
		$w itemconfigure $24HrText -text AM
	    }
	    if {$hour == 0} {set hour 12}
	}
	set hr1 [expr int($hour/10)]
	set hr0 [expr $hour-$hr1*10]
	$w delete sec0 sec1 min1 min0 hr1 hr0
	drawOneDigit $w sec0 $sec0 $displayColor(Second)
	drawOneDigit $w sec1 $sec1 $displayColor(Second)
	drawOneDigit $w min0 $min0 $displayColor(Minute)
	drawOneDigit $w min1 $min1 $displayColor(Minute)
	drawOneDigit $w hr0  $hr0  $displayColor(Hour)
	drawOneDigit $w hr1  $hr1  $displayColor(Hour)

	$w itemconfigure $dayText -text [lindex $dayOfWeek [expr int($day)]]\
		-fill $displayColor(Day)

    } elseif {$curDispMd=="Alarm"} {
        set min1 [expr int($alarmMin/10)]
	set min0 [expr $alarmMin-$min1*10]
	if {!$24HMd} {
	    if {$alarmHr >= 12} {
		$w itemconfigure $24HrText -text PM
		set alarmHr [expr $alarmHr-12]
	    } else {
		$w itemconfigure $24HrText -text AM
	    }
	    if {$alarmHr == 0} {set alarmHr 12}
	}
	set hr1 [expr int($alarmHr/10)]
	set hr0 [expr $alarmHr-$hr1*10]
	$w delete min1 min0 hr1 hr0
	drawOneDigit $w min0 $min0 $displayColor(Minute)
	drawOneDigit $w min1 $min1 $displayColor(Minute)
	drawOneDigit $w hr0  $hr0  $displayColor(Hour)
	drawOneDigit $w hr1  $hr1  $displayColor(Hour)
	
    } else {
	error "Unrecognized display mode!"
    }

    # Check Alarm/Chime
    if {$beep_on} {
	if {$curAlarmMd=="Alarm" || $curAlarmMd=="Both"} {
	    set beeping 1 
	}
    } elseif {$beep_off} {
	set beeping 0
    }
    # Sound the Alarm/Chime
    if {$beeping} {
	bell
	if {$time != 0} {
	    # Make sure that alarm exactly beeps one second.
	    incr beepCount
	    if {$beepCount >= [expr 60.0/$time]} {
		set beeping   0
		set beepCount 0
	    }
	}
    } elseif {$chime} {
	if {$curAlarmMd=="Chime" || $curAlarmMd=="Both"} {
	    if {!$hasChimed} {
		bell; bell;
                # Let the chime do not sound twice within one second. 
		set hasChimed 1
	    }
	}
    } elseif {$second != $minute} {
	# Let the chime be able to sound again.
	set hasChimed 0
    }

    # Turn on/off the light
    if {$toggleLightSwitch} {
	if {$curLightSwitch == "Off"} {
	    $w configure -background white
	    set curLightSwitch On
	} else {
	    $w configure -background grey
	    set curLightSwitch Off
	}
    }
    
}

tkwait visibility $c