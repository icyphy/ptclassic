# Makefile to build ptcl, tycho and pigiRpc binaries
# @(#)ptbin.mk	1.83	02/22/99
#
# Copyright (c) 1990-1998 The Regents of the University of California.
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
	VERSION =	0.7.2devel.$(BASENAME)
endif

ifdef ALLBINARIES
ifndef PTRIM
ifndef PTINY
	FULL =		1
endif
endif
endif

ifdef PTCL
# To build ptcl with Tk, follow the steps below: 
# 1. Recompile Star.cc:
#    cd $PTOLEMY/obj.$PTARCH/kernel
#    rm -f Star.o
#    make USERFLAGS="-DPT_PTCL_WITH_TK -I$PTOLEMY/tcltk/tcl/include -I/usr/openwin/include" install
#
# 2. Create the following  $PTOLEMY/obj.$PTARCH/ptcl/override.mk file
# --start--
#     GPP_FLAGS += -DPT_PTCL_WITH_TK -I../../src/ptklib -I$PTOLEMY/tcltk/tcl/include -I/usr/openwin/include"
#     PTCL_WITH_TK = 1
#     TK = 1
#     # If you are building under Unix,
#     # copy src/tcltk/tk8.0.5pt/generic/tkConsole.c to src/ptcl and then
#     # uncomment the next two lines:
#     #LIBS += tkConsole.o
#     #LIBFILES += tkConsole.o
# --end--
#
# 3. Rebuild ptcl:
#    cd $PTOLEMY/obj.$PTARCH/ptcl
#    make clean
#    make
#		
ifndef PTCL_WITH_TK
	TK =
endif
endif

# Note that some of these settings will be filtered out by stars.mk
ifdef FULL
	PIGI = 		$(BASENAME)$(BINARY_EXT)
	VERSION_DESC =	'With All Common Domains'
	BDF =		1
	C50 =		1
	CGFULL =	1
	CG56 =		1
	CGCFULL =	1
	DDF =		1
	DEFULL =	1
	FSM = 		1
	HOF =		1
	IPUS =		1
	MDSDF =		1
	PN =		1
	ACS =		1
	SDFFULL =	1
	SR =		1
	SRCGC =		1
	VHDLFULL =	1
	VHDLB =		1
endif

ifdef ACSBIN
	PIGI=		$(BASENAME).acs$(BINARY_EXT)
	VERSION_DESC =	'With Adaptive Computing Systems (ACS) Domain only'
	ACS =		1
endif

ifdef PTINY
	PIGI=		$(BASENAME).ptiny$(BINARY_EXT)
	VERSION_DESC =	'With SDF (no image stars) and DE domains only'
	DE =		1
	HOF =		1
	SDF =		1
	SDFTK =		1
	SDFDFM =	1
	SDFDSP =	1
	SDFMATRIX =	1
	SDFMATLAB =	1
endif

ifdef PTRIM
	PIGI =		$(BASENAME).ptrim$(BINARY_EXT)
	VERSION_DESC =	'With SDF, DE, BDF, DDF and CGC domains only'
	BDF =		1
	CGCFULL =	1
	CGFULL =	1
	DDF =		1
	DE =		1
	HOF =		1
	SDFFULL =	1
endif

# build up the list of dependent directories and libraries
include $(ROOT)/mk/stars.mk

# matlab.mk and mathematica.mk check these vars before traversing the path
NEED_MATLABDIR = 	1
NEED_MATHEMATICADIR = 	1

PIGI_OBJS += $(STARS) $(TARGETS) $(MISC_OBJS)

PIGI_BINARIES = 	$(BASENAME) \
			$(BASENAME).debug$(BINARY_EXT)\
			$(BASENAME).debug.purify$(BINARY_EXT) \
			$(BASENAME).debug.quantify$(BINARY_EXT) \
			$(BASENAME).debug.purecov$(BINARY_EXT)

EVERY_BINARY= $(PIGI_BINARIES) \
		$(BASENAME).acs \
		$(BASENAME).acs.debug \
		$(BASENAME).acs.debug.purify \
		$(BASENAME).acs.debug.quantify \
		$(BASENAME).acs.debug.purecov \
		$(BASENAME).ptiny \
		$(BASENAME).ptiny.debug \
		$(BASENAME).ptiny.debug.purify \
		$(BASENAME).ptiny.debug.quantify \
		$(BASENAME).ptiny.debug.purecov \
		$(BASENAME).ptrim \
		$(BASENAME).ptrim.debug \
		$(BASENAME).ptrim.debug.purify \
		$(BASENAME).ptrim.debug.quantify \
		$(BASENAME).ptrim.debug.purecov

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
# On certain archs (hppa9), we can run strip -x and get a smaller binary
# Note that doing a full strip on a binary will disable incremental linking
# 
# This is the defautl target
$(PIGI): $(PT_DEPEND) $(ADD_OBJS)
	echo char '*gVersion = "Version:' $(VERSION) \
		$(VERSION_DESC) \
		'%created' `date` '";' | sed 's/%/\\n/g' > version.c
	echo "char DEFAULT_DOMAIN[] = \"$(DEFAULT_DOMAIN)\";" \
		>> version.c
	$(CC) -c version.c
	$(PURELINK) $(LINKER) $(LINKFLAGS) $(PIGI_OBJS) $(LIBS) -o $@
	$(STRIP_DEBUG) $@

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
		rm -f $@
		ln $< $@

else

INSTALL += $(BINDIR)/$(BASENAME)$(BINARY_EXT) \
		$(BINDIR)/$(BASENAME).ptrim$(BINARY_EXT) \
		$(BINDIR)/$(BASENAME).ptiny$(BINARY_EXT) \
		$(BINDIR)/$(BASENAME).acs$(BINARY_EXT)


# If the user wants to build anything but a full pigiRpc or ptcl, then
# we want to be careful not to depend on the full set of dependencies.
# The way we do this is that we declare everything but $(BASENAME)
# to be .PHONY so that they will always be built.  Since the rules
# for these binaries invokes a submake process, the new submake process
# will have the proper dependencies for the specific binary we are building.
.PHONY: $(BASENAME).ptrim$(BINARY_EXT) \
	$(BASENAME).ptiny$(BINARY_EXT) \
	$(BASENAME).ptrim.debug$(BINARY_EXT) \
	$(BASENAME).ptiny.debug$(BINARY_EXT) \
	$(BASENAME).ptrim.debug.purify$(BINARY_EXT) \
	$(BASENAME).ptiny.debug.purify$(BINARY_EXT) \
	$(BASENAME).ptrim.debug.quantify$(BINARY_EXT) \
	$(BASENAME).ptiny.debug.quantify$(BINARY_EXT) \
	$(BASENAME).ptrim.debug.purecov$(BINARY_EXT) \
	$(BASENAME).ptiny.debug.purecov$(BINARY_EXT) \
	$(BASENAME).acs$(BINARY_EXT) \
	$(BASENAME).acs.debug$(BINARY_EXT) \
	$(BASENAME).acs.debug.purify$(BINARY_EXT) \
	$(BASENAME).acs.debug.quantify$(BINARY_EXT) \
	$(BASENAME).acs.debug.purecov

# The .ptrim and .ptiny files below should not depend on $(PT_DEPEND), or
# else we must have all the libs installed to build ptrim and ptiny, even
# though ptrim and ptiny do not use all the libs.

$(BASENAME)$(BINARY_EXT): $(PT_DEPEND)
	$(MAKE) FULL=1 BASENAME=$(BASENAME) $@

$(BASENAME).ptrim$(BINARY_EXT): 
	$(MAKE) PTRIM=1 BASENAME=$(BASENAME) $@

$(BASENAME).ptiny$(BINARY_EXT):
	$(MAKE) PTINY=1 BASENAME=$(BASENAME) $@

$(BASENAME).acs$(BINARY_EXT):
	$(MAKE) ACSBIN=1 BASENAME=$(BASENAME) DEFAULT_DOMAIN=ACS $@



$(BASENAME).debug$(BINARY_EXT): $(PT_DEPEND)
	$(MAKE) FULL=1 BASENAME=$(BASENAME) $@

$(BASENAME).ptrim.debug$(BINARY_EXT): 
	$(MAKE) PTRIM=1 BASENAME=$(BASENAME) $@

$(BASENAME).ptiny.debug$(BINARY_EXT): 
	$(MAKE) PTINY=1 BASENAME=$(BASENAME) $@

$(BASENAME).acs.debug$(BINARY_EXT): 
	$(MAKE) ACSBIN=1 BASENAME=$(BASENAME) DEFAULT_DOMAIN=ACS $@



$(BASENAME).debug.purify$(BINARY_EXT): $(PT_DEPEND)
	$(MAKE) FULL=1 BASENAME=$(BASENAME) INCLUDE_PN_DOMAIN=no $@

$(BASENAME).ptrim.debug.purify$(BINARY_EXT):
	$(MAKE) PTRIM=1 BASENAME=$(BASENAME) $@

$(BASENAME).ptiny.debug.purify$(BINARY_EXT):
	$(MAKE) PTINY=1 BASENAME=$(BASENAME) $@

$(BASENAME).acs.debug.purify$(BINARY_EXT):
	$(MAKE) ACSBIN=1 BASENAME=$(BASENAME) DEFAULT_DOMAIN=ACS $@



$(BASENAME).debug.quantify$(BINARY_EXT): $(PT_DEPEND)
	$(MAKE) FULL=1 BASENAME=$(BASENAME) INCLUDE_PN_DOMAIN=no $@

$(BASENAME).ptrim.debug.quantify$(BINARY_EXT): 
	$(MAKE) PTRIM=1 BASENAME=$(BASENAME) $@

$(BASENAME).ptiny.debug.quantify$(BINARY_EXT): 
	$(MAKE) PTINY=1 BASENAME=$(BASENAME) $@

$(BASENAME).acs.debug.quantify$(BINARY_EXT): 
	$(MAKE) ACSBIN=1 BASENAME=$(BASENAME) DEFAULT_DOMAIN=ACS $@



$(BASENAME).debug.purecov$(BINARY_EXT): $(PT_DEPEND)
	$(MAKE) FULL=1 BASENAME=$(BASENAME) INCLUDE_PN_DOMAIN=no $@

$(BASENAME).ptrim.debug.purecov$(BINARY_EXT): 
	$(MAKE) PTRIM=1 BASENAME=$(BASENAME) $@

$(BASENAME).ptiny.debug.purecov$(BINARY_EXT): 
	$(MAKE) PTINY=1 BASENAME=$(BASENAME) $@

$(BASENAME).acs.debug.purecov$(BINARY_EXT): 
	$(MAKE) ACSBIN=1 BASENAME=$(BASENAME) DEFAULT_DOMAIN=ACS $@


$(BINDIR)/$(BASENAME)$(BINARY_EXT): $(BASENAME)$(BINARY_EXT)
	$(MAKE) FULL=1 BASENAME=$(BASENAME) $@

$(BINDIR)/$(BASENAME).ptrim$(BINARY_EXT): $(BASENAME).ptrim$(BINARY_EXT) 
	$(MAKE) PTRIM=1 BASENAME=$(BASENAME) $@

$(BINDIR)/$(BASENAME).ptiny$(BINARY_EXT): $(BASENAME).ptiny$(BINARY_EXT) 
	$(MAKE) PTINY=1 BASENAME=$(BASENAME) $@

$(BINDIR)/$(BASENAME).acs$(BINARY_EXT): $(BASENAME).acs$(BINARY_EXT)
	$(MAKE) ACSBIN=1 BASENAME=$(BASENAME) DEFAULT_DOMAIN=ACS $@

endif #ALLBINARIES


install: $(INSTALL)

# Build four binaries
all: makefile $(PTLIB) $(PIGI) $(PIGI).acs $(PIGI).ptiny $(PIGI).ptrim

# Print the names of all the binaries that can be produced
echo_every_binary:
	@echo $(EVERY_BINARY)

# Build as many as 12 binaries
everything: $(REALCLEAN_STUFF)

# You might find it helpful to use the PTDEPEND rule to debug dependencies:
# make ACSBIN=1 BASENAME=pigiRpc DEFAULT_DOMAIN=ACS PTDEPEND
# First, find out what the submake command is that build the binary:
#   cxh@carson 153% make pigiRpc.acs
#   make ACSBIN=1 BASENAME=pigiRpc DEFAULT_DOMAIN=ACS \
# 	  pigiRpc.acs 
#
# Then use those same settings, but use the PTDEPEND target: 
#   cxh@carson 154% make ACSBIN=1 BASENAME=pigiRpc DEFAULT_DOMAIN=ACS PTDEPEND
#
PTDEPEND:
	echo $(PT_DEPEND)

