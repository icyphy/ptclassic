# Generating star-demo cross indices
# $Id$
# Author: Brian L. Evans
#
# Copyright (c) 1990-1995 The Regents of the University of California.
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
#    ptkNormalizePathNames thestring
#    ptkStripSubDir dirpath
#    ptkStripPath dirpath
#    ptkGetStarBaseName fullstarname
#    ptkCheckWritableDir pathname
#    ptkSwapPairsInList facetpairs
#
# 2. Routines to read Oct facets
#
#    ptkFindOctFacetDirs pathname
#    ptkReadOctFacetDirs facetdirlist
#    ptkReadAllFacetPairs pathname
#
# 3. Routines to manipulate lists of facet pairs
#
#    ptkFacetPairsToTriplets facetpairs
#    ptkMakeStarDemoIndex pathname
#
# 4. Routines to convert lists of facet pairs into World Wide Web format
#
#    ptkStarDemoIndexToWWW starlist header
#    ptkMakeWWWOctFacetIndex pathname locationdesc
#    ptkMakeWWWStarDemoIndex domainname
#    ptkWriteWWWStarDemoDir domainlist wwwdirectory
#
# To generate the ptdesign directory public_html/star-demo-index with
# the star-demo indices for all released domains, use the command
#
# ptkWriteWWWStarDemoDir \
#     "bdf cg cgc cg56 cg96 de ddf mdsdf pn sdf silage thor vhdlb vhdlf" \
#     /users/ptdesign/public_html/star-demo-index
#
# This file does not depend on Tk.


# 1. SUPPORTING ROUTINES

# ptkNormalizePathNames
# replace the value of $PTOLEMY with $PTOLEMY
proc ptkNormalizePathNames { thestring } {
  global env
  if { [info exists env(PTOLEMY)] } {
    regsub -all $env(PTOLEMY) $thestring \$PTOLEMY newstring
    return $newstring
  }
  return $thestring
}

# ptkStripSubDir
# removes the sub-directory in a path name, e.g., 
# /users/ble/ptolemy becomes /users/ble
proc ptkStripSubDir { dirpath } {
  set lastpos [string last "/" $dirpath]
  if { $lastpos < 0 } {
    set newdirpath $dirpath
  } else {
    incr lastpos -1
    set newdirpath [string range $dirpath 0 $lastpos]
  }
  return $newdirpath
}

# ptkStripPath
# removes the path name and keeps the trailing filename, e.g., 
# /users/ble/ptolemy becomes ptolemy
proc ptkStripPath { dirpath } {
  set lastpos [string last "/" $dirpath]
  incr lastpos
  string range $dirpath $lastpos end
}

# ptkGetStarBaseName
# extracts the star's base name from its full name, e.g.
# MatlabCx_M.input=0.output=1 becomes MatlabCx_M
# /users/friend/ptolemy/src/Add.input=2 becomes Add
proc ptkGetStarBaseName { fullstarname } {
  set starname [ptkStripPath $fullstarname]
  set lastpos [string first "." $starname]
  if { $lastpos < 0 } {
    set basename $starname
  } else {
    incr lastpos -1
    set basename [string range $starname 0 $lastpos]
  }
  return $basename
}

# ptkCheckWritableDir
# returns 1 if the pathname is a writable directory, and 0 otherwise
proc ptkCheckWritableDir { pathname } {
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

# ptkSwapPairsInList
# for each pair in the list, {element1 element2}, swap the order of elements
proc ptkSwapPairsInList { facetpairs } {
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

# ptkFindOctFacetDirs
# recursively search for all facet directories in the directory tree pathname
# use "catch" in case the exec command causes an error
proc ptkFindOctFacetDirs { pathname } {
  # recursively find all schematic sub-directories (success if retval is 0)
  set retval [catch "exec find $pathname -name \"schematic\" -print" \
                    schematicFiles]

  # remove the schematic sub-directory from the list of file names
  set facetdirlist ""
  if { $retval == 0 } {
    foreach sfile $schematicFiles {
      lappend facetdirlist [ptkStripSubDir $sfile]
    }
  }
  return $facetdirlist
}

# ptkReadOctFacetDirs
# reads the contents of all facet directories in the facetlist
# use "catch" in case the exec command causes an error
proc ptkReadOctFacetDirs { facetdirlist } {
  set facetpairlist ""
  foreach facetdir $facetdirlist {
    set retval [catch "exec octls $facetdir" otherfacets]
    if { $retval != 0 } continue
    set facetpair [split $otherfacets "\n"]
    eval lappend facetpairlist $facetpair
  }
  return $facetpairlist
}

# ptkReadAllFacetPairs
# returns a list of {demopath, starpath} pairs for each
# palette found by recursively traversing the pathname
# use "catch" in case the exec command causes an error
proc ptkReadAllFacetPairs { pathname } {
  return [ptkReadOctFacetDirs [ptkFindOctFacetDirs $pathname]]
}


# 3. ROUTINES TO MANIPULATE LISTS OF FACET PAIRS

# ptkFacetPairsToTriplets
# for each facet pair, {demopath starpath}, swap the order and
# prepend the star name for sorting, which makes a triplet
# {starname starpath demopath}
proc ptkFacetPairsToTriplets { facetpairs } {
  set swapped ""
  set numelements [llength $facetpairs]
  for { set i 0 } { $i < $numelements } { incr i } {
    set pairlist [lindex $facetpairs $i]
    # first element: star name
    set triplet [ptkGetStarBaseName [ptkStripPath [lindex $pairlist 1]]]
    # second element: star path
    lappend triplet [lindex $pairlist 1]
    # third element: demo path
    lappend triplet [lindex $pairlist 0]
    lappend swapped $triplet
  }
  return $swapped
}

# ptkMakeStarDemoIndex
# a. create list of {demopath, starpath} pairs via ptkReadAllFacetPairs
# b. convert list to {starname, starpath, demopath} triplets via
#    ptkSwapPairs
# c. sort list of triplets so that the list is alphabetized by star name
# d. compress the lists of triplets to return a list of
#    {star demo1 demo2 demo3 ...}
proc ptkMakeStarDemoIndex { pathname } {
  set tripletlist \
      [lsort [ptkFacetPairsToTriplets [ptkReadAllFacetPairs $pathname]]]
  set numelements [llength $tripletlist]
  set demolist ""
  set laststarname ""
  set firsttimeflag 1

  for { set i 0 } { $i < $numelements } { incr i } {
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

# ptkStarDemoIndexToWWW
# convert a starlist returned by ptkMakeStarDemoIndex into World Wide Web format
proc ptkStarDemoIndexToWWW { starlist header } {
  set wwwcode "$header\n<ul>\n"
  set numelements [llength $starlist]
  for { set i 0 } { $i < $numelements } { incr i } {
    set treelist [lindex $starlist $i]
    set starname [lindex $treelist 0]
    set demolist [join [lrange $treelist 1 end] ", "]
    set wwwcode "$wwwcode<li>$starname: $demolist\n"
  }
  set wwwcode "$wwwcode</ul>\n"
  return $wwwcode
}

# ptkMakeWWWOctFacetIndex
# create a list in World Wide Web format of all stars
# used in Oct facets in a given directory tree
proc ptkMakeWWWOctFacetIndex { pathname locationdesc } {
  set header "Stars, galaxies, and universes $locationdesc"
  set starlist [ptkMakeStarDemoIndex $pathname]
  ptkNormalizePathNames [ptkStarDemoIndexToWWW $starlist $header]
}

# ptkMakeWWWStarDemoIndex
# generate a World Wide Web (WWW) index for a particular PTOLEMY domain
proc ptkMakeWWWStarDemoIndex { domainname } {
  global env
  if { ! [info exists env(PTOLEMY)] } return

  set lcdomainname [string tolower $domainname]
  set ucdomainname [string toupper $domainname]
  set pathname "$env(PTOLEMY)/src/domains/$lcdomainname"
  ptkMakeWWWOctFacetIndex $pathname "in the $ucdomainname domain"
}

# ptkWriteWWWStarDemoDir
# create an entire World Wide Web directory, including index.html
# it will backup "index.html" if it does not exist
# returns 1 if successful, and 0 if failure
proc ptkWriteWWWStarDemoDir { domainlist wwwdirectory } {
  # exit if the wwwdirectory is not writable
  if { ! [ptkCheckWritableDir $wwwdirectory] } {
    return 0
  }

  # write out the header of "index.html"
  set indexfilename "$wwwdirectory/index.html"
  if { [file exists $indexfilename] } {
    set retval [catch "exec mv $indexfilename $indexfilename~"]
  }
  set indexfile [open $indexfilename w]
  set title "Star, Galaxy, and Universe Cross-Index"
  puts $indexfile "<--"
  puts $indexfile "Automatically Generated by the Tcl Command"
  puts $indexfile "ptkWriteWWWStarDemoDir $domainlist $wwwdirectory"
  puts $indexfile "-->\n"
  puts $indexfile "<html>\n<head>\n<title>$title</title>\n</head>\n<body>\n"
  puts $indexfile "<h1>$title</h1>\n"
  puts $indexfile "Indices are available for the following Ptolemy domains:\n"
  puts $indexfile "<ul>"

  # write out each domain file
  foreach domain $domainlist {
    set f [open "$wwwdirectory/$domain.html" w]
    puts $f [ptkMakeWWWStarDemoIndex $domain]
    close $f
    puts $indexfile "<li><a href=\"$domain.html\">$domain</a>"
  }

  # write trailer and close "index.html"
  puts $indexfile "</ul>\n\n<p>\n<hr>\n\n</body>\n</html>"
  close $indexfile

  return 1
}
