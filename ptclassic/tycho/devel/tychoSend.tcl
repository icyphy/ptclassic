# Author: Jose Luis Pino
# Version: $Id$
#
# Copyright (c) 1990-%Q% The Regents of the University of California.
# 	All Rights Reserved.
#
# usage: tychoSend tychoCommand

tk appname tySend

set tclProgs [winfo interps]
if { [expr ![regexp {tycho} $tclProgs]] } {
    exec tycho -pigi &
    while { [expr ![regexp {tycho} $tclProgs]] } {
	set tclProgs [winfo interps]
    }
}

foreach prog $tclProgs {
    if {[regexp {tycho} $prog]} {
	break
    }
}

send $prog source $argv
exit
