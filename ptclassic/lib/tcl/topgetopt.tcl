#
# topgetopt.tcl
#
# The function has "top" prefix b/c it is conceptually part of my
# "top" library.
#
# Based on "@(#)getopt.tcl	1.5	12/7/91" by Phil Lapsley
#

proc topgetopt.tcl { } { }

# Simple "getopt" for TCL.
#
# option_list is a list of option specs.  Each spec is a 3-tuple
# consisting of the option name, a cooresponding
# tcl variable name in the callers environment, and a mode.  The
# variable name defaults to the option name, and the mode defaults to
# single. The modes:
# 	single:		sets the variable to the next argument.
# 	append:		lappends the next argument to the variable.
#			this allows multiple instances of the same option.
#	boolean:	sets the variable to 0 if the argument prefix is "+"
#			 and to 1 of the argument prefix is "-".
# "getopt" sets the
# variables named in the option_list that were specified in arg_list,
# and returns the remainder of arg_list after the first non "-" or "+" option.
# If a bad option specifier is encountered, scanning stops and getopt
# aborts using error.
#
# For example, the option_list:
#
#	{ min max { file filename } { toplevel toplevel boolean }
#
# means that the option "-min value" or "-max value" should set the
# variables "min" or "max" to the specified value, and "-file value"
# should set the variable "filename" to the specified value.  "toplevel"
# sets the variable "toplevel", and is a boolean.  I.e., the -toplevel
# option takes no argument.
#

proc topgetopt { option_list arg_list } {
    set n [llength $arg_list]
    for { set i 0 } { $i < $n } { incr i } {
	set arg [lindex $arg_list $i]
	set argkey [string index $arg 0]
	if { "$argkey"!="-" && "$argkey"!="+" } {
	    return [lrange $arg_list $i end]
	}
	set argname [string range $arg 1 end]
	set matched 0
	foreach opt $option_list {
	    if { "[lindex $opt 0]"=="$argname" } {
		set optlen [llength $opt]
		set pntVar pntVar$i
		upvar 1 [lindex $opt [expr { ($optlen > 1) ? 1 : 0 }]] $pntVar
		# lindex returns empty string for out-of-range
		case [lindex $opt 2] {
		  b* {
		    set $pntVar [expr {"$argkey"=="-" ? 1 : 0}]
		  }
		  a* {
		    lappend $pntVar [lindex $arg_list [incr i 1] ]
		  }
	          default {
		    set $pntVar [lindex $arg_list [incr i 1] ]
		  }
		}
# puts stdout "got [lindex $opt 0] -- [lindex $opt 1] -- [set $pntVar]"
		set matched 1
		break
	    }
	}
	if { $matched == 0 } {
	    error "No match for argument ``$arg''"
	}
    }
    return ""
}
