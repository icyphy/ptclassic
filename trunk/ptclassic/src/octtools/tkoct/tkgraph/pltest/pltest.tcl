#
# tkpole startup script
#

puts stdout "Welcome to XPoleLoci..."

source [info library]/init.tcl
source $tk_library/tk.tcl

lappend auto_path . ../test

if [info exists geometry] {
    wm geometry . $geometry
}

# topdbgmode on xp.axis
topdbgmode on xp.seqwdg
topdbgmode on xp.rubber
topdbgmode on xp.plane

proc tkerror { message } {
    puts stdout [format "TKError: %s" $message]
}

proc toperror { message } {
    global errorInfo
    puts stdout [format "TOPError: %s" $message]
    puts stdout $errorInfo
}

xpdefvals.tcl
xpaxis.tcl
xpplanegraph.tcl
xplocigraph.tcl


#----------------------------------------------------------------------------
#----------------------------------------------------------------------------

if 1 then {
    toplevel .plane -class XPPlaneGraph
    xpLociPlaneGraph .plane
    wm geometry .plane 300x300
    wm minsize .plane 0 0
}
