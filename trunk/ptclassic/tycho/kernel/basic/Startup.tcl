# Tycho start-up procedures.
#
# @Author: John Reekie
#
# @Version: $Id$
#
# @Copyright (c)  The Regents of the University of California.
# All rights reserved.
# 
# Permission is hereby granted, without written agreement and without
# license or royalty fees, to use, copy, modify, and distribute this
# software and its documentation for any purpose, provided that the
# above copyright notice and the following two paragraphs appear in all
# copies of this software.
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
# 						PT_COPYRIGHT_VERSION_2
# 						COPYRIGHTENDKEY
##########################################################################

# FIXME: this is for debugging only, and should go away
proc ::tycho::_announce { msg } {
    global tyDebug
    if [info exists tyDebug] {
	puts $msg
	flush stdout
	update
    }
}

##########################################################################
# ::tycho::commandLineArguments
#
# This procedure converts command-line arguments into
# entries in the <b>tychofeatures</b> global array.
# It should be called before calling <b>::tycho::start</b>,
# if you have arguments accepted from a command line.
# Note that <b.::tycho::Start</b> will read configuration
# parameters from <b>tychofeatures</b>, but that arguments
# given to the procedure override and update <b>tychofeatures</b>.
# The reason that this procedure exists is to allow a more
# natural style of command line.
#
# The command-line arguments are as follows:
#
# <dl>
#
# <dt><b>-debug</b>
#
# <dd>Turn on debugging. This flag is simply used to decide
# whether certain debug messages are printed to the console
# or not.
#
# <dt><b>-noconsole</b>
#
# <dd>Do not create and display the Tycho command console.
# This option is useful it starting Tycho to run test suites.
# The <b>-nogui</b> option will force this option on.
#
# <dt><b>-nogui</b>
#
# <dd>Do not load any of the GUI-related packages. These packages
# can still be loaded later, provided that Tycho is running
# in itkwish. If Tycho is running in itclsh, this option
# will be forced on.
#
# <dt><b>-nowelcome</b>
#
# <dd>Do not display the welcome window on startup. The TychoHTML
# package will not be loaded if this option is on. This option
# will reduce startup time. The <b>-nogui</b> option will force
# this option on.
#
# <dt><b>-shell</b>
#
# <dd>Continue a simple command interpreter in the start-up shell.
# This option is useful when starting Tycho for debugging, and
# when running interactively with the <b>-nogui</b> flag set.
#
# </dl>
#
proc ::tycho::commandLineArguments {args} {

}

##########################################################################
# ::tycho::start
#
# Start the Tycho graphical user interface. This procedure
# accepts a number of arguments that specify the startup
# parameters. The arguments are also inserted into the
# global array <code>tychofeatures</code>, in case they
# are needed later on.
#
# This procedure is called from the tywish and tyshell
# executable scripts. It can also be called from within
# your own Tcl/Tk application to initialize or load
# certain portions of Tycho. The command-line arguments
# by default call "package require" on certain Tycho packages,
# but if you have a non-standard Tycho installation, you
# may prefer to set the <b>-nopackages</b> option and perform
# the "package require" in your own code.
#
proc ::tycho::start {args} {
    # Require other packages. For now, we just require all packages,
    # but later on we implement package dependencies
    #
    uplevel #0 {
	package require Ptolemy
	package require TychoKernel
	package require TychoTextEdit
	package require TychoVisEdit

	# Slate is sort of independent from the rest...
	package require Slate
	package require Graph
    }
    ::tycho::_announce "About to create a TclShell"
    uplevel #0 {
	::tycho::view TclShell \
		-geometry +0+0 \
		-master 1 \
		-name   .mainConsole \
		-stdout 1 \
		-text "Welcome to Tycho\n"
    }
}
