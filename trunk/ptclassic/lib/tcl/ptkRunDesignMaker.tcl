# Definition of the control panel for the DesignMaker
# Author: Asawaree Kalavade
# $Id$
#
# Copyright (c) 1990-1993 The Regents of the University of California.
# All rights reserved.
#
# Permission is hereby granted, without written agreement and without
# license or royalty fees, to use, copy, modify, and distribute this
# software and its documentation for any purpose, provided that the above
# copyright notice and the following two paragraphs appear in all copies
# of this software.
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
#                                                         COPYRIGHTENDKEY


#######################################################################
global dmm_prevFullName
global dmm_reliefFlag
global dmm_doneFlag
global dmm_yesOrNoFlag

# dummy value for startup
set dmmActiveFlag(main) IDLE
#######################################################################
# The following functions (returnYes, returnNo, ptkShowNonDet and
# ptkAskYesOrNo) are related to displaying non determinacy
# and asking the user whether or not to continue the run.
#######################################################################
proc returnYes {} {
	global dmm_doneFlag
	global dmm_yesOrNoFlag
	set dmm_yesOrNoFlag 1
	set dmm_doneFlag 1
}
proc returnNo {} {
	global dmm_doneFlag
	global dmm_yesOrNoFlag
	set dmm_yesOrNoFlag 0
	set dmm_doneFlag 1
}
proc ptkShowNonDet {file} {
        set fileID [open $file r]
        set text [read $fileID]
        close $fileID
        ptkMessage $text
}
proc ptkAskYesOrNo {octHandle question strYes strNo fileNm} {
    	set ctrlPanel .run_designmaker_$octHandle

	global dmm_yesOrNoFlag
	set dmm_doneFlag 0

	frame $ctrlPanel.yesNoSelector -bd 10
	label $ctrlPanel.yesNoSelector.label \
		-text $question -fg [ptkColor red]
	pack append $ctrlPanel.yesNoSelector \
		$ctrlPanel.yesNoSelector.label {top fill}
	radiobutton $ctrlPanel.yesNoSelector.yes -text $strYes \
		-variable yesOrNo -value 1 -relief flat \
		-command returnYes
	radiobutton $ctrlPanel.yesNoSelector.no -text $strNo \
		-variable yesOrNo -value 0 -relief flat \
		-command returnNo
        radiobutton $ctrlPanel.yesNoSelector.displayND \
		-text "Show Nondeterminacy" \
		-variable showND -value 0 -relief flat \
        	-command "ptkShowNonDet $fileNm" -width 20

	pack append $ctrlPanel.yesNoSelector \
		$ctrlPanel.yesNoSelector.displayND {frame w} \
		$ctrlPanel.yesNoSelector.yes {frame w} \
		$ctrlPanel.yesNoSelector.no {frame w}

	pack $ctrlPanel.yesNoSelector -expand 1 -after $ctrlPanel.des

	tkwait variable dmm_doneFlag
	catch {destroy $ctrlPanel.yesNoSelector}
	return $dmm_yesOrNoFlag
}
########################################################################
# Ask for the full name of the block to run. Used by runUpto and runThis
#######################################################################
proc askFullName {octHandle} {
	global dmm_prevFullName
	global dmm_reliefFlag
    	set ctrlPanel .run_designmaker_$octHandle

	set dmm_doneFlag 0

	frame $ctrlPanel.fullNameSelector -bd 10
	label $ctrlPanel.fullNameSelector.label \
		-text "enter full name of star and hit OK"
	entry $ctrlPanel.fullNameSelector.entry -relief sunken \
		-width 10 -bg [ptkColor ivory]
	$ctrlPanel.fullNameSelector.entry insert @0 $dmm_prevFullName
	pack append $ctrlPanel.fullNameSelector \
		$ctrlPanel.fullNameSelector.label {top fill}
	pack append $ctrlPanel.fullNameSelector \
		$ctrlPanel.fullNameSelector.entry {top fill}
	checkbutton $ctrlPanel.fullNameSelector.done -text "OK" \
		-variable dmm_reliefFlag -relief flat \
		-command "set dmm_doneFlag 1"
	pack append $ctrlPanel.fullNameSelector \
		$ctrlPanel.fullNameSelector.done {top fill}

	pack $ctrlPanel.fullNameSelector -expand 1 -after $ctrlPanel.des

	tkwait variable dmm_doneFlag
	set strFullNm [$ctrlPanel.fullNameSelector.entry get]
	set dmm_prevFullName $strFullNm
	catch {destroy $ctrlPanel.fullNameSelector}
	return $strFullNm
}
#######################################################################
# check if any other dmm universe is active
#######################################################################
proc dmmCheckActiveFlags {} {
	global dmmActiveFlag
	# Allow only action in only one universe at a time
	
	set sz [array size dmmActiveFlag]
	for {set idx $sz} {$idx > 0} {incr idx -1} {
		set uname [lindex [array names dmmActiveFlag] [expr $idx - 1]]
		if {$dmmActiveFlag($uname) == {BUSY}} {
			ptkImportantMessage .error \
			"Sorry. Only one action in one DMM universe at a time."
			return 1
		}
	}
	return 0
}
#######################################################################
# The runAll function. Calls runAll in DesignMaker.
#######################################################################
proc ptkRunAll {name octHandle} {
	global dmmActiveFlag
	if {[dmmCheckActiveFlags]} {
		return
	}

	# sink the button and make dmmActiveFlag BUSY
    	set dmmControlPanel .run_designmaker_$octHandle
	catch {$dmmControlPanel.flow.runAll configure -relief sunken}
	set dmmActiveFlag($name) BUSY

	# save old highlight facet (lastFacet ptr) 
	# and set it to current universe
	set oldHighLightFacet [ptkGetHighlightFacet]
	ptkSetHighlightFacet $octHandle

	# save universe and set the new one to current universe
	set olduniverse [curuniverse]
	curuniverse $name

	ptkCompile $octHandle

        # runAll defined in PDMM.cc
        runAll

	# reset lastFacet and curuniverse pointers
	curuniverse $olduniverse
	ptkSetHighlightFacet $oldHighLightFacet

	# raise button and make dmmActiveFlag IDLE
	catch {$dmmControlPanel.flow.runAll configure -relief raised}
	set dmmActiveFlag($name) IDLE
}
#######################################################################
# The runUpto function. Calls runUpto in DesignMaker.
#######################################################################
proc ptkRunUpto {name octHandle} {

	global dmm_reliefFlag
	set dmm_reliefFlag 0

	global dmmActiveFlag
	if {[dmmCheckActiveFlags]} {
		return
	}

	# sink the button and make dmmActiveFlag BUSY
    	set dmmControlPanel .run_designmaker_$octHandle
	catch {$dmmControlPanel.flow.runUpto configure -relief sunken}
	set dmmActiveFlag($name) BUSY

	set uptoStarName [askFullName $octHandle]

	# save old highlight facet (lastFacet ptr) 
	# and set it to current universe
	set oldHighLightFacet [ptkGetHighlightFacet]
	ptkSetHighlightFacet $octHandle

	# save universe and set the new one to current universe
	set olduniverse [curuniverse]
	curuniverse $name

	ptkCompile $octHandle

       	# runUpto defined in PDMM.cc
       	runUpto $uptoStarName

	# reset lastFacet and curuniverse pointers
	curuniverse $olduniverse
	ptkSetHighlightFacet $oldHighLightFacet

	# raise button and make dmmActiveFlag IDLE
	catch {$dmmControlPanel.flow.runUpto configure -relief raised}
	set dmmActiveFlag($name) IDLE
}
#######################################################################
# The runThis function. Calls runThis in DesignMaker.
#######################################################################
proc ptkRunThis {name octHandle} {

	global dmm_reliefFlag
	set dmm_reliefFlag 0

	global dmmActiveFlag
	if {[dmmCheckActiveFlags]} {
		return
	}

	# sink the button and make dmmActiveFlag BUSY
    	set dmmControlPanel .run_designmaker_$octHandle
	catch {$dmmControlPanel.flow.runThis configure -relief sunken}
	set dmmActiveFlag($name) BUSY

	set thisStarName [askFullName $octHandle]

	# save old highlight facet (lastFacet ptr) 
	# and set it to current universe
	set oldHighLightFacet [ptkGetHighlightFacet]
	ptkSetHighlightFacet $octHandle

	# save universe and set the new one to current universe
	set olduniverse [curuniverse]
	curuniverse $name

	ptkCompile $octHandle

       	# runThis defined in PDMM.cc
       	runThis $thisStarName

	# restore lastFacet and curuniverse pointers
	curuniverse $olduniverse
	ptkSetHighlightFacet $oldHighLightFacet

	# raise button and make dmmActiveFlag IDLE
	catch {$dmmControlPanel.flow.runThis configure -relief raised}
	set dmmActiveFlag($name) IDLE
}
#######################################################################
# The save the DMM attributes to oct
#######################################################################
proc ptkSaveFacet { octHandle } {

	global dmmActiveFlag
	if {[dmmCheckActiveFlags]} {
		return
	}

	# sink the button and make dmmActiveFlag BUSY
    	set dmmControlPanel .run_designmaker_$octHandle
	catch {$dmmControlPanel.data.saveFacet configure -relief sunken}
	set dmmActiveFlag($name) BUSY

	# save old highlight facet (lastFacet ptr) 
	# and set it to current universe
	set oldHighLightFacet [ptkGetHighlightFacet]
	ptkSetHighlightFacet $octHandle

       	# use the ptkCloseFacet command
	ptkCloseFacet $octHandle

	# restore lastFacet
	ptkSetHighlightFacet $oldHighLightFacet

	# raise button and make dmmActiveFlag IDLE
	catch {$dmmControlPanel.data.saveFacet configure -relief raised}
	set dmmActiveFlag($name) IDLE
}
#######################################################################
# Reset the DMM attributes to undefined (never run before). This is 
# especially useful when forcing the system to run.
#######################################################################
proc ptkResetStatus {name octHandle} {

	global dmmActiveFlag
	if {[dmmCheckActiveFlags]} {
		return
	}

	# sink the button and make dmmActiveFlag BUSY
    	set dmmControlPanel .run_designmaker_$octHandle
	catch {$dmmControlPanel.des.resetStatus configure -relief sunken}
	set dmmActiveFlag($name) BUSY

	# save old highlight facet (lastFacet ptr) 
	# and set it to current universe
	set oldHighLightFacet [ptkGetHighlightFacet]
	ptkSetHighlightFacet $octHandle

	# save universe and set the new one to current universe
	set olduniverse [curuniverse]
	curuniverse $name

	# resetFacet defined in PDMM.cc
	resetFacet

	# restore lastFacet and curuniverse pointers
	curuniverse $olduniverse
	ptkSetHighlightFacet $oldHighLightFacet

	# raise button and make dmmActiveFlag IDLE
	catch {$dmmControlPanel.des.resetStatus configure -relief raised}
	set dmmActiveFlag($name) IDLE
}
#######################################################################
# Procedure to bring up the control panel for DesignMaker
#######################################################################
proc ptkRunDesignMaker {dmmUnivName dmmUnivOctHandle} {

    	global ptkOctHandles dmmActiveFlag 

    	set dmmControlPanel .run_designmaker_$dmmUnivOctHandle
    	if {[winfo exists $dmmControlPanel]} {
		wm deiconify $dmmControlPanel
		raise $dmmControlPanel
	    	return
    	}

    	set dmmActiveFlag($dmmUnivName) IDLE
	catch {destroy $dmmControlPanel}

    	set ptkOctHandles($dmmUnivName) $dmmUnivOctHandle

    	# Sets the Class of the Window.  This is used to set all options
    	#   for widgets used in the Contol window
    	toplevel $dmmControlPanel -class PigiControl
    	wm title $dmmControlPanel "Run-DesignMaker $dmmUnivName"
    	wm iconname $dmmControlPanel "Run-DesignMaker $dmmUnivName"

    	message $dmmControlPanel.msg \
	    	-width 25c \
	    	-text "DesignMaker \nControl panel for \"$dmmUnivName\"" \
	    	-justify center

    	# The following empty frames are created so that they are available
    	# to stars to insert custom controls into the control panel.
    	# By convention, we tend to use "high" for entries, "middle"
    	# for buttons, and "low" for sliders.  The full name for the
    	# frame is .run_${octHandle}.$position, where $name is the name of the
    	# universe, and $position is "high", "middle", or "low".
    	frame $dmmControlPanel.high
    	frame $dmmControlPanel.middle
    	frame $dmmControlPanel.low

	# the animation stuff
	global ptkGrphAnimationFlag
	set ptkGrphAnimationFlag 0

	global ptkTxtAnimationFlag
	set ptkTxtAnimationFlag 0

    	frame $dmmControlPanel.disfr
    	button $dmmControlPanel.disfr.dismiss -text "DISMISS" -command \
	"ptkRunDesignMakerDel $dmmUnivName $dmmControlPanel $dmmUnivOctHandle"
    	pack $dmmControlPanel.disfr.dismiss -side top -fill both -expand yes

    	frame $dmmControlPanel.abortfr
    	button $dmmControlPanel.abortfr.abort -text "ABORT" -command \
	"ptkRunDesignMakerAbort $dmmUnivName"
    	pack $dmmControlPanel.abortfr.abort -side top -fill both

	# Define the panel of control buttons
	# animation support
	frame $dmmControlPanel.anim -bd 10
            checkbutton $dmmControlPanel.anim.gr -text "Graphical Animation" \
                -variable ptkGrphAnimationFlag -relief flat \
                -command {ptkGrAnimation $ptkGrphAnimationFlag}

            checkbutton $dmmControlPanel.anim.tx -text "Textual Animation" \
                -variable ptkTxtAnimationFlag -relief flat \
                -command {ptkTxAnimation $ptkTxtAnimationFlag}

       	pack append $dmmControlPanel.anim \
		$dmmControlPanel.anim.gr {top frame w} \
		$dmmControlPanel.anim.tx {top frame w}

    	message $dmmControlPanel.runmsg \
	    	-width 25c -text "Flow Management" \
	    	-justify center

	frame $dmmControlPanel.flow -bd 10

	global dmm_prevFullName
	set dmm_prevFullName $dmmUnivName
	button $dmmControlPanel.flow.runAll -text "Run All" \
	-command "ptkRunAll $dmmUnivName $dmmUnivOctHandle" \
        	-width 20

	button $dmmControlPanel.flow.runUpto -text "Run Upto" \
		-command "ptkRunUpto $dmmUnivName $dmmUnivOctHandle" \
        	-width 20

	button $dmmControlPanel.flow.runThis -text "Run This" \
		-command "ptkRunThis $dmmUnivName $dmmUnivOctHandle" \
        	-width 20

	pack append $dmmControlPanel.flow \
		$dmmControlPanel.flow.runAll {top fill} \
		$dmmControlPanel.flow.runUpto {top fill} \
		$dmmControlPanel.flow.runThis {top fill}

    	message $dmmControlPanel.datamsg \
	    	-width 25c -text "Data Management" \
	    	-justify center

	frame $dmmControlPanel.data -bd 10
	button $dmmControlPanel.data.saveFacet -text "Save Facet" \
		-command "ptkSaveFacet $dmmUnivOctHandle" \
        	-width 20

	pack append $dmmControlPanel.data \
		$dmmControlPanel.data.saveFacet {left fill}

    	message $dmmControlPanel.desmsg \
	    	-width 25c -text "Design Management" \
	    	-justify center

	frame $dmmControlPanel.des -bd 10
	button $dmmControlPanel.des.resetStatus -text "Reset Design Status" \
		-command "ptkResetStatus $dmmUnivName $dmmUnivOctHandle" \
        	-width 20

	pack append $dmmControlPanel.des \
		$dmmControlPanel.des.resetStatus {top fill}

    	pack $dmmControlPanel.msg -fill both -expand yes
    	pack $dmmControlPanel.high -fill x -padx 10 -expand yes
    	pack $dmmControlPanel.middle -fill x -padx 10 -expand yes
	pack $dmmControlPanel.anim
	pack $dmmControlPanel.runmsg
	pack $dmmControlPanel.flow
	pack $dmmControlPanel.datamsg
	pack $dmmControlPanel.data
	pack $dmmControlPanel.desmsg
	pack $dmmControlPanel.des
    	pack $dmmControlPanel.low -fill x -padx 10 -expand yes
    	pack $dmmControlPanel.abortfr -fill both
    	pack $dmmControlPanel.disfr -fill both -expand yes

    	wm geometry $dmmControlPanel +400+400
    	focus $dmmControlPanel
    	wm protocol $dmmControlPanel WM_DELETE_WINDOW \
	"ptkRunDesignMakerDel $dmmUnivName $dmmControlPanel $dmmUnivOctHandle"

	set olduniverse [curuniverse]

	if {[catch {ptkCompile $dmmUnivOctHandle} msg] == 1} {
		# An error has occurred
		$dmmActiveFlag($dmmUnivName) IDLE
		ptkImportantMessage .error $msg
	}

	curuniverse $olduniverse
}
#######################################################################
# Procedure to delete a run-DesignMaker control window
#######################################################################
proc ptkRunDesignMakerDel { name window octHandle } {
	ptkClearHighlights
    	catch {unset ptkOctHandles($name)}
    	catch {unset dmmActiveFlag($name)}

	deluniverse $name

    	# most important thing: destroy the window
    	catch {destroy $window}
}
#######################################################################
# Procedure to halt a run
#######################################################################
proc ptkRunDesignMakerAbort { name } {
	global dmmActiveFlag
	if {![info exists dmmActiveFlag($name)]} {
		# ignore
		return
	}
	if {$dmmActiveFlag($name) == {IDLE}} {
		# ignore
		return
	}
	halt
}
#######################################################################
