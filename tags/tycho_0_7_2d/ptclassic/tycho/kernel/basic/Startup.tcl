# Tycho start-up procedures. These procedures are not
# contained in any Tycho package, since the arguments
# to them control which packages are loaded into Tycho.
#
# @Author: John Reekie
#
# @Version: @(#)Startup.tcl	1.9 08/05/98
#
# @Copyright (c) 1998 The Regents of the University of California.
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
#### ::tycho::processCommandLine
#
# <B>Preliminary</b> -- do not use.
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
# <dt><b>-autoquit</b>
#
# <dd>Quit Tycho when there are no more windows open. This
# is useful for test suites and the like. It is ignored if
# <b>-nogui</b> is on.
#
# <dt><b>-debug</b>
#
# <dd>Turn on debugging. This flag is simply used to decide
# whether certain debug messages are printed to the console
# or not.
#
# <dt><b>-e</b> _file_
#
# <dd>Evaluate the given file. See <b>-source</b>,
#
# <dt><b>-files</b> _file_ _file_
#
# <dd>Open the given files. More than one file can
# be given. In general, it is recommended that the
# file names simply be placed after all other options,
# without the <b>-files</b> flag: any arguments following
# the last valid command-line option  are taken to be files
# to be opened.  The last option is signaled either by an
# argument that does not begin with a dash, or by a 
# double dash (this is needed if their are file names that
# being with a dash). File opening will be disabled if the
# <b>-nogui</b> option is set.
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
# <dt><b>-nopackages</b>
#
# <dd>Do not load any packages into Tycho at this time.
# This is useful in certain situations where you want
# to override or replace the default Tycho packages.
# If this option is set but other options will cause
# Tycho to do things (for example, create a console
# window), then you need to make sure that you have
# loaded those packages _before_ calling <b>::tycho::start</b>.
#
# <dt><b>-notychorc</b>
#
# <dd>Do not attempt to locate and source a tychorc.tcl
# file in the user's home directory. Otherwise, the user
# file will be sourced after initialization but before
# executing any scripts or sourcing any files. The user
# should make sure that the <b>tychorc.tcl</b> file checks
# <b>tychofeatures(-nogui)</b> before executing any code that
# requires a graphical interface.
#
# <dt><b>-nowelcome</b>
#
# <dd>Do not display the welcome window on startup. The TychoHTML
# package will not be loaded if this option is on. This option
# will reduce startup time. The <b>-nogui</b> option will force
# this option on.
#
# <dt><b>-packages</b> _package_ _package_
#
# <dd>Load the given packages. These packages will be loaded
# at the end of the initialization process, but before creating
# any GUI elements.
#
# <dt><b>-script</b> _script_
#
# <dd>Execute the given script. The script will execute after
# the rest of Tycho initialization, abd after the file given
# by <b>-source</b> is sourced, but before any files are
# opened.
#
# <dt><b>-shell</b>
#
# <dd>Continue a simple command interpreter in the start-up shell.
# This option is useful when starting Tycho for debugging, and
# when running interactively with the <b>-nogui</b> flag set.
#
# <dt><b>-source</b> _file_
#
# <dd>Evaluate the given file. The file will be sourced
# after the rest of initialization completes, but before evaluation
# of the <b>-script</b> option (if there is one).
#
# <dt><b>-quit</b>
#
# <dd>Exit Tycho after executing the Tcl code given by <b>-script</b>
# and <b>-source</b>. This option will be ignored if neither of
# the above are provided.
#
# </dl>
#
proc ::tycho::processCommandLine {args} {
    global tychofeatures

    # Initialize tychofeatures
    set tychofeatures(-autoquit) 0
    set tychofeatures(-debug) 0
    set tychofeatures(-files) {}
    set tychofeatures(-noconsole) 0
    set tychofeatures(-nogui) 0
    set tychofeatures(-nopackages) 0
    set tychofeatures(-notychorc) 0
    set tychofeatures(-nowelcome) 0
    set tychofeatures(-packages) {}
    set tychofeatures(-script) {}
    set tychofeatures(-shell) 0
    set tychofeatures(-source) {}
    set tychofeatures(-quit) 1

    # Process command-line args
    while { $args != "" } {
	set option [lindex $args 0]
	set args [lreplace $args 0 0]

	if ![string match {-*} $option] {
	    # Got files
	    eval lappend tychofeatures(-files) [list $option] $args
	    break
	} elseif { $option == "--" } {
	    # Got flag signalling end of options
	    eval lappend tychofeatures(-files) $args
	    break
	}
	switch -exact -- $option {
	    "-autoquit" {
		set tychofeatures(-autoquit) 1
	    }
	    "-debug" {
		set tychofeatures(-debug) 1
	    }
	    "-files" {
		# FIXME
		while { ![string match {-*} $option] } {
		    lappend tychofeatures(-files) $option
		    set option [lindex $args 0]
		    set args [lreplace $args 0 0]
		}
	    }
	    "-noconsole" {
		set tychofeatures(-noconsole) 1
	    }
	    "-nogui" {
		set tychofeatures(-nogui) 1
	    }
	    "-nopackages" {
		set tychofeatures(-nopackages) 1
	    }
	    "-notychorc" {
		set tychofeatures(-notychorc) 1
	    }
	    "-nowelcome" {
		set tychofeatures(-nowelcome) 1
	    }
	    "-packages" {
		while { ![string match {-*} $option] } {
		    # FIXME
		    lappend tychofeatures(-packages) $option
		    set option [lindex $args 0]
		    set args [lreplace $args 0 0]
		}
	    }
	    "-script" {
		set tychofeatures(-script) [lindex $args 0]
		set args [lreplace $args 0 0]
	    }
	    "-shell" {
		set tychofeatures(-shell) 1
	    }
	    "-source" -
	    "-e" {
		set tychofeatures(-source) [lindex $args 0]
		set args [lreplace $args 0 0]
	    }
	    "-quit" {
		set tychofeatures(-quit) 1
	    }
	    default {
		puts "Unknown option: $option"
	    }
	}
    }
    # Force -nogui on if running in itclsh
    if { [file tail [info nameofexecutable]] == "itclsh" } {
	set tychofeatures(-nogui) 1
    }
}

##########################################################################
##### ::tycho::start
#
# <B>Preliminary</b> -- do not use.
#
# Start the Tycho graphical user interface. This procedure
# accepts a number of arguments that specify the startup
# parameters, in the The arguments are also inserted into the
# global array <code>tychofeatures</code>, in case they
# are needed later on. Each element of <code>tychofeatures</code>
# is a value that corresponds to the presence of arguments
# documented in <b>::tycho::processCommandLine</b>
# command-line argument with the same name. For example,
# if the command-line argument <b>-noconsole</b> is
# present, then <b>tychofeatures(-noconsole)</b> is
# set to one, otherwise it is set to zero.
#
# This procedure is called from the <b>tywish</b> and
# <b>tyshell</b> executable scripts. It can also be called from
# within your own Tcl/Tk application to initialize or load
# certain portions of Tycho. The command-line arguments
# cause a call to "package require" on certain Tycho packages,
# but if you have a non-standard Tycho installation, you
# may prefer to set the <b>-nopackages</b> option and perform
# the "package require" in your own code.
#
proc ::tycho::start {args} {
    global tychofeatures

    # Update the tychofeatures array first
    if { [llength $args] & 1 } {
	error "The ::tycho::start procedure requires \
		an option-value list: $args"
    } else {
	array set tychofeatures $args
    }

    # The main division here is between the non-graphical and
    # graphical versions. It's easier to farm the startup
    # of these two versions to separate procs.
    if $tychofeatures(-nogui) {
	::tycho::startKernel
    } else {
	::tycho::startGui
    }

    # Now evaluate the user script, if there is one
    if { $tychofeatures(-script) != "" } {
	uplevel #0 $tychofeatures(-script)
    }

    # Source specified files in order
    # FIXME: How do we get absolute paths???
    foreach file $tychofeatures(-files) {
	uplevel #0 $file
    }

    # If a shell was requested, start it
    if $tychofeatures(-shell) {
	::tycho::runShell
    }

    # If auto-quit was set, exit now
    # FIXME: should this call ::tclexit if the gui has
    # been loaded???
    if $tychofeatures(-autoquit) {
	::exit
    }
}

##########################################################################
##### ::tycho::loadTychorc
#
# <B>Preliminary</b> -- do not use.
#
# Load the user's tychorc file, if there is one and <b>-notychorc</b>
# was not specified. The file is located in either
# <code>~/.Tycho/tychorc.tcl</code> or
# <code>~/.tycho/tychorc.tcl</code>.
#
proc ::tycho::loadTychorc {} {
    global tychofeatures
    global env

    # Find the file
    if !$tychofeatures(-notychorc) {
	set tychorc [glob -nocomplain \
		[file join $env(HOME) .tycho tychorc.tcl]]

	if { $tychorc == {} || ! [file readable $tychorc] } {
	    set tychorc [glob -nocomplain \
		    [file join $env(HOME) .Tycho tychorc.tcl]]
	}
	# Evaluate it
	if { $tychorc != {} && [file readable $tychorc] } {
	    if [catch {uplevel #0 {source $tychostartfile}} msg] {
		if $tychofeatures(-nogui) {
		    puts "Failure sourcing $tychorc:\n$msg"
		} else {
		    ::tycho::warn "Failure sourcing $tychorc.\n$msg"
		}
	    }
	}
    }
}


##########################################################################
##### ::tycho::runShell
#
# <B>Preliminary</b> -- do not use.
#
# Run a simple interactive command shell in the startup window.
#
#
proc ::tycho::runShell {} {
    global errorInfo

    # Make stdin non-blocking and stdout non-buffered
    fconfigure stdin -blocking 0
    fconfigure stdout -buffering none

    # Create a new proc called tclputs if there isn't one.
    # This is necessary because the GUI has intercepted
    # the "puts" command, and renamed the standard tcl 
    # command to tclputs
    if { [infoWhichCommand tclputs] == "" } {
	uplevel #0 {
	    proc tclputs {args} {
		eval ::puts $args
	    }
	}
    }

    # Run the simple command interpreter
    set command ""
    while 1 {
	# Top-level prompt
	tclputs -nonewline "tycho% "

	# Get non-blocked input string. If null, try again
	set command [gets stdin]
	while { $command == "" } {
	    update
	    set command [gets stdin]
	}
	# Check if the command is complete, and not terminated by
	# a back-slash. Otherwise, output the continuation
	# prompt (">"), and get another input line
	while { ![info complete $command] \
		|| [regexp {\\$} $command] } {
	    if { [regexp {\\$} $command] } {
		set command \
			[string range $command 0 [expr \
			[string length $command] -2]]
	    } else {
		append command \n
	    }
	    tclputs -nonewline "> "

	    set commandNext [gets stdin]
	    while { $commandNext == "" } {
		update
		set commandNext [gets stdin]
	    }
	    append command $commandNext
	}
	# Now we have a complete command. Execute it, and produce
	# the stack trace if it raised an error.
	if ![catch {eval $command} result] {
	    tclputs $result
	} else {
	    # tclputs $msg
	    tclputs stderr $errorInfo
	}
    }
}

##########################################################################
##### ::tycho::startKernel
#
# <B>Preliminary</b> -- do not use.
#
# Start the Tycho non-graphical kernel. This procedure
# looks at the parameters <b>tychofeatures</b> to decide
# what to do.
#
proc ::tycho::startKernel {args} {
    global tychofeatures

}

##########################################################################
##### ::tycho::startGui
#
# <B>Preliminary</b> -- do not use.
#
# Start the Tycho graphical kernel. This procedure
# looks at the parameters <b>tychofeatures</b> to decide
# what to do.
#
proc ::tycho::startGui {args} {
    global tychofeatures

    # We don't use the toplevel window called ".", so withdraw it.
    wm withdraw .
    tk appname tycho

    # Load the standard set of packages needed to run the GUI,
    # if they are installed. If they aren't installed, ignore
    # the inessential ones. If -nopackages is set, don't load any.
    if !$tychofeatures(-nopackages) {
	package require tycho.kernel.gui
	
	# FIXME: this is too many
	foreach pkg {
	    tycho.kernel.html
	    tycho.edit.textedit
	    tycho.edit.visedit
	    tycho.edit.graphedit
	    tycho.util.devtools
	} {
	    if { [lsearch -exact [package names] $pkg] != -1 } {
		uplevel #0 package require $pkg
	    }
	}
    }

    # Now load any packages explicitly asked for by the caller
    foreach pkg $tychofeatures(-packages) {
	if { [lsearch -exact [package names] $pkg] != -1 } {
	    uplevel #0 package require $pkg
	}
    }
    
    # If the welcome window is asked for, load it
    if !$tychofeatures(-nowelcome) {
	# ::tycho::welcomeMessage $TychoBinaryInfo $TychoVersionInfo
    }
    # package require Graph

    # Before creating any windows, load the user's tychorc.tcl
    # file, if there is one
    ::tycho::loadTychorc

    # For now, create a shell
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
