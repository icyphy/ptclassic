# Graphical interface to the tcl dynamic loader
#
# @Author: Christopher Hylands, Edward A. Lee
#
# @Version: @(#)Loader.tcl	1.14	07/21/98
#
# @Copyright (c) 1996-1998 The Regents of the University of California.
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


#######################################################################
#### loadIfNotPresent
# Load 'package' if 'command' is not present.
# If we can't load package, then search first in the Ptolemy tcl tree,
#
# If tk is not present, and we can't load the package, we call error.
#
# If tk is present, then prompt the user for the file to be loaded.
# If the package still cannot be loaded, return an error.  
#
# Note that this is somewhat different from most of the other procs and
# classes in kernel/basic in that it will bring up a window if tk is
# present.
# 
# The optional packagePathList argument consists of a list of 2 element
# sublists that contain other package names and pathnames to try and load
# packagePathList can be used to try to load the package under an alternative
# name.  The sublists consists of two elements, an alternative packagename
# and a directory to try loading in.  The alternative packagename can be
# empty.
#
# The example below attempts to load tytimer.so
# <tcl><pre>
# ::tycho::loadIfNotPresent ::tycho::timer tytimer
# </pre></tcl>
#
proc ::tycho::loadIfNotPresent {command package {packagePathList {}}} {
    global PTOLEMY env TYCHO

    # "info procs" does not work in itcl, so we use "info which"
    if {[infoWhichCommand $command] == {}} {
        # Command is not loaded.  Load it.
	if [catch {package require $package} errMsg] {

	    # package require failed, so first we look in the Ptolemy Tree

	    set packageLibName \
		    lib$package
	    # We could be missing the library if it is not the case
	    # as the package.
	    set lowerCasePackageLibName \
		    lib[string tolower $package]

	    if { $packageLibName != $lowerCasePackageLibName } {
		set libNameList [list $packageLibName $lowerCasePackageLibName]
	    } else {
		set libNameList [list $packageLibName]
	    }

	    set libDirList ""
	    if [info exists env(PTARCH)] {
		set PTARCH $env(PTARCH)
		if [file isdirectory [file join $TYCHO lib.$PTARCH]] {
		    lappend libDirList \
			    [file join $TYCHO lib.$PTARCH]
		}
	    }
	    if {[info exists env(PTARCH)] && \
		    [info exists PTOLEMY]} {
		# If we are here, the Ptolemy is probably installed
		set PTARCH $env(PTARCH)
		
		lappend libDirList \
			[file join $PTOLEMY tcltk itcl.$PTARCH lib]
		lappend libDirList \
			[file join $PTOLEMY tcltk itcl.$PTARCH lib itcl]

            }

            # Get the package name and directory from packagePathList
            foreach packagePath $packagePathList {
                # packagePath is a list: { {altpackagename} {altlibdir}}
                if {[llength [lindex $packagePath 0]] > 0} {
		    if {[lsearch \
			    $libNameList [lindex $packagePath 0]]  == -1 } {
			lappend libNameList [lindex $packagePath 0]
		    }
                }
                if {[llength [lindex $packagePath 1]] > 0} {
		    if {[lsearch $libDirList [lindex $packagePath 1]] == -1 } {
			lappend libDirList [lindex $packagePath 1]
		    }
                }
            }
            

            # Try to load each package from each directory.
            # If we load successfully, then return.
            foreach libDir $libDirList {
                if [file isdirectory $libDir] {
                    foreach libName $libNameList {
			set libFile [file join $libDir \
				$libName[info sharedlibextension]]
			puts "loading $libFile"
			if [file exists $libFile] {
			    if [catch {load $libFile} errMsg] {
				if [info exists tk_version] {
				    # We had an error, and we have Tk
				    # Show the error message and bring up 
				    # a yes/no/cancel window
				    set nm [::tycho::autoName .loaderYNC]
				    ::tycho::YesNoQuery $nm \
					    -text "load $libFile\nfailed with\
					    the following error message:\n\
					    $errMsg\n\
					    Do you want to try other\
					    directories?"
				    set response [::tycho::Dialog::wait $nm]
				    if {$response == 0 } {
					::tycho::silentError
				    }
				} else {
				    # No Tk, so call error
				    error "Failed to load `$libFile':\n\
					    $errMsg"
				}
			    } else {
				# We loaded ok, so we are done.
				return
			    }
			}
		    }
		}
            }

	    while [info exists tk_version] {
		# We did not load ok, so we prompt the user.
		set loadLibPath [::tycho::queryfilename \
			"Enter the name of the library that contains the\n \
                        command `$command' in the package `$package'\n\
                        The following directories were searched:\n\
                        $libDirList\n\
                        for the following packages:\n\
                        $libNameList"]

		if {$loadLibPath == {} } {
		    ::tycho::silentError
		}

		load $loadLibPath
		if {[infoWhichCommand $command] != {}} {
		    # We loaded the command so exit the while loop
		    break
		}
		# That didn't work, try again
		if ![::tycho::askuser \
                        "Command `$command' still does not exist\nTry again?"] {
		    error "`$command' not dynamically loaded"
		}
	    }
	}
    }
}
