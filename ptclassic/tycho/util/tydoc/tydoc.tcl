# Package load file for the tycho.util.tydoc package
#
# @Author: John Reekie
#
# @Version: @(#)tydoc.tcl	1.14 01/12/99
#
# @Copyright (c) 1995-1998 The Regents of the University of California.
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


# Based on pp 344-346 of Harrison and McClellan's "Effective Tcl/Tk
# Programming" book

package provide tycho.util.tydoc 2.0

# Don't include 'package require tycho.kernel.basic', you will break
# the standalone tydoc distribution.  Instead, we define namespaceTail,
# namespaceQualifiers and  infoWhichCommand locally
#package require tycho.kernel.basic

### Start of code duplicated from tycho/kernel/basic/Compat.tcl

##########################################################################
#### namespaceTail
# 
# Use this instead of "namespace tail" or "info namespace tail"
#
proc namespaceTail {n} {
    global tcl_version
    if { $tcl_version >= 8.0 && ${itcl::version} >= 3.0 } {
	return [namespace tail $n]
    } else {
	return [info namespace tail $n]
    }
}

##########################################################################
#### namespaceQualifiers
# 
# Use this instead of "info namespace qualifiers"
#
proc namespaceQualifiers {n} {
    global tcl_version
    if { $tcl_version >= 8.0 && ${itcl::version} >= 3.0 } {
	return [namespace qualifiers $n]
    } else {
	return [info namespace qualifiers $n]
    }
}

##########################################################################
#### infoWhichCommand
# 
# Use this instead of "info which -command foo" or "info commands foo."
#
proc infoWhichCommand {c} {
    global tcl_version
    if { $tcl_version >= 8.0 && ${itcl::version} >= 3.0 } {
	uplevel info commands $c
    } else {
	uplevel info which -command $c
    }
}

### End of code duplicated from tycho/kernel/basic/Compat.tcl

global env auto_path
set env(TYDOC_LIBRARY) [file dirname [info script]]
if { [lsearch -exact $auto_path $env(TYDOC_LIBRARY)] == -1 } {
    lappend auto_path $env(TYDOC_LIBRARY)
}

######################################################################
#### tydoc
# This proc generates HTML frome itcl files
# Usage: tydoc [-v] [-t "title"] itcl_file [itcl_file . . .] 
# -v: verbose html output
# -t "title": use "title" as the title for the index page
#
proc tydoc {args} {
    global errorInfo
    set argc [llength $args]
    set title "Index of classes"
    set verbose 0
    set debug 0
    set switchCount 0
    for {set n 0} {$n < $argc} {incr n} {
	if { [ lindex $args $n] == "-t" } {
	    set title [lindex $argc [incr n]]
	    incr switchCount 2
	}
	if { [ lindex $args $n] == "-v" } {
	    set verbose 1
	    incr switchCount
	} 
	if { [ lindex $args $n] == "-d" } {
	    set debug 1
	    incr switchCount
	}
	if { [ lindex $args $n] == "-notestdir" } {
	    if {"[file tail [pwd]]" == "test"} {
		puts "tydoc called in a directory named test,\
			so no documentation need be generated"
		exit
	    }
	    incr switchCount
	}
    }

    if {$argc == 0 || $argc == $switchCount} {
	puts "tydoc called with no files,\
		so no documentation need be generated"
	exit
    }



    set generateIndex 1
    set errorInfo ""
    set retval ""
    if [catch {set retval [::tycho::HTMLDocSys::generateHTML $verbose $debug \
	    $generateIndex $title \
	    [lrange $args $switchCount end]]} err ] {
	puts "tydoc: $err\n$errorInfo"
	
    }
    if {$retval != ""} {
	puts "tydoc returned $retval\n$errorInfo"
	exit 3
    }
}
