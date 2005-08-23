# Package load file for the tycho.kernel.gui package
#
# @Author: John Reekie
#
# @Version: @(#)gui.tcl	1.12 10/27/99
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


# Based on pp 344-346 of Harrison and McClellan's "Effective Tcl/Tk
# Programming" book

package require tycho.kernel.basic
package require tycho.kernel.model
package provide tycho.kernel.gui 2.0

global env auto_path
set env(GUI_LIBRARY) [file dirname [info script]]
if { [lsearch -exact $auto_path $env(GUI_LIBRARY)] == -1 } {
    lappend auto_path $env(GUI_LIBRARY)
}

# Some files are going to be needed anway, so source them now
uplevel #0 {
    source [file join $env(GUI_LIBRARY) FontManager.itcl]
    source [file join $env(GUI_LIBRARY) ColorManager.itcl]
    source [file join $env(GUI_LIBRARY) TopLevel.itcl]
    source [file join $env(GUI_LIBRARY) TWidget.itcl]
    source [file join $env(GUI_LIBRARY) View.itcl]

    # Note: ErrorMessage MUST be sourced at startup in
    # order to override the default error-handling procs
    source [file join $env(GUI_LIBRARY) ErrorMessage.itcl]
    source [file join $env(GUI_LIBRARY) File.itcl]
    
    source [file join $env(GUI_LIBRARY) MenuSupport.itcl]
    source [file join $env(GUI_LIBRARY) MenuBar.itcl]
}

### COMPATIBILITY

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

# Determine whether we exit when there are no more windows.
::tycho::TopLevel::exitWhenNoMoreWindows $tychoExitWhenNoMoreWindows

# We could try sourcing Displayer.itcl here, but 'tycho' and 'tycho foo'
# behave slightly differently, so it is not worthit.
::tycho::TopLevel::normalExit $tychoShouldWeDoRegularExit


##### This section should be in tycho.kernel.basic

### PROTOCOLS
::tycho::register protocol "file" \
	-class ::tycho::ResourceFile \
	-label "Local Disk File"

### USER PROFILE
::tycho::register profile [file join ~ .Tycho profile]

########### Miscellaneous and conditional modes

# If we open a shared object, try to load it as a Tycho task
if { "[info sharedlibext]" != "" } {
    # Ptolemy Classic compiled under NT does not support shared libs
    # so [info sharedlibext] will return the empty string, so
    # trying to view the Ptolemy copyright will try to load it as
    # a shared library. 
    ::tycho::register mode "sharedlibrary" \
	    -command {::tycho::controlpanel {%s}}
    ::tycho::register extensions "sharedlibrary" [info sharedlibext]
}

##### End

### STYLESHEETS
::tycho::register stylesheet "colors" \
	[file join {$TYCHO} kernel styles colors.style] \
	[file join ~ .Tycho styles colors.style]

::tycho::register stylesheet "console" \
	[file join {$TYCHO} kernel styles console.style] \
	[file join ~ .Tycho styles console.style]

::tycho::register stylesheet "fonts" \
	[file join {$TYCHO} kernel styles fonts.style] \
	[file join ~ .Tycho styles fonts.style]

::tycho::register stylesheet "interaction" \
	[file join {$TYCHO} kernel styles interaction.style] \
	[file join ~ .Tycho styles interaction.style]

::tycho::register stylesheet "java" \
	[file join {$TYCHO} kernel styles java.style] \
	[file join ~ .Tycho styles java.style]

::tycho::register stylesheet "layout" \
	[file join {$TYCHO} kernel styles layout.style] \
	[file join ~ .Tycho styles layout.style]

::tycho::register stylesheet "stacktrace" \
	[file join {$TYCHO} kernel styles stacktrace.style] \
	[file join ~ .Tycho styles stacktrace.style]

::tycho::register stylesheet "system" \
	[file join {$TYCHO} kernel styles system.style] \
	[file join ~ .Tycho styles system.style]

::tycho::register stylesheet "text" \
	[file join {$TYCHO} kernel styles text.style] \
	[file join ~ .Tycho styles text.style]


### CATEGORIES
::tycho::register category new "text" -label "Text Editors" -underline 0
::tycho::register category new "html" -label "HTML Viewers" -underline 5
::tycho::register category new "graphics" -label "Graphics Editors" -underline 0
::tycho::register category new "tool" -label "Tools" -underline 3

::tycho::register category open "text" -label "Open Text Editors"
::tycho::register category open "html" -label "Open HTML Viewers"
::tycho::register category open "graphics" -label "Open Graphics Editors"
::tycho::register category open "tool" -label "Open Tools"

### MODE MAPPINGS

############# text editing modes
if [::tycho::stylesheet get interaction viewHTMLSourceUponOpen] {
    ::tycho::register extensions "edithtml" .html .htm .htl
} else {
    ::tycho::register extensions "html" .html .htm .htl
}

::tycho::register extensions "image" .gif .ppm .pgm .xbm

########### graphical editing modes
::tycho::register extensions "indexbrowser" .idx

########### special viewing modes
# FIXME: .idoc should bring up "generic" class viewer.
# Language-specific cases are handled by the mode variable
# in the header string.
# ::tycho::register extensions "idoc" .idoc
# ::tycho::register extensions "itclclass" .itclclass
# ::tycho::register extensions "javaclass" .javaclass

########### tool modes
::tycho::register extensions "itclclasslist" .icl

########### contents modes	
::tycho::register contents "vemfacet" [file join schematic {contents;} ]


### MODES
############# text editors

# Plain text
# The plain text editor always appears first in the menu.
::tycho::register mode "text" \
	-command {::tycho::view Edit -file {%s}} \
	-viewclass ::tycho::Edit \
	-label {Plain Text Editor} \
	-category "text" \
	-underline 0

# Tycho index browser
::tycho::register mode "indexbrowser" \
	-command {::tycho::Dialog::new IndexBrowser \
	[::tycho::autoName .idx] -file {%s}}

########### tools (alphabetical)

# Color picker -- needs refinement
::tycho::register mode "colorpicker" \
	-command {::tycho::pickcolor} \
	-viewclass ::tycho::ColorPicker \
	-label {Color Picker}  \
	-category "tool" \
	-underline 0

# Font picker -- needs refinement
::tycho::register mode "fontpicker" \
	-command {::tycho::queryfont} \
	-viewclass ::tycho::FontDialog \
	-label {Font Picker}  \
	-category "tool" \
	-underline 0

# Monitor window for exec'ed processes -- will not run on the Macintosh
if {$tcl_platform(platform) != "macintosh"} {
    ::tycho::register mode "monitor" \
	    -command {::tycho::view Monitor -toolbar 1} \
	    -viewclass ::tycho::Monitor \
	    -label {Exec Window}  \
	    -category "tool" \
	    -underline 0
}

# Tcl shell
::tycho::register mode "tclshell" \
	-command {::tycho::view TclShell -file {%s}} \
	-viewclass ::tycho::TclShell \
	-label {Tcl Shell}  \
	-category "tool" \
	-underline 4

# Tycho Builder Tool
::tycho::register mode "builder" \
	-command {::tycho::view BuilderDialog -toolbar 1} \
	-label {Tycho Builder}  \
	-category "tool" \
	-underline 6


# HELP MENU ENTRIES

# About...
::tycho::register help about \
	-label "About Tycho" \
	-underline 0 \
	-command {::tycho::welcomeMessage $TychoBinaryInfo $TychoVersionInfo}

# Tycho home page
::tycho::register help homepage \
	-label "Tycho Home Page" \
	-underline 0 \
	-command {::tycho::openContext \
	[file join $TYCHO doc index.html] "html"}

# Help on this widget
::tycho::register help usersguide \
	-label "Guide to \[%Q info class\] Widget" \
	-underline 0 \
	-command "%Q help"

# Concept index
::tycho::register help conceptindex \
	-label "Concept Index..." \
	-underline 8 \
	-command {::tycho::openContext [file join \$TYCHO lib idx tycho.idx]}

# Code index
::tycho::register help codeindex \
	-label "Code Index..." -underline 0 \
	-command {set _nmBogosity [::tycho::autoName .index]; \
        ::tycho::IndexBrowser $_nmBogosity \
	-file [file join \$TYCHO lib idx codeDoc.idx] \
	-width 80; $_nmBogosity centerOnScreen}

# Open Itcl documentation
# FIXME: Make openItclHTMLPage a proc!!!
::tycho::register help itclmanpages \
	-label "Itcl Man Pages" \
	-underline 5 \
	-command "%Q ::tycho::File::openItclHTMLPage"

# The Preferences menu
::tycho::register help preferences \
	-label "Style Preferences..." \
	-underline 0 \
	-command { ::tycho::stylechooser }

### SHORTCUTS, BINDINGS
# Note that the shortcuts preference is read only once at startup,
# so changing it requires restarting Tycho for the changes to be visible.
set tycho_bindings [::tycho::Shortcuts::choose \
        [::tycho::stylesheet get interaction shortcuts]]

# Define a procedure to respond to changes in the shortcuts preference
# by posting a message that a restart is needed for the changes to take
# effect.
#
proc _tychoBindingsNotifyRestart {method args} {
    if { $method == "attrset" } {
        if { [lindex [lindex $args 0] end] == "shortcuts" } {
            array set attrs [lreplace $args 0 0]
            if [::info exists attrs(-value)] {
                ::tycho::inform \
                        "Tycho will use the $attrs(-value) keyboard \
                        shortcuts the next time you start it.\n\
                        To prevent this, do not save the changes permanently."
            }
        }
    }
}

set __s [::tycho::stylesheet access interaction]
$__s subscribe notify checkforbindings _tychoBindingsNotifyRestart
unset __s

if {$tycho_bindings == {unix}} {
    # To prevent handled characters from appearing in Entry windows,
    bind Entry <Control-x><o> { }
    bind Entry <Control-x><k> { }
    # Remove Control-X from the <<Cut>> event 
    # In Itcl2.2, we have virtual events, so Control-Key-x is bound to
    #<<Cut>> in tk.tcl.  This means that we cannot have things like C-x C-r
    # bound to reload.
    global tcl_version
    if { $tcl_version >= 8.0 || [namespaceEval ::itcl {set version}] > 2.1 } {
	::event delete <<Cut>> <Control-Key-x>
    }
}

### Global Class Bindings
#
# The following ensure that if an entry box or text widget has the
# focus, and the user types a character, that all that happens is that
# the character appears in the entry box (simply add a "break" to the
# standard Tk binding).
bind Entry <Key> "tkEntryInsert %W %A; break"
bind Entry <Control-h> "tkEntryBackspace %W; break"
bind Text <Key> "tkTextInsert %W %A; break"
bind Text <Return> {tkTextInsert %W \n; break}
# One exception to allow "all" to catch it (to post menus)
bind Text <Key-F10> { }

bind Entry <<ClearToStart>> {
    clipboard clear -displayof %W
    clipboard append -displayof %W [%W get]
    %W delete 0 insert
}

bind Entry <<ClearToEnd>> {
    clipboard clear -displayof %W
    clipboard append -displayof %W \
	    [string range [%W get] [%W index insert] [%W index end]]
    %W delete insert end
}

# There are a few bindings that we want to work everywhere
bind all <<Open>> {::tycho::File::openWin %W; break}

# The following totally silly emacs binding interferes with our
# search mechanism.
bind Entry <Control-t> {}

# A binding in the Text widget to transpose characters
# conflicts with using C-t for search, and is bogus anyway, so remove it.
bind text <Control-t> "break"

