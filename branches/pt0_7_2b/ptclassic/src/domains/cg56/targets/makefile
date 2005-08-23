# Makefile for extra CG56 domain targets
# Copyright (c) 1990-1996 The Regents of the University of California.
# All Rights Reserved.
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
#                                               PT_COPYRIGHT_VERSION_2
#                                               COPYRIGHTENDKEY
#
# Programmer: Jose Luis Pino and Joe T. Buck
# @(#)make.template	1.35	7/10/96
# Created: 2/19/92
#
# If this file's name is "make.template" and there is no makefile in
# this directory, do
#	cp make.template makefile
#	make depend
# This will add the dependencies to the makefile.

# Root of Ptolemy directory
ROOT=../../../..
# True source directory
VPATH = $(ROOT)/src/domains/cg56/targets

# get configuration info
CONFIG=$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

# SDF directory
SDFDIR=$(ROOT)/src/domains/sdf/kernel

# CG directory
CGDIR=$(ROOT)/src/domains/cg/kernel
CGSTARDIR=$(ROOT)/src/domains/cg/stars


# CGC directory
CGCDIR=$(ROOT)/src/domains/cgc/kernel
CGCTAR=$(ROOT)/src/domains/cgc/targets/main
CGCSTARDIR=$(ROOT)/src/domains/cgc/stars

# CG56 kernel support
CG56DIR=$(ROOT)/src/domains/cg56/kernel
CG56STARDIR=$(ROOT)/src/domains/cg56/stars

PARDIR = $(ROOT)/src/domains/cg/parScheduler
CGTARDIR = $(ROOT)/src/domains/cg/targets

COMPAT= $(ROOT)/src/compat/ptolemy
S56PL_SRCS = 	CG56XCAsynchComm.pl CG56XCSynchComm.pl \
		CGCXBase.pl CGCXSynchComm.pl CGCXAsynchComm.pl \
		CG56XCSend.pl CG56XCReceive.pl \
		CGCXSend.pl CGCXReceive.pl \
		CG56XCPoke.pl CG56XCPeek.pl \
		CGCXPoke.pl CGCXPeek.pl

INCL=-I$(CG56DIR) -I$(CGDIR) -I$(CGSTARDIR) -I$(SDFDIR) -I$(KERNDIR) \
     -I$(PARDIR) -I$(CGTARDIR) -I$(CG56STARDIR) -I$(CGCSTARDIR) \
     -I$(CGCDIR) $(S56WHI) \
     -I$(COMPAT) -I$(CGCTAR)

# stars to make
PL_SRCS = CG56MultiSimReceive.pl CG56MultiSimSend.pl $(S56PL_SRCS)
EXTRA_SRCS = $(PL_SRCS)

# Used by make checkjunk
OTHERSRCS = XCPeek XCPoke XPeek XPoke

# objects to make
OBJS=	CG56MultiSimTarget.o Sim56Target.o S56XTarget.o Sub56Target.o \
        $(PL_SRCS:.pl=.o)

SRCS=	$(OBJS:.o=.cc)

HDRS=	$(OBJS:.o=.h)

ifeq ($(USE_SHARED_LIBS),yes) 

# If we are building shared libraries, then we can put all the targets in
# a shared library.
PTLIB = 		libcg56targets.$(LIBSUFFIX)
all:		$(SRCS) makefile $(OBJS) $(PTLIB)
install:	all $(LIBDIR)/$(PTLIB)

else

# We are not building a shared library, so we link in .o files.
all:	$(SRCS) makefile $(OBJS)
install:	all
	@echo Installation complete.
PTLIB=none	# required by common.mk
endif

# include common definitions, rules
include $(ROOT)/mk/common.mk

# Workaround bug in gcc-2.7.2 optimizer
ifeq ($(USE_GPLUSPLUS),yes)
CG56XCReceive.o: CG56XCReceive.cc
	$(UNOPTIMIZED_WARNING_MESSAGE)
	$(UNOPTIMIZED_COMPILE_RULE) $<
CG56XCSend.o: CG56XCSend.cc
	$(UNOPTIMIZED_WARNING_MESSAGE)
	$(UNOPTIMIZED_COMPILE_RULE) $<
endif

# Don't add anything after the next line; makedepend will zap it.
# DO NOT DELETE THIS LINE -- make depend depends on it.

CG56MultiSimTarget.o: CG56MultiSimTarget.cc \
 ../../../../src/kernel/pt_fstream.h ../../../../src/kernel/Error.h \
 ../../../../src/domains/cg/kernel/CGUtilities.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/DataStruct.h ../../../../src/kernel/type.h \
 CG56MultiSimTarget.h \
 ../../../../src/domains/cg/targets/CGMultiTarget.h \
 ../../../../src/domains/cg/kernel/MultiTarget.h \
 ../../../../src/domains/cg/kernel/CGTarget.h \
 ../../../../src/kernel/Target.h ../../../../src/kernel/Block.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/IntState.h \
 ../../../../src/kernel/StringState.h \
 ../../../../src/kernel/KnownState.h \
 ../../../../src/domains/cg/kernel/SymbolList.h \
 ../../../../src/domains/cg/kernel/NamedList.h \
 ../../../../src/kernel/SimControl.h \
 ../../../../src/domains/cg/kernel/CodeStreamList.h \
 ../../../../src/domains/cg/kernel/CodeStream.h \
 ../../../../src/domains/cg/kernel/UniqueStringList.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/cg/kernel/ImplementationCost.h \
 ../../../../src/domains/cg/kernel/ConversionTable.h \
 ../../../../src/domains/cg/kernel/IntArray.h \
 ../../../../src/kernel/StringArrayState.h \
 ../../../../src/kernel/IntArrayState.h \
 ../../../../src/domains/cg/kernel/ProcMemory.h \
 ../../../../src/domains/cg/kernel/MReq.h \
 ../../../../src/domains/cg/kernel/AsmStar.h \
 ../../../../src/domains/cg/kernel/AsmPortHole.h \
 ../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h \
 ../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../src/domains/cg/kernel/MemMap.h \
 ../../../../src/kernel/IntervalList.h CG56MultiSimSend.h \
 ../../../../src/domains/cg56/kernel/CG56Star.h \
 ../../../../src/domains/cg56/kernel/CG56PortHole.h \
 ../../../../src/domains/cg56/kernel/MotorolaPortHole.h \
 ../../../../src/domains/cg56/kernel/MotorolaAttributes.h \
 CG56MultiSimReceive.h \
 ../../../../src/domains/cg56/kernel/CG56Target.h \
 ../../../../src/domains/cg56/kernel/MotorolaTarget.h \
 ../../../../src/domains/cg/kernel/AsmTarget.h \
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/Scheduler.h ../../../../src/kernel/Galaxy.h \
 ../../../../src/kernel/GalIter.h \
 ../../../../src/domains/cg/kernel/RepStateList.h Sim56Target.h \
 ../../../../src/domains/cg56/kernel/MotorolaSimTarget.h \
 ../../../../src/kernel/KnownTarget.h \
 ../../../../src/kernel/FixState.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/domains/cg/parScheduler/ParScheduler.h \
 ../../../../src/domains/cg/parScheduler/ParProcessors.h \
 ../../../../src/domains/cg/parScheduler/UniProcessor.h \
 ../../../../src/domains/cg/parScheduler/ParNode.h \
 ../../../../src/domains/sdf/kernel/EGNode.h \
 ../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../src/kernel/DoubleLink.h \
 ../../../../src/domains/sdf/kernel/EGGate.h \
 ../../../../src/domains/sdf/kernel/ExpandedGraph.h \
 ../../../../src/domains/cg/kernel/Profile.h \
 ../../../../src/kernel/DynamicGalaxy.h \
 ../../../../src/domains/cg/parScheduler/ParGraph.h
Sim56Target.o: Sim56Target.cc Sim56Target.h \
 ../../../../src/domains/cg56/kernel/CG56Target.h \
 ../../../../src/domains/cg56/kernel/MotorolaTarget.h \
 ../../../../src/domains/cg/kernel/AsmTarget.h \
 ../../../../src/domains/cg/kernel/CGTarget.h \
 ../../../../src/kernel/Target.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/Block.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/IntState.h \
 ../../../../src/kernel/StringState.h \
 ../../../../src/kernel/KnownState.h \
 ../../../../src/domains/cg/kernel/SymbolList.h \
 ../../../../src/domains/cg/kernel/NamedList.h \
 ../../../../src/kernel/SimControl.h \
 ../../../../src/domains/cg/kernel/CodeStreamList.h \
 ../../../../src/domains/cg/kernel/CodeStream.h \
 ../../../../src/domains/cg/kernel/UniqueStringList.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/cg/kernel/ImplementationCost.h \
 ../../../../src/domains/cg/kernel/ConversionTable.h \
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/Scheduler.h ../../../../src/kernel/Galaxy.h \
 ../../../../src/kernel/GalIter.h ../../../../src/kernel/DFPortHole.h \
 ../../../../src/domains/cg/kernel/RepStateList.h \
 ../../../../src/domains/cg/kernel/ProcMemory.h \
 ../../../../src/domains/cg/kernel/MReq.h \
 ../../../../src/domains/cg/kernel/AsmStar.h \
 ../../../../src/domains/cg/kernel/AsmPortHole.h \
 ../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../src/domains/cg/kernel/MemMap.h \
 ../../../../src/kernel/IntervalList.h \
 ../../../../src/domains/cg56/kernel/MotorolaSimTarget.h \
 ../../../../src/kernel/KnownTarget.h
S56XTarget.o: S56XTarget.cc S56XTarget.h \
 ../../../../src/domains/cg56/kernel/CG56Target.h \
 ../../../../src/domains/cg56/kernel/MotorolaTarget.h \
 ../../../../src/domains/cg/kernel/AsmTarget.h \
 ../../../../src/domains/cg/kernel/CGTarget.h \
 ../../../../src/kernel/Target.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/Block.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/IntState.h \
 ../../../../src/kernel/StringState.h \
 ../../../../src/kernel/KnownState.h \
 ../../../../src/domains/cg/kernel/SymbolList.h \
 ../../../../src/domains/cg/kernel/NamedList.h \
 ../../../../src/kernel/SimControl.h \
 ../../../../src/domains/cg/kernel/CodeStreamList.h \
 ../../../../src/domains/cg/kernel/CodeStream.h \
 ../../../../src/domains/cg/kernel/UniqueStringList.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/cg/kernel/ImplementationCost.h \
 ../../../../src/domains/cg/kernel/ConversionTable.h \
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/Scheduler.h ../../../../src/kernel/Galaxy.h \
 ../../../../src/kernel/GalIter.h ../../../../src/kernel/DFPortHole.h \
 ../../../../src/domains/cg/kernel/RepStateList.h \
 ../../../../src/domains/cg/kernel/ProcMemory.h \
 ../../../../src/domains/cg/kernel/MReq.h \
 ../../../../src/domains/cg/kernel/AsmStar.h \
 ../../../../src/domains/cg/kernel/AsmPortHole.h \
 ../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../src/domains/cg/kernel/MemMap.h \
 ../../../../src/kernel/IntervalList.h \
 ../../../../src/kernel/KnownTarget.h CGCXSynchComm.h \
 CG56XCSynchComm.h ../../../../src/domains/cg56/kernel/CG56Star.h \
 ../../../../src/domains/cg56/kernel/CG56PortHole.h \
 ../../../../src/domains/cg56/kernel/MotorolaPortHole.h \
 ../../../../src/domains/cg56/kernel/MotorolaAttributes.h \
 ../../../../src/kernel/IntArrayState.h CGCXBase.h \
 ../../../../src/domains/cgc/stars/CGCFix.h \
 ../../../../src/domains/cgc/kernel/CGCStar.h \
 ../../../../src/domains/cgc/kernel/CGCPortHole.h \
 ../../../../src/kernel/PrecisionState.h CG56XCReceive.h CG56XCSend.h \
 CGCXSend.h CGCXReceive.h CGCXPeek.h CGCXAsynchComm.h CGCXPoke.h \
 ../../../../src/kernel/FixState.h CG56XCPeek.h CG56XCAsynchComm.h \
 CG56XCPoke.h
Sub56Target.o: Sub56Target.cc Sub56Target.h \
 ../../../../src/domains/cg56/kernel/CG56Target.h \
 ../../../../src/domains/cg56/kernel/MotorolaTarget.h \
 ../../../../src/domains/cg/kernel/AsmTarget.h \
 ../../../../src/domains/cg/kernel/CGTarget.h \
 ../../../../src/kernel/Target.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/Block.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/IntState.h \
 ../../../../src/kernel/StringState.h \
 ../../../../src/kernel/KnownState.h \
 ../../../../src/domains/cg/kernel/SymbolList.h \
 ../../../../src/domains/cg/kernel/NamedList.h \
 ../../../../src/kernel/SimControl.h \
 ../../../../src/domains/cg/kernel/CodeStreamList.h \
 ../../../../src/domains/cg/kernel/CodeStream.h \
 ../../../../src/domains/cg/kernel/UniqueStringList.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/cg/kernel/ImplementationCost.h \
 ../../../../src/domains/cg/kernel/ConversionTable.h \
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/Scheduler.h ../../../../src/kernel/Galaxy.h \
 ../../../../src/kernel/GalIter.h ../../../../src/kernel/DFPortHole.h \
 ../../../../src/domains/cg/kernel/RepStateList.h \
 ../../../../src/domains/cg/kernel/ProcMemory.h \
 ../../../../src/domains/cg/kernel/MReq.h \
 ../../../../src/domains/cg/kernel/AsmStar.h \
 ../../../../src/domains/cg/kernel/AsmPortHole.h \
 ../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../src/domains/cg/kernel/MemMap.h \
 ../../../../src/kernel/IntervalList.h \
 ../../../../src/kernel/KnownTarget.h
CG56MultiSimReceive.o: CG56MultiSimReceive.cc CG56MultiSimReceive.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h \
 ../../../../src/domains/cg56/kernel/CG56Star.h \
 ../../../../src/domains/cg/kernel/AsmStar.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/cg/kernel/AsmPortHole.h \
 ../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/cg/kernel/CGTarget.h \
 ../../../../src/kernel/Target.h ../../../../src/kernel/IntState.h \
 ../../../../src/kernel/StringState.h \
 ../../../../src/kernel/KnownState.h \
 ../../../../src/domains/cg/kernel/SymbolList.h \
 ../../../../src/domains/cg/kernel/NamedList.h \
 ../../../../src/kernel/SimControl.h \
 ../../../../src/domains/cg/kernel/CodeStreamList.h \
 ../../../../src/domains/cg/kernel/CodeStream.h \
 ../../../../src/domains/cg/kernel/UniqueStringList.h \
 ../../../../src/domains/cg/kernel/ImplementationCost.h \
 ../../../../src/domains/cg/kernel/ConversionTable.h \
 ../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../src/domains/cg56/kernel/CG56PortHole.h \
 ../../../../src/domains/cg56/kernel/MotorolaPortHole.h \
 ../../../../src/domains/cg56/kernel/MotorolaAttributes.h \
 ../../../../src/domains/cg/kernel/ProcMemory.h \
 ../../../../src/domains/cg/kernel/MReq.h \
 ../../../../src/domains/cg/kernel/MemMap.h \
 ../../../../src/kernel/IntervalList.h
CG56MultiSimSend.o: CG56MultiSimSend.cc CG56MultiSimSend.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h \
 ../../../../src/domains/cg56/kernel/CG56Star.h \
 ../../../../src/domains/cg/kernel/AsmStar.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/cg/kernel/AsmPortHole.h \
 ../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/cg/kernel/CGTarget.h \
 ../../../../src/kernel/Target.h ../../../../src/kernel/IntState.h \
 ../../../../src/kernel/StringState.h \
 ../../../../src/kernel/KnownState.h \
 ../../../../src/domains/cg/kernel/SymbolList.h \
 ../../../../src/domains/cg/kernel/NamedList.h \
 ../../../../src/kernel/SimControl.h \
 ../../../../src/domains/cg/kernel/CodeStreamList.h \
 ../../../../src/domains/cg/kernel/CodeStream.h \
 ../../../../src/domains/cg/kernel/UniqueStringList.h \
 ../../../../src/domains/cg/kernel/ImplementationCost.h \
 ../../../../src/domains/cg/kernel/ConversionTable.h \
 ../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../src/domains/cg56/kernel/CG56PortHole.h \
 ../../../../src/domains/cg56/kernel/MotorolaPortHole.h \
 ../../../../src/domains/cg56/kernel/MotorolaAttributes.h \
 ../../../../src/domains/cg/kernel/ProcMemory.h \
 ../../../../src/domains/cg/kernel/MReq.h \
 ../../../../src/domains/cg/kernel/MemMap.h \
 ../../../../src/kernel/IntervalList.h
CG56XCAsynchComm.o: CG56XCAsynchComm.cc CG56XCAsynchComm.h \
 ../../../../src/domains/cg56/kernel/CG56Star.h \
 ../../../../src/domains/cg/kernel/AsmStar.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/cg/kernel/AsmPortHole.h \
 ../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/cg/kernel/CGTarget.h \
 ../../../../src/kernel/Target.h ../../../../src/kernel/IntState.h \
 ../../../../src/kernel/StringState.h \
 ../../../../src/kernel/KnownState.h \
 ../../../../src/domains/cg/kernel/SymbolList.h \
 ../../../../src/domains/cg/kernel/NamedList.h \
 ../../../../src/kernel/SimControl.h \
 ../../../../src/domains/cg/kernel/CodeStreamList.h \
 ../../../../src/domains/cg/kernel/CodeStream.h \
 ../../../../src/domains/cg/kernel/UniqueStringList.h \
 ../../../../src/domains/cg/kernel/ImplementationCost.h \
 ../../../../src/domains/cg/kernel/ConversionTable.h \
 ../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../src/domains/cg56/kernel/CG56PortHole.h \
 ../../../../src/domains/cg56/kernel/MotorolaPortHole.h \
 ../../../../src/domains/cg56/kernel/MotorolaAttributes.h \
 ../../../../src/kernel/IntArrayState.h
CG56XCSynchComm.o: CG56XCSynchComm.cc CG56XCSynchComm.h \
 ../../../../src/domains/cg56/kernel/CG56Star.h \
 ../../../../src/domains/cg/kernel/AsmStar.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/cg/kernel/AsmPortHole.h \
 ../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/cg/kernel/CGTarget.h \
 ../../../../src/kernel/Target.h ../../../../src/kernel/IntState.h \
 ../../../../src/kernel/StringState.h \
 ../../../../src/kernel/KnownState.h \
 ../../../../src/domains/cg/kernel/SymbolList.h \
 ../../../../src/domains/cg/kernel/NamedList.h \
 ../../../../src/kernel/SimControl.h \
 ../../../../src/domains/cg/kernel/CodeStreamList.h \
 ../../../../src/domains/cg/kernel/CodeStream.h \
 ../../../../src/domains/cg/kernel/UniqueStringList.h \
 ../../../../src/domains/cg/kernel/ImplementationCost.h \
 ../../../../src/domains/cg/kernel/ConversionTable.h \
 ../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../src/domains/cg56/kernel/CG56PortHole.h \
 ../../../../src/domains/cg56/kernel/MotorolaPortHole.h \
 ../../../../src/domains/cg56/kernel/MotorolaAttributes.h \
 ../../../../src/kernel/IntArrayState.h
CGCXBase.o: CGCXBase.cc CGCXBase.h \
 ../../../../src/domains/cgc/stars/CGCFix.h \
 ../../../../src/domains/cgc/kernel/CGCStar.h \
 ../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h \
 ../../../../src/domains/cg/kernel/CGTarget.h \
 ../../../../src/kernel/Target.h ../../../../src/kernel/IntState.h \
 ../../../../src/kernel/StringState.h \
 ../../../../src/kernel/KnownState.h \
 ../../../../src/domains/cg/kernel/SymbolList.h \
 ../../../../src/domains/cg/kernel/NamedList.h \
 ../../../../src/kernel/SimControl.h \
 ../../../../src/domains/cg/kernel/CodeStreamList.h \
 ../../../../src/domains/cg/kernel/CodeStream.h \
 ../../../../src/domains/cg/kernel/UniqueStringList.h \
 ../../../../src/domains/cg/kernel/ImplementationCost.h \
 ../../../../src/domains/cg/kernel/ConversionTable.h \
 ../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../src/domains/cgc/kernel/CGCPortHole.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/compat/ptolemy/compat.h
CGCXSynchComm.o: CGCXSynchComm.cc CGCXSynchComm.h CG56XCSynchComm.h \
 ../../../../src/domains/cg56/kernel/CG56Star.h \
 ../../../../src/domains/cg/kernel/AsmStar.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/cg/kernel/AsmPortHole.h \
 ../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/cg/kernel/CGTarget.h \
 ../../../../src/kernel/Target.h ../../../../src/kernel/IntState.h \
 ../../../../src/kernel/StringState.h \
 ../../../../src/kernel/KnownState.h \
 ../../../../src/domains/cg/kernel/SymbolList.h \
 ../../../../src/domains/cg/kernel/NamedList.h \
 ../../../../src/kernel/SimControl.h \
 ../../../../src/domains/cg/kernel/CodeStreamList.h \
 ../../../../src/domains/cg/kernel/CodeStream.h \
 ../../../../src/domains/cg/kernel/UniqueStringList.h \
 ../../../../src/domains/cg/kernel/ImplementationCost.h \
 ../../../../src/domains/cg/kernel/ConversionTable.h \
 ../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../src/domains/cg56/kernel/CG56PortHole.h \
 ../../../../src/domains/cg56/kernel/MotorolaPortHole.h \
 ../../../../src/domains/cg56/kernel/MotorolaAttributes.h \
 ../../../../src/kernel/IntArrayState.h CGCXBase.h \
 ../../../../src/domains/cgc/stars/CGCFix.h \
 ../../../../src/domains/cgc/kernel/CGCStar.h \
 ../../../../src/domains/cgc/kernel/CGCPortHole.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/compat/ptolemy/compat.h
CGCXAsynchComm.o: CGCXAsynchComm.cc CGCXAsynchComm.h CGCXBase.h \
 ../../../../src/domains/cgc/stars/CGCFix.h \
 ../../../../src/domains/cgc/kernel/CGCStar.h \
 ../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h \
 ../../../../src/domains/cg/kernel/CGTarget.h \
 ../../../../src/kernel/Target.h ../../../../src/kernel/IntState.h \
 ../../../../src/kernel/StringState.h \
 ../../../../src/kernel/KnownState.h \
 ../../../../src/domains/cg/kernel/SymbolList.h \
 ../../../../src/domains/cg/kernel/NamedList.h \
 ../../../../src/kernel/SimControl.h \
 ../../../../src/domains/cg/kernel/CodeStreamList.h \
 ../../../../src/domains/cg/kernel/CodeStream.h \
 ../../../../src/domains/cg/kernel/UniqueStringList.h \
 ../../../../src/domains/cg/kernel/ImplementationCost.h \
 ../../../../src/domains/cg/kernel/ConversionTable.h \
 ../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../src/domains/cgc/kernel/CGCPortHole.h \
 ../../../../src/kernel/PrecisionState.h
CG56XCSend.o: CG56XCSend.cc CG56XCSend.h CG56XCSynchComm.h \
 ../../../../src/domains/cg56/kernel/CG56Star.h \
 ../../../../src/domains/cg/kernel/AsmStar.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/cg/kernel/AsmPortHole.h \
 ../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/cg/kernel/CGTarget.h \
 ../../../../src/kernel/Target.h ../../../../src/kernel/IntState.h \
 ../../../../src/kernel/StringState.h \
 ../../../../src/kernel/KnownState.h \
 ../../../../src/domains/cg/kernel/SymbolList.h \
 ../../../../src/domains/cg/kernel/NamedList.h \
 ../../../../src/kernel/SimControl.h \
 ../../../../src/domains/cg/kernel/CodeStreamList.h \
 ../../../../src/domains/cg/kernel/CodeStream.h \
 ../../../../src/domains/cg/kernel/UniqueStringList.h \
 ../../../../src/domains/cg/kernel/ImplementationCost.h \
 ../../../../src/domains/cg/kernel/ConversionTable.h \
 ../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../src/domains/cg56/kernel/CG56PortHole.h \
 ../../../../src/domains/cg56/kernel/MotorolaPortHole.h \
 ../../../../src/domains/cg56/kernel/MotorolaAttributes.h \
 ../../../../src/kernel/IntArrayState.h
CG56XCReceive.o: CG56XCReceive.cc CG56XCReceive.h CG56XCSynchComm.h \
 ../../../../src/domains/cg56/kernel/CG56Star.h \
 ../../../../src/domains/cg/kernel/AsmStar.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/cg/kernel/AsmPortHole.h \
 ../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/cg/kernel/CGTarget.h \
 ../../../../src/kernel/Target.h ../../../../src/kernel/IntState.h \
 ../../../../src/kernel/StringState.h \
 ../../../../src/kernel/KnownState.h \
 ../../../../src/domains/cg/kernel/SymbolList.h \
 ../../../../src/domains/cg/kernel/NamedList.h \
 ../../../../src/kernel/SimControl.h \
 ../../../../src/domains/cg/kernel/CodeStreamList.h \
 ../../../../src/domains/cg/kernel/CodeStream.h \
 ../../../../src/domains/cg/kernel/UniqueStringList.h \
 ../../../../src/domains/cg/kernel/ImplementationCost.h \
 ../../../../src/domains/cg/kernel/ConversionTable.h \
 ../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../src/domains/cg56/kernel/CG56PortHole.h \
 ../../../../src/domains/cg56/kernel/MotorolaPortHole.h \
 ../../../../src/domains/cg56/kernel/MotorolaAttributes.h \
 ../../../../src/kernel/IntArrayState.h CGCXReceive.h CGCXSynchComm.h \
 CGCXBase.h ../../../../src/domains/cgc/stars/CGCFix.h \
 ../../../../src/domains/cgc/kernel/CGCStar.h \
 ../../../../src/domains/cgc/kernel/CGCPortHole.h \
 ../../../../src/kernel/PrecisionState.h
CGCXSend.o: CGCXSend.cc CGCXSend.h CGCXSynchComm.h CG56XCSynchComm.h \
 ../../../../src/domains/cg56/kernel/CG56Star.h \
 ../../../../src/domains/cg/kernel/AsmStar.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/cg/kernel/AsmPortHole.h \
 ../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/cg/kernel/CGTarget.h \
 ../../../../src/kernel/Target.h ../../../../src/kernel/IntState.h \
 ../../../../src/kernel/StringState.h \
 ../../../../src/kernel/KnownState.h \
 ../../../../src/domains/cg/kernel/SymbolList.h \
 ../../../../src/domains/cg/kernel/NamedList.h \
 ../../../../src/kernel/SimControl.h \
 ../../../../src/domains/cg/kernel/CodeStreamList.h \
 ../../../../src/domains/cg/kernel/CodeStream.h \
 ../../../../src/domains/cg/kernel/UniqueStringList.h \
 ../../../../src/domains/cg/kernel/ImplementationCost.h \
 ../../../../src/domains/cg/kernel/ConversionTable.h \
 ../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../src/domains/cg56/kernel/CG56PortHole.h \
 ../../../../src/domains/cg56/kernel/MotorolaPortHole.h \
 ../../../../src/domains/cg56/kernel/MotorolaAttributes.h \
 ../../../../src/kernel/IntArrayState.h CGCXBase.h \
 ../../../../src/domains/cgc/stars/CGCFix.h \
 ../../../../src/domains/cgc/kernel/CGCStar.h \
 ../../../../src/domains/cgc/kernel/CGCPortHole.h \
 ../../../../src/kernel/PrecisionState.h
CGCXReceive.o: CGCXReceive.cc CGCXReceive.h CGCXSynchComm.h \
 CG56XCSynchComm.h ../../../../src/domains/cg56/kernel/CG56Star.h \
 ../../../../src/domains/cg/kernel/AsmStar.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/cg/kernel/AsmPortHole.h \
 ../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/cg/kernel/CGTarget.h \
 ../../../../src/kernel/Target.h ../../../../src/kernel/IntState.h \
 ../../../../src/kernel/StringState.h \
 ../../../../src/kernel/KnownState.h \
 ../../../../src/domains/cg/kernel/SymbolList.h \
 ../../../../src/domains/cg/kernel/NamedList.h \
 ../../../../src/kernel/SimControl.h \
 ../../../../src/domains/cg/kernel/CodeStreamList.h \
 ../../../../src/domains/cg/kernel/CodeStream.h \
 ../../../../src/domains/cg/kernel/UniqueStringList.h \
 ../../../../src/domains/cg/kernel/ImplementationCost.h \
 ../../../../src/domains/cg/kernel/ConversionTable.h \
 ../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../src/domains/cg56/kernel/CG56PortHole.h \
 ../../../../src/domains/cg56/kernel/MotorolaPortHole.h \
 ../../../../src/domains/cg56/kernel/MotorolaAttributes.h \
 ../../../../src/kernel/IntArrayState.h CGCXBase.h \
 ../../../../src/domains/cgc/stars/CGCFix.h \
 ../../../../src/domains/cgc/kernel/CGCStar.h \
 ../../../../src/domains/cgc/kernel/CGCPortHole.h \
 ../../../../src/kernel/PrecisionState.h
CG56XCPoke.o: CG56XCPoke.cc CG56XCPoke.h CG56XCAsynchComm.h \
 ../../../../src/domains/cg56/kernel/CG56Star.h \
 ../../../../src/domains/cg/kernel/AsmStar.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/cg/kernel/AsmPortHole.h \
 ../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/cg/kernel/CGTarget.h \
 ../../../../src/kernel/Target.h ../../../../src/kernel/IntState.h \
 ../../../../src/kernel/StringState.h \
 ../../../../src/kernel/KnownState.h \
 ../../../../src/domains/cg/kernel/SymbolList.h \
 ../../../../src/domains/cg/kernel/NamedList.h \
 ../../../../src/kernel/SimControl.h \
 ../../../../src/domains/cg/kernel/CodeStreamList.h \
 ../../../../src/domains/cg/kernel/CodeStream.h \
 ../../../../src/domains/cg/kernel/UniqueStringList.h \
 ../../../../src/domains/cg/kernel/ImplementationCost.h \
 ../../../../src/domains/cg/kernel/ConversionTable.h \
 ../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../src/domains/cg56/kernel/CG56PortHole.h \
 ../../../../src/domains/cg56/kernel/MotorolaPortHole.h \
 ../../../../src/domains/cg56/kernel/MotorolaAttributes.h \
 ../../../../src/kernel/IntArrayState.h
CG56XCPeek.o: CG56XCPeek.cc CG56XCPeek.h CG56XCAsynchComm.h \
 ../../../../src/domains/cg56/kernel/CG56Star.h \
 ../../../../src/domains/cg/kernel/AsmStar.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/cg/kernel/AsmPortHole.h \
 ../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/cg/kernel/CGTarget.h \
 ../../../../src/kernel/Target.h ../../../../src/kernel/IntState.h \
 ../../../../src/kernel/StringState.h \
 ../../../../src/kernel/KnownState.h \
 ../../../../src/domains/cg/kernel/SymbolList.h \
 ../../../../src/domains/cg/kernel/NamedList.h \
 ../../../../src/kernel/SimControl.h \
 ../../../../src/domains/cg/kernel/CodeStreamList.h \
 ../../../../src/domains/cg/kernel/CodeStream.h \
 ../../../../src/domains/cg/kernel/UniqueStringList.h \
 ../../../../src/domains/cg/kernel/ImplementationCost.h \
 ../../../../src/domains/cg/kernel/ConversionTable.h \
 ../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../src/domains/cg56/kernel/CG56PortHole.h \
 ../../../../src/domains/cg56/kernel/MotorolaPortHole.h \
 ../../../../src/domains/cg56/kernel/MotorolaAttributes.h \
 ../../../../src/kernel/IntArrayState.h
CGCXPoke.o: CGCXPoke.cc CGCXPoke.h CGCXAsynchComm.h CGCXBase.h \
 ../../../../src/domains/cgc/stars/CGCFix.h \
 ../../../../src/domains/cgc/kernel/CGCStar.h \
 ../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h \
 ../../../../src/domains/cg/kernel/CGTarget.h \
 ../../../../src/kernel/Target.h ../../../../src/kernel/IntState.h \
 ../../../../src/kernel/StringState.h \
 ../../../../src/kernel/KnownState.h \
 ../../../../src/domains/cg/kernel/SymbolList.h \
 ../../../../src/domains/cg/kernel/NamedList.h \
 ../../../../src/kernel/SimControl.h \
 ../../../../src/domains/cg/kernel/CodeStreamList.h \
 ../../../../src/domains/cg/kernel/CodeStream.h \
 ../../../../src/domains/cg/kernel/UniqueStringList.h \
 ../../../../src/domains/cg/kernel/ImplementationCost.h \
 ../../../../src/domains/cg/kernel/ConversionTable.h \
 ../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../src/domains/cgc/kernel/CGCPortHole.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/kernel/FixState.h
CGCXPeek.o: CGCXPeek.cc CGCXPeek.h CGCXAsynchComm.h CGCXBase.h \
 ../../../../src/domains/cgc/stars/CGCFix.h \
 ../../../../src/domains/cgc/kernel/CGCStar.h \
 ../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h \
 ../../../../src/domains/cg/kernel/CGTarget.h \
 ../../../../src/kernel/Target.h ../../../../src/kernel/IntState.h \
 ../../../../src/kernel/StringState.h \
 ../../../../src/kernel/KnownState.h \
 ../../../../src/domains/cg/kernel/SymbolList.h \
 ../../../../src/domains/cg/kernel/NamedList.h \
 ../../../../src/kernel/SimControl.h \
 ../../../../src/domains/cg/kernel/CodeStreamList.h \
 ../../../../src/domains/cg/kernel/CodeStream.h \
 ../../../../src/domains/cg/kernel/UniqueStringList.h \
 ../../../../src/domains/cg/kernel/ImplementationCost.h \
 ../../../../src/domains/cg/kernel/ConversionTable.h \
 ../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../src/domains/cgc/kernel/CGCPortHole.h \
 ../../../../src/kernel/PrecisionState.h
