

#
# The technology root dir, if not specified, is handled by
# tapGetDefaultRoot(), which uses "$OCTTOOLS/tech"
#
proc ged_init_env { } {
    global env

    if ![info exist env(PTOLEMY)] {
	set env(PTOLEMY) ~ptolemy
    }
    if ![info exist env(OCTTOOLS)] {
	set env(OCTTOOLS) $env(PTOLEMY)
    }

#    topdbgmode on
#    topdbgmode on xp.ged
#    topdbgmode off xp.ged.draw
}

ged_init_env

#source $tk_library/tkAux/init.tcl
#source ./fileselect.tcl


source $env(PTOLEMY)/src/tkvem/tkAux/init.tcl
source $env(PTOLEMY)/src/tkvem/tkvem/fileselect.tcl

proc ged_bind_class {} {
    option add *XPGedWidget.background grey startupFile
    option add *XPGedWidget.scale 0.4 startupFile
    option add *XPGedWidget.labelFonts "*-helvetica-medium-r-normal-*-iso8859-*" startupFile

    tk_bindForTraversal XPGedWidget
    focus_bindForTraversal XPGedWidget
    bind XPGedWidget <1> {gedSelect %W %x %y}
    bind XPGedWidget <Motion> {gedMotion %W %x %y 0}
    bind XPGedWidget <2> {%W scan mark %x %y}
    bind XPGedWidget <B2-Motion> {%W scan dragto %x %y}
    bind XPGedWidget <Double-2> {%W scan contract 2 3}
    bind XPGedWidget <Shift-2> {%W scan mark %x %y}
    bind XPGedWidget <Shift-B2-Motion> {%W scan stretchto %x %y}

    bind XPGedWidget f {gedViewFull %W}
    bind XPGedWidget i {gedInside %W %x %y contents}
    bind XPGedWidget I {gedInside %W %x %y interface}
}

proc gedPrsParamPropStr { propStr } {
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

proc gedAddParamsToText { text params } {
    foreach param $params {
	$text insert end "[lindex $param 0]:\t[lindex $param 2]\n"
    }
}

proc gedMotion { wd px py cycleB } {
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
    switch [lindex $fmt 1] {
      FACET {
        $wmi.tw insert end "Cell:\t[file tail [lindex $fmt 4]]\n"
        $wmi.tw insert end "View:\t[lindex $fmt 5]\n"
        $wmi.tw insert end "Facet:\t[lindex $fmt 6]\n"
	if ![catch {toct_prop "[lindex $fmt 0]>property params"} paramProp] {
	    gedAddParamsToText $wmi.tw [gedPrsParamPropStr $paramProp]
	}
      }
      INSTANCE {
        $wmi.tw insert end "Name:\t[lindex $fmt 2]\n"
        $wmi.tw insert end "Master:\t[file tail [lindex $fmt 4]]\n"
	if ![catch {toct_prop "[lindex $fmt 0]>property params"} paramProp] {
	    gedAddParamsToText $wmi.tw [gedPrsParamPropStr $paramProp]
	}
      }
      PATH {
	if ![catch {toct_fmt "[lindex $fmt 0] < net"} netFmt] {
            $wmi.tw insert end "Net:\t[lindex $netFmt 2]\n"
	}
      }
    }
}

proc _gedGetInst { wd px py} {
    upvar #0 $wd wvars

    if {"[set facetId $wvars(facetId)]"=="" } { return "" }
    set octPt [$wd imap $px $py]
    if [catch {toct_get "$facetId>closest $octPt 0"} instId] {
	return ""
    }
    return $instId
}

proc gedInside {wd px py facetType} {
    upvar #0 $wd wvars
    set w $wvars(w)

    if {"[set instId [_gedGetInst $wd $px $py]]"=="" } return
    if [catch {toct_get "$instId > master ::$facetType"} masterId] {
puts stdout "get master failed: $masterId"
	return
    }
    ged_window .ged[gensym] -facet $masterId
}

proc gedSelect { wd px py } {
    gedMotion $wd $px $py 1
}

proc gedViewFull { wd } {
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


proc gedFacetChange { w } {
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
puts stdout "Opening $facetDesc as $facetId"

    wm title $w "GED $facetDesc"
    wm iconname $w "GED [file tail $facetCell]"

    if ![winfo ismapped $w.main.ged] {
        tkwait visibility $w.main.ged
    }
    gedViewFull $w.main.ged
}

proc gedNew { old_w } {
    set w [ged_window .ged[gensym]]
}

#
# Put up a file selector box to open a facet
#
proc gedOpen { w } {
    global $w
    set facetname ""
    catch {unset ${w}(openfacetname)}
    modalDialog transient fileselect $w.gedopen \
      -message "Open GED Facet" -new 0 -filter gedOpen.FileSelFilter \
      -textvariable ${w}(openfacetname)
    widget_waitVariable ${w}(openfacetname)
    set ofn [set ${w}(openfacetname)]
    modalDialog.end $w.gedopen

    if { "$ofn"=="" } { return }
    $w.main.ged conf -facet $ofn
    gedFacetChange $w
#puts stdout "gedopen $ofn"
}


proc toctIsFacet { facet } {
    return [expr [file isdir $facet] && [file isdir $facet/schematic] && \
      [file isfile $facet/schematic/contents\;] ]
}

proc gedOpen.FileSelFilter { filename } {
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

proc gedPtolRun {w runB} {
    set wd $w.main.ged
    upvar #0 $wd wvars

    if { "[set facetId $wvars(facetId)]"=="" } return
    set fmt [toct_fmt $facetId]
    set fullFacet [lindex $fmt 4]:[lindex $fmt 5]:contents
    set o2p ../oct2ptcl/sun4.opt/oct2ptcl

    if { $runB } {
        exec $o2p -r -g -t $fullFacet | ptcl >&@ stdout
    } else {
        exec $o2p -r -t $fullFacet >&@ stdout
    }
}

proc ged_window { w args} {
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
    $mb.file.m add com -lab "New" -und 0 -com "gedNew $w"
    $mb.file.m add com -lab "Open" -und 0 -com "gedOpen $w"
    $mb.file.m add com -lab "Close" -und 0 -com "destroy $w"
    $mb.file.m add com -lab "eXit" -und 1 -com "destroy ."

    pack [menubutton $mb.view -menu $mb.view.m -text "View" -und 0] -side left
    menu $mb.view.m
    $mb.view.m add com -lab "Full" -und 0 -com "gedViewFull $w.main.ged"

    pack [menubutton $mb.opts -menu $mb.opts.m -text "Options" -und 0] -side left
    menu $mb.opts.m
#    $mb.opts.m add com -lab "eXit" -und 1 -com "destroy ."

    pack [menubutton $mb.ptol -menu $mb.ptol.m -text "Ptolemy" -und 0] \
      -side left
    menu $mb.ptol.m
    $mb.ptol.m add com -lab "Compile" -und 0 -com "gedPtolRun $w 0"
    $mb.ptol.m add com -lab "Run" -und 0 -com "gedPtolRun $w 1"

    tk_menuBar $mb $mb.file $mb.opts

    pack [label $mb.loc -font fixed -width 10] -side right

    pack [frame $w.main] -side top -fill both -expand 1
    pack [eval gedwidget {$w.main.ged} $args] \
      -side left -fill both -expand 1

    pack [frame $w.main.info] -side left
    set wmi $w.main.info
    pack [label $wmi.type -width 20] -side top -anc n
    pack [text $wmi.tw -width 30 -font fixed -wrap none] -side top -anc n

    gedFacetChange $w
    focus_goTo $w.main.ged

    return $w
}

proc doit { } {
    global env
    wm withdraw .
    ged_window .g -facet $env(PTOLEMY)/init.pal:schematic:contents
#    ged_window .g -facet upSample:schematic:contents
#    gedOpen .g
}

ged_init_env
ged_bind_class
doit
