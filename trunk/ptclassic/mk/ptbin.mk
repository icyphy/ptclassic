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
#   Make a non-shared library
#
# To create a custom executable, you should define a file named override.mk
# in your $(ROOT)/obj.$(PTARCH)/$(PIGI) directory. You should also have a 
# symbolic link from $(ROOT)/src.$(PTARCH)/$(PIGI)/SCCS to the ptolemy
# SCCS directory $(PTOLEMY/src.$(PTARCH)/$(PIGI)/SCCS.
# The override.mk file should define the variables necessary from stars.mk
# to pull in the domains you want.  It also must define the $(PIGI) variable.
# Here a minimal override.mk which will just pull in SDF:
#
# PIGI =	pigiRpc
# SDFFULL =	1

ifndef VERSION
	VERSION =	0.6devel.$(BASENAME)
endif

ifdef ALLBINARIES
ifndef PTRIM
ifndef PTINY
	FULL =		1
endif
endif
endif

ifdef FULL
	PIGI = 		$(BASENAME)
	VERSION_DESC =	'With All Common Domains'
	BDF =		1
	CG56 =		1
	CG96 =		1
	CGCFULL =	1
	CGFULL =	1
	CODESIGN =	1
	DDF =		1
	DE =		1
	ifeq ($(INCLUDE_IPUS_DOMAIN),yes)
		# Non G++ compilers fail to compile IPUS' templates
		IPUS =		1
	endif
	DMM = 		1
	HOF =		1
	MDSDF =		1
	ifeq ($(INCLUDE_PN_DOMAIN),yes)
		PN =		1
	endif
	SDFFULL =	1
	SILAGE =	1
	THOR =		1
	VHDL =		1
	VHDLF =		1
	VHDLB =		1
endif

ifdef PTINY
	PIGI=		$(BASENAME).ptiny
	VERSION_DESC =	'With SDF (no image stars) and DE domains only'
	DE =		1
	HOF =		1
	SDF =		1
	SDFTK =		1
	SDFDSP =	1
	SDFDMM =	1
	SDFMATRIX =	1
	SDFMATLAB =	1
endif

ifdef PTRIM
	PIGI =		$(BASENAME).ptrim
	VERSION_DESC =	'With SDF, DE, BDF, DDF and CGC domains only'
	BDF =		1
	CGCFULL =	1
	CGFULL =	1
	DDF =		1
	DE =		1
	HOF =		1
	SDFFULL =	1
endif

ifdef PTCP
	PIGI =		$(BASENAME).ptcp
	VERSION_DESC =	'With DE, and CP domain only, on the sun4 only'
	DEFAULT_DOMAIN = CP
	DE =		1
	HOF =		1
	CP =		1
endif

ifndef TK
	# Don't need Higher Order Functions.  ptcl does not need HOF
	HOF=
	# DMM needs Oct.
	DMM=
	# SDFDMM uses TclScript
	SDFDMM =
endif

include $(ROOT)/mk/stars.mk

# matlab.mk and mathematica.mk check these vars before traversing the path
NEED_MATLABDIR = 	1
NEED_MATHEMATICADIR = 	1

PIGI_OBJS += $(STARS) $(TARGETS) $(MISC_OBJS)

PIGI_BINARIES = 	$(PIGI) $(PIGI).debug $(PIGI).debug.purify \
			$(PIGI).debug.quantify $(PIGI).debug.purecov

EVERY_BINARY= $(PIGI_BINARIES) \
		$(PIGI).ptiny $(PIGI).ptiny.debug $(PIGI).ptiny.debug.purify \
		$(PIGI).ptiny.debug.quantify $(PIGI).ptiny.debug.purecov \
		$(PIGI).ptrim $(PIGI).ptrim.debug $(PIGI).ptrim.debug.purify \
		$(PIGI).ptrim.debug.quantify $(PIGI).ptrim.debug.purecov \
		$(PIGI).ptcp $(PIGI).ptcp.debug $(PIGI).ptcp.debug.purify \
		$(PIGI).ptcp.debug.quantify $(PIGI).ptcp.debug.purecov
REALCLEAN_STUFF =	$(EVERY_BINARY)

####################################################################
# Variable definitions are above this point.  Rules are below this point.

# We have three main binaries, ptiny, ptrim and pigi.
# Each binary has the following versions:
#		 vanilla, debug, quantify, purify, purecov
# At the end we define a few special cases
# We also have ptcp, which includes the CP domain.  The CP domain
# is present on the sun4 only
# See the calling makefile for the all: rule

####################################################################
# PIGI versions

INSTALL += makefile $(BINDIR)/$(PIGI)

ifndef ALLBINARIES
# This is the default target
$(PIGI): $(PT_DEPEND) $(ADD_OBJS)
	echo char '*gVersion = "Version:' $(VERSION) \
		$(VERSION_DESC) \
		'%created' `date` '";' | sed 's/%/\\n/g' > version.c
	echo "char DEFAULT_DOMAIN[] = \"$(DEFAULT_DOMAIN)\";" \
		>> version.c
	$(CC) -c version.c
	$(PURELINK) $(LINKER) $(LINKFLAGS) $(PIGI_OBJS) $(LIBS) -o $@

# Same, with debugging symbols.
$(PIGI).debug: $(PT_DEPEND) $(ADD_OBJS)
	echo char '*gVersion = "Version:' $(VERSION) \
		$(VERSION_DESC) \
		'and debugging symbols.' \
		'%created' `date` '";' | sed 's/%/\\n/g' > version.c
	echo "char DEFAULT_DOMAIN[] = \"$(DEFAULT_DOMAIN)\";" \
		>> version.c
	$(CC) -c version.c
	$(PURELINK) $(LINKER) $(LINKFLAGS_D) $(PIGI_OBJS) $(LIBS) -o $@

# Same, with debugging symbols and purify
$(PIGI).debug.purify: $(PT_DEPEND) $(ADD_OBJS)
	echo char '*gVersion = "Version:' $(VERSION) \
		$(VERSION_DESC) \
		'and debugging symbols and purify.' \
		'%created' `date` '";' | sed 's/%/\\n/g' > version.c
	echo "char DEFAULT_DOMAIN[] = \"$(DEFAULT_DOMAIN)\";" \
		>> version.c
	$(CC) -c version.c
	$(PURIFY) $(LINKER) $(LINKFLAGS_D) $(PIGI_OBJS) $(LIBS) -o $@

# Same, with quantify, for profiling.
$(PIGI).debug.quantify: $(PT_DEPEND) $(ADD_OBJS)
	echo char '*gVersion = "Version:' $(VERSION) \
		$(VERSION_DESC) \
		'and debugging symbols and quantify.' \
		'%created' `date` '";' | sed 's/%/\\n/g' > version.c
	echo "char DEFAULT_DOMAIN[] = \"$(DEFAULT_DOMAIN)\";" \
		>> version.c
	$(CC) -c version.c
	$(QUANTIFY) $(LINKER) $(LINKFLAGS_D) $(PIGI_OBJS) $(LIBS) -o $@

# Same, with purecov, for code coverage measurements.
$(PIGI).debug.purecov: $(PT_DEPEND) $(ADD_OBJS)
	echo char '*gVersion = "Version:' $(VERSION) \
		$(VERSION_DESC) \
		'and debugging symbols and purecov.' \
		'%created' `date` '";' | sed 's/%/\\n/g' > version.c
	echo "char DEFAULT_DOMAIN[] = \"$(DEFAULT_DOMAIN)\";" \
		>> version.c
	$(CC) -c version.c
	$(PURECOV) $(LINKER) $(LINKFLAGS_D) $(PIGI_OBJS) $(LIBS) -o $@

$(BINDIR)/$(PIGI): $(PIGI)
		@echo Installing $<
		rm -f $(BINDIR)/$(PIGI)
		ln $< $(BINDIR)/$(PIGI)

else

INSTALL += $(BINDIR)/$(BASENAME) $(BINDIR)/$(BASENAME).ptrim $(BINDIR)/$(BASENAME).ptiny

$(BASENAME): $(PT_DEPEND)
	$(MAKE) FULL=1 BASENAME=$(BASENAME) $(BASENAME)

$(BASENAME).ptrim: $(PT_DEPEND)
	$(MAKE) PTRIM=1 BASENAME=$(BASENAME) $(BASENAME).ptrim

$(BASENAME).ptiny: $(PT_DEPEND)
	$(MAKE) PTINY=1 BASENAME=$(BASENAME) $(BASENAME).ptiny

$(BASENAME).ptcp: $(PT_DEPEND)
	$(MAKE) PTCP=1 BASENAME=$(BASENAME) $(BASENAME).ptcp

$(BASENAME).debug: $(PT_DEPEND)
	$(MAKE) FULL=1 BASENAME=$(BASENAME) $(BASENAME).debug

$(BASENAME).ptrim.debug: $(PT_DEPEND)
	$(MAKE) PTRIM=1 BASENAME=$(BASENAME) $(BASENAME).ptrim.debug

$(BASENAME).ptiny.debug: $(PT_DEPEND)
	$(MAKE) PTINY=1 BASENAME=$(BASENAME) $(BASENAME).ptiny.debug

$(BASENAME).ptcp.debug: $(PT_DEPEND)
	$(MAKE) PTCP=1 BASENAME=$(BASENAME) $(BASENAME).ptcp.debug


$(BASENAME).debug.purify: $(PT_DEPEND)
	$(MAKE) FULL=1 BASENAME=$(BASENAME) $(BASENAME).debug.purify

$(BASENAME).ptrim.debug.purify: $(PT_DEPEND)
	$(MAKE) PTRIM=1 BASENAME=$(BASENAME) $(BASENAME).ptrim.debug.purify

$(BASENAME).ptiny.debug.purify: $(PT_DEPEND)
	$(MAKE) PTINY=1 BASENAME=$(BASENAME) $(BASENAME).ptiny.debug.purify


$(BASENAME).debug.quantify: $(PT_DEPEND)
	$(MAKE) FULL=1 BASENAME=$(BASENAME) $(BASENAME).debug.quantify

$(BASENAME).ptrim.debug.quantify: $(PT_DEPEND)
	$(MAKE) PTRIM=1 BASENAME=$(BASENAME) $(BASENAME).ptrim.debug.quantify

$(BASENAME).ptiny.debug.quantify: $(PT_DEPEND)
	$(MAKE) PTINY=1 BASENAME=$(BASENAME) $(BASENAME).ptiny.debug.quantify

$(BASENAME).ptcp.debug.quantify: $(PT_DEPEND)
	$(MAKE) PTCP=1 BASENAME=$(BASENAME) $(BASENAME).ptcp.debug.quantify


$(BASENAME).debug.purecov: $(PT_DEPEND)
	$(MAKE) FULL=1 BASENAME=$(BASENAME) $(BASENAME).debug.purecov

$(BASENAME).ptrim.debug.purecov: $(PT_DEPEND)
	$(MAKE) PTRIM=1 BASENAME=$(BASENAME) $(BASENAME).ptrim.debug.purecov

$(BASENAME).ptiny.debug.purecov: $(PT_DEPEND)
	$(MAKE) PTINY=1 BASENAME=$(BASENAME) $(BASENAME).ptiny.debug.purecov

$(BASENAME).ptcp.debug.purecov: $(PT_DEPEND)
	$(MAKE) PTCP=1 BASENAME=$(BASENAME) $(BASENAME).ptcp.debug.purecov


$(BINDIR)/$(BASENAME): $(BASENAME) 
	$(MAKE) FULL=1 BASENAME=$(BASENAME) $(BINDIR)/$(BASENAME)

$(BINDIR)/$(BASENAME).ptrim: $(BASENAME).ptrim 
	$(MAKE) PTRIM=1 BASENAME=$(BASENAME) $(BINDIR)/$(BASENAME).ptrim

$(BINDIR)/$(BASENAME).ptiny: $(BASENAME).ptiny 
	$(MAKE) PTINY=1 BASENAME=$(BASENAME) $(BINDIR)/$(BASENAME).ptiny

$(BINDIR)/$(BASENAME).ptcp: $(BASENAME).ptcp 
	$(MAKE) PTCP=1 BASENAME=$(BASENAME) $(BINDIR)/$(BASENAME).ptcp

endif #ALLBINARIES

PTDEPEND:
	echo $(PT_DEPEND)

install: $(INSTALL)

# Build three binaries
all: makefile $(LIB) $(PIGI) $(PIGI).ptiny $(PIGI).ptrim

# Print the names of all the binaries that can be produced
echo_every_binary:
	@echo $(EVERY_BINARY)

# Build as many as 12 binaries
everything: $(REALCLEAN_STUFF)
