# Makefile for Ptolemy Tycho system, editors/textedit/test directory.
#
# @(#)makefile	1.6 04/07/98
# Author: Christopher Hylands
#
# Copyright (c) 1996-1998 The Regents of the University of California.
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
VPATH =		$(ROOT)/editors/textedit/test

# Get configuration info
CONFIG =	$(ROOT)/mk/tycho.mk
include $(CONFIG)

TCL_SRCS = 	

GRAPHICAL_TESTS = \
	testEditC.itcl \
	testEditHTML.itcl \
	testEditItcl.itcl \
	testFiles.itcl \
	testPtWidgets.itcl \
	testWidgets.itcl

ITCL_SRCS = $(GRAPHICAL_TESTS) \
	alltests.itcl all.itcl

EXTRA_SRCS =	$(ITCL_SRCS)

MISC_FILES = \
	Blink.html \
	Blink.java \
	CTestFile.c \
	CppTestFile.cc \
	DrawTest.java \
	PtclTestFile.pt \
	SDFMyRamp.pl \
	evalTest.itcl \
	myTclCmd.c \
	wristwatch.strl \
	ItclClassListA.itcl \
	ItclClassListB.itcl \
	ItclClassListC.itcl \
	EditItcl.itcl

# Sources that may or may not be present, but if they are present, we don't
# want make checkjunk to barf on them.
OPTIONAL_FILES = \
	SDFMyRamp.cc \
	SDFMyRamp.h \
	SDFMyRamp.htm \
	cedit \
	class.dag \
	doc

# 'make realclean' should remove tclIndex
REALCLEAN_STUFF = $(OPTIONAL_FILES)

tests:: test_graphics

test_graphics: alltests.itcl all.itcl $(TCL_SRCS) $(ITCL_SRCS)
	tytest $(GRAPHICAL_TESTS)

# We don't include common.mk since we don't use the compiler here
include $(ROOT)/mk/no-compile.mk
