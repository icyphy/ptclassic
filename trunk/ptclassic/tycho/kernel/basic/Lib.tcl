# Master initialization file for the Tycho tcl/tk Ptolemy interface.
# This depends on a variable TYCHO having been set pointing to the
# Tycho installation directory.
#
# @Author: Edward A. Lee
#
# @Version: $Id$
#
# Copyright (c) 1995-1997 The Regents of the University of California.
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

########################################################################

### PROTOCOLS
::tycho::register protocol "file" \
	-class ::tycho::ResourceFile \
	-label "Local Disk File"

::tycho::register protocol "http" \
	-class ::tycho::ResourceHTTP \
	-label "Hy-Time Transport Protocol (HTTP)"

::tycho::register protocol "ftp" \
	-class ::tycho::ResourceFTP \
	-label "File Transfer Protocol (FTP)"

::tycho::register protocol "mailto" \
	-class ::tycho::ResourceMail \
	-label "Mail Protocol"

# FIXME: Make conditional upon presence of Java
::tycho::register protocol "weld" \
	-class ::tycho::ResourceWeld \
	-label "Java-Based Interface to WELD"


### USER PROFILE
::tycho::register profile [file join ~ .Tycho profile]

### Stylesheets
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


::tycho::register stylesheet "editcolors" \
	[file join {$TYCHO} editors textedit editcolors.style] \
	[file join ~ .Tycho styles editcolors.style]

::tycho::register stylesheet "edithtml" \
	[file join {$TYCHO} editors textedit edithtml.style] \
	[file join ~ .Tycho styles edithtml.style]

::tycho::register stylesheet "makefile" \
	[file join {$TYCHO} editors textedit makefile.style] \
	[file join ~ .Tycho styles makefile.style]

::tycho::register stylesheet "graphics" \
	[file join {$TYCHO} editors visedit graphics.style] \
	[file join ~ .Tycho styles graphics.style]


::tycho::register stylesheet "tydoc" \
	[file join {$TYCHO} lib tydoc tydoc.style] \
	[file join ~ .Tycho styles tydoc.style]


### SHORTCUTS, BINDINGS
# FIXME: Need a way to have this set when the preference changes.
set tycho_bindings [::tycho::Shortcuts::choose \
        [::tycho::stylesheet get interaction shortcuts]]

if {$tycho_bindings == {unix}} {
    # To prevent handled characters from appearing in Entry windows,
    bind Entry <Control-x><o> { }
    bind Entry <Control-x><k> { }
    # Remove Control-X from the <<Cut>> event 
    # In Itcl2.2, we have virtual events, so Control-Key-x is bound to
    #<<Cut>> in tk.tcl.  This means that we cannot have things like C-x C-r
    # bound to reload.
    if {[namespace ::itcl {set version}] > 2.1 } {
	::event delete <<Cut>> <Control-Key-x>
    }
}

### Global Class Bindings
#
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
bind Entry <<Open>> {::tycho::File::openWin %W; break}

# The following totally silly emacs binding interferes with our
# search mechanism.
bind Entry <Control-t> {}

# A binding in the Text widget to transpose characters
# conflicts with using C-t for search, and is bogus anyway, so remove it.
bind text <Control-t> "break"

### CATEGORIES
::tycho::register category new "text" -label "Text Editors" -underline 0
::tycho::register category new "html" -label "HTML Viewers" -underline 5
::tycho::register category new "graphics" -label "Graphics Editors" -underline 0
::tycho::register category new "tool" -label "Tools" -underline 3
::tycho::register category new "ptolemy" -label "Ptolemy Tools" -underline 6

::tycho::register category open "text" -label "Open Text Editors"
::tycho::register category open "html" -label "Open HTML Viewers"
::tycho::register category open "graphics" -label "Open Graphics Editors"
::tycho::register category open "tool" -label "Open Tools"
::tycho::register category open "ptolemy" -label "Open Ptolemy Tools"

### MODE MAPPINGS

############# text editing modes
::tycho::register extensions "c" .sched .c .y
::tycho::register extensions "c++" .C .cc .h .H
::tycho::register extensions "esterel" .strl
::tycho::register extensions "forest" .fst
::tycho::register extensions "html" .html .htm .htl
::tycho::register extensions "image" .gif .ppm .pgm .xbm
::tycho::register extensions "itcl" .itcl .itk
::tycho::register extensions "java" .java

# .vc is for Microsoft Visual c++
::tycho::register extensions "makefile" .mk .template .vc
::tycho::register extensions "ptcl" .pt .ptcl
::tycho::register extensions "ptlang" .pl
::tycho::register extensions "sdl" .sdl
::tycho::register extensions "tcl" .tcl .tim .layout

########### graphical editing modes
::tycho::register extensions "dag" .dag
::tycho::register extensions "dfg" .dfg
::tycho::register extensions "std" .std
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

########### filename modes	
::tycho::register filenames "makefile" Makefile makefile GNUmakefile

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

# C
::tycho::register mode "c" \
	-command {::tycho::view EditC -file {%s}} \
	-viewclass ::tycho::EditC \
	-label {C Editor}  \
	-category "text" \
	-underline 0

# C++
::tycho::register mode "c++" \
	-command {::tycho::view EditCpp -file {%s}} \
	-viewclass ::tycho::EditCpp \
	-label {C++ Editor}  \
	-category "text" \
	-underline 1

# Esterel
::tycho::register mode "esterel" \
	-command {::tycho::view EditEsterel -file {%s}} \
	-viewclass ::tycho::EditEsterel \
	-label {Esterel Editor}  \
	-category "text" \
	-underline 0

# HTML editor
::tycho::register mode "edithtml" \
	-command {::tycho::view EditHTML -file {%s}} \
	-viewclass ::tycho::EditHTML \
	-label {HTML Editor}  \
	-category "text" \
	-underline 0

# Images
::tycho::register mode "image" \
	-command {::tycho::view HTML -file {%s} -image 1 -toolbar 1} \
	-category "html" \
	-underline 0

# HTML viewer
::tycho::register mode "html" \
	-command {::tycho::view HTML -file {%s} -toolbar 1} \
	-viewclass ::tycho::HTML \
	-label {HTML Viewer}  \
	-category "html" \
	-underline 5

# Itcl
::tycho::register mode "itcl" \
	-command {::tycho::view EditItcl -file {%s}} \
	-viewclass ::tycho::EditItcl \
	-label {Itcl Editor}  \
	-category "text" \
	-underline 0

# Java
::tycho::register mode "java" \
	-command {::tycho::view EditJava -file {%s}} \
	-viewclass ::tycho::EditJava \
	-label {Java Editor}  \
	-category "text" \
	-underline 0

# Makefiles and Microsoft Visual C++
::tycho::register mode "makefile" \
	-command {::tycho::view EditMake -file {%s}} \
	-viewclass ::tycho::EditMake \
	-label {Makefile Editor}  \
	-category "text" \
	-underline 0

# SDL -- System description language
::tycho::register mode "sdl" \
	-command {::tycho::view EditSDL -file {%s}} \
	-viewclass ::tycho::EditSDL \
	-label {SDL Editor} \
	-category "text" \
	-underline 0

# Tcl
::tycho::register mode "tcl"  \
	-command {::tycho::view EditTcl -file {%s}} \
	-viewclass ::tycho::EditTcl \
	-label {Tcl Editor}  \
	-category "text" \
	-underline 0

########### graphical editors (alphabetical)

# Directed-acyclic graph viewer
::tycho::register mode "dag" \
	-command {::tycho::view EditDAG -file {%s}} \
	-viewclass ::tycho::EditDAG \
	-label {DAG Editor}  \
	-category "graphics" \
	-underline 0

# NOTE: Not useful on its own.
# ::tycho::register mode {.fsm} \
	#       {::tycho::view EditFSM -file {%s}} \
	#       {Finite state machine editor}

# Tree viewer
::tycho::register mode "forest" \
	-command {::tycho::view EditForest -file {%s}} \
	-viewclass ::tycho::EditForest \
	-label {Forest Editor}  \
	-category "graphics" \
	-underline 0

# Prototype dataflow graph editor
#::tycho::register mode "dfg" \
#	-command {::tycho::editgraph {%s}} \
#	-viewclass ::tycho::EditGraph \
#	-label {Graph Editor}  \
#	-category "graphics" \
#	-underline 0

# State-transition diagram editor
::tycho::register mode "std" \
	-command {::tycho::view EditSTD -file {%s}} \
	-viewclass ::tycho::EditSTD \
	-label {State Transition Diagram Editor}  \
	-category "graphics" \
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

# "Diff" viewer -- will not run on the Macintosh
if {$tcl_platform(platform) != "macintosh"} {
    ::tycho::register mode "diffviewer" \
	    -command {::tycho::view EditDiff -toolbar 1} \
	    -viewclass ::tycho::EditDiff \
	    -label {Diff Viewer}  \
	    -category "tool" \
	    -underline 0
}

# Directory Search
::tycho::register mode "dirsearch" \
        -command {set w [::tycho::autoName .dirsearch]; \
        ::tycho::DirSearch $w -geometry +0+0;\
	    wm deiconify $w} \
	    -label {Directory Search}  \
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

# Glimpse interface -- will not run on the Macintosh
if {$tcl_platform(platform) != "macintosh"} {
    ::tycho::register mode "glimpse" \
	    -command {set w [::tycho::autoName .glimpse]; \
	    ::tycho::Glimpse $w -geometry +0+0;\
	    wm deiconify $w} \
	    -label {Glimpse}  \
	    -category "tool" \
	    -underline 0
}

# Itcl class list and class diagram generator
::tycho::register mode "itclclasslist" \
	-command {::tycho::view ItclClassList -file {%s} -toolbar 1} \
	-viewclass ::tycho::ItclClassList \
	-label {Itcl Class List}  \
	-category "tool" \
	-underline 5

# Tcl profiling tool
::tycho::register mode "profile" \
	-command {::tycho::view ProfileTcl -file {%s} -toolbar 1} \
	-label {Tcl Profiler}  \
	-category "tool" \
	-underline 4

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


########### Compound viewers (alphabetical)

# Cliff's IDoc viewer (still under construction)
#::tycho::register mode "idoc" \
#	-command {::tycho::view IDoc -file {%s}} \
#	-viewclass ::tycho::IDoc \
#	-label {IDoc Viewer}  \
#	-category "html" \
#	-underline 0

::tycho::register mode "itclclass" \
	-command {::tycho::view ItclViewer -file {%s}} \
	-viewclass ::tycho::ItclViewer \
	-label {Itcl Class Viewer}  \
	-category "html" \
	-underline 0

::tycho::register mode "javaclass" \
	-command {::tycho::view JavaViewer -file {%s}} \
	-viewclass ::tycho::JavaViewer \
	-label {Java Class Viewer}  \
	-category "html" \
	-underline 0

########### Miscellaneous and conditional modes

# If we open a shared object, try to load it as a Tycho task
::tycho::register mode "sharedlibrary" \
	-command {::tycho::controlpanel {%s}}
::tycho::register extensions "sharedlibrary" [info sharedlibext]

# Matlab console
if {[uplevel #0 info commands matlab] != {}} {
    ::tycho::register mode "matlab" \
	    -command {::tycho::view Matlab -file {%s}} \
	    -viewclass ::tycho::Matlab \
	    -label {Matlab Console}  \
	    -category "tool"
}

# Mathematica console
if {[uplevel #0 info commands mathematica] != {}} {
    ::tycho::register mode "mathematica" \
	    -command {::tycho::view Mathematica -file {%s}} \
	    -viewclass ::tycho::Mathematica \
	    -label {Mathematica Console}  \
	    -category "tool"
}

########### Ptolemy bits

# Ptcl -- Ptolemy's Tcl interface language
::tycho::register mode "ptcl" \
	-command {::tycho::view EditPtcl -file {%s}} \
	-viewclass ::tycho::EditPtcl \
	-label {Ptcl Editor}  \
	-category "ptolemy" \
	-underline 1

# Ptlang -- Ptolemy's star definition language
::tycho::register mode "ptlang" \
	-command {::tycho::view EditPtlang -file {%s}} \
	-viewclass ::tycho::EditPtlang \
	-label {Ptlang Editor}  \
	-category "ptolemy" \
	-underline 2

# The VEM facet mode behaves differently if we are inside Ptolemy
if $ptolemyfeature(octtools) {
    # Ptolemy and vem are present.  Use them.
    ::tycho::register mode "vemfacet" \
	    -command {::pvOpenWindow \
	    [::ptkOpenFacet {%s} schematic contents]} \
	    -label {Vem Facet} \
	    -category "ptolemy"
} else {
    # Vem is not present.
    ::tycho::register mode "vemfacet" \
	    -command {::tycho::view EditPalette -facet {%s}} \
	    -viewclass ::tycho::EditPalette \
	    -label {Palette Editor} \
	    -category "ptolemy" \
	    -underline 0
}
# Retargetting editor
if { $ptolemyfeature(ptolemy)} {
    ::tycho::register mode "retarget" \
	    -command {::tycho::view Retarget -file {%s} -toolbar 1} \
	    -viewclass ::tycho::Retarget \
	    -label {Ptolemy Retargeter}  \
	    -category "ptolemy"
}


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

# Tycho class diagram
::tycho::register help classdiagram \
	-label "Class Diagram" \
	-underline 6 \
	-command {::tycho::openContext \
	[file join \$TYCHO doc tychoClasses.dag]}

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

# Ptolemy-only help entries
if $ptolemyfeature(ptolemy) {
    # About Ptolemy
    ::tycho::register help ptabout \
	    -label "About Ptolemy" \
	    -underline 0 \
	    -command ptkStartupMessage \
	    -before usersguide

    # Ptolemy home page
    ::tycho::register help pthomepage \
	    -label "Ptolemy Home Page" \
	    -underline 0 \
	    -command {::tycho::openContext \
	    [file join $PTOLEMY doc html index.html] "html"} \
	    -before usersguide
}

