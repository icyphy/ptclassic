# Version: $Id$
#
#---------------------------------------------------------------------------
# Copyright (c) 1994 The Regents of the University of California.
# All rights reserved.
#
# Permission is hereby granted, without written agreement and without
# license or royalty fees, to use, copy, modify, and distribute this
# software and its documentation for any purpose, provided that the above
# copyright notice and the following two paragraphs appear in all copies
# of this software.
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
# The technology root dir, if not specified, is handled by
# tapGetDefaultRoot(), which uses "$OCTTOOLS/tech"
#

# tkoct_init_env is similar to pigi_init_env
proc tkoct_init_env { } {
    global env tcl_prompt1 tcl_prompt2 tk_library tk_version
    global ptolemy PTOLEMY

    set ptolemy $env(PTOLEMY)
    set PTOLEMY $env(PTOLEMY)

    if ![info exist env(PTOLEMY)] {
	set env(PTOLEMY) [glob ~ptolemy]
    }
    if ![info exist env(OCTTOOLS)] {
	set env(OCTTOOLS) $env(PTOLEMY)
    }
    if ![info exist env(PT_DISPLAY)] {
	set env(PT_DISPLAY) "xedit -name ptolemy_code %s"
    }
    if ![info exist env(PTPWD)] {
        # FIXME: I'm not sure of the portability of this
	# $PTOLEMY/lib/dialog.tcl said:
#D 1.19  94/02/07 10:21:26 cxh   19 18   00001/00001/00321
#PWD is not set on the mips, so I added a new var, PTPWD to pigiEnv.csh
#and pigiRpcShell.  If you use PWD, be sure that it works on the mips.
#
#
#D 1.18  94/01/23 20:28:24 cxh   18 17   00002/00001/00320
#Changed [pwd] back to $env(PWD).  Apparently Ptolemy on suns will
#consistently start up in the home directory.
#
#
#D 1.17  94/01/23 20:14:44 cxh   17 16   00002/00002/00319
#Don't refer to the environment PWD var, it does not exist under
#ultrix.

	set env(PTPWD) [pwd]
    }
#    topdbgmode on
#    topdbgmode on xp.ged
#    topdbgmode off xp.ged.draw
    set env(TCL_LIBRARY) $env(PTOLEMY)/tcltk/tcl/lib/tcl
    set env(TK_LIBRARY) $env(PTOLEMY)/tcltk/tk/lib/tk
    set tk_library $env(TK_LIBRARY)
    #uplevel #0 source $env(PTOLEMY)/tcl/tcl[info tclversion]/lib/init.tcl
    #uplevel #0 source $env(PTOLEMY)/tcl/tk$tk_version/lib/tk.tcl
    uplevel #0 source $env(PTOLEMY)/tcltk/tcl/lib/tcl/init.tcl
    uplevel #0 source $env(PTOLEMY)/tcltk/tk/lib/tk/tk.tcl

    set tcl_prompt1 "puts -nonewline stdout {tkoct> }"
    set tcl_prompt2 "puts -nonewline stdout {tkoct? }"

}

tkoct_init_env

#source $tk_library/tkAux/init.tcl
#source ./fileselect.tcl


#source $env(PTOLEMY)/src/tkvem/tkAux/init.tcl
#source $env(PTOLEMY)/src/tkvem/tkvem/fileselect.tcl

# Load the help system for ptcl
source $ptolemy/lib/tcl/ptcl_help.tcl

# A procedure to return the rgb value of all color names used in Pigi.
source $ptolemy/lib/tcl/ptkColor.tcl

# Set the Defaults for fonts, colors, etc. for the Pigi Windows.
source $ptolemy/lib/tcl/ptkOptions.tcl

# Other Pigi procedures
#source $ptolemy/lib/tcl/message.tcl
source $ptolemy/lib/tcl/utilities.tcl
source $ptolemy/lib/tcl/dialog.tcl
source $ptolemy/lib/tcl/ptkBind.tcl
#source $ptolemy/lib/tcl/ptkControl.tcl
#source $ptolemy/lib/tcl/ptkRunAllDemos.tcl
source $ptolemy/lib/tcl/ptkParams.tcl
#source $ptolemy/lib/tcl/ptkBarGraph.tcl
#source $ptolemy/lib/tcl/ptkPrfacet.tcl


proc tkoct_bind_class {} {
    option add *XPGedWidget.background grey startupFile
    option add *XPGedWidget.scale 0.4 startupFile
    option add *XPGedWidget.labelFonts "*-helvetica-medium-r-normal-*-iso8859-*" startupFile

    tk_bindForTraversal XPGedWidget
    # From tkAux
    #focus_bindForTraversal XPGedWidget
    bind XPGedWidget <1> {tkoctSelect %W %x %y}
    bind XPGedWidget <Motion> {tkoctMotion %W %x %y 0}
    bind XPGedWidget <2> {%W scan mark %x %y}
    bind XPGedWidget <B2-Motion> {%W scan dragto %x %y}
    bind XPGedWidget <Double-2> {%W scan contract 2 3}
    bind XPGedWidget <Shift-2> {%W scan mark %x %y}
    bind XPGedWidget <Shift-B2-Motion> {%W scan stretchto %x %y}

    bind XPGedWidget f {tkoctViewFull %W}
    # Strip off the .main.ged part
    bind XPGedWidget F {tkoctOpen [file rootname [file rootname %W]] }
    bind XPGedWidget i {tkoctInside %W %x %y contents}
    bind XPGedWidget I {tkoctInside %W %x %y interface}

}

proc tkoctPrsParamPropStr { propStr } {
    set propList ""
    set props [split $propStr "\1"]
    if { [llength $props]==1 } {
	# try old style
	set props [split $propStr "|"]
        set props [lreplace $props 0 0]
	while { [llength $props]>0 } {
	    set paramEl [lrange $props 0 1]
	    set props [lreplace $props 0 1]
	    if { [llength $paramEl]!=2 } break
	    lappend propList paramEl \
	      [list [lindex $paramEl 0] FLOAT [lindex $paramEl 1]]
	}
    } else {
	# First discard the numParams element
	set props [lreplace $props 0 0]
	while { [llength $props]>0 } {
	    set paramEl [lrange $props 0 2]
	    set props [lreplace $props 0 2]
	    if { [llength $paramEl]!=3 } break
	    lappend propList $paramEl
	}
    }
    return $propList
}

proc tkoctAddParamsToText { text params } {
    foreach param $params {
	$text insert end "[lindex $param 0]:\t[lindex $param 2]\n"
    }
}

proc tkoctMotion { wd px py cycleB } {
    upvar #0 $wd wvars
    set w $wvars(w)

    if {"[set facetId $wvars(facetId)]"=="" } {
	return
    }
    set octPt [$wd imap $px $py]
    $w.menubar.loc conf -text $octPt

    set oldSel [expr { $cycleB ? $wvars(selId) : "" }]
    set octScale [lindex [$wd imap -scale 10 10] 0]
    if [catch {toct_fmt "$facetId>closest $octPt $octScale $oldSel"} fmt] {
        puts stdout "$wd: selerr $fmt"
	return
    }
    if { "[lindex $fmt 0]"=="$wvars(selId)" } {
	return
    }
    set wvars(selId) [lindex $fmt 0]
    set wmi $wvars(w).main.info
    $wmi.type conf -text "[lindex $fmt 1]"
    $wmi.tw delete 1.0 end

    #foreach item $fmt { $wmi.tw insert end "$item\n"}

    switch [lindex $fmt 1] {
      FACET {
        $wmi.tw insert end "Cell:\t[file tail [lindex $fmt 4]]\n"
        $wmi.tw insert end "View:\t[lindex $fmt 5]\n"
        $wmi.tw insert end "Facet:\t[lindex $fmt 6]\n"
	if ![catch {toct_prop "[lindex $fmt 0]>property params"} paramProp] {
	    tkoctAddParamsToText $wmi.tw [tkoctPrsParamPropStr $paramProp]
	}
      }
      INSTANCE {
        $wmi.tw insert end "Name:\t[lindex $fmt 2]\n"
        $wmi.tw insert end "Master:\t[file tail [lindex $fmt 4]]\n"
	if ![catch {toct_prop "[lindex $fmt 0]>property params"} paramProp] {
	    tkoctAddParamsToText $wmi.tw [tkoctPrsParamPropStr $paramProp]
	}
      }
      PATH {
	if ![catch {toct_fmt "[lindex $fmt 0] < net"} netFmt] {
            $wmi.tw insert end "Net:\t[lindex $netFmt 2]\n"
	}
      }
    }
}

proc _tkoctGetInst { wd px py} {
    upvar #0 $wd wvars

    if {"[set facetId $wvars(facetId)]"=="" } { return "" }
    set octPt [$wd imap $px $py]
    if [catch {toct_get "$facetId>closest $octPt 0"} instId] {
	return ""
    }
    return $instId
}

proc tkoctInside {wd px py facetType} {
    upvar #0 $wd wvars
    set w $wvars(w)


    if {"[set instId [_tkoctGetInst $wd $px $py]]"=="" } return
#puts "tkoctInside: wd=$wd px=$px py=$py facetType=$facetType, instId=$instId"
    if [catch {toct_get "$instId > master ::$facetType"} masterId] {
      puts stdout "get master failed: $masterId"
	return
    }
#puts "tkoctInside: masterId=$masterId"
    if [ catch {tkoct_window .ged[gensym] -facet $masterId}] {
      tkoctInspectFile $masterId
    }
}

# Try to view masterid as a file
# FIXME: we assume that if a facets is domain/icons/foo, then
# the source is at domain/src/DOMAINfoo.pl
proc tkoctInspectFile {facet} {
  global env
  global PTOLEMY
  set facetFmt [toct_fmt "facet $facet"]
  set facetCell [lindex $facetFmt 4]
  set domainDir [file dirname [file dirname $facetCell]]
  
  set fileName "$domainDir/stars/[string toupper [file tail $domainDir]][file tail $facetCell].pl"
  puts "tkoctInspectFile $facet"
  set command [format $env(PT_DISPLAY) $fileName]
  eval exec $command &
}

proc tkoctSelect { wd px py } {
    tkoctMotion $wd $px $py 1
}

proc tkoctViewFull { wd } {
    upvar #0 $wd wvars
    if {"[set facetId $wvars(facetId)]"=="" } {
	return
    }
    set bbox [toct_bbox "$facetId"]
    if { [lindex $bbox 0]==[lindex $bbox 2] } {
	puts stdout "$wd: Can't view full because facet is empty."
	return
    }
    set wd_x [winfo width $wd]
    set wd_y [winfo height $wd]
    if { $wd_x < 20 || $wd_y < 20 } {
	puts stdout "$wd: Window is too small to view full."
	return
    }

    set scx [expr ($wd_x-10)/double([lindex $bbox 2]-[lindex $bbox 0])]
    set scy [expr ($wd_y-10)/double([lindex $bbox 3]-[lindex $bbox 1])]
    $wd conf -scale [expr $scx < $scy ? $scx : $scy]
    $wd center [expr ([lindex $bbox 0]+[lindex $bbox 2])/2] \
      [expr ([lindex $bbox 1]+[lindex $bbox 3])/2]
}


proc tkoctFacetChange { w } {
    set wd $w.main.ged
    upvar #0 $wd wvars
    set wvars(selId) ""
    set facet [lindex [$w.main.ged conf -facet] 4]
    if { "$facet"=="" } {
	set facet "NoName"; set facetId ""
	set facetCell $facet
	set facetDesc $facet
    } else {
	set facetFmt [toct_fmt "facet $facet"]
	set facetId [lindex $facetFmt 0]
	set facetCell [lindex $facetFmt 4]
	set facetDesc $facetCell:[lindex $facetFmt 5]:[lindex $facetFmt 6]
    }
    set wvars(facetId) $facetId
    #puts stdout "Opening $facet $facetDesc as $facetId"

    wm title $w "TKOCT $facetDesc"
    wm iconname $w "TKOCT [file tail $facetCell]"

    if ![winfo ismapped $w.main.ged] {
        tkwait visibility $w.main.ged
    }
    if [catch {tkoctViewFull $w.main.ged}] {
      return -code error
    }
}

proc tkoctNew { old_w } {
    set w [tkoct_window .ged[gensym]]
}

#
# Put up a file selector box to open a facet
#
proc tkoctOpen { w } {
    global $w
    # FIXME: We try to reuse the pigi file dialog box, but we need
    # to override ptkOpenFacet and pvOpenWindow and we need to pass
    # the facet name as a global.
    global openfacetName

    set facetname ""
    catch {unset ${w}(openfacetname)}
    #modalDialog transient fileselect $w.tkoctOpen \
    #  -message "Open TKOCT Facet" -new 0 -filter tkoctOpen.FileSelFilter \
    #  -textvariable ${w}(openfacetname)
    dialog.openFacet 

    #widget_waitVariable ${w}(openfacetname)
    #set ofn [set ${w}(openfacetname)]
    #modalDialog.end $w.tkoctOpen
    tkwait variable openfacetName
    set ofn $openfacetName

    puts stdout "tkoctOpen $w $ofn"
    if { "$ofn"=="" } { return }
    $w.main.ged conf -facet $ofn
    tkoctFacetChange $w

}
# FIXME: pigilib/POct.cc defines ptkOpenFacet too!, but we can't use it
# ptkOpenFacet <file_name_of_cell> [view] [facet]
# If no view is specified, the default "schematic" is used.
# If no facet is specified, the default "contents" is used.

#
proc ptkOpenFacet { file {view "schematic"} {facet "contents"} } {
  global openfacetName
  set openfacetName $file
}
# FIXME: pigilib/PVem.cc defines pvOpenWindow too!, but we can't use it
# pvOpenWindow <OctInstanceHandle>
proc pvOpenWindow { handle } {

}


proc toctIsFacet { facet } {
    return [expr [file isdir $facet] && [file isdir $facet/schematic] && \
      [file isfile $facet/schematic/contents\;] ]
}

proc tkoctOpen.FileSelFilter { filename } {
    if [regsub -all {\$([^/.-]+)} $filename {$env(\1)} osfilename] {
	global env
	if [catch "eval concat $osfilename" osfilename] {
		error "Error in environment variable substituition: $filename"
	}
        regsub {.*//} $osfilename {/} osfilename
    }
    if ![file isdirectory $osfilename] {
	error "File ``$filename'' is not a directory nor facet."
    }
    if [toctIsFacet $osfilename] {
	return [list file $filename [file tail $filename] $osfilename]
    }
    return [list directory $filename [file tail $filename]/ $osfilename]
}

proc tkoctPtolRun {w runB} {
    set wd $w.main.ged
    upvar #0 $wd wvars

    if { "[set facetId $wvars(facetId)]"=="" } return
    set fmt [toct_fmt $facetId]
    set fullFacet [lindex $fmt 4]:[lindex $fmt 5]:contents
    #set o2p ../oct2ptcl/sun4.opt/oct2ptcl
    set o2p oct2ptcl

    #puts "runB= $runB, exec $o2p -r -g -t $fullFacet | ptcl"
    if { $runB } {
        exec $o2p -r -g -t $fullFacet | ptcl >&@ stdout
    } else {
        exec $o2p -r -t $fullFacet >&@ stdout
    }
}

proc tkoct_window { w args} {
    upvar #0 $w.main.ged wvars

    set wvars(w)	$w
    set wvars(selId)	""

    toplevel $w
    wm geom $w 600x400
    wm minsize $w 10 10
    # wm title is set below
    set mb $w.menubar; pack [frame $mb] -side top -fill x
    pack [menubutton $mb.file -menu $mb.file.m -text "File" -und 0] -side left
    menu $mb.file.m
    $mb.file.m add com -lab "New" -und 0 -com "tkoctNew $w"
    $mb.file.m add com -lab "Open" -und 0 -com "tkoctOpen $w"
    $mb.file.m add com -lab "Close" -und 0 -com "destroy $w"
    $mb.file.m add com -lab "eXit" -und 1 -com "destroy ."

    pack [menubutton $mb.view -menu $mb.view.m -text "View" -und 0] -side left
    menu $mb.view.m
    $mb.view.m add com -lab "Full" -und 0 -com "tkoctViewFull $w.main.ged"

    pack [menubutton $mb.opts -menu $mb.opts.m -text "Options" -und 0] -side left
    menu $mb.opts.m
#    $mb.opts.m add com -lab "eXit" -und 1 -com "destroy ."

    pack [menubutton $mb.ptol -menu $mb.ptol.m -text "Ptolemy" -und 0] \
      -side left
    menu $mb.ptol.m
    $mb.ptol.m add com -lab "Compile" -und 0 -com "tkoctPtolRun $w 0"
    $mb.ptol.m add com -lab "Run" -und 0 -com "tkoctPtolRun $w 1"

    tk_menuBar $mb $mb.file $mb.opts

    pack [label $mb.loc -font fixed -width 10] -side right

    pack [frame $w.main] -side top -fill both -expand 1
    pack [eval gedwidget {$w.main.ged} $args] \
      -side left -fill both -expand 1

    pack [frame $w.main.info] -side left
    set wmi $w.main.info
    pack [label $wmi.type -width 20] -side top -anc n
    pack [text $wmi.tw -width 30 -font fixed -wrap none] -side top -anc n

    if [catch {tkoctFacetChange $w}] {
      destroy $w
      return -code error
    } else {
      # From tkAux, we need to fix this
      #focus_goTo $w.main.ged
      bind $w.main.ged <Enter> {focus %W} 
      focus $w.main.ged
    }
    return $w
}

proc tkoct_startup { } {
    global env
    wm withdraw .
    if { [file exists {init.pal/schematic/contents;} ] } {
      tkoct_window .g -facet init.pal:schematic:contents
    } else {
      # FIXME: if init.pal does not exist, we should create one
      puts "init.pal/schematic/contents; does not exist, opening \$PTOLEMY/init.pal instead"
      tkoct_window .g -facet $env(PTOLEMY)/init.pal:schematic:contents
    }
#    tkoct_window .g -facet upSample:schematic:contents
#    tkoctOpen .g
}


# gensym
# return a unique string like mktemp
# Uses the private global _gensym_count
proc gensym {} {
  global _gensym_count
  if {[ info exists _gensym_count]} {
    incr _gensym_count
  } else {
    set _gensym_count 0
  }
  return [format "%05d" $_gensym_count]
}

tkoct_bind_class
tkoct_startup
