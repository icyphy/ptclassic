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
# The procs in this file are loosely based on the Tcl-DP commands.
#
# FIXME: This client/server setup is very insecure.  If the server is
# running on a machine, then anyone on the local machine can connect
# to the server and run arbitrary commands as the user that is running
# the server.


########################################################################
#### tydp_StartJava
# Setup a connect to a jtclsh 
#
proc tydp_StartJava {} {
    global tycho
    if {![info exists tycho]} {
	set tycho [file join / users ptdesign]
    }
    # FIXME: we can only have one Tcl Blend server running on a machine
    # because we hardwire the port number
    set port 19876
    # FIXME: this will probably only work under Unix right now.
    # FIXME: we need to somehow close this process
    # FIXME: we need to save the output
    set pid [exec jtclsh [file join $tycho kernel tydp.tcl] > /tmp/jtclsh.out &]
    exec sleep 5
    set socket [tydp_MakeRPCClient 19876]
    return [list $socket $pid]
}

########################################################################
#### tydp_MakeRCPClient
# Setup a client connection on a specific port
#
proc tydp_MakeRPCClient {port} {
    puts "attempting to connect"
    set clientSocket [socket localhost $port]
    puts "tydp_MakeRPCClient [_tydp_sockName $clientSocket]"
    fconfigure $clientSocket -blocking 
    #fileevent $clientSocket readable "_tydp_ReadOrClose $clientSocket"
    #fileevent $clientSocket writable "_tydp_Write $clientSocket"
    return $clientSocket
}

########################################################################
#### tydp_MakeRCPServer
# Set up a server on a specific port
#
proc tydp_MakeRPCServer {port} {
    set serverSocket [socket -server _tydp_serverCmd $port]
    puts "tydp_MakeRPCServer $port: serverSocket = $serverSocket"
    puts "tydp_MakeRPCServer: [_tydp_sockName $serverSocket]"
    # Need this for a tclsh
    vwait forever
}

########################################################################
#### tydp_RPC
# Run a remote command on channel.  Wait for the results
#
proc tydp_RPC {channel args} {
    eval [puts $channel $args]
    flush $channel
    fconfigure $channel -blocking 0
    # FIXME: this is a busy loop, and it sucks.
    set rl {}
    while {[llength $rl] != 2} {
	append rl [read $channel]
    }
    if {[lindex $rl 0]} {
	global errorInfo
	set errorInfo [lindex $rl 0]
	error [lindex $rl 1]
    } else {
	return [lindex $rl 1]
    }
}

########################################################################
# Procs internal to tydp are below here.  All internal procs start
# with _tydp.
# If you call these from methods other than the extern tydp methods,
# then rats will come over and chew your toes off in your sleep.  
########################################################################

########################################################################
#### _tydp_sockName
# Used for debugging.  Given a socket channel, return the address,
# hostname and port in human readable form
proc _tydp_sockName {socket} {
    set rv [fconfigure $socket -sockname]
    return "$socket: address: [lindex $rv 0] hostname: [lindex $rv 1] \
	    port: [lindex $rv 2]"
}

########################################################################
#### _tydp_serverCmd
# This command is called whenever the server gets an inital connection
# from a client.  For security reasons, we only allow connections from
# localhost.
#
proc _tydp_serverCmd {channel clientAddress clientPort} {
    if { "$clientAddress" != "127.0.0.1"} {
	puts stderr "_tydp_serverCmd: $clientAddress != 127.0.0.1"
	exit
    }

    puts "_tydp_serverCmd $channel $clientAddress $clientPort"
    fconfigure $channel -blocking 0
    fileevent $channel readable "_tydp_ReadOrClose $channel"
    #fileevent $channel writable "_tydp_Write $channel"
}

########################################################################
#### _tydp_ReadOrClose
# Event handler for reading from sockets.
# 
proc _tydp_ReadOrClose {fd} {
    puts "_tydp_ReadOrClose: $fd"
    set line [gets $fd]
    puts "$line"
    if [eof $fd] {
	puts "_tydp_ReadOrClose: eof $fd"
        if ![catch {close $fd} errMsg] {
	    puts "_tydp_ReadOrClose: close $fd failed: $errMsg "
	}
	return;
    }
    if [catch {eval $line} result] {
	global errorInfo
	puts $fd [list 1 $result $errorInfo]
	flush $fd
    } else {
	puts $fd [list 0 $result]
	flush $fd
    }
}

#
# If this file is sourced in a Tcl that has the java package loaded
# then start up the server and wait forever
if {[lsearch [package names ] java] == 1} {
    tydp_MakeRPCServer 19876
    vwait foreever
}

