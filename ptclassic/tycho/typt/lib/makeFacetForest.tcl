# Procedure to make a graph describing a Octtools facet hierarchy.
#
# @Authors: Edward A. Lee, Christopher Hylands
#
# @Version: @(#)tychoMakeClassDAG.tcl	1.2 9/2/96
#
# @Copyright (c) 1995-1996 The Regents of the University of California.
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

# FIXME: octls only works on facets that it can write

#### tychoMkFacetGraph
# Make a class graph of the type displayed by the Tycho EditDAG class.
# The first argument is the name of the graph. The second argument is
# the name of the graph file to create. The rest of the arguments are
# any number of file names from which the graph should be created.
# These file names should be absolute (or relative to an environment
# variable like TYCHO) that the resulting hyperlinks work from any
# directory. To create a graph for the entire Tycho tree, use the
# procedure <code>tychoStandardClasses</code>.
#
proc tychoMkFacetGraph {name filename args} {
    set entries {}
    set outfd [open $filename w]
    # Put in titles and a reasonable default size.
    puts $outfd "\{configure -canvasheight 600\} \{configure -canvaswidth 800\}"
    puts $outfd "\{centeredText \{$name\} title \{\} black \{\{helvetica 24 bold i\} \{times 24 bold i\}\}\}"
    # NOTE: Unix-only implementation:
    puts $outfd "\{centeredText \{created: [exec date]\} subtitle title firebrick \{\{helvetica 16 bold i\} \{times 16 bold i\}\}\}"

    puts $outfd {{add src/domains/cgc/demo/init.pal {}}}
    tychoMkFacet [lindex $args 0] $outfd $args
    close $outfd
}

set autoName 0
set paletteList {}
proc tychoMkFacet { nm outfd parentPalette} {
    global autoName paletteList
    set infd [open "|octls $parentPalette" r]
    while {[gets $infd line ] >= 0} {
	regexp {([^ ]+) ([^ ]+)} $line dummy1 dummy2 palette 
	
	# FIXME: do all palettes have a .pal suffix?
	# Do all dogs go to heaven?
	if [regexp {.pal} $palette ] {
            # Check the list of facets we have already visited.
            # If the facet is not in the list, then visit it.
            # FIXME: if we were not generating a DAG, we would not have to
            # do this.
            if {[lsearch $paletteList $palette] == -1} {
                puts $outfd "\{add  \{$palette\} \{label \{$palette\} link \{$palette \{\} \}\} \{$parentPalette\}\}"	    
                puts $palette
                flush stdout
                lappend paletteList $palette
                tychoMkFacet $palette $outfd $palette
            }
	} {
            # A facet, not a palette containing facets
            puts $outfd "\{add  \{$palette[incr autoName]\} \{label \{$palette\} link \{$palette \{\} \}\} \{$parentPalette\}\}"	    
        }
    }
    close $infd
}

#### tychoStandardClasses
# Update the Tycho classes graph.
#
proc tychoStandardDAG {} {
    global PTOLEMY
    set olddir [pwd]

    eval tychoMkFacetGraph {{Ptdesign Class Hierarchy}} \
	    /tmp/PtdesignFacets.dag /users/ptdesign/init.pal
}
tychoStandardDAG
