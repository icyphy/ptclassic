# Pathname utilities that don't use the window system, that
# deal with URL-style names and Resources.
#
# @Author: John Reekie
#
# @Version: @(#)URLPath.tcl	1.10 04/29/98
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
#######################################################################

##########################################################################
#### readFileHeader
#
# Read the header string from a file and set the elements of
# the passed array, with each index being the name of a headed
# variable. The file name can be a local file name or a
# URL-style name. Return 1 if a header was found and parsed,
# or zero if there was no header or it was malformed. The
# header can be multiline.
#
proc ::tycho::readFileHeader {filename var} {
    upvar $var v
    set fo [::tycho::resource new $filename]
    $fo open
    set line [$fo gets]
    while { ![$fo eof] && $line == "" } {
	set line [$fo gets]
    }
    set string ""
    while { [regexp -- {-\*-.*-\*-} $line line] } {
        append string $line\n
        set line [$fo gets]
    }
    $fo close
    delete object $fo
    if { $string == "" } {
        return 0
    } else {
        return [::tycho::parseHeaderString $string v]
    }
}

#####################################################################
#### url
# Manipulate url-style filenames. This procedure is needed because
# the Tcl file{} procedure does not correctly work with names like
# <code>http://foo/bar</code>. Until file{} is fixed, it is recommended
# that <code>::tycho::url</code> be used instead of file{} to
# manipulate file names in any situation where a filename might
# be a url-style name.
#
# The syntax of a url-style name is:
# <br>
# [<i>protocol</i><b>:</b>] [//<i>server</i>] <i>path</i>
# <br>
# Note that the protocol and server parts are optional, so regular
# filenames are supported too. In this case, the name will be called
# a <i>local name</i>; otherwise, it is called a <i>network name</i>.
# The protocol is allowed to consist only of <i>lower-case alphabetic
# characters</i>.
#
# This procedure supports platform-independence
# in a similar way to file{}:
# <ul>
# <li>If the name is a network name, such as
# <code>http://</code> or <code>file:/</code>, then URL-style
# slashes are assumed for the file path. On Windows and the Mac,
# backslashes and colons are treated as part of the path, not
# as directory separators.
# <li>Otherwise, the filename is manipulated however file{} does it,
# so platform-dependent local names work properly when passed through
# this procedure.
# </ul>
#
# The *split* and *join* cases are treated specially.
# In this case, a network name is represented as a list containing
# two extra elements at the head of the list: the protocol name
# and the server name (with an optional port number attached).
# For example, the platform-independent equivalent of the
# URL <code>http://ptolemy.eecs/foo/bar</code> is
# <code>{http: ptolemy foo bar}</code>;  the platform-independent
# equivalent of the URL <code>file:/users/johnr/</code> is
# <code>{file: {} users johnr}</code> (note empty server name).
# Note that, since all network-style names are absolute, the
# list does not contains a "/" character, as it does for absolute
# local names.
# Local names are treated exactly as for Tcl file{},
# so that the the platform-independent equivalent of
# <code>/users/ptolemy</code> is <code>/ users ptolemy</code>.
# In addition to the file-name manipulation options of Tcl
# file{}, this procedure contains some additional options for
# testing for and extracting the protocol option, and so on.
#
# The valid options are:
# <ul>
# <li><code>::tycho::url aslocal</code> _name_: Convert a
# local name using url-style slashed into a platform
# dependent name. This is useful for converting names
# such as <code>file:/users/ptolemy</code> into the local
# equivalent.
# <li><code>::tycho::url dirname</code> _name_: Return the name
# excluding the last component. Tilde substitution is performed
# only on local names. If the name is a network name and the
# path is empty, return the name unchanged.
# <li><code>::tycho::url expand</code> _name_: Return a
# "normalized" pathname. If a local name, the result is
# obtained from <code>::tycho::expandPath</code>. If a
# network name, the result has components like <code>..</code>
# removed.
# <li><code>::tycho::url join</code> _name_ ?_name_ ...?: Return the
# platform-dependent path name from the arguments. If the first
# element of the list if a protocol name, then construct a network
# name using slashes; if not, then construct a platform-dependent
# local name.
# <li><code>::tycho::url extension</code> _name_: Return the character
# after the last period in the last component of the name.
# <li><code>::tycho::url join</code> _name_ ?_name_ ...?: Return the
# platform-dependent path name from the arguments. If the first
# element of the list if a protocol name, then construct a network
# name using slashes; if not, then construct a platform-dependent
# local name.
# <li><code>::tycho::url path</code> _name_: Return the
# path component of the name. If it is a local name,
# return the name. (In this case, the result is not
# a platform-dependent name -- to get one, use the
# *aslocal* option.) If a network name and no path is given,
# return <code>/</code>.
# <li><code>::tycho::url pathtype</code> _name_: Return the
# "type" of a path -- absolute, relative, or volumerelative.
# If _name_ is a network name, always return
# <code>absolute</code>. If it is a local name, return whatever
# Tcl file{} returns. (To tell if a name is a network-style
# name, use the *protocol* option and test for null. The
# *pathtype* option is not used for this test in order that
# existing code that uses Tcl <code>file pathtype</code> is
# easy to update to use <code>::tycho::url</code>.)
# <li><code>::tycho::url protocol</code> _name_: Return the
# protocol component of the name. If it is a local name,
# return null.
# <li><code>::tycho::url rootname</code> _name_: Return all
# characters up to the last period in the last component of
# the name. (If the last component contains no period, return
# the whole thing.)
# <li><code>::tycho::url server</code> _name_: Return the
# server component of the name. If it is a local name,
# return null. Note also that network-style names may
# contain a null server component, as in
# <code>file:/users/ptolemy</code>.
# <li><code>::tycho::url split</code> _name_: Return the
# platform-independent path name from the platform-dependent name.
# If the name is a network name -- that is, starts with a protocol
# name, then the returned last contains the protocol name, server
# name, and individual components of the path; otherwise, it returns
# the same thing as Tcl <code>file split</code>.
# <li><code>::tycho::url tail</code> _name_: Return all
# characters after the last directory separator.
# </ul>
#
# <b>Implementation note</b>: In order to ensure that only
# registered protocols are recognized, we have to use the
# <code>::tycho::Registry</code> class to test if a string
# that looks like it starts with a protocol actually is.
#
ensemble ::tycho::url {
    # Convert a local name using url-style slashes into a platform
    # dependent name.
    option aslocal {name} {
	if { [regexp {^([a-z]+):} $name _ protocol] \
		&& [::tycho::registered protocol $protocol] } {
	    error "Not a local name"
	}
        # FIXME: Will this work reliably on the Mac???
	eval file join [file split $name]
    }
    # Return the name excluding the last component.
    option dirname {name} {
	if { [regexp {^([a-z]+):(//[^/]*)?(.*)$} $name _ protocol server path] \
		&& [::tycho::registered protocol $protocol] } {
	    # Network name
	    if { $path == "" } {
		# Empty path -- return name
		return $name
            } else {
		set pt [split $path /]
		set path [join [lreplace $pt end end] /]
		return $protocol:$server$path
	    }
	} else {
	    # Local name
	    file dirname $name
	}
    }
    # Expand the name into a normalized version.
    option expand {name} {
        if { [regexp {^([a-z]+):(//[^/]*)?(.*)$} $name _ protocol server path] \
		&& [::tycho::registered protocol $protocol] } {
	    # Network name
	    if { $path != "" } {
                set res {}
                foreach c [split $path /] {
                    if { $c == ".." && [llength $c] > 0 } {
                        set res [lreplace $res end end]
                    } else {
                        lappend res $c
                    }
                }
                set path [join $res /]
            }
            return $protocol:$server$path
	} else {
	    # Local name
	    ::tycho::expandPath $name
	}
    }
    # Return the name extension.
    option extension {name} {
	if { [regexp {^([a-z]+):(//[^/]*)?(.*)$} $name _ protocol server path] \
		&& [::tycho::registered protocol $protocol] } {
	    # Network name
	    if { $path == "" } {
		return ""
	    } else {
		set nm [lindex [split $path /] end]
		set nms [split $nm .]
		if { [llength $nms] > 1 } {
		    return .[lindex $nms end]
		} else {
		    return {}
		}
	    }
	} else {
	    # Local name
	    file extension $name
	}
    }
    # Return the platform-dependent path name from the arguments.
    option join {name args} {
	if { [regexp {^([a-z]+):(//[^/]*)?(.*)$} $name _ protocol server path] \
		&& [::tycho::registered protocol $protocol] } {
	    # Network name -- we need to figure out how much
            # if the name also contains server and path parts
            # Get the correct server name
            if { $server == "" && $args != "" } {
                set server [string trim [lindex $args 0] /]
                set args [lreplace $args 0 0]
            } else {
                set server [string trim $server /]
            }

            # Get a list representing the path (only)
            set plist [split [string trim $path /] /]

            # Process additional args to add to plist
            foreach a $args {
                if [regexp {^(/|~)} $a] {
                    # Absolute, so dump the path so far
                    set plist [list [string trim $a /]]
                } else {
                    # Relative, so append to path
                    lappend plist [string trim $a /]
                }
            }

            # Reconstruct the name and return it
            if { $server == "" } {
                return $protocol:/[join $plist /]
            } else {
                return $protocol://$server/[join $plist /]
            }
	} else {
	    # Local name
	    eval file join [list $name] $args
	}
    }
    # Return the path component of the name.
    option path {name} {
	if { [regexp {^([a-z]+):(//[^/]*)?(.*)$} $name _ protocol server path] \
		&& [::tycho::registered protocol $protocol] } {
	    # Network name
            if { $path == "" } {
                return "/"
            } else {
                return $path
            }
	} else {
	    # Local name
	    return $name
	}
    }
    # Return the "type" of a path
    option pathtype {name} {
	if { [regexp {^([a-z]+):} $name _ protocol] \
		&& [::tycho::registered protocol $protocol] } {
	    # Network name
            return "absolute"
        } elseif [regexp {^[a-z]:} $name] {
            # Windoze path
            return "absolute"
        } elseif {[regexp {^\$.*} $name] || [regexp {^\~.*} $name]} {
            # If it begins with $, then it's absolute. Currently ::file 
            # doesn't take care of this case (ie. [file pathtype \$PTOLEMY])
            return "absolute"
         } else {
	    # Local name
	    file pathtype $name
	}
    }
    #  Return the protocol component of the name.
    option protocol {name} {
	if { [regexp {^([a-z]+):} $name _ protocol] \
		&& [::tycho::registered protocol $protocol] } {
	    # Network name
	    return [string tolower $protocol]
	} else {
	    # Local name
	    return {}
	}
    }
    # Return the name up to the last period of the last component.
    option rootname {name} {
	if { [regexp {^([a-z]+):(//[^/]*)?(.*)$} $name _ protocol server path] \
		&& [::tycho::registered protocol $protocol] } {
	    # Network name
	    if { $path == "" } {
		return $protocol:$server
	    } else {
		set pt [split $path /]
		set path [join [lreplace $pt end end] /]
                set tt [split [lindex $pt end] .]
                if { [llength $tt] > 1 } {
                    set tt [join [lreplace \
                            [split [lindex $pt end] .] end end] .]
                }
                return $protocol:$server$path/$tt
	    }
	} else {
	    # Local name
	    file rootname $name
	}
    }
    # Return the server component of the name.
    option server {name} {
	if { [regexp {^([a-z]+):(//[^/]*)?} $name _ protocol server] \
		&& [::tycho::registered protocol $protocol] } {
	    # Network name
	    return [string trimleft $server /]
	} else {
	    # Local name
	    return {}
	}
    }
    # Split a platform-dependent local name or a network name into a list
    option split {name} {
	if { [regexp {^([a-z]+):(//([^/]*))?(.*)$} $name \
                    _ protocol _ server path] \
		&& [::tycho::registered protocol $protocol] } {
	    # Network name
	    if { $path == "" } {
		list $protocol: $server
	    } else {
		concat [list $protocol: $server] \
                        [lreplace [split $path /] 0 0]
	    }
	} else {
	    # Local name
	    file split $name
	}
    }
    # Return the last component of the name
    option tail {name} {
    set nn [string trimright $name /]
	if { [regexp {^([a-z]+):(//[^/]*)?(.*)$} $name _ protocol server path] \
		&& [::tycho::registered protocol $protocol] } {
            # Need the following to parse correctly: ftp://ptolemy/pub/dir/
            set path [string trimright $path /]
	    # Network name
	    lindex [split $path /] end
	} else {
	    # Local name
	    file tail $name
	}
    }
}

#####################################################################
#### urlPathSearch
# Given a filename and a list (note: not colon-separated,
# as this will interfere with url-style names) of url-style pathnames, 
# return the path of the first file found relative to the
# names in the list. Note that if the filename includes
# directories, it is expected to be a Unix-style name
# containing slashes -- this is because this function is
# typically used to look for files obtained from portable
# files. If <i>basepath</i> is supplied, then any entries in
# the search path that are relative will be made relative to
# this directory; otherwise they will be ignored.
#
proc ::tycho::urlPathSearch {name searchpath {basepath {}}} {
    set f [split $name /]
    if { $basepath != "" } {
        set b [::tycho::url split $basepath]
    }
    foreach p $searchpath {
        if { [::tycho::url pathtype $p] == "relative" } {
            if { $b != "" } {
                set p [eval ::tycho::url join [concat $b [file split $p]]]
            } else {
                continue
            }
        }
	set file [eval ::tycho::url join [concat [::tycho::url split $p] $f]]
	if [::tycho::resource exists $file] {
	    return $file
	}
    }
    return ""
}

