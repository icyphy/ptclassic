# Tcl/Tk source for a display that shows the values of the star inputs
#
# Author: Edward A. Lee
# Version: $Id$
#
# Copyright (c) 1993 The Regents of the University of California.
# All rights reserved.
# See the file $PTOLEMY/copyright for copyright notice,
# limitation of liability, and disclaimer of warranty provisions.
#

set s $ptkControlPanel.label_$starID

# If a window with the right name already exists, we assume it was
# created by a previous run of the very same star, and hence can be
# used for this new run.

if {![winfo exists $s]} {

    proc tkShowValueMakeWindow {putInCntrPan win label numInputs univ starID} {
	global ptkControlPanel
        if {$putInCntrPan} {
	    frame $win
	    pack after $ptkControlPanel.low $win top
        } {
            toplevel $win
            wm title $win "Show Values"
            wm iconname $win "Show Values"
        }

        frame $win.f
        message $win.msg -width 12c -text $label

	# The following flag is used if the waitBetweenOutputs parameter is set
	global $starID
	set ${starID}(tkShowValueWaitTrig) 0
        for {set i 0} {$i < $numInputs} {incr i} {
	    frame $win.f.att$i -class Attention
    	    button $win.f.att$i.button -relief raised -width 40 \
		-command "incr ${starID}(tkShowValueWaitTrig)" \
		-state disabled
	    pack append $win.f.att$i $win.f.att$i.button \
	    		{top frame w pady 4 expand filly}
	    pack append $win.f $win.f.att$i {top expand filly}
        }
        pack append $win $win.msg {top expand} $win.f top

        if {!$putInCntrPan} {
            button $win.ok -text "DISMISS" -command \
		"catch {incr ${starID}(tkShowValueWaitTrig)}
		 ptkStop $univ
		 destroy $win"
            pack append $win $win.ok {top fillx}
        }

	if {[set ${starID}(wait_between_outputs)]} {
	    # Modify the control panel stop button to release the wait
	    # HACK ALERT: highly non-local code.  Will work only if the
	    # the design of the control panel does not change.
	    $ptkControlPanel.panel.stop configure -command \
		"incr ${starID}(tkShowValueWaitTrig); ptkStop $univ"
	    bind $ptkControlPanel.iter.entry <Escape> \
		"incr ${starID}(tkShowValueWaitTrig); ptkStop $univ"
	    bind $ptkControlPanel <Escape> \
		"incr ${starID}(tkShowValueWaitTrig); ptkStop $univ"
	}
    }

    tkShowValueMakeWindow [set ${starID}(put_in_control_panel)] \
	$s [set ${starID}(label)] \
	[set ${starID}(numInputs)] [curuniverse] $starID

    proc tkShowValueSetValues {starID numInputs win} {
        set c $win.f
        set inputVals [grabInputs_$starID]
	for {set i 0} {$i < $numInputs} {incr i} {
            set in [lindex $inputVals $i]
            $win.f.att$i.button configure -text $in
	}
    }

    proc tkShowValueWait {flag starID numInputs win} {
	if {$flag} {
	    global $starID
	    set ${starID}(tkShowValueWaitTrig) 0
	    for {set i 0} {$i < $numInputs} {incr i} {
	        $win.f.att$i.button configure -state normal
	    }
	    tkwait variable ${starID}(tkShowValueWaitTrig)
	    for {set i 0} {$i < $numInputs} {incr i} {
		# In case the window was dismissed, trap errors
	        catch {$win.f.att$i.button configure -state disabled}
	    }
	}
    }

    # In the following definition, the value of $s
    # is evaluated when the file is sourced.
    proc goTcl_$starID {starID} "
        tkShowValueSetValues $starID [set ${starID}(numInputs)] $s
	tkShowValueWait [set ${starID}(wait_between_outputs)] $starID \
		[set ${starID}(numInputs)] $s
    "

    proc destructorTcl_$starID {starID} {
        global $starID
        if {[set ${starID}(put_in_control_panel)]} {
            # Remove the meters from the control panel, if they still exist
            global ptkControlPanel
            destroy $ptkControlPanel.label_$starID
        }
    }
}
unset s
