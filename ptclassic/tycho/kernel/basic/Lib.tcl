# Master initialization file for the Tycho tcl/tk Ptolemy interface.
# This depends on a variable TYCHO having been set pointing to the
# Tycho installation directory.
#
# @Author: Edward A. Lee
#
# @Version: $Id$
#
# Copyright (c) 1995-1996 The Regents of the University of California.
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

# Register the standard context-sensitive editors
namespace ::tycho {
    global ptolemypresent tcl_platform
    ############# text editors (alphabetical except the first one)

    ::tycho::File::registerExtensions {} \
            {::tycho::view Edit -file {%s}} \
            {Plain text editor} "text"

    ::tycho::File::registerExtensions {.sched .c .y} \
            {::tycho::view EditC -file {%s}} \
            {C editor} "text"
    ::tycho::File::registerExtensions {.C .cc .h .H} \
            {::tycho::view EditCpp -file {%s}} \
            {C++ editor} "text"
    ::tycho::File::registerExtensions {.strl} \
            {::tycho::view EditEsterel -file {%s}} \
            {Esterel editor} "text"
    ::tycho::File::registerExtensions {.fst} \
            {::tycho::view EditForest -file {%s}} \
            {Forest editor} "graphics"
    ::tycho::File::registerExtensions {} \
            {::tycho::view EditHTML -file {%s}} \
            {HTML editor} "text"
    ::tycho::File::registerExtensions {.gif .ppm .pgm .xbm} \
            {::tycho::view HTML -file {%s} -image 1 -toolbar 1} \
            {}
    ::tycho::File::registerExtensions {.html .htm .htl} \
            {::tycho::view HTML -file {%s} -toolbar 1} \
            {}
    ::tycho::File::registerExtensions {.itcl .itk} \
            {::tycho::view EditItcl -file {%s}} \
            {Itcl editor} "text"
    ::tycho::File::registerExtensions {.java} \
            {::tycho::view EditJava -file {%s}} \
            {Java editor} "text"
    # .vc is for Microsoft Visual c++
    ::tycho::File::registerExtensions {.mk .template .vc} \
            {::tycho::view EditMake -file {%s}} \
	    {Makefile editor} "text"
    ::tycho::File::registerFilenames {Makefile makefile GNUmakefile} \
            {::tycho::view EditMake -file {%s}} \
	    {} 
    ::tycho::File::registerExtensions {.pt .ptcl} \
            {::tycho::view EditPtcl -file {%s}} \
            {Ptcl editor} "text"
    ::tycho::File::registerExtensions .pl \
            {::tycho::view EditPtlang -file {%s}} \
            {Ptlang editor} "text"
    ::tycho::File::registerExtensions .tcl \
            {::tycho::view EditTcl -file {%s}} \
            {Tcl editor} "text"

    # For now, TIM files open a Tcl editor
    ::tycho::File::registerExtensions .tim \
            {::tycho::view EditTcl -file {%s}} \
            {TIM editor} "text"

    ########### graphical editors (alphabetical)
    ::tycho::File::registerExtensions {.dag} \
            {::tycho::view EditDAG -file {%s}} \
            {DAG editor} "graphics"
    # NOTE: Not useful on its own.
    # ::tycho::File::registerExtensions {.fsm} \
    #       {::tycho::view EditFSM -file {%s}} \
    #       {Finite state machine editor}
    ::tycho::File::registerExtensions {.graph} \
            {::tycho::view EditGraph -file {%s}} \
            {Graph editor} "graphics"
    if !$ptolemypresent {
        # Vem is not present.
        ::tycho::File::registerContents [file join schematic {contents;}] \
                {::tycho::view EditPalette -facet {%s}} \
                {Palette editor} "graphics"
    }
    # NOTE: Not ready for release
    # NOTE: Put back in after release 1.1.2 -- hjr
    ::tycho::File::registerExtensions {.std} \
	    {::tycho::view EditSTD -file {%s}} \
	    {State transition diagram editor} "graphics"

    ::tycho::File::registerExtensions {.idx} \
            {::tycho::Dialog::new IndexBrowser [::tycho::autoName .idx] \
            -file {%s}} \
            {}
    if $ptolemypresent {
        # Ptolemy and vem are present.  Use them.
        ::tycho::File::registerContents [file join schematic {contents;}] \
                {::pvOpenWindow [::ptkOpenFacet {%s} schematic contents]} \
                {Vem facet} "graphics"
    }

    ########### tools (alphabetical)

    #::tycho::File::registerExtensions {} \
    #        {::tycho::view CommandShell -file {%s}} \
    #        {Command shell} "tools"

    if {$tcl_platform(platform) != "macintosh"} {
	::tycho::File::registerExtensions {} \
		{set w [::tycho::autoName .glimpse]; \
                ::tycho::Glimpse $w -geometry +0+0;\
                wm deiconify $w} \
		{Glimpse} "tools"
    }

    ::tycho::File::registerExtensions {.icl} \
            {::tycho::view ItclClassList -file {%s} -toolbar 1} \
            {Itcl Class List} "tools"

    if {[uplevel #0 info commands matlab] != {}} {
	::tycho::File::registerExtensions {} \
                {::tycho::view Matlab -file {%s}} \
                {Matlab console} "tools"
    }
    if {[uplevel #0 info commands mathematica] != {}} {
	::tycho::File::registerExtensions {} \
                {::tycho::view Mathematica -file {%s}} \
                {Mathematica console} "tools"
    }
    ::tycho::File::registerExtensions {} \
            {::tycho::view ProfileTcl -file {%s} -toolbar 1} \
            {Tcl profiler} "tools"
    ::tycho::File::registerExtensions {} \
            {::tycho::view TclShell -file {%s}} \
            {Tcl shell} "tools"

    # If we open a shared object, try to load it as a Tycho task
    ::tycho::File::registerExtensions [info sharedlibext] \
            {::tycho::controlpanel {%s}} \
            {}

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
