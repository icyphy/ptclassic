# Script to start up TclBlend under windows.  From tclsh80, or wish80, type:
#      source $env(TYCHO)/lib/windows/tclblend.tcl
#
# @Author: Christopher Hylands
#
# @Version: @(#)tclblend.tcl	1.2 04/29/98
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
