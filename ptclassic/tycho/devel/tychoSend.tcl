# Author: Jose Luis Pino
# Version: $Id$
#
# Copyright (c) 1990-%Q% The Regents of the University of California.
# 	All Rights Reserved.
#
# usage: tychoSend tychoCommand

set tclProgs [winfo interps]
if { [expr ![regexp {tycho} $tclProgs]] } {
    exec tycho &
    while { [expr ![regexp {tycho} $tclProgs]] } {
	set tclProgs [winfo interps]
    }
}
foreach prog $tclProgs {
    if {[regexp {tycho} $prog]} {
	break
    }
}
set sendCommand "send {$prog} $argv"
eval $sendCommand
exit
