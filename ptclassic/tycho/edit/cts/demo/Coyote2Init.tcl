# Coyote2 Initialization for CTS Domain
#
# $Id$
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
# Initialization script for the SecOrder demo

# Update procedures
# FIXME: We need to figure out how to use namespaces
# properly for this!

proc ::_freq {sim val} {
    catch {$sim send "\$vertex0 setParam frequency $val"}
}

proc ::_ampl {sim val} {
    catch {$sim send "\$vertex5 setParam gain $val"}
}

proc ::_innerfb {sim val} {
    catch {$sim send "\$vertex8 setParam gain [expr -$val]"}
}

proc ::_outerfb {sim val} {
    catch {$sim send "\$vertex7 setParam gain [expr -$val]"}
}

# Create a slider panel for the squarewave source
$runWindow add source {
    ::tycho::SliderPanel $itk_interior.source -label Source
} {
    keep -background -cursor -font
}
$runWindow show source -side left -expand no \
	-padx 2 -pady 4 \
	-anchor n

# Create the two sliders
$source add freq -label Freq -from 0.1 -to 2 \
	-resolution 0.1 \
	-digits 2 \
	-command "_freq $simulation" \
	-initial 0.5 \
	-totalwidth 40

$source add ampl -label Ampl -from 500 -to 1500 \
	-resolution 50 \
	-command "_ampl $simulation" \
	-initial 1000 \
	-totalwidth 40

# Create a slider panel for the gain controls
$runWindow add gain {
    ::tycho::SliderPanel $itk_interior.gain -label Feedback
} {
    keep -background -cursor -font
}
$runWindow show gain -side left -expand no \
	-padx 2 -pady 4 \
	-anchor n 

# Create the two sliders
$gain add innerfb -label Inner -from 20 -to 30 \
	-resolution 0.01 -digits 3 \
	-command "_innerfb $simulation" \
	-initial 25 \
	-totalwidth 40

$gain add outerfb -label Outer -from 2000 -to 3000 \
	-resolution 10 -digits 3 \
	-command "_outerfb $simulation" \
	-initial 2500 \
	-totalwidth 40
