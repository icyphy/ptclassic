# Pathname utilities that don't use the window system
#
# @Author: Edward A. Lee
# @Contributors: Joel King, Farhana Sheikh, Christopher Hylands
#
# @Version: $Id$
#
# @Copyright (c) 1995-%Q% The Regents of the University of California.
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
    set path [string trim $path]
    if {[string first \$ $path] == 0} {
        global ::env
        set slash [string first / $path]
        if {$slash > 0} {
            set envvar [string range $path 1 [expr {$slash-1}]]
        } {
            set envvar [string range $path 1 end]
        }
        if [info exists env($envvar)] {
            set envval $env($envvar)
        } elseif [uplevel #0 info exists $envvar] {
            global $envvar
            set envval [set $envvar]
        } else {
            # No such variable.
            error "No such variable: $envvar"
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
            set path "$dir/$tail"
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
            set path [format "%s/%s" [pwd] [file tail $path]]
        }
        cd $savedir
    }
    return $path
}

# COMPATIBILITY procedure.  Use <code>::tycho::expandPath</code>.
proc ::ptkExpandEnvVar { path } {::tycho::expandPath $path}

##############################################################################
#### invoke
# Invoke an external program, passing it the specified arguments.
# The format of the command is exactly that of the Tcl "exec" command.
# The main purpose of this procedure is to generate an informative
# error message if the "exec" fails.
#
proc ::tycho::invoke { args } {
    # NOTE: Unix-ism.
    if [catch {eval exec $args} msg] {
        # The command failed.  It would be nice to check to see
        # whether the program exists, and generate a suitable message,
        # but it's not clear how to do this.
        error "The following command invocation failed:\n \
                $args\n \
                Is the program [lindex $args 0] installed and in your path?\n \
                $msg"
    }
}

##############################################################################
#### mkdir
# Create a directory
#
proc ::tycho::mkdir { args } {
    # Unix-ism
    eval exec mkdir $args
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
#### rm
# Remove a file
# FIXME: In tcl7.6, this should go away
#
proc ::tycho::rm { args } {
    global tcl_platform
    if { $tcl_platform(platform) != "windows" } { 
	# Unix-ism
	eval exec rm $args
    }
}

#####################################################################
#### tmpFileName
# Return a temporary filename that does not exist yet.
# The <CODE>TMPDIR</CODE> environment variable is as a base used if it is
# present, if <CODE>TMPDIR</CODE> is not present, then <CODE>/tmp</CODE>
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
    global env
    if [info exists env(TMPDIR)] {
	set tmpdir $env(TMPDIR)
    } else {
	set tmpdir /tmp
    }
    
    return [::tycho::uniqueFileName [ file join $tmpdir $stem] $extension]
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
