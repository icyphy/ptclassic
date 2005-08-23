# Script to start up TclBlend under windows
#
# @Author: Christopher Hylands
#
# @Version: @(#)jtclsh.tcl	1.2 04/29/98
#
# @Copyright (c) 1997-1998 The Regents of the University of California.
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

#       Tycho refers to the directory where the Tycho distribution is with
#       the environment variable $TYCHO. In unix, under csh, you would do
#       something like setenv TYCHO /usr/local/tycho.
#       
#       You can either hardwire this variable by setting it in the start
#       up script, or under NT, you can set the TYCHO environment variable
#       by clicking on Start, then Settings, then Control Panel and
#       finally the System icon. In the System Properties window that
#       comes up, select Environment. There, you can add TYCHO as a User
#       Variable. Incidentally, this is where you set your path.

# Set the TYCHO variable
if ![info exists env(TYCHO)] {
    set env(TYCHO) "c:/tycho0.2.1devel"
}

if ![info exists env(JAVA_HOME)] {
    set env(JAVA_HOME) "c:/jdk1.1.4"
}
# If you are having problems, uncomment the next line
# set tyDebug 1


regsub {\\} "$env(TYCHO)" {/} tmp
set env(TYCHO) $tmp

regsub {\\} "$env(JAVA_HOME)" {/} tmp
set env(JAVA_HOME} $tmp

set TYCHO $env(TYCHO)
set PTOLEMY $TYCHO/..

set env(CLASSPATH) "$env(JAVA_HOME)/lib/classes.zip\;$TYCHO/java\;$TYCHO/java/tycho\;c:/Program Files/Tcl/lib/tclblend1.0/tclblend.zip"

package require java

if { $argc > 1 } {
    # The user called jwish or jtclsh with some args, so we source
    # the first one.
    source [lindex $argv 1]
} else {
    if [info exists tk_version] {
        set tcl_prompt1 "jwish% "
    } else {
        set tcl_prompt1 "jtclsh% "
    }

    # This is a really lame effort to implement a prompt.
    # We should be using tkCon or something.

    set _tychoTtyCommand ""
    while 1 {
        puts -nonewline $tcl_prompt1
        flush stdout
        set _tychoTtyCommand [gets stdin]
        while { ![info complete $_tychoTtyCommand] \
                || [regexp {\\$} $_tychoTtyCommand] } {
            if { [regexp {\\$} $_tychoTtyCommand] } {
                set _tychoTtyCommand \
                        [string range $_tychoTtyCommand 0 [expr \
                        [string length $_tychoTtyCommand] -2]]
            } else {
                append _tychoTtyCommand \n
            }
            puts -nonewline "> "
            flush stdout
            append _tychoTtyCommand [gets stdin]
        }
        if [catch {puts [eval $_tychoTtyCommand]} msg] {
            # puts stderr $msg
            puts stderr $errorInfo
        }
    }
}
