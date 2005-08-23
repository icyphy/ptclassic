# This Tcl script works recursively on a Ptolemy tree.
#
# Copyright (c) 1990-1999 The Regents of the University of California.
# All Rights Reserved.
# See $PTOLEMY/copyright for the complete copyright notice.
#
# Author: Kennard White
# Version: @(#)ptfixtree.tcl	1.16	01/28/99

proc pftUsage { {msg} "Usage Information" } {
    puts stderr $msg
    puts stderr {usage: ptfixtree [-list] [-v] [+R] [-within path] [-d domain] ... facets ...}
    puts stderr "\t-v\tturns on verbose message."
    puts stderr "\t+R\tturns off recursive traversal of heirarchy."
    puts stderr "\t-within\tspecifies a path which all facets must be within"
    puts stderr "\t\tmultiple -within options may be given.  Remember to"
    puts stderr "\t\tsingle quote paths containing $ and ~ terms.  If no"
    puts stderr "\t\t-within option is given, cells may be anywhere."
    puts stderr "\t-list\tThe masters in each facet will be listed."
    puts stderr "\t\tNo changes will be made to the facets."

    puts stderr "\nThe program will examine each facet, verifying the masters"
    puts stderr "in the facet.  If a master does not have a valid"
    puts stderr "path, is not a valid OCT facet, or is not within one of paths"
    puts stderr "specified by -within, then the user will be prompted for"
    puts stderr "a replacement cell.  The facet will be fixed using octmvlib."

    puts stderr "\nTypical usage would be:"
    puts stderr "\tptfixtree chirp modulate"
    puts stderr "or"
    puts stderr "\tptfixtree -within '\$PTOLEMY' -within '~me' chirp modulate"
    puts stderr "where chirp and modulate are ptolemy facets.  The second form"
    puts stderr "would be used to verify that no cells in obscure directories"
    puts stderr "are referenced."

    puts stderr "\nThe -d option specified that all facets visited should"
    puts stderr "\tbe converted to 'domain'"

    puts stderr "\nAdditional (rarely used) options:"
    puts stderr "\t-octls path\tSpecifies location of octls binary"
    puts stderr "\t-octmvlib path\tSpecifies location of octmvlib binary"
    puts stdout "\t\t\tCurrent version uses octfix, not octmvlib"

    puts stderr "\nSend bug reports to ptolemy@eecs.berkeley.edu"
    puts stderr "Please include *all* program output."

    exit 1
}


proc pftGetFullFacet { facet } {
    if { "$facet"=="" } {
	error "Missing cell name."
    }
    if { [llength [split $facet :]] == 1 } {
	append facet ":schematic"
    }
    if { [llength [split $facet :]] == 2 } {
	append facet ":interface"
    }
    return $facet
}

proc pftGetContentsFacet { facet } {
    if { "$facet"=="" } {
	error "Missing cell name."
    }
    set finfo [split $facet :]
    set viewName [expr { [llength $finfo] == 1 
      ? "schematic" : [lindex $finfo 1] } ]
    return "[lindex $finfo 0]:$viewName:contents"
}

proc pftOctMvLib { facet oldpat newpat } {
    global path_octmvlib do_verbose

    set facet [pftGetFullFacet $facet]
    puts stdout "    $facet: $oldpat --> $newpat"
    if [catch {exec $path_octmvlib -N $newpat -O $oldpat $facet} result] {
	puts stderr "********* octfix failed ***********"
	puts stderr "failed on: $facet: $oldpat --> $newpat"
	puts stderr "$result"
	exit 1
    }
    if { $do_verbose } {
        puts stdout "\t\t$result"
    }
}


proc pftOctReDomain { facet olddomain newdomain } {
    global path_octredomain do_verbose

    set facet [pftGetFullFacet $facet]
    puts stdout "    $facet: changing domain to $newdomain"
    set capnewdomain [string toupper $newdomain]
    if [catch {exec $path_octredomain -d $capnewdomain $facet} result] {
	puts stderr "********* octredomain failed ***********"
	puts stderr "failed on: $facet: $olddomain --> $newdomain"
	puts stderr "$result"
	exit 1
    }
    if { $do_verbose } {
        puts stdout "\t\t$result"
    }
}



proc pftGetUniqueList { oldlist } {
    set oldlist [lsort $oldlist]
    set newlist ""
    set previtem ""
    foreach item $oldlist {
	if { "$item"!="$previtem" } {
	    lappend newlist $item
	    set previtem $item
	}
    }
    return $newlist
}

proc pftOctLs { facet } {
    global path_octls
    set masterlines [exec $path_octls -f $facet]
    return [pftGetUniqueList [split $masterlines "\n"]]
}

# checkcells is an array of cells we still need to check
# goodcells is an array of cells we have already checked
proc pftAddCheckCells { cells } {
    global checkcells goodcells
    foreach facet $cells {
        set facet [pftGetContentsFacet $facet]
	if { [info exists goodcells($facet)] } {
	    continue
	}
	set checkcells($facet) 1
    }
}

proc pftSetGoodFacet { facet mode } {
    global checkcells goodcells

    set facet [pftGetContentsFacet $facet]
    if { [info exists goodcells($facet)] } {
	    error "Cell $facet is already good"
    }
    set goodcells($facet) $mode
    unset checkcells($facet)
}

proc pftExpandCellPath { cell } {
    global env

    set cellPathExpr $cell
#    puts stdout "Expanding $cellPathExpr"
    regsub -all {\$([^/]*)} $cell {$env(\1)} cellPathExpr
    set cellPath [eval concat $cellPathExpr]
    return [glob $cellPath]
}


proc pftIsFacetB { facet {whyvar ""} {expand ""} } {
    set facet [pftGetFullFacet $facet]
    set finfo [split $facet :]
    set cell [lindex $finfo 0]
    set cellPath $cell
    if { "$whyvar"!="" } {
	upvar $whyvar why
    }
    if { "$expand"!="" } {
	if { [catch {pftExpandCellPath $cell} cellPath] } {
	    set why "Invalid facet(path): $facet\n\t(error $cellPath)"
	    return 0
	}
    }
    if { ![file isdir $cellPath] || ![file isdir $cellPath/[lindex $finfo 1]] 
      || ![file isfile "$cellPath/[lindex $finfo 1]/[lindex $finfo 2];"] } {
	set why "Invalid facet(OCT): $facet\n\t(cellPath $cellPath)"
	return 0
    }
    return 1
}

proc pftIsWithinB { facet } {
    global within_paths
    if { "$within_paths"=="" } {
	return 1
    }
    set cell [lindex [split $facet :] 0]
    foreach path $within_paths {
#puts stdout "WithinB: ``$cell'' ``$path''"
	if { [string match "$path*" $cell] } {
	    return 1
	}
    }
    return 0
}

proc pftIsGoodFacetB { facet {whyvar ""} } {
    if { "$whyvar"!="" } {
	upvar $whyvar why
    }
    if { ![pftIsWithinB $facet] } {
	set why "Invalid facet(within): $facet"
	return 0
    }
    return [pftIsFacetB $facet why expand]
}

#
# See if our database already lists a mapping for $cell.  If so, return
# the replacement cell, otherwise empty string.  $cell must be a cell
# and not a facet
#
proc pftGetCellMap { cell } {
    global mapcells
    if { [info exist mapcells($cell)] } {
	return $mapcells($cell)
    }
    return ""
}

proc pftSetCellMap { cell newcell } {
    global mapcells
    global mapcelldirs
    if { [info exist mapcells($cell)] } {
	error "Mapping for cell $cell redefined."
    }
    set mapcells($cell) $newcell
    set celldir [file dirname $cell]
    if { "$celldir" != "." } {
	# We don't use file dirname, because it expands ~
	set celltail [file tail $newcell]
	regsub "/$celltail$" $newcell "" newcelldir
	set mapcelldirs($celldir) $newcelldir
	puts "Adding $celldir -> $newcelldir"
    }
}

proc pftGetCellDirMap { cell } {
    global mapcelldirs
    set celldir [file dirname $cell]
    if { [info exist mapcelldirs($celldir)] } {
	return $mapcelldirs($celldir)
    }
    return "" 
}

#
# Prompt the user for a replacement for $cell.
#
proc pftPromptSubst { cell } {
    while { ![eof stdin] } {
	puts stdout "  Enter replacement for $cell:"
	set celltail [file tail $cell]
	set newcelldir [pftGetCellDirMap $cell]
	if { "$newcelldir"!="" } {
	    puts stdout "  Hit return for $newcelldir/$celltail\n"
	}
	puts stdout "\n\t===> " nonew
	flush stdout
	set newfacet [gets stdin]
	if { "$newfacet"==":skip" } {
	    error ":skip"
	}
	if { "$newfacet"=="" } {
	    set newfacet "$newcelldir/$celltail"

	}
	if [pftIsGoodFacetB $newfacet why] {
	    return $newfacet
	}
	puts stdout "  Your replacement is not valid:\n\t$why"
	puts stdout "\t...try Again.  Type :skip to skip this facet. "
    }
    puts stdout "Yikes: <EOF> on stdin! Bye!"
    exit 1
}

proc pftPromptAndReplace { facet master } {
    global mapcells

    set oldcell [lindex [split $master :] 0]
    set newcell [pftGetCellMap $oldcell]
    if { "$newcell"=="" } {
    	set newcell [pftPromptSubst $oldcell]
	pftSetCellMap $oldcell $newcell
    }
    pftOctMvLib $facet $oldcell: $newcell
}



proc pftFixFacet { facet } {
    global mapcells do_recur do_verbose do_listonly do_domainchange

    if { ![pftIsFacetB $facet why expand] } {
	puts stdout "\tCan't examine $facet:\n\t$why\n\tSkipping..."
	return "skip"
    }
    if { [catch {set masters [pftOctLs $facet]} why]} {
	puts stdout "\toctls failed to open $facet:\n\t$why\n\tSkipping..."
	return "skip"	
    }
    if { "$do_domainchange" != "0" } {
	regsub -all {:} $facet {/} facetpath
	if [file writable "$facetpath\;"] {
	    pftOctReDomain $facet "dummy" $do_domainchange
	} else {
	    puts "Can't redomain $facet, it is not writable"
	}
	set substr "src/domains/[string tolower $do_domainchange]/"
	foreach master $masters {
	    set doit 1
	    set oldcell [lindex [split $master :] 0]
	    set newcell [pftGetCellMap $oldcell]
	    if { "$newcell"=="" } {
		if { ![regsub {src/domains/[^/]+/} $oldcell $substr newcell]} {
		    puts "Can't substitute $substr into $master"
		    set doit 0
		} else {
		pftSetCellMap $oldcell $newcell
		}
	    }
	    if {$doit} {
		pftOctMvLib $facet $oldcell: $newcell
	    }

	}
    } else {
	foreach master $masters {
	    if { $do_verbose } {
		puts stdout "  Verify master $master"
	    }
	    set minfo [split $master ":"]
	    if { [pftGetCellMap [lindex $minfo 0]] != "" } {
		pftPromptAndReplace $facet $master
		return "redo"
	    }
	    if { ![pftIsGoodFacetB $master why] } {
		puts stdout "\tBad master:\n\t$why"
		if [catch {pftPromptAndReplace $facet $master} why] {
		    if { "$why"==":skip" } {
			puts stdout ">>>>> $facet NOT fixed (bad $master) <<<<<"
			return "skip"
		    }
		    error $why
		}
		return "redo"
	    }
	}
    }
    if { $do_recur } {
    	pftAddCheckCells $masters
    }
    return "done"
}

proc pftListFacet { facet } {
    global mapcells do_recur

    if { ![pftIsFacetB $facet why expand] } {
	puts stdout "\tCan't examine $facet:\n\t$why\n\tSkipping..."
	return
    }
    set finfo [split $facet ":"]
    if { [catch {set masters [pftOctLs $facet]} why]} {
	puts stdout "\toctls failed to open $facet:\n\t$why\n\tSkipping..."
	return
    }
    puts stdout "[lindex $finfo 0]:[lindex $finfo 1] contains" no
    if { "$masters"=="" } {
	puts stdout " no masters"
	return
    } else {
	puts stdout ": "
    }
    foreach master $masters {
	set minfo [split $master ":"]
	set badstr ""
	if { ![pftIsGoodFacetB $master why] } {
	    set badstr "BAD($why)"
	}
	puts stdout "\t[lindex $minfo 0]:[lindex $minfo 1] $badstr"
    }
    if { $do_recur } {
    	pftAddCheckCells $masters
    }
}

proc pftMainLoop { } {
    global checkcells do_listonly
    while 1 {
	set facets [array names checkcells]
	if { "$facets" == "" } {
	    break
	}
	foreach facet $facets {
	    if { $do_listonly } {
	        pftListFacet $facet
		pftSetGoodFacet $facet "done"
	    } else {
     		set finfo [split $facet :]
    		set cell [lindex $finfo 0]
		set cellPath $cell
		if { [catch {pftExpandCellPath $cell} cellPath] } {
		    puts stdout "Invalid facet(path): $facet\n\t(error $cellPath)"
		}
   		if { [file isdir "$cellPath"] && 
		     [file isdir "$cellPath/[lindex $finfo 1]"] && 
		     [file isfile \
			"$cellPath/[lindex $finfo 1]/[lindex $finfo 2];"] && 
		     ! [file writable \
			"$cellPath/[lindex $finfo 1]/[lindex $finfo 2];"]} {
		    puts stdout "Skipping $facet"
 		    pftSetGoodFacet $facet "done"
   		} else {
		    puts stdout "Examining $facet"
		    while 1 {
			set status [pftFixFacet $facet]
			if { "$status"!="redo" } {
			    pftSetGoodFacet $facet $status
			    break
		    	}
		    }
		}
	    }
	}
    }
}

proc pftProcessArgs { } {
    global argv env checkcells do_recur within_paths do_verbose do_listonly
    global do_domainchange
    global path_octls path_octmvlib path_octredomain
    set within_paths ""
    set do_verbose 0
    set do_recur 1
    set do_listonly 0
    set do_domainchange 0
    set path_octmvlib ""
    set path_octls ""
    set path_octredomain ""

    set argCells [topgetopt {
      {v do_verbose boolean} 
      {R do_recur boolean} 
      {list do_listonly boolean} 
      {d do_domainchange} 
      {within within_paths append} 
      {octmvlib path_octmvlib} 
      {octls path_octls} 
      } $argv]
    if [llength $argCells]==0 {
	pftUsage "No cells specified."
    }
    pftAddCheckCells $argCells

    if { $path_octmvlib == "" } {
        set path_octmvlib $env(PTOLEMY)/bin.$env(PTARCH)/octfix
    }
    if { ![file isfile $path_octmvlib] || ![file exec $path_octmvlib] } {
	puts stderr "Can't find octmvlib program."
	puts stderr "(tried $path_octmvlib)"
	pftUsage "Setup failed."
    }

    if { $path_octls == "" } {
        set path_octls $env(PTOLEMY)/bin.$env(PTARCH)/octls
    }
    if { ![file isfile $path_octls] || ![file exec $path_octls] } {
	puts stderr "Can't find octls program."
	puts stderr "(tried $path_octls)"
	pftUsage "Setup failed."
    }

    if { $path_octredomain == "" } {
        set path_octredomain $env(PTOLEMY)/bin.$env(PTARCH)/octredomain
    }
    if { ![file isfile $path_octredomain] || ![file exec $path_octredomain] } {
	puts stderr "Can't find octredomain program."
	puts stderr "(tried $path_octredomain)"
	pftUsage "Setup failed."
    }
}


#   In Ptolemy 0.7.1 and earlier, some of the facets in
#   $PTOLEMY/lib/ptolemy had the % character in their names, which
#   causes no end of trouble under NT4.0 with Cygwin 20.1.
#   In Ptolemy 0.7.1 and later, we changed the name of these
#   files and substituted in the string "percent" for the character
#   "%".
#   See also $PTOLEMY/bin/fixntpaths, src/pigilib/local.h

proc pftPreLoadMap {} {
    global env
    if [file isdirectory "$env(PTOLEMY)/lib/colors/ptolemy/percentcCursor"] {
	puts "Preloading \$PTOLEMY/lib/colors/ptolemy/% conversions"
	pftSetCellMap {$PTOLEMY/lib/colors/ptolemy/%cCursor} \
		{$PTOLEMY/lib/colors/ptolemy/percentcCursor}

	pftSetCellMap {$PTOLEMY/lib/colors/ptolemy/%dBus} \
		{$PTOLEMY/lib/colors/ptolemy/percentdBus}

	pftSetCellMap {$PTOLEMY/lib/colors/ptolemy/%dDelay} \
		{$PTOLEMY/lib/colors/ptolemy/percentdDelay}

	pftSetCellMap {$PTOLEMY/lib/colors/ptolemy/%dDelay2} \
		{$PTOLEMY/lib/colors/ptolemy/percentdDelay2}

	pftSetCellMap {$PTOLEMY/lib/colors/ptolemy/%iNewGal} \
		{$PTOLEMY/lib/colors/ptolemy/percentiNewGal}

	pftSetCellMap {$PTOLEMY/lib/colors/ptolemy/%iNewPal} \
		{$PTOLEMY/lib/colors/ptolemy/percentiNewPal}

	pftSetCellMap {$PTOLEMY/lib/colors/ptolemy/%iNewStar} \
		{$PTOLEMY/lib/colors/ptolemy/percentiNewStar}

	pftSetCellMap {$PTOLEMY/lib/colors/ptolemy/%iNewUniv} \
		{$PTOLEMY/lib/colors/ptolemy/percentiNewUniv}

	pftSetCellMap {$PTOLEMY/lib/colors/ptolemy/%pInput} \
		{$PTOLEMY/lib/colors/ptolemy/percentpInput}

	pftSetCellMap {$PTOLEMY/lib/colors/ptolemy/%pOutput} \
		{$PTOLEMY/lib/colors/ptolemy/percentpOutput}

	# lib/technology is usually a link to lib/colors
	pftSetCellMap {$PTOLEMY/lib/technology/ptolemy/%dDelay} \
		{$PTOLEMY/lib/technology/ptolemy/percentdDelay}

	pftSetCellMap {$PTOLEMY/lib/technology/ptolemy/%pInput} \
		{$PTOLEMY/lib/technology/ptolemy/percentpInput}

	pftSetCellMap {$PTOLEMY/lib/technology/ptolemy/%pOutput} \
		{$PTOLEMY/lib/technology/ptolemy/percentpOutput}

	# NT can't have files or directories named 'con'
	pftSetCellMap {$PTOLEMY/lib/colors/ptolemy/con} \
		{$PTOLEMY/lib/colors/ptolemy/con0}
    }
}

proc pftMain { } {
    global env

    puts stdout "Ptolemy Fix Tree (ptfixtree version 0.1)"

    if ![info exist env(PTOLEMY)] {
	set env(PTOLEMY) [glob ~ptolemy]
    }
    if ![info exist env(PTARCH)] {
	if { [catch {exec $env(PTOLEMY)/bin/ptarch} arch] } {
	    pftUsage "Unknown PTARCH: $arch"
	}
	set env(PTARCH) $arch
    }

    uplevel #0 source $env(PTOLEMY)/lib/tcl/topgetopt.tcl

    pftProcessArgs
    pftPreLoadMap
    pftMainLoop
}


pftMain
