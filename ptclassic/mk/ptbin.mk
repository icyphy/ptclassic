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
# Programmers:  Christopher Hylands, Jose Luis Pino
# Based on pigiRpc/makefile, originally by Joe Buck,
#  with modifications from just about everyone on the Ptolemy Team.
#
# This makefile is included by makefiles that build standalone binaries.
# By having one common makefile, we don't need to update lots of other
# makefiles.  Currently, ptcl, pigiRpc and tycho use this file

# Todo:
#   Check out with compile-sdf
#   Build Shared itcl libraries
#   Test out pigiRpc.cp
#   Make a non-shared library
#
# Bugs:
#   version.*.c is not regenerated each time, so the build time is not updated
#   version.*.c does not contain a description of the binary.
#   make realclean fails in ptcl and tycho because $(PIGI) is not defined

# To create a custom executable, you should define a file named override.mk
# in your $(ROOT)/obj.$(PTARCH)/$(PIGI) directory. You should also have a 
# symbolic link from $(ROOT)/src.$(PTARCH)/$(PIGI)/SCCS to the ptolemy
# SCCS directory $(PTOLEMY/src.$(PTARCH)/$(PIGI)/SCCS.
# The override.mk file should define the variables necessary from stars.mk
# to pull in the domains you want.  It also must define the $(PIGI) variable.
# Here a minimal override.mk which will just pull in SDF:
#
# PIGI=pigiRpc
# SDFFULL=1

VERSION =	0.6devel.$(BASENAME)

ifdef ALLBINARIES
ifndef PTRIM
ifndef PTINY
	FULL=1
endif
endif
endif

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

REALCLEAN_STUFF= $(PIGI_BINARIES) \
		$(PIGI).ptiny $(PIGI).ptiny.debug $(PIGI).ptiny.debug.purify \
		$(PIGI).ptiny.debug.quantify $(PIGI).ptiny.debug.purecov \
		$(PIGI).ptrim $(PIGI).ptrim.debug $(PIGI).ptrim.debug.purify \
		$(PIGI).ptrim.debug.quantify $(PIGI).ptrim.debug.purecov

####################################################################
# Variable definitions are above this point.  Rules are below this point.

# We have three main binaries, ptiny, ptrim and pigi.
# Each binary has the following versions:
#		 vanilla, debug, quantify, purify, purecov
# At the end we define a few special cases

# See the calling makefile for the all: rule


####################################################################
# PIGI versions

INSTALL += makefile $(BINDIR)/$(PIGI)

ifdef ALLBINARIES
INSTALL += $(BINDIR)/$(BASENAME).ptrim $(BINDIR)/$(BASENAME).ptiny

$(BASENAME).ptrim: $(PT_DEPEND)
	make PTRIM=1 BASENAME=$(BASENAME) $(BASENAME).ptrim

$(BASENAME).ptiny: $(PT_DEPEND)
	make PTINY=1 BASENAME=$(BASENAME) $(BASENAME).ptiny

$(BASENAME).ptrim.debug: $(PT_DEPEND)
	make PTRIM=1 BASENAME=$(BASENAME) $(BASENAME).ptrim.debug

$(BASENAME).ptiny.debug: $(PT_DEPEND)
	make PTINY=1 BASENAME=$(BASENAME) $(BASENAME).ptiny.debug

$(BASENAME).ptrim.debug.purify: $(PT_DEPEND)
	make PTRIM=1 BASENAME=$(BASENAME) $(BASENAME).ptrim.debug.purify

$(BASENAME).ptiny.debug.purify: $(PT_DEPEND)
	make PTINY=1 BASENAME=$(BASENAME) $(BASENAME).ptiny.debug.purify

$(BASENAME).ptrim.debug.quantify: $(PT_DEPEND)
	make PTRIM=1 BASENAME=$(BASENAME) $(BASENAME).ptrim.debug.quantify

$(BASENAME).ptiny.debug.quantify: $(PT_DEPEND)
	make PTINY=1 BASENAME=$(BASENAME) $(BASENAME).ptiny.debug.quantify

$(BASENAME).ptrim.debug.purecov: $(PT_DEPEND)
	make PTRIM=1 BASENAME=$(BASENAME) $(BASENAME).ptrim.debug.purecov

$(BASENAME).ptiny.debug.purecov: $(PT_DEPEND)
	make PTINY=1 BASENAME=$(BASENAME) $(BASENAME).ptiny.debug.purecov

$(BINDIR)/$(BASENAME).ptrim: $(BASENAME).ptrim 
	make PTRIM=1 BASENAME=$(BASENAME) $(BINDIR)/$(BASENAME).ptrim

$(BINDIR)/$(BASENAME).ptiny: $(BASENAME).ptiny 
	make PTINY=1 BASENAME=$(BASENAME) $(BINDIR)/$(BASENAME).ptiny

endif #ALLBINARIES

$(PIGI): $(PT_DEPEND) $(ADD_OBJS)
	$(PURELINK) $(LINKER) $(LINKFLAGS) $(PIGI_OBJS) $(LIBS) -o $@

# Same, with debugging symbols.
$(PIGI).debug: $(PT_DEPEND) $(ADD_OBJS)
	$(PURELINK) $(LINKER) $(LINKFLAGS_D) $(PIGI_OBJS) $(LIBS) -o $@

# Same, with debugging symbols and purify
$(PIGI).debug.purify: $(PT_DEPEND) $(ADD_OBJS)
	$(PURIFY) $(LINKER) $(LINKFLAGS_D) $(PIGI_OBJS) $(LIBS) -o $@

# Same, with quantify, for profiling.
$(PIGI).debug.quantify: $(PT_DEPEND) $(ADD_OBJS)
	$(QUANTIFY) $(LINKER) $(LINKFLAGS_D) $(PIGI_OBJS) $(LIBS) -o $@

# Same, with purecov, for code coverage measurements.
$(PIGI).debug.purecov: $(PT_DEPEND) $(ADD_OBJS)
	$(PURECOV) $(LINKER) $(LINKFLAGS_D) $(PIGI_OBJS) $(LIBS) -o $@

version.$(PIGI).o: $(PT_DEPEND)
	echo char '*gVersion = "Version:' $(VERSION) \
		'%created' `date` '";' | sed 's/%/\\n/g' > version.$(PIGI).c
	$(CC) -c version.$(PIGI).c

$(BINDIR)/$(PIGI): $(PIGI)
		@echo Installing $<
		rm -f $(BINDIR)/$(PIGI)
		ln $< $(BINDIR)/$(PIGI)

install: $(INSTALL)
