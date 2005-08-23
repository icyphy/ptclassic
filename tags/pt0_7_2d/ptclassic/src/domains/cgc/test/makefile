# Makefile for testing Ptolemy CGC stars 
#
# @(#)makefile	1.1 12/09/97
# Author: Christopher Hylands
#
# Copyright (c) 1997 The Regents of the University of California.
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

# Root of Tycho directory
ROOT =		../../../..

# True source directory
VPATH =		$(ROOT)/src/domains/cgc/test

# Get configuration info
CONFIG =	$(ROOT)/mk/tycho.mk
include $(CONFIG)

TCL_SRCS = testCGCDefs.tcl

# Demos that use stars that are not present in pitcl
SIMPLE_PTCL_TESTS = \
	testCGCContrib.tcl


# testCGCStars.tcl should be first, since it instantiates one of every star.
SIMPLE_TESTS = \
	testCGCStars.tcl \

GRAPHICAL_TESTS = \
	dummy

# Don't include GRAPHICAL_TESTS until we actually have some
ITCL_SRCS =	\
	$(SIMPLE_TESTS) $(SIMPLE_PTCL_TESTS) \
	alltests.itcl all.itcl

EXTRA_SRCS =	$(TCL_SRCS) $(ITCL_SRCS) \

# Files or directories that are present, but that 'make checkjunk' 
# should not complain about
MISC_FILES = #README

# make checkjunk will not report OPTIONAL_FILES as trash
# make realclean removes OPTIONAL_FILES
OPTIONAL_FILES =

# 'make realclean' should remove tclIndex
REALCLEAN_STUFF =

PTCL_PTINY = $(ROOT)/obj.$(PTARCH)/ptcl/ptcl.ptiny
PTCL = $(ROOT)/obj.$(PTARCH)/ptcl/ptcl
tests:: test_simple test_ptcl

# These tests do not require a graphics terminal
test_simple: $(TCL_SRCS) $(ITCL_SRCS)
	echo "set TYCHO $(PTOLEMY)/tycho; foreach i [list $(SIMPLE_TESTS)] {puts \$$i; source \$$i};     puts \"Total: [expr \$$PASSED + \$$FAILED] (Passed: \$$PASSED Failed: \$$FAILED)\"" | \
		$(PTCL_PTINY)

# These tests do not require a graphics terminal, but do use ptcl
test_ptcl: $(TCL_SRCS) $(ITCL_SRCS)
	echo "set TYCHO $(PTOLEMY)/tycho; foreach i [list $(SIMPLE_PTCL_TESTS)] {puts \$$i; source \$$i};     puts \"Total: [expr \$$PASSED + \$$FAILED] (Passed: \$$PASSED Failed: \$$FAILED)\"" | \
		$(PTCL)

# These tests require a graphics terminal
test_graphics: alltests.itcl all.itcl $(TCL_SRCS) $(ITCL_SRCS)
	tycho -noconsole -e alltests.itcl

FORCE:

# We don't include common.mk since we don't use the compiler here
include $(ROOT)/mk/no-compile.mk
