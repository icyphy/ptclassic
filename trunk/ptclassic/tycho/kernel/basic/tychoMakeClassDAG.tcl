# Procedure to make a graph describing a class hierarchy.
#
# @Authors: Edward A. Lee, Christopher Hylands
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

# Pick up ::tycho::expandPath
source [ file join $TYCHO kernel Path.tcl]

#### tychoMkClassGraph
# Make a class graph of the type displayed by the Tycho EditDAG class.
# The first argument is the name of the graph. The second argument is
# the name of the graph file to create. The rest of the arguments are
# any number of file names from which the graph should be created.
# If any of these files is not readable, it is ignored.
# These file names should be absolute (or relative to an environment
# variable like TYCHO) so that the resulting hyperlinks work from any
# directory. To create a graph for the entire Tycho tree, use the
# procedure <code>tychoStandardClasses</code>.
#
# We could have tydoc generate this information, but it would be much slower.
#
proc tychoMkClassGraph {name filename args} {
    set entries {}
    set retval {}
    set classexp "\n\[ \t\]*class\[ \t\]+(\[^\{ \t\n\]*)"
    set inheritexp "\n\[ \t\]*inherit\[ \t\]+(\[^\n\]*)"
    set procexp "\n\[ \t\]*proc\[ \t\]+"
    set nonClassFileList {}

    foreach file $args {
	if {$file == {} } {
	    continue
	}
	set ffn [::tycho::expandPath $file]
        if {![file readable $ffn] || [file isdirectory $ffn]} {
            continue
        }
	set fd [open $ffn r]
 	if [catch {set contents [read $fd]} errMsg] {
            error "Error while reading $file:\n$errMsg"
        }
	close $fd

        set nm {}
	set classEnd 0

	# Read through the file, looking for the start of classes.
	# Each time we find the start of a class, we save the location
	# and then the next time through, start the search after
	# the start of the last class so that we pick up the next class
	
	while {[regexp -indices $classexp \
		[string range $contents $classEnd end] \
		classStartIndices nm] != 0} {
	    # FIXME: it is just easier to get the name by running regexp
	    # again.
	    regexp $classexp [string range $contents \
		    $classEnd end] \
		    matchvar nm
	    set classEnd [expr {[lindex $classStartIndices 1] + \
		    $classEnd}]
            set classfile($nm) $file
	    set htmlfile($nm) [file join [file dirname $file] \
		    doc codeDoc [info namespace tail $nm].html]
	    #puts "$nm $classStartIndices $classEnd \
	    #    [string range $contents $classEnd [expr {$classEnd+40}]]"

	    # Look for inheritance only if a class definition was found.
	    if {$nm != {}} {
		if {[regexp -indices $inheritexp \
			[string range $contents $classEnd end] \
			matchvar inheritIndices] != 0} {
		    # Check for the case where we have multiple classes
		    # in a file, but the first class has no parents.
		    # If the indices of the inherits clause are greater
		    # than the indices of the next class clause, then
		    # the current class has no parents, as the inherit
		    # clause we found belongs to the next class clause.
		    
		    if {[regexp -indices $classexp \
			    [string range $contents $classEnd end] \
			    classStartIndices tnm] != 0 } {
			if {[lindex $classEnd 0] < \
				[lindex $inheritIndices 0]} {
			    #puts "continuing: $nm $classEnd"
			    set parent($nm) {}
			    continue
			}
		    }

		    # FIXME: it is just easier to get the name by running
		    # regexp again.
		    regexp $inheritexp \
			[string range $contents $classEnd end] \
			matchvar pnm
		    if {$nm == $pnm} {
			#puts "In $file, $nm cannot be its own parent"
			set parent($nm) {}

		    } else {
			#puts "nm=$nm, pnm=$pnm"
			set parent($nm) {}
			set parentList [split $pnm]
			foreach parentElement $parentList {
			    if {$parentElement != {}} {
				lappend parent($nm) $parentElement
			    }
			}
		    }

		} {
		    set parent($nm) {}
		}
	    }
        }
	if {$classEnd == 0 } {
	    # If we are here, then this file does not define a class,
	    # so we check to see if it defines any procs.  If it does
	    # we add it to the list of files that define procs but not classes.
	    
	    if {[regexp $procexp $contents] != 0} {
		lappend nonClassFileList $file
	    }
	}
    }
        
    set fd [open $filename w]

    # Put in titles and a reasonable default size.
    puts $fd "\{configure -canvasheight 600\} \{configure -canvaswidth 800\}"
    puts $fd "\{titleSet title \{Tycho Class Hierarchy\}\}"
    puts $fd "\{titleSet subtitle \{created:\
	    [clock format [clock seconds]]\}\}"
    foreach nm [array names classfile] {
        set pnt $parent($nm)
        set rxp "\[ \t\]*class\[ \t\]+$nm"
        # Remove namespace information.
        # FIXME: This should be encoded to show up somehow.
        set sname $nm
        regexp "::(\[^: \t\]*)\$" $nm match sname

        puts $fd "\{add $nm \{label \{$sname\} altlink \{$classfile($nm)\
		\{$rxp\}\} link \{$htmlfile($nm) \{\}\}\} \{$pnt\}\}"
    }

    # Dump the files that don't define classes, but do define procs

    # Build the parent node
    puts $fd "\{add nonClassFilesNode \{label \{Non-Class Files\} \} \{\}\}"
    set pnt "nonClassFilesNode"

    # Traverse the list, and determine what directories have non-class files
    # in them.  Then create child nodes with the directory names.

    set nonClassDirectory(dummy) dummy
    foreach nonClassFile [lsort $nonClassFileList] {
	set dirname [file dirname $nonClassFile]
	if ![info exists nonClassDirectory($dirname)] {
	    set label [::tycho::simplifyPath $dirname]
	    # If the label is more than 32 chars, only print out the last 32
	    if {[string length $label] > 32} {
		set label "...[string range $label \
			[expr {[string length $label]-32}] end]"
	    }
	    puts $fd "\{add \{$label\} \{label \{$label\} \} \{$pnt\}\}"
	    set nonClassDirectory($dirname) "$label"
	}
    }

    foreach nonClassFile [lsort $nonClassFileList] {
	set nm [file tail [file rootname $nonClassFile]]
	set sname $nm
	set basefile [file rootname [file tail $nonClassFile]]
	set dirname [file dirname $nonClassFile]
	set html [file join $dirname doc codeDoc $basefile.html]

	puts $fd "\{add $nm \{label \{$sname\} altlink \{$nonClassFile\
		\{\}\} link \{$html \{\}\}\}\
		\{$nonClassDirectory($dirname)\}\}"
	# Save the current node in the slot for the current directory.
	# This makes the class diagram much wider
	#set nonClassDirectory($dirname) $nm
    }
    close $fd
    return $retval
}

#### tychoStandardClasses
# Update the Tycho classes graph.
#
proc tychoStandardDAG {} {
    global TYCHO
    set olddir [pwd]
    cd $TYCHO

    # If you update this list, update
    # $TYCHO/editors/textedit/ItclClassList.itcl too.
    set dirs [list \
            kernel \
            [file join editors textedit] \
            [file join editors visedit] \
            [file join editors slate] \
            [file join typt editors ] \
            [file join typt kernel ] \
            [file join typt controls ] \
            [file join lib idx ] \
            [file join lib tydoc ]
    ]

    foreach dir $dirs {
	if [file isdirectory "$dir"] {
	    eval lappend files [glob \
		    [file join $dir {{*.itcl,*.tcl,*.tk,*.itk}}]]
	}
    }
    foreach file $files {
        lappend filesenv [file join "\$TYCHO" $file]
    }
    set retval [eval tychoMkClassGraph {{Tycho Class Hierarchy}} [file join doc tychoClasses.dag] $filesenv ]
    cd $olddir
    return $retval
}
