# Master initialization file for the Tycho tcl/tk Ptolemy interface.
# Sourcing this file will open one or more tycho editors, depending
# on the command-line arguments.  If no command-line arguments are given,
# then a console window is opened.
#
# @Authors: Edward A. Lee, Christopher Hylands
#
# @Version: $Id$
#
# @Copyright (c) 1995-%Q% The Regents of the University of California.
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

# We don't use the toplevel window called ".", so withdraw it.
wm withdraw .

if ![regexp Iwidgets [package name]] {
    # Iwidgets is not present, we use the buttonbox from it.
    # Load in the packages.  For example, on the mac, we need this.
    package require Itcl
    package require Itk
    package require Iwidgets
}

# Check for Itcl2.1 first, since Itcl2.0 does not have [file join . . .]
if {${itcl::version} < 2.1} {
    error "Tycho must be run under Itcl2.1 or later, as Itcl2.0 does
    not have the tcl7.5 'file join' command, which is used for portability.
    If you are running tycho standalone, then you need an Itcl2.1 itkwish
    binary, so you should check your path for a different itkwish binary.
    If you are running tycho with Ptolemy, then you need to install
    Itcl2.1 and recompile tysh."
    exit 3
}

########################################################################
# If the environment variable TYCHO is set, use that to determine
# where the tycho libraries are.  Otherwise, use PTOLEMY.  If neither
# is set, try to find a user named ptolemy.  If all this fails, issue
# an error message.
#
global ::env ::tk_library ::tk_version
global ::ptolemy ::PTOLEMY
global ::tycho ::TYCHO

if [info exist env(TYCHO)] {
    set tycho $env(TYCHO)
    set TYCHO $env(TYCHO)
}

if [info exist env(PTOLEMY)] {
    set ptolemy $env(PTOLEMY)
    set PTOLEMY $env(PTOLEMY)
    if {![info exists tycho]} {
	set tycho [file join $ptolemy tycho]
	set TYCHO [file join $ptolemy tycho]
    }
}

if {![info exists tycho]} {
    # Neither environment variable is set.
    # See whether there is a ~ptolemy or ~ptdesign user, in that order,
    # that has tycho installed.
    if [file exists [glob [file join ~ptolemy tycho]]] {
	set tycho [glob [file join ~ptolemy tycho]]
	set TYCHO $tycho
    } {
	if [file exists [glob [file join ~ptdesign tycho]]] {
	    set tycho [glob [file join ~ptdesign tycho]]
	    set TYCHO $tycho
	}
    }
}

if {![info exists tycho] || ![file exists $tycho]} {
    # All of the above failed, give up
    error {To run tycho, set your environment variable TYCHO to the tycho home}
}

# If the ptolemy variables are not yet set, set them relative to tycho.
if {![info exists ptolemy]} {
    set ptolemy [file join $tycho ..]
    set PTOLEMY $ptolemy
}

global ::tychokernel
set tychokernel [file join $tycho kernel]

global ::tycholib
set tycholib [file join $tycho lib]

global ::tychoslate
set tychoslate [file join $tycho slate]

global ::tychoeditors
set tychoeditors [file join $tycho editors]

global ::tychopt
set tychopt [file join $tycho typt]

# Check to see whether the usual exit mechanism (where we exit
# if there are no more windows) is enabled.
if {![info exists tychoExitWhenNoMoreWindows]} {
    set tychoExitWhenNoMoreWindows 1
}
# Check to see whether the usual exit mechanism (where we enable 
# C-x C-c and have the exit command in the File menu) is enabled.
if {![info exists tychoShouldWeDoRegularExit]} {
    set tychoShouldWeDoRegularExit 1
}

# Note that it is NOT normally acceptable to rely on the TCL_LIBRARY
# and TK_LIBRARY environment variables, because most other tcl
# apps rely on them.  We have to set them here.  However, if Tycho
# is distributed without Ptolemy, we don't know where the libraries
# are, so we have no choice but to rely on the previously set values
# of these variables.  In this case, we assume that we are running
# under itkwish, and therefore the initialization files have already
# been sourced.

if [file isdirectory [file join $PTOLEMY tcltk itcl lib]] {
    
    if {$tk_version >= 4.1 } {
	# Really, we should be checking the environment variables here, rather
	# than just overriding them.
	if [file isdirectory [file join $PTOLEMY tcltk itcl lib itcl tcl]] {
	    set env(TCL_LIBRARY) [file join $PTOLEMY tcltk itcl lib itcl tcl]
	    set env(TK_LIBRARY) [file join $PTOLEMY tcltk itcl lib itcl tk]
	    set tk_library $env(TK_LIBRARY)
	    uplevel #0 {
		source [file join $PTOLEMY tcltk itcl lib itcl tcl init.tcl]
		source [file join $PTOLEMY tcltk itcl lib itcl tk tk.tcl]
		source [file join $PTOLEMY tcltk itcl lib itcl itcl itcl.tcl]
		source [file join $PTOLEMY tcltk itcl lib itcl itk itk.tcl]
		source [file join $PTOLEMY tcltk itcl lib itcl \
			iwidgets iwidgets.tcl]
	    }
	}
    } else {
	if [file isdirectory [file join $PTOLEMY tcltk itcl lib tcl]] {
	    set env(TCL_LIBRARY) [file join $PTOLEMY tcltk itcl lib tcl]
	    set env(TK_LIBRARY) [file join $PTOLEMY tcltk itcl lib tk]
	    set tk_library $env(TK_LIBRARY)
	    uplevel #0 {
		source [file join $PTOLEMY tcltk itcl lib tcl init.tcl]
		source [file join $PTOLEMY tcltk itcl lib tk tk.tcl]
		source [file join $PTOLEMY tcltk itcl lib itcl init.itcl]
		source [file join $PTOLEMY tcltk itcl lib itk init.itk]
	    }
	}
    }
}
uplevel #0 {
    set ::auto_path [linsert $auto_path 0 [file join $tychoeditors textedit ] ]
    set ::auto_path [linsert $auto_path 0 [file join $tychoeditors visedit ] ]
    set ::auto_path [linsert $auto_path 0 [file join $tychoslate kernel ] ]
    set ::auto_path [linsert $auto_path 0 [file join $tychoslate pictures ] ]
    set ::auto_path [linsert $auto_path 0 [file join $tychoslate shapes ] ]
    set ::auto_path [linsert $auto_path 0 [file join $tychoslate interactors ] ]
    set ::auto_path [linsert $auto_path 0 [file join $tychoslate combinators ] ]
    set ::auto_path [linsert $auto_path 0 [file join $tycholib widgets ] ]
    set ::auto_path [linsert $auto_path 0 [file join $tycholib tydoc ] ]
    set ::auto_path [linsert $auto_path 0 [file join $tycholib util ] ]
    set ::auto_path [linsert $auto_path 0 [file join $tychopt kernel ] ]
    set ::auto_path [linsert $auto_path 0 [file join $tychopt lib ] ]
    set ::auto_path [linsert $auto_path 0 [file join $tychopt editors ] ]
    source [file join $tychokernel Lib.tcl]
}

if {![info exists tychoWelcomeWindow]} {
    # If tychoWelcomeWindow is 0, then no 'Mr. Tycho' window is opened
    set tychoWelcomeWindow 1
}
if {![info exists tychoConsoleWindow]} {
    # If tychoWelcomeWindow is 0, then no console window is opened.
    set tychoConsoleWindow 1
}
set tychoOpenFiles 0

# Source ~/.Tycho/tychorc.tcl if it exists.
set tychostartfile [glob -nocomplain [file join $env(HOME) .Tycho tychorc.tcl]]
if {$tychostartfile != {} && \
	[file exists $tychostartfile] && \
	[file readable $tychostartfile]} {
    uplevel #0 {source $tychostartfile}
}
unset tychostartfile

# If tycho was started with -e tclscript, then we open up all the files
# and then source tclscript
set sawDashE 0
set tclscripttosource {} 

# Check this in case we are being sourced within a interpreter where
# argv has been unset.
if {[info exists argv]} {
    foreach file $argv {
	if {$file == {-nowelcome}} {
	    set tychoWelcomeWindow 0
	} elseif {$file == {-noconsole}} {
	    set tychoConsoleWindow 0
	} elseif {$file == {-e}} {
	    set sawDashE 1
	} elseif {$sawDashE == 1} {
	    # Ok, we saw the -e argument, now eat up the tclscript to source
	    set sawDashE 0
	    set tclscripttosource $file
	} else {
	    set tychoOpenFiles 1
	    ::tycho::File::openContext $file
	}
    }
}
# Retrieve binary and version info. If the two variables already exist,
# then we must be in a tysh binary we they are set by SetVersionInfo
# which is in TyConsole.cc. Otherwise we are being called from itkwish,
# or another script. 
if {![info exists TychoBinaryInfo]} {
    set TychoBinaryInfo itkwish
}
if {![info exists TychoVersionInfo]} {
    set TychoVersionInfo "Version 0.1.1devel"
}
# To disable the welcome message, set the global variable
# tychoWelcomeMessage to zero before sourcing this file.
# This can be done in your .Tycho/tychorc.tcl file.
#
if {$tychoWelcomeWindow != 0} {
    ::tycho::welcomeMessage $TychoBinaryInfo $TychoVersionInfo
}
# Determine whether we exit when there are no more windows.
::tycho::TopLevel::exitWhenNoMoreWindows $tychoExitWhenNoMoreWindows
::tycho::Displayer::normalExit $tychoShouldWeDoRegularExit
# If there are no command-line arguments, and the -noconsole
# option was not given, open a console window
if {$tychoOpenFiles == 0} {
    if {$tychoConsoleWindow != 0} {
	uplevel #0 {::tycho::Displayer .mainConsole -geometry +0+0 -master 1; \
                ::tycho::TclShell .mainConsole.s \
                -stdout 1 -text "Welcome to Tycho\n"}
	wm deiconify .mainConsole
    }
}

if {$tclscripttosource!={} } {
    uplevel #0 {source $tclscripttosource}
}
unset tclscripttosource sawDashE

tk appname tycho

unset tychoWelcomeWindow tychoConsoleWindow \
        tychoOpenFiles tychoExitWhenNoMoreWindows tychoShouldWeDoRegularExit

