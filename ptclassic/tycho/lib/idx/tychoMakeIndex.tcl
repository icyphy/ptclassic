# Procedure to make an index to a set of HTML documents.
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

## BOGOSITY alert! This file now generates TIM format files,
# so we have to load a bunch of needed files:
namespace ::tycho
source ../../kernel/Object.itcl
source ../../kernel/Interchange.itcl
source ../../kernel/Model.itcl
source ../../kernel/HyperlinkIndex.itcl
source ../../kernel/Path.tcl

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

proc tychoCompareNocase {first second} {
    set stfirst [string tolower $first]
    set stsecond [string tolower $second]
    string compare $stfirst $stsecond
}

#### tychoMkIndex
# Make an index file of the type read by the Tycho IndexBrowser class.
# The first argument is the name of the index.
# The second argument is the name of the index file to create.
# The fourth argument generates a one-level hierarchy if true.
# The rest of the arguments are any number of file names
# from which the index should be created.  If any of these files
# is missing or is not readable, then that file is ignored.
#
proc tychoMkIndex {name filename prependTYCHO nested args } {
    #puts "name=$name, filename=$filename files=$args"
    foreach file $args {        
        if ![file readable $file] {
            continue
        }
	set fd [open $file r]
	set contents [read $fd]
	close $fd
	set nameexp "<\[ \t\]*a\[ \t\]+name\[ \t\]*=\[ \t\]*\"?(\[^>\"\]*)\"?>"
	while {[regexp -nocase -indices $nameexp $contents matchvar matchname] \
		!= 0} {
	    set nm [string range $contents [lindex $matchname 0] \
		    [lindex $matchname 1]]
	    set ix [lindex $nm 0]
	    if { $prependTYCHO != 0} {
		lappend entries($ix) [list $nm [file join \$TYCHO $file] $nm]
	    } else {
		lappend entries($ix) [list $nm $file $nm]
	    }
	    set contents [string range $contents [lindex $matchname 1] end]
	}
    }
    set fd [open "$filename" w]
    # Put a comment line that gives reasonable output in netscape.
    puts $fd "# <h1>This is a Tycho index file. \
            Use tycho to view it.</h1> <nothtml"
    puts $fd [list $name]
    puts $fd \{
    if { $nested } {
	foreach entry [lsort -command tychoCompareFirst [array names entries]] {
	    puts $fd \{
	    puts $fd "$entry"
	    puts $fd \{
	    puts $fd [list "$name: $entry"]
	    puts $fd \{
	    foreach item $entries($entry) {
		puts $fd [list $item]
	    }
	    puts $fd \}\}\}
	}
    } else {
	foreach entry [lsort -command tychoCompareFirst [array names entries]] {
	    foreach item $entries($entry) {
		puts $fd [list $item]
	    }
	}
    }

    puts $fd \}
    puts $fd "# >"
    close $fd
}

#### tychoMkTIMIndex
# Make an index file in TIM format. This can also be read by the
# Tycho IndexBrowser class.
# The first argument is the name of the index.
# The second argument is the name of the index file to create.
# The third argument causes "$TYCHO/" to be prepended to the file name.
# The fourth argument generates a one-level hierarchy if true.
# The rest of the arguments are any number of file names
# from which the index should be created.  If any of these files
# is missing or is not readable, then that file is ignored.
#
proc tychoMkTIMIndex {name filename prependTYCHO nested args } {
    set index [::tycho::HyperlinkIndex [::tycho::autoName index] \
	    -datafile $filename \
	    -statefile [file rootname $filename].snapshot]
    $index modelconfigure -title $name
    $index modelconfigure -directory [file dirname $filename]

    foreach file $args {
	if $prependTYCHO {
	    $index parseFile [file join \$TYCHO $file]
	} else {
	    $index parseFile $file
	}
    }
    # Sort the index, case-insensitive
    $index sort -command tychoCompareNocase

    # Make it nested if asked to
    if { $nested } {
        $index nest
    }

    # Write to its files
    $index save

    delete object $index
}

#### tychoFindAllHTML
# Search the current directory and recursively all subdirectories
# for files with the extension .html, and return a list of all the file
# names relative to the current directory.  Note that we skip
# html files in the codeDoc directory.
#
# Note that we don't cd down into directories and then cd back up
# because if we do, and the directory is a link, we end up somewhere
# else.
#
proc tychoFindAllHTML { {dirname .} {depth 0}} {
    global env
    if [info exists env(PTARCH)] {
	set objdir "obj.$env(PTARCH)"
    } else {
	set objdir "obj"
    }
    cd $dirname
    set files [glob -nocomplain {*.htm*}]
    foreach name [exec ls] {
	if [file isdirectory $name] {
	    # Skip SCCS, RCS, adm, test directories and anything called "junk"
	    # Skip pn/lang
	    # The list of directories to avoid should be the same as
	    # the list in tychoFindCodeDocHTML, except here we 
	    # skip codeDoc too.
	    if {$name != {SCCS} && \
		    $name != {RCS} && \
		    $name != {adm} && \
		    $name != {codeDoc} && \
		    $name != {collections} && \
		    $name != {devel} && \
		    $name != {itclhtml} && \
		    $name != {javahtml} && \
		    $name != {junk} && \
		    $name != {lang} && \
		    $name != {src} && \
		    $name != {test} && \
		    $name != $objdir } {
		set subfiles [tychoFindAllHTML [file join $dirname $name] \
			[expr $depth + 1]]
		cd $dirname
		foreach file $subfiles {
		    lappend files [file join $name $file]
		}
	    }
	}
    }
    return $files
}

#### tychoFindCodeDocHTML
# Search the current directory and recursively all subdirectories
# for files with the extension .html, and return a list of all the file
# names relative to the current directory.  We only return html files in
# the codeDoc directory.
#
# Note that we don't cd down into directories and then cd back up
# because if we do, and the directory is a link, we end up somewhere
# else.
#
proc tychoFindCodeDocHTML { {dirname .} {depth 0}} {
    global env
    if [info exists env(PTARCH)] {
	set objdir "obj.$env(PTARCH)"
    } else {
	set objdir "obj"
    }
    cd $dirname
    set files [glob -nocomplain [file join codeDoc *.html]]
    foreach name [exec ls] {
	if [file isdirectory $name] {
	    # Skip SCCS, RCS, adm, test directories and anything called "junk"
	    # Skip pn/lang
	    # The list of directories to avoid should be the same as
	    # the list in tychoFindAllHTML, except here we don't
	    # skip codeDoc.
	    if {$name != {SCCS} && \
		    $name != {RCS} && \
		    $name != {adm} && \
		    $name != {collections} && \
		    $name != {devel} && \
		    $name != {itclhtml} && \
		    $name != {java} && \
		    $name != {javahtml} && \
		    $name != {junk} && \
		    $name != {lang} && \
		    $name != {test} && \
		    $name != {src} && \
		    $name != {tyjava} && \
		    $name != $objdir } {
		set subfiles [tychoFindCodeDocHTML [file join $dirname $name] \
			[expr $depth + 1]]
		cd $dirname
		foreach file $subfiles {
		    lappend files [file join $name $file]
		}
	    }
	}
    }
    return $files
}

#### ptolemyFindStarHTML
# Search the current directory and recursively all subdirectories
# for files with the extension .html, and return a list of all the file
# names relative to the current directory.  We only return html files in
# the codeDoc directory.
#
# Note that we don't cd down into directories and then cd back up
# because if we do, and the directory is a link, we end up somewhere
# else.
#
#proc ptolemyFindStarHTML { {dirname .} {depth 0}} {
#    cd $dirname
#    set files [glob -nocomplain [file join codeDoc *.html]]
#    foreach name [exec ls] {
#	if [file isdirectory $name] {
#	    # Skip SCCS, RCS, adm, test directories and anything called "junk"
#	    if {$name != {SCCS} && \
#		    $name != {RCS} && \
#		    $name != {adm} && \
#		    $name != {test} && \
#		    $name != {junk} } {
#		set subfiles [ptolemyFindStarHTML [file join $dirname $name] \
#			[expr $depth + 1]]
#		cd $dirname
#		foreach file $subfiles {
#		    lappend files [file join $name $file]
#		}
#	    }
#	}
#    }
#    return $files
#}

#### tychoStandardIndex
# Update the Tycho master index.
# All files with the extension .html in the tree rooted at the environment
# variable TYCHO are included.
#
proc tychoStandardIndex {} {
    global TYCHO
    set olddir [pwd]
    cd $TYCHO

    set files [tychoFindAllHTML $TYCHO]
    # cd back in case we have followed links in tychoFindAllHTML
    cd $TYCHO
    # The standard index is not nested
    eval tychoMkTIMIndex {{Tycho index}} \
    	    [list [file join $TYCHO lib idx tycho.idx]] 1 0 $files 
    cd $olddir
}

#### tychoCodeDocIndex
# Update the Tycho code index.
# All files in the codeDoc directories with the extension .html in the
# tree rooted at the environment.
# variable TYCHO are included.
#
proc tychoCodeDocIndex {} {
    global TYCHO
    set olddir [pwd]
    cd $TYCHO

    set files [tychoFindCodeDocHTML $TYCHO]
    # cd back in case we have followed links in tychoFindAllHTML
    cd $TYCHO
    # The code index is nested
    eval tychoMkTIMIndex {{Tycho Itcl Code Index}} \
	    [list [file join $TYCHO lib idx codeDoc.idx]] 1 1 $files
    cd $olddir
}


#### ptolemyStarHTMLIndex
# Update the Tycho code index.
# All files in the codeDoc directories with the extension .html in the
# tree rooted at the environment.
# Note that we assume that outputfilename and args are all in one
# directory, and we strip off any leading paths information
proc ptolemyStarHTMLIndex { domain outputfilename args} {
    if {$domain == "thor" || [llength $args] == 0} {
	puts "$domain does not contain stars that have .htm documentation,"
	puts "  creating $outputfilename"
	cd [file dirname $outputfilename]
	set fd [open [file tail $outputfilename] w]
	puts $fd "{thor stars}"
	puts $fd "{\n{{} {} {}}}"
	close $fd
	return
    }
    # Create a title from the outputfilename 
    # FIXME: this is a big time hack.
    set subdomain {}
    regexp {.*/domains/.*/(.*)/stars} $outputfilename dummy subdomain
    if {$subdomain != {}} {
	set title "$domain $subdomain stars"
    } else {
	set title "$domain stars"
    }
    set newargs {}
    foreach arg $args {
	lappend newargs [file tail $arg]
    }

    set olddir [pwd]
    cd [file dirname [lindex $args 0]]
    eval tychoMkIndex [list $title] [file tail $outputfilename] 0 0 $newargs
    cd $olddir
}

proc _tychoAdjustIndexItem { CWD item } {
    if {[llength $item] != 3} {
	error "Syntax error in index item: $item \n\
		Should contain the form { id filename point }"
    }

    # Fix the filename by prepending the CWD

    # Currently, we discard the subtitle, but we could add it in
    # and extend the IndexBrowser to be able to narrow the search
    #set name "[lindex $item 0] ($subtitle)"
    set name [lindex $item 0]
    set filename [lindex $item 1]
    set point [lindex $item 2]
		
    # Process the filename, and adjust accordingly
    if { [string range $filename 0 2] == "../" } {
	# If the filename starts with a leading '../' strip it off
	set newfilename [string range $filename 3 end]
    } elseif { [string index $filename 0] == "$"}  {
	# If the filename begins with $, then the user is using
	# a variable so just copy
	set newfilename $filename
    } else {
	# Append the current working directory on
	set newfilename $CWD/$filename
    }
    return [list $name $newfilename "$point"]
}

#### tychoMergeIndices
# Merge a bunch of indices that were generated by ptolemyStarHTMLIndex
# Also produce a forest that contains a the sub indices
#
proc tychoMergeIndices {title outputfilename args} {
    set entries {}

    # Dump out a forest that contains all the sub indices
    set forestfd [open $outputfilename.fst w]

    # Put in titles and a reasonable default size.
#    puts $forestfd \
#	    "\{configure -canvasheight 600\} \{configure -canvaswidth 800\}"

    puts $forestfd \
	    "\{centeredText \{$title\} title \{\} black \{\{helvetica 24 bold i\} \{times 24 bold i\}\}\}"

    puts $forestfd \
	    "\{add $outputfilename \{label \{$title\} link \{$outputfilename \{\}\}\} \{\}\}"

    # Open up each file and stick the contents into a list
    # Update the forest file too.
    foreach file $args {
	set isfilefst [file readable $file.fst ]
	set fd [open $file r]
	set contents [read $fd]
	close $fd

	# Remove comments.
	regsub -all "(^|\n)\[ \t\]*#\[^\n\]*(\n|$)" $contents {} contents

	set CWD [file dirname $file]
	
	if {[llength $contents] != 2} {
	    error "Syntax error in index file $file \n\
		    Should contain the form: name { items }, instead we got\n\
	            '$contents'"
	}
	set subtitle [lindex $contents 0]
	if {!$isfilefst} {
	    puts $forestfd \
		"\{add $file \{label \{$subtitle\} link \{$file \{\}\}\} $outputfilename \}"
	}
	set items {}
	set count 1
	foreach item [lindex $contents 1] {
	    # We have two styles of index items, two entry and three entry
	    if {[llength $item] == 2 } {
		# Two entry index items are recursive items
		set subindex [lindex [lindex $item 1] 1]
		set subentries {}
		foreach subitem $subindex {
		    lappend subentries [_tychoAdjustIndexItem $CWD $subitem]
		}
		lappend entries [list [lindex $item 0] \
			[list [lindex [lindex $item 1] 0] $subentries]]
	    } else {
		lappend entries [_tychoAdjustIndexItem $CWD $item]
	    }
	}

	# If there is a Forest file, then merge that too
	if {$isfilefst} {
	    set fd [open $file.fst r]
	    set contents [read $fd]
	    close $fd

	    foreach item $contents {
		if {[lindex $item 0] == "add"} {
		    set operator [lindex $item 0]
		    set nodename [lindex $item 1]
		    set contents [lindex $item 2]
		    set parent [lindex $item 3]
		    
		    # Assume that contents is 'label xxx link xxx'
		    set label [lindex $contents 1]
		    set link [lindex $contents 3]
		    set linkfile [lindex $link 0] 
		    set linkpoint [lindex $link 1] 

		    if {$parent == {} } {
			set newparent $outputfilename
		    } else {
			set newparent $CWD/$parent
		    }

		    # Write the new node
		    puts $forestfd \
			    "\{add $CWD/$nodename \{label \{$label\} link \{$CWD/$linkfile \{$linkpoint\}\}\} $newparent \}"
		}
	    }
	}
    }
    close $forestfd

    # Dump out the newly merged file
    set fd [open $outputfilename w]
    # Put a comment line that gives reasonable output in netscape.
    puts $fd "# <h1>This is a Tycho index file. \
            Use tycho to view it.</h1> <nothtml"
    puts $fd [list $title]
    puts $fd \{
    foreach entry [lsort -command tychoCompareFirst $entries] {
	puts $fd [list $entry]
    }
    puts $fd \}
    puts $fd "# >"
    close $fd
}

#### tychoVpathMergeIndices
# Like tychoMergeIndices, but takes a vpath argument
# contains a pathname that is deleted from all of the input args
# The outputfilename arg is not modified, only the pathnames to
# the index files contained in args
proc tychoVpathMergeIndices {title outputfilename vpath args} {
    set newargs {}
    foreach arg $args {
	regsub $vpath $arg {} newarg
	lappend newargs $newarg
    }
    eval tychoMergeIndices [list $title] $outputfilename $newargs
}
