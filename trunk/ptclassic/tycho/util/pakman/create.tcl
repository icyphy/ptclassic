# create.tcl: Procs to create a new Tcl package
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
#### create
# Create a new package. The first argument is the name of
# the package. Following arguments are option-value
# pairs. See doc/index.html for the options. By default,
# a new directory is created forthe package with the same
# name as the package but in lower-case. If the directory
# already exists, the script will proceed, but will only
# create files that do not already exist. (This can be useful
# for updating packages if the package format changes.)
#
proc ::pakman::create {pkgname args} {
    global env

    array set options $args

    # Figure the default path
    if { $options(-path) == "" } {
	set tail [string tolower [lindex [split $pkgname .] end]]
	set options(-path) [file join [file dirname $pkgname] $tail]
    } else {
	set tail [string tolower [file tail $options(-path)]]
    }
    # Figure the template directory
    if { $options(-template) == "" } {
	if { $options(-style) == "" } {
	    # Look for template
	    set t [file join [pwd] template]]
	    if { [file exists $t] && [file isdirectory $t] } {
		set options(-template) $t
	    } else {
		set t [file join $env(PAKMAN_LIBRARY) template]
		if { [file exists $t] && [file isdirectory $t] } {
		    set options(-template) $t
		} else {
		    set options(-style) "simple"
		}
	    }
	}
    }
    if { $options(-template) == "" } {
	set t [file join [pwd] template.$options(-style)]]
	if { [file exists $t] && [file isdirectory $t] } {
	    set options(-template) $t
	} else {
	    set t [file join $env(PAKMAN_LIBRARY) template.$options(-style)]
	    if { [file exists $t] && [file isdirectory $t] } {
		set options(-template) $t
	    } else {
		error "Could not find template directory: $t"
	    }
	}
    }
    # Get the copyright
    foreach f {copyright Copyright COPYRIGHT} {
	if [file exists [file join $options(-template) $f]] {
	    set fd [open [file join $options(-template) $f]]
	    set options(-copyright) [read $fd]
	    close $fd
	    break
	}
    }

    # Call the auxiliary function to recursively process options
    eval ::pakman::_create $options(-template) $options(-path) \
	    $pkgname [array get options]
}

#######################################################################
#### _create
# Helper function for creating a new package. This function
# is called recursively.
#
proc ::pakman::_create {template dir pkgname args} {
    array set options $args

    # Create the directory
    if ![file exists $dir] {
	if $options(-debug) {
	    puts "Creating directory $dir"
	} elseif !$options(-silent) {
	    puts "$dir"
	}
	file mkdir $dir
    } else {
	if $options(-debug) {
	    puts "Entering directory $dir"
	}
    }

    # It is possible that the template directory doesn't
    # exist, so check first and return if not
    if ![file exists $template] {
	return
    }

    # Get substitutions
    array set substs $args
    unset substs(-silent) substs(-debug) substs(-path)
    set substs(-PACKAGE) [string toupper $pkgname]
    set substs(-Package) $pkgname
    set substs(-package) [string tolower $pkgname]
    set tail [lindex [split $pkgname .] end]
    set substs(-PKG) [string toupper $tail]
    set substs(-Pkg) $tail
    set substs(-pkg) [string tolower $tail]

    # Copy default files, substitute package name etc
    foreach file [glob -nocomplain [file join $template _*]] {
	regsub ^_ [file tail $file] {} file
	
	# Filter out irritating crap
	if { [string match {*~} $file] \
		|| [string match {#*#} $file] } {
	    continue
	}

	# Any occurrence of @ has the package name tail
	# subsituted
	regsub -all {@} $file $substs(-pkg) dest

	# Process each file in the directory
	if [file exists [file join $dir $dest]] {
	    if $options(-debug) {
		puts "Ignoring [file join $dir $dest]"
	    }
	} else {
	    set fd [open [file join $template _$file]]
	    set text [read $fd]
	    close $fd

	    regsub -all %percent $text % text
	    if [info exists substs(-copyright)] {
		regsub -all %copyright% $text $substs(-copyright) text
	    }
	    foreach {opt val} [array get substs] {
		if { $opt != "-copyright" } {
		    regsub -all %[string trimleft $opt -]% $text $val text
		}
	    }
	    
	    if $options(-debug) {
		puts "Writing [file join $dir $dest]"
	    } elseif !$options(-silent) {
		puts "  $dest"
	    }
	    set fd [open [file join $dir $dest] "w"]
	    puts $fd $text
	    close $fd

	    # Check the file in
	    if { $options(-sccs) != 0 } {
		::pakman::sccscreate $dir $dest \
			$options(-sccs) $options(-debug)
	    }
	}
    }

    # Process subdirectories
    set subdirs {}
    foreach subdir [glob -nocomplain [file join $template *]] {
	if [file isdirectory $subdir] {
	    if { [lsearch -exact {SCCS RCS CVS} $subdir] == -1 } {
		lappend subdirs [file tail $subdir]
	    }
	}
    }
    foreach subdir $subdirs {
	eval ::pakman::_create \
		[list [file join $template $subdir]] \
		[list [file join $dir $subdir]] \
		[list $pkgname] \
		$args
    }
}

#######################################################################
#### sccscreate
# Create an SCCS directory in the given directory if there
# isn't one, and then check the file in.
#
proc ::pakman::sccscreate {dir file script {debug {}}} {
    if ![file exists [file join $dir SCCS]] {
	if $debug {
	    puts "Creating directory [file join $dir SCCS]"
	}
	file mkdir [file join $dir SCCS]

	if { $script != "" } {
	    set cwd [pwd]
	    cd $dir
	    catch {uplevel #0 $script} msg
	    cd $cwd
	    if $debug {
		puts $msg
	    }
	}
    }

    if { ![file exists [file join $dir SCCS s.$file]] } {
	set cwd [pwd]
	cd $dir
	catch {exec sccs create $file} msg
	if $debug {
	    regsub -all "\n" $msg ", " msg
	    puts "Checking in $file: $msg"
	}
	catch {file delete ,$file}
	cd $cwd
    }
}
