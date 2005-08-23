# Package load file for the Tycho tycho.edit.textedit package
#
# @Author: John Reekie
#
# @Version: @(#)textedit.tcl	1.3 02/26/98
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
package require tycho.kernel.gui
package provide tycho.edit.textedit 2.0

set env(TEXTEDIT_LIBRARY) [file dirname [info script]]
if { [lsearch -exact $auto_path $env(TEXTEDIT_LIBRARY)] == -1 } {
    lappend auto_path $env(TEXTEDIT_LIBRARY)
}

### Stylesheets
::tycho::register stylesheet "editcolors" \
	[file join $env(TEXTEDIT_LIBRARY) editcolors.style] \
	[file join ~ .Tycho styles editcolors.style]

::tycho::register stylesheet "edithtml" \
	[file join $env(TEXTEDIT_LIBRARY) edithtml.style] \
	[file join ~ .Tycho styles edithtml.style]

::tycho::register stylesheet "makefile" \
	[file join $env(TEXTEDIT_LIBRARY) makefile.style] \
	[file join ~ .Tycho styles makefile.style]

### MODE MAPPINGS

############# text editing modes
::tycho::register extensions "c" .sched .c .y
# .js is JavaScript
::tycho::register extensions "c++" .C .cc .h .H .js
::tycho::register extensions "esterel" .strl
::tycho::register extensions "forest" .fst
::tycho::register extensions "itcl" .itcl .itk
::tycho::register extensions "java" .java

# .vc is for Microsoft Visual c++
::tycho::register extensions "makefile" .mk .template .vc
::tycho::register extensions "ptcl" .pt .ptcl
::tycho::register extensions "ptlang" .pl
::tycho::register extensions "sdl" .sdl
::tycho::register extensions "tcl" .tcl .tim .layout

########### filename modes	
::tycho::register filenames "makefile" Makefile makefile GNUmakefile


### MODES

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

