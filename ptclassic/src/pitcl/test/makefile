# Makefile for Ptolemy Itcl interface (PITcl): test directory
#
# $Id$
# Authors: Edward A. Lee, Christopher Hylands
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
ROOT =		../../..

# True source directory
VPATH =		$(ROOT)/src/pitcl/test

# Get configuration info
CONFIG =	$(ROOT)/mk/tycho.mk
include $(CONFIG)

TCL_SRCS =

SIMPLE_TESTS = \
	testBlock.itcl \
	testMisc.itcl \
	testPorts.itcl \
	testRemove.itcl

GRAPHICAL_TESTS =

ITCL_SRCS =

EXTRA_SRCS =	$(TCL_SRCS) $(ITCL_SRCS)

MISC_FILES =

# Sources that may or may not be present, but if they are present, we don't
# want make checkjunk to barf on them.
OPTIONAL_FILES =

# 'make realclean' should remove tclIndex
REALCLEAN_STUFF =

tests:: test_graphics

# These tests do not require a graphics terminal
test_simple: $(TCL_SRCS) $(ITCL_SRCS)
	echo "foreach i [list $(SIMPLE_TESTS)] {puts \$$i; source \$$i}" | \
		$(ITCLSH)

# These tests require a graphics terminal
test_graphics: alltests.itcl
	tycho -e alltests.itcl

FORCE:

test_exec_msg:
	@echo "This message is used to test the Exec class"

# We don't include common.mk since we don't use the compiler here
include $(ROOT)/mk/no-compile.mk
