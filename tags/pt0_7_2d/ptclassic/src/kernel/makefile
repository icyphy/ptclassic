# Makefile for Ptolemy library and main module for interpreter
# Version identification:
# @(#)make.template	2.84	08/25/97
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
# Date of creation: 3/19/90
#
# If this file's name is "make.template" and there is no makefile in
# this directory, do
#	cp make.template makefile
#	make depend
# This will add the dependencies to the makefile.

# Root of Ptolemy directory
ROOT=../..
# Where we are
VPATH = ../../src/kernel
# get configuration info
CONFIG=$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

# Library name
# Build a non-shared libptolemy.a for faster startup time
# It would be nice if we built both
#LIB=libptolemy.$(LIBSUFFIX)
LIBNONSHARED=	libptolemy.$(LIBNONSHAREDSUFFIX)

# No extra directories to include
INCL = $(PTCOMPAT_INCSPEC)

# Library sources.  CCS_WITH_H_FILES includes all .cc files with a
# corresponding .h file with the same root.

CCS_WITH_H_FILES=\
 AutoFork.cc \
 AutoForkNode.cc \
 Block.cc \
 Clock.cc \
 Cluster.cc \
 ComplexArrayState.cc \
 ComplexState.cc \
 ComplexSubset.cc \
 PortHole.cc \
 DFPortHole.cc \
 DataFlowStar.cc \
 DataStruct.cc \
 Display.cc \
 DoubleLink.cc \
 Domain.cc \
 DynDFPortHole.cc \
 DynDFStar.cc \
 DynamicGalaxy.cc \
 Error.cc \
 EventHorizon.cc \
 FileMessage.cc \
 FileParticle.cc \
 Fix.cc \
 FixArrayState.cc \
 FixState.cc \
 FlagArray.cc \
 FloatArrayState.cc \
 FloatState.cc \
 FloatVecData.cc \
 Fraction.cc \
 GalIter.cc \
 Galaxy.cc \
 Geodesic.cc \
 GraphUtils.cc \
 HashTable.cc \
 Histogram.cc \
 IntArrayState.cc \
 IntState.cc \
 IntVecData.cc \
 InterpGalaxy.cc \
 InterpUniverse.cc \
 IntervalList.cc \
 InvokeInterp.cc \
 KnownBlock.cc \
 KnownState.cc \
 KnownTarget.cc \
 Linker.cc \
 Matrix.cc \
 MatrixParticle.cc \
 Message.cc \
 NamedObj.cc \
 Particle.cc \
 ParticleQueue.cc \
 ParticleVector.cc \
 Plasma.cc \
 PrecisionState.cc \
 PriorityQueue.cc \
 ProfileTimer.cc \
 PtGate.cc \
 PtThread.cc \
 Scheduler.cc \
 Scope.cc \
 SimControl.cc \
 Star.cc \
 State.cc \
 StringArrayState.cc \
 StringList.cc \
 StringMessage.cc \
 StringParticle.cc \
 StringState.cc \
 SubMatrix.cc \
 Target.cc \
 TimeVal.cc \
 Tokenizer.cc \
 Universe.cc \
 Wormhole.cc \
 checkConnect.cc \
 logNew.cc \
 miscFuncs.cc \
 paths.cc \
 ptsignals.cc \
 pt_fstream.cc \
 textAnimate.cc

# The complete list of .cc files used in the library
SRCS=$(CCS_WITH_H_FILES) \
 expandPath.cc \
 subCharByString.cc \
 RanGen.cc \
 hashstring.cc

# Header files.  When adding a new header, add it here only if there is
# no corresponding .cc file; otherwise add the .cc file to CCS_WITH_H_FILES.

HDRS= $(CCS_WITH_H_FILES:.cc=.h) \
 Attribute.h \
 CircularBuffer.h \
 CompiledUniverse.h \
 ConstIters.h \
 InfString.h \
 Linker.sysdep.h \
 ParticleStack.h \
 PtCondition.h \
 SimAction.h \
 dataType.h \
 ieee.h \
 isa.h \
 type.h

# Library objects
OBJS=$(SRCS:.cc=.o)


all:	makefile $(LIBNONSHARED)

# "make install" puts things in the official place
install:	makefile $(LIBDIR)/$(LIBNONSHARED) $(FLUSH_CACHE)
ifdef FLUSH_CACHE
	# FLUSH_CACHE is defined for hppa
	rm -f $(LIBDIR)/$(FLUSH_CACHE)
	ln $(FLUSH_CACHE) $(LIBDIR)	
endif

EXTRA_SRCS=XDomain.ccP ArrayState.ccP ArrayState.hP ScalarState.ccP ScalarState.hP flush_cache.s

# Commands to make some sources from templates

FloatState.h:	ScalarState.hP
	cd $(VPATH); genclass -2 Float val double val ScalarState;\
	sed 's/"Float"/"FLOAT"/' Float.double.ScalarState.h > FloatState.h
	rm Float.double.ScalarState.*

FloatState.cc:	ScalarState.ccP
	cd $(VPATH); genclass -2 Float val double val ScalarState;\
	sed 's/"Float"/"FLOAT"/' Float.double.ScalarState.cc > FloatState.cc
	rm Float.double.ScalarState.*

IntState.h:	ScalarState.hP
	cd $(VPATH); genclass -2 Int val int val ScalarState;\
	sed 's/"Int"/"INT"/' Int.int.ScalarState.h > IntState.h
	rm Int.int.ScalarState.*

IntState.cc:	ScalarState.ccP
	cd $(VPATH); genclass -2 Int val int val ScalarState;\
	sed 's/"Int"/"INT"/' Int.int.ScalarState.cc > IntState.cc;\
	rm Int.int.ScalarState.*


FloatArrayState.h FloatArrayState.cc : ArrayState.hP ArrayState.ccP
	cd $(VPATH); genclass -2 Float val double val ArrayState; \
	mv Float.double.ArrayState.h FloatArrayState.h; \
	sed 's/"FloatARRAY/"FLOATARRAY/' \
		Float.double.ArrayState.cc > FloatArrayState.cc; \
	rm Float.double.ArrayState.cc

IntArrayState.h IntArrayState.cc : ArrayState.hP ArrayState.ccP
	cd $(VPATH); genclass -2 Int val int val ArrayState; \
	mv Int.int.ArrayState.h IntArrayState.h; \
	sed 's/"IntARRAY/"INTARRAY/' Int.int.ArrayState.cc > IntArrayState.cc; \
	rm Int.int.ArrayState.cc

# For dynamic linking on the hppa
ifdef FLUSH_CACHE
$(FLUSH_CACHE):
	$(HP_AS) $(VPATH)/`basename $(FLUSH_CACHE) .o`.s
endif
include $(ROOT)/mk/common.mk

# Don't add anything after the next line; makedepend will zap it.
# DO NOT DELETE THIS LINE -- make depend depends on it.
AutoFork.o: AutoFork.cc AutoFork.h Geodesic.h NamedObj.h StringList.h \
 miscFuncs.h logNew.h Error.h DataStruct.h type.h isa.h FlagArray.h \
 ParticleStack.h Particle.h dataType.h Fix.h ComplexSubset.h \
 PortHole.h Attribute.h KnownBlock.h Block.h State.h InvokeInterp.h \
 Linker.h Galaxy.h Star.h
AutoForkNode.o: AutoForkNode.cc AutoForkNode.h Geodesic.h NamedObj.h \
 StringList.h miscFuncs.h logNew.h Error.h DataStruct.h type.h isa.h \
 FlagArray.h ParticleStack.h Particle.h dataType.h Fix.h \
 ComplexSubset.h AutoFork.h
Block.o: Block.cc Block.h NamedObj.h StringList.h miscFuncs.h logNew.h \
 Error.h DataStruct.h type.h isa.h FlagArray.h PortHole.h dataType.h \
 Particle.h Fix.h ComplexSubset.h Attribute.h State.h InvokeInterp.h \
 ConstIters.h Galaxy.h Star.h SimControl.h Scope.h DynamicGalaxy.h \
 KnownBlock.h Linker.h
Clock.o: Clock.cc Clock.h TimeVal.h Error.h type.h logNew.h \
 ../../src/compat/ptolemy/compat.h
Cluster.o: Cluster.cc Cluster.h DynamicGalaxy.h Galaxy.h Star.h \
 Block.h NamedObj.h StringList.h miscFuncs.h logNew.h Error.h \
 DataStruct.h type.h isa.h FlagArray.h PortHole.h dataType.h \
 Particle.h Fix.h ComplexSubset.h Attribute.h State.h InvokeInterp.h \
 GraphUtils.h GalIter.h Scheduler.h SimControl.h Scope.h InfString.h
ComplexArrayState.o: ComplexArrayState.cc ComplexArrayState.h State.h \
 InvokeInterp.h DataStruct.h type.h logNew.h StringList.h miscFuncs.h \
 Error.h NamedObj.h isa.h FlagArray.h ComplexSubset.h Attribute.h \
 Tokenizer.h KnownState.h Block.h PortHole.h dataType.h Particle.h \
 Fix.h
ComplexState.o: ComplexState.cc ComplexState.h State.h InvokeInterp.h \
 DataStruct.h type.h logNew.h StringList.h miscFuncs.h Error.h \
 NamedObj.h isa.h FlagArray.h ComplexSubset.h Attribute.h KnownState.h \
 Block.h PortHole.h dataType.h Particle.h Fix.h Tokenizer.h
ComplexSubset.o: ComplexSubset.cc ComplexSubset.h
PortHole.o: PortHole.cc PortHole.h NamedObj.h StringList.h miscFuncs.h \
 logNew.h Error.h DataStruct.h type.h isa.h FlagArray.h dataType.h \
 Particle.h Fix.h ComplexSubset.h Attribute.h CircularBuffer.h Block.h \
 State.h InvokeInterp.h Geodesic.h ParticleStack.h Plasma.h PtGate.h \
 GalIter.h Galaxy.h Star.h EventHorizon.h Domain.h Wormhole.h \
 Universe.h Target.h
DFPortHole.o: DFPortHole.cc DFPortHole.h PortHole.h NamedObj.h \
 StringList.h miscFuncs.h logNew.h Error.h DataStruct.h type.h isa.h \
 FlagArray.h dataType.h Particle.h Fix.h ComplexSubset.h Attribute.h \
 DataFlowStar.h Star.h Block.h State.h InvokeInterp.h Fraction.h \
 CircularBuffer.h Geodesic.h ParticleStack.h
DataFlowStar.o: DataFlowStar.cc DataFlowStar.h Star.h Block.h \
 NamedObj.h StringList.h miscFuncs.h logNew.h Error.h DataStruct.h \
 type.h isa.h FlagArray.h PortHole.h dataType.h Particle.h Fix.h \
 ComplexSubset.h Attribute.h State.h InvokeInterp.h Fraction.h \
 DFPortHole.h
DataStruct.o: DataStruct.cc DataStruct.h type.h logNew.h
Display.o: Display.cc Display.h type.h logNew.h miscFuncs.h Error.h \
 StringList.h DataStruct.h Block.h NamedObj.h isa.h FlagArray.h \
 PortHole.h dataType.h Particle.h Fix.h ComplexSubset.h Attribute.h \
 State.h InvokeInterp.h paths.h SimControl.h ieee.h \
 ../../src/compat/ptolemy/compat.h
DoubleLink.o: DoubleLink.cc DoubleLink.h type.h logNew.h
Domain.o: Domain.cc Domain.h StringList.h miscFuncs.h logNew.h Error.h \
 DataStruct.h type.h Block.h NamedObj.h isa.h FlagArray.h PortHole.h \
 dataType.h Particle.h Fix.h ComplexSubset.h Attribute.h State.h \
 InvokeInterp.h Star.h Galaxy.h Scheduler.h SimControl.h \
 EventHorizon.h AutoForkNode.h Geodesic.h ParticleStack.h AutoFork.h
DynDFPortHole.o: DynDFPortHole.cc DynDFPortHole.h DFPortHole.h \
 PortHole.h NamedObj.h StringList.h miscFuncs.h logNew.h Error.h \
 DataStruct.h type.h isa.h FlagArray.h dataType.h Particle.h Fix.h \
 ComplexSubset.h Attribute.h
DynDFStar.o: DynDFStar.cc DynDFStar.h DataFlowStar.h Star.h Block.h \
 NamedObj.h StringList.h miscFuncs.h logNew.h Error.h DataStruct.h \
 type.h isa.h FlagArray.h PortHole.h dataType.h Particle.h Fix.h \
 ComplexSubset.h Attribute.h State.h InvokeInterp.h Fraction.h \
 DFPortHole.h
DynamicGalaxy.o: DynamicGalaxy.cc DynamicGalaxy.h Galaxy.h Star.h \
 Block.h NamedObj.h StringList.h miscFuncs.h logNew.h Error.h \
 DataStruct.h type.h isa.h FlagArray.h PortHole.h dataType.h \
 Particle.h Fix.h ComplexSubset.h Attribute.h State.h InvokeInterp.h
Error.o: Error.cc Error.h Scheduler.h StringList.h miscFuncs.h \
 logNew.h DataStruct.h type.h SimControl.h NamedObj.h isa.h \
 FlagArray.h PtGate.h
EventHorizon.o: EventHorizon.cc EventHorizon.h NamedObj.h StringList.h \
 miscFuncs.h logNew.h Error.h DataStruct.h type.h isa.h FlagArray.h \
 PortHole.h dataType.h Particle.h Fix.h ComplexSubset.h Attribute.h \
 Star.h Block.h State.h InvokeInterp.h Wormhole.h Universe.h Galaxy.h \
 Target.h Plasma.h ParticleStack.h PtGate.h CircularBuffer.h Domain.h \
 Scheduler.h SimControl.h Geodesic.h
FileMessage.o: FileMessage.cc FileMessage.h Message.h Particle.h \
 StringList.h miscFuncs.h logNew.h Error.h DataStruct.h type.h \
 dataType.h Fix.h ComplexSubset.h isa.h InfString.h
FileParticle.o: FileParticle.cc FileParticle.h Message.h Particle.h \
 StringList.h miscFuncs.h logNew.h Error.h DataStruct.h type.h \
 dataType.h Fix.h ComplexSubset.h isa.h Plasma.h ParticleStack.h \
 PtGate.h
Fix.o: Fix.cc Fix.h Error.h type.h logNew.h PrecisionState.h State.h \
 InvokeInterp.h DataStruct.h StringList.h miscFuncs.h NamedObj.h isa.h \
 FlagArray.h ComplexSubset.h Attribute.h \
 ../../src/compat/ptolemy/compat.h
FixArrayState.o: FixArrayState.cc FixArrayState.h State.h \
 InvokeInterp.h DataStruct.h type.h logNew.h StringList.h miscFuncs.h \
 Error.h NamedObj.h isa.h FlagArray.h ComplexSubset.h Attribute.h \
 Fix.h PrecisionState.h Tokenizer.h KnownState.h Block.h PortHole.h \
 dataType.h Particle.h
FixState.o: FixState.cc FixState.h State.h InvokeInterp.h DataStruct.h \
 type.h logNew.h StringList.h miscFuncs.h Error.h NamedObj.h isa.h \
 FlagArray.h ComplexSubset.h Attribute.h Fix.h PrecisionState.h \
 KnownState.h Block.h PortHole.h dataType.h Particle.h Tokenizer.h
FlagArray.o: FlagArray.cc FlagArray.h logNew.h
FloatArrayState.o: FloatArrayState.cc FloatArrayState.h State.h \
 InvokeInterp.h DataStruct.h type.h logNew.h StringList.h miscFuncs.h \
 Error.h NamedObj.h isa.h FlagArray.h ComplexSubset.h Attribute.h \
 Tokenizer.h KnownState.h Block.h PortHole.h dataType.h Particle.h \
 Fix.h
FloatState.o: FloatState.cc FloatState.h State.h InvokeInterp.h \
 DataStruct.h type.h logNew.h StringList.h miscFuncs.h Error.h \
 NamedObj.h isa.h FlagArray.h ComplexSubset.h Attribute.h Tokenizer.h \
 KnownState.h Block.h PortHole.h dataType.h Particle.h Fix.h
FloatVecData.o: FloatVecData.cc FloatVecData.h Message.h Particle.h \
 StringList.h miscFuncs.h logNew.h Error.h DataStruct.h type.h \
 dataType.h Fix.h ComplexSubset.h isa.h
Fraction.o: Fraction.cc Fraction.h
GalIter.o: GalIter.cc GalIter.h Galaxy.h Star.h Block.h NamedObj.h \
 StringList.h miscFuncs.h logNew.h Error.h DataStruct.h type.h isa.h \
 FlagArray.h PortHole.h dataType.h Particle.h Fix.h ComplexSubset.h \
 Attribute.h State.h InvokeInterp.h ConstIters.h
Galaxy.o: Galaxy.cc Star.h Block.h NamedObj.h StringList.h miscFuncs.h \
 logNew.h Error.h DataStruct.h type.h isa.h FlagArray.h PortHole.h \
 dataType.h Particle.h Fix.h ComplexSubset.h Attribute.h State.h \
 InvokeInterp.h Galaxy.h KnownBlock.h Linker.h Scheduler.h \
 SimControl.h ConstIters.h GraphUtils.h GalIter.h
Geodesic.o: Geodesic.cc Geodesic.h NamedObj.h StringList.h miscFuncs.h \
 logNew.h Error.h DataStruct.h type.h isa.h FlagArray.h \
 ParticleStack.h Particle.h dataType.h Fix.h ComplexSubset.h Block.h \
 PortHole.h Attribute.h State.h InvokeInterp.h Plasma.h PtGate.h \
 IntState.h
GraphUtils.o: GraphUtils.cc GraphUtils.h StringList.h miscFuncs.h \
 logNew.h Error.h DataStruct.h type.h Galaxy.h Star.h Block.h \
 NamedObj.h isa.h FlagArray.h PortHole.h dataType.h Particle.h Fix.h \
 ComplexSubset.h Attribute.h State.h InvokeInterp.h GalIter.h \
 InfString.h
HashTable.o: HashTable.cc Error.h miscFuncs.h logNew.h HashTable.h \
 type.h
Histogram.o: Histogram.cc Histogram.h logNew.h Display.h type.h \
 miscFuncs.h Error.h StringList.h DataStruct.h
IntArrayState.o: IntArrayState.cc IntArrayState.h State.h \
 InvokeInterp.h DataStruct.h type.h logNew.h StringList.h miscFuncs.h \
 Error.h NamedObj.h isa.h FlagArray.h ComplexSubset.h Attribute.h \
 Tokenizer.h KnownState.h Block.h PortHole.h dataType.h Particle.h \
 Fix.h
IntState.o: IntState.cc IntState.h State.h InvokeInterp.h DataStruct.h \
 type.h logNew.h StringList.h miscFuncs.h Error.h NamedObj.h isa.h \
 FlagArray.h ComplexSubset.h Attribute.h Tokenizer.h KnownState.h \
 Block.h PortHole.h dataType.h Particle.h Fix.h
IntVecData.o: IntVecData.cc IntVecData.h Message.h Particle.h \
 StringList.h miscFuncs.h logNew.h Error.h DataStruct.h type.h \
 dataType.h Fix.h ComplexSubset.h isa.h
InterpGalaxy.o: InterpGalaxy.cc InterpGalaxy.h DynamicGalaxy.h \
 Galaxy.h Star.h Block.h NamedObj.h StringList.h miscFuncs.h logNew.h \
 Error.h DataStruct.h type.h isa.h FlagArray.h PortHole.h dataType.h \
 Particle.h Fix.h ComplexSubset.h Attribute.h State.h InvokeInterp.h \
 Geodesic.h ParticleStack.h Domain.h Plasma.h PtGate.h KnownBlock.h \
 Linker.h KnownState.h IntState.h
InterpUniverse.o: InterpUniverse.cc InterpUniverse.h Universe.h \
 Galaxy.h Star.h Block.h NamedObj.h StringList.h miscFuncs.h logNew.h \
 Error.h DataStruct.h type.h isa.h FlagArray.h PortHole.h dataType.h \
 Particle.h Fix.h ComplexSubset.h Attribute.h State.h InvokeInterp.h \
 Target.h InterpGalaxy.h DynamicGalaxy.h KnownBlock.h Linker.h \
 KnownTarget.h SimControl.h
IntervalList.o: IntervalList.cc IntervalList.h type.h logNew.h \
 miscFuncs.h Error.h
InvokeInterp.o: InvokeInterp.cc InvokeInterp.h Error.h
KnownBlock.o: KnownBlock.cc KnownBlock.h Block.h NamedObj.h \
 StringList.h miscFuncs.h logNew.h Error.h DataStruct.h type.h isa.h \
 FlagArray.h PortHole.h dataType.h Particle.h Fix.h ComplexSubset.h \
 Attribute.h State.h InvokeInterp.h Linker.h Domain.h Star.h \
 Wormhole.h Universe.h Galaxy.h Target.h EventHorizon.h
KnownState.o: KnownState.cc KnownState.h StringList.h miscFuncs.h \
 logNew.h Error.h DataStruct.h type.h State.h InvokeInterp.h \
 NamedObj.h isa.h FlagArray.h ComplexSubset.h Attribute.h Block.h \
 PortHole.h dataType.h Particle.h Fix.h IntState.h FloatState.h
KnownTarget.o: KnownTarget.cc Linker.h Target.h StringList.h \
 miscFuncs.h DataStruct.h type.h logNew.h Block.h NamedObj.h isa.h \
 FlagArray.h PortHole.h dataType.h Particle.h Fix.h ComplexSubset.h \
 Attribute.h State.h InvokeInterp.h Error.h KnownTarget.h KnownBlock.h \
 Domain.h
Linker.o: Linker.cc Linker.sysdep.h ../../src/compat/ptolemy/compat.h \
 Linker.h Error.h miscFuncs.h StringList.h DataStruct.h type.h \
 logNew.h pt_fstream.h paths.h
Matrix.o: Matrix.cc Matrix.h Message.h Particle.h StringList.h \
 miscFuncs.h logNew.h Error.h DataStruct.h type.h dataType.h Fix.h \
 ComplexSubset.h isa.h ComplexArrayState.h State.h InvokeInterp.h \
 NamedObj.h FlagArray.h Attribute.h FixArrayState.h PrecisionState.h \
 FloatArrayState.h IntArrayState.h PortHole.h Plasma.h ParticleStack.h \
 PtGate.h
MatrixParticle.o: MatrixParticle.cc MatrixParticle.h SubMatrix.h \
 Matrix.h Message.h Particle.h StringList.h miscFuncs.h logNew.h \
 Error.h DataStruct.h type.h dataType.h Fix.h ComplexSubset.h isa.h \
 ComplexArrayState.h State.h InvokeInterp.h NamedObj.h FlagArray.h \
 Attribute.h FixArrayState.h PrecisionState.h FloatArrayState.h \
 IntArrayState.h PortHole.h Plasma.h ParticleStack.h PtGate.h
Message.o: Message.cc Message.h Particle.h StringList.h miscFuncs.h \
 logNew.h Error.h DataStruct.h type.h dataType.h Fix.h ComplexSubset.h \
 isa.h Plasma.h ParticleStack.h PtGate.h
NamedObj.o: NamedObj.cc NamedObj.h StringList.h miscFuncs.h logNew.h \
 Error.h DataStruct.h type.h isa.h FlagArray.h Block.h PortHole.h \
 dataType.h Particle.h Fix.h ComplexSubset.h Attribute.h State.h \
 InvokeInterp.h SimControl.h
Particle.o: Particle.cc Particle.h StringList.h miscFuncs.h logNew.h \
 Error.h DataStruct.h type.h dataType.h Fix.h ComplexSubset.h Plasma.h \
 ParticleStack.h PtGate.h ComplexArrayState.h State.h InvokeInterp.h \
 NamedObj.h isa.h FlagArray.h Attribute.h FixArrayState.h \
 PrecisionState.h FloatArrayState.h IntArrayState.h SubMatrix.h \
 Matrix.h Message.h PortHole.h
ParticleQueue.o: ParticleQueue.cc ParticleQueue.h ParticleStack.h \
 Particle.h StringList.h miscFuncs.h logNew.h Error.h DataStruct.h \
 type.h dataType.h Fix.h ComplexSubset.h
ParticleVector.o: ParticleVector.cc ParticleVector.h Message.h \
 Particle.h StringList.h miscFuncs.h logNew.h Error.h DataStruct.h \
 type.h dataType.h Fix.h ComplexSubset.h isa.h
Plasma.o: Plasma.cc Plasma.h ParticleStack.h Particle.h StringList.h \
 miscFuncs.h logNew.h Error.h DataStruct.h type.h dataType.h Fix.h \
 ComplexSubset.h PtGate.h
PrecisionState.o: PrecisionState.cc PrecisionState.h State.h \
 InvokeInterp.h DataStruct.h type.h logNew.h StringList.h miscFuncs.h \
 Error.h NamedObj.h isa.h FlagArray.h ComplexSubset.h Attribute.h \
 Fix.h KnownState.h Block.h PortHole.h dataType.h Particle.h \
 Tokenizer.h
PriorityQueue.o: PriorityQueue.cc PriorityQueue.h type.h logNew.h \
 Error.h
ProfileTimer.o: ProfileTimer.cc ProfileTimer.h TimeVal.h \
 ../../src/compat/ptolemy/compat.h
PtGate.o: PtGate.cc PtCondition.h PtGate.h logNew.h Error.h
PtThread.o: PtThread.cc PtThread.h DataStruct.h type.h logNew.h
Scheduler.o: Scheduler.cc Target.h StringList.h miscFuncs.h logNew.h \
 Error.h DataStruct.h type.h Block.h NamedObj.h isa.h FlagArray.h \
 PortHole.h dataType.h Particle.h Fix.h ComplexSubset.h Attribute.h \
 State.h InvokeInterp.h Scheduler.h SimControl.h Galaxy.h Star.h
Scope.o: Scope.cc Scope.h Galaxy.h Star.h Block.h NamedObj.h \
 StringList.h miscFuncs.h logNew.h Error.h DataStruct.h type.h isa.h \
 FlagArray.h PortHole.h dataType.h Particle.h Fix.h ComplexSubset.h \
 Attribute.h State.h InvokeInterp.h DynamicGalaxy.h Wormhole.h \
 Universe.h Target.h EventHorizon.h
SimControl.o: SimControl.cc ../../src/compat/ptolemy/compat.h \
 SimAction.h DataStruct.h type.h logNew.h SimControl.h PtGate.h \
 ptsignals.h
Star.o: Star.cc Target.h StringList.h miscFuncs.h logNew.h Error.h \
 DataStruct.h type.h Block.h NamedObj.h isa.h FlagArray.h PortHole.h \
 dataType.h Particle.h Fix.h ComplexSubset.h Attribute.h State.h \
 InvokeInterp.h Star.h GalIter.h Galaxy.h SimControl.h
State.o: State.cc InvokeInterp.h Tokenizer.h DataStruct.h type.h \
 logNew.h State.h StringList.h miscFuncs.h Error.h NamedObj.h isa.h \
 FlagArray.h ComplexSubset.h Attribute.h Block.h PortHole.h dataType.h \
 Particle.h Fix.h KnownState.h PtGate.h Scope.h Galaxy.h Star.h \
 DynamicGalaxy.h
StringArrayState.o: StringArrayState.cc StringArrayState.h State.h \
 InvokeInterp.h DataStruct.h type.h logNew.h StringList.h miscFuncs.h \
 Error.h NamedObj.h isa.h FlagArray.h ComplexSubset.h Attribute.h \
 Tokenizer.h KnownState.h Block.h PortHole.h dataType.h Particle.h \
 Fix.h
StringList.o: StringList.cc StringList.h miscFuncs.h logNew.h Error.h \
 DataStruct.h type.h InfString.h
StringMessage.o: StringMessage.cc StringMessage.h Message.h Particle.h \
 StringList.h miscFuncs.h logNew.h Error.h DataStruct.h type.h \
 dataType.h Fix.h ComplexSubset.h isa.h InfString.h
StringParticle.o: StringParticle.cc StringParticle.h Message.h \
 Particle.h StringList.h miscFuncs.h logNew.h Error.h DataStruct.h \
 type.h dataType.h Fix.h ComplexSubset.h isa.h Plasma.h \
 ParticleStack.h PtGate.h
StringState.o: StringState.cc miscFuncs.h StringState.h State.h \
 InvokeInterp.h DataStruct.h type.h logNew.h StringList.h Error.h \
 NamedObj.h isa.h FlagArray.h ComplexSubset.h Attribute.h KnownState.h \
 Block.h PortHole.h dataType.h Particle.h Fix.h Tokenizer.h
SubMatrix.o: SubMatrix.cc SubMatrix.h Matrix.h Message.h Particle.h \
 StringList.h miscFuncs.h logNew.h Error.h DataStruct.h type.h \
 dataType.h Fix.h ComplexSubset.h isa.h ComplexArrayState.h State.h \
 InvokeInterp.h NamedObj.h FlagArray.h Attribute.h FixArrayState.h \
 PrecisionState.h FloatArrayState.h IntArrayState.h PortHole.h \
 Plasma.h ParticleStack.h PtGate.h
Target.o: Target.cc Target.h StringList.h miscFuncs.h logNew.h Error.h \
 DataStruct.h type.h Block.h NamedObj.h isa.h FlagArray.h PortHole.h \
 dataType.h Particle.h Fix.h ComplexSubset.h Attribute.h State.h \
 InvokeInterp.h Scheduler.h SimControl.h Galaxy.h Star.h GalIter.h \
 ConstIters.h Domain.h StringArrayState.h
TimeVal.o: TimeVal.cc TimeVal.h ../../src/compat/ptolemy/compat.h
Tokenizer.o: Tokenizer.cc Tokenizer.h DataStruct.h type.h logNew.h \
 miscFuncs.h
Universe.o: Universe.cc Target.h StringList.h miscFuncs.h logNew.h \
 Error.h DataStruct.h type.h Block.h NamedObj.h isa.h FlagArray.h \
 PortHole.h dataType.h Particle.h Fix.h ComplexSubset.h Attribute.h \
 State.h InvokeInterp.h Universe.h Galaxy.h Star.h GalIter.h \
 KnownTarget.h
Wormhole.o: Wormhole.cc Wormhole.h type.h logNew.h Universe.h Galaxy.h \
 Star.h Block.h NamedObj.h StringList.h miscFuncs.h Error.h \
 DataStruct.h isa.h FlagArray.h PortHole.h dataType.h Particle.h Fix.h \
 ComplexSubset.h Attribute.h State.h InvokeInterp.h Target.h \
 EventHorizon.h Scheduler.h SimControl.h Domain.h
checkConnect.o: checkConnect.cc Galaxy.h Star.h Block.h NamedObj.h \
 StringList.h miscFuncs.h logNew.h Error.h DataStruct.h type.h isa.h \
 FlagArray.h PortHole.h dataType.h Particle.h Fix.h ComplexSubset.h \
 Attribute.h State.h InvokeInterp.h GalIter.h checkConnect.h
logNew.o: logNew.cc
miscFuncs.o: miscFuncs.cc Attribute.h ConstIters.h Block.h NamedObj.h \
 StringList.h miscFuncs.h logNew.h Error.h DataStruct.h type.h isa.h \
 FlagArray.h PortHole.h dataType.h Particle.h Fix.h ComplexSubset.h \
 State.h InvokeInterp.h Galaxy.h Star.h
paths.o: paths.cc miscFuncs.h StringList.h DataStruct.h type.h \
 logNew.h Error.h paths.h
ptsignals.o: ptsignals.cc ../../src/compat/ptolemy/compat.h \
 ptsignals.h
pt_fstream.o: pt_fstream.cc pt_fstream.h miscFuncs.h logNew.h Error.h \
 StringList.h DataStruct.h type.h ../../src/compat/ptolemy/compat.h
textAnimate.o: textAnimate.cc SimControl.h Star.h Block.h NamedObj.h \
 StringList.h miscFuncs.h logNew.h Error.h DataStruct.h type.h isa.h \
 FlagArray.h PortHole.h dataType.h Particle.h Fix.h ComplexSubset.h \
 Attribute.h State.h InvokeInterp.h textAnimate.h
expandPath.o: expandPath.cc Error.h Tokenizer.h DataStruct.h type.h \
 logNew.h StringList.h miscFuncs.h
subCharByString.o: subCharByString.cc StringList.h miscFuncs.h \
 logNew.h Error.h DataStruct.h type.h
RanGen.o: RanGen.cc logNew.h
hashstring.o: hashstring.cc logNew.h miscFuncs.h Error.h
