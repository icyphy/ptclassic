# main.tcl: Main procs of pakman
#
# @Author: John Reekie
#
# @Version: @(#)main.tcl	1.4 02/11/98
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
#### pakman
#
# The top-level entry procedure. If no arguments are given,
# print the help information.
#
proc ::pakman {{subcommand {}} args} {
    if { $subcommand == "" } {
	::pakman::help
	return
    }
    switch -exact -- $subcommand {
	"create" {
	    set pkgname [lindex $args 0]
	    set args [lreplace $args 0 0]
	    set opts [::pakman::getopt -flags {
		-debug -help -silent
	    } -options {
		-path {} -style {} -sccs 0 -template {} -version 1.0
	    } -aliases {
		-d -debug -s -silent -p -path
	    } -arguments $args -anyoption 1]

	    eval ::pakman::create $pkgname $opts
	}
	"sccs" {
	    set path [lindex $args 0]
	    set args [lreplace $args 0 0]
	    set opts [::pakman::getopt -flags {
		-debug -help -silent
	    } -options {
		-command {} -exclude {}
	    } -aliases {
		-d -debug -s -silent -p -path
	    } -arguments $args]

	    eval ::pakman::sccs [list $path] $opts
	}
	"mirror" {
	    puts "Not implemented yet"
	}
	"version" {
	    puts "Not implemented yet"
	}
	"provides" {
	    puts "Not implemented yet"
	}
	"requires" {
	    puts "Not implemented yet"
	}
	"help" {
	    ::pakman::help
	}
	default {
	    error "Unknown subcommand \"$subcommand\""
	}
    }
}


#######################################################################
#### help
# Print help information. To generate this quickly, open the
# documentation in netscape and just cut and paste
# the relevant text!
# 
proc ::pakman::help {} {
    # FIXME
    puts {
     -d 
     -debug 
          Print debugging information. 
     -h 
     -help 
	Print out this list of options. 
     -s 
     -silent 
     -quiet 
          Print no information at all to stdout or stderr. 
    }
}

#######################################################################
#### getopt
# Process command-line arguments, and return a list that
# can be used to set an array. The arguments are option
# value pairs, where the following will be accepted:
#
# <dl> 
# <dt><b>-aliases</b> <i>list</i>
# <dd>A list of option or flag names and an "alias" which
# will appear in the result list.
# 
# <dt><b>-arguments</b> <i>list</i>
# <dd>The command-line arguments.
# 
# <dt><b>-flags</b> <i>list</i>
# <dd>A list of accepted flags.
#
# <dt><b>-options</b> <i>list</i>
# <dd>A list of accepted options and their initial values.
#
# <dt><b>-anyflag</b> <i>boolean</i>
# <dd>If true, any flag will be accepted. The default is false.
#
# <dt><b>-anyoption</b> <i>boolean</i>
# <dd>If true, any option will be accepted. The default is false.
#
# <dt><b>-anyvalue</b> <i>boolean</i>
# <dd>If true, any value that is not attached to an option will
# be accepted. The default is false.
#
# </dl>
#
# The <b>anyflag</b> and <b>-anyoption</b> options are mutually
# exclusive. Any unrecognized flags, options, or values will be returned
# in the result list with the null string as index.
#
proc ::pakman::getopt {args} {
    array set opts {
	-flags {} -options {}
	-anyflag 0 -anyoption 0 -anyvalue 0
	-arguments {}
    }
    array set opts $args
    foreach flag $opts(-flags) {
	set _f($flag) 0
    }
    array set _o $opts(-options)
    array set result [concat $opts(-options) [array get _f]]
    array set _a $opts(-aliases)

    # Process command-line args
    set arguments $opts(-arguments)
    while { $arguments != "" } {
	set option [lindex $arguments 0]
	set arguments [lreplace $arguments 0 0]
	if ![string match {-*} $option] {
	    # Got funny argument
	    if $opts(-anyvalue) {
		lappend result() $option
	    } else {
		error "Invalid argument: \"$option\""
	    }
	    continue

	} else {
	    # Remap if needed
	    if [info exists _a($option)] {
		set option $_a($option)
	    }
	    # Check for flag
	    if [info exists _f($option)] {
		if [info exists _a($option] {
		    set result($_a($option)) 1
		} else {
		    set result($option) 1
		}
	    } elseif [info exists _o($option)] {
		# Check for option
		if [info exists _a($option] {
		    set result($_a($option)) [lindex $arguments 0]
		} else {
		    set result($option) [lindex $arguments 0]
		}
		set arguments [lreplace $arguments 0 0]
	    } elseif $opts(-anyflag) {
		set result($option) 1
	    } elseif $opts(-anyoption) {
		set result($option) [lindex $arguments 0]
		set arguments [lreplace $arguments 0 0]
	    } else {
		error "Unknown option \"$option\""
	    }
	}
    }
    return [array get result]
}
