# Makefile for VHDL domain support
# Version identification:
# @(#)make.template	1.15 02/17/99
#
# Copyright (c) 1990-1999 The Regents of the University of California.
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
# If this file's name is "make.template" and there is no makefile in
# this directory, do
#	cp make.template makefile
#	make depend
# This will add the dependencies to the makefile.

# Root of Ptolemy directory
ROOT=../../../..
# True source directory
VPATH = $(ROOT)/src/domains/vhdl/kernel

# do this for Synopsys
ARCH = $(PTARCH)

# get configuration info
CONFIG=$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

# SDF directory
SDFDIR=$(ROOT)/src/domains/sdf/kernel

# CG directories
CGDIR=$(ROOT)/src/domains/cg/kernel

INCL=-I$(KERNDIR) -I$(CGDIR) -I$(SDFDIR)

# Library name
PTLIB=libvhdl.$(LIBSUFFIX)

# Header files
HDRS= \
	VHDLArc.h \
	VHDLCluster.h \
	VHDLCompDecl.h \
	VHDLDependency.h \
	VHDLFiring.h \
	VHDLFiregroup.h \
	VHDLForkNode.h \
	VHDLGeneric.h \
	VHDLGeodesic.h \
	VHDLMux.h \
	VHDLObj.h \
	VHDLPort.h  \
	VHDLPortHole.h \
	VHDLReg.h \
	VHDLSignal.h \
	VHDLStar.h \
	VHDLState.h \
	VHDLTarget.h \
	VHDLToken.h \
	VHDLTypedObj.h \
	VHDLVariable.h \
	VHDLWormhole.h \

# Library objects
OBJS= $(HDRS:.h=.o) VHDLDomain.o

# Library sources
SRCS= $(OBJS:.o=.cc)

all:	makefile $(PTLIB)

# "make install" puts things in the official place
install:	makefile $(LIBDIR)/$(PTLIB)

# include common definitions, rules
include $(ROOT)/mk/common.mk

# Don't add anything after the next line; makedepend will zap it.
# DO NOT DELETE THIS LINE -- make depend depends on it.
VHDLArc.o: VHDLArc.cc VHDLArc.h VHDLTypedObj.h VHDLObj.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/HashTable.h
VHDLCluster.o: VHDLCluster.cc VHDLCluster.h VHDLObj.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/HashTable.h \
 VHDLFiring.h VHDLGeneric.h VHDLTypedObj.h VHDLPort.h VHDLSignal.h \
 VHDLVariable.h
VHDLCompDecl.o: VHDLCompDecl.cc VHDLCompDecl.h VHDLObj.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/HashTable.h \
 VHDLPort.h VHDLTypedObj.h VHDLSignal.h VHDLVariable.h VHDLFiring.h \
 VHDLGeneric.h
VHDLDependency.o: VHDLDependency.cc VHDLDependency.h VHDLObj.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/HashTable.h
VHDLFiring.o: VHDLFiring.cc VHDLFiring.h VHDLObj.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/HashTable.h \
 VHDLGeneric.h VHDLTypedObj.h VHDLPort.h VHDLSignal.h VHDLVariable.h
VHDLFiregroup.o: VHDLFiregroup.cc VHDLFiregroup.h VHDLObj.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/HashTable.h \
 VHDLFiring.h VHDLGeneric.h VHDLTypedObj.h VHDLPort.h VHDLSignal.h \
 VHDLVariable.h
VHDLForkNode.o: VHDLForkNode.cc VHDLForkNode.h \
 ../../../../src/kernel/AutoFork.h VHDLGeodesic.h \
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
 ../../../../src/kernel/ParticleStack.h
VHDLGeneric.o: VHDLGeneric.cc VHDLGeneric.h VHDLTypedObj.h VHDLObj.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/HashTable.h
VHDLGeodesic.o: VHDLGeodesic.cc VHDLGeodesic.h \
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
 ../../../../src/kernel/ParticleStack.h VHDLPortHole.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/kernel/State.h ../../../../src/kernel/InvokeInterp.h
VHDLMux.o: VHDLMux.cc VHDLMux.h VHDLFiring.h VHDLObj.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/HashTable.h \
 VHDLGeneric.h VHDLTypedObj.h VHDLPort.h VHDLSignal.h VHDLVariable.h
VHDLObj.o: VHDLObj.cc VHDLObj.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/HashTable.h
VHDLPort.o: VHDLPort.cc VHDLPort.h VHDLTypedObj.h VHDLObj.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/HashTable.h \
 VHDLSignal.h VHDLVariable.h VHDLFiring.h VHDLGeneric.h
VHDLPortHole.o: VHDLPortHole.cc VHDLPortHole.h VHDLGeodesic.h \
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
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/kernel/State.h ../../../../src/kernel/InvokeInterp.h
VHDLReg.o: VHDLReg.cc VHDLReg.h VHDLFiring.h VHDLObj.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/HashTable.h \
 VHDLGeneric.h VHDLTypedObj.h VHDLPort.h VHDLSignal.h VHDLVariable.h
VHDLSignal.o: VHDLSignal.cc VHDLSignal.h VHDLTypedObj.h VHDLObj.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/HashTable.h \
 VHDLPort.h VHDLVariable.h VHDLFiring.h VHDLGeneric.h
VHDLStar.o: VHDLStar.cc VHDLStar.h \
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
 ../../../../src/domains/cg/kernel/CodeBlock.h VHDLPortHole.h \
 VHDLGeodesic.h ../../../../src/domains/cg/kernel/CGGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/kernel/Tokenizer.h VHDLTarget.h \
 ../../../../src/domains/cg/kernel/HLLTarget.h \
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/Scheduler.h ../../../../src/kernel/Galaxy.h \
 ../../../../src/kernel/GalIter.h VHDLFiring.h VHDLObj.h \
 ../../../../src/kernel/HashTable.h VHDLGeneric.h VHDLTypedObj.h \
 VHDLPort.h VHDLSignal.h VHDLVariable.h VHDLArc.h VHDLCompDecl.h \
 ../../../../src/kernel/FloatArrayState.h \
 ../../../../src/kernel/IntArrayState.h \
 ../../../../src/kernel/ComplexArrayState.h
VHDLState.o: VHDLState.cc VHDLState.h VHDLTypedObj.h VHDLObj.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/HashTable.h
VHDLTarget.o: VHDLTarget.cc VHDLTarget.h \
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
 VHDLStar.h ../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/domains/cg/kernel/CodeBlock.h VHDLPortHole.h \
 VHDLGeodesic.h ../../../../src/domains/cg/kernel/CGGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/kernel/Tokenizer.h VHDLFiring.h VHDLObj.h \
 ../../../../src/kernel/HashTable.h VHDLGeneric.h VHDLTypedObj.h \
 VHDLPort.h VHDLSignal.h VHDLVariable.h VHDLArc.h VHDLCompDecl.h \
 ../../../../src/kernel/FloatArrayState.h \
 ../../../../src/kernel/IntArrayState.h \
 ../../../../src/kernel/ComplexArrayState.h \
 ../../../../src/domains/cg/kernel/CGUtilities.h
VHDLToken.o: VHDLToken.cc VHDLToken.h VHDLTypedObj.h VHDLObj.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/HashTable.h \
 VHDLFiring.h VHDLGeneric.h VHDLPort.h VHDLSignal.h VHDLVariable.h
VHDLTypedObj.o: VHDLTypedObj.cc VHDLTypedObj.h VHDLObj.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/HashTable.h
VHDLVariable.o: VHDLVariable.cc VHDLVariable.h VHDLTypedObj.h \
 VHDLObj.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/HashTable.h
VHDLWormhole.o: VHDLWormhole.cc VHDLWormhole.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h \
 ../../../../src/domains/cg/kernel/CGWormhole.h \
 ../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/NamedObj.h \
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
 ../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../src/domains/cg/kernel/CGWormBase.h \
 ../../../../src/kernel/Wormhole.h ../../../../src/kernel/Universe.h \
 ../../../../src/kernel/Galaxy.h ../../../../src/kernel/EventHorizon.h \
 ../../../../src/domains/cg/kernel/Profile.h \
 ../../../../src/domains/cg/kernel/IntArray.h \
 ../../../../src/domains/cg/kernel/MultiTarget.h VHDLStar.h \
 VHDLPortHole.h VHDLGeodesic.h \
 ../../../../src/domains/cg/kernel/CGGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/kernel/Tokenizer.h \
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/Scheduler.h ../../../../src/kernel/GalIter.h
VHDLDomain.o: VHDLDomain.cc ../../../../src/kernel/Domain.h \
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
 ../../../../src/kernel/KnownTarget.h VHDLForkNode.h \
 ../../../../src/kernel/AutoFork.h VHDLGeodesic.h \
 ../../../../src/domains/cg/kernel/CGGeodesic.h \
 ../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h VHDLTarget.h \
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
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/cg/kernel/ImplementationCost.h \
 ../../../../src/domains/cg/kernel/ConversionTable.h \
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/Scheduler.h ../../../../src/kernel/Galaxy.h \
 ../../../../src/kernel/GalIter.h VHDLStar.h \
 ../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/domains/cg/kernel/CodeBlock.h VHDLPortHole.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/kernel/Tokenizer.h VHDLFiring.h VHDLObj.h \
 ../../../../src/kernel/HashTable.h VHDLGeneric.h VHDLTypedObj.h \
 VHDLPort.h VHDLSignal.h VHDLVariable.h VHDLArc.h VHDLCompDecl.h \
 ../../../../src/kernel/FloatArrayState.h \
 ../../../../src/kernel/IntArrayState.h \
 ../../../../src/kernel/ComplexArrayState.h VHDLWormhole.h \
 ../../../../src/domains/cg/kernel/CGWormhole.h \
 ../../../../src/domains/cg/kernel/CGWormBase.h \
 ../../../../src/kernel/Wormhole.h ../../../../src/kernel/Universe.h \
 ../../../../src/kernel/EventHorizon.h \
 ../../../../src/domains/cg/kernel/Profile.h \
 ../../../../src/domains/cg/kernel/IntArray.h \
 ../../../../src/domains/cg/kernel/MultiTarget.h
