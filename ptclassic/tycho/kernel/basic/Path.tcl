# Pathname utilities that don't use the window system or handle
# url-style names (see URL.tcl for that).
#
# @Author: Edward A. Lee
# @Contributors: Joel King, Farhana Sheikh, Christopher Hylands
#
# @Version: @(#)Path.tcl	1.75 08/05/98
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

##############################################################################
#### egrep
# Simple implementation of the Unix egrep command.
#
# If a grep binary is in the user's path, then use it
#
# The code below returns all the lines that have a # as the first character
# in two files.
# <tcl><pre>
# ::tycho::egrep {^#} &#92
#    [file join $TYCHO makefile] &#92
#    [file join $TYCHO kernel makefile]
# </pre></tcl>
#
# egrep can also take an optional <code>-i</code> or <code>-nocase</code>
# argument.  egrep will expand any glob style pathnames:
# <tcl><pre>
# ::tycho::egrep -i author *
# </pre></tcl>
#
proc ::tycho::egrep {regexp args} {
    set retval {}
    set regexparg {}
    set greparg {}
    switch -- $regexp {
        -i {set regexparg -nocase
        set greparg -i
        set regexp [lindex $args 0]
        set filelist [lrange $args 1 end]
    }
        -nocase {set regexparg -nocase
        set greparg -i
        set regexp [lindex $args 0]
        set filelist [lrange $args 1 end]
    }
        default {set filelist $args}
    }

    if {[ ::tycho::pathEnvSearch egrep] != {}} {
        # We have an egrep binary, use it.
        set files [eval glob -nocomplain $filelist]
        if { $files == ""} {
            error "::tycho::egrep called with empty file arg.\
                    args were `$regexp' `$files'"
        }
	if [catch {set retval [split \
		[eval exec egrep -n $greparg \"$regexp\" $files] "\n"] ;#"} \
		err] {
	    global errorCode
	    # If errorCode is 1, then no values were found.
	    # See the Unix egrep man page for details about egrep return vals. 
	    if {[lindex $errorCode 2] != 1} {
		error "::tycho::egrep: exec of egrep caused an error:\
			$errorCode\n$err"
	    }
	}
        return $retval
    } else {

    # Only show the first 100 matches
    set count 1
    while {[llength $filelist] > 0 && [incr count] < 100} {
        set file [lindex $filelist 0]
        set filelist [lreplace $filelist 0 0]
        if [file isdirectory $file] {
            continue
        }
        if {![file exists $file]} {
            # If we don't find the file, try expanding the argument
            set globlist [glob -nocomplain $file]
            if {![file exists [lindex $globlist 0]]} {
                lappend retval "$file does not exist"
                continue
            }
            set tfilelist $globlist
            if {[llength [lrange $filelist 1 end]] > 0} {
                lappend tfilelist [lrange $filelist 1 end]
            }
            set filelist $tfilelist
            set file [lindex $globlist 0]
        }
        if [catch {set fd [open $file "r"]} errMsg] {
            lappend retval $errMsg
            continue
        }
        set linecount 0
        while {[gets $fd linein] >= 0 } {
            incr linecount
            if {$regexparg == {} } {
                if [regexp $regexp $linein] {
                    lappend retval "$file: $linecount: $linein"
                }
            } else {
                # Handle the -i or -nocase
                if [regexp $regexparg $regexp $linein] {
                    lappend retval "$file: $linecount: $linein"
                }

            }
        }
        close $fd
    }
    if { $count > 99 } {
        lappend retval "*** egrep terminated, more than $count files matched"
    }
    return $retval
  }
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
# path should return the same string. If _path_ is a null string,
# return a null string.
#
# Formerly, this procedure was in File.itcl, but it was moved so that it
# could be used in scripts that use itclsh, which does not have windows.#
#
proc ::tycho::expandPath { path } {
    global tcl_platform
    set path [string trim $path]
    if { $path == "" } {
        return ""
    }
    
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
        set splat [file split $path]
        # Test for non-existent user
        set user [lindex $splat 0]
        set relpath [lrange $splat 1 end]
	set path [eval file join [concat [file split [glob $user]] $relpath]]
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
# Given a system-standard (usually colon) separated list of pathnames, 
# expand each path, reassemble the (usually colon) separated list and 
# return it.
#
# <b>Note</b>: Should procedure does not work with url-style
# names. Perhaps it should be modified to do so?
#
proc ::tycho::expandPathSplit {inputPathList} {
    set outputList {}
    set separator [::tycho::pathSeparator]
    foreach inputPath [split $inputPathList $separator] {
	lappend outputList [::tycho::expandPath $inputPath]
    }
    return [join $outputList $separator]
}

########################################################################
#### directoryParent
# Return the directory parent string for the current system (i.e. Unix = ../)
#
proc ::tycho::directoryParent {} {
    global env tcl_platform
    
    switch $tcl_platform(platform) {
        unix {
            return {../}
        }
        windows {
            #This works better than "..\" in Tcl
            return {../}
        } 
        macintosh {
            return {..:}
        }
        default {error "Don't know the directory separator on \
                $tcl_platform(platform) yet."}
    }
}    

########################################################################
#### directorySeparator
# Return the directory separator for the current system (i.e. Unix = /)
#
# NOTE: This returns / for Windows because Tcl supports the forward slash
# under Windows better than the Windows standard backslash.
#
proc ::tycho::directorySeparator {} {
    global env tcl_platform
    
    switch $tcl_platform(platform) {
        unix {
            return {/}
        }
        windows {
            #This works better than "\" in Tcl
            return {/}
        } 
        macintosh {
            return {:}
        }
        default {error "Don't know the directory separator on \
                $tcl_platform(platform) yet."}
    }
}

##############################################################################
#### isRelative
# Return true if the the pathname is not an absolute pathname, and
# it does not start with tilde or a dollar sign
proc ::tycho::isRelative {pathname} {
    set char [string index $pathname 0]
    if { [file pathtype $pathname] != "absolute" && \
            [file pathtype $pathname] != "volumerelative" && \
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
    if {[namespaceEval ::itcl {set ::itcl::version} ] >= 2.2} {
	eval file mkdir $args
    } else {
	if { $tcl_platform(platform) == "unix" } { 
	    # Unix-ism
	    eval exec mkdir $args
	}
    }
}

##############################################################################
#### noCase
# Turn a filename into a case insensitive format suitable for passing to glob.
# Example:
# <tcl><pre>
# ::tycho::noCase temp/file.txt
# </pre></tcl>
# 
proc ::tycho::noCase {filename} {
    global tcl_platform

    set depth 0
    set retval {}
    set length [string length $filename]
    for {set i 0} {$i < $length} {incr i} {
        set char [string index $filename $i]
        if {$char == "\["} {
            incr depth
            append retval $char
        } elseif {$char == "\]"} {
            incr depth -1
            append retval $char
        } elseif {($char == ":") && ($tcl_platform(platform) == "windows")} {
            # fix for Windoze drive letters (glob {[Cc]:} doesn't work)
            set retval [string range $filename 0 $i] 
        } elseif {$depth == 0} {
            # if it's a letter, replace with [lower UPPER]
            if [regexp {^[a-z]$} $char] {
                append retval "\[$char[string toupper $char]\]"
            } elseif [regexp {^[A-Z]$} $char] {
                append retval "\[$char[string tolower $char]\]"
            } else {
                append retval $char
            }
        } else {
            append retval $char
        }   
    }
    return $retval
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
    return [::tycho::pathSearch $filename \
            [split $env(PATH) [::tycho::pathSeparator]]]

}

##############################################################################
#### pathSearch
# Search the path list for filename, if it is found then return the
# directory where it was found.  If it is not found, return the empty 
# string
#
proc ::tycho::pathSearch {filename {path {}}} {
    global env tcl_platform

    if {$path == {} } {
        if ![info exists env(PATH)] {
            error "pathSearch: \$PATH is not set!"
        }
        set path [split $env(PATH) [::tycho::pathSeparator]]
    }  
    foreach dir $path {
	if [file executable [file join $dir $filename]] {
	    return $dir
	}
    }
    # If we are under windows, and the filename does not end
    # with .exe, then search for $filename.exe
    if { $tcl_platform(platform) == "windows" && \
            [regexp {\.exe} $filename] == 0 } {
        foreach dir $path {
            if [file executable [file join $dir $filename.exe]] {
                return $dir
            }
	}
    }

    return {}
}

##############################################################################
#### pathSeparator
# Return the path separator for the current system. FIXME: Macintosh is ?
#
proc ::tycho::pathSeparator {} {
    global env tcl_platform
    
    switch $tcl_platform(platform) {
        unix {
            return {:}
        }
        windows {
            return {;}
        }
        default {error "Don't know the path separator on \
                $tcl_platform(platform) yet."}
    }
}

##########################################################################
#### parseHeaderString
#
# Parse the given header string and set the elements of
# the passed array, with each index being the name of a headed
# variable. Returns 1 if successful, or zero if the header was
# malformed. The header format is the same as the emacs mode string,
# either being:
# <pre>
#   -*- _mode_ -*-
# </pre>
# or
# <pre>
#   -*- mode: _mode_ { ; _name_: _value_ } -*-
# </pre>
#
# Because Tycho uses a number of variables in the header, we allow
# it be multi-line, with each line being of the second form above.
#
proc ::tycho::parseHeaderString {string var} {
    upvar $var v
    set result 0

    # Remove delimiters and leading and trailing space
    regsub -all -- {-\*-} $string {} string
    set string [string trim $string " \t;"]

    # Try for just the mode
    if [regexp {^[^:;]+$} $string mode] {
        set v(mode) $mode
        return 1
    }

    # Parse name-value in each line
    foreach line [split $string "\n"] {
        set line [string trim $line " \t;"]
        while { [regexp "^(\[^: \t\]+)\[ \t\]*:\[ \t\]*(\[^;\]+)(.*)\$" \
                $line _ name value line] } {
            set v($name) [string trim $value " \t"]
            set line [string trimleft $line " \t;"]
            set result 1
        }
    }
    return $result
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
	    [file pathtype $dstFile] != "absolute" && \
        [file pathtype $dstFile] != "volumerelative" } {
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
    global tcl_platform
    if { $::itcl::version >= 2.2} {
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
    # If a file is a link to a nonexistant file, then the link will
    # exist, but not be readable.
    if {[file exists $file] && \
	    ![file writable $file] || ![file readable $file]} {
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
# <CODE>/export/watson/watson2/ptolemy/tycho/README.txt</CODE>, then we
# should get back <CODE>$TYCHO/README.txt</CODE>.
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
# The following example expands and then simplifies $TYCHO/README.txt:
# <tcl><pre>
# ::tycho::simplifyPath [::tycho::expandPath &#92
#    [file join &#92$TYCHO README.txt]] &#92
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
# The <CODE>TMPDIR</CODE> environment variable is used as a base if it is,
# if <CODE>TMPDIR</CODE> is not present, then
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
