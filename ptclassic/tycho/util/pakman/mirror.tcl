# mirror.tcl: Procs to make a mirror of a directory
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
#### mirror
# Make a mirror of a directory. The path and mirror directories are
# traversed, and the "mirror" directory is given the same structure
# as the path directory. In each directory, any directories in all-caps
# are linked to symbolically instead of being created. After
# completion, the mirror tree has "make sources" executed in it.
#
proc ::pakman::mirror {path mirror args} {
    global env

    set cwd [pwd]

    cd $path
    set path [pwd]
    cd $cwd

    array set options $args
    set debug $options(-debug)

    # Create this directory
    if ![file exists $mirror] {
	file mkdir $mirror
    }
    cd $mirror
    set mirror [pwd]

    cd $path
    puts $mirror

    # Figure out what to link and what to expand
    set exclude [concat $options(-exclude) SCCS RCS CVS]
    set include $options(-include)
    set link {}
    set expand {}
    foreach dir [glob -nocomplain *] {
	if { [file type $dir] != "directory" } {
	    if { [string match {makefile*} $dir] \
		    || [string match {Makefile*} $dir] \
		    || [string match {MAKEFILE*} $dir] } {
		lappend link $dir
	    }
	    continue
	}
	if { [lsearch -exact $exclude $dir] >= 0 } {
	    lappend link $dir
	} elseif { [lsearch -exact $include $dir] >= 0 }  {
	    lappend expand $dir
	} else {
	    if { [file exists [file join $path $dir SCCS]] \
		    || [file exists [file join $path $dir RCS]] \
		    || [file exists [file join $path $dir CCS]] } {
		lappend expand $dir
	    } else {
		lappend link $dir
	    }
	}
    }

    # Make symbolic links
    foreach dir $link {
	if ![file exists [file join $mirror $dir]] {
	    if $debug {
		puts "Linking [file join $mirror $dir]"
	    }
	    puts "exec ln -s \
		    [file join $path $dir] \
		    [file join $mirror $dir]"
	    catch {exec ln -s \
		    [file join $path $dir] \
		    [file join $mirror $dir]} msg
	    if $debug {
		puts $msg
	    }
	}
    }

    # Execute script
    if { $options(-command) != "" } {
	catch {uplevel #0 $options(-command)} msg
	if $debug {
	    puts $msg
	}
    }

    # Expand subdirectories
    foreach dir $expand {
	if $debug {
	    puts "Expanding [file join $mirror $dir]"
	}
	puts "mirror \
		[list [file join $path $dir] [file join $mirror $dir]] \
		$args"

	eval mirror \
		[list [file join $path $dir] [file join $mirror $dir]] \
		$args
    }
    cd $cwd
}
