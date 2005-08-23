# Package load file for the tycho.kernel.html package
#
# @Author: John Reekie
#
# @Version: @(#)html.tcl	1.3 03/23/98
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
package provide tycho.kernel.html 2.0

global env auto_path
set env(HTML_LIBRARY) [file dirname [info script]]
if { [lsearch -exact $auto_path $env(HTML_LIBRARY)] == -1 } {
    lappend auto_path $env(HTML_LIBRARY)
}

### PROTOCOLS
::tycho::register protocol "http" \
	-class ::tycho::ResourceHTTP \
	-label "Hy-Time Transport Protocol (HTTP)"

::tycho::register protocol "ftp" \
	-class ::tycho::ResourceFTP \
	-label "File Transfer Protocol (FTP)"

::tycho::register protocol "mailto" \
	-class ::tycho::ResourceMail \
	-label "Mail Protocol"

### MODES

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

