# Graphical interface to the tcl dynamic loader
#
# @Author: Christopher Hylands, Edward A. Lee
#
# @Version: $Id$
#
# @Copyright (c) 1996 The Regents of the University of California.
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
# 
#                                        PT_COPYRIGHT_VERSION_2
#                                        COPYRIGHTENDKEY
#######################################################################


catch {delete class ::tycho::Loader}
#######################################################################
#### Loader
# This class is an Interface to the tcl dynamic loader.  The tcl 
# dynamic loader can load in shared libraries and objects that contain
# C code that implement new tcl commands.
# <p>
# Here is an example of how to use the Loader:
# <tcl><pre>
#     ::tycho::Loader .x
# </pre></tcl>
#
class ::tycho::Loader {

    constructor {args} {}
    destructor {}
    
    ###################################################################
    ####                         public methods                    ####

    # # Load 'package' if 'command' is not present.
    method loadIfNotPresent {command package}

}

#######################################################################
#### constructor
#
body ::tycho::Loader::constructor {args} {
}


#######################################################################
#### destructor
#
body ::tycho::Loader::destructor {} {
}


    ###################################################################
    ###################################################################
    ####                      public methods                       ####

#######################################################################
#### NameA
# Load 'package' if 'command' is not present.
# If we can't load package, then search first in the Ptolemy tcl tree,
# Then prompt the user for the file to be loaded.
# If the package still cannot be loaded, return an error.
body ::tycho::Loader::loadIfNotPresent {command package} {
    global PTOLEMY env

    # "info procs" does not work in itcl, so we use "info which"
    if {[info which -command $command] == {}} {
        # Command is not loaded.  Load it.
	if [catch {package require $package} errMsg] {

	    # package require failed, so first we look in the Ptolemy Tree

	    set packageLibName \
		    $package[info sharedlibextension]
	    # We could be missing the library if it is not the case
	    # as the package.
	    set lowerCasePackageLibName \
		    [string tolower $package][info sharedlibextension]

	    if [info exists env(PTARCH)] {
		# If we are here, the Ptolemy is probably installed
		set PTARCH $env(PTARCH)
		set ptolemyLibList "$PTOLEMY/tcltk/itcl.$PTARCH/lib \
			$PTOLEMY/tcltk/itcl.$PTARCH/lib/itcl"
		foreach ptolemyLib $ptolemyLibList {
		    if [file isdirectory $ptolemyLib] {
			if { ![catch \
				{load $ptolemyLib/lib$packageLibName} ] || \
				![catch \
				{load \
				$ptolemyLib/lib$lowerCasepackageLibName} ] } {
			    # We loaded ok, so we are done.
			    return
			}
		    }
		}
	    }

	    while {1} {
		# We did not load ok, so we prompt the user.
		set loadLibPath [::tycho::DialogWindow::newModal FileBrowser \
			[::tycho::autoName .fileBrowser] \
			-bitmap questhead \
			-text \
			"Enter the name of the library that contains the\n command `$command' in the package `$package'" ]

		puts $loadLibPath
		load $loadLibPath
	
		if {[info which -command $command] != {}} {
		    # We loaded the command so exit the while loop
		    puts "----[info which -command $command]"
		    break
		}
		# That didn't work, try again
		set tryAgain [::tycho::DialogWindow::newModal YesNoQuery .y \
			-text \
			"Command `$command' still does not exist\nTry again?"]
		if !$tryAgain {
		    error "`$command' not dynamically loaded"
		}
	    }
		
	}
    }
}

::tycho::Loader .x
.x loadIfNotPresent fooprofile tclxfoo

