##########################################################################
#
# Author:  Jose Luis Pino
#
# Version: $Id$
#
# usage: safeTycho tychoScriptFile
#
# Tcl routines to run a 'safe' tycho script.  There are currently two commands
# that pass trough from our safe tycho shell to a tycho process:
#
# open a file in tycho:
#     openContext <fileName> {setInsert <position>}
# source a local tycho tcl/tk script:
#     source <fileName>:
#
# For a listing of standard safe tcl commands, see the tcl manual page
# for the command interp
#
##########################################################################
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
##########################################################################

# It would be useful if the script writer could specify which tycho
# binary to run - by they are all registered as 'tycho' so until that
# is changed, we'll not be able to support this.  
# Another issue - is that we must insure that the tycho binary the script
# writer requests is actually a tycho binary.
# When these issues are addressed this procedure could be registered with 
# safe slave tcl interpreter.

proc tychoInterpreter {{binary "$env(PTOLEMY)/bin.$env(PTARCH)/tysh.ptrim"}} {
    set tclProgs [winfo interps]
    if { [expr ![regexp {tycho} $tclProgs]] } {
	exec tycho -ptrim &
	while { [expr ![regexp {tycho} $tclProgs]] } {
	    set tclProgs [winfo interps]
	}
    }
    foreach prog $tclProgs {
	if {[regexp {tycho} $prog]} {
	    return $prog
	}
    }
}

# check if a string may have embedded tcl code - embedded tcl code
# could compromise our security
proc embeddedCode {string} {
    return [regexp {;|\[|\]+} $string]
}

# open a file in tycho given a path and an option position
proc openContext {fileName {position 0}} {
    global tycho
    if { [file exists $fileName] } {
	if {$position == 0} {
	    send $tycho File::openContext $fileName
	} else {
	    if {[embeddedCode $position]} {
		puts stderr "Position '$position' for $fileName may contain embedded TCL code."
	    } else {
		send $tycho eval \
		    "\[File::openContext $fileName\] setInsert $position"
	    }
	}
    } else {
	puts stderr "File $fileName does not exist"
	exit
    }
}    

# run a tcl script in a slave safe tcl interpreter
proc runScriptSafely {fileName} {
    global safeInterp
    if [catch {open $fileName r} fileId] {
	puts stderr "Cannot open $fileName: $fileId"
	exit
    }
    while {[gets $fileId line] >= 0 } {
	$safeInterp eval $line
    }
    close $fileId
}

# source a local tcltk script in tycho.  We assume local scripts are
# not unsecure - is this a bad assumption?  If not, we could allow 
# the site to have a master list of secure paths and make sure the file
# is in that path
proc tychoSource {fileName} {
    global tycho
    if { [file exists $fileName] } {
	send $tycho source $fileName
    } else {
	puts stderr "Script $fileName not found"
    }
}

# declare my intepreter type
tk appname safeTycho

# create a safe slave tcl/tk interp
set safeInterp [interp create -safe]

# declare the valid tycho functions for this slave interpreter
$safeInterp alias openContext openContext
$safeInterp alias source tychoSource

# declare a $PTOLEMY variable so that $PTOLEMY path names can be
# supplied by script
if {[info exists env(PTOLEMY)]} {
    $safeInterp eval set PTOLEMY $env(PTOLEMY)
} else {
    $safeInterp eval set PTOLEMY [glob ~ptolemy]
}

# find a tycho interpreter to attach to - if one doesn't exist - start one
set tycho [tychoInterpreter]

# run the downloaded script in our safe slave interpreter
runScriptSafely $argv

exit
    
