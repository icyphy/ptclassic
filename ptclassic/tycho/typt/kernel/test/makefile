# Makefile for Ptolemy Tycho system, typt/kernel/test directory.
#
# $Id$
# Author: Christopher Hylands
#
# Copyright (c) 1996-%Q% The Regents of the University of California.
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
VPATH =		$(ROOT)/typt/kernel/test

# Get configuration info
CONFIG =	$(ROOT)/mk/tycho.mk
include $(CONFIG)

TCL_SRCS =

SIMPLE_TESTS = \
	testBlock.itcl \
	testGalaxy.itcl \
	testUniverse.itcl

GRAPHICAL_TESTS = \
	testDomainInfo.itcl \
	testProfile.itcl \
	testPtWelcomeMessage.itcl

ITCL_SRCS = \
	$(SIMPLE_TESTS) $(GRAPHICAL_TESTS)

EXTRA_SRCS =	$(TCL_SRCS) $(ITCL_SRCS)

MISC_FILES =

# 'make realclean' should remove tclIndex
REALCLEAN_STUFF =

tests:: test_simple test_graphics

# These tests do not require a graphics terminal
test_simple: $(TCL_SRCS) $(ITCL_SRCS)

# These tests require a graphics terminal
test_graphics: $(TCL_SRCS) $(ITCL_SRCS)
	tycho -e testDomainInfo.itcl
	tycho -e testProfile.itcl
	tycho -e testPtWelcomeMessage.itcl

# We don't include common.mk since we don't use the compiler here
include $(ROOT)/mk/no-compile.mk
