#
# topmisc.tcl
#
# This is a collection of obscure misc function...they should prob. be
# better sorted out.
#

proc topmisc.tcl { } { }

proc topNull { args } {
}

proc topGenSN { } {
    global top
    if ![info exist top(sn)] {
	set top(sn) 0
    }
    return [incr top(sn)]
}

proc topEcho { args } {
    puts stdout "$args"
}

#
# This is somewhat tricky: we might be redefined ourselves.  Tcl
# doesnt handle this (it starts interpreting garbage).
# So we rename ourselves to a temp name.
# When done, if we have been redefined, we leave ourselves with the temp
# name to be deleted next time, otherwise
# we rename ourselves back to the original name.
#
proc topReloadAllScripts { } {
    set func topReloadAllScripts
#    auto_reset -- this is broken for some reason
    set mods [info procs *.tcl]
    foreach m $mods {
	rename $m $m-temp
    }
    catch { rename $func-temp "" }
    rename $func $func-temp
    foreach m $mods {
	puts stdout "INFO: Reload loading module $m"
    	if [catch $m error] {
    	    if { [info procs $m]=="" } {
		rename $m-temp $m
	        puts stdout "ERROR: Unable to reload module $m"
	    }
	}
        catch { rename $m-temp "" }
    }
    if { [info procs $func]=="" } {
        rename $func-temp $func
	return
    }
}

proc rescript { } topReloadAllScripts
