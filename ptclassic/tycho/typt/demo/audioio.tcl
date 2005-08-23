# The control panel for a straight-through audio demo
#
# @Author: John Reekie
#
# @Version: @(#)audioio.tcl	1.2 03/03/97
#
# @Copyright (c) 1996-1997 The Regents of the University of California.
# All rights reserved.
#
# Permission is hereby granted, without written agreement and without
# license or royalty fees, to use, copy, modify, and distribute this
# software and its documentation for any purpose, provided that the above
# copyright notice and the following two paragraphs appear in all copies
# of this software.
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
#                                        PT_COPYRIGHT_VERSION_2
#                                        COPYRIGHTENDKEY
#######################################################################

# Turn on debugging
# controlPanel debug 1

# Add a help button
controlPanel configure -helpfile {$tycho/typt/demo/audioio.html}

# Create the AudioIO control sub-panel
::tycho::AudioIO controlPanel.io

# Add it to the main panel
controlPanel add controlPanel.io -side left


