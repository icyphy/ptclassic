# @(#)make.template	1.15	04/14/98
# Copyright (c) 1990-1995 The Regents of the University of California.
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
# For creating an SDF loop scheduler.

SRCS =	LSNode.cc LSCluster.cc LSGraph.cc LoopScheduler.cc \
		MergeList.cc ClusterNodeList.cc \
		BooleanMatrix.cc ReachabilityMatrix.cc \
		DecomGal.cc DecomScheduler.cc LoopTarget.cc
HDRS =	LSNode.h LSCluster.h LSGraph.h LoopScheduler.h \
		MergeList.h ClusterNodeList.h \
		BooleanMatrix.h ReachabilityMatrix.h \
		DecomGal.h DecomScheduler.h

# Root of Ptolemy directory
ROOT=../../../..
# True source directory
VPATH = $(ROOT)/src/domains/sdf/loopScheduler

# get configuration info
CONFIG=$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

SDFKERNEL = $(ROOT)/src/domains/sdf/kernel
INCL =-I$(SDFKERNEL) -I$(KERNDIR)

OBJS = $(SRCS:.cc=.o)
PTLIB = libls.$(LIBSUFFIX)

all:	makefile $(PTLIB) 

# "make install" puts things in the official place
install:	all $(LIBDIR)/$(PTLIB)


# include common definitions, rules
include $(ROOT)/mk/common.mk


# Don't add anything after the next line; makedepend will zap it.
# DO NOT DELETE THIS LINE -- make depend depends on it.

LSNode.o: LSNode.cc LSNode.h \
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
 ../../../../src/domains/sdf/kernel/EGGate.h
LSCluster.o: LSCluster.cc LSCluster.h LSNode.h \
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
 ../../../../src/domains/sdf/kernel/SDFCluster.h \
 ../../../../src/kernel/DynamicGalaxy.h \
 ../../../../src/kernel/Galaxy.h \
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/Scheduler.h \
 ../../../../src/kernel/SimControl.h ../../../../src/kernel/GalIter.h \
 LSGraph.h ../../../../src/domains/sdf/kernel/ExpandedGraph.h \
 ReachabilityMatrix.h ClusterNodeList.h \
 ../../../../src/kernel/Target.h
LSGraph.o: LSGraph.cc LSGraph.h \
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
 ../../../../src/domains/sdf/kernel/EGGate.h ReachabilityMatrix.h \
 ../../../../src/kernel/Galaxy.h LSNode.h LSCluster.h \
 ../../../../src/domains/sdf/kernel/SDFCluster.h \
 ../../../../src/kernel/DynamicGalaxy.h \
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/Scheduler.h \
 ../../../../src/kernel/SimControl.h ../../../../src/kernel/GalIter.h \
 ClusterNodeList.h
LoopScheduler.o: LoopScheduler.cc LoopScheduler.h \
 ../../../../src/domains/sdf/kernel/SDFCluster.h \
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
 ../../../../src/kernel/DynamicGalaxy.h \
 ../../../../src/kernel/Galaxy.h \
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/Scheduler.h \
 ../../../../src/kernel/SimControl.h ../../../../src/kernel/GalIter.h \
 DecomGal.h DecomScheduler.h LSGraph.h \
 ../../../../src/domains/sdf/kernel/ExpandedGraph.h \
 ../../../../src/domains/sdf/kernel/EGNode.h \
 ../../../../src/kernel/DoubleLink.h \
 ../../../../src/domains/sdf/kernel/EGGate.h ReachabilityMatrix.h \
 LSNode.h LSCluster.h ../../../../src/kernel/Target.h \
 ../../../../src/kernel/pt_fstream.h
MergeList.o: MergeList.cc MergeList.h \
 ../../../../src/kernel/DoubleLink.h ../../../../src/kernel/type.h \
 ../../../../src/kernel/logNew.h LSNode.h \
 ../../../../src/domains/sdf/kernel/EGNode.h \
 ../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/DataStruct.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h \
 ../../../../src/domains/sdf/kernel/EGGate.h LSCluster.h \
 ../../../../src/domains/sdf/kernel/SDFCluster.h \
 ../../../../src/kernel/DynamicGalaxy.h \
 ../../../../src/kernel/Galaxy.h \
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/Scheduler.h \
 ../../../../src/kernel/SimControl.h ../../../../src/kernel/GalIter.h \
 LSGraph.h ../../../../src/domains/sdf/kernel/ExpandedGraph.h \
 ReachabilityMatrix.h ClusterNodeList.h
ClusterNodeList.o: ClusterNodeList.cc ClusterNodeList.h \
 ../../../../src/kernel/DataStruct.h ../../../../src/kernel/type.h \
 ../../../../src/kernel/logNew.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 LSNode.h ../../../../src/domains/sdf/kernel/EGNode.h \
 ../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/isa.h ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/PortHole.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h \
 ../../../../src/kernel/DoubleLink.h \
 ../../../../src/domains/sdf/kernel/EGGate.h LSGraph.h \
 ../../../../src/domains/sdf/kernel/ExpandedGraph.h \
 ReachabilityMatrix.h ../../../../src/kernel/Galaxy.h LSCluster.h \
 ../../../../src/domains/sdf/kernel/SDFCluster.h \
 ../../../../src/kernel/DynamicGalaxy.h \
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/Scheduler.h \
 ../../../../src/kernel/SimControl.h ../../../../src/kernel/GalIter.h
BooleanMatrix.o: BooleanMatrix.cc BooleanMatrix.h \
 ../../../../src/kernel/logNew.h
ReachabilityMatrix.o: ReachabilityMatrix.cc ReachabilityMatrix.h \
 BooleanMatrix.h ../../../../src/domains/sdf/kernel/EGGate.h \
 ../../../../src/kernel/DataStruct.h ../../../../src/kernel/type.h \
 ../../../../src/kernel/logNew.h ../../../../src/kernel/DoubleLink.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/PortHole.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/isa.h ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h LSNode.h \
 ../../../../src/domains/sdf/kernel/EGNode.h \
 ../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h LSGraph.h \
 ../../../../src/domains/sdf/kernel/ExpandedGraph.h \
 ../../../../src/kernel/Galaxy.h LSCluster.h \
 ../../../../src/domains/sdf/kernel/SDFCluster.h \
 ../../../../src/kernel/DynamicGalaxy.h \
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/Scheduler.h \
 ../../../../src/kernel/SimControl.h ../../../../src/kernel/GalIter.h \
 ClusterNodeList.h
DecomGal.o: DecomGal.cc DecomGal.h \
 ../../../../src/domains/sdf/kernel/SDFCluster.h \
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
 ../../../../src/kernel/DynamicGalaxy.h \
 ../../../../src/kernel/Galaxy.h \
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/Scheduler.h \
 ../../../../src/kernel/SimControl.h ../../../../src/kernel/GalIter.h \
 DecomScheduler.h LSGraph.h \
 ../../../../src/domains/sdf/kernel/ExpandedGraph.h \
 ../../../../src/domains/sdf/kernel/EGNode.h \
 ../../../../src/kernel/DoubleLink.h \
 ../../../../src/domains/sdf/kernel/EGGate.h ReachabilityMatrix.h \
 LSNode.h LSCluster.h
DecomScheduler.o: DecomScheduler.cc DecomScheduler.h \
 ../../../../src/domains/sdf/kernel/SDFCluster.h \
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
 ../../../../src/kernel/DynamicGalaxy.h \
 ../../../../src/kernel/Galaxy.h \
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/Scheduler.h \
 ../../../../src/kernel/SimControl.h ../../../../src/kernel/GalIter.h \
 LSGraph.h ../../../../src/domains/sdf/kernel/ExpandedGraph.h \
 ../../../../src/domains/sdf/kernel/EGNode.h \
 ../../../../src/kernel/DoubleLink.h \
 ../../../../src/domains/sdf/kernel/EGGate.h ReachabilityMatrix.h \
 LSNode.h LSCluster.h DecomGal.h ../../../../src/kernel/Target.h \
 MergeList.h
LoopTarget.o: LoopTarget.cc ../../../../src/kernel/Target.h \
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
 ../../../../src/kernel/KnownTarget.h LoopScheduler.h \
 ../../../../src/domains/sdf/kernel/SDFCluster.h \
 ../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h \
 ../../../../src/kernel/DynamicGalaxy.h \
 ../../../../src/kernel/Galaxy.h \
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/Scheduler.h \
 ../../../../src/kernel/SimControl.h ../../../../src/kernel/GalIter.h \
 ../../../../src/kernel/StringState.h \
 ../../../../src/kernel/KnownState.h \
 ../../../../src/kernel/FloatState.h
