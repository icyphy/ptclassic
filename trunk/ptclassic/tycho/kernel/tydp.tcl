# Distributed Processing Interface
#
# @Author: Christopher Hylands
#
# @Version: $Id$
#
# @Copyright (c) 1998 The Regents of the University of California.
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
#                                        PT_COPYRIGHT_VERSION_2
#                                        COPYRIGHTENDKEY
#######################################################################

# The procs in this file implement a simple interface between two
# Tcl interpreters running as separate processes.
# We use this to communicate between Itcl2.2 and Tcl Blend.  We
# cannot run these in the same process, they require different versions
# of Tcl
#
# The procs in this file are based on the Tcl-DP commands.

#######################################################################
#### tydp_connect
# Tcl-DP usage: dp_connect tcp -host $host -port $port] 0]
#
proc tydp_connect {host port} {
}

#######################################################################
#### tydp_RDO
# Perform a remote procedure call without return value.
#
proc tydp_RDO {channel args} {
}

#######################################################################
#### tydp_RPC
# Perform a remote procedure call.
#
proc tydp_RPC {channel args} {
}


########################################################################
#### tydp_MakeRPCClient
#
proc tydp_MakeRPCClient {host port {checkCmd none}} {
    # puts "attempting to connect"
    set client [lindex [tydp_connect tcp -host $host -port $port] 0]
    # puts "connected -- waiting for reply"
    set return [gets $client]
    # puts stdout $return
    if {[lindex $return 1] == "refused:"} {
	close $client
	error $return
    }
    if {[string match "Server not responding*" $return]} {
        close $client
        error $return
    }
    tydp_admin register $client -check $checkCmd
    tydp_CleanupRPC $client
    # puts stdout "Created $client"
    return $client
}

proc tydp_MakeRPCServer {{port 0} {loginFunc none} {checkCmd none}
			{retPort 0}} {
    # puts "tydp_MakeRPCServer $port $loginFunc $checkCmd $retPort"
    set rv [tydp_connect tcp -server 1 -myport $port]
    # puts "rv = $rv"
    set server [lindex $rv 0]

    fileevent $server readable "tydp_AcceptRPCConnection $loginFunc $checkCmd $server"
    tydp_atexit appendUnique "close $server"
    tydp_atclose $server append "tydp_ShutdownServer $server"
    return $server
}

#######################################################################
#
# This creates two "callbacks" which will clean up RPC connections
# behind the user's back.  If the user calls "close $rpcchan", we
# "alias" this to "tydp_CloseRPC $rpcchan" which is defined below.
#
# Likewise, if the user tries to exit tclsh/wish, we close the
# RPC channel first (which will, in turn, call tydp_CloseRPC as above.
#

proc tydp_CleanupRPC {file} {
    tydp_atclose $file appendUnique "tydp_CloseRPC $file"
    tydp_atexit appendUnique "close $file"
}


proc tydp_CloseRPC {file} {
    tydp_RDO $file dp_CloseRPCFile
    tydp_admin delete $file
    tydp_atexit delete "close $file"
    # puts stdout "close $file"
}
