# Makefile for Ptolemy C Code Generation uniprocessor targets
# @(#)make.template	1.3	10/01/96
# Copyright (c) 1990-1997 The Regents of the University of California.
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
# Code generation targets

# Root of Ptolemy directory
ROOT =		../../../../..
# True source directory
VPATH = 	$(ROOT)/src/domains/cgc/targets/uniproc

# get configuration info
CONFIG =	$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

SDFKERNEL = 	$(ROOT)/src/domains/sdf/kernel
BDFKERNEL = 	$(ROOT)/src/domains/bdf/kernel
CGKERNEL = 	$(ROOT)/src/domains/cg/kernel
CGCKERNEL = 	$(ROOT)/src/domains/cgc/kernel
CGCSTARS = 	$(ROOT)/src/domains/cgc/stars
PARDIR = 	$(ROOT)/src/domains/cg/parScheduler
CGTARDIR =	$(ROOT)/src/domains/cg/targets
DLDIR =		$(ROOT)/src/domains/cg/dlScheduler
MACRODIR =	$(ROOT)/src/domains/cg/macroScheduler
CGDDFDIR =	$(ROOT)/src/domains/cg/ddfScheduler


INCL=-I$(CGCKERNEL) -I$(CGKERNEL) -I$(BDFKERNEL) -I$(SDFKERNEL) -I$(KERNDIR) \
	-I$(PARDIR) -I$(CGTARDIR) -I$(CGCSTARS)  -I$(DLDIR) -I$(MACRODIR) \
	-I$(CGDDFDIR)


PL_SRCS = \
	CGCSDFBase.pl \
	CGCSDFSend.pl \
	CGCSDFReceive.pl

PLSRC = $(PL_SRCS:.pl=.cc)

SRCS =	CreateSDFStar.cc \
	CGCBDFTarget.cc \
	CGCMakefileTarget.cc \
	$(PLSRC) 

EXTRA_SRCS = $(PL_SRCS)

HDRS=$(SRCS:.cc=.h)

OBJS = $(SRCS:.cc=.o)

ifeq ($(USE_SHARED_LIBS),yes) 

# If we are building shared libraries, then we can put all the targets in
# a shared library.
PTLIB = 		libcgctargets.$(LIBSUFFIX)
all:		$(SRCS) makefile $(OBJS) $(PTLIB)
install:	all $(LIBDIR)/$(PTLIB)

else
PTLIB=none	# required by common.mk
all:		makefile $(OBJS)
# "make install" puts things in the official place
install:	all
		@echo Installation complete.
endif

# include common definitions, rules
include $(ROOT)/mk/common.mk

# Don't add anything after the next line; makedepend will zap it.
# DO NOT DELETE THIS LINE -- make depend depends on it.

CreateSDFStar.o: CreateSDFStar.cc \
 ../../../../../src/domains/cg/kernel/CGUtilities.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/Domain.h \
 ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/domains/cgc/kernel/CGCPortHole.h \
 ../../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../../src/kernel/DynDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/PrecisionState.h CreateSDFStar.h \
 ../../../../../src/domains/cgc/kernel/CGCTarget.h \
 ../../../../../src/domains/cg/kernel/HLLTarget.h \
 ../../../../../src/domains/cg/kernel/CGTarget.h \
 ../../../../../src/kernel/Target.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/domains/cg/kernel/SymbolList.h \
 ../../../../../src/domains/cg/kernel/NamedList.h \
 ../../../../../src/kernel/SimControl.h \
 ../../../../../src/domains/cg/kernel/CodeStreamList.h \
 ../../../../../src/domains/cg/kernel/CodeStream.h \
 ../../../../../src/domains/cg/kernel/UniqueStringList.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/cg/kernel/ImplementationCost.h \
 ../../../../../src/domains/cg/kernel/ConversionTable.h \
 ../../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../../src/kernel/Scheduler.h \
 ../../../../../src/kernel/Galaxy.h \
 ../../../../../src/kernel/GalIter.h \
 ../../../../../src/kernel/StringArrayState.h \
 ../../../../../src/kernel/HashTable.h \
 ../../../../../src/kernel/KnownTarget.h \
 ../../../../../src/kernel/KnownBlock.h \
 ../../../../../src/kernel/Linker.h \
 ../../../../../src/kernel/InterpGalaxy.h \
 ../../../../../src/kernel/DynamicGalaxy.h CGCSDFReceive.h \
 CGCSDFBase.h ../../../../../src/domains/cgc/kernel/CGCStar.h \
 ../../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../../src/kernel/DynDFStar.h \
 ../../../../../src/domains/cg/kernel/CodeBlock.h CGCSDFSend.h \
 ../../../../../src/kernel/Geodesic.h \
 ../../../../../src/kernel/ParticleStack.h
CGCBDFTarget.o: CGCBDFTarget.cc CGCBDFTarget.h \
 ../../../../../src/domains/cgc/kernel/CGCTarget.h \
 ../../../../../src/domains/cg/kernel/HLLTarget.h \
 ../../../../../src/domains/cg/kernel/CGTarget.h \
 ../../../../../src/kernel/Target.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/domains/cg/kernel/SymbolList.h \
 ../../../../../src/domains/cg/kernel/NamedList.h \
 ../../../../../src/kernel/SimControl.h \
 ../../../../../src/domains/cg/kernel/CodeStreamList.h \
 ../../../../../src/domains/cg/kernel/CodeStream.h \
 ../../../../../src/domains/cg/kernel/UniqueStringList.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/cg/kernel/ImplementationCost.h \
 ../../../../../src/domains/cg/kernel/ConversionTable.h \
 ../../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../../src/kernel/Scheduler.h \
 ../../../../../src/kernel/Galaxy.h \
 ../../../../../src/kernel/GalIter.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/StringArrayState.h \
 ../../../../../src/kernel/HashTable.h \
 ../../../../../src/domains/bdf/kernel/BDFCluster.h \
 ../../../../../src/domains/bdf/kernel/BDFStar.h \
 ../../../../../src/kernel/DynDFStar.h \
 ../../../../../src/domains/bdf/kernel/BDFPortHole.h \
 ../../../../../src/kernel/DynDFPortHole.h \
 ../../../../../src/kernel/DynamicGalaxy.h \
 ../../../../../src/domains/bdf/kernel/BDFScheduler.h \
 ../../../../../src/domains/bdf/kernel/BDFBool.h \
 ../../../../../src/kernel/KnownTarget.h \
 ../../../../../src/domains/cgc/kernel/CGCPortHole.h \
 ../../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../../src/kernel/PrecisionState.h
CGCMakefileTarget.o: CGCMakefileTarget.cc \
 ../../../../../src/kernel/pt_fstream.h \
 ../../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/logNew.h \
 ../../../../../src/domains/cg/kernel/CGUtilities.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/Error.h \
 ../../../../../src/domains/cgc/kernel/CGCStar.h \
 ../../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../../src/kernel/DynDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../../src/kernel/DynDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/domains/cg/kernel/CGTarget.h \
 ../../../../../src/kernel/Target.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/domains/cg/kernel/SymbolList.h \
 ../../../../../src/domains/cg/kernel/NamedList.h \
 ../../../../../src/kernel/SimControl.h \
 ../../../../../src/domains/cg/kernel/CodeStreamList.h \
 ../../../../../src/domains/cg/kernel/CodeStream.h \
 ../../../../../src/domains/cg/kernel/UniqueStringList.h \
 ../../../../../src/domains/cg/kernel/ImplementationCost.h \
 ../../../../../src/domains/cg/kernel/ConversionTable.h \
 ../../../../../src/domains/cgc/kernel/CGCPortHole.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/KnownTarget.h CGCMakefileTarget.h \
 ../../../../../src/domains/cgc/kernel/CGCTarget.h \
 ../../../../../src/domains/cg/kernel/HLLTarget.h \
 ../../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../../src/kernel/Scheduler.h \
 ../../../../../src/kernel/Galaxy.h \
 ../../../../../src/kernel/GalIter.h \
 ../../../../../src/kernel/StringArrayState.h \
 ../../../../../src/kernel/HashTable.h \
 ../../../../../src/kernel/Tokenizer.h
CGCSDFBase.o: CGCSDFBase.cc CGCSDFBase.h \
 ../../../../../src/domains/cgc/kernel/CGCStar.h \
 ../../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../../src/kernel/DynDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../../src/kernel/DynDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/domains/cg/kernel/CGTarget.h \
 ../../../../../src/kernel/Target.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/domains/cg/kernel/SymbolList.h \
 ../../../../../src/domains/cg/kernel/NamedList.h \
 ../../../../../src/kernel/SimControl.h \
 ../../../../../src/domains/cg/kernel/CodeStreamList.h \
 ../../../../../src/domains/cg/kernel/CodeStream.h \
 ../../../../../src/domains/cg/kernel/UniqueStringList.h \
 ../../../../../src/domains/cg/kernel/ImplementationCost.h \
 ../../../../../src/domains/cg/kernel/ConversionTable.h \
 ../../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../../src/domains/cgc/kernel/CGCPortHole.h \
 ../../../../../src/kernel/PrecisionState.h
CGCSDFSend.o: CGCSDFSend.cc CGCSDFSend.h CGCSDFBase.h \
 ../../../../../src/domains/cgc/kernel/CGCStar.h \
 ../../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../../src/kernel/DynDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../../src/kernel/DynDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/domains/cg/kernel/CGTarget.h \
 ../../../../../src/kernel/Target.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/domains/cg/kernel/SymbolList.h \
 ../../../../../src/domains/cg/kernel/NamedList.h \
 ../../../../../src/kernel/SimControl.h \
 ../../../../../src/domains/cg/kernel/CodeStreamList.h \
 ../../../../../src/domains/cg/kernel/CodeStream.h \
 ../../../../../src/domains/cg/kernel/UniqueStringList.h \
 ../../../../../src/domains/cg/kernel/ImplementationCost.h \
 ../../../../../src/domains/cg/kernel/ConversionTable.h \
 ../../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../../src/domains/cgc/kernel/CGCPortHole.h \
 ../../../../../src/kernel/PrecisionState.h
CGCSDFReceive.o: CGCSDFReceive.cc CGCSDFReceive.h CGCSDFBase.h \
 ../../../../../src/domains/cgc/kernel/CGCStar.h \
 ../../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../../src/kernel/DynDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../../src/kernel/DynDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/domains/cg/kernel/CGTarget.h \
 ../../../../../src/kernel/Target.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/domains/cg/kernel/SymbolList.h \
 ../../../../../src/domains/cg/kernel/NamedList.h \
 ../../../../../src/kernel/SimControl.h \
 ../../../../../src/domains/cg/kernel/CodeStreamList.h \
 ../../../../../src/domains/cg/kernel/CodeStream.h \
 ../../../../../src/domains/cg/kernel/UniqueStringList.h \
 ../../../../../src/domains/cg/kernel/ImplementationCost.h \
 ../../../../../src/domains/cg/kernel/ConversionTable.h \
 ../../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../../src/domains/cgc/kernel/CGCPortHole.h \
 ../../../../../src/kernel/PrecisionState.h
