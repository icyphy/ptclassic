# Makefile to build ptcl, tycho and pigiRpc binaries
# $Id$

# Copyright (c) 1990-%Q% The Regents of the University of California.
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
#		       
# Programmer:  Christopher Hylands, Jose Luis Pino
# Based on pigiRpc/makefile, originally by Joe Buck,
#  with modifications from just about everyone on the Ptolemy Team.
#
# This makefile is included by makefiles that build standalone binaries.
# By having one common makefile, we don't need to update lots of other
# makefiles.  Currently, ptcl, pigiRpc and tycho use this file

# Todo:
# Check out with compile-sdf
# Build Shared itcl libraries
# Test out pigiRpc.cp
# Make a non-shared library

VERSION =	0.6devel.$(BASENAME)

ifdef FULL
	PIGI=$(BASENAME)
	BDF=1
	CG56=1
	CG96=1
	CGCFULL=1
	CGFULL=1
	DDF=1
	DE=1
	IPUS=1
	MDSDF=1
	PN=1
	SDFFULL=1
	SILAGE=1
	THOR=1
	VHDL=1
endif

ifdef PTINY
	PIGI=$(BASENAME).ptiny
	DE=1
	SDF=1
	SDFTK=1
	SDFDSP=1
	SDFMATRIX=1
	SDFMATLAB=1
endif

ifdef PTRIM
	PIGI=$(BASENAME).ptrim
	BDF=1
	CGCFULL=1
	CGFULL=1
	DDF=1
	DE=1
	SDFFULL=1
endif

ifndef TK
	# Don't need Higher Order Functions.  ptcl does not need HOF
	HOF=
endif

include $(ROOT)/mk/stars.mk

PIGI_OBJS += $(STARS) $(TARGETS)

PIGI_BINARIES = 	$(PIGI) $(PIGI).debug $(PIGI).debug.purify \
			$(PIGI).debug.quantify $(PIGI).debug.purecov

REALCLEAN_STUFF= $(PIGI_BINARIES)

####################################################################
# Variable definitions are above this point.  Rules are below this point.

# We have three main binaries, ptiny, ptrim and pigi.
# Each binary has the following versions:
#		 vanilla, debug, quantify, purify, purecov
# At the end we define a few special cases

# See the calling makefile for the all: rule


####################################################################
# PIGI versions

ifdef ALLBINARIES
INSTALL = makefile $(BINDIR)/$(BASENAME).ptrim \
	$(BINDIR)/$(BASENAME).ptiny $(BINDIR)/$(BASENAME)
install: $(INSTALL)

$(BASENAME).ptrim:
	make PTRIM=1 BASENAME=$(BASENAME) $(BASENAME).ptrim

$(BASENAME).ptiny:
	make PTINY=1 BASENAME=$(BASENAME) $(BASENAME).ptiny

$(BASENAME):
	make FULL=1 BASENAME=$(BASENAME) $(BASENAME)

$(BASENAME).ptrim.debug:
	make PTRIM=1 BASENAME=$(BASENAME) $(BASENAME).ptrim.debug

$(BASENAME).ptiny.debug:
	make PTINY=1 BASENAME=$(BASENAME) $(BASENAME).ptiny.debug

$(BASENAME).debug:
	make FULL=1 BASENAME=$(BASENAME) $(BASENAME).debug

$(BASENAME).ptrim.debug.purify:
	make PTRIM=1 BASENAME=$(BASENAME) $(BASENAME).ptrim.debug.purify

$(BASENAME).ptiny.debug.purify:
	make PTINY=1 BASENAME=$(BASENAME) $(BASENAME).ptiny.debug.purify

$(BASENAME).debug.purify:
	make FULL=1 BASENAME=$(BASENAME) $(BASENAME).debug.purify

$(BASENAME).ptrim.debug.quantify:
	make PTRIM=1 BASENAME=$(BASENAME) $(BASENAME).ptrim.debug.quantify

$(BASENAME).ptiny.debug.quantify:
	make PTINY=1 BASENAME=$(BASENAME) $(BASENAME).ptiny.debug.quantify

$(BASENAME).debug.quantify:
	make FULL=1 BASENAME=$(BASENAME) $(BASENAME).debug.quantify

$(BASENAME).ptrim.debug.purecov:
	make PTRIM=1 BASENAME=$(BASENAME) $(BASENAME).ptrim.debug.purecov

$(BASENAME).ptiny.debug.purecov:
	make PTINY=1 BASENAME=$(BASENAME) $(BASENAME).ptiny.debug.purecov

$(BASENAME).debug.purecov:
	make FULL=1 BASENAME=$(BASENAME) $(BASENAME).debug.purecov

$(BINDIR)/$(BASENAME).ptrim: $(BASENAME).ptrim
	make PTRIM=1 BASENAME=$(BASENAME) install

$(BINDIR)/$(BASENAME).ptiny: $(BASENAME).ptiny
	make PTINY=1 BASENAME=$(BASENAME) install

$(BINDIR)/$(BASENAME): $(BASENAME).debug.purecov
	make FULL=1 BASENAME=$(BASENAME) install

else #ALLBINARIES

$(PIGI): $(PT_DEPEND) $(ADD_SRCS)
	$(PURELINK) $(LINKER) $(LINKFLAGS) $(PIGI_OBJS) $(LIBS) -o $@

# Same, with debugging symbols.
$(PIGI).debug: $(PT_DEPEND) $(ADD_SRCS)
	$(PURELINK) $(LINKER) $(LINKFLAGS_D) $(PIGI_OBJS) $(LIBS) -o $@

# Same, with debugging symbols and purify
$(PIGI).debug.purify: $(PT_DEPEND) $(ADD_SRCS)
	$(PURIFY) $(LINKER) $(LINKFLAGS_D) $(PIGI_OBJS) $(LIBS) -o $@

# Same, with quantify, for profiling.
$(PIGI).debug.quantify: $(PT_DEPEND) $(ADD_SRCS)
	$(QUANTIFY) $(LINKER) $(LINKFLAGS_D) $(PIGI_OBJS) $(LIBS) -o $@

# Same, with purecov, for code coverage measurements.
$(PIGI).debug.purecov: $(PT_DEPEND) $(ADD_SRCS)
	$(PURECOV) $(LINKER) $(LINKFLAGS_D) $(PIGI_OBJS) $(LIBS) -o $@

$(BINDIR)/$(PIGI): $(PIGI)
		@echo Installing $<
		rm -f $(BINDIR)/$(PIGI)
		ln $< $(BINDIR)/$(PIGI)

INSTALL += makefile $(BINDIR)/$(PIGI)
install: $(INSTALL)

endif #ALLBINARIES
