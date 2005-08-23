# Procedures needed for compatibility between Tcl versions
#
# @Author: John Reekie
#
# @Version: @(#)Compat.tcl	1.5 07/21/98
#
# @Copyright (c) 1995-1998 The Regents of the University of California.
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
#######################################################################

##########################################################################
#### namespaceEval
# Evaluate the argument in the given namespace. If there is no
# argument, make sure that the namespace exists. This is needed
# for backward compatibility with Itcl2.2.
#
proc namespaceEval {ns {script {}}} {
    global tcl_version
    if { $tcl_version >= 8.0 } {
	uplevel namespace eval $ns [list $script]
    } else {
	uplevel namespace $ns [list $script]
    }
}

##########################################################################
#### winfoMegawidget
# 
# Use this instead of "winfo megawidget."
#
proc winfoMegawidget {w} {
    global tcl_version
    if { $tcl_version >= 8.0 } {
	while {$w != ""} {
	    if {![catch {uplevel "$w isa ::itk::Archetype"}]} {
		return $w
	    } else {
		set w [winfo parent $w]
	    }
	}
	return $w
    } else {
	winfo megawidget $w
    }
}

##########################################################################
#### namespaceTail
# 
# Use this instead of "namespace tail" or "info namespace tail"
#
proc namespaceTail {n} {
    global tcl_version
    if { $tcl_version >= 8.0 && ${itcl::version} >= 3.0 } {
	return [namespace tail $n]
    } else {
	return [info namespace tail $n]
    }
}

##########################################################################
#### namespaceChildren
# 
# Use this instead of "info namespace children"
#
proc namespaceChildren {n} {
    global tcl_version
    if { $tcl_version >= 8.0 && ${itcl::version} >= 3.0 } {
	return [namespace children $n]
    } else {
	return [info namespace children $n]
    }
}

##########################################################################
#### namespaceQualifiers
# 
# Use this instead of "info namespace qualifiers"
#
proc namespaceQualifiers {n} {
    global tcl_version
    if { $tcl_version >= 8.0 && ${itcl::version} >= 3.0 } {
	return [namespace qualifiers $n]
    } else {
	return [info namespace qualifiers $n]
    }
}

##########################################################################
#### @scope
# 
# Defined for Itcl version 3.0, since it disappeared. Sheesh...
#
if { ${itcl::version} >= 3.0 } {
    proc @scope {ns args} {
	uplevel namespace inscope $ns $args
    }
}

##########################################################################
#### infoClasses
# 
# Use this instead of "info classes"
#
proc infoClasses {{c {}}} {
    global tcl_version
    if { $tcl_version >= 8.0 && ${itcl::version} >= 3.0 } {
	uplevel ::itcl::find classes $c
    } else {
	uplevel info classes $c
    }
}

##########################################################################
#### infoObjects
# 
# Use this instead of "info objects"
#
proc infoObjects {{o {}}} {
    global tcl_version
    if { $tcl_version >= 8.0 && ${itcl::version} >= 3.0 } {
	uplevel ::itcl::find objects $o
    } else {
	uplevel info objects $o
    }
}

##########################################################################
#### infoWhichCommand
# 
# Use this instead of "info which -command foo" or "info commands foo."
#
proc infoWhichCommand {c} {
    global tcl_version
    if { $tcl_version >= 8.0 && ${itcl::version} >= 3.0 } {
	uplevel info commands $c
    } else {
	uplevel info which -command $c
    }
}

##########################################################################
#### infoWhich
# 
# Use this instead of "info which foo" or "info commands foo."
#
proc infoWhich {c} {
    global tcl_version
    if { $tcl_version >= 8.0 && ${itcl::version} >= 3.0 } {
	uplevel info commands $c
    } else {
	uplevel info which $c
    }
}

##########################################################################
#### infoContext
# 
# Use this instead of "info context" or "namespace current"
#
proc infoContext {} {
    global tcl_version
    if { $tcl_version >= 8.0 && ${itcl::version} >= 3.0 } {
	uplevel namespace current
    } else {
	uplevel info context
    }
}
##########################################################################
#### winfoCommand
# 
# Use this instead of "winfo command win"
#
proc winfoCommand {w} {
    global tcl_version
    if { $tcl_version >= 8.0 && ${itcl::version} >= 3.0 } {
	namespace which -command $w
    } else {
	winfo command $w
    }
}
