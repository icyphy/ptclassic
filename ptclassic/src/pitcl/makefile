# Makefile for Ptolemy/ITcl interpreter
# Version identification:
# @(#)make.template	1.83 09/04/98
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
# Date of creation: 3/4/92
# Authors: J. Buck, Christopher Hylands, Jose Luis Pino
#
# If this file's name is "make.template" and there is no makefile in
# this directory, do
#	cp make.template makefile
#	make depend
# This will add the dependencies to the makefile.

# Root of Ptolemy directory
ROOT =		../..
# Path (absolute or relative) to the object directory root
OBJDIR =	..
# True source directory
VPATH =		$(ROOT)/src/pitcl

# get configuration info
CONFIG =	$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

DIRS =	doc test

# if you want to create a custom pigiRpc, define a override.mk
# file that defines the necessary variables to pull in the domains
# you want.  See $(ROOT)/mk/ptbin.mk.  This files must at least 
# define the variable PITCL which defines the name of the pitcl executable.

-include override.mk

ifndef BASENAME
ifndef PIGI
	ALLBINARIES = 1
	BASENAME = pitcl
	PIGI = pitcl
endif
endif

# $PTOLEMY/mk/ptbin.mk checks $(PTCL) 
# If PTCL is set, we don't include Tk.
PTCL = 1
# Use libpitcl instead of libptcl
PITCL = 1

INCL = 		-I$(KERNDIR) -I$(ITCL_INCDIR) -I$(TCL_INCDIR) \
		$(PTCOMPAT_INCSPEC) $(MATLAB_INCSPEC) $(MATLABIFC_INCSPEC) \
		$(MATHEMATICA_INCSPEC) $(MATHEMATICAIFC_INCSPEC) \
		-I$(VPATH)/../ptcl

C_INCL =	$(PTCOMPAT_INCSPEC) -I$(TCL_INCDIR) -I$(TK_INCDIR)

# Sources: LIB_SRCS contains the files that make up libpitcl.so.
# NON_LIB_SRCS contains the files that do not go into libpitcl.so.
LIB_SRCS =	MatlabTcl.cc MathematicaTcl.cc PITcl.cc ptclInit.cc \
		ptclError.cc ptclGantt.cc \
		ptclDescription.cc
LIB_CSRCS = 	displayFile.c
NON_LIB_SRCS =	ptclAppInit.cc
TCL_SRCS =	gendoc.tcl
# SRCS are used by make depend, so they should be C/C++ files
SRCS =		$(LIB_SRCS) $(LIB_CSRCS) $(NON_LIB_SRCS)

EXTRA_SRCS =	$(TCL_SRCS) make.template

# Files or directories that are present, but that 'make checkjunk' 
# should not complain about
MISC_FILES =		doc test

# 'make realclean' should remove these
REALCLEAN_STUFF =	doc/codeDoc/ptcl.html

# Header files
HDRS =		displayFile.h \
		MatlabTcl.h MathematicaTcl.h PITcl.h ganttChart.h \
		ptclDescription.h

# Objects that make up the library
OBJS =		$(LIB_SRCS:.cc=.o) $(LIB_CSRCS:.c=.o)

# HPUX has a hard time handling symbols that are defined both in a static
# library and in a shared library, so, since we have different Error::
# functions for pigiRpc, ptcl, pitcl and tycho, we need to be sure that
# libpigi, libptcl, libpitcl and libtycho are all static on the hppa.
ifdef USE_CORE_STATIC_LIBS
	LIBNONSHARED =		libpitcl.a
	INSTALL = $(LIBDIR)/$(LIBNONSHARED)
else
	LIBR =		libpitcl.$(LIBSUFFIX)
	INSTALL = $(LIBDIR)/$(LIBR)
endif

PT_DEPEND += $(NON_LIB_SRCS:.cc=.o)

PIGI_OBJS += $(NON_LIB_SRCS:.cc=.o) 


# ptbin.mk has the default rule that creates a binary and the all: rule
include $(ROOT)/mk/ptbin.mk

all: $(PIGI) $(PIGI).ptiny $(PIGI).ptrim doc/codeDoc/pitcl.html

docs: doc/codeDoc/pitcl.html

doc/codeDoc/pitcl.html: PITcl.cc gendoc.tcl
	@echo "Generating html documentation from PITcl.cc"
	(cd $(VPATH); \
	  echo "set PTOLEMY $(PTOLEMY); source $(VPATH)/gendoc.tcl" | $(ITCLSH))

updatetychodocs: docs
	cp doc/codeDoc/pitcl.html ../../tycho/typt/doc/internals
# tycommon.mk does not include a rule to build make.template, so 
# we add a rule here.  We can't just use common.mk as it will no
# do things like run 'make sources' in the DIRS subdirectories
# We can't just add the makefile: make.template rule to tycommon.mk
# since most of the directories that include tycommon.mk do not have
# a make.template file.
#
# The depend, makefile and makefiles rules are from compile.mk

ifeq ($(strip $(LIBR)),)
LIBR=dummylib
endif

ifeq ($(strip $(LIBR_DEBUG)),)
LIBR_DEBUG=dummydebuglib
endif

# Rule for building a C++ library
# We use a GNU make conditional here
$(LIBR):	$(OBJS)
ifeq ($(USE_SHARED_LIBS),yes) 
	rm -f $@
	$(SHARED_LIBRARY_COMMAND) $@ $(OBJS) 
	# HPUX wants shared libraries to be 555 for performance reasons
	chmod 555 $@
else
	rm -f $@
	$(AR) cq $@ $(OBJS)
	$(RANLIB) $@
endif

$(LIBNONSHARED): $(OBJS)
	rm -f $@
	$(AR) cq $@ $^
	$(RANLIB) $@

$(LIBDIR)/$(LIBNONSHARED):	$(LIBNONSHARED) $(EXP)
	rm -f $@
	ln $(LIBNONSHARED) $(LIBDIR)

# AIX used EXP for export lists
$(EXP): $(LIBR)


DEPEND_INCL=$(INCL) $(C_INCL) $(SYSTEM_DEPEND_INCL)

depend:		$(SRCS) $(HDRS)
# We use a GNU make conditional here
ifeq ($(USE_MAKEDEPEND),yes) 
		# HPPA CC and SunSoft CC do not understand the -M option
		# so we use makedepend
		#
		# Note that makedepend does not produce dependencies
		# as GNU g++ -M, so you will probably want to run
		# 'make depend' from an PTARCH that uses
		# g++ -M instead of makedepend.  One way to do this
		# would be to do (Assuming you have g++ installed):
		# 	cd $PTOLEMY; make PTARCH=sol2 depend
		cd $(VPATH); $(DEPEND) $(DEPEND_INCL) $(SRCS)
else
		cd $(VPATH); \
                $(DEPEND) $(DEPEND_INCL) $(SRCS)|\
                        cat make.template - > makefile.n;\
                mv -f makefile.n makefile
endif

makefile:	make.template $(MDEPS)
		cd $(VPATH); rm -f makefile; cp make.template makefile; \
		$(MAKE) -f make.template sources depend
		if [ ! -f makefile ]; then ln -s $(VPATH)/makefile makefile; fi
		@echo makefile remade -- you must rerun make.
		exit 1

makefiles:	makefile

# Matlab settings
include $(ROOT)/mk/matlab.mk

# Mathematica settings
include $(ROOT)/mk/mathematica.mk

# include common definitions, rules
include $(ROOT)/mk/tycommon.mk


# Don't add anything after the next line; makedepend will zap it.
# DO NOT DELETE THIS LINE -- make depend depends on it.
MatlabTcl.o: MatlabTcl.cc ../../src/kernel/miscFuncs.h \
 ../../src/kernel/logNew.h ../../src/kernel/Error.h \
 ../../src/kernel/StringList.h ../../src/kernel/DataStruct.h \
 ../../src/kernel/type.h ../../src/utils/libexttools/MatlabIfcFuns.h \
 ../../src/compat/ptolemy/compat.h \
 ../../src/utils/libexttools/MatlabIfc.h ../../src/kernel/InfString.h \
 ../../src/utils/libexttools/MatlabIfc.h.Auto MatlabTcl.h \
 ../../src/utils/libexttools/InstanceManager.h
MathematicaTcl.o: MathematicaTcl.cc ../../src/kernel/miscFuncs.h \
 ../../src/kernel/logNew.h ../../src/kernel/Error.h \
 ../../src/kernel/StringList.h ../../src/kernel/DataStruct.h \
 ../../src/kernel/type.h ../../src/kernel/InfString.h \
 ../../src/utils/libexttools/MathematicaIfc.h \
 ../../src/utils/libptmathematica/MathematicaIfcFuns.h \
 ../../src/compat/ptolemy/compat.h MathematicaTcl.h \
 ../../src/utils/libexttools/InstanceManager.h
PITcl.o: PITcl.cc ../../src/kernel/isa.h PITcl.h \
 ../../tcltk/tcl/include/tcl.h ../../src/kernel/NamedObj.h \
 ../../src/kernel/StringList.h ../../src/kernel/miscFuncs.h \
 ../../src/kernel/logNew.h ../../src/kernel/Error.h \
 ../../src/kernel/DataStruct.h ../../src/kernel/type.h \
 ../../src/kernel/FlagArray.h ../../src/kernel/SimControl.h \
 ../../src/kernel/InterpUniverse.h ../../src/kernel/Universe.h \
 ../../src/kernel/Galaxy.h ../../src/kernel/Star.h \
 ../../src/kernel/Block.h ../../src/kernel/PortHole.h \
 ../../src/kernel/dataType.h ../../src/kernel/Particle.h \
 ../../src/kernel/Fix.h ../../src/kernel/ComplexSubset.h \
 ../../src/kernel/Attribute.h ../../src/kernel/State.h \
 ../../src/kernel/InvokeInterp.h ../../src/kernel/Target.h \
 ../../src/kernel/InterpGalaxy.h ../../src/kernel/DynamicGalaxy.h \
 ../../src/kernel/KnownBlock.h ../../src/kernel/Linker.h \
 ../../src/kernel/KnownTarget.h ../../src/kernel/Domain.h \
 ../../src/kernel/textAnimate.h ../../src/kernel/ConstIters.h \
 ../../src/kernel/Scheduler.h ../../src/kernel/InfString.h \
 ../../src/kernel/Wormhole.h ../../src/kernel/EventHorizon.h \
 ../../src/kernel/Geodesic.h ../../src/kernel/ParticleStack.h \
 ptclDescription.h MathematicaTcl.h \
 ../../src/utils/libexttools/MathematicaIfc.h \
 ../../src/utils/libptmathematica/MathematicaIfcFuns.h \
 ../../src/compat/ptolemy/compat.h \
 ../../src/utils/libexttools/InstanceManager.h MatlabTcl.h \
 ../../src/utils/libexttools/MatlabIfc.h \
 ../../src/utils/libexttools/MatlabIfcFuns.h \
 ../../src/utils/libexttools/MatlabIfc.h.Auto
ptclInit.o: ptclInit.cc ../../src/compat/ptolemy/compat.h \
 ../../src/pitcl/../ptcl/PTcl.h ../../src/kernel/NamedObj.h \
 ../../src/kernel/StringList.h ../../src/kernel/miscFuncs.h \
 ../../src/kernel/logNew.h ../../src/kernel/Error.h \
 ../../src/kernel/DataStruct.h ../../src/kernel/type.h \
 ../../src/kernel/isa.h ../../src/kernel/FlagArray.h \
 ../../src/kernel/SimControl.h
ptclError.o: ptclError.cc ../../src/pitcl/../ptcl/PTcl.h \
 ../../src/kernel/NamedObj.h ../../src/kernel/StringList.h \
 ../../src/kernel/miscFuncs.h ../../src/kernel/logNew.h \
 ../../src/kernel/Error.h ../../src/kernel/DataStruct.h \
 ../../src/kernel/type.h ../../src/kernel/isa.h \
 ../../src/kernel/FlagArray.h ../../src/kernel/SimControl.h
ptclGantt.o: ptclGantt.cc ../../src/pitcl/../ptcl/PTcl.h \
 ../../src/kernel/NamedObj.h ../../src/kernel/StringList.h \
 ../../src/kernel/miscFuncs.h ../../src/kernel/logNew.h \
 ../../src/kernel/Error.h ../../src/kernel/DataStruct.h \
 ../../src/kernel/type.h ../../src/kernel/isa.h \
 ../../src/kernel/FlagArray.h ../../src/kernel/SimControl.h \
 ganttChart.h
ptclDescription.o: ptclDescription.cc ../../src/kernel/PortHole.h \
 ../../src/kernel/NamedObj.h ../../src/kernel/StringList.h \
 ../../src/kernel/miscFuncs.h ../../src/kernel/logNew.h \
 ../../src/kernel/Error.h ../../src/kernel/DataStruct.h \
 ../../src/kernel/type.h ../../src/kernel/isa.h \
 ../../src/kernel/FlagArray.h ../../src/kernel/dataType.h \
 ../../src/kernel/Particle.h ../../src/kernel/Fix.h \
 ../../src/kernel/ComplexSubset.h ../../src/kernel/Attribute.h \
 ../../src/kernel/Star.h ../../src/kernel/Block.h \
 ../../src/kernel/State.h ../../src/kernel/InvokeInterp.h \
 ../../src/kernel/Galaxy.h ../../src/kernel/GalIter.h \
 ../../src/kernel/pt_fstream.h
displayFile.o: displayFile.c displayFile.h
ptclAppInit.o: ptclAppInit.cc ../../src/kernel/Linker.h
