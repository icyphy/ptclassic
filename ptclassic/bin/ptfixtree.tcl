#!/usr/tools/bin/wish -f
#
# ptfixtree (pft)
# 
# Author: Kennard White (kennard@ohm)
#
# $Id$
#

proc pftUsage { {msg} "Usage Information" } {
    puts stderr $msg
    puts stderr {usage: ptfixtree [-list] [-v] [+R] [-within path] ... facets ...}
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
    puts stderr "\tptfixtree -within '\$PTOLEMY' -within '~me' chirp modulate"
    puts stderr "where chirp and modulate are ptolemy facets."

    puts stderr "\nAdditional (rarely used) options:"
    puts stderr "\t-octls path\tSpecifies location of octls binary"
    puts stderr "\t-octmvlib path\tSpecifies location of octmvlib binary"
    puts stdout "\t\t\tCurrent version uses octfix, not octmvlib"

    puts stderr "\nSend bug reports to kennard@ohm."
    puts stderr "Please include *all* program output."

    exit 1
}

source ~ptolemy/tcl/lib/topgetopt.tcl

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
	puts stderr "********* octmvlib failed ***********"
	puts stderr "failed on: $facet: $oldpat --> $newpat"
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
    if { [info exist mapcells($cell)] } {
	error "Mapping for cell $cell redefined."
    }
    set mapcells($cell) $newcell
}

#
# Prompt the user for a replacement for $cell.
#
proc pftPromptSubst { cell } {
    while { ![eof stdin] } {
	puts stdout "  Enter replacement for $cell:\n\t===> " nonew
	flush stdout
	set newfacet [gets stdin]
	if { "$newfacet"==":skip" } {
	    error ":skip"
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
    global mapcells do_recur do_verbose do_listonly

    if { ![pftIsFacetB $facet why expand] } {
	puts stdout "\tCan't examine $facet:\n\t$why\n\tSkipping..."
	return "skip"
    }
    set masters [pftOctLs $facet]
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
    if { $do_recur } {
    	pftAddCheckCells $masters
    }
    return "done"
}

proc pftListFacet { facet } {
    global mapcells do_recur do_verbose do_listonly

    if { ![pftIsFacetB $facet why expand] } {
	puts stdout "\tCan't examine $facet:\n\t$why\n\tSkipping..."
	return
    }
    set finfo [split $facet ":"]
    set masters [pftOctLs $facet]
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

proc pftProcessArgs { } {
    global argv env checkcells do_recur within_paths do_verbose do_listonly
    global path_octls path_octmvlib
    set within_paths ""
    set do_verbose 0
    set do_recur 1
    set do_listonly 0
    set path_octmvlib ""
    set path_octls ""

    set argCells [topgetopt {
      {v do_verbose boolean} 
      {R do_recur boolean} 
      {list do_listonly boolean} 
      {within within_paths append} 
      {octmvlib path_octmvlib} 
      {octls path_octls} 
      } $argv]
    if [llength $argCells]==0 {
	pftUsage "No cells specified."
    }
#puts stdout "WithinPaths: ``$within_paths''"
    pftAddCheckCells $argCells

    if ![info exist env(PTOLEMY)] {
	set env(PTOLEMY) [glob ~ptolemy]
    }
    if ![info exist env(ARCH)] {
	if { [catch {exec /bin/arch} arch] } {
	    if { [catch {exec /bin/machine} arch] } {
		pftUsage "Unknown ARCH"
	    }
	}
	set env(ARCH) $arch
    }
#        set path_octmvlib $env(PTOLEMY)/octtools/bin.$env(ARCH)/octmvlib
    if { $path_octmvlib == "" } {
        set path_octmvlib $env(PTOLEMY)/bin.$env(ARCH)/octfix
    }
    if { ![file isfile $path_octmvlib] || ![file exec $path_octmvlib] } {
	puts stderr "Can't find octmvlib program."
	puts stderr "(tried $path_octmvlib)"
	pftUsage "Setup failed."
    }

    if { $path_octls == "" } {
        set path_octls $env(PTOLEMY)/bin.$env(ARCH)/octls
    }
    if { ![file isfile $path_octls] || ![file exec $path_octls] } {
	puts stderr "Can't find octls program."
	puts stderr "(tried $path_octls)"
	pftUsage "Setup failed."
    }
}

puts stdout "Ptolemy Fix Tree (ptfixtree version 0.1)"
pftProcessArgs
pftMainLoop

destroy .
