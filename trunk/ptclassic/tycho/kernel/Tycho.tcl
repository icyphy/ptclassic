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

if [info exists tk_version] {
    wm withdraw .
    tk appname tycho
}

# Do we still need this? -cliffc
#if ![regexp Iwidgets [package name]] {
#    # Iwidgets is not present, we use the buttonbox from it.
#    # Load in the packages.  For example, on the mac, we need this.
#    package require Itcl
#    package require Itk
#    package require Iwidgets
#}

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

# Currently Tycho.tcl checks this variable to workaround a bug
if {$tcl_platform(platform) == "macintosh"} {
    set tyDebug 1
    set tyMacBug 1
}

# FIXME: this is for debugging only, and should go away
proc ::tycho::_announce { msg } {
    global tyDebug
    if [info exists tyDebug] {
	puts $msg
	flush stdout
	update
    }
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

::tycho::_announce "TYCHO=$TYCHO"

global ::ptolemyfeature

# pitclpresent is set to 1 if the ::pitcl commands are present,
# which indicates that we have access to the ptolemy kernel
# via ::pitcl
if {[uplevel #0 info namespace all pitcl] == "pitcl"} {
    set ptolemyfeature(pitcl) 1
    set ptolemyfeature(ptolemy) 1
} {
    set ptolemyfeature(pitcl) 0
}

# ptclpresent is set to 1 if the ptcl commands are present,
# which indicates that we have access to the ptolemy kernel
# via the older ptcl interface
if {[uplevel #0 info commands domains] !=  {}} {
    set ptolemyfeature(ptcl) 1
    set ptolemyfeature(ptolemy) 1
} {
    set ptolemyfeature(ptcl) 0
}

# octtoolspresent is set to 1 if the ptk commands are present,
# which indicates that we have access to the tcl/octtools commands.
if {[uplevel #0 info commands ptkOpenFacet] != {} && \
	[uplevel #0 info commands pvOpenWindow] != {}} {
    set ptolemyfeature(octtools) 1
    set ptolemyfeature(ptolemy) 1
} {
    set ptolemyfeature(octtools) 0
}

global ::tychokernel
set tychokernel [file join $tycho kernel]

global ::tycholib
set tycholib [file join $tycho lib]

global ::tychoeditors
set tychoeditors [file join $tycho editors]

global ::tychopt
set tychopt [file join $tycho typt]

global ::tychoslate
set tychoslate [file join $tycho editors slate]

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
    
    if { [info exists tk_version] && $tk_version >= 4.1 } {
	if [file isdirectory [file join $PTOLEMY tcltk itcl lib itcl tcl]] {
	    # Check the environment variables here, rather
	    # than just overriding them.
	    if ![info exists env(TCL_LIBRARY)] {
		set env(TCL_LIBRARY) [file join $PTOLEMY \
			tcltk itcl lib itcl tcl]
	    }
	    if ![info exists env(TK_LIBRARY)] {
		set env(TK_LIBRARY) [file join $PTOLEMY \
			tcltk itcl lib itcl tk]
	    }
	    set tk_library $env(TK_LIBRARY)
	    uplevel #0 {
		if [catch {
		    source [file join $PTOLEMY \
			    tcltk itcl lib itcl tcl init.tcl]
		    source [file join $PTOLEMY \
			    tcltk itcl lib itcl tk tk.tcl]
		    source [file join $PTOLEMY \
			    tcltk itcl lib itcl itcl itcl.tcl]
		    source [file join $PTOLEMY \
			    tcltk itcl lib itcl itk itk.tcl]
		    source [file join $PTOLEMY \
			    tcltk itcl lib itcl iwidgets iwidgets.tcl]
		} errMsg ] {
		    puts "Error sourcing Ptolemy tcltk files:\n\
			    $errMsg\n\n This sort of thing could happen if\
			    you are using a itkwish that is a later\n\
			    release than the itkwish in your\
			    Ptolemy installation.\n Or it could mean trouble\
			    with your Ptolemy tcltk installation.\n\
			    Continuing. . ."
		}
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
    set ::auto_path [linsert $auto_path 0 [file join $tychoeditors slate ] ]

    set ::auto_path [linsert $auto_path 0 [file join $tycholib tydoc ] ]
    set ::auto_path [linsert $auto_path 0 [file join $tychopt kernel ] ]
    set ::auto_path [linsert $auto_path 0 [file join $tychopt lib ] ]
    set ::auto_path [linsert $auto_path 0 [file join $tychopt editors ] ]
    set ::auto_path [linsert $auto_path 0 [file join $tychopt controls ] ]
}
::tycho::_announce "Set auto_path"

# auto-loading
# Set up the directories to be searched in order of priority.
#
global ::tychokernel
set ::auto_path [linsert $auto_path 0 $tychokernel ]

# Create the tycho namespace
namespace ::tycho

# Make the tycho namespace visible at the current scope
# Note that this greatly weakens namespace protection, but
# itcl makes it rather awkward without it.  In the body of
# a procedure for a class within namespace ::tycho, for example,
# the namespace ::tycho is not visible.  Apparently, only the local
# namespace of the class and the global namespace are visible.
# import add ::tycho
#
uplevel #0 {
    # Source the preference manager and the default preferences.
    # Might as well source needed files first
    source [file join $tychokernel Path.tcl]
    source [file join $tychokernel Object.itcl]
    source [file join $tychokernel Interchange.itcl]
    source [file join $tychokernel Model.itcl]
    source [file join $tychokernel WidgetPreferences.itcl]
    ::tycho::_announce "Handled preferences"

    # More files that we are going to need right away, so there
    # is no point in deferring them to auto-loading.
    source [file join $tychokernel Uninstantiable.itcl]
}
if [info exists tk_version] {
    uplevel #0 {
	::tycho::_announce "About to source FontManager.itcl"
	source [file join $tychokernel FontManager.itcl]
	source [file join $tychokernel ColorManager.itcl]
	source [file join $tychokernel CircularList.itcl]
	source [file join $tychokernel TopLevel.itcl]
	source [file join $tychokernel TWidget.itcl]
	source [file join $tychokernel View.itcl]
	source [file join $tychokernel Dialog.itcl]
	source [file join $tychokernel Message.itcl]
	source [file join $tychokernel ErrorMessage.itcl]
	source [file join $tychokernel File.itcl]

	source [file join $tychokernel Edit.itcl]
	source [file join $tychokernel HTML.itcl]
	source [file join $tychokernel HTMLMessage.itcl]
	source [file join $tychokernel WelcomeMessage.itcl]
	source [file join $tychokernel MenuSupport.itcl]
	source [file join $tychokernel PopupMenu.itcl]
	::tycho::_announce "Sourced File.itcl"
	# Load the library file
	source [file join $tychokernel Lib.tcl]
    }
}


if {![info exists tychoWelcomeWindow]} {
    # If tychoWelcomeWindow is 0, then no 'Mr. Tycho' window is opened
    set tychoWelcomeWindow 1
}
if {![info exists tychoConsoleWindow]} {
    # If tychoConsoleWindow is 0, then no console window is opened.
    set tychoConsoleWindow 1
}
set tychoOpenFiles 0


# Source ~/.Tycho/tychorc.tcl if it exists.
set tychostartfile [glob -nocomplain [file join $env(HOME) .Tycho tychorc.tcl]]
if {$tychostartfile != {} && \
	[file exists $tychostartfile] && \
	[file readable $tychostartfile]} {
    if [catch {uplevel #0 {source $tychostartfile}} msg] {
        ::tycho::warn "Failure sourcing $tychostartfile.\n$msg"
    }
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
::tycho::_announce "Processed argv"

# Retrieve binary and version info. If the two variables already exist,
# then we must be in a tysh binary we they are set by SetVersionInfo
# which is in TyConsole.cc. Otherwise we are being called from itkwish,
# or another script. 
if {![info exists TychoBinaryInfo]} {
    set TychoBinaryInfo itkwish
}
# Builder uses tycho_version.
# If you change ::tycho_version, then update $TYCHO/lib/mac/tycho-mac. 
global ::tycho_version
set tycho_version 0.2.1devel

if {![info exists TychoVersionInfo]} {
    if [info exists tk_version] {
	set TychoVersionInfo "Version $tycho_version,\
		running under Itcl ${itcl::version}, Tcl$tcl_patchLevel,\
		Tk$tk_version, \$TYCHO = $TYCHO"
    } else {
	set TychoVersionInfo "Version $tycho_version,\
		running under Itcl ${itcl::version}, Tcl$tcl_patchLevel,\
		\$TYCHO = $TYCHO"
    }
}
# To disable the welcome message, use the Misc preference
# under the help button.
#
if { [info exists tk_version] && $tychoWelcomeWindow \
    && [::tycho::preference get misc welcomeWindow] \
    && ! [::tycho::preference get misc slowNetwork] } {
    ::tycho::_announce "About to create a welcome message"
    # Don't source ButtonBox here, tycho foo.html will fail
    # uplevel #0 {
    #	source [file join $tychokernel ButtonBox.itcl]
    # }
    ::tycho::welcomeMessage $TychoBinaryInfo $TychoVersionInfo
    ::tycho::_announce "Done creating a welcome message"

}
if [info exists tk_version] {
    # Determine whether we exit when there are no more windows.
    ::tycho::TopLevel::exitWhenNoMoreWindows $tychoExitWhenNoMoreWindows

    # We could try sourcing Displayer.itcl here, but 'tycho' and 'tycho foo'
    # behave slightly differently, so it is not worthit.
    ::tycho::Displayer::normalExit $tychoShouldWeDoRegularExit
}

# FIXME: if the user starts with slowNetwork==1 and then sets it to 0
# then we don't adjust the insertOffTime.  A fix might be to have
# some sort of callback in the preference manager.
if [::tycho::preference get misc slowNetwork] {
    option add *insertOffTime 0
}

# FIXME: Workaround for bug on the Mac under itcl2.2
if [info exists tyMacBug] {
    ::tycho::_announce "Returning early out of Tycho.tcl because of a Mac bug"
    ::tycho::_announce " Note: This will always happen on the Mac if tyMacBug is set"
    return
}

# If there are no command-line arguments, and the -noconsole
# option was not given, open a console window
if {$tychoOpenFiles == 0} {
    if {[info exists tk_version] && $tychoConsoleWindow != 0} {
	::tycho::_announce "About to create a TclShell"
	uplevel #0 {
	    source [file join $tychokernel StatusBar.itcl]
	    source [file join $tychokernel MenuBar.itcl]
	    source [file join $tychokernel TclShell.itcl]
	    ::tycho::view TclShell \
		    -geometry +0+0 \
		    -master 1 \
		    -name   .mainConsole \
		    -stdout 1 \
		    -text "Welcome to Tycho\n"
	}
	# In theory, this could be a Build proc, but who wants to load it?
	if { ![file exists [file join $TYCHO kernel doc codeDoc File.html]] \
		|| ![file exists [file join $TYCHO doc tychoClasses.dag]] \
		|| ![file exists [file join $TYCHO lib idx tycho.idx]] \
		|| ![file exists [file join $TYCHO lib idx codeDoc.idx]] } {
	    ::tycho::BuilderDialog::checkFiles
	}
    }
}

if {$tclscripttosource!={} } {
    uplevel #0 {source $tclscripttosource}
}
::tycho::_announce "About to unset"
unset tclscripttosource sawDashE

unset tychoWelcomeWindow tychoConsoleWindow \
        tychoOpenFiles tychoExitWhenNoMoreWindows tychoShouldWeDoRegularExit

# tycho -tty uses this to provide a command loop
if ![info exists tk_version] {
    set _tychoTtyCommand ""
    while 1 {
	puts -nonewline "ty% "
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
