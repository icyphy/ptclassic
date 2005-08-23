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
# Makefile for PN domain kernel
# Version @(#)make.template	2.5 1/23/96
# Copyright 1990-1996 The Regents of the University of California.
# All Rights Reserved.
# Author:	T. M. Parks
# Created:	6 February 1992

ROOT=../../../..
# True source directory
VPATH=$(ROOT)/src/domains/pn/kernel

# get configuration info
CONFIG=$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

INCL_THREADS=-I$(ROOT)/thread/include.$(PTARCH) -I$(ROOT)/src/thread/posix
SDFDIR=$(ROOT)/src/domains/sdf/kernel

INCL= $(INCL_THREADS) -I$(KERNDIR) -I$(SDFDIR)

# Library name
PTLIB=libpn.$(LIBSUFFIX)

# Library sources
CC_WITH_H=\
	PNTarget.cc PNScheduler.cc PNStar.cc\
	PNWormhole.cc PNEventHorizon.cc\
	PNPortHole.cc PNGeodesic.cc PNForkNode.cc\
	PNThread.cc

# Source files
SRCS=	$(CC_WITH_H)\
	PNDomain.cc

# Header files
HDRS=	$(CC_WITH_H:.cc=.h)

# Library objects
OBJS = $(SRCS:.cc=.o)

all:	makefile $(PTLIB)

# "make install" puts things in the official place
install:	makefile $(LIBDIR)/$(PTLIB)

EXTRA_SRCS=

include $(ROOT)/mk/common.mk
PNTarget.o: PNTarget.cc ../../../../src/kernel/Galaxy.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h PNTarget.h \
 ../../../../src/kernel/Target.h PNScheduler.h PNThread.h \
 ../../../../src/thread/posix/PosixThread.h \
 ../../../../src/kernel/PtThread.h \
 ../../../../src/thread/posix/PosixCondition.h \
 ../../../../src/kernel/PtCondition.h \
 ../../../../src/thread/posix/PosixMonitor.h \
 ../../../../src/kernel/PtGate.h ../../../../src/kernel/Scheduler.h \
 ../../../../src/kernel/SimControl.h
PNScheduler.o: PNScheduler.cc PNThread.h \
 ../../../../src/thread/posix/PosixThread.h \
 ../../../../src/kernel/PtThread.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/thread/posix/PosixCondition.h \
 ../../../../src/kernel/PtCondition.h \
 ../../../../src/thread/posix/PosixMonitor.h \
 ../../../../src/kernel/PtGate.h PNScheduler.h \
 ../../../../src/kernel/Scheduler.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/SimControl.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/isa.h ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/PortHole.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/Fraction.h ../../../../src/kernel/GalIter.h \
 ../../../../src/kernel/Galaxy.h PNGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h
PNStar.o: PNStar.cc PNStar.h \
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
 ../../../../src/kernel/DFPortHole.h PNPortHole.h
PNWormhole.o: PNWormhole.cc PNWormhole.h \
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
 ../../../../src/kernel/EventHorizon.h PNStar.h \
 ../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../src/kernel/DataFlowStar.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h PNPortHole.h PNScheduler.h \
 PNThread.h ../../../../src/thread/posix/PosixThread.h \
 ../../../../src/kernel/PtThread.h \
 ../../../../src/thread/posix/PosixCondition.h \
 ../../../../src/kernel/PtCondition.h \
 ../../../../src/thread/posix/PosixMonitor.h \
 ../../../../src/kernel/PtGate.h ../../../../src/kernel/Scheduler.h \
 ../../../../src/kernel/SimControl.h
PNEventHorizon.o: PNEventHorizon.cc PNEventHorizon.h \
 ../../../../src/kernel/EventHorizon.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h PNPortHole.h \
 ../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h PNScheduler.h PNThread.h \
 ../../../../src/thread/posix/PosixThread.h \
 ../../../../src/kernel/PtThread.h \
 ../../../../src/thread/posix/PosixCondition.h \
 ../../../../src/kernel/PtCondition.h \
 ../../../../src/thread/posix/PosixMonitor.h \
 ../../../../src/kernel/PtGate.h ../../../../src/kernel/Scheduler.h \
 ../../../../src/kernel/SimControl.h
PNPortHole.o: PNPortHole.cc PNPortHole.h \
 ../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h
PNGeodesic.o: PNGeodesic.cc PNThread.h \
 ../../../../src/thread/posix/PosixThread.h \
 ../../../../src/kernel/PtThread.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/thread/posix/PosixCondition.h \
 ../../../../src/kernel/PtCondition.h \
 ../../../../src/thread/posix/PosixMonitor.h \
 ../../../../src/kernel/PtGate.h PNGeodesic.h \
 ../../../../src/kernel/Geodesic.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/isa.h ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h
PNForkNode.o: PNForkNode.cc PNForkNode.h PNGeodesic.h \
 ../../../../src/kernel/Geodesic.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/PtGate.h ../../../../src/kernel/AutoFork.h
PNThread.o: PNThread.cc PNThread.h \
 ../../../../src/thread/posix/PosixThread.h \
 ../../../../src/kernel/PtThread.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/thread/posix/PosixCondition.h \
 ../../../../src/kernel/PtCondition.h \
 ../../../../src/thread/posix/PosixMonitor.h \
 ../../../../src/kernel/PtGate.h ../../../../src/kernel/DataFlowStar.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/isa.h ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/PortHole.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/Fraction.h ../../../../src/kernel/DFPortHole.h
PNDomain.o: PNDomain.cc ../../../../src/kernel/Domain.h \
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
 ../../../../src/kernel/KnownTarget.h PNTarget.h \
 ../../../../src/kernel/Target.h PNForkNode.h PNGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/PtGate.h ../../../../src/kernel/AutoFork.h \
 PNWormhole.h ../../../../src/kernel/Wormhole.h \
 ../../../../src/kernel/Universe.h ../../../../src/kernel/Galaxy.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/EventHorizon.h \
 PNStar.h ../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../src/kernel/DataFlowStar.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h PNPortHole.h PNEventHorizon.h
