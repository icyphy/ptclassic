# Procedure to make an index to a set of HTML documents.
#
# @Author: Edward A. Lee
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

#### tychoCompareFirst
# Given two lists, return -1, 0, or 1 depending on whether the first
# element of this first list is alphabetically less than, equal, or greater
# than the first element of the second list.
#
proc tychoCompareFirst {first second} {
    set stfirst [string tolower [lindex $first 0]]
    set stsecond [string tolower [lindex $second 0]]
    string compare $stfirst $stsecond
}

#### tychoMkIndex
# Make an index file of the type read by the Tycho IndexBrowser class.
# The first argument is the name of the index.
# The second argument is the name of the index file to create.
# The rest of the arguments are any number of file names
# from which the index should be created.
#
proc tychoMkIndex {name filename args} {
    set entries {}
    foreach file $args {
	set fd [open $file r]
	set contents [read $fd]
	close $fd
	set nameexp "<\[ \t\]*a\[ \t\]+name\[ \t\]*=\[ \t\]*\"?(\[^>\"\]*)\"?>"
	while {[regexp -nocase -indices $nameexp $contents matchvar matchname] \
		!= 0} {
	    set nm [string range $contents [lindex $matchname 0] \
		    [lindex $matchname 1]]
	    set entry [list $nm $file $nm]
	    lappend entries $entry
	    set contents [string range $contents [lindex $matchname 1] end]
	}
    }
    set fd [open $filename w]
    puts $fd [list $name]
    puts $fd \{
    foreach entry [lsort -command tychoCompareFirst $entries] {
	puts $fd [list $entry]
    }
    puts $fd \}
    close $fd
}

#### tychoFindAllHTML
# Search the current directory and recursively all subdirectories
# for files with the extension .html, and return a list of all the file
# names relative to the current directory.
#
proc tychoFindAllHTML {} {
    set files [glob -nocomplain {*.html}]
    foreach name [exec ls] {
	if [file isdirectory $name] {
	    # Skip SCCS and RCS directories and anything called "junk"
	    if {$name != {SCCS} && $name != {RCS} && $name != {junk} } {
		cd $name
		set subfiles [tychoFindAllHTML]
		cd ..
		foreach file $subfiles {
		    lappend files "$name/$file"
		}
	    }
	}
    }
    return $files
}

#### tychoStandardIndex
# Update the Tycho master index.
# All files with the extension .html in the tree rooted at the environment
# variable TYCHO are included.
#
proc tychoStandardIndex {} {
    global TYCHO
    set olddir [pwd]
    cd $TYCHO

    set files [tychoFindAllHTML]
    # cd back in case we have followed links in tychoFindAllHTML
    cd $TYCHO
    eval tychoMkIndex {{Tycho index}} tycho.idx $files
    cd $olddir
}
