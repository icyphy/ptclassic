# Server side of Remote Procedure Calls between itkwish and Tcl Blend
#
# @Author: Christopher Hylands
#
# @Version: @(#)JRPCServer.tcl	1.9 05/08/98
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

# The procs in this file implement the server side of a simple interface
# between two Tcl interpreters running as separate processes.
# We use this to communicate between Itcl2.2 and Tcl Blend.  We
# cannot run these in the same process, they require different versions
# of Tcl.
#
# See JRPC.itcl for the client side.
#
# FIXME: This client/server setup is very insecure.  If the server is
# running on a machine, then anyone on the local machine can connect
# to the server and run arbitrary commands as the user that is running
# the server.


puts !!!$argv

# Ugly nasty global variable for the command read from the socket
# so far
set _JRPC_command_ ""

########################################################################
#### JRPCServer_CreateServer
# Set up a server on a specific port
# This proc never returns.
#
proc JRPCServer_CreateServer {port} {
    set serverSocket [socket -server _JRPCServer_RegisterClient $port]
    # vwait turns on the event loop in Tcl Shell
    vwait forever
}


########################################################################
# Procs internal to JRPCServer are below here.  All internal procs start
# with _JRPCServer.
########################################################################

########################################################################
#### _JRPCServer_RegisterClient
# This proc is called whenever the server gets an inital connection
# from a client.  For security reasons, we only allow connections from
# localhost.
#
proc _JRPCServer_RegisterClient {channel clientAddress clientPort} {
    if { "$clientAddress" != "127.0.0.1"} {
	puts stderr "_JRPCServer_RegisterClient: Security Error! \n\
		Someone is trying to connect from a machine other than the \
		local machine.\n\
		$clientAddress != 127.0.0.1"
		exit
    }
    fconfigure $channel -blocking 0
    fileevent $channel readable "_JRPCServer_ReadOrClose $channel"
}

########################################################################
#### _JRPCServer_ReadOrClose
# Event handler for reading from sockets.
# 
proc _JRPCServer_ReadOrClose {fd} {
    global _JRPC_command_

    set line [gets $fd]
    # puts !$line
    # puts "_JRPCServer_ReadOrClose: $line"

    # Check for cookie and execute command if one was got
    if { $line == "_JRPC_EndCommand_Cookie_" } {
	global debug
	#if $debug {
	    puts "JRPCServer: \"[string trimright $_JRPC_command_ \n]\""
	#}
        if [catch {uplevel #0 $_JRPC_command_} result] {
            puts $fd [list 1 $result]
            flush $fd
        } else {
            puts $fd [list 0 $result]
            flush $fd
        }
        set _JRPC_command_ ""

    } else {
        append _JRPC_command_ $line \n
    }
    
    # Exit if done
    if [eof $fd] {
        if ![catch {close $fd} errMsg] {
	    puts "_JRPCServer_ReadOrClose: close $fd failed: $errMsg "
	}
	return;
    }
}

#
# If this file is sourced in a Tcl that has the java package loaded
# then start up the server and wait forever
#
if {[lsearch [package names] java] != -1} {
    if { [llength $argv] >= 2 } {
        set portnum [lindex $argv 2]
    } else {
        set portnum 19876
    }
    if { [llength $argv] >= 3 } {
	set debug [lindex $argv 3]
    }
    JRPCServer_CreateServer $portnum
}

