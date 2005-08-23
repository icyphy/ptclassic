# Makefile for BDF domain support
# Version identification:
# @(#)make.template	2.13	08/21/96
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
# Date of creation: 1/8/91
#
# If this file's name is "make.template" and there is no makefile in
# this directory, do
#	cp make.template makefile
#	make depend
# This will add the dependencies to the makefile.

# Root of Ptolemy directory
ROOT=../../../..
# True source directory
VPATH = $(ROOT)/src/domains/bdf/kernel

# get configuration info
CONFIG=$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

# SDF domain
SDFDIR=$(ROOT)/src/domains/sdf/kernel

INCL=-I$(KERNDIR) -I$(SDFDIR)

# Library name
PTLIB=libbdf.$(LIBSUFFIX)

# Header files
HDRS= \
BDFBool.h \
BDFClustPort.h \
BDFCluster.h \
BDFPortHole.h \
BDFRelIter.h \
BDFSchList.h \
BDFScheduler.h \
BDFStar.h \
BDFTarget.h \
BDFWormhole.h

# Library sources
SRCS= $(HDRS:.h=.cc) BDFDomain.cc

# Library objects
OBJS= $(SRCS:.cc=.o)

all:	makefile $(PTLIB)

# "make install" puts things in the official place
install:	makefile $(LIBDIR)/$(PTLIB)

# include common definitions, rules
include $(ROOT)/mk/common.mk

# This is a GNU make conditional
#ifeq ($(PTARCH),hppa)
#BDFSchList.o: BDFSchList.cc
#	@echo "DANGER: serious hppa specific bug here."
#	@echo "If, under gcc-2.5.8, BDFSchList is compiled with optimization"
#	@echo "turned on, then at link time, the symbol:"
#	@echo "   info__C12BDFSchedulerRC8PortHole (data)"
#	@echo "will be undefined."
#	@echo "This symbol comes from void BDFTestNode::run(BDFScheduler& sch)"
#	@echo "   int test = sch.getBoolValue(port);"
#	@echo "So we compile this file without the optimizer on hppa only"
#	$(CPLUSPLUS) $(CC_SHAREDFLAGS) -DUSG -g $(WARNINGS) -I$(VPATH) $(INCL) -c $< 
#endif

# Alpha version seems to have the same problem

#ifeq ($(PTARCH),alpha)
#BDFSchList.o: BDFSchList.cc
#	@echo "DANGER: serious GNU compiler bug suspected here."
#	@echo "If, under gcc-2.6.0, BDFSchList is compiled with optimization"
#	@echo "turned on, then at link time, the symbol:"
#	@echo "   info__C12BDFSchedulerRC8PortHole (data)"
#	@echo "will be undefined."
#	@echo "This symbol comes from void BDFTestNode::run(BDFScheduler& sch)"
#	@echo "   int test = sch.getBoolValue(port);"
#	@echo "So we compile this file without the optimizer on alpha as well"
#	$(CPLUSPLUS) $(CC_SHAREDFLAGS) -DUSG -g $(WARNINGS) -I$(VPATH) $(INCL) -c $< 
#endif

# Don't add anything after the next line; makedepend will zap it.
# DO NOT DELETE THIS LINE -- make depend depends on it.

BDFBool.o: BDFBool.cc BDFBool.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h
BDFClustPort.o: BDFClustPort.cc BDFClustPort.h BDFPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h BDFCluster.h BDFStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/kernel/DynamicGalaxy.h \
 ../../../../src/kernel/Galaxy.h BDFScheduler.h BDFBool.h \
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/Scheduler.h \
 ../../../../src/kernel/SimControl.h ../../../../src/kernel/GalIter.h \
 BDFRelIter.h ../../../../src/kernel/Plasma.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/PtGate.h ../../../../src/kernel/Geodesic.h
BDFCluster.o: BDFCluster.cc BDFCluster.h BDFStar.h \
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
 ../../../../src/kernel/Fraction.h BDFPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h \
 ../../../../src/kernel/DynamicGalaxy.h \
 ../../../../src/kernel/Galaxy.h BDFScheduler.h BDFBool.h \
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/Scheduler.h \
 ../../../../src/kernel/SimControl.h ../../../../src/kernel/GalIter.h \
 BDFClustPort.h BDFRelIter.h ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/Target.h ../../../../src/kernel/pt_fstream.h \
 ../../../../src/domains/sdf/kernel/DynDFScheduler.h
BDFPortHole.o: BDFPortHole.cc ../../../../src/kernel/Block.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h BDFPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h \
 ../../../../src/kernel/CircularBuffer.h
BDFRelIter.o: BDFRelIter.cc BDFClustPort.h BDFPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h BDFCluster.h BDFStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/kernel/DynamicGalaxy.h \
 ../../../../src/kernel/Galaxy.h BDFScheduler.h BDFBool.h \
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/Scheduler.h \
 ../../../../src/kernel/SimControl.h ../../../../src/kernel/GalIter.h \
 BDFRelIter.h
BDFSchList.o: BDFSchList.cc BDFSchList.h BDFBool.h \
 ../../../../src/kernel/PortHole.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/Scheduler.h \
 ../../../../src/kernel/SimControl.h BDFScheduler.h BDFStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/kernel/DataFlowStar.h \
 ../../../../src/kernel/Fraction.h BDFPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h \
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/Galaxy.h ../../../../src/kernel/GalIter.h
BDFScheduler.o: BDFScheduler.cc ../../../../src/kernel/type.h \
 ../../../../src/kernel/logNew.h BDFScheduler.h BDFStar.h \
 ../../../../src/kernel/DynDFStar.h \
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
 ../../../../src/kernel/Fraction.h BDFPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h BDFBool.h \
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/Scheduler.h \
 ../../../../src/kernel/SimControl.h ../../../../src/kernel/Galaxy.h \
 ../../../../src/kernel/GalIter.h BDFSchList.h \
 ../../../../src/kernel/FloatState.h ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/ConstIters.h
BDFStar.o: BDFStar.cc BDFStar.h ../../../../src/kernel/DynDFStar.h \
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
 ../../../../src/kernel/Fraction.h BDFPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h \
 ../../../../src/kernel/SimControl.h \
 ../../../../src/kernel/textAnimate.h
BDFTarget.o: BDFTarget.cc BDFTarget.h ../../../../src/kernel/Target.h \
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
 ../../../../src/kernel/FloatState.h BDFScheduler.h BDFStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Fraction.h BDFPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h BDFBool.h \
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/Scheduler.h \
 ../../../../src/kernel/SimControl.h ../../../../src/kernel/Galaxy.h \
 ../../../../src/kernel/GalIter.h BDFCluster.h \
 ../../../../src/kernel/DynamicGalaxy.h \
 ../../../../src/kernel/pt_fstream.h
BDFWormhole.o: BDFWormhole.cc BDFWormhole.h \
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
 ../../../../src/kernel/EventHorizon.h BDFStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/kernel/DataFlowStar.h \
 ../../../../src/kernel/Fraction.h BDFPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h BDFScheduler.h BDFBool.h \
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/Scheduler.h \
 ../../../../src/kernel/SimControl.h ../../../../src/kernel/GalIter.h
BDFDomain.o: BDFDomain.cc ../../../../src/kernel/Domain.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/Block.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h BDFTarget.h \
 ../../../../src/kernel/Target.h ../../../../src/kernel/StringState.h \
 ../../../../src/kernel/KnownState.h ../../../../src/kernel/IntState.h \
 ../../../../src/kernel/FloatState.h \
 ../../../../src/kernel/KnownTarget.h BDFWormhole.h \
 ../../../../src/kernel/Wormhole.h ../../../../src/kernel/Universe.h \
 ../../../../src/kernel/Galaxy.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/EventHorizon.h BDFStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/kernel/DataFlowStar.h \
 ../../../../src/kernel/Fraction.h BDFPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h \
 ../../../../src/kernel/AutoForkNode.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/AutoFork.h
