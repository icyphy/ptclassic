# Makefile for Ptolemy Tycho system, editors/slate/test directory.
#
# $Id$
# Makefile Author: Christopher Hylands
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


# Root of Ptolemy directory
ROOT =		../../..

# True source directory
VPATH =		$(ROOT)/tycho/editors/slate

# get configuration info
CONFIG =	$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

TCL_SRCS = 
  
ITCL_SRCS = \
	sample.itcl \
	stars.itcl

EXTRA_SRCS =	$(TCL_SRCS) $(ITCL_SRCS)

MISC_FILES = \
	748.gif \
	900ss.gif \
	australia.gif \
	bind.tcl \
	constraint.tcl \
	demo.itcl \
	makefile \
	mantra.gif \
	pigi.itcl \
	sample.itcl \
	simple.tcl \
	stars.itcl \
	trace.itcl

# make checkjunk should not complain about the OLD directory
OPTIONAL_FILES = OLD

# 'make realclean' should remove tclIndex
REALCLEAN_STUFF =

# Include information on all stars and targets.
include $(ROOT)/mk/stars.mk

all install: $(MISC_SRCS)

# We don't include common.mk since we don't use the compiler here
include $(ROOT)/mk/no-compile.mk

