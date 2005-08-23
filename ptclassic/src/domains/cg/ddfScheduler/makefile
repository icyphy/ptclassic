# Version identification:
# @(#)make.template	1.5	1/1/96
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
VPATH = 	$(ROOT)/src/domains/cg/ddfScheduler

# get configuration info
CONFIG =	$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

ifeq ($(OPTIMIZER),-O2)
	# Purify 3.0.1 under Solaris2.4 with gcc-2.7.0/libg++-2.7
	# fails to process libcgddf.so if it is compiled with -O2
	OPTIMIZER =	-O
endif

# CG directory
CGDIR =		$(ROOT)/src/domains/cg/kernel
SDFDIR = 	$(ROOT)/src/domains/sdf/kernel
PARDIR = 	$(ROOT)/src/domains/cg/parScheduler
DLDIR = 	$(ROOT)/src/domains/cg/dlScheduler
MACRODIR = 	$(ROOT)/src/domains/cg/macroScheduler

INCL = -I$(KERNDIR) -I$(SDFDIR) -I$(CGDIR) -I$(PARDIR) -I$(DLDIR) -I$(MACRODIR)

# Library name
PTLIB = 	libcgddf.$(LIBSUFFIX)

# Header files
HDRS = 	CGDDFScheduler.h CGDDFCluster.h CGConScheduler.h \
	Distribution.h Case.h DoWhile.h For.h Recur.h DyConstruct.h \
	distributions.h dist_template.h \
	CGCaseScheduler.h CGDoWhileScheduler.h \
	CGForScheduler.h CGRecurScheduler.h	
 
# Library objects
OBJS =	CGDDFScheduler.o CGDDFCluster.o CGConScheduler.o \
	Distribution.o distributions.o dist_debug.o \
	Case.o DoWhile.o Recur.o For.o DyConstruct.o \
	CGCaseScheduler.o CGForScheduler.o \
	CGDoWhileScheduler.o CGRecurScheduler.o

# Library sources
SRCS =	$(OBJS:.o=.cc)

all:	makefile $(PTLIB)

# "make install" puts things in the official place
install:	makefile $(LIBDIR)/$(PTLIB)

# include common definitions, rules
include $(ROOT)/mk/common.mk

# Don't add anything after the next line; makedepend will zap it.
# DO NOT DELETE THIS LINE -- make depend depends on it.

CGDDFScheduler.o: CGDDFScheduler.cc CGDDFScheduler.h \
 ../../../../src/domains/cg/macroScheduler/MacroScheduler.h \
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
 ../../../../src/domains/cg/parScheduler/ParGraph.h \
 ../../../../src/domains/cg/macroScheduler/MacroParProcs.h \
 ../../../../src/domains/cg/dlScheduler/DLParProcs.h \
 ../../../../src/domains/cg/dlScheduler/DLGraph.h \
 ../../../../src/domains/cg/dlScheduler/DLNode.h \
 ../../../../src/domains/cg/macroScheduler/MacroGraph.h CGDDFCluster.h \
 ../../../../src/domains/cg/macroScheduler/CGMacroCluster.h \
 CGConScheduler.h Distribution.h
CGDDFCluster.o: CGDDFCluster.cc CGDDFCluster.h \
 ../../../../src/domains/cg/macroScheduler/CGMacroCluster.h \
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
 ../../../../src/domains/cg/kernel/MultiTarget.h \
 ../../../../src/domains/cg/macroScheduler/MacroScheduler.h \
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
 ../../../../src/domains/cg/parScheduler/ParGraph.h \
 ../../../../src/domains/cg/macroScheduler/MacroParProcs.h \
 ../../../../src/domains/cg/dlScheduler/DLParProcs.h \
 ../../../../src/domains/cg/dlScheduler/DLGraph.h \
 ../../../../src/domains/cg/dlScheduler/DLNode.h \
 ../../../../src/domains/cg/macroScheduler/MacroGraph.h \
 CGConScheduler.h Distribution.h ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/Plasma.h ../../../../src/kernel/PtGate.h \
 ../../../../src/domains/cg/kernel/CGUtilities.h CGCaseScheduler.h \
 Case.h DyConstruct.h CGDoWhileScheduler.h DoWhile.h CGForScheduler.h \
 For.h CGRecurScheduler.h Recur.h
CGConScheduler.o: CGConScheduler.cc CGConScheduler.h \
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
 ../../../../src/domains/cg/kernel/Profile.h \
 ../../../../src/domains/cg/kernel/IntArray.h Distribution.h \
 ../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h \
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
 ../../../../src/kernel/DataFlowStar.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/cg/kernel/ImplementationCost.h \
 ../../../../src/domains/cg/kernel/ConversionTable.h \
 ../../../../src/domains/cg/macroScheduler/CGMacroCluster.h \
 ../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../src/kernel/DynamicGalaxy.h \
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/GalIter.h \
 ../../../../src/domains/cg/macroScheduler/MacroScheduler.h \
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
 ../../../../src/domains/cg/parScheduler/ParGraph.h \
 ../../../../src/domains/cg/macroScheduler/MacroParProcs.h \
 ../../../../src/domains/cg/dlScheduler/DLParProcs.h \
 ../../../../src/domains/cg/dlScheduler/DLGraph.h \
 ../../../../src/domains/cg/dlScheduler/DLNode.h \
 ../../../../src/domains/cg/macroScheduler/MacroGraph.h \
 ../../../../src/kernel/EventHorizon.h CGDDFCluster.h
Distribution.o: Distribution.cc Distribution.h dist_template.h \
 distributions.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/StringState.h \
 ../../../../src/kernel/State.h ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/kernel/KnownState.h ../../../../src/kernel/Block.h \
 ../../../../src/kernel/PortHole.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/domains/cg/kernel/MultiTarget.h \
 ../../../../src/domains/cg/kernel/CGTarget.h \
 ../../../../src/kernel/Target.h ../../../../src/kernel/IntState.h \
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
 ../../../../src/domains/cg/kernel/IntArray.h
distributions.o: distributions.cc distributions.h Distribution.h \
 dist_template.h ../../../../src/domains/cg/kernel/MultiTarget.h \
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
 ../../../../src/kernel/FloatState.h
dist_debug.o: dist_debug.cc distributions.h Distribution.h \
 dist_template.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h
Case.o: Case.cc ../../../../src/kernel/type.h \
 ../../../../src/kernel/logNew.h Case.h DyConstruct.h \
 ../../../../src/kernel/DataStruct.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/isa.h ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/Galaxy.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/kernel/State.h ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/GalIter.h \
 ../../../../src/domains/cg/macroScheduler/CGMacroCluster.h \
 ../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/kernel/DataFlowStar.h \
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
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/Scheduler.h \
 ../../../../src/domains/cg/kernel/Profile.h \
 ../../../../src/domains/cg/kernel/IntArray.h \
 ../../../../src/domains/cg/kernel/MultiTarget.h \
 ../../../../src/domains/cg/macroScheduler/MacroScheduler.h \
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
 ../../../../src/domains/cg/parScheduler/ParGraph.h \
 ../../../../src/domains/cg/macroScheduler/MacroParProcs.h \
 ../../../../src/domains/cg/dlScheduler/DLParProcs.h \
 ../../../../src/domains/cg/dlScheduler/DLGraph.h \
 ../../../../src/domains/cg/dlScheduler/DLNode.h \
 ../../../../src/domains/cg/macroScheduler/MacroGraph.h
DoWhile.o: DoWhile.cc ../../../../src/kernel/type.h \
 ../../../../src/kernel/logNew.h DoWhile.h DyConstruct.h \
 ../../../../src/kernel/DataStruct.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/isa.h ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/Galaxy.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/kernel/State.h ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/GalIter.h \
 ../../../../src/domains/cg/macroScheduler/CGMacroCluster.h \
 ../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/kernel/DataFlowStar.h \
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
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/Scheduler.h \
 ../../../../src/domains/cg/kernel/Profile.h \
 ../../../../src/domains/cg/kernel/IntArray.h \
 ../../../../src/domains/cg/kernel/MultiTarget.h \
 ../../../../src/domains/cg/macroScheduler/MacroScheduler.h \
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
 ../../../../src/domains/cg/parScheduler/ParGraph.h \
 ../../../../src/domains/cg/macroScheduler/MacroParProcs.h \
 ../../../../src/domains/cg/dlScheduler/DLParProcs.h \
 ../../../../src/domains/cg/dlScheduler/DLGraph.h \
 ../../../../src/domains/cg/dlScheduler/DLNode.h \
 ../../../../src/domains/cg/macroScheduler/MacroGraph.h
Recur.o: Recur.cc ../../../../src/kernel/type.h \
 ../../../../src/kernel/logNew.h Recur.h DyConstruct.h \
 ../../../../src/kernel/DataStruct.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/isa.h ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/Galaxy.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/kernel/State.h ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/domains/cg/macroScheduler/CGMacroCluster.h \
 ../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/kernel/DataFlowStar.h \
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
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/Scheduler.h ../../../../src/kernel/GalIter.h \
 ../../../../src/domains/cg/kernel/Profile.h \
 ../../../../src/domains/cg/kernel/IntArray.h \
 ../../../../src/domains/cg/kernel/MultiTarget.h \
 ../../../../src/domains/cg/macroScheduler/MacroScheduler.h \
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
 ../../../../src/domains/cg/parScheduler/ParGraph.h \
 ../../../../src/domains/cg/macroScheduler/MacroParProcs.h \
 ../../../../src/domains/cg/dlScheduler/DLParProcs.h \
 ../../../../src/domains/cg/dlScheduler/DLGraph.h \
 ../../../../src/domains/cg/dlScheduler/DLNode.h \
 ../../../../src/domains/cg/macroScheduler/MacroGraph.h
For.o: For.cc ../../../../src/kernel/type.h \
 ../../../../src/kernel/logNew.h For.h DyConstruct.h \
 ../../../../src/kernel/DataStruct.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/isa.h ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/Galaxy.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/kernel/State.h ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/GalIter.h \
 ../../../../src/domains/cg/macroScheduler/CGMacroCluster.h \
 ../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/kernel/DataFlowStar.h \
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
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/Scheduler.h \
 ../../../../src/domains/cg/kernel/Profile.h \
 ../../../../src/domains/cg/kernel/IntArray.h \
 ../../../../src/domains/cg/kernel/MultiTarget.h \
 ../../../../src/domains/cg/macroScheduler/MacroScheduler.h \
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
 ../../../../src/domains/cg/parScheduler/ParGraph.h \
 ../../../../src/domains/cg/macroScheduler/MacroParProcs.h \
 ../../../../src/domains/cg/dlScheduler/DLParProcs.h \
 ../../../../src/domains/cg/dlScheduler/DLGraph.h \
 ../../../../src/domains/cg/dlScheduler/DLNode.h \
 ../../../../src/domains/cg/macroScheduler/MacroGraph.h
DyConstruct.o: DyConstruct.cc DyConstruct.h \
 ../../../../src/kernel/DataStruct.h ../../../../src/kernel/type.h \
 ../../../../src/kernel/logNew.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/isa.h ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/Galaxy.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/kernel/State.h ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/domains/cg/macroScheduler/CGMacroCluster.h \
 ../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/kernel/DataFlowStar.h \
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
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/Scheduler.h ../../../../src/kernel/GalIter.h \
 ../../../../src/domains/cg/kernel/Profile.h \
 ../../../../src/domains/cg/kernel/IntArray.h \
 ../../../../src/domains/cg/kernel/MultiTarget.h \
 ../../../../src/domains/cg/macroScheduler/MacroScheduler.h \
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
 ../../../../src/domains/cg/parScheduler/ParGraph.h \
 ../../../../src/domains/cg/macroScheduler/MacroParProcs.h \
 ../../../../src/domains/cg/dlScheduler/DLParProcs.h \
 ../../../../src/domains/cg/dlScheduler/DLGraph.h \
 ../../../../src/domains/cg/dlScheduler/DLNode.h \
 ../../../../src/domains/cg/macroScheduler/MacroGraph.h
CGCaseScheduler.o: CGCaseScheduler.cc CGCaseScheduler.h \
 CGConScheduler.h ../../../../src/kernel/Scheduler.h \
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
 ../../../../src/domains/cg/kernel/Profile.h \
 ../../../../src/domains/cg/kernel/IntArray.h Distribution.h \
 ../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h \
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
 ../../../../src/kernel/DataFlowStar.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/cg/kernel/ImplementationCost.h \
 ../../../../src/domains/cg/kernel/ConversionTable.h \
 ../../../../src/domains/cg/macroScheduler/CGMacroCluster.h \
 ../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../src/kernel/DynamicGalaxy.h \
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/GalIter.h \
 ../../../../src/domains/cg/macroScheduler/MacroScheduler.h \
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
 ../../../../src/domains/cg/parScheduler/ParGraph.h \
 ../../../../src/domains/cg/macroScheduler/MacroParProcs.h \
 ../../../../src/domains/cg/dlScheduler/DLParProcs.h \
 ../../../../src/domains/cg/dlScheduler/DLGraph.h \
 ../../../../src/domains/cg/dlScheduler/DLNode.h \
 ../../../../src/domains/cg/macroScheduler/MacroGraph.h Case.h \
 DyConstruct.h distributions.h dist_template.h \
 ../../../../src/kernel/Domain.h \
 ../../../../src/domains/cg/kernel/CGDDFCode.h
CGForScheduler.o: CGForScheduler.cc CGForScheduler.h CGConScheduler.h \
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
 ../../../../src/domains/cg/kernel/Profile.h \
 ../../../../src/domains/cg/kernel/IntArray.h Distribution.h \
 ../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h \
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
 ../../../../src/kernel/DataFlowStar.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/cg/kernel/ImplementationCost.h \
 ../../../../src/domains/cg/kernel/ConversionTable.h \
 ../../../../src/domains/cg/macroScheduler/CGMacroCluster.h \
 ../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../src/kernel/DynamicGalaxy.h \
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/GalIter.h \
 ../../../../src/domains/cg/macroScheduler/MacroScheduler.h \
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
 ../../../../src/domains/cg/parScheduler/ParGraph.h \
 ../../../../src/domains/cg/macroScheduler/MacroParProcs.h \
 ../../../../src/domains/cg/dlScheduler/DLParProcs.h \
 ../../../../src/domains/cg/dlScheduler/DLGraph.h \
 ../../../../src/domains/cg/dlScheduler/DLNode.h \
 ../../../../src/domains/cg/macroScheduler/MacroGraph.h For.h \
 DyConstruct.h distributions.h dist_template.h \
 ../../../../src/kernel/KnownBlock.h ../../../../src/kernel/Linker.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h CGDDFCluster.h \
 ../../../../src/domains/cg/kernel/CGDDFCode.h
CGDoWhileScheduler.o: CGDoWhileScheduler.cc CGDoWhileScheduler.h \
 CGConScheduler.h ../../../../src/kernel/Scheduler.h \
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
 ../../../../src/domains/cg/kernel/Profile.h \
 ../../../../src/domains/cg/kernel/IntArray.h Distribution.h \
 ../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h \
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
 ../../../../src/kernel/DataFlowStar.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/cg/kernel/ImplementationCost.h \
 ../../../../src/domains/cg/kernel/ConversionTable.h \
 ../../../../src/domains/cg/macroScheduler/CGMacroCluster.h \
 ../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../src/kernel/DynamicGalaxy.h \
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/GalIter.h \
 ../../../../src/domains/cg/macroScheduler/MacroScheduler.h \
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
 ../../../../src/domains/cg/parScheduler/ParGraph.h \
 ../../../../src/domains/cg/macroScheduler/MacroParProcs.h \
 ../../../../src/domains/cg/dlScheduler/DLParProcs.h \
 ../../../../src/domains/cg/dlScheduler/DLGraph.h \
 ../../../../src/domains/cg/dlScheduler/DLNode.h \
 ../../../../src/domains/cg/macroScheduler/MacroGraph.h DoWhile.h \
 DyConstruct.h distributions.h dist_template.h \
 ../../../../src/domains/cg/kernel/CGDDFCode.h
CGRecurScheduler.o: CGRecurScheduler.cc CGRecurScheduler.h \
 CGConScheduler.h ../../../../src/kernel/Scheduler.h \
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
 ../../../../src/domains/cg/kernel/Profile.h \
 ../../../../src/domains/cg/kernel/IntArray.h Distribution.h \
 ../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h \
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
 ../../../../src/kernel/DataFlowStar.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/cg/kernel/ImplementationCost.h \
 ../../../../src/domains/cg/kernel/ConversionTable.h \
 ../../../../src/domains/cg/macroScheduler/CGMacroCluster.h \
 ../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../src/kernel/DynamicGalaxy.h \
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/GalIter.h \
 ../../../../src/domains/cg/macroScheduler/MacroScheduler.h \
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
 ../../../../src/domains/cg/parScheduler/ParGraph.h \
 ../../../../src/domains/cg/macroScheduler/MacroParProcs.h \
 ../../../../src/domains/cg/dlScheduler/DLParProcs.h \
 ../../../../src/domains/cg/dlScheduler/DLGraph.h \
 ../../../../src/domains/cg/dlScheduler/DLNode.h \
 ../../../../src/domains/cg/macroScheduler/MacroGraph.h Recur.h \
 DyConstruct.h distributions.h dist_template.h \
 ../../../../src/kernel/KnownBlock.h ../../../../src/kernel/Linker.h \
 ../../../../src/domains/cg/kernel/CGDDFCode.h
