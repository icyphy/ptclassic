# Makefile for Ptolemy tcl/tk library (used primarily for 'make sources')

# Version Identification:
# @(#)makefile	1.16 02/04/99
# Copyright (c) 1990-1999 The Regents of the University of California.
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

ROOT = ../..
VPATH = $(ROOT)/lib/tcl

# get configuration info
CONFIG=$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

# Avoid messages from tycommon.mk
LIBR = dummylib

EXTRA_SRCS = \
esterel2pl.tcl \
gantt.tcl \
mathexpr.tcl \
message.tcl \
pigilib.tcl \
ptcl.tcl \
ptcl_help.tcl \
ptk.rc \
ptkBarGraph.tcl \
ptkBind.tcl \
ptkColor.tcl \
ptkControl.tcl \
ptkGantt.tcl \
ptkLogicAnalyzer.itcl \
ptkOptions.tcl \
ptkParams.tcl \
ptkPrfacet.tcl \
ptkRunAllDemos.tcl \
tkMathematica.tcl \
tkoct.tcl \
topgetopt.tcl \
utilities.tcl

MISC_FILES = \
Ptolemy.xbm \
left.xbm \
ptolemySmall.gif \
ptolemyWelcome.gif \
right.xbm

# We don't include common.mk since we don't use the compiler here
include $(ROOT)/mk/no-compile.mk
