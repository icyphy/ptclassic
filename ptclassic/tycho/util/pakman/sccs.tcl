# sccs.tcl: Procs to place files under sccs control
#
# @Author: John Reekie
#
# @Version: $Id$
#
# @Copyright (c) 1998 The Regents of the University of California.
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
#### sccs
# Place files under SCCS control.
#
proc ::pakman::sccs {path args} {
    global env

    set cwd [pwd]
    cd $path

    array set options $args
    set debug $options(-debug)
    puts [pwd]

    # Create the SCCS directory
    if ![file exists SCCS] {
	puts "  SCCS"
	file mkdir SCCS
    }

    # Execute script
    if { $options(-command) != "" } {
	catch {uplevel #0 $options(-command)} msg
	if $options(-debug) {
	    puts $msg
	}
    }

    # Check in files
    foreach file [glob -nocomplain *] {
	if { [file isfile $file] \
		&& ![file exists [file join SCCS s.$file]] } {
	    catch {exec sccs create $file} msg
	    if $debug {
		regsub -all "\n" $msg ", " msg
		puts "Checking in $file: $msg"
	    }
	    catch {file delete ,$file}
	}
    }

    # Process subdirectories
    set subdirs {}
    set exclude [concat $options(-exclude) SCCS RCS CVS]
    foreach subdir [glob -nocomplain *] {
	if { [file type $subdir] == "directory" } {
	    if { [lsearch -exact $exclude $subdir] == -1 } {
		catch {eval ::pakman::sccs [list $subdir] $args}
	    }
	}
    }
    cd $cwd
}
