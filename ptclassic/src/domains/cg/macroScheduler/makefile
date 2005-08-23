# @(#)make.template	1.5	02/17/99
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
# macro scheduling


# Root of Ptolemy directory
ROOT=../../../..
# True source directory
VPATH = $(ROOT)/src/domains/cg/macroScheduler

# get configuration info
CONFIG = 	$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

SDFKERNEL = 	$(ROOT)/src/domains/sdf/kernel
CGKERNEL =	$(ROOT)/src/domains/cg/kernel
PARKERNEL =	$(ROOT)/src/domains/cg/parScheduler
DLKERNEL =	$(ROOT)/src/domains/cg/dlScheduler

INCL = 	-I$(SDFKERNEL) -I$(CGKERNEL) -I$(PARKERNEL) -I$(DLKERNEL) -I$(KERNDIR)

SRCS =	CGMacroCluster.cc MacroGraph.cc MacroScheduler.cc MacroParProcs.cc \
	CGMacroStar.cc

HDRS =  CGMacroCluster.h MacroGraph.h MacroScheduler.h MacroParProcs.h \
	CGMacroStar.h

OBJS =	$(SRCS:.cc=.o)

PTLIB =	libMacro.$(LIBSUFFIX)

all:	makefile $(PTLIB)

# "make install" puts things in the official place
install:	makefile $(LIBDIR)/$(PTLIB)

# include common definitions, rules
include $(ROOT)/mk/common.mk

# Don't add anything after the next line; makedepend will zap it.
# DO NOT DELETE THIS LINE -- make depend depends on it.

CGMacroCluster.o: CGMacroCluster.cc CGMacroCluster.h \
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
 ../../../../src/kernel/DynamicGalaxy.h \
 ../../../../src/kernel/Galaxy.h \
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/Scheduler.h ../../../../src/kernel/GalIter.h \
 ../../../../src/domains/cg/kernel/Profile.h \
 ../../../../src/domains/cg/kernel/IntArray.h \
 ../../../../src/domains/cg/kernel/MultiTarget.h MacroScheduler.h \
 ../../../../src/domains/cg/parScheduler/ParScheduler.h \
 ../../../../src/kernel/pt_fstream.h \
 ../../../../src/domains/cg/parScheduler/ParProcessors.h \
 ../../../../src/domains/cg/parScheduler/UniProcessor.h \
 ../../../../src/domains/cg/parScheduler/ParNode.h \
 ../../../../src/domains/sdf/kernel/EGNode.h \
 ../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../src/kernel/DoubleLink.h \
 ../../../../src/domains/sdf/kernel/EGGate.h \
 ../../../../src/domains/sdf/kernel/ExpandedGraph.h \
 ../../../../src/domains/cg/parScheduler/ParGraph.h MacroParProcs.h \
 ../../../../src/domains/cg/dlScheduler/DLParProcs.h \
 ../../../../src/domains/cg/dlScheduler/DLGraph.h \
 ../../../../src/domains/cg/dlScheduler/DLNode.h MacroGraph.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/Plasma.h ../../../../src/kernel/PtGate.h
MacroGraph.o: MacroGraph.cc MacroGraph.h \
 ../../../../src/domains/cg/dlScheduler/DLGraph.h \
 ../../../../src/domains/cg/parScheduler/ParGraph.h \
 ../../../../src/domains/sdf/kernel/ExpandedGraph.h \
 ../../../../src/domains/sdf/kernel/EGNode.h \
 ../../../../src/domains/sdf/kernel/SDFStar.h \
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
 ../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h \
 ../../../../src/kernel/DoubleLink.h \
 ../../../../src/domains/sdf/kernel/EGGate.h \
 ../../../../src/kernel/Galaxy.h \
 ../../../../src/domains/cg/parScheduler/ParNode.h \
 ../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
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
 ../../../../src/domains/cg/kernel/Profile.h \
 ../../../../src/domains/cg/kernel/IntArray.h \
 ../../../../src/domains/cg/dlScheduler/DLNode.h CGMacroCluster.h \
 ../../../../src/kernel/DynamicGalaxy.h \
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/Scheduler.h ../../../../src/kernel/GalIter.h \
 ../../../../src/domains/cg/kernel/MultiTarget.h MacroScheduler.h \
 ../../../../src/domains/cg/parScheduler/ParScheduler.h \
 ../../../../src/kernel/pt_fstream.h \
 ../../../../src/domains/cg/parScheduler/ParProcessors.h \
 ../../../../src/domains/cg/parScheduler/UniProcessor.h \
 MacroParProcs.h ../../../../src/domains/cg/dlScheduler/DLParProcs.h \
 ../../../../src/kernel/ConstIters.h \
 ../../../../src/kernel/KnownBlock.h ../../../../src/kernel/Linker.h
MacroScheduler.o: MacroScheduler.cc MacroScheduler.h \
 ../../../../src/domains/cg/parScheduler/ParScheduler.h \
 ../../../../src/kernel/pt_fstream.h \
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
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
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/GalIter.h \
 ../../../../src/kernel/DataFlowStar.h \
 ../../../../src/kernel/Fraction.h ../../../../src/kernel/DFPortHole.h \
 ../../../../src/domains/cg/kernel/MultiTarget.h \
 ../../../../src/domains/cg/kernel/CGTarget.h \
 ../../../../src/kernel/Target.h ../../../../src/kernel/IntState.h \
 ../../../../src/kernel/StringState.h \
 ../../../../src/kernel/KnownState.h \
 ../../../../src/domains/cg/kernel/SymbolList.h \
 ../../../../src/domains/cg/kernel/NamedList.h \
 ../../../../src/domains/cg/kernel/CodeStreamList.h \
 ../../../../src/domains/cg/kernel/CodeStream.h \
 ../../../../src/domains/cg/kernel/UniqueStringList.h \
 ../../../../src/domains/cg/kernel/ImplementationCost.h \
 ../../../../src/domains/cg/kernel/ConversionTable.h \
 ../../../../src/domains/cg/kernel/IntArray.h \
 ../../../../src/domains/cg/parScheduler/ParProcessors.h \
 ../../../../src/domains/cg/parScheduler/UniProcessor.h \
 ../../../../src/domains/cg/parScheduler/ParNode.h \
 ../../../../src/domains/sdf/kernel/EGNode.h \
 ../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../src/kernel/DoubleLink.h \
 ../../../../src/domains/sdf/kernel/EGGate.h \
 ../../../../src/domains/sdf/kernel/ExpandedGraph.h \
 ../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../src/domains/cg/kernel/Profile.h \
 ../../../../src/kernel/DynamicGalaxy.h \
 ../../../../src/domains/cg/parScheduler/ParGraph.h MacroParProcs.h \
 ../../../../src/domains/cg/dlScheduler/DLParProcs.h \
 ../../../../src/domains/cg/dlScheduler/DLGraph.h \
 ../../../../src/domains/cg/dlScheduler/DLNode.h MacroGraph.h \
 CGMacroCluster.h
MacroParProcs.o: MacroParProcs.cc MacroParProcs.h \
 ../../../../src/domains/cg/dlScheduler/DLParProcs.h \
 ../../../../src/domains/cg/parScheduler/ParProcessors.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/domains/cg/parScheduler/UniProcessor.h \
 ../../../../src/domains/cg/parScheduler/ParNode.h \
 ../../../../src/domains/sdf/kernel/EGNode.h \
 ../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h \
 ../../../../src/kernel/DoubleLink.h \
 ../../../../src/domains/sdf/kernel/EGGate.h \
 ../../../../src/domains/sdf/kernel/ExpandedGraph.h \
 ../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
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
 ../../../../src/domains/cg/kernel/Profile.h \
 ../../../../src/domains/cg/kernel/IntArray.h \
 ../../../../src/kernel/DynamicGalaxy.h \
 ../../../../src/kernel/Galaxy.h \
 ../../../../src/domains/cg/kernel/MultiTarget.h \
 ../../../../src/domains/cg/dlScheduler/DLGraph.h \
 ../../../../src/domains/cg/parScheduler/ParGraph.h \
 ../../../../src/domains/cg/dlScheduler/DLNode.h MacroGraph.h \
 CGMacroCluster.h ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/Scheduler.h ../../../../src/kernel/GalIter.h \
 MacroScheduler.h \
 ../../../../src/domains/cg/parScheduler/ParScheduler.h \
 ../../../../src/kernel/pt_fstream.h
CGMacroStar.o: CGMacroStar.cc CGMacroStar.h CGMacroCluster.h \
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
 ../../../../src/kernel/DynamicGalaxy.h \
 ../../../../src/kernel/Galaxy.h \
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/Scheduler.h ../../../../src/kernel/GalIter.h \
 ../../../../src/domains/cg/kernel/Profile.h \
 ../../../../src/domains/cg/kernel/IntArray.h \
 ../../../../src/domains/cg/kernel/MultiTarget.h MacroScheduler.h \
 ../../../../src/domains/cg/parScheduler/ParScheduler.h \
 ../../../../src/kernel/pt_fstream.h \
 ../../../../src/domains/cg/parScheduler/ParProcessors.h \
 ../../../../src/domains/cg/parScheduler/UniProcessor.h \
 ../../../../src/domains/cg/parScheduler/ParNode.h \
 ../../../../src/domains/sdf/kernel/EGNode.h \
 ../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../src/kernel/DoubleLink.h \
 ../../../../src/domains/sdf/kernel/EGGate.h \
 ../../../../src/domains/sdf/kernel/ExpandedGraph.h \
 ../../../../src/domains/cg/parScheduler/ParGraph.h MacroParProcs.h \
 ../../../../src/domains/cg/dlScheduler/DLParProcs.h \
 ../../../../src/domains/cg/dlScheduler/DLGraph.h \
 ../../../../src/domains/cg/dlScheduler/DLNode.h MacroGraph.h \
 ../../../../src/kernel/Domain.h
