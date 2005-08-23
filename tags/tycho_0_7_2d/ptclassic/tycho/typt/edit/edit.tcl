# Package load file for the tycho.typt.edit package
#
# @Author: John Reekie
#
# @Version: @(#)edit.tcl	1.1 05/07/98
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

package require tycho.typt.kernel
package provide tycho.typt.edit 0.2.1

global env auto_path

set env(PTEDIT_LIBRARY) [file dirname [info script]]
if { [lsearch -exact $auto_path $env(PTEDIT_LIBRARY)] == -1 } {
    lappend auto_path $env(PTEDIT_LIBRARY)
}

########### edit

if $ptolemyfeature(ptolemyinstalled) {
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
}

