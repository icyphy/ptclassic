# Pathname utilities that don't use the window system
#
# @Author: Edward A. Lee
# @Contributors: Joel King, Farhana Sheikh, Christopher Hylands
#
# @Version: $Id$
#
# @Copyright (c) 1995-1997 The Regents of the University of California.
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
#### autoName 
# Return a name constructed by augmenting the provided
# stem with a number to guarantee that the name is unique. A global
# (within the tycho namespace) array autoNames is used to keep track of
# the numbers used for each stem. This procedure should be used for
# window classes instead of the #auto facility in itcl to assign to the
# class valid names for windows. Window names must begin with a period.
# By convention, for a class named "Class", we would use the stem
# ".class". Thus, the single argument should be ".class".
# Note that if you invoke this outside the namespace "tycho", you
# must call it "::tycho::autoName".
#
proc ::tycho::autoName {stem} {
    global ::autoNames
    if {[info exists autoNames] && [info exists autoNames($stem)]} {
        incr autoNames($stem)
    } else {
        set autoNames($stem) 0
    }
    return "$stem$autoNames($stem)"
}

########################################################################
#### evalIfNewer
# If targetFile does not exist, or if sourceFile has a modification time 
# later than the file targetFile, then evaluate the rest of the args.
# This is similar to what the make command does.
#
proc ::tycho::evalIfNewer {sourceFile targetFile args} {
    if {![file exists $targetFile] || \
	    [file mtime $sourceFile] >= [file mtime $targetFile]} {
	return [eval $args]
    }
}

########################################################################
#### expandPath
# Expand a filename, returning an absolute filename with the complete path.
# The argument might begin with an environment variable, a
# global Tcl variable, or a reference to a user's home directory. If
# the argument begins with a dollar sign ($), then everything up to
# the first slash is taken to be a variable name.  If an environment
# variable exists with that name, then the dollar sign and the variable
# name are replaced with the value of the environment variable. For
# example, if the value of of the environment variable TYCHO is
# /usr/tools/tycho, then
# <pre>
#     ::tycho::expandPath \$TYCHO/tmp
# </pre>
# returns /usr/tools/tycho/tmp. If there is no such environment
# variable, but a global Tcl variable with the given name exists, then
# the value of that variable is substituted. If no such variable is
# defined as well, then an error is reported. Similarly, "~user/foo"
# will be expanded (on Unix systems) to "/users/user/foo", assuming
# that "user" has his or her home directory at "/users/user". If there
# is no user named "user", or we are not on a Unix system, then an
# error is reported. In all cases, any extra spaces at the beginning or
# the end of the given path are removed. Moreover, filenames are
# normalized before being returned. Symbolic links are followed, as are
# fields in the path like "/../". Thus, any two references to the same
# path should return the same string.
#
# Formerly, this procedure was in File.itcl, but it was moved so that it
# could be used in scripts that use itclsh, which does not have windows.#
#
proc ::tycho::expandPath { path } {
    global tcl_platform
    set path [string trim $path]
    
    switch $tcl_platform(platform) {
	macintosh {
	    # On the mac, 'file join {$tycho:foo}' results in :$tycho:foo
	    # So we may need to pull the leading : off.
	    if [string match {:$*} $path] {
		set path [string rang $path 1 end]
	    }
	    set directorySeparator ":"
	}
	default {
	    set directorySeparator "/"
	}
    }
    if {[string first \$ $path] == 0} {
        global ::env


	set slash [string first $directorySeparator  $path]
	if {$slash > 0} {
	    set envvar [string range $path 1 [expr {$slash-1}]]
	} {
	    set envvar [string range $path 1 end]
	}		

	set envvar [string range [lindex [file split $path] 0] 1 end]

	if {$tcl_platform(platform) == "macintosh" } {
	    set envvar [string trimright $envvar :] 
	}

        if [info exists env($envvar)] {
            set envval $env($envvar)
        } elseif {"$envvar" != "" && [uplevel #0 info exists $envvar]} {
            global $envvar
            set envval [set $envvar]
        } else {
            # No such variable.
            error "No such variable: `$envvar' in the path `$path'."
        }


        if {$slash > 0} {
            set path [format "%s%s" $envval [string range $path $slash end]]
        } {
            set path $envval
        }
    } elseif {[string first ~ $path] == 0} {
        if [catch {set path [glob $path]}] {
            # glob failed. Possibly the file does not exist.
            set dir [file dirname $path]
            if [catch {set dir [glob $dir]}] {
                error "Directory does not exist: $dir"
            }
            set tail [file tail $path]
            set path [file join $dir $tail]
        }
    }
    # Get a consistent filename using "cd".  Note that this may not work
    # on non-Unix machines.  The catch is in case the directory does not
    # exist.
    catch {
        set savedir [pwd]
        # If the filename itself is a directory, normalize it
        if [file isdirectory $path] {
            cd $path
            set path [pwd]
        } {
            cd [file dirname $path]
	    set path [file join [pwd] [file tail $path]]
        }
        cd $savedir
    }
    return $path
}

# COMPATIBILITY procedure.  Use <code>::tycho::expandPath</code>.
proc ::ptkExpandEnvVar { path } {::tycho::expandPath $path}

########################################################################
#### expandPathSplit
# Given a colon separated list of pathnames, expand each path, 
# reassemble the colon separated list and return it.
#
proc ::tycho::expandPathSplit {inputPathList} {
    set outputList {}
    foreach inputPath [split $inputPathList :] {
	lappend outputList [::tycho::expandPath $inputPath]
    }
    return [join $outputList :]
}

##############################################################################
#### isRelative
# Return true if the the pathname is not an absolute pathname, and
# it does not start with tilde or a dollar sign
proc ::tycho::isRelative {pathname} {
    set char [string index $pathname 0]
    if { [file pathtype $pathname] != "absolute" && \
	    $char != "~" && $char != "\$"} {
	return 1
    }
    return 0
}


##############################################################################
#### mkdir
# Create a directory
#
proc ::tycho::mkdir { args } {
    global ::itcl::version tcl_platform
    if {[namespace ::itcl {set ::itcl::version} ] >= 2.2} {
	eval file mkdir $args
    } else {
	if { $tcl_platform(platform) == "unix" } { 
	    # Unix-ism
	    eval exec mkdir $args
	}
    }
}

##############################################################################
#### pathEnvSearch
# Search the user's PATH environment variable for filename
# 
proc ::tycho::pathEnvSearch {filename} {
    global env tcl_platform
    if ![info exists env(PATH)] {
	error "pathEnvSearch: \$PATH is not set!"
    }
    switch $tcl_platform(platform) {
        unix {
            return [::tycho::pathSearch $filename [split $env(PATH) :]]
        }
        windows {
            return [::tycho::pathSearch $filename [split $env(PATH) ";"]]
        }
        default { return {}}
    }
}

##############################################################################
#### pathSearch
# Search the path list for filename, if it is found then return the
# directory where it was found.  If it is not found, return the empty 
# string
#
proc ::tycho::pathSearch {filename {path {}}} {
    if {$path == {} } {
	global env tcl_platform
	switch $tcl_platform(platform) {
	    unix {set path [split $env(PATH) :]}
	    default {error "Don't know how to handle paths on \
		    $tcl_platform(platform) yet. env(PATH) = $env(PATH)"}
	}
    }  
    foreach dir $path {
	if [file executable [file join $dir $filename]] {
	    return $dir
	}
    }
    return {}
}

##############################################################################
#### relativePath 
# Given two pathnames srcFile and dstFile, return a relative pathname
# that goes from the srcFile to the dstFile.  This is useful for
# creating HTML links that are relative, thus avoiding the non-portable 
# use of $TYCHO.
#
proc ::tycho::relativePath {srcFile dstFile} {
    #puts "$srcFile $dstFile"
    if { [string index $dstFile 0] != "$" && \
	    [file pathtype $dstFile] != "absolute" } {
	return $dstFile
    }
    set srcList [file split [::tycho::expandPath $srcFile]]
    set dstList [file split [::tycho::expandPath $dstFile]]

    set equalCount 0
    foreach srcElement $srcList dstElement $dstList {
	if {$srcElement != $dstElement} {
	    break
	}
	incr equalCount
    }

    set results {}

    if {$equalCount < [llength $srcList]} {
	set srcLength [llength $srcList]
	set dstLength [llength $dstList]

	set upDirs [expr {[llength $srcList] - $equalCount}]
	#puts "$srcList $dstList $equalCount $srcLength $dstLength $upDirs"

	for {set i 0} {$i < $upDirs} {incr i} {
	    lappend results ..
	}
    }
    set results [concat $results [lrange $dstList $equalCount end]]
    if {"$results" == {}} {
	return [file tail $dstFile]
    }
    return [eval file join $results]
}

##############################################################################
#### rootDir
# Return the name of the root directory.  Under Unix this is "/".
# Under Mac the volume name is returned.  Under Windows 
#
proc ::tycho::rootDir {pathname} {
    global tcl_platform
    switch $tcl_platform(platform) {
	macintosh {
	    if { [file pathtype $pathname] == "absolute"} {
		return [lindex [file split $pathname] 0]
	    }
	    return [lindex [file split [pwd]] 0]
	}
	unix {
	    return "/"
	}
	windows {
	    switch [file pathtype $pathname] {
		absolute {
		    return [lindex [file split $pathname] 0]
		}
		volumerelative {
		    return [lindex [file split $pathname] 0]
		}
		relative {
		    return [lindex [file split [pwd]] 0]
		}
	    }
	}
    }

}

##############################################################################
#### rm
# Remove a file
# FIXME: In tcl7.6, this should go away
#
proc ::tycho::rm { args } {
    global itcl::version tcl_platform
    if {[namespace ::itcl {set ::itcl::version} ] >= 2.2} {
	if {[lindex $args 0] == "-f" || [lindex $args 0] == "-rf"} {
	    eval file delete -force -- $args
	} else {
	    eval file delete -- $args
	}
    } else {
	if { $tcl_platform(platform) == "unix" } { 
	    # Unix-ism
	    eval exec rm $args
	}
    }
}

##############################################################################
#### rmIfNotWritable
# Remove a file if we can't open it for writing.
# Lots of times a user will have symbolic links to a master tree
# that they don't have write permission to.  If one of these links is
# a derived file, such as a documentation file, then the user will
# not be able to update the documentation file if it is not writable
# by the user.  The solution is to break the link.
#
proc ::tycho::rmIfNotWritable { file } {
    if {[file exists $file] && ![file writable $file]} {
	file delete -force $file
    }

}
#####################################################################
#### simplifyPath
# Given a path and a list of environment variables, try to simplify
# the path by checking to see if we can use an environment variable instead
# of a long string.
#
# For example, if the TYCHO environment variable is set to
# <CODE>/users/ptolemy/tycho</CODE>, and this directory is actually
# automounted at <CODE>/export/watson/watson2/ptolemy/tycho</CODE>, and
# we pass simplifyPath the pathname
# <CODE>/export/watson/watson2/ptolemy/tycho/README</CODE>, then we
# should get back <CODE>$TYCHO/README</CODE>.
#
# If we cannot simplify the pathname, then we return the original pathname.
#
# If we specify a list of environment variables, then the first environment
# variable that matches a non-zero number of characters is the environment
# variable that is used
#
# If we don't specify a list of environment variables (the default), then
# we return the shortest string that results from subsituting environment
# variables.
#
# The following example expands and then simplifies $TYCHO/README.files:
# <tcl><pre>
# ::tycho::simplifyPath [::tycho::expandPath &#92
#    [file join &#92$TYCHO README.files]] &#92
#    [list TYCHO]]
# </pre></tcl>
# 
proc ::tycho::simplifyPath {pathName {envVarList {}}} {
    global env
    set returnOnFirstMatch 1
    set expandedPathName [::tycho::expandPath $pathName]
    if {$envVarList == {}} {
	# Return the shortest string, since we have no idea what order
	# we are getting the environment variables
	set returnOnFirstMatch 0
	set envVarList [array names env]
    }
    set lastMatch $expandedPathName
    foreach envVar $envVarList {
	# Does the variable exist, is it non-empty, is it anything other
        # than PWD and does it specify a non-relative pathname?
	if {[info exists env($envVar)] == 1 && \
		"$env($envVar)" != "" &&  \
                "$envVar" != "PWD" && \
		[file pathtype $env($envVar)] != "relative" } {
	    set expandedEnvVar [::tycho::expandPath $env($envVar)]
	    if { [string first $expandedEnvVar $expandedPathName] != -1} {
		set goodMatch [file join \$$envVar \
			[string range $expandedPathName \
			[expr {[string length $expandedEnvVar] +1 }] \
			end]]
		#puts "$expandedPathName $goodMatch $envVar \
		#	$env($envVar) $expandedEnvVar"
		if {$returnOnFirstMatch == 1} {
		    return $goodMatch
		}
		if {[string length $goodMatch] < [string length $lastMatch]} {
		    set lastMatch $goodMatch
		}
	    }
	}
    }
    return $lastMatch
}

#####################################################################
#### tmpFileName
# Return a temporary filename that does not exist yet.
# The <CODE>TMPDIR</CODE> environment variable is used as a base if it is
# present, if <CODE>TMPDIR</CODE> is not present, then
# <CODE>env(TEMP)</CODE> is used. If neither are present<CODE>/tmp</CODE>
# is used.
# If the optional arg `stem' is present, then a unique string is appended
# on to it.  The stem arg defaults to tytmp.
# If the optional arg `extension' is present, then the extension is appended
# after the unique string is appended.  If extension is the empty string
# then nothing is appended.  The default is the empty string.
#
# For example, the following command under Unix:
# <tcl><pre>
# ::tycho::tmpFileName myfile .itcl
# </pre></tcl>
# might return something like <CODE>/tmp/myfile1.itcl</CODE>
#
proc ::tycho::tmpFileName { {stem {tytmp}} {extension {}}} {
    # Unix-isms here
    # DOS-ism added ;) Mac-isms, where are you?
    global env
    if [info exists env(TMPDIR)] {
	set tmpdir $env(TMPDIR)
    } elseif [info exists env(TEMP)] {
	set tmpdir $env(TEMP)
    } else {
	set tmpdir /tmp
    }
    
    return [::tycho::uniqueFileName [ file join $tmpdir $stem] $extension]
}

#####################################################################
#### tychoDir
# Return the (abbreviated) path to the Tycho directory. In Unix,
# this is ~/.Tycho. If the directory does not exist, make it.
# Note that the directory name will need to be passed to
# <code>::tycho::expandPath</code> before being given to the
# file system.
#
proc ::tycho::tychoDir {} {
    global tcl_platform
    set dotTycho [file join [glob ~] .Tycho]

    if {![file exists $dotTycho]} {
	::tycho::mkdir $dotTycho
    } {
        if {![file isdirectory $dotTycho]} {
            error "Cannot create $dotTycho directory because a file \
                    exists by that name."
        }
    }
    return [file join ~ .Tycho]
}

#####################################################################
#### uniqueFileName
# Return a filename that is unique, and does not yet exist
# If the optional arg `stem' is present, then a unique string is appended
# on to it.  The stem arg defaults to tyuniq.
# If the optional arg `extension' is present, then the extension is appended
# after the unique string is appended.
#
# For example
# <tcl><pre>
# ::tycho::uniqueFileName myfile .itcl
# </pre></tcl>
# might return something like <CODE>myfile1.itcl</CODE>
#
proc ::tycho::uniqueFileName { {stem {tyuniq}} {extension {}}} {
    while {1} {
        if {$extension == {} } {
            set tmpname "[::tycho::autoName $stem]"
        } else {
            set tmpname "[::tycho::autoName $stem]$extension"
        }
	if {![file exists $tmpname]} {
	    break
	}
    }
    return $tmpname
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
# "type" of a path -- aboslute, relative, or volumerelative.
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
ensemble ::tycho::url {
    # Convert a local name using url-style slashes into a platform
    # dependent name.
    option aslocal {name} {
	if [regexp {^[a-z]+:/+} $name] {
	    error "Not a local name"
	}
        # FIXME: Will this work reliably on the Mac???
	eval file join [file split $name]
    }
    # Return the name excluding the last component.
    option dirname {name} {
	if [regexp {^[a-z]+:/+} $name] {
	    # Network name
	    regexp {^([a-z]+:)(//[^/]*)?(.*)$} $name _ protocol server path
	    if { $path == "" } {
		# Empty path -- return name
		return $name
            } else {
		set pt [split $path /]
		set path [join [lreplace $pt end end] /]
		return $protocol$server$path
	    }
	} else {
	    # Local name
	    file dirname $name
	}
    }
    # Expand the name into a normalized version.
    option expand {name} {
        if [regexp {^[a-z]+:/} $name] {
	    # Network name
	    regexp {^([a-z]+:)(//[^/]*)?(.*)$} $name _ protocol server path
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
            return $protocol$server$path
	} else {
	    # Local name
	    ::tycho::expandPath $name
	}
    }
    # Return the name extension.
    option extension {name} {
	if [regexp {^[a-z]+:/} $name] {
	    # Network name
	    regexp {^([a-z]+:)(//[^/]*)?(.*)$} $name _ protocol server path
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
	if [regexp {^[a-z]+:} $name] {
	    # Network name -- we need to figure out how much
            # if the name also contains server and path parts
	    regexp {^([a-z]+:)(//[^/]*)?(.*)$} $name _ protocol server path

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
                return $protocol/[join $plist /]
            } else {
                return $protocol//$server/[join $plist /]
            }
	} else {
	    # Local name
	    eval file join [list $name] $args
	}
    }
    # Return the path component of the name.
    option path {name} {
	if [regexp {^[a-z]+:/+} $name] {
	    # Network name
	    regexp {^([a-z]+:)(//[^/]*)?(.*)$} $name _ protocol server path
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
	if [regexp {^[a-z]+:/+} $name] {
	    # Network name
	    return "absolute"
	} else {
	    # Local name
	    file pathtype $name
	}
    }
    #  Return the protocol component of the name.
    option protocol {name} {
	if [regexp {^([a-z]+):+} $name _ protocol] {
	    # Network name
	    return [string tolower $protocol]
	} else {
	    # Local name
	    return {}
	}
    }
    # Return the name up to the last period of the last component.
    option rootname {name} {
	if [regexp {^[a-z]+:/} $name] {
	    # Network name
	    regexp {^([a-z]+:)(//[^/]*)?(.*)$} $name _ protocol server path
	    if { $path == "" } {
		return $protocol$server
	    } else {
		set pt [split $path /]
		set path [join [lreplace $pt end end] /]
                set tt [split [lindex $pt end] .]
                if { [llength $tt] > 1 } {
                    set tt [join [lreplace \
                            [split [lindex $pt end] .] end end] .]
                }
                return $protocol$server$path/$tt
	    }
	} else {
	    # Local name
	    file rootname $name
	}
    }
    # Return the server component of the name.
    option server {name} {
	if [regexp {^[a-z]+:/+} $name] {
	    # Network name
	    regexp {^([a-z]+:)(//[^/]*)?} $name _ protocol server
	    return [string trimleft $server /]
	} else {
	    # Local name
	    return {}
	}
    }
    # Split a platform-dependent local name or a network name into a list
    option split {name} {
	if [regexp {^[a-z]+:/+} $name] {
	    # Network name
	    regexp {^([a-z]+:)(//([^/]*))?(.*)$} $name \
                    _ protocol _ server path
	    if { $path == "" } {
		list $protocol $server
	    } else {
		concat [list $protocol $server] \
                        [lreplace [split $path /] 0 0]
	    }
	} else {
	    # Local name
	    file split $name
	}
    }
    # Return the last component of the name
    option tail {name} {
	if [regexp {^[a-z]+:/+} $name] {
	    # Network name
	    regexp {^([a-z]+:)(//[^/]*)?(.*)$} $name _ protocol server path
	    lindex [split $path /] end
	} else {
	    # Local name
	    file tail $name
	}
    }
}


