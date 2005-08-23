# Makefile for MDSDF domain support
# Version identification:
# @(#)make.template	1.5 3/7/96
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
#
# Date of creation: 9/8/93
#
# If this file's name is "make.template" and there is no makefile in
# this directory, do
#	cp make.template makefile
#	make depend
# This will add the dependencies to the makefile.

ROOT=../../../..
# True source directory
VPATH = $(ROOT)/src/domains/mdsdf/kernel

# get configuration info
CONFIG=$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

#SDF kernel
SDFKERNDIR=$(ROOT)/src/domains/sdf/kernel
INCL = -I$(SDFKERNDIR) -I$(KERNDIR)

# Library name
PTLIB=libmdsdf.$(LIBSUFFIX)

# Library sources
SRCS= \
 MDSDFPortHole.cc MDSDFScheduler.cc MDSDFStar.cc MDSDFWormhole.cc \
 MDSDFDomain.cc MDSDFGeodesic.cc MDSDFTarget.cc MDSDFForkNode.cc

# Header files
HDRS= MDSDFPortHole.h MDSDFScheduler.h MDSDFStar.h MDSDFWormhole.h \
 MDSDFGeodesic.h MDSDFTarget.h MDSDFForkNode.h

# Library objects
OBJS= $(SRCS:.cc=.o)

all:	makefile $(PTLIB)

# "make install" puts things in the official place
install:	makefile $(LIBDIR)/$(PTLIB)

# include common definitions, rules
include $(ROOT)/mk/common.mk

# Don't add anything after the next line; makedepend will zap it.
# DO NOT DELETE THIS LINE -- make depend depends on it.

MDSDFPortHole.o: MDSDFPortHole.cc MDSDFPortHole.h \
 ../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/kernel/MatrixParticle.h \
 ../../../../src/kernel/SubMatrix.h ../../../../src/kernel/Matrix.h \
 ../../../../src/kernel/Message.h \
 ../../../../src/kernel/ComplexArrayState.h \
 ../../../../src/kernel/State.h ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/FixArrayState.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/kernel/FloatArrayState.h \
 ../../../../src/kernel/IntArrayState.h MDSDFGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/Plasma.h ../../../../src/kernel/PtGate.h \
 MDSDFStar.h ../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/Fraction.h
MDSDFScheduler.o: MDSDFScheduler.cc MDSDFScheduler.h \
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
 MDSDFPortHole.h ../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../src/kernel/MatrixParticle.h \
 ../../../../src/kernel/SubMatrix.h ../../../../src/kernel/Matrix.h \
 ../../../../src/kernel/Message.h \
 ../../../../src/kernel/ComplexArrayState.h \
 ../../../../src/kernel/FixArrayState.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/kernel/FloatArrayState.h \
 ../../../../src/kernel/IntArrayState.h MDSDFGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h MDSDFStar.h \
 ../../../../src/domains/sdf/kernel/SDFStar.h
MDSDFStar.o: MDSDFStar.cc MDSDFStar.h \
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
 ../../../../src/kernel/DFPortHole.h MDSDFPortHole.h \
 ../../../../src/kernel/MatrixParticle.h \
 ../../../../src/kernel/SubMatrix.h ../../../../src/kernel/Matrix.h \
 ../../../../src/kernel/Message.h \
 ../../../../src/kernel/ComplexArrayState.h \
 ../../../../src/kernel/FixArrayState.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/kernel/FloatArrayState.h \
 ../../../../src/kernel/IntArrayState.h MDSDFGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/SimControl.h
MDSDFWormhole.o: MDSDFWormhole.cc MDSDFWormhole.h \
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
 ../../../../src/kernel/EventHorizon.h MDSDFStar.h \
 ../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../src/kernel/DataFlowStar.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h MDSDFPortHole.h \
 ../../../../src/kernel/MatrixParticle.h \
 ../../../../src/kernel/SubMatrix.h ../../../../src/kernel/Matrix.h \
 ../../../../src/kernel/Message.h \
 ../../../../src/kernel/ComplexArrayState.h \
 ../../../../src/kernel/FixArrayState.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/kernel/FloatArrayState.h \
 ../../../../src/kernel/IntArrayState.h MDSDFGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h
MDSDFDomain.o: MDSDFDomain.cc ../../../../src/kernel/Domain.h \
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
 ../../../../src/kernel/KnownTarget.h MDSDFScheduler.h \
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/Scheduler.h \
 ../../../../src/kernel/SimControl.h ../../../../src/kernel/Galaxy.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/GalIter.h \
 ../../../../src/kernel/DataFlowStar.h \
 ../../../../src/kernel/Fraction.h ../../../../src/kernel/DFPortHole.h \
 MDSDFPortHole.h ../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../src/kernel/MatrixParticle.h \
 ../../../../src/kernel/SubMatrix.h ../../../../src/kernel/Matrix.h \
 ../../../../src/kernel/Message.h \
 ../../../../src/kernel/ComplexArrayState.h \
 ../../../../src/kernel/FixArrayState.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/kernel/FloatArrayState.h \
 ../../../../src/kernel/IntArrayState.h MDSDFGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h MDSDFStar.h \
 ../../../../src/domains/sdf/kernel/SDFStar.h MDSDFWormhole.h \
 ../../../../src/kernel/Wormhole.h ../../../../src/kernel/Universe.h \
 ../../../../src/kernel/EventHorizon.h MDSDFForkNode.h \
 ../../../../src/kernel/AutoFork.h
MDSDFGeodesic.o: MDSDFGeodesic.cc MDSDFGeodesic.h \
 ../../../../src/kernel/Geodesic.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/SubMatrix.h ../../../../src/kernel/Matrix.h \
 ../../../../src/kernel/Message.h \
 ../../../../src/kernel/ComplexArrayState.h \
 ../../../../src/kernel/State.h ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/kernel/FixArrayState.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/kernel/FloatArrayState.h \
 ../../../../src/kernel/IntArrayState.h \
 ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/MatrixParticle.h MDSDFPortHole.h \
 ../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h ../../../../src/kernel/Plasma.h \
 ../../../../src/kernel/PtGate.h ../../../../src/kernel/Fraction.h
MDSDFTarget.o: MDSDFTarget.cc ../../../../src/kernel/Galaxy.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h MDSDFTarget.h \
 ../../../../src/kernel/Target.h ../../../../src/kernel/StringState.h \
 ../../../../src/kernel/KnownState.h ../../../../src/kernel/IntState.h \
 ../../../../src/kernel/FloatState.h MDSDFScheduler.h \
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/Scheduler.h \
 ../../../../src/kernel/SimControl.h ../../../../src/kernel/GalIter.h \
 ../../../../src/kernel/DataFlowStar.h \
 ../../../../src/kernel/Fraction.h ../../../../src/kernel/DFPortHole.h \
 MDSDFPortHole.h ../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../src/kernel/MatrixParticle.h \
 ../../../../src/kernel/SubMatrix.h ../../../../src/kernel/Matrix.h \
 ../../../../src/kernel/Message.h \
 ../../../../src/kernel/ComplexArrayState.h \
 ../../../../src/kernel/FixArrayState.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/kernel/FloatArrayState.h \
 ../../../../src/kernel/IntArrayState.h MDSDFGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h MDSDFStar.h \
 ../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../src/kernel/pt_fstream.h
MDSDFForkNode.o: MDSDFForkNode.cc MDSDFForkNode.h MDSDFGeodesic.h \
 ../../../../src/kernel/Geodesic.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/SubMatrix.h ../../../../src/kernel/Matrix.h \
 ../../../../src/kernel/Message.h \
 ../../../../src/kernel/ComplexArrayState.h \
 ../../../../src/kernel/State.h ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/kernel/FixArrayState.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/kernel/FloatArrayState.h \
 ../../../../src/kernel/IntArrayState.h \
 ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/MatrixParticle.h \
 ../../../../src/kernel/AutoFork.h
