# @(#)make.template	1.16	9/2/96
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
# Code generation targets


# Root of Ptolemy directory
ROOT=../../../..
# True source directory
VPATH = $(ROOT)/src/domains/cg/targets

# get configuration info
CONFIG=$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

SDFKERNEL = $(ROOT)/src/domains/sdf/kernel
BDFKERNEL = $(ROOT)/src/domains/bdf/kernel
CGKERNEL = $(ROOT)/src/domains/cg/kernel
CGSTARS = $(ROOT)/src/domains/cg/stars
LSKERNEL = $(ROOT)/src/domains/sdf/loopScheduler
DCDIR = $(ROOT)/src/domains/cg/dcScheduler
DLDIR = $(ROOT)/src/domains/cg/dlScheduler
HuDIR = $(ROOT)/src/domains/cg/HuScheduler
HierDIR = $(ROOT)/src/domains/cg/hierScheduler
PARDIR = $(ROOT)/src/domains/cg/parScheduler
MACRODIR = $(ROOT)/src/domains/cg/macroScheduler
CGDDFDIR = $(ROOT)/src/domains/cg/ddfScheduler

INCL=-I$(CGSTARS) -I$(CGKERNEL) -I$(BDFKERNEL) -I$(SDFKERNEL) -I$(KERNDIR) \
	-I$(LSKERNEL) -I$(DCDIR)  -I$(DLDIR) -I$(HuDIR) -I$(PARDIR) \
	-I$(HierDIR) -I$(MACRODIR) -I$(CGDDFDIR) -I$(PTCL_INCDIR)

SRCS=CGMultiTarget.cc CGSharedBus.cc CGBDFTarget.cc CGCostTarget.cc
HDRS=$(SRCS:.cc=.h)

OBJS = $(SRCS:.cc=.o)

all:	makefile $(OBJS)

# "make install" puts things in the official place
install:	all
		@echo Installation complete.

PTLIB=none	# required by common.mk

# include common definitions, rules
include $(ROOT)/mk/common.mk

# Don't add anything after the next line; makedepend will zap it.
# DO NOT DELETE THIS LINE -- make depend depends on it.

CGMultiTarget.o: CGMultiTarget.cc ../../../../src/kernel/Domain.h \
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
 ../../../../src/domains/cg/kernel/Profile.h \
 ../../../../src/domains/cg/kernel/IntArray.h \
 ../../../../src/domains/cg/parScheduler/ParNode.h \
 ../../../../src/domains/sdf/kernel/EGNode.h \
 ../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
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
 ../../../../src/domains/cg/parScheduler/ParProcessors.h \
 ../../../../src/domains/cg/parScheduler/UniProcessor.h \
 ../../../../src/kernel/DynamicGalaxy.h \
 ../../../../src/kernel/Galaxy.h \
 ../../../../src/domains/cg/kernel/MultiTarget.h CGMultiTarget.h \
 ../../../../src/kernel/StringArrayState.h \
 ../../../../src/kernel/IntArrayState.h \
 ../../../../src/domains/cg/dlScheduler/DLScheduler.h \
 ../../../../src/domains/cg/parScheduler/ParScheduler.h \
 ../../../../src/kernel/pt_fstream.h \
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/Scheduler.h ../../../../src/kernel/GalIter.h \
 ../../../../src/domains/cg/parScheduler/ParGraph.h \
 ../../../../src/domains/cg/dlScheduler/DLParProcs.h \
 ../../../../src/domains/cg/dlScheduler/DLGraph.h \
 ../../../../src/domains/cg/dlScheduler/DLNode.h \
 ../../../../src/domains/cg/HuScheduler/HuScheduler.h \
 ../../../../src/domains/cg/HuScheduler/HuParProcs.h \
 ../../../../src/domains/cg/HuScheduler/HuGraph.h \
 ../../../../src/domains/cg/HuScheduler/HuNode.h \
 ../../../../src/domains/cg/dcScheduler/DeclustScheduler.h \
 ../../../../src/domains/cg/dcScheduler/DCArcList.h \
 ../../../../src/domains/cg/dcScheduler/DCNode.h \
 ../../../../src/domains/cg/dcScheduler/DCGraph.h \
 ../../../../src/domains/cg/dcScheduler/DCClusterList.h \
 ../../../../src/domains/cg/dcScheduler/DCCluster.h \
 ../../../../src/domains/cg/dcScheduler/DCClustArcList.h \
 ../../../../src/domains/cg/dcScheduler/DCParProcs.h \
 ../../../../src/domains/cg/ddfScheduler/CGDDFScheduler.h \
 ../../../../src/domains/cg/macroScheduler/MacroScheduler.h \
 ../../../../src/domains/cg/macroScheduler/MacroParProcs.h \
 ../../../../src/domains/cg/macroScheduler/MacroGraph.h \
 ../../../../src/kernel/KnownTarget.h \
 ../../../../src/domains/cg/stars/CGSpread.h \
 ../../../../src/domains/cg/stars/CGVarTime.h \
 ../../../../src/domains/cg/stars/CGCollect.h \
 ../../../../src/domains/cg/stars/CGSend.h \
 ../../../../src/domains/cg/stars/CGReceive.h \
 ../../../../src/domains/cg/kernel/CGWormBase.h \
 ../../../../src/kernel/Wormhole.h ../../../../src/kernel/Universe.h \
 ../../../../src/kernel/EventHorizon.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/domains/sdf/loopScheduler/BooleanMatrix.h \
 ../../../../src/domains/cg/hierScheduler/HierScheduler.h \
 ../../../../src/kernel/Cluster.h ../../../../src/kernel/GraphUtils.h
CGSharedBus.o: CGSharedBus.cc CGSharedBus.h CGMultiTarget.h \
 ../../../../src/domains/cg/kernel/MultiTarget.h \
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
 ../../../../src/domains/cg/kernel/IntArray.h \
 ../../../../src/kernel/StringArrayState.h \
 ../../../../src/kernel/IntArrayState.h \
 ../../../../src/domains/cg/parScheduler/UniProcessor.h \
 ../../../../src/domains/cg/parScheduler/ParNode.h \
 ../../../../src/domains/sdf/kernel/EGNode.h \
 ../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h \
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
 ../../../../src/kernel/Galaxy.h ../../../../src/kernel/KnownTarget.h
CGBDFTarget.o: CGBDFTarget.cc CGBDFTarget.h \
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
 ../../../../src/domains/bdf/kernel/BDFCluster.h \
 ../../../../src/domains/bdf/kernel/BDFStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/domains/bdf/kernel/BDFPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h \
 ../../../../src/kernel/DynamicGalaxy.h \
 ../../../../src/kernel/Galaxy.h \
 ../../../../src/domains/bdf/kernel/BDFScheduler.h \
 ../../../../src/domains/bdf/kernel/BDFBool.h \
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/Scheduler.h ../../../../src/kernel/GalIter.h \
 ../../../../src/kernel/KnownTarget.h
CGCostTarget.o: CGCostTarget.cc ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/Scheduler.h \
 ../../../../src/kernel/SimControl.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/isa.h ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/PortHole.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DFPortHole.h \
 ../../../../src/kernel/KnownTarget.h \
 ../../../../src/kernel/pt_fstream.h \
 ../../../../src/kernel/ProfileTimer.h \
 ../../../../src/kernel/TimeVal.h ../../../../src/kernel/KnownBlock.h \
 ../../../../src/kernel/Linker.h \
 ../../../../src/ptcl/ptclDescription.h \
 ../../../../src/kernel/Galaxy.h \
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/GalIter.h \
 ../../../../src/kernel/DataFlowStar.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/cg/kernel/CGUtilities.h CGCostTarget.h \
 ../../../../src/kernel/StringArrayState.h \
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
 ../../../../src/domains/cg/kernel/ConversionTable.h
