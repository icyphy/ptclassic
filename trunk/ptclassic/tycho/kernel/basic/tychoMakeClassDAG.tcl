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

#### tychoMkClassGraph
# Make a class graph of the type displayed by the Tycho EditDAG class.
# The first argument is the name of the graph.
# The second argument is the name of the graph file to create.
# The rest of the arguments are any number of file names
# from which the graph should be created.
# FIXME: This is a temporary hack.  tydoc should generate this info.
#
proc tychoMkClassGraph {name filename args} {
    set entries {}
    foreach file $args {
	set fd [open $file r]
	set contents [read $fd]
	close $fd
        set nm {}
	set classexp "\n\[ \t\]*class\[ \t\]+(\[^\{ \t\n\]*)"
	if {[regexp $classexp $contents matchvar nm] != 0} {
            set classfile($nm) $file
	}
	set inheritexp "\n\[ \t\]*inherit\[ \t\]+(\[^ \t\n\]*)"
        # Look for inheritance only if a class definition was found.
        if {$nm != {}} {
            if {[regexp $inheritexp $contents matchvar pnm] \
                    != 0} {
                set parent($nm) $pnm
            } {
                set parent($nm) {}
            }
        }
    }
        
    set fd [open $filename w]
    foreach nm [array names classfile] {
        set pnt $parent($nm)
        set rxp "\[ \t\]*class\[ \t\]+$nm"
        # Remove namespace information.
        # FIXME: This should be encoded to show up somehow.
        set sname $nm
        regexp "::(\[^: \t\]*)\$" $nm match sname
        puts $fd "\{add $nm \{label \{$sname\} link \{$classfile($nm) \{$rxp\}\}\} \{$pnt\}\}"
    }
    puts $fd "\{verifyAll 1\}"
    close $fd
}
