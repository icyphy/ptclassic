# Generating star-demo cross indices
# @(#)starindex.tcl	1.12	10/05/96
# Authors: Brian L. Evans, Christopher Hylands
#
# Copyright (c) 1995-1998 The Regents of the University of California.
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
#                                               PT_COPYRIGHT_VERSION_2
#                                               COPYRIGHTENDKEY
#
# This file is organized as follows:
#
# 1. Routines to support the others
#
#    starindex_NormalizePathNames thestring
#    starindex_StripSubDir dirpath
#    starindex_StripPath dirpath
#    starindex_GetStarBaseName fullstarname
#    starindex_CheckWritableDir pathname
#    starindex_SwapPairsInList facetpairs
#
# 2. Routines to read Oct facets
#
#    starindex_FindOctFacetDirs pathname
#    starindex_ReadOctFacetDirs facetdirlist
#    starindex_ReadAllFacetPairs pathname
#
# 3. Routines to manipulate lists of facet pairs
#
#    starindex_FacetPairsToTriplets facetpairs
#    starindex_MakeStarDemoIndex pathname
#
# 4. Routines to convert lists of facet pairs into World Wide Web format
#
#    starindex_StarDemoIndexToWWW starlist header
#    starindex_MakeWWWOctFacetIndex pathname locationdesc
#    starindex_MakeWWWStarDemoIndex domainname
#    starindex_WriteWWWStarDemoDir domainlist wwwdirectory
#
# To generate the ptdesign directory public_html/star-demo-index with
# the star-demo indices for all released domains, use the command
#
# starindex_WriteWWWStarDemoDir \
#     "bdf cg cgc cg56 cg96 de ddf mdsdf pn sdf silage thor vhdl vhdlb vhdlf" \
#     /users/ptdesign/public_html/star-demo-index
#
# This file is a Tcl script.  It does not depend on Tk.


# 1. SUPPORTING ROUTINES

# starindex_NormalizePathNames
# replace the value of $PTOLEMY with $PTOLEMY
proc starindex_NormalizePathNames { thestring } {
  global env
  if { [info exists env(PTOLEMY)] } {
    regsub -all $env(PTOLEMY) $thestring \$PTOLEMY newstring
    return $newstring
  }
  return $thestring
}

# starindex_StripSubDir
# removes the sub-directory in a path name, e.g., 
# /users/ble/ptolemy becomes /users/ble
proc starindex_StripSubDir { dirpath } {
  set lastpos [string last "/" $dirpath]
  if { $lastpos < 0 } {
    set newdirpath $dirpath
  } else {
    incr lastpos -1
    set newdirpath [string range $dirpath 0 $lastpos]
  }
  return $newdirpath
}

# starindex_StripPath
# removes the path name and keeps the trailing filename, e.g., 
# /users/ble/ptolemy becomes ptolemy
proc starindex_StripPath { dirpath } {
  set lastpos [string last "/" $dirpath]
  incr lastpos
  string range $dirpath $lastpos end
}

# starindex_GetStarBaseName
# extracts the star's base name from its full name, e.g.
# MatlabCx_M.input=0.output=1 becomes MatlabCx_M
# /users/friend/ptolemy/src/Add.input=2 becomes Add
proc starindex_GetStarBaseName { fullstarname } {
  set starname [starindex_StripPath $fullstarname]
  set lastpos [string first "." $starname]
  if { $lastpos < 0 } {
    set basename $starname
  } else {
    incr lastpos -1
    set basename [string range $starname 0 $lastpos]
  }
  return $basename
}

# starindex_CheckWritableDir
# returns 1 if the pathname is a writable directory, and 0 otherwise
proc starindex_CheckWritableDir { pathname } {
  # check for existence and protection on the wwwdirectory
  if { ! [file exists $pathname] } {
    puts "$pathname does not exist"
    return 0
  }
  if { ! [file isdirectory $pathname] } {
    puts "$pathname is not a directory"
    return 0
  }
  if { ! [file writable $pathname] } {
    puts "The directory $pathname does not have write permission"
    return 0
  }
  return 1;
}

# starindex_SwapPairsInList
# for each pair in the list, {element1 element2}, swap the order of elements
proc starindex_SwapPairsInList { facetpairs } {
  set swapped ""
  set numelements [llength $facetpairs]
  for { set i 0 } { $i < $numelements } { incr i } {
    set pairlist [lindex $facetpairs $i]
    # get second element for first location
    set newpair [lindex $pairlist 1]
    # get first element for second location
    lappend newpair [lindex $pairlist 0]
    lappend swapped $newpair
  }
  return $swapped
}


# 2. ROUTINES TO READ CONTENTS OF OCT FACETS

# starindex_FindOctFacetDirs
# recursively search for all facet directories in the directory tree pathname
# use "catch" in case the exec command causes an error
proc starindex_FindOctFacetDirs { pathname } {
  puts -nonewline "Finding Oct Facet Directories . . . "
  flush stdout
  # recursively find all schematic sub-directories (success if retval is 0)
  set retval [catch "exec find $pathname -follow -name \"schematic\" -print" \
                    schematicFiles]

  # remove the schematic sub-directory from the list of file names
  set facetdirlist ""
  if { $retval == 0 } {
    foreach sfile $schematicFiles {
      lappend facetdirlist [starindex_StripSubDir $sfile]
    }
  } else {
      puts "find returned $retval: $schematicFiles"
  }
  #puts "dbg: starindex_FindOctFacetDirs $pathname $facetdirlist"
  puts "[llength $facetdirlist] directories found."
  return $facetdirlist
}

# starindex_ReadOctFacetDirs
# reads the contents of all facet directories in the facetlist
# use "catch" in case the exec command causes an error
proc starindex_ReadOctFacetDirs { facetdirlist } {
  set facetpairlist ""
  puts -nonewline "Reading Oct Facets, Directory Count: "
  foreach facetdir $facetdirlist {
    puts -nonewline "."
    flush stdout
    set retval [catch "exec octls $facetdir" otherfacets]
    #puts "dbg: starindex_ReadOctFacetDirs: $otherfacets"
    if { $retval != 0 } continue
    set facetpair [split [starindex_NormalizePathNames $otherfacets] "\n"]
    eval lappend facetpairlist $facetpair
  }
  puts "Done."
  return $facetpairlist
}

# starindex_ReadAllFacetPairs
# returns a list of {demopath, starpath} pairs for each
# palette found by recursively traversing the pathname
# use "catch" in case the exec command causes an error
proc starindex_ReadAllFacetPairs { pathname } {
  return [starindex_ReadOctFacetDirs [starindex_FindOctFacetDirs $pathname]]
}


# 3. ROUTINES TO MANIPULATE LISTS OF FACET PAIRS

# starindex_FacetPairsToTriplets
# for each facet pair, {demopath starpath}, swap the order and
# prepend the star name for sorting, which makes a triplet
# {starname starpath demopath}
proc starindex_FacetPairsToTriplets { facetpairs } {
  set swapped ""
  set numelements [llength $facetpairs]
  # get the triplets: star name, star path, demo path 
  for { set i 0 } { $i < $numelements } { incr i } {
    set pairlist [lindex $facetpairs $i]
    set triplet [starindex_GetStarBaseName [starindex_StripPath [lindex $pairlist 1]]]
    lappend triplet [lindex $pairlist 1]
    lappend triplet [lindex $pairlist 0]
    lappend swapped $triplet
  }
  return $swapped
}

# starindex_MakeStarDemoIndex
# a. create list of {demopath, starpath} pairs via starindex_ReadAllFacetPairs
# b. convert list to {starname, starpath, demopath} triplets via
#    starindex_SwapPairs
# c. sort list of triplets so that the list is alphabetized by star name
# d. compress the lists of triplets to return a list of
#    {star demo1 demo2 demo3 ...}
proc starindex_MakeStarDemoIndex { pathname } {
    #puts "dbg: starindex_MakeStarDemoIndex $pathname"
  set tripletlist \
      [lsort [starindex_FacetPairsToTriplets [starindex_ReadAllFacetPairs $pathname]]]
  set numelements [llength $tripletlist]
  puts "Done sorting triplet list, $numelements elements."
  set demolist ""
  set laststarname ""
  set firsttimeflag 1

  for { set i 0 } { $i < $numelements } { incr i } {
    # puts "dbg: starindex_MakeStarDemoIndex $i"
    set triplet [lindex $tripletlist $i]
    set starname [lindex $triplet 0]
    set demopath [lindex $triplet 2]

    # first time through, initialize demolist and laststarname
    if { $firsttimeflag } {
      set firsttimeflag 0
      set laststarname $starname
      set demolist $demopath
    # check if the current star name is different from the last star name
    } elseif { "$starname" != "$laststarname" } {
      lappend treeform [concat $laststarname [lsort $demolist]]
      set laststarname $starname
      set demolist $demopath
    # add the demo path if it's not in the current list of demos for star
    } elseif { [lsearch $demolist $demopath] < 0 } {
      lappend demolist $demopath
    }
  }

  if { $numelements > 0 } {
    lappend treeform [concat $laststarname [lsort $demolist]]
  } else {
    set treeform ""
  }

  return $treeform
}


# 4. ROUTINES TO CONVERT LISTS OF FACET PAIRS TO WORLD WIDE WEB FORMAT

# starindex_StarDemoIndexToWWW
# convert a starlist returned by starindex_MakeStarDemoIndex
# into World Wide Web format
proc starindex_StarDemoIndexToWWW { fd starlist header } {
  #puts "dbg: starindex_StarDemoIndexToWWW \{ starlist $header\}"
  puts $fd "$header\n<ul>\n"
  set numelements [llength $starlist]
  for { set i 0 } { $i < $numelements } { incr i } {
    set treelist [lindex $starlist $i]
    set starname [lindex $treelist 0]
    puts -nonewline $fd "\n<li>$starname: \
            <a href=\"[lindex $treelist 1]\">[lindex $treelist 1]</a>"
    foreach demo [lrange $treelist 2 end]  {
	puts -nonewline $fd ", <a href=\"$demo\">$demo</a>"
    }
  }
  puts $fd "</ul>\n<hr>Updated: [clock format [clock seconds]]\n"
}

# starindex_MakeWWWOctFacetIndex
# create a list in World Wide Web format of all stars
# used in Oct facets in a given directory tree
proc starindex_MakeWWWOctFacetIndex { fd pathname locationdesc } {
  set header "Stars, galaxies, and universes $locationdesc"
  set starlist [starindex_MakeStarDemoIndex $pathname]
  #puts "dbg: starindex_MakeWWWOctFacetIndex $pathname $locationdesc"
  #starindex_NormalizePathNames [starindex_StarDemoIndexToWWW $starlist $header]
  starindex_StarDemoIndexToWWW $fd $starlist $header
}

# starindex_MakeWWWStarDemoIndex
# generate a World Wide Web (WWW) index for a particular PTOLEMY domain
proc starindex_MakeWWWStarDemoIndex { fd domainname } {
  global env
  if { ! [info exists env(PTOLEMY)] } return

  set lcdomainname [string tolower $domainname]
  set ucdomainname [string toupper $domainname]
  set pathname "$env(PTOLEMY)/src/domains/$lcdomainname"
  starindex_MakeWWWOctFacetIndex $fd $pathname "in the $ucdomainname domain"
}

###############################################################
# The entry points to procs in this file are below this point
#

# starindex_WriteWWWStarDemoDir
# create an entire World Wide Web directory, including index.html
# it will backup "index.html" if it does not exist
# returns 1 if successful, and 0 if failure
proc starindex_WriteWWWStarDemoDir { domainlist wwwdirectory } {
  # exit if the wwwdirectory is not writable
  if { ! [starindex_CheckWritableDir $wwwdirectory] } {
    return 0
  }

  # write out the header of "index.html"
  set indexfilename "$wwwdirectory/index.html"
  if { [file exists $indexfilename] } {
    set retval [catch "exec mv $indexfilename $indexfilename~"]
  }
  set indexfile [open $indexfilename w]
  set title "Star, Galaxy, and Universe Cross-Index"
  puts $indexfile "<!--"
  puts $indexfile "Automatically Generated by the Tcl Command"
  puts $indexfile "starindex_WriteWWWStarDemoDir $domainlist $wwwdirectory"
  puts $indexfile "-->\n"
  puts $indexfile "<html>\n<head>\n<title>$title</title>\n</head>\n<body>\n"
  puts $indexfile "<h1>$title</h1>\n"
  puts $indexfile "Indices are available for the following Ptolemy domains:\n"
  puts $indexfile "<ul>"

  # write out each domain file
  foreach domain $domainlist {
    puts -nonewline "$domain "
    flush stdout  
    set f [open "$wwwdirectory/$domain.html" w]
    starindex_MakeWWWStarDemoIndex $f $domain
    close $f
    puts $indexfile "<li><a href=\"$domain.html\">$domain</a>"
  }

  # write trailer and close "index.html"
  puts $indexfile "</ul>\n\n<p>\n<hr>"
  puts $indexfile "Updated: [clock format [clock seconds]]\n\n</body>\n</html>"
  close $indexfile

  return 1
}

# starindex_WriteDemoIndex
# Create a starDemo.idx file from the demos in the current directory
# returns 1 if successful, and 0 if failure
proc starindex_WriteDemoIndex { domainname } {
    set outfile starDemo.idx

    set fd [open $outfile w]

    global env
    if { ! [info exists env(PTOLEMY)] } return
   
    set lcdomainname [string tolower $domainname]
    set ucdomainname [string toupper $domainname]
    # If we add 'demo' to this pathname, then we don't get the palettes
    set pathname "$env(PTOLEMY)/src/domains/$lcdomainname"

    set header "$lcdomainname stars/demo-palette cross reference"
    set starlist [starindex_MakeStarDemoIndex $pathname]

    #puts "dbg: starlist: $starlist"

    puts -nonewline " About to generate $outfile. . ."
    puts $fd "\{$header\}\n \{"
    set numelements [llength $starlist]
    for { set i 0 } { $i < $numelements } { incr i } {
	set treelist [lindex $starlist $i]
	set starname [lindex $treelist 0]
	
	if { [llength $treelist ] > 2} {
	    # We have a recursive index, that is an index that has
	    # one key, and multiple entries.
	    puts $fd "\{\{$starname facet, $ucdomainname users\} \{"
	    puts $fd " \{Users of $starname\} \{"
	    foreach demo [lrange $treelist 1 end]  {
		# Keep track of each demo we see for the demo.idx
		set demoArray($demo) 1
		puts $fd "  \{$demo $demo \{\}\} "
	    }
	    puts $fd "  \}\n \}\n\}"
	} else {
	    # We have a simple index entry, a key and a value
	    puts $fd "\{\{$starname facet, $ucdomainname user\} \
		    [lindex $treelist 1] \{\}\}"
   	    set demoArray([lindex $treelist 1]) 1
	}
    }
    puts $fd "\}"
    close $fd

    # Generate the demo.idx file, which is an index file that lists
    # all the demos
    set outfile demo.idx
    puts -nonewline " About to generate $outfile. . ."
    set fd [open $outfile w]
    set header "$lcdomainname demos and palettes"
    puts $fd "\{$header\}\n \{"
    foreach el [lsort [array names demoArray]] {
	set facetName [file tail $el]
	if {[file extension $facetName] == ".pal"} {
	    puts $fd "{[list "$facetName palette, $ucdomainname domain"]\
		    $el {}}"
	} else {
	    puts $fd "{[list "$facetName universe, $ucdomainname domain"]\
		    $el {}}"
	}
    }

    puts $fd "\}"
    close $fd
    puts "Done."
    return 1
}

