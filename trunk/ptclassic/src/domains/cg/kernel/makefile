# Makefile for CG domain support
# Version identification:
# @(#)make.template	1.43	9/4/96
# Copyright (c) 1990-1996 The Regents of the University of California.
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
# Date of creation: 9/14/90
#
# If this file's name is "make.template" and there is no makefile in
# this directory, do
#	cp make.template makefile
#	make depend
# This will add the dependencies to the makefile.

# Root of Ptolemy directory
ROOT=../../../..
# True source directory
VPATH = $(ROOT)/src/domains/cg/kernel

# get configuration info
CONFIG=$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

# SDF directory
SDFDIR=$(ROOT)/src/domains/sdf/kernel
LOOPDIR=$(ROOT)/src/domains/sdf/loopScheduler

# We need to include ptcl/displayFile.h for CGUtilities.cc
INCL= $(PTCOMPAT_INCSPEC) -I$(PTCL_INCDIR) \
	-I$(KERNDIR) -I$(SDFDIR) -I$(LOOPDIR)

# Library name
PTLIB=libcg.$(LIBSUFFIX)

# .h files with matching cc files
H_WITH_MATCH= \
AnyAsmStar.h \
AnyCGStar.h \
AsmPortHole.h \
AsmForkNode.h \
AsmGeodesic.h \
AsmStar.h \
AsmTarget.h \
MultiTarget.h \
HLLTarget.h \
CGDDFCode.h \
CGPortHole.h \
CGGeodesic.h \
CGStar.h \
CGTarget.h \
CGUtilities.h \
CGWormBase.h \
CGWormhole.h \
CodeBlock.h \
CodeStream.h \
CodeStreamList.h \
ConversionTable.h \
ImplementationCost.h \
IntArray.h \
MReq.h \
MemMap.h \
NamedList.h \
ProcMemory.h \
Profile.h \
RepStateList.h \
SymbolList.h \
UniqueStringList.h

# Header files
HDRS= $(H_WITH_MATCH) 

# Library objects
OBJS= $(H_WITH_MATCH:.h=.o) CGDomain.o AsmTargetInit.o

# Library sources
SRCS= $(OBJS:.o=.cc)

all:	makefile $(PTLIB)

# "make install" puts things in the official place
install:	makefile $(LIBDIR)/$(PTLIB)


# include common definitions, rules
include $(ROOT)/mk/common.mk

# Don't add anything after the next line; makedepend will zap it.
# DO NOT DELETE THIS LINE -- make depend depends on it.

AnyAsmStar.o: AnyAsmStar.cc AnyAsmStar.h AsmStar.h \
 ../../../../src/kernel/Attribute.h AsmPortHole.h CGPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/ComplexSubset.h CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/Fraction.h CGTarget.h \
 ../../../../src/kernel/Target.h ../../../../src/kernel/IntState.h \
 ../../../../src/kernel/StringState.h \
 ../../../../src/kernel/KnownState.h SymbolList.h NamedList.h \
 ../../../../src/kernel/SimControl.h CodeStreamList.h CodeStream.h \
 UniqueStringList.h ImplementationCost.h ConversionTable.h CodeBlock.h \
 ../../../../src/kernel/Domain.h CGWormhole.h CGWormBase.h \
 ../../../../src/kernel/Wormhole.h ../../../../src/kernel/Universe.h \
 ../../../../src/kernel/Galaxy.h ../../../../src/kernel/EventHorizon.h \
 Profile.h IntArray.h MultiTarget.h AsmForkNode.h AsmGeodesic.h \
 CGGeodesic.h ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/AutoFork.h
AnyCGStar.o: AnyCGStar.cc AnyCGStar.h CGStar.h \
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
 ../../../../src/kernel/Fraction.h CGPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h CGTarget.h \
 ../../../../src/kernel/Target.h ../../../../src/kernel/IntState.h \
 ../../../../src/kernel/StringState.h \
 ../../../../src/kernel/KnownState.h SymbolList.h NamedList.h \
 ../../../../src/kernel/SimControl.h CodeStreamList.h CodeStream.h \
 UniqueStringList.h ImplementationCost.h ConversionTable.h CodeBlock.h \
 ../../../../src/kernel/Domain.h CGWormhole.h CGWormBase.h \
 ../../../../src/kernel/Wormhole.h ../../../../src/kernel/Universe.h \
 ../../../../src/kernel/Galaxy.h ../../../../src/kernel/EventHorizon.h \
 Profile.h IntArray.h MultiTarget.h \
 ../../../../src/kernel/AutoForkNode.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/AutoFork.h
AsmPortHole.o: AsmPortHole.cc AsmPortHole.h CGPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h AsmGeodesic.h CGGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h
AsmForkNode.o: AsmForkNode.cc AsmForkNode.h AsmGeodesic.h \
 AsmPortHole.h CGPortHole.h ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h CGGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/AutoFork.h
AsmGeodesic.o: AsmGeodesic.cc AsmGeodesic.h AsmPortHole.h CGPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h CGGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h ProcMemory.h MReq.h AsmStar.h \
 CGStar.h ../../../../src/kernel/DynDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/Fraction.h CGTarget.h \
 ../../../../src/kernel/Target.h ../../../../src/kernel/IntState.h \
 ../../../../src/kernel/StringState.h \
 ../../../../src/kernel/KnownState.h SymbolList.h NamedList.h \
 ../../../../src/kernel/SimControl.h CodeStreamList.h CodeStream.h \
 UniqueStringList.h ImplementationCost.h ConversionTable.h CodeBlock.h \
 MemMap.h ../../../../src/kernel/IntervalList.h
AsmStar.o: AsmStar.cc AsmStar.h ../../../../src/kernel/Attribute.h \
 AsmPortHole.h CGPortHole.h ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/ComplexSubset.h CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/Fraction.h CGTarget.h \
 ../../../../src/kernel/Target.h ../../../../src/kernel/IntState.h \
 ../../../../src/kernel/StringState.h \
 ../../../../src/kernel/KnownState.h SymbolList.h NamedList.h \
 ../../../../src/kernel/SimControl.h CodeStreamList.h CodeStream.h \
 UniqueStringList.h ImplementationCost.h ConversionTable.h CodeBlock.h \
 AsmTarget.h ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/Scheduler.h ../../../../src/kernel/Galaxy.h \
 ../../../../src/kernel/GalIter.h RepStateList.h ProcMemory.h MReq.h \
 MemMap.h ../../../../src/kernel/IntervalList.h \
 ../../../../src/kernel/FixState.h \
 ../../../../src/kernel/PrecisionState.h
AsmTarget.o: AsmTarget.cc AsmTarget.h CGTarget.h \
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
 ../../../../src/kernel/KnownState.h SymbolList.h NamedList.h \
 ../../../../src/kernel/SimControl.h CodeStreamList.h CodeStream.h \
 UniqueStringList.h ../../../../src/kernel/DataFlowStar.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Fraction.h \
 ImplementationCost.h ConversionTable.h \
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/Scheduler.h ../../../../src/kernel/Galaxy.h \
 ../../../../src/kernel/GalIter.h ../../../../src/kernel/DFPortHole.h \
 RepStateList.h AsmStar.h AsmPortHole.h CGPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h CGStar.h \
 ../../../../src/kernel/DynDFStar.h CodeBlock.h ProcMemory.h MReq.h \
 MemMap.h ../../../../src/kernel/IntervalList.h CGUtilities.h \
 ../../../../src/kernel/KnownBlock.h ../../../../src/kernel/Linker.h \
 ../../../../src/kernel/pt_fstream.h \
 ../../../../src/kernel/checkConnect.h
MultiTarget.o: MultiTarget.cc CGStar.h \
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
 ../../../../src/kernel/Fraction.h CGPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h CGTarget.h \
 ../../../../src/kernel/Target.h ../../../../src/kernel/IntState.h \
 ../../../../src/kernel/StringState.h \
 ../../../../src/kernel/KnownState.h SymbolList.h NamedList.h \
 ../../../../src/kernel/SimControl.h CodeStreamList.h CodeStream.h \
 UniqueStringList.h ImplementationCost.h ConversionTable.h CodeBlock.h \
 MultiTarget.h IntArray.h \
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/Scheduler.h ../../../../src/kernel/Galaxy.h \
 ../../../../src/kernel/GalIter.h Profile.h CGDDFCode.h
HLLTarget.o: HLLTarget.cc HLLTarget.h CGTarget.h \
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
 ../../../../src/kernel/KnownState.h SymbolList.h NamedList.h \
 ../../../../src/kernel/SimControl.h CodeStreamList.h CodeStream.h \
 UniqueStringList.h ../../../../src/kernel/DataFlowStar.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Fraction.h \
 ImplementationCost.h ConversionTable.h \
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/Scheduler.h ../../../../src/kernel/Galaxy.h \
 ../../../../src/kernel/GalIter.h ../../../../src/kernel/DFPortHole.h \
 ../../../../src/kernel/KnownTarget.h CGUtilities.h \
 ../../../../src/kernel/pt_fstream.h
CGDDFCode.o: CGDDFCode.cc CGDDFCode.h MultiTarget.h CGTarget.h \
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
 ../../../../src/kernel/KnownState.h SymbolList.h NamedList.h \
 ../../../../src/kernel/SimControl.h CodeStreamList.h CodeStream.h \
 UniqueStringList.h ../../../../src/kernel/DataFlowStar.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Fraction.h \
 ImplementationCost.h ConversionTable.h IntArray.h
CGPortHole.o: CGPortHole.cc ../../../../src/kernel/Block.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h CGPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h \
 ../../../../src/kernel/CircularBuffer.h CGGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h
CGGeodesic.o: CGGeodesic.cc CGGeodesic.h CGPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/Fraction.h ../../../../src/kernel/SimControl.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h
CGStar.o: CGStar.cc CGStar.h ../../../../src/kernel/DynDFStar.h \
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
 ../../../../src/kernel/Fraction.h CGPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h CGTarget.h \
 ../../../../src/kernel/Target.h ../../../../src/kernel/IntState.h \
 ../../../../src/kernel/StringState.h \
 ../../../../src/kernel/KnownState.h SymbolList.h NamedList.h \
 ../../../../src/kernel/SimControl.h CodeStreamList.h CodeStream.h \
 UniqueStringList.h ImplementationCost.h ConversionTable.h CodeBlock.h \
 CGGeodesic.h ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h CGWormBase.h \
 ../../../../src/kernel/Wormhole.h ../../../../src/kernel/Universe.h \
 ../../../../src/kernel/Galaxy.h ../../../../src/kernel/EventHorizon.h \
 Profile.h IntArray.h MultiTarget.h
CGTarget.o: CGTarget.cc CGTarget.h ../../../../src/kernel/Target.h \
 ../../../../src/kernel/StringList.h \
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
 ../../../../src/kernel/KnownState.h SymbolList.h NamedList.h \
 ../../../../src/kernel/SimControl.h CodeStreamList.h CodeStream.h \
 UniqueStringList.h ../../../../src/kernel/DataFlowStar.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Fraction.h \
 ImplementationCost.h ConversionTable.h \
 ../../../../src/kernel/Domain.h ../../../../src/kernel/KnownBlock.h \
 ../../../../src/kernel/Linker.h ../../../../src/kernel/GalIter.h \
 ../../../../src/kernel/Galaxy.h ../../../../src/kernel/EventHorizon.h \
 ../../../../src/kernel/pt_fstream.h \
 ../../../../src/kernel/Tokenizer.h \
 ../../../../src/kernel/ProfileTimer.h \
 ../../../../src/kernel/TimeVal.h \
 ../../../../src/kernel/checkConnect.h \
 ../../../../src/domains/sdf/loopScheduler/LoopScheduler.h \
 ../../../../src/domains/sdf/kernel/SDFCluster.h \
 ../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h \
 ../../../../src/kernel/DynamicGalaxy.h \
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/Scheduler.h \
 ../../../../src/domains/sdf/kernel/AcyLoopScheduler.h \
 ../../../../src/domains/sdf/kernel/AcyCluster.h \
 ../../../../src/domains/sdf/kernel/SynDFCluster.h \
 ../../../../src/kernel/Cluster.h ../../../../src/kernel/GraphUtils.h \
 CGUtilities.h CGStar.h ../../../../src/kernel/DynDFStar.h \
 CGPortHole.h ../../../../src/kernel/DynDFPortHole.h CodeBlock.h
CGUtilities.o: CGUtilities.cc ../../../../src/kernel/pt_fstream.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 CGUtilities.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/compat/ptolemy/compat.h \
 ../../../../src/ptcl/displayFile.h
CGWormBase.o: CGWormBase.cc CGWormBase.h \
 ../../../../src/kernel/Wormhole.h ../../../../src/kernel/type.h \
 ../../../../src/kernel/logNew.h ../../../../src/kernel/Universe.h \
 ../../../../src/kernel/Galaxy.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/DataStruct.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h ../../../../src/kernel/Target.h \
 ../../../../src/kernel/EventHorizon.h Profile.h IntArray.h \
 MultiTarget.h CGTarget.h ../../../../src/kernel/IntState.h \
 ../../../../src/kernel/StringState.h \
 ../../../../src/kernel/KnownState.h SymbolList.h NamedList.h \
 ../../../../src/kernel/SimControl.h CodeStreamList.h CodeStream.h \
 UniqueStringList.h ../../../../src/kernel/DataFlowStar.h \
 ../../../../src/kernel/Fraction.h ImplementationCost.h \
 ConversionTable.h ../../../../src/kernel/GalIter.h \
 ../../../../src/kernel/Scheduler.h
CGWormhole.o: CGWormhole.cc CGWormhole.h CGStar.h \
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
 ../../../../src/kernel/Fraction.h CGPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h CGTarget.h \
 ../../../../src/kernel/Target.h ../../../../src/kernel/IntState.h \
 ../../../../src/kernel/StringState.h \
 ../../../../src/kernel/KnownState.h SymbolList.h NamedList.h \
 ../../../../src/kernel/SimControl.h CodeStreamList.h CodeStream.h \
 UniqueStringList.h ImplementationCost.h ConversionTable.h CodeBlock.h \
 CGWormBase.h ../../../../src/kernel/Wormhole.h \
 ../../../../src/kernel/Universe.h ../../../../src/kernel/Galaxy.h \
 ../../../../src/kernel/EventHorizon.h Profile.h IntArray.h \
 MultiTarget.h ../../../../src/kernel/GalIter.h \
 ../../../../src/kernel/Scheduler.h
CodeBlock.o: CodeBlock.cc CodeBlock.h \
 ../../../../src/kernel/DataStruct.h ../../../../src/kernel/type.h \
 ../../../../src/kernel/logNew.h
CodeStream.o: CodeStream.cc CodeStream.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h UniqueStringList.h
CodeStreamList.o: CodeStreamList.cc CodeStreamList.h NamedList.h \
 ../../../../src/kernel/DataStruct.h ../../../../src/kernel/type.h \
 ../../../../src/kernel/logNew.h CodeStream.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 UniqueStringList.h
ConversionTable.o: ConversionTable.cc ConversionTable.h \
 ../../../../src/kernel/dataType.h
ImplementationCost.o: ImplementationCost.cc ImplementationCost.h
IntArray.o: IntArray.cc IntArray.h ../../../../src/kernel/type.h \
 ../../../../src/kernel/logNew.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h \
 ../../../../src/kernel/DataStruct.h
MReq.o: MReq.cc MReq.h AsmStar.h ../../../../src/kernel/Attribute.h \
 AsmPortHole.h CGPortHole.h ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/ComplexSubset.h CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/Fraction.h CGTarget.h \
 ../../../../src/kernel/Target.h ../../../../src/kernel/IntState.h \
 ../../../../src/kernel/StringState.h \
 ../../../../src/kernel/KnownState.h SymbolList.h NamedList.h \
 ../../../../src/kernel/SimControl.h CodeStreamList.h CodeStream.h \
 UniqueStringList.h ImplementationCost.h ConversionTable.h CodeBlock.h
MemMap.o: MemMap.cc MemMap.h MReq.h AsmStar.h \
 ../../../../src/kernel/Attribute.h AsmPortHole.h CGPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/ComplexSubset.h CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/Fraction.h CGTarget.h \
 ../../../../src/kernel/Target.h ../../../../src/kernel/IntState.h \
 ../../../../src/kernel/StringState.h \
 ../../../../src/kernel/KnownState.h SymbolList.h NamedList.h \
 ../../../../src/kernel/SimControl.h CodeStreamList.h CodeStream.h \
 UniqueStringList.h ImplementationCost.h ConversionTable.h CodeBlock.h
NamedList.o: NamedList.cc NamedList.h \
 ../../../../src/kernel/DataStruct.h ../../../../src/kernel/type.h \
 ../../../../src/kernel/logNew.h ../../../../src/kernel/miscFuncs.h \
 ../../../../src/kernel/Error.h
ProcMemory.o: ProcMemory.cc ProcMemory.h MReq.h AsmStar.h \
 ../../../../src/kernel/Attribute.h AsmPortHole.h CGPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/ComplexSubset.h CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/Fraction.h CGTarget.h \
 ../../../../src/kernel/Target.h ../../../../src/kernel/IntState.h \
 ../../../../src/kernel/StringState.h \
 ../../../../src/kernel/KnownState.h SymbolList.h NamedList.h \
 ../../../../src/kernel/SimControl.h CodeStreamList.h CodeStream.h \
 UniqueStringList.h ImplementationCost.h ConversionTable.h CodeBlock.h \
 MemMap.h ../../../../src/kernel/IntervalList.h
Profile.o: Profile.cc Profile.h IntArray.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h
RepStateList.o: RepStateList.cc RepStateList.h \
 ../../../../src/kernel/State.h ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h ../../../../src/kernel/type.h \
 ../../../../src/kernel/logNew.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/kernel/ConstIters.h ../../../../src/kernel/Block.h \
 ../../../../src/kernel/PortHole.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Galaxy.h ../../../../src/kernel/Star.h
SymbolList.o: SymbolList.cc SymbolList.h NamedList.h \
 ../../../../src/kernel/DataStruct.h ../../../../src/kernel/type.h \
 ../../../../src/kernel/logNew.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h
UniqueStringList.o: UniqueStringList.cc UniqueStringList.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h
CGDomain.o: CGDomain.cc ../../../../src/kernel/Domain.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/Block.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h CGTarget.h \
 ../../../../src/kernel/Target.h ../../../../src/kernel/IntState.h \
 ../../../../src/kernel/StringState.h \
 ../../../../src/kernel/KnownState.h SymbolList.h NamedList.h \
 ../../../../src/kernel/SimControl.h CodeStreamList.h CodeStream.h \
 UniqueStringList.h ../../../../src/kernel/DataFlowStar.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Fraction.h \
 ImplementationCost.h ConversionTable.h \
 ../../../../src/kernel/KnownTarget.h \
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/Scheduler.h ../../../../src/kernel/Galaxy.h \
 ../../../../src/kernel/GalIter.h ../../../../src/kernel/DFPortHole.h \
 CGPortHole.h ../../../../src/kernel/DynDFPortHole.h AsmForkNode.h \
 AsmGeodesic.h AsmPortHole.h CGGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/AutoFork.h CGWormhole.h CGStar.h \
 ../../../../src/kernel/DynDFStar.h CodeBlock.h CGWormBase.h \
 ../../../../src/kernel/Wormhole.h ../../../../src/kernel/Universe.h \
 ../../../../src/kernel/EventHorizon.h Profile.h IntArray.h \
 MultiTarget.h AsmTarget.h RepStateList.h
AsmTargetInit.o: AsmTargetInit.cc AsmTarget.h CGTarget.h \
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
 ../../../../src/kernel/KnownState.h SymbolList.h NamedList.h \
 ../../../../src/kernel/SimControl.h CodeStreamList.h CodeStream.h \
 UniqueStringList.h ../../../../src/kernel/DataFlowStar.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Fraction.h \
 ImplementationCost.h ConversionTable.h \
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/Scheduler.h ../../../../src/kernel/Galaxy.h \
 ../../../../src/kernel/GalIter.h ../../../../src/kernel/DFPortHole.h \
 RepStateList.h AsmStar.h AsmPortHole.h CGPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h CGStar.h \
 ../../../../src/kernel/DynDFStar.h CodeBlock.h \
 ../../../../src/kernel/FloatState.h ../../../../src/kernel/FixState.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/kernel/IntArrayState.h \
 ../../../../src/kernel/FloatArrayState.h \
 ../../../../src/kernel/FixArrayState.h ProcMemory.h MReq.h MemMap.h \
 ../../../../src/kernel/IntervalList.h
