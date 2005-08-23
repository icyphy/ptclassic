#
# iwidgets.tcl
# ----------------------------------------------------------------------
# Invoked automatically by [incr Tk] upon startup to initialize
# the [incr Widgets] package.
# ----------------------------------------------------------------------
#  AUTHOR: Mark L. Ulferts               EMAIL: mulferts@spd.dsccc.com
#
#  @(#) $Id$
# ----------------------------------------------------------------------
#                Copyright (c) 1995  Mark L. Ulferts
# ======================================================================
# See the file "license.terms" for information on usage and
# redistribution of this file, and for a DISCLAIMER OF ALL WARRANTIES.

package require Tcl 8.0
package require Tk 8.0
package require Itcl 3.0
package require Itk 3.0

namespace eval ::iwidgets {
    variable library [file dirname [info script]]
    variable version 2.2.0
}

lappend auto_path [file join $iwidgets::library scripts]
package provide Iwidgets $iwidgets::version
