# Makefile to build ptcl, tycho and pigiRpc biPTRIM$Id$

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
# Programmer:  Christopher Hylands
# Based on pigiRpc/makefile, originally by Joe Buck,
#  with modifications from just about everyone on the Ptolemy Team.
#
# This makefile is included by makefiles that build standalone binaries.
# By having one common makefile, we don't need to update lots of other
# makefiles.  Currently, ptcl, pigiRpc and tycho use this file

# This makefile expects the following variables to be defined:
#  $(BASENAME), $(MAIN), $(SPECIAL_LIBS), $(SPECIAL_LIBS_DEPENDS)
#  $(NEED_PALETTES), $(NEED_HOF), $(NEED_GRAPHICS), $(MAIN_DEPEND)
# For example declarations, see $(PTOLEMY)/src/pigiRpc/makefile

# Todo:
# Check out with compile-sdf
# Build Shared itcl libraries
# Include Init_itcl in pigi and ptcl
# Test out pigiRpc.cp
# Make a non-shared library

# The version number.
VERSION =	0.6devel.$(BASENAME)

# Names of binaries.
PTINY	=	$(BASENAME).ptiny
PTRIM	=	$(BASENAME).ptrim
PIGI	=	$(BASENAME)

# Below we have a few GNU make conditionals used to differentiate the binaries.
ifeq ($(NEED_PALETTES),yes)
# We need palette files here
PTINY_PALETTES =	defpalettes-ptiny.o
PTRIM_PALETTES =	defpalettes-ptrim.o
PIGI_PALETTES =		defpalettes-pigi.o
else
PTINY_PALETTES =
PTRIM_PALETTES =
PIGI_PALETTES =
endif

ifeq ($(NEED_HOF),no)
# Don't need Higher Order Functions.  ptcl does not need HOF
HOF_LIBS =
endif

ifeq ($(NEED_GRAPHICS),no)
# Don't need graphics.  ptcl does not have tk
ITK_LIBSPEC =
TK_LIBSPEC =
X11_LIBSPEC =
PTINY_TCL_STAR_LIBS =
PTRIM_TCL_STAR_LIBS =
TCL_STAR_LIBS =
endif

# Below we define variables for each class of binary

# Define libraries to be used at link time
GENERIC_LIBS =	$(HOF_LIBS) \
		$(SPECIAL_LIBS) \
		$(ITCL_LIBSPEC) $(ITK_LIBSPEC) $(TK_LIBSPEC) $(TCL_LIBSPEC) \
		$(X11_LIBSPEC) $(SYSLIBS) $(LIB_FLUSH_CACHE)
PTINY_LIBS = 	$(PTINY_STAR_LIBS) $(PTINY_TCL_STAR_LIBS) $(GENERIC_LIBS)
PTRIM_LIBS = 	$(PTRIM_STAR_LIBS) $(PTRIM_TCL_STAR_LIBS) $(GENERIC_LIBS)
PIGI_LIBS = 	$(STAR_LIBS) $(TCL_STAR_LIBS) $(THREAD_LIBS) $(GENERIC_LIBS)
PTCP_LIBS=	$(PTCP_STAR_LIBS) $(LWP_LIBS) $(GENERIC_LIBS)

# Define what stars you want to include
ifeq ($(USE_SHARED_LIBS),yes) 
# If we are using shared libraries, then we need not have .o files that
# bring in the stars from the libraries.
PTINY_STARS=
PTRIM_STARS=
ALLSTARS=
PTCP_STARS=
else
PTINY_STARS = $(PTINY_SDFSTARS) \
		$(DESTARS) \
		$(PTINY_TCLSTARS) $(HOFSTARS)
PTRIM_STARS = $(SDFSTARS) \
		$(CGCSTARS) $(CGSTARS) \
		$(DDFSTARS) $(DESTARS) $(BDFSTARS) \
		$(PTRIM_TCLSTARS) $(HOFSTARS)
ALLSTARS = $(THREAD_STARS) $(OPTIONAL_STARS) $(SDFSTARS) \
		$(CGCSTARS) $(CG96STARS) $(CG56STARS) $(SilageSTARS) \
		$(VHDLFSTARS) $(VHDLBSTARS) $(CGSTARS) \
		$(DDFSTARS) $(THORSTARS) $(DESTARS) $(BDFSTARS) \
		$(MDSDFSTARS) $(IPUSSTARS) \
		$(TCLSTARS) $(HOFSTARS)

PTCP_STARS = $(THORSTARS) $(DESTARS) $(CPSTARS)
endif # $(USE_SHARED_LIBRARYS)

# Define all the objects that end up in the binary
PTINY_OBJS =	$(MAIN) $(PTINY_PALETTES) $(PTINYY_STARS) $(PTINY_TARGETS)
PTRIM_OBJS =	$(MAIN) $(PTRIM_PALETTES) $(PTRIM_STARS) $(PTRIM_TARGETS)
PIGI_OBJS =	$(MAIN) $(PIGI_PALETTES) $(PIGI_STARS) $(PIGI_TARGETS)
PTCP_OBJS =	$(MAIN) $(PTCP_PALETTES) $(PTCP_STARS) $(PTCP_TARGETS)

# Define what the binary is dependent on
GENERIC_DEPEND = $(MAIN_DEPEND) $(SPECIAL_LIB_DEPEND) $(LIBDIR)/libptolemy.a
PTINY_DEPEND =  $(GENERIC_DEPEND) $(PTINY_PALETTES) \
			$(PTINY_STAR_LIBFILES) $(PTINY_STARS) $(PTINY_TARGETS)
PTRIM_DEPEND =  $(GENERIC_DEPEND) $(PTRIM_PALETTES) \
			$(PTRIM_STAR_LIBFILES) $(PTRIM_STARS) $(PTRIM_TARGETS)
PIGI_DEPEND =	$(GENERIC_DEPEND) $(PIGI_PALETTES) \
			$(PIGI_STAR_LIBFILES) $(PIGI_STARS) $(PIGI_TARGETS)
PTCP_DEPEND =  	$(GENERIC_DEPEND) $(PTCP_PALETTES) \
			$(PTCP_STAR_LIBFILES) $(PTCP_STARS) $(PTCP_TARGETS)

# Names of binaries to produce
PTINY_BINARIES = 	$(PTINY) $(PTINY).debug $(PTINY).debug.purify \
			$(PTINY).debug.quantify $(PTINY).debug.purecov
PTRIM_BINARIES = 	$(PTRIM) $(PTRIM).debug $(PTRIM).debug.purify \
			$(PTRIM).debug.quantify $(PTRIM).debug.purecov
PIGI_BINARIES = 	$(PIGI) $(PIGI).debug $(PIGI).debug.purify \
			$(PIGI).debug.quantify $(PIGI).debug.purecov
ALL_BINARIES = $(PTINY_BINARIES) $(PTRIM_BINARIES) $(PIGI_BINARIES) \
			$(PIGI).cp
# extra stuff to delete with 'make realclean'
REALCLEAN_STUFF=	$(ALL_BINARIES)



####################################################################

# Variable definitions are above this point.  Rules are below this point.

# We have three main binaries, ptiny, ptrim and pigi.
# Each binary has the following versions:
#		 vanilla, debug, quantify, purify, purecov
# At the end we define a few special cases

# See the calling makefile for the all: rule

# Build vanilla and debug versions
everything: makefile $(PTINY) $(PTRIM) $(PIGI) \
		$(PTINY).debug $(PTRIM).debug $(PIGI).debug

# Build absolutely everything, including Pure Inc versions
ptitanic: makefile $(ALL_BINARIES)


####################################################################
# PTINY versions

# Small version of Ptolemy, with only sdf domain (no image stars) and
# de domain.
$(PTINY): $(PTINY_DEPEND)
	echo char '*gVersion = "Version:' $(VERSION) \
		'(sdf (no image stars) and de only)' \
		'%created' `date` '";' | sed 's/%/\\n/g' > version.c
	$(CC) -c version.c
	$(PURELINK) $(LINKER) $(LINKFLAGS) $(PTINY_OBJS) $(PTINY_LIBS) \
		version.o -o $@


# Same, with debugging symbols.
$(PTINY).debug: $(PTINY_DEPEND)
	$(CC) -c version.c
	echo char '*gVersion = "Version:' $(VERSION) \
		'(sdf (no image stars) and de only,' \
		'%created' `date` '";' | sed 's/%/\\n/g' > version.c
	$(PURELINK) $(LINKER) $(LINKFLAGS_D) $(PTINY_OBJS) $(PTINY_LIBS) \
		version.o -o $@

# Same, with debugging symbols and purify
$(PTINY).debug.purify: $(PTINY_DEPEND)
	$(CC) -c version.c
	echo char '*gVersion = "Version:' $(VERSION) \
		'(sdf (no image stars) and de only,' \
		'% with debug symbols and purify)' \
		'%created' `date` '";' | sed 's/%/\\n/g' > version.c
	$(PURIFY) $(LINKER) $(LINKFLAGS_D) $(PTINY_OBJS) $(PTINY_LIBS) \
		version.o -o $@

# Same, with quantify, for profiling.
$(PTINY).debug.quantify: $(PTINY_DEPEND)
	$(CC) -c version.c
	echo char '*gVersion = "Version:' $(VERSION) \
		'(sdf (no image stars) and de only,' \
		'% with debug symbols and quantify)' \
		'%created' `date` '";' | sed 's/%/\\n/g' > version.c
	$(QUANTIFY) $(LINKER) $(LINKFLAGS_D) $(PTINY_OBJS) $(PTINY_LIBS) \
		version.o -o $@

# Same, with purecov, for code coverage measurements.
$(PTINY).debug.purecov: $(PTINY_DEPEND)
	$(CC) -c version.c
	echo char '*gVersion = "Version:' $(VERSION) \
		'(sdf (no image stars) and de only,' \
		'% with debug symbols and purecov)' \
		'%created' `date` '";' | sed 's/%/\\n/g' > version.c
	$(PURECOV) $(LINKER) $(LINKFLAGS_D) $(PTINY_OBJS) $(PTINY_LIBS) \
		version.o -o $@


####################################################################
# PTRIM versions

# Small version of Ptolemy, with only sdf domain (no image stars) and
# de domain.
$(PTRIM): $(PTRIM_DEPEND)
	echo char '*gVersion = "Version:' $(VERSION) \
		'(sdf, ddf, bdf, de, and cgc (minus parallel targets))' \
		'%created' `date` '";' | sed 's/%/\\n/g' > version.c
	$(CC) -c version.c
	$(PURELINK) $(LINKER) $(LINKFLAGS) $(PTRIM_OBJS) $(PTRIM_LIBS) \
		version.o -o $@


# Same, with debugging symbols.
$(PTRIM).debug: $(PTRIM_DEPEND)
	$(CC) -c version.c
	echo char '*gVersion = "Version:' $(VERSION) \
		'(sdf, ddf, bdf, de, and cgc (minus parallel targets)' \
		'% with debug symbols' \
		'%created' `date` '";' | sed 's/%/\\n/g' > version.c
	$(PURELINK) $(LINKER) $(LINKFLAGS_D) $(PTRIM_OBJS) $(PTRIM_LIBS) \
		version.o -o $@

# Same, with debugging symbols and purify
$(PTRIM).debug.purify: $(PTRIM_DEPEND)
	$(CC) -c version.c
	echo char '*gVersion = "Version:' $(VERSION) \
		'(sdf, ddf, bdf, de, and cgc (minus parallel targets)' \
		'% with debug symbols and purify)' \
		'%created' `date` '";' | sed 's/%/\\n/g' > version.c
	$(PURIFY) $(LINKER) $(LINKFLAGS_D) $(PTRIM_OBJS) $(PTRIM_LIBS) \
		version.o -o $@

# Same, with quantify, for profiling.
$(PTRIM).debug.quantify: $(PTRIM_DEPEND)
	$(CC) -c version.c
	echo char '*gVersion = "Version:' $(VERSION) \
		'(sdf, ddf, bdf, de, and cgc (minus parallel targets)' \
		'% with debug symbols and quantify)' \
		'%created' `date` '";' | sed 's/%/\\n/g' > version.c
	$(QUANTIFY) $(LINKER) $(LINKFLAGS_D) $(PTRIM_OBJS) $(PTRIM_LIBS) \
		version.o -o $@

# Same, with purecov, for code coverage measurements.
$(PTRIM).debug.purecov: $(PTRIM_DEPEND)
	$(CC) -c version.c
	echo char '*gVersion = "Version:' $(VERSION) \
		'(sdf, ddf, bdf, de, and cgc (minus parallel targets)' \
		'% with debug symbols and purecov)' \
		'%created' `date` '";' | sed 's/%/\\n/g' > version.c
	$(PURECOV) $(LINKER) $(LINKFLAGS_D) $(PTRIM_OBJS) $(PTINY_LIBS) \
		version.o -o $@


####################################################################
# PIGI versions

# Small version of Ptolemy, with only sdf domain (no image stars) and
# de domain.
$(PIGI): $(PIGI_DEPEND)
	echo char '*gVersion = "Version:' $(VERSION) \
		'%created' `date` '";' | sed 's/%/\\n/g' > version.c
	$(CC) -c version.c
	$(PURELINK) $(LINKER) $(LINKFLAGS) $(PIGI_OBJS) $(PIGI_LIBS) \
		version.o -o $@


# Same, with debugging symbols.
$(PIGI).debug: $(PIGI_DEPEND)
	$(CC) -c version.c
	echo char '*gVersion = "Version:' $(VERSION) \
		'(with debug symbols)' \
		'%created' `date` '";' | sed 's/%/\\n/g' > version.c
	$(PURELINK) $(LINKER) $(LINKFLAGS_D) $(PIGI_OBJS) $(PIGI_LIBS) \
		version.o -o $@

# Same, with debugging symbols and purify
$(PIGI).debug.purify: $(PIGI_DEPEND)
	$(CC) -c version.c
	echo char '*gVersion = "Version:' $(VERSION) \
		'(with debug symbols and purify)' \
		'%created' `date` '";' | sed 's/%/\\n/g' > version.c
	$(PURIFY) $(LINKER) $(LINKFLAGS_D) $(PIGI_OBJS) $(PIGI_LIBS) \
		version.o -o $@

# Same, with quantify, for profiling.
$(PIGI).debug.quantify: $(PIGI_DEPEND)
	$(CC) -c version.c
	echo char '*gVersion = "Version:' $(VERSION) \
		'(with debug symbols and quantify)' \
		'%created' `date` '";' | sed 's/%/\\n/g' > version.c
	$(QUANTIFY) $(LINKER) $(LINKFLAGS_D) $(PIGI_OBJS) $(PIGI_LIBS) \
		version.o -o $@

# Same, with purecov, for code coverage measurements.
$(PIGI).debug.purecov: $(PIGI_DEPEND)
	$(CC) -c version.c
	echo char '*gVersion = "Version:' $(VERSION) \
		'(with debug symbols and purecov)' \
		'%created' `date` '";' | sed 's/%/\\n/g' > version.c
	$(PURECOV) $(LINKER) $(LINKFLAGS_D) $(PIGI_OBJS) $(PIGI_LIBS) \
		version.o -o $@



##########################################################################
# Small version of Ptolemy with only thor, de, and cp domains.
# All dataflow domains are intentionally excluded to help prevent
# excessive core dumps.  Only available on the sun4 architecture.
ifeq ($(PTARCH),sun4)
$(PIGI).cp: $(PTCP_DEPEND)
	echo char '*gVersion = "Version:' $(VERSION) \
		'(with CP domain)' \
		'%created' `date` '";' | sed 's/%/\\n/g' > version.c
	$(CC) -c version.c
	$(PURELINK) $(LINKER) $(LINKFLAGS) $(PTCP_OBJS) $(PTCP_LIBS) \
		version.o -o $@
	$(STRIP_DEBUG) $@
else
$(PIGI).cp:
	@echo "The CP domain is currently only supported on the sun4 platform"
endif
