# Master initialization file for the Tycho tcl/tk Ptolemy interface.
# This depends on a variable TYCHO having been set pointing to the
# Tycho installation directory.
#
# @Author: Edward A. Lee
#
# @Version: $Id$
#
# Copyright (c) 1995-%Q% The Regents of the University of California.
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

# Files that we are going to need right away, so there is no
# point in deferring them to auto-loading.
uplevel #0 {
    source [file join $tychokernel Color.tcl]
    source [file join $tychokernel TopLevel.itcl]
    source [file join $tychokernel TychoWidget.itcl]
    source [file join $tychokernel Dismiss.itcl]
    source [file join $tychokernel DialogWindow.itcl]
    source [file join $tychokernel FontManager.itcl]
    source [file join $tychokernel Message.itcl]
    source [file join $tychokernel ErrorMessage.itcl]
}

# Register the standard context-sensitive editors
# Some of these are commented out because they are not ready for release.
namespace ::tycho {
    ::tycho::File::registerEditor {} ::tycho::Edit {Plain text editor}
    ::tycho::File::registerEditor {.sched .c .y} ::tycho::EditC {C editor}
    ::tycho::File::registerEditor {.C .cc .h .H} ::tycho::EditCpp {C++ editor}
    ::tycho::File::registerEditor {.strl} ::tycho::EditEsterel {Esterel editor}
    ::tycho::File::registerEditor {} ::tycho::EditHTML {HTML editor}
    ::tycho::File::registerEditor {.itcl .itk} ::tycho::EditItcl {Itcl editor}
    ::tycho::File::registerEditor .tcl ::tycho::EditTcl {Tcl editor}
    ::tycho::File::registerEditor .pl ::tycho::EditPtlang {Ptlang editor}
    ::tycho::File::registerEditor {} ::tycho::TclConsole {Tcl console}
    ::tycho::File::registerEditor {} ::tycho::ProfileTcl {Tcl profiler}

    if {[uplevel #0 info commands matlab] != {}} {
	::tycho::File::registerEditor {} ::tycho::Matlab {Matlab console}
    }
    if {[uplevel #0 info commands mathematica] != {}} {
	::tycho::File::registerEditor {} ::tycho::Mathematica \
		{Mathematica console}
    }
    ::tycho::File::registerEditor {.html .htm .htl} ::tycho::HTML {}
    ::tycho::File::registerEditor {.mk .template} ::tycho::EditMake \
	    {Makefile editor} {Makefile makefile GNUmakefile}
    # ::tycho::File::registerEditor {.fsm} ::tycho::EditFSM \
    #   {Finite state machine editor}
    ::tycho::File::registerEditor {.std} ::tycho::EditSTD {State transition diagram editor}
    ::tycho::File::registerEditor {.idx} ::tycho::IndexBrowser {}
}

# FIXME: This entry binding patch may not be needed in the future.
# The following patch is contributed by Eric H. Herrin II.
#
# override some default Entry bindings for speed; 
# Itcl 2.0 seems *way* too slow using the defaults... 
#
bind Entry <KeyPress> {
    if {"%A" != "{}"} {
        set insert [%W index insert]
        catch {
            if {([%W index sel.first] <= $insert)
            && ([%W index sel.last] >= $insert)} {
                %W delete sel.first sel.last
            }
        }
        %W insert insert %A
        incr insert
        set left [%W index @0]
        if {$left > $insert} {
            %W xview $insert
            return
        }
        set x [winfo width %W]
        set right [%W index @$x]
        incr right -1
        if {$right < $insert} {
            while {$right < $insert} {
                incr left
                incr right
            }
            %W xview $left
        }
    }
}
