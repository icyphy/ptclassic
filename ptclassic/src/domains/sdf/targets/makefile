# @(#)make.template	1.12	8/4/95
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
# Special SDF targets.

LIBSRCS =	CompiledUniverse.cc CompiledError.cc
SRCS =	$(LIBSRCS) CompileTarget.cc
HDRS =	CompileTarget.h
 
# Root of Ptolemy directory
ROOT=../../../..
# True source directory
VPATH = $(ROOT)/src/domains/sdf/targets

# get configuration info
CONFIG=$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

SDFKERNEL = $(ROOT)/src/domains/sdf/kernel
LOOPKERNEL = $(ROOT)/src/domains/sdf/loopScheduler
CGKERNEL = $(ROOT)/src/domains/cg/kernel

INCL =-I$(SDFKERNEL) -I$(LOOPKERNEL) -I$(KERNDIR) -I$(CGKERNEL)

OBJS = $(LIBSRCS:.cc=.o)
PTLIB = libcompileSDF.$(LIBSUFFIX)
EXTRA_DESTS = CompileTarget.o

all:	makefile $(PTLIB) $(EXTRA_DESTS)

# "make install" does nothing extraordinary
install:	all $(LIBDIR)/$(PTLIB)
		@echo Installation complete.


# include common definitions, rules
include $(ROOT)/mk/common.mk


# Don't add anything after the next line; makedepend will zap it.
# DO NOT DELETE THIS LINE -- make depend depends on it.

CompiledUniverse.o: CompiledUniverse.cc \
 ../../../../src/kernel/CompiledUniverse.h \
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
 ../../../../src/kernel/InvokeInterp.h
CompiledError.o: CompiledError.cc ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/DataStruct.h ../../../../src/kernel/type.h \
 ../../../../src/kernel/isa.h ../../../../src/kernel/FlagArray.h
CompileTarget.o: CompileTarget.cc CompileTarget.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/IntState.h \
 ../../../../src/kernel/State.h ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/Block.h \
 ../../../../src/kernel/PortHole.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h \
 ../../../../src/domains/cg/kernel/HLLTarget.h \
 ../../../../src/domains/cg/kernel/CGTarget.h \
 ../../../../src/kernel/Target.h ../../../../src/kernel/StringState.h \
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
 ../../../../src/kernel/Scheduler.h ../../../../src/kernel/Galaxy.h \
 ../../../../src/kernel/GalIter.h ../../../../src/kernel/DFPortHole.h \
 ../../../../src/kernel/Scope.h ../../../../src/kernel/DynamicGalaxy.h \
 ../../../../src/kernel/KnownTarget.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/ConstIters.h \
 ../../../../src/domains/sdf/kernel/AcyLoopScheduler.h \
 ../../../../src/domains/sdf/kernel/SDFCluster.h \
 ../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../src/domains/sdf/kernel/AcyCluster.h \
 ../../../../src/domains/sdf/kernel/SynDFCluster.h \
 ../../../../src/kernel/Cluster.h ../../../../src/kernel/GraphUtils.h \
 ../../../../src/domains/sdf/loopScheduler/LoopScheduler.h \
 ../../../../src/kernel/pt_fstream.h
