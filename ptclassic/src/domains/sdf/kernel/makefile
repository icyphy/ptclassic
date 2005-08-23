# Makefile for SDF domain support
# Version identification:
# @(#)make.template	2.22	01/05/99
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
VPATH = $(ROOT)/src/domains/sdf/kernel

# get configuration info
CONFIG=$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)
INCL = -I$(KERNDIR) -I$(PTCL_INCDIR)

# Library name
PTLIB=libsdf.$(LIBSUFFIX)

# Used to create .dlls under nt4.vc
DLLDEPS =     $(KERNEL_DLLDEP) $(PTMATHEMATICA_DLLDEP) $(PTCL_DESCRIPTION_OBJ)

# Header files
HDRS= DynDFScheduler.h EGGate.h EGNode.h ExpandedGraph.h \
 SDFPortHole.h SDFScheduler.h SDFStar.h SDFWormhole.h \
 SynDFCluster.h AcyCluster.h AcyLoopScheduler.h \
 SDFCluster.h SDFTarget.h SDFPTclTarget.h WriteASCIIFiles.h

# Library sources
SRCS= $(HDRS:.h=.cc) SDFDomain.cc

# Library objects
OBJS= $(SRCS:.cc=.o)

all:	makefile $(PTLIB)

# "make install" puts things in the official place
install:	makefile $(LIBDIR)/$(PTLIB)



# include common definitions, rules
include $(ROOT)/mk/common.mk


# Don't add anything after the next line; makedepend will zap it.
# DO NOT DELETE THIS LINE -- make depend depends on it.

DynDFScheduler.o: DynDFScheduler.cc DynDFScheduler.h \
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
 ../../../../src/kernel/DataFlowStar.h \
 ../../../../src/kernel/Fraction.h ../../../../src/kernel/GalIter.h \
 SDFScheduler.h ../../../../src/kernel/DFPortHole.h \
 ../../../../src/kernel/checkConnect.h
EGGate.o: EGGate.cc EGGate.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/DoubleLink.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/PortHole.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/isa.h ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h EGNode.h SDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/Fraction.h SDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h
EGNode.o: EGNode.cc EGNode.h SDFStar.h \
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
 ../../../../src/kernel/Fraction.h SDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h \
 ../../../../src/kernel/DoubleLink.h EGGate.h
ExpandedGraph.o: ExpandedGraph.cc ExpandedGraph.h EGNode.h SDFStar.h \
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
 ../../../../src/kernel/Fraction.h SDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h \
 ../../../../src/kernel/DoubleLink.h EGGate.h \
 ../../../../src/kernel/GalIter.h ../../../../src/kernel/Galaxy.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h
SDFPortHole.o: SDFPortHole.cc SDFStar.h \
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
 ../../../../src/kernel/Fraction.h SDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h \
 ../../../../src/kernel/CircularBuffer.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h
SDFScheduler.o: SDFScheduler.cc ../../../../src/kernel/type.h \
 ../../../../src/kernel/logNew.h SDFScheduler.h \
 ../../../../src/kernel/Scheduler.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/SimControl.h ../../../../src/kernel/Galaxy.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/GalIter.h \
 ../../../../src/kernel/DataFlowStar.h \
 ../../../../src/kernel/Fraction.h ../../../../src/kernel/DFPortHole.h \
 SDFStar.h SDFPortHole.h ../../../../src/kernel/FloatState.h \
 ../../../../src/kernel/Target.h ../../../../src/kernel/EventHorizon.h \
 ../../../../src/kernel/checkConnect.h
SDFStar.o: SDFStar.cc SDFStar.h ../../../../src/kernel/DataFlowStar.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/Fraction.h SDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h
SDFWormhole.o: SDFWormhole.cc SDFWormhole.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/Wormhole.h \
 ../../../../src/kernel/Universe.h ../../../../src/kernel/Galaxy.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h ../../../../src/kernel/Target.h \
 ../../../../src/kernel/EventHorizon.h SDFStar.h \
 ../../../../src/kernel/DataFlowStar.h \
 ../../../../src/kernel/Fraction.h SDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h SDFScheduler.h \
 ../../../../src/kernel/Scheduler.h \
 ../../../../src/kernel/SimControl.h ../../../../src/kernel/GalIter.h
SynDFCluster.o: SynDFCluster.cc SynDFCluster.h \
 ../../../../src/kernel/Cluster.h \
 ../../../../src/kernel/DynamicGalaxy.h \
 ../../../../src/kernel/Galaxy.h ../../../../src/kernel/Star.h \
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
 ../../../../src/kernel/GraphUtils.h ../../../../src/kernel/GalIter.h \
 ../../../../src/kernel/DataFlowStar.h \
 ../../../../src/kernel/Fraction.h ../../../../src/kernel/DFPortHole.h
AcyCluster.o: AcyCluster.cc AcyCluster.h SynDFCluster.h \
 ../../../../src/kernel/Cluster.h \
 ../../../../src/kernel/DynamicGalaxy.h \
 ../../../../src/kernel/Galaxy.h ../../../../src/kernel/Star.h \
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
 ../../../../src/kernel/GraphUtils.h ../../../../src/kernel/GalIter.h
AcyLoopScheduler.o: AcyLoopScheduler.cc AcyLoopScheduler.h \
 SDFScheduler.h ../../../../src/kernel/Scheduler.h \
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
 SDFCluster.h SDFStar.h SDFPortHole.h \
 ../../../../src/kernel/DynamicGalaxy.h AcyCluster.h SynDFCluster.h \
 ../../../../src/kernel/Cluster.h ../../../../src/kernel/GraphUtils.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/Target.h ../../../../src/kernel/pt_fstream.h
SDFCluster.o: SDFCluster.cc SDFCluster.h SDFStar.h \
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
 ../../../../src/kernel/Fraction.h SDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h \
 ../../../../src/kernel/DynamicGalaxy.h \
 ../../../../src/kernel/Galaxy.h SDFScheduler.h \
 ../../../../src/kernel/Scheduler.h \
 ../../../../src/kernel/SimControl.h ../../../../src/kernel/GalIter.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/Plasma.h ../../../../src/kernel/PtGate.h \
 ../../../../src/kernel/Target.h ../../../../src/kernel/pt_fstream.h
SDFTarget.o: SDFTarget.cc SDFTarget.h ../../../../src/kernel/Target.h \
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
 ../../../../src/kernel/StringState.h \
 ../../../../src/kernel/KnownState.h ../../../../src/kernel/IntState.h \
 ../../../../src/kernel/FloatState.h SDFScheduler.h \
 ../../../../src/kernel/Scheduler.h \
 ../../../../src/kernel/SimControl.h ../../../../src/kernel/Galaxy.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/GalIter.h \
 ../../../../src/kernel/DataFlowStar.h \
 ../../../../src/kernel/Fraction.h ../../../../src/kernel/DFPortHole.h \
 SDFCluster.h SDFStar.h SDFPortHole.h \
 ../../../../src/kernel/DynamicGalaxy.h \
 ../../../../src/kernel/pt_fstream.h AcyLoopScheduler.h AcyCluster.h \
 SynDFCluster.h ../../../../src/kernel/Cluster.h \
 ../../../../src/kernel/GraphUtils.h \
 ../../../../src/kernel/Tokenizer.h
SDFPTclTarget.o: SDFPTclTarget.cc ../../../../src/kernel/miscFuncs.h \
 ../../../../src/kernel/logNew.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/Scheduler.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/DataStruct.h ../../../../src/kernel/type.h \
 ../../../../src/kernel/SimControl.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/isa.h ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/PortHole.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h ../../../../src/kernel/Galaxy.h \
 ../../../../src/kernel/GalIter.h ../../../../src/kernel/DFPortHole.h \
 ../../../../src/kernel/KnownTarget.h \
 ../../../../src/kernel/ProfileTimer.h \
 ../../../../src/kernel/TimeVal.h \
 ../../../../src/ptcl/ptclDescription.h SDFTarget.h \
 ../../../../src/kernel/Target.h ../../../../src/kernel/StringState.h \
 ../../../../src/kernel/KnownState.h ../../../../src/kernel/IntState.h \
 ../../../../src/kernel/FloatState.h SDFPTclTarget.h
WriteASCIIFiles.o: WriteASCIIFiles.cc ../../../../src/kernel/type.h \
 ../../../../src/kernel/logNew.h ../../../../src/kernel/miscFuncs.h \
 ../../../../src/kernel/Error.h WriteASCIIFiles.h
SDFDomain.o: SDFDomain.cc ../../../../src/kernel/Domain.h \
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
 ../../../../src/kernel/KnownTarget.h SDFTarget.h \
 ../../../../src/kernel/Target.h ../../../../src/kernel/StringState.h \
 ../../../../src/kernel/KnownState.h ../../../../src/kernel/IntState.h \
 ../../../../src/kernel/FloatState.h SDFWormhole.h \
 ../../../../src/kernel/Wormhole.h ../../../../src/kernel/Universe.h \
 ../../../../src/kernel/Galaxy.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/EventHorizon.h SDFStar.h \
 ../../../../src/kernel/DataFlowStar.h \
 ../../../../src/kernel/Fraction.h SDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h \
 ../../../../src/kernel/AutoForkNode.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/AutoFork.h SDFPTclTarget.h \
 ../../../../src/kernel/GalIter.h
