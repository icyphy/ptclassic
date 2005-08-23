# Compatbility classes for running Slate stand-alone.
# These are just empty shells of some root Tycho classes.
#
# @Version: @(#)classes.tcl	1.1 07/29/98
# @Author: H. John Reekie
#
# @Copyright (c) 1996- The Regents of the University of California.
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

#######################################################################
#### Object
# This is the base class for Tycho objects that are neither windows nor
# widgets.
#
class ::tycho::Object {
    if ![info exists itcl::version] {
	inherit ::obstcl::Object
    }
}

#######################################################################
#### TWidget
# This is the base class for Tycho widgets
#
class ::tycho::TWidget {
    inherit ::itk::Widget
    constructor {args} {}
}

#########################################################################
#### Uninstantiable
# This is the superclass for classes that cannot be instantiated.
#
class ::tycho::Uninstantiable {
    if ![info exists itcl::version] {
	inherit ::obstcl::Object
    }
    constructor {args} {
	error "The ::tycho::Uninstantiable class cannot be instantiated"
    }
}

