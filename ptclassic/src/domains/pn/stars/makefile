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
# Makefile for Ptolemy PN stars
# @(#)make.template	1.8 2/9/96

ROOT= ../../../..
VPATH= $(ROOT)/src/domains/pn/stars

# get configuration info
CONFIG= $(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

DOMAIN= PN
STAR_MK= pnstars

INCL_THREADS=-I$(ROOT)/thread/include.$(PTARCH) -I$(ROOT)/src/thread/posix

SDFDIR=	$(ROOT)/src/domains/sdf/kernel
PNDIR=	$(ROOT)/src/domains/pn/kernel
COMPAT=	$(ROOT)/src/compat/ptolemy

INCL=	-I$(COMPAT) -I$(PNDIR) -I$(SDFDIR) -I$(KERNDIR) \
	-I$(VPATH) $(INCL_THREADS)

# Library name
PTLIB= lib$(STAR_MK).$(LIBSUFFIX)

# source files for all stars, using preprocessor
# base classes must be listed before derived classes
PL_SRCS= \
 PNSplice.pl \
 PNCommutator.pl \
 PNDelayInt.pl \
 PNDistributor.pl \
 PNEndCase.pl \
 PNFilter.pl \
 PNIncrementInt.pl \
 PNLastOfN.pl \
 PNMerge.pl \
 PNModuloInt.pl \
 PNSift.pl

# The .cc files
SRCS= $(PL_SRCS:.pl=.cc)

# object files for all stars
OBJS= $(SRCS:.cc=.o)

# header files for all stars
HDRS= $(SRCS:.cc=.h)

EXTRA_SRCS= $(STAR_MK).mk $(STAR_MK).c $(PL_SRCS)

all:		makefile stars_all

install:	stars_install

# include commone definitions, rules
include $(ROOT)/mk/common.mk
PNSplice.o: PNSplice.cc PNSplice.h \
 ../../../../src/domains/pn/kernel/PNThread.h \
 ../../../../src/thread/posix/PosixThread.h \
 ../../../../src/kernel/PtThread.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/thread/posix/PosixCondition.h \
 ../../../../src/kernel/PtCondition.h \
 ../../../../src/thread/posix/PosixMonitor.h \
 ../../../../src/kernel/PtGate.h ../../../../src/kernel/InterpGalaxy.h \
 ../../../../src/kernel/DynamicGalaxy.h \
 ../../../../src/kernel/Galaxy.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/isa.h ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/PortHole.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/domains/pn/kernel/PNStar.h \
 ../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../src/kernel/DataFlowStar.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h \
 ../../../../src/domains/pn/kernel/PNPortHole.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h
PNCommutator.o: PNCommutator.cc PNCommutator.h \
 ../../../../src/domains/pn/kernel/PNStar.h \
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
 ../../../../src/domains/pn/kernel/PNPortHole.h \
 ../../../../src/kernel/IntState.h
PNDelayInt.o: PNDelayInt.cc PNDelayInt.h \
 ../../../../src/domains/pn/kernel/PNStar.h \
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
 ../../../../src/domains/pn/kernel/PNPortHole.h \
 ../../../../src/kernel/IntArrayState.h
PNDistributor.o: PNDistributor.cc PNDistributor.h \
 ../../../../src/domains/pn/kernel/PNStar.h \
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
 ../../../../src/domains/pn/kernel/PNPortHole.h \
 ../../../../src/kernel/IntState.h
PNEndCase.o: PNEndCase.cc PNEndCase.h \
 ../../../../src/domains/pn/kernel/PNStar.h \
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
 ../../../../src/domains/pn/kernel/PNPortHole.h
PNFilter.o: PNFilter.cc PNFilter.h \
 ../../../../src/domains/pn/kernel/PNStar.h \
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
 ../../../../src/domains/pn/kernel/PNPortHole.h \
 ../../../../src/kernel/IntState.h
PNIncrementInt.o: PNIncrementInt.cc PNIncrementInt.h \
 ../../../../src/domains/pn/kernel/PNStar.h \
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
 ../../../../src/domains/pn/kernel/PNPortHole.h \
 ../../../../src/kernel/IntState.h
PNLastOfN.o: PNLastOfN.cc PNLastOfN.h \
 ../../../../src/domains/pn/kernel/PNStar.h \
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
 ../../../../src/domains/pn/kernel/PNPortHole.h
PNMerge.o: PNMerge.cc PNMerge.h \
 ../../../../src/domains/pn/kernel/PNStar.h \
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
 ../../../../src/domains/pn/kernel/PNPortHole.h
PNModuloInt.o: PNModuloInt.cc PNModuloInt.h \
 ../../../../src/domains/pn/kernel/PNStar.h \
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
 ../../../../src/domains/pn/kernel/PNPortHole.h \
 ../../../../src/kernel/IntState.h
PNSift.o: PNSift.cc PNSift.h PNSplice.h \
 ../../../../src/domains/pn/kernel/PNThread.h \
 ../../../../src/thread/posix/PosixThread.h \
 ../../../../src/kernel/PtThread.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/thread/posix/PosixCondition.h \
 ../../../../src/kernel/PtCondition.h \
 ../../../../src/thread/posix/PosixMonitor.h \
 ../../../../src/kernel/PtGate.h ../../../../src/kernel/InterpGalaxy.h \
 ../../../../src/kernel/DynamicGalaxy.h \
 ../../../../src/kernel/Galaxy.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/isa.h ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/PortHole.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/domains/pn/kernel/PNStar.h \
 ../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../src/kernel/DataFlowStar.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h \
 ../../../../src/domains/pn/kernel/PNPortHole.h
