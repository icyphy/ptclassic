#
# tkpole startup script
#

puts stdout "Welcome to TKGraph..."

source [info library]/init.tcl
source $tk_library/tk.tcl

lappend auto_path ../lib

if [info exists geometry] {
    wm geometry . $geometry
}

#topdbgmode on xp.axis
#topdbgmode on xp.seqwdg
#topdbgmode on xp.rubber
#topdbgmode on xp.plane

proc tkerror { message } {
    global errorInfo
    puts stdout [format "TKError: %s" $message]
    puts stdout $errorInfo
}

proc toperror { message } {
    global errorInfo
    puts stdout [format "TOPError: %s" $message]
    puts stdout $errorInfo
}

xpdefvals.tcl
xpaxis.tcl
xpplanegraph.tcl
xpseqgraph.tcl

#----------------------------------------------------------------------------
#----------------------------------------------------------------------------

if 1 then {
    toplevel .seq -class XPSeqGraph
    xpSimpleSeqGraph .seq
    wm geometry .seq 300x300
    wm minsize .seq 0 0

    xpSSG.AddFakeData .seq
}


#----------------------------------------------------------------------------
#----------------------------------------------------------------------------

if 1 then {
    toplevel .plane -class XPPlaneGraph
    xpSimplePlaneGraph .plane
    wm geometry .plane 300x300
    wm minsize .plane 0 0

    .plane.draw silarinsert {2 4} 2
    .plane.draw silarinsert {3 3} -2
    .plane.draw silarinsert {4 .5} -1 conj
}
