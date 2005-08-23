# Ptolemy proc that calls exec
#
# @Authors: Christopher Hylands and Kevin Chang
#
# @Version: @(#)Tyexec.tcl	1.1 05/12/98
#
# @Copyright (c) 1996- The Regents of the University of California.
# All rights reserved.
#
# Permission is hereby granted, without written agreement and without
# license or royalty fees, to use, copy, modify, and distribute this
# software and its documentation for any purpose, provided that the above
# copyright notice and the following two paragraphs appear in all copies
# o f this software.
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
#                                        PT_COPYRIGHT_VERSION_2
#                                        COPYRIGHTENDKEY
#######################################################################

##############################################################################
#### tyexec
# Invoke an external program, passing it the specified arguments.
# The format of the command is exactly that of the Tcl "exec" command.
# The main purpose of this procedure is to generate an informative
# error message if the "exec" fails.
#
proc ::tycho::tyexec { args } {
    global tcl_platform
    if {$tcl_platform(platform) == "macintosh"} {
	error "Sorry, the tcl \"exec\" command is not supported on the\
		Macintosh.\nThe command:\n$args\ncannot be executed"
    }
    # NOTE: Unix-ism.
    if [catch {eval exec $args} msg] {
        # The command failed.  It would be nice to check to see
        # whether the program exists, and generate a suitable message,
        # but it's not clear how to do this.
        error "The following command invocation failed:\n \
                $args\n \
                Is the program [lindex $args 0] installed and in your path?\n \
                $msg"
    }
}

