# This Tcl script invokes starindex.tcl to traverse a list of domains
# to generate an HTML cross-index of all stars, galaxies, and universes.
#
# Copyright (c) 1990-%Q% The Regents of the University of California.
# All Rights Reserved.
# See $PTOLEMY/copyright for the complete copyright notice.
#
# Programmer: Brian Evans
# Version: $Id$
#
# Set ptolemy and PTOLEMY as global Tcl variables
set ptolemy $env(PTOLEMY)
set PTOLEMY $env(PTOLEMY)

source $PTOLEMY/lib/tcl/starindex.tcl

# Parse arguments
if { $argc != 2 } {
  puts stderr "Usage: mkStarIndex domainList destDirectory"
  exit 1
} else {
  starindex_WriteWWWStarDemoDir [lindex $argv 0] [lindex $argv 1]
}
