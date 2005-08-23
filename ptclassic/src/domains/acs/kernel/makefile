# Makefile for ACS domain support
# Version identification:
# @(#)make.template	1.15 08/15/00
# @Copyright (c) 1998-2000 The Regents of the University of California.
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
# Date of creation: 9/14/90
#
# If this file's name is "make.template" and there is no makefile in
# this directory, do
#	cp make.template makefile
#	make depend
# This will add the dependencies to the makefile.

ROOT=../../../..
# True source directory
VPATH = $(ROOT)/src/domains/acs/kernel

# get configuration info
CONFIG=$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

# Include path
CGKERNEL = 	$(ROOT)/src/domains/cg/kernel
ACSKERNEL = 	$(ROOT)/src/domains/acs/kernel
ACSTARGET =	$(ROOT)/src/domains/acs/targets
SDFKERNEL = 	$(ROOT)/src/domains/sdf/kernel
PIGILIB = 	$(ROOT)/src/pigilib

INCL =		-I$(CGKERNEL) -I$(ACSKERNEL) -I$(ACSTARGET) -I$(SDFKERNEL) -I$(KERNDIR) -I$(PIGILIB) $(PTCOMPAT_INCSPEC)

# Library name
PTLIB=libacs.$(LIBSUFFIX)

# Header files
HDRS= \
	ACSCore.h \
	ACSCorona.h \
	ACSPortHole.h \
	ACSScheduler.h \
	ACSStar.h \
	ACSTarget.h \
	ACSWormhole.h \
	ACSKnownCategory.h \
	ACSFPSimCore.h \
	ACSFixSimCore.h \
	ACSCGCore.h \
	ACSCGCCore.h \
	ACSGeodesic.h \
	ACSFPCGCCore.h \
	ACSForkNode.h \
	ACSCGFPGACore.h

# Library sources
SRCS= $(HDRS:.h=.cc) ACSDomain.cc

# Library objects
OBJS= $(SRCS:.cc=.o)

all:	makefile $(PTLIB)

# "make install" puts things in the official place
install:	makefile $(LIBDIR)/$(PTLIB)

# include common definitions, rules
include $(ROOT)/mk/common.mk

# Commands to make some sources from templates

# Don't add anything after the next line; makedepend will zap it.
# DO NOT DELETE THIS LINE -- make depend depends on it.



ACSCore.o: ACSCore.cc ACSCore.h ACSStar.h \
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
 ../../../../src/domains/cg/kernel/CodeBlock.h ACSPortHole.h \
 ACSGeodesic.h ../../../../src/domains/cg/kernel/CGGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/PrecisionState.h ACSCorona.h \
 ACSKnownCategory.h
ACSCorona.o: ACSCorona.cc ACSCorona.h ACSStar.h \
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
 ../../../../src/domains/cg/kernel/CodeBlock.h ACSPortHole.h \
 ACSGeodesic.h ../../../../src/domains/cg/kernel/CGGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/PrecisionState.h ACSCore.h ACSKnownCategory.h \
 ../../../../src/kernel/KnownBlock.h ../../../../src/kernel/Linker.h \
 ACSTarget.h ../../../../src/domains/cg/kernel/HLLTarget.h \
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/Scheduler.h ../../../../src/kernel/Galaxy.h \
 ../../../../src/kernel/GalIter.h ../../../../src/kernel/FloatState.h
ACSPortHole.o: ACSPortHole.cc ../../../../src/kernel/ConstIters.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h ../../../../src/kernel/Galaxy.h \
 ../../../../src/kernel/Star.h ACSPortHole.h \
 ../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h ACSGeodesic.h \
 ../../../../src/domains/cg/kernel/CGGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/PrecisionState.h ACSTarget.h \
 ../../../../src/domains/cg/kernel/HLLTarget.h \
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
 ../../../../src/kernel/DataFlowStar.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/cg/kernel/ImplementationCost.h \
 ../../../../src/domains/cg/kernel/ConversionTable.h \
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/Scheduler.h ../../../../src/kernel/GalIter.h \
 ../../../../src/kernel/FloatState.h ACSKnownCategory.h ACSCore.h \
 ACSStar.h ../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/domains/cg/kernel/CodeBlock.h ACSCorona.h \
 ../../../../src/kernel/Plasma.h ../../../../src/kernel/PtGate.h \
 ../../../../src/kernel/CircularBuffer.h
ACSScheduler.o: ACSScheduler.cc ACSScheduler.h \
 ../../../../src/kernel/Scheduler.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/SimControl.h \
 ../../../../src/kernel/Galaxy.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/isa.h ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/PortHole.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h
ACSStar.o: ACSStar.cc ACSStar.h \
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
 ../../../../src/domains/cg/kernel/CodeBlock.h ACSPortHole.h \
 ACSGeodesic.h ../../../../src/domains/cg/kernel/CGGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/PrecisionState.h ACSTarget.h \
 ../../../../src/domains/cg/kernel/HLLTarget.h \
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/Scheduler.h ../../../../src/kernel/Galaxy.h \
 ../../../../src/kernel/GalIter.h ../../../../src/kernel/FloatState.h \
 ACSKnownCategory.h ACSCore.h ACSCorona.h
ACSTarget.o: ACSTarget.cc ACSTarget.h \
 ../../../../src/domains/cg/kernel/HLLTarget.h \
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
 ../../../../src/kernel/FloatState.h ACSKnownCategory.h ACSCore.h \
 ACSStar.h ../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/domains/cg/kernel/CodeBlock.h ACSPortHole.h \
 ACSGeodesic.h ../../../../src/domains/cg/kernel/CGGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/PrecisionState.h ACSCorona.h \
 ../../../../src/kernel/KnownTarget.h \
 ../../../../src/kernel/pt_fstream.h \
 ../../../../src/kernel/Tokenizer.h \
 ../../../../src/domains/sdf/kernel/AcyLoopScheduler.h \
 ../../../../src/domains/sdf/kernel/SDFCluster.h \
 ../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../src/kernel/DynamicGalaxy.h \
 ../../../../src/domains/sdf/kernel/AcyCluster.h \
 ../../../../src/domains/sdf/kernel/SynDFCluster.h \
 ../../../../src/kernel/Cluster.h ../../../../src/kernel/GraphUtils.h
ACSWormhole.o: ACSWormhole.cc ACSWormhole.h \
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
 ../../../../src/kernel/EventHorizon.h ACSStar.h \
 ../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/kernel/DataFlowStar.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h \
 ../../../../src/domains/cg/kernel/CGTarget.h \
 ../../../../src/kernel/IntState.h \
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
 ../../../../src/domains/cg/kernel/CodeBlock.h ACSPortHole.h \
 ACSGeodesic.h ../../../../src/domains/cg/kernel/CGGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/PrecisionState.h ACSCGFPGACore.h \
 ../../../../src/domains/acs/targets/StringArray.h \
 ../../../../src/domains/acs/targets/Pipealign.h \
 ../../../../src/domains/acs/targets/ACSIntArray.h \
 ../../../../src/domains/acs/targets/acs_starconsts.h \
 ../../../../src/domains/acs/targets/Pin.h \
 ../../../../src/domains/acs/targets/Connectivity.h \
 ../../../../src/domains/acs/targets/Delay.h \
 ../../../../src/domains/acs/targets/Capability.h \
 ../../../../src/domains/acs/targets/Constants.h \
 ../../../../src/domains/acs/targets/Resource.h \
 ../../../../src/domains/acs/targets/acs_vhdl_lang.h \
 ../../../../src/domains/acs/kernel/ACSCGFPGACore.h ACSCGCore.h \
 ACSCore.h ACSCorona.h ACSKnownCategory.h
ACSKnownCategory.o: ACSKnownCategory.cc \
 ../../../../src/kernel/DataStruct.h ../../../../src/kernel/type.h \
 ../../../../src/kernel/logNew.h ACSKnownCategory.h ACSCore.h \
 ACSStar.h ../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/isa.h ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/PortHole.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/ComplexSubset.h \
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
 ../../../../src/domains/cg/kernel/CodeBlock.h ACSPortHole.h \
 ACSGeodesic.h ../../../../src/domains/cg/kernel/CGGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/PrecisionState.h ACSCorona.h
ACSFPSimCore.o: ACSFPSimCore.cc ACSFPSimCore.h ACSCore.h ACSStar.h \
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
 ../../../../src/domains/cg/kernel/CodeBlock.h ACSPortHole.h \
 ACSGeodesic.h ../../../../src/domains/cg/kernel/CGGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/PrecisionState.h ACSCorona.h \
 ACSKnownCategory.h
ACSFixSimCore.o: ACSFixSimCore.cc ACSFixSimCore.h ACSCore.h ACSStar.h \
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
 ../../../../src/domains/cg/kernel/CodeBlock.h ACSPortHole.h \
 ACSGeodesic.h ../../../../src/domains/cg/kernel/CGGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/PrecisionState.h ACSCorona.h \
 ACSKnownCategory.h
ACSCGCore.o: ACSCGCore.cc ../../../../src/kernel/Attribute.h \
 ACSCGCore.h ACSCore.h ACSStar.h \
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
 ../../../../src/kernel/State.h ../../../../src/kernel/InvokeInterp.h \
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
 ../../../../src/domains/cg/kernel/CodeBlock.h ACSPortHole.h \
 ACSGeodesic.h ../../../../src/domains/cg/kernel/CGGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/PrecisionState.h ACSCorona.h \
 ACSKnownCategory.h ../../../../src/kernel/FixState.h \
 ../../../../src/kernel/FixArrayState.h \
 ../../../../src/kernel/Tokenizer.h \
 ../../../../src/kernel/ComplexState.h \
 ../../../../src/domains/cg/kernel/CGUtilities.h
ACSCGCCore.o: ACSCGCCore.cc ACSCGCCore.h ACSCGCore.h ACSCore.h \
 ACSStar.h ../../../../src/domains/cg/kernel/CGStar.h \
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
 ../../../../src/domains/cg/kernel/CodeBlock.h ACSPortHole.h \
 ACSGeodesic.h ../../../../src/domains/cg/kernel/CGGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/PrecisionState.h ACSCorona.h \
 ACSKnownCategory.h ../../../../src/kernel/Tokenizer.h \
 ../../../../src/kernel/ComplexState.h \
 ../../../../src/kernel/FixState.h \
 ../../../../src/kernel/FixArrayState.h \
 ../../../../src/domains/cg/kernel/CGUtilities.h
ACSGeodesic.o: ACSGeodesic.cc ACSGeodesic.h \
 ../../../../src/domains/cg/kernel/CGGeodesic.h \
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
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h ACSPortHole.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/kernel/State.h ../../../../src/kernel/InvokeInterp.h
ACSFPCGCCore.o: ACSFPCGCCore.cc ACSFPCGCCore.h ACSCGCCore.h \
 ACSCGCore.h ACSCore.h ACSStar.h \
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
 ../../../../src/domains/cg/kernel/CodeBlock.h ACSPortHole.h \
 ACSGeodesic.h ../../../../src/domains/cg/kernel/CGGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/PrecisionState.h ACSCorona.h \
 ACSKnownCategory.h
ACSForkNode.o: ACSForkNode.cc ACSForkNode.h ACSGeodesic.h \
 ../../../../src/domains/cg/kernel/CGGeodesic.h \
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
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/AutoFork.h
ACSCGFPGACore.o: ACSCGFPGACore.cc ../../../../src/kernel/Attribute.h \
 ACSCGFPGACore.h ../../../../src/domains/acs/targets/StringArray.h \
 ../../../../src/domains/acs/targets/Pipealign.h \
 ../../../../src/domains/acs/targets/ACSIntArray.h \
 ../../../../src/domains/acs/targets/acs_starconsts.h \
 ../../../../src/domains/acs/targets/Pin.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h \
 ../../../../src/domains/acs/targets/Connectivity.h \
 ../../../../src/domains/acs/targets/Delay.h \
 ../../../../src/domains/acs/targets/Capability.h \
 ../../../../src/domains/acs/targets/Constants.h \
 ../../../../src/domains/acs/targets/Resource.h \
 ../../../../src/domains/acs/targets/acs_vhdl_lang.h \
 ../../../../src/domains/acs/kernel/ACSCGFPGACore.h ACSCGCore.h \
 ACSCore.h ACSStar.h ../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/isa.h ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/PortHole.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/ComplexSubset.h ../../../../src/kernel/State.h \
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
 ../../../../src/domains/cg/kernel/CodeBlock.h ACSPortHole.h \
 ACSGeodesic.h ../../../../src/domains/cg/kernel/CGGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/PrecisionState.h ACSCorona.h \
 ACSKnownCategory.h ACSWormhole.h ../../../../src/kernel/Wormhole.h \
 ../../../../src/kernel/Universe.h ../../../../src/kernel/Galaxy.h \
 ../../../../src/kernel/EventHorizon.h \
 ../../../../src/kernel/FixState.h \
 ../../../../src/kernel/FixArrayState.h \
 ../../../../src/kernel/Tokenizer.h \
 ../../../../src/kernel/ComplexState.h \
 ../../../../src/domains/cg/kernel/CGUtilities.h
ACSDomain.o: ACSDomain.cc ../../../../src/kernel/Domain.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/Block.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h ../../../../src/kernel/Target.h \
 ../../../../src/kernel/Galaxy.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/KnownTarget.h ACSScheduler.h \
 ../../../../src/kernel/Scheduler.h \
 ../../../../src/kernel/SimControl.h ACSWormhole.h \
 ../../../../src/kernel/Wormhole.h ../../../../src/kernel/Universe.h \
 ../../../../src/kernel/EventHorizon.h ACSStar.h \
 ../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/kernel/DataFlowStar.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h \
 ../../../../src/domains/cg/kernel/CGTarget.h \
 ../../../../src/kernel/IntState.h \
 ../../../../src/kernel/StringState.h \
 ../../../../src/kernel/KnownState.h \
 ../../../../src/domains/cg/kernel/SymbolList.h \
 ../../../../src/domains/cg/kernel/NamedList.h \
 ../../../../src/domains/cg/kernel/CodeStreamList.h \
 ../../../../src/domains/cg/kernel/CodeStream.h \
 ../../../../src/domains/cg/kernel/UniqueStringList.h \
 ../../../../src/domains/cg/kernel/ImplementationCost.h \
 ../../../../src/domains/cg/kernel/ConversionTable.h \
 ../../../../src/domains/cg/kernel/CodeBlock.h ACSPortHole.h \
 ACSGeodesic.h ../../../../src/domains/cg/kernel/CGGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/PrecisionState.h ACSCGFPGACore.h \
 ../../../../src/domains/acs/targets/StringArray.h \
 ../../../../src/domains/acs/targets/Pipealign.h \
 ../../../../src/domains/acs/targets/ACSIntArray.h \
 ../../../../src/domains/acs/targets/acs_starconsts.h \
 ../../../../src/domains/acs/targets/Pin.h \
 ../../../../src/domains/acs/targets/Connectivity.h \
 ../../../../src/domains/acs/targets/Delay.h \
 ../../../../src/domains/acs/targets/Capability.h \
 ../../../../src/domains/acs/targets/Constants.h \
 ../../../../src/domains/acs/targets/Resource.h \
 ../../../../src/domains/acs/targets/acs_vhdl_lang.h \
 ../../../../src/domains/acs/kernel/ACSCGFPGACore.h ACSCGCore.h \
 ACSCore.h ACSCorona.h ACSKnownCategory.h ACSForkNode.h \
 ../../../../src/kernel/AutoFork.h ACSTarget.h \
 ../../../../src/domains/cg/kernel/HLLTarget.h \
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/GalIter.h ../../../../src/kernel/FloatState.h
