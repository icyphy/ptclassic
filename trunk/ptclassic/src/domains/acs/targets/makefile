# Makefile for Ptolemy Adaptive Computing Systems Targets
# @(#)make.template	1.5 08/02/01
# Copyright (c) 1998-2001 The Regents of the University of California.
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
# Code generation targets

# Root of Ptolemy directory
ROOT =		../../../..
# True source directory
VPATH = 	$(ROOT)/src/domains/acs/targets

# get configuration info
CONFIG =	$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

ACSKERNEL = 	$(ROOT)/src/domains/acs/kernel
ACSTARGET = 	$(ROOT)/src/domains/acs/targets
CGKERNEL = 	$(ROOT)/src/domains/cg/kernel
SDFKERNEL = 	$(ROOT)/src/domains/sdf/kernel
LOOPDIR =	$(ROOT)/src/domains/sdf/loopScheduler

INCL =		-I$(ACSKERNEL) -I$(ACSTARGET) -I$(CGKERNEL) -I$(SDFKERNEL) -I$(KERNDIR) -I$(LOOPDIR) $(PTCOMPAT_INCSPEC) 

PL_SRCS = 

PLSRC = $(PL_SRCS:.pl=.cc)

SRCS =	\
	ACSIntArray.cc \
	DoubleArray.cc \
	StringArray.cc \
	CoreList.cc \
	Connection_List.cc \
	utilities.cc \
	Delay.cc \
	Connectivity.cc \
	Pin.cc \
	acs_vhdl_lang.cc \
	Directory.cc \
	BindingRule.cc \
	Arch.cc \
	Fpga.cc \
	Port.cc \
	Address.cc \
	Sg_Constructs.cc \
	Port_Timing.cc \
	Pipealign.cc \
	Capability.cc \
	Resource.cc \
	Constants.cc \
	Sequencer.cc \
	HWio_add.cc \
	HWutils.cc \
	HWio_matlab.cc \
	HWTarget.cc \
	ACSCGFPGATarget.cc \
	ACSCGTarget.cc \
	ACSCGCTarget.cc 
#	ACSJavaTarget.cc \
#	ACSFixTarget.cc \
#	ACSFloatTarget.cc \
#	ACSSimTarget.cc \
#	$(PLSRC) 

EXTRA_SRCS = $(PL_SRCS)

HDRS=$(SRCS:.cc=.h)

OBJS = $(SRCS:.cc=.o)

ifeq ($(USE_SHARED_LIBS),yes) 

# If we are building shared libraries, then we can put all the targets in
# a shared library.
PTLIB = 	libacstargets.$(LIBSUFFIX)
all:		$(SRCS) makefile $(OBJS) $(PTLIB)
install:	all $(LIBDIR)/$(PTLIB)

else
PTLIB=none	# required by common.mk
all:		makefile $(OBJS)
# "make install" puts things in the official place
install:	all
		@echo Installation complete.
endif

# include common definitions, rules
include $(ROOT)/mk/common.mk

# Don't add anything after the next line; makedepend will zap it.
# DO NOT DELETE THIS LINE -- make depend depends on it.


ACSIntArray.o: ACSIntArray.cc ACSIntArray.h
DoubleArray.o: DoubleArray.cc DoubleArray.h
StringArray.o: StringArray.cc StringArray.h
CoreList.o: CoreList.cc CoreList.h \
 ../../../../src/domains/acs/kernel/ACSCGFPGACore.h \
 ../../../../src/domains/acs/targets/StringArray.h \
 ../../../../src/domains/acs/targets/CoreList.h \
 ../../../../src/domains/acs/targets/Pipealign.h \
 ../../../../src/domains/acs/targets/ACSIntArray.h \
 ../../../../src/domains/acs/targets/acs_starconsts.h \
 ../../../../src/domains/acs/targets/Connection_List.h \
 ../../../../src/domains/acs/targets/Pin.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h \
 ../../../../src/domains/acs/targets/Connectivity.h \
 ../../../../src/domains/acs/targets/Delay.h \
 ../../../../src/domains/acs/targets/Capability.h \
 ../../../../src/domains/acs/targets/Constants.h \
 ../../../../src/domains/acs/targets/Resource.h \
 ../../../../src/domains/acs/targets/Sg_Constructs.h \
 ../../../../src/kernel/KnownBlock.h ../../../../src/kernel/Block.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h ../../../../src/kernel/Linker.h \
 ../../../../src/domains/acs/targets/Fpga.h \
 ../../../../src/domains/acs/targets/Sequencer.h \
 ../../../../src/domains/acs/targets/Port_Timing.h \
 ../../../../src/domains/acs/targets/acs_vhdl_lang.h \
 ../../../../src/domains/acs/kernel/ACSCGCore.h \
 ../../../../src/domains/acs/kernel/ACSCore.h \
 ../../../../src/domains/acs/kernel/ACSStar.h \
 ../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
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
 ../../../../src/domains/acs/kernel/ACSPortHole.h \
 ../../../../src/domains/acs/kernel/ACSGeodesic.h \
 ../../../../src/domains/cg/kernel/CGGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/domains/acs/kernel/ACSCorona.h \
 ../../../../src/domains/acs/kernel/ACSKnownCategory.h \
 ../../../../src/domains/acs/kernel/ACSWormhole.h \
 ../../../../src/kernel/Wormhole.h ../../../../src/kernel/Universe.h \
 ../../../../src/kernel/Galaxy.h ../../../../src/kernel/EventHorizon.h
Connection_List.o: Connection_List.cc Connection_List.h ACSIntArray.h
utilities.o: utilities.cc utilities.h
Delay.o: Delay.cc Delay.h ACSIntArray.h
Connectivity.o: Connectivity.cc Connectivity.h ACSIntArray.h \
 acs_starconsts.h
Pin.o: Pin.cc Pin.h ACSIntArray.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h StringArray.h Connectivity.h \
 acs_starconsts.h Delay.h
acs_vhdl_lang.o: acs_vhdl_lang.cc acs_vhdl_lang.h CoreList.h \
 ../../../../src/domains/acs/kernel/ACSCGFPGACore.h \
 ../../../../src/domains/acs/targets/StringArray.h \
 ../../../../src/domains/acs/targets/CoreList.h \
 ../../../../src/domains/acs/targets/Pipealign.h \
 ../../../../src/domains/acs/targets/ACSIntArray.h \
 ../../../../src/domains/acs/targets/acs_starconsts.h \
 ../../../../src/domains/acs/targets/Connection_List.h \
 ../../../../src/domains/acs/targets/Pin.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h \
 ../../../../src/domains/acs/targets/Connectivity.h \
 ../../../../src/domains/acs/targets/Delay.h \
 ../../../../src/domains/acs/targets/Capability.h \
 ../../../../src/domains/acs/targets/Constants.h \
 ../../../../src/domains/acs/targets/Resource.h \
 ../../../../src/domains/acs/targets/Sg_Constructs.h \
 ../../../../src/kernel/KnownBlock.h ../../../../src/kernel/Block.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h ../../../../src/kernel/Linker.h \
 ../../../../src/domains/acs/targets/Fpga.h \
 ../../../../src/domains/acs/targets/Sequencer.h \
 ../../../../src/domains/acs/targets/Port_Timing.h \
 ../../../../src/domains/acs/targets/acs_vhdl_lang.h \
 ../../../../src/domains/acs/kernel/ACSCGCore.h \
 ../../../../src/domains/acs/kernel/ACSCore.h \
 ../../../../src/domains/acs/kernel/ACSStar.h \
 ../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
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
 ../../../../src/domains/acs/kernel/ACSPortHole.h \
 ../../../../src/domains/acs/kernel/ACSGeodesic.h \
 ../../../../src/domains/cg/kernel/CGGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/domains/acs/kernel/ACSCorona.h \
 ../../../../src/domains/acs/kernel/ACSKnownCategory.h \
 ../../../../src/domains/acs/kernel/ACSWormhole.h \
 ../../../../src/kernel/Wormhole.h ../../../../src/kernel/Universe.h \
 ../../../../src/kernel/Galaxy.h ../../../../src/kernel/EventHorizon.h \
 ACSIntArray.h StringArray.h Pin.h Connectivity.h acs_starconsts.h
Directory.o: Directory.cc Directory.h
BindingRule.o: BindingRule.cc BindingRule.h ACSIntArray.h
Arch.o: Arch.cc Arch.h CoreList.h \
 ../../../../src/domains/acs/kernel/ACSCGFPGACore.h \
 ../../../../src/domains/acs/targets/StringArray.h \
 ../../../../src/domains/acs/targets/CoreList.h \
 ../../../../src/domains/acs/targets/Pipealign.h \
 ../../../../src/domains/acs/targets/ACSIntArray.h \
 ../../../../src/domains/acs/targets/acs_starconsts.h \
 ../../../../src/domains/acs/targets/Connection_List.h \
 ../../../../src/domains/acs/targets/Pin.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h \
 ../../../../src/domains/acs/targets/Connectivity.h \
 ../../../../src/domains/acs/targets/Delay.h \
 ../../../../src/domains/acs/targets/Capability.h \
 ../../../../src/domains/acs/targets/Constants.h \
 ../../../../src/domains/acs/targets/Resource.h \
 ../../../../src/domains/acs/targets/Sg_Constructs.h \
 ../../../../src/kernel/KnownBlock.h ../../../../src/kernel/Block.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h ../../../../src/kernel/Linker.h \
 ../../../../src/domains/acs/targets/Fpga.h \
 ../../../../src/domains/acs/targets/Sequencer.h \
 ../../../../src/domains/acs/targets/Port_Timing.h \
 ../../../../src/domains/acs/targets/acs_vhdl_lang.h \
 ../../../../src/domains/acs/kernel/ACSCGCore.h \
 ../../../../src/domains/acs/kernel/ACSCore.h \
 ../../../../src/domains/acs/kernel/ACSStar.h \
 ../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
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
 ../../../../src/domains/acs/kernel/ACSPortHole.h \
 ../../../../src/domains/acs/kernel/ACSGeodesic.h \
 ../../../../src/domains/cg/kernel/CGGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/domains/acs/kernel/ACSCorona.h \
 ../../../../src/domains/acs/kernel/ACSKnownCategory.h \
 ../../../../src/domains/acs/kernel/ACSWormhole.h \
 ../../../../src/kernel/Wormhole.h ../../../../src/kernel/Universe.h \
 ../../../../src/kernel/Galaxy.h ../../../../src/kernel/EventHorizon.h \
 Fpga.h Port.h Sg_Constructs.h ACSIntArray.h Port_Timing.h Address.h \
 Pin.h utilities.h StringArray.h
Fpga.o: Fpga.cc Fpga.h ACSIntArray.h CoreList.h \
 ../../../../src/domains/acs/kernel/ACSCGFPGACore.h \
 ../../../../src/domains/acs/targets/StringArray.h \
 ../../../../src/domains/acs/targets/CoreList.h \
 ../../../../src/domains/acs/targets/Pipealign.h \
 ../../../../src/domains/acs/targets/ACSIntArray.h \
 ../../../../src/domains/acs/targets/acs_starconsts.h \
 ../../../../src/domains/acs/targets/Connection_List.h \
 ../../../../src/domains/acs/targets/Pin.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h \
 ../../../../src/domains/acs/targets/Connectivity.h \
 ../../../../src/domains/acs/targets/Delay.h \
 ../../../../src/domains/acs/targets/Capability.h \
 ../../../../src/domains/acs/targets/Constants.h \
 ../../../../src/domains/acs/targets/Resource.h \
 ../../../../src/domains/acs/targets/Sg_Constructs.h \
 ../../../../src/kernel/KnownBlock.h ../../../../src/kernel/Block.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h ../../../../src/kernel/Linker.h \
 ../../../../src/domains/acs/targets/Fpga.h \
 ../../../../src/domains/acs/targets/acs_vhdl_lang.h \
 ../../../../src/domains/acs/kernel/ACSCGCore.h \
 ../../../../src/domains/acs/kernel/ACSCore.h \
 ../../../../src/domains/acs/kernel/ACSStar.h \
 ../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
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
 ../../../../src/domains/acs/kernel/ACSPortHole.h \
 ../../../../src/domains/acs/kernel/ACSGeodesic.h \
 ../../../../src/domains/cg/kernel/CGGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/domains/acs/kernel/ACSCorona.h \
 ../../../../src/domains/acs/kernel/ACSKnownCategory.h \
 ../../../../src/domains/acs/kernel/ACSWormhole.h \
 ../../../../src/kernel/Wormhole.h ../../../../src/kernel/Universe.h \
 ../../../../src/kernel/Galaxy.h ../../../../src/kernel/EventHorizon.h \
 Pin.h Sequencer.h Port_Timing.h acs_starconsts.h Sg_Constructs.h \
 acs_vhdl_lang.h
Port.o: Port.cc Port.h \
 ../../../../src/domains/acs/kernel/ACSCGFPGACore.h \
 ../../../../src/domains/acs/targets/StringArray.h \
 ../../../../src/domains/acs/targets/CoreList.h \
 ../../../../src/domains/acs/targets/Pipealign.h \
 ../../../../src/domains/acs/targets/ACSIntArray.h \
 ../../../../src/domains/acs/targets/acs_starconsts.h \
 ../../../../src/domains/acs/targets/Connection_List.h \
 ../../../../src/domains/acs/targets/Pin.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h \
 ../../../../src/domains/acs/targets/Connectivity.h \
 ../../../../src/domains/acs/targets/Delay.h \
 ../../../../src/domains/acs/targets/Capability.h \
 ../../../../src/domains/acs/targets/Constants.h \
 ../../../../src/domains/acs/targets/Resource.h \
 ../../../../src/domains/acs/targets/Sg_Constructs.h \
 ../../../../src/kernel/KnownBlock.h ../../../../src/kernel/Block.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h ../../../../src/kernel/Linker.h \
 ../../../../src/domains/acs/targets/Fpga.h \
 ../../../../src/domains/acs/targets/Sequencer.h \
 ../../../../src/domains/acs/targets/Port_Timing.h \
 ../../../../src/domains/acs/targets/acs_vhdl_lang.h \
 ../../../../src/domains/acs/kernel/ACSCGCore.h \
 ../../../../src/domains/acs/kernel/ACSCore.h \
 ../../../../src/domains/acs/kernel/ACSStar.h \
 ../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
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
 ../../../../src/domains/acs/kernel/ACSPortHole.h \
 ../../../../src/domains/acs/kernel/ACSGeodesic.h \
 ../../../../src/domains/cg/kernel/CGGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/domains/acs/kernel/ACSCorona.h \
 ../../../../src/domains/acs/kernel/ACSKnownCategory.h \
 ../../../../src/domains/acs/kernel/ACSWormhole.h \
 ../../../../src/kernel/Wormhole.h ../../../../src/kernel/Universe.h \
 ../../../../src/kernel/Galaxy.h ../../../../src/kernel/EventHorizon.h \
 Sg_Constructs.h CoreList.h ACSIntArray.h Fpga.h Port_Timing.h \
 Address.h Pin.h utilities.h
Address.o: Address.cc Address.h \
 ../../../../src/domains/acs/kernel/ACSCGFPGACore.h \
 ../../../../src/domains/acs/targets/StringArray.h \
 ../../../../src/domains/acs/targets/CoreList.h \
 ../../../../src/domains/acs/targets/Pipealign.h \
 ../../../../src/domains/acs/targets/ACSIntArray.h \
 ../../../../src/domains/acs/targets/acs_starconsts.h \
 ../../../../src/domains/acs/targets/Connection_List.h \
 ../../../../src/domains/acs/targets/Pin.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h \
 ../../../../src/domains/acs/targets/Connectivity.h \
 ../../../../src/domains/acs/targets/Delay.h \
 ../../../../src/domains/acs/targets/Capability.h \
 ../../../../src/domains/acs/targets/Constants.h \
 ../../../../src/domains/acs/targets/Resource.h \
 ../../../../src/domains/acs/targets/Sg_Constructs.h \
 ../../../../src/kernel/KnownBlock.h ../../../../src/kernel/Block.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h ../../../../src/kernel/Linker.h \
 ../../../../src/domains/acs/targets/Fpga.h \
 ../../../../src/domains/acs/targets/Sequencer.h \
 ../../../../src/domains/acs/targets/Port_Timing.h \
 ../../../../src/domains/acs/targets/acs_vhdl_lang.h \
 ../../../../src/domains/acs/kernel/ACSCGCore.h \
 ../../../../src/domains/acs/kernel/ACSCore.h \
 ../../../../src/domains/acs/kernel/ACSStar.h \
 ../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
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
 ../../../../src/domains/acs/kernel/ACSPortHole.h \
 ../../../../src/domains/acs/kernel/ACSGeodesic.h \
 ../../../../src/domains/cg/kernel/CGGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/domains/acs/kernel/ACSCorona.h \
 ../../../../src/domains/acs/kernel/ACSKnownCategory.h \
 ../../../../src/domains/acs/kernel/ACSWormhole.h \
 ../../../../src/kernel/Wormhole.h ../../../../src/kernel/Universe.h \
 ../../../../src/kernel/Galaxy.h ../../../../src/kernel/EventHorizon.h \
 CoreList.h ACSIntArray.h Pin.h Sg_Constructs.h utilities.h
Sg_Constructs.o: Sg_Constructs.cc Sg_Constructs.h \
 ../../../../src/kernel/KnownBlock.h ../../../../src/kernel/Block.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h ../../../../src/kernel/Linker.h \
 CoreList.h ../../../../src/domains/acs/kernel/ACSCGFPGACore.h \
 ../../../../src/domains/acs/targets/StringArray.h \
 ../../../../src/domains/acs/targets/CoreList.h \
 ../../../../src/domains/acs/targets/Pipealign.h \
 ../../../../src/domains/acs/targets/ACSIntArray.h \
 ../../../../src/domains/acs/targets/acs_starconsts.h \
 ../../../../src/domains/acs/targets/Connection_List.h \
 ../../../../src/domains/acs/targets/Pin.h \
 ../../../../src/domains/acs/targets/Connectivity.h \
 ../../../../src/domains/acs/targets/Delay.h \
 ../../../../src/domains/acs/targets/Capability.h \
 ../../../../src/domains/acs/targets/Constants.h \
 ../../../../src/domains/acs/targets/Resource.h \
 ../../../../src/domains/acs/targets/Sg_Constructs.h \
 ../../../../src/domains/acs/targets/acs_vhdl_lang.h \
 ../../../../src/domains/acs/kernel/ACSCGCore.h \
 ../../../../src/domains/acs/kernel/ACSCore.h \
 ../../../../src/domains/acs/kernel/ACSStar.h \
 ../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
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
 ../../../../src/domains/acs/kernel/ACSPortHole.h \
 ../../../../src/domains/acs/kernel/ACSGeodesic.h \
 ../../../../src/domains/cg/kernel/CGGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/domains/acs/kernel/ACSCorona.h \
 ../../../../src/domains/acs/kernel/ACSKnownCategory.h \
 ../../../../src/domains/acs/kernel/ACSWormhole.h \
 ../../../../src/kernel/Wormhole.h ../../../../src/kernel/Universe.h \
 ../../../../src/kernel/Galaxy.h ../../../../src/kernel/EventHorizon.h \
 Fpga.h ACSIntArray.h Pin.h Sequencer.h Port_Timing.h acs_starconsts.h \
 acs_vhdl_lang.h
Port_Timing.o: Port_Timing.cc Port_Timing.h ACSIntArray.h \
 acs_starconsts.h
Pipealign.o: Pipealign.cc Pipealign.h ACSIntArray.h acs_starconsts.h
Capability.o: Capability.cc Capability.h StringArray.h CoreList.h \
 ../../../../src/domains/acs/kernel/ACSCGFPGACore.h \
 ../../../../src/domains/acs/targets/StringArray.h \
 ../../../../src/domains/acs/targets/CoreList.h \
 ../../../../src/domains/acs/targets/Pipealign.h \
 ../../../../src/domains/acs/targets/ACSIntArray.h \
 ../../../../src/domains/acs/targets/acs_starconsts.h \
 ../../../../src/domains/acs/targets/Connection_List.h \
 ../../../../src/domains/acs/targets/Pin.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h \
 ../../../../src/domains/acs/targets/Connectivity.h \
 ../../../../src/domains/acs/targets/Delay.h \
 ../../../../src/domains/acs/targets/Capability.h \
 ../../../../src/domains/acs/targets/Constants.h \
 ../../../../src/domains/acs/targets/Resource.h \
 ../../../../src/domains/acs/targets/Sg_Constructs.h \
 ../../../../src/kernel/KnownBlock.h ../../../../src/kernel/Block.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h ../../../../src/kernel/Linker.h \
 ../../../../src/domains/acs/targets/Fpga.h \
 ../../../../src/domains/acs/targets/Sequencer.h \
 ../../../../src/domains/acs/targets/Port_Timing.h \
 ../../../../src/domains/acs/targets/acs_vhdl_lang.h \
 ../../../../src/domains/acs/kernel/ACSCGCore.h \
 ../../../../src/domains/acs/kernel/ACSCore.h \
 ../../../../src/domains/acs/kernel/ACSStar.h \
 ../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
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
 ../../../../src/domains/acs/kernel/ACSPortHole.h \
 ../../../../src/domains/acs/kernel/ACSGeodesic.h \
 ../../../../src/domains/cg/kernel/CGGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/domains/acs/kernel/ACSCorona.h \
 ../../../../src/domains/acs/kernel/ACSKnownCategory.h \
 ../../../../src/domains/acs/kernel/ACSWormhole.h \
 ../../../../src/kernel/Wormhole.h ../../../../src/kernel/Universe.h \
 ../../../../src/kernel/Galaxy.h ../../../../src/kernel/EventHorizon.h \
 acs_starconsts.h
Resource.o: Resource.cc Resource.h
Constants.o: Constants.cc Constants.h ACSIntArray.h acs_starconsts.h
Sequencer.o: Sequencer.cc Sequencer.h CoreList.h \
 ../../../../src/domains/acs/kernel/ACSCGFPGACore.h \
 ../../../../src/domains/acs/targets/StringArray.h \
 ../../../../src/domains/acs/targets/CoreList.h \
 ../../../../src/domains/acs/targets/Pipealign.h \
 ../../../../src/domains/acs/targets/ACSIntArray.h \
 ../../../../src/domains/acs/targets/acs_starconsts.h \
 ../../../../src/domains/acs/targets/Connection_List.h \
 ../../../../src/domains/acs/targets/Pin.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h \
 ../../../../src/domains/acs/targets/Connectivity.h \
 ../../../../src/domains/acs/targets/Delay.h \
 ../../../../src/domains/acs/targets/Capability.h \
 ../../../../src/domains/acs/targets/Constants.h \
 ../../../../src/domains/acs/targets/Resource.h \
 ../../../../src/domains/acs/targets/Sg_Constructs.h \
 ../../../../src/kernel/KnownBlock.h ../../../../src/kernel/Block.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h ../../../../src/kernel/Linker.h \
 ../../../../src/domains/acs/targets/Fpga.h \
 ../../../../src/domains/acs/targets/Sequencer.h \
 ../../../../src/domains/acs/targets/Port_Timing.h \
 ../../../../src/domains/acs/targets/acs_vhdl_lang.h \
 ../../../../src/domains/acs/kernel/ACSCGCore.h \
 ../../../../src/domains/acs/kernel/ACSCore.h \
 ../../../../src/domains/acs/kernel/ACSStar.h \
 ../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
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
 ../../../../src/domains/acs/kernel/ACSPortHole.h \
 ../../../../src/domains/acs/kernel/ACSGeodesic.h \
 ../../../../src/domains/cg/kernel/CGGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/domains/acs/kernel/ACSCorona.h \
 ../../../../src/domains/acs/kernel/ACSKnownCategory.h \
 ../../../../src/domains/acs/kernel/ACSWormhole.h \
 ../../../../src/kernel/Wormhole.h ../../../../src/kernel/Universe.h \
 ../../../../src/kernel/Galaxy.h ../../../../src/kernel/EventHorizon.h
HWio_add.o: HWio_add.cc HWio_add.h ACSCGFPGATarget.h ACSCGTarget.h \
 ../../../../src/domains/acs/kernel/ACSTarget.h \
 ../../../../src/domains/cg/kernel/HLLTarget.h \
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
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/Scheduler.h ../../../../src/kernel/Galaxy.h \
 ../../../../src/kernel/GalIter.h ../../../../src/kernel/DFPortHole.h \
 ../../../../src/kernel/FloatState.h \
 ../../../../src/domains/acs/kernel/ACSKnownCategory.h \
 ../../../../src/domains/acs/kernel/ACSCore.h \
 ../../../../src/domains/acs/kernel/ACSStar.h \
 ../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../src/domains/acs/kernel/ACSPortHole.h \
 ../../../../src/domains/acs/kernel/ACSGeodesic.h \
 ../../../../src/domains/cg/kernel/CGGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/domains/acs/kernel/ACSCorona.h acs_starconsts.h \
 ACSIntArray.h DoubleArray.h StringArray.h CoreList.h \
 ../../../../src/domains/acs/kernel/ACSCGFPGACore.h \
 ../../../../src/domains/acs/targets/StringArray.h \
 ../../../../src/domains/acs/targets/CoreList.h \
 ../../../../src/domains/acs/targets/Pipealign.h \
 ../../../../src/domains/acs/targets/ACSIntArray.h \
 ../../../../src/domains/acs/targets/acs_starconsts.h \
 ../../../../src/domains/acs/targets/Connection_List.h \
 ../../../../src/domains/acs/targets/Pin.h \
 ../../../../src/domains/acs/targets/Connectivity.h \
 ../../../../src/domains/acs/targets/Delay.h \
 ../../../../src/domains/acs/targets/Capability.h \
 ../../../../src/domains/acs/targets/Constants.h \
 ../../../../src/domains/acs/targets/Resource.h \
 ../../../../src/domains/acs/targets/Sg_Constructs.h \
 ../../../../src/kernel/KnownBlock.h ../../../../src/kernel/Linker.h \
 ../../../../src/domains/acs/targets/Fpga.h \
 ../../../../src/domains/acs/targets/Sequencer.h \
 ../../../../src/domains/acs/targets/Port_Timing.h \
 ../../../../src/domains/acs/targets/acs_vhdl_lang.h \
 ../../../../src/domains/acs/kernel/ACSCGCore.h \
 ../../../../src/domains/acs/kernel/ACSWormhole.h \
 ../../../../src/kernel/Wormhole.h ../../../../src/kernel/Universe.h \
 ../../../../src/kernel/EventHorizon.h Connection_List.h Arch.h Fpga.h \
 Port.h Sg_Constructs.h Port_Timing.h Address.h Pin.h utilities.h \
 acs_vhdl_lang.h BindingRule.h Directory.h
HWutils.o: HWutils.cc HWutils.h ACSCGFPGATarget.h ACSCGTarget.h \
 ../../../../src/domains/acs/kernel/ACSTarget.h \
 ../../../../src/domains/cg/kernel/HLLTarget.h \
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
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/Scheduler.h ../../../../src/kernel/Galaxy.h \
 ../../../../src/kernel/GalIter.h ../../../../src/kernel/DFPortHole.h \
 ../../../../src/kernel/FloatState.h \
 ../../../../src/domains/acs/kernel/ACSKnownCategory.h \
 ../../../../src/domains/acs/kernel/ACSCore.h \
 ../../../../src/domains/acs/kernel/ACSStar.h \
 ../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../src/domains/acs/kernel/ACSPortHole.h \
 ../../../../src/domains/acs/kernel/ACSGeodesic.h \
 ../../../../src/domains/cg/kernel/CGGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/domains/acs/kernel/ACSCorona.h acs_starconsts.h \
 ACSIntArray.h DoubleArray.h StringArray.h CoreList.h \
 ../../../../src/domains/acs/kernel/ACSCGFPGACore.h \
 ../../../../src/domains/acs/targets/StringArray.h \
 ../../../../src/domains/acs/targets/CoreList.h \
 ../../../../src/domains/acs/targets/Pipealign.h \
 ../../../../src/domains/acs/targets/ACSIntArray.h \
 ../../../../src/domains/acs/targets/acs_starconsts.h \
 ../../../../src/domains/acs/targets/Connection_List.h \
 ../../../../src/domains/acs/targets/Pin.h \
 ../../../../src/domains/acs/targets/Connectivity.h \
 ../../../../src/domains/acs/targets/Delay.h \
 ../../../../src/domains/acs/targets/Capability.h \
 ../../../../src/domains/acs/targets/Constants.h \
 ../../../../src/domains/acs/targets/Resource.h \
 ../../../../src/domains/acs/targets/Sg_Constructs.h \
 ../../../../src/kernel/KnownBlock.h ../../../../src/kernel/Linker.h \
 ../../../../src/domains/acs/targets/Fpga.h \
 ../../../../src/domains/acs/targets/Sequencer.h \
 ../../../../src/domains/acs/targets/Port_Timing.h \
 ../../../../src/domains/acs/targets/acs_vhdl_lang.h \
 ../../../../src/domains/acs/kernel/ACSCGCore.h \
 ../../../../src/domains/acs/kernel/ACSWormhole.h \
 ../../../../src/kernel/Wormhole.h ../../../../src/kernel/Universe.h \
 ../../../../src/kernel/EventHorizon.h Connection_List.h Arch.h Fpga.h \
 Port.h Sg_Constructs.h Port_Timing.h Address.h Pin.h utilities.h \
 acs_vhdl_lang.h BindingRule.h Directory.h
HWio_matlab.o: HWio_matlab.cc HWio_matlab.h ACSCGFPGATarget.h \
 ACSCGTarget.h ../../../../src/domains/acs/kernel/ACSTarget.h \
 ../../../../src/domains/cg/kernel/HLLTarget.h \
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
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/Scheduler.h ../../../../src/kernel/Galaxy.h \
 ../../../../src/kernel/GalIter.h ../../../../src/kernel/DFPortHole.h \
 ../../../../src/kernel/FloatState.h \
 ../../../../src/domains/acs/kernel/ACSKnownCategory.h \
 ../../../../src/domains/acs/kernel/ACSCore.h \
 ../../../../src/domains/acs/kernel/ACSStar.h \
 ../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../src/domains/acs/kernel/ACSPortHole.h \
 ../../../../src/domains/acs/kernel/ACSGeodesic.h \
 ../../../../src/domains/cg/kernel/CGGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/domains/acs/kernel/ACSCorona.h acs_starconsts.h \
 ACSIntArray.h DoubleArray.h StringArray.h CoreList.h \
 ../../../../src/domains/acs/kernel/ACSCGFPGACore.h \
 ../../../../src/domains/acs/targets/StringArray.h \
 ../../../../src/domains/acs/targets/CoreList.h \
 ../../../../src/domains/acs/targets/Pipealign.h \
 ../../../../src/domains/acs/targets/ACSIntArray.h \
 ../../../../src/domains/acs/targets/acs_starconsts.h \
 ../../../../src/domains/acs/targets/Connection_List.h \
 ../../../../src/domains/acs/targets/Pin.h \
 ../../../../src/domains/acs/targets/Connectivity.h \
 ../../../../src/domains/acs/targets/Delay.h \
 ../../../../src/domains/acs/targets/Capability.h \
 ../../../../src/domains/acs/targets/Constants.h \
 ../../../../src/domains/acs/targets/Resource.h \
 ../../../../src/domains/acs/targets/Sg_Constructs.h \
 ../../../../src/kernel/KnownBlock.h ../../../../src/kernel/Linker.h \
 ../../../../src/domains/acs/targets/Fpga.h \
 ../../../../src/domains/acs/targets/Sequencer.h \
 ../../../../src/domains/acs/targets/Port_Timing.h \
 ../../../../src/domains/acs/targets/acs_vhdl_lang.h \
 ../../../../src/domains/acs/kernel/ACSCGCore.h \
 ../../../../src/domains/acs/kernel/ACSWormhole.h \
 ../../../../src/kernel/Wormhole.h ../../../../src/kernel/Universe.h \
 ../../../../src/kernel/EventHorizon.h Connection_List.h Arch.h Fpga.h \
 Port.h Sg_Constructs.h Port_Timing.h Address.h Pin.h utilities.h \
 acs_vhdl_lang.h BindingRule.h Directory.h
HWTarget.o: HWTarget.cc HWTarget.h ACSCGFPGATarget.h ACSCGTarget.h \
 ../../../../src/domains/acs/kernel/ACSTarget.h \
 ../../../../src/domains/cg/kernel/HLLTarget.h \
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
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/Scheduler.h ../../../../src/kernel/Galaxy.h \
 ../../../../src/kernel/GalIter.h ../../../../src/kernel/DFPortHole.h \
 ../../../../src/kernel/FloatState.h \
 ../../../../src/domains/acs/kernel/ACSKnownCategory.h \
 ../../../../src/domains/acs/kernel/ACSCore.h \
 ../../../../src/domains/acs/kernel/ACSStar.h \
 ../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../src/domains/acs/kernel/ACSPortHole.h \
 ../../../../src/domains/acs/kernel/ACSGeodesic.h \
 ../../../../src/domains/cg/kernel/CGGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/domains/acs/kernel/ACSCorona.h acs_starconsts.h \
 ACSIntArray.h DoubleArray.h StringArray.h CoreList.h \
 ../../../../src/domains/acs/kernel/ACSCGFPGACore.h \
 ../../../../src/domains/acs/targets/StringArray.h \
 ../../../../src/domains/acs/targets/CoreList.h \
 ../../../../src/domains/acs/targets/Pipealign.h \
 ../../../../src/domains/acs/targets/ACSIntArray.h \
 ../../../../src/domains/acs/targets/acs_starconsts.h \
 ../../../../src/domains/acs/targets/Connection_List.h \
 ../../../../src/domains/acs/targets/Pin.h \
 ../../../../src/domains/acs/targets/Connectivity.h \
 ../../../../src/domains/acs/targets/Delay.h \
 ../../../../src/domains/acs/targets/Capability.h \
 ../../../../src/domains/acs/targets/Constants.h \
 ../../../../src/domains/acs/targets/Resource.h \
 ../../../../src/domains/acs/targets/Sg_Constructs.h \
 ../../../../src/kernel/KnownBlock.h ../../../../src/kernel/Linker.h \
 ../../../../src/domains/acs/targets/Fpga.h \
 ../../../../src/domains/acs/targets/Sequencer.h \
 ../../../../src/domains/acs/targets/Port_Timing.h \
 ../../../../src/domains/acs/targets/acs_vhdl_lang.h \
 ../../../../src/domains/acs/kernel/ACSCGCore.h \
 ../../../../src/domains/acs/kernel/ACSWormhole.h \
 ../../../../src/kernel/Wormhole.h ../../../../src/kernel/Universe.h \
 ../../../../src/kernel/EventHorizon.h Connection_List.h Arch.h Fpga.h \
 Port.h Sg_Constructs.h Port_Timing.h Address.h Pin.h utilities.h \
 acs_vhdl_lang.h BindingRule.h Directory.h
ACSCGFPGATarget.o: ACSCGFPGATarget.cc ACSCGFPGATarget.h ACSCGTarget.h \
 ../../../../src/domains/acs/kernel/ACSTarget.h \
 ../../../../src/domains/cg/kernel/HLLTarget.h \
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
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/Scheduler.h ../../../../src/kernel/Galaxy.h \
 ../../../../src/kernel/GalIter.h ../../../../src/kernel/DFPortHole.h \
 ../../../../src/kernel/FloatState.h \
 ../../../../src/domains/acs/kernel/ACSKnownCategory.h \
 ../../../../src/domains/acs/kernel/ACSCore.h \
 ../../../../src/domains/acs/kernel/ACSStar.h \
 ../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../src/domains/acs/kernel/ACSPortHole.h \
 ../../../../src/domains/acs/kernel/ACSGeodesic.h \
 ../../../../src/domains/cg/kernel/CGGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/domains/acs/kernel/ACSCorona.h acs_starconsts.h \
 ACSIntArray.h DoubleArray.h StringArray.h CoreList.h \
 ../../../../src/domains/acs/kernel/ACSCGFPGACore.h \
 ../../../../src/domains/acs/targets/StringArray.h \
 ../../../../src/domains/acs/targets/CoreList.h \
 ../../../../src/domains/acs/targets/Pipealign.h \
 ../../../../src/domains/acs/targets/ACSIntArray.h \
 ../../../../src/domains/acs/targets/acs_starconsts.h \
 ../../../../src/domains/acs/targets/Connection_List.h \
 ../../../../src/domains/acs/targets/Pin.h \
 ../../../../src/domains/acs/targets/Connectivity.h \
 ../../../../src/domains/acs/targets/Delay.h \
 ../../../../src/domains/acs/targets/Capability.h \
 ../../../../src/domains/acs/targets/Constants.h \
 ../../../../src/domains/acs/targets/Resource.h \
 ../../../../src/domains/acs/targets/Sg_Constructs.h \
 ../../../../src/kernel/KnownBlock.h ../../../../src/kernel/Linker.h \
 ../../../../src/domains/acs/targets/Fpga.h \
 ../../../../src/domains/acs/targets/Sequencer.h \
 ../../../../src/domains/acs/targets/Port_Timing.h \
 ../../../../src/domains/acs/targets/acs_vhdl_lang.h \
 ../../../../src/domains/acs/kernel/ACSCGCore.h \
 ../../../../src/domains/acs/kernel/ACSWormhole.h \
 ../../../../src/kernel/Wormhole.h ../../../../src/kernel/Universe.h \
 ../../../../src/kernel/EventHorizon.h Connection_List.h Arch.h Fpga.h \
 Port.h Sg_Constructs.h Port_Timing.h Address.h Pin.h utilities.h \
 acs_vhdl_lang.h BindingRule.h Directory.h \
 ../../../../src/domains/cg/kernel/CGUtilities.h \
 ../../../../src/compat/ptolemy/compat.h \
 ../../../../src/kernel/InfString.h \
 ../../../../src/kernel/KnownTarget.h \
 ../../../../src/kernel/pt_fstream.h
ACSCGTarget.o: ACSCGTarget.cc ACSCGTarget.h \
 ../../../../src/domains/acs/kernel/ACSTarget.h \
 ../../../../src/domains/cg/kernel/HLLTarget.h \
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
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/Scheduler.h ../../../../src/kernel/Galaxy.h \
 ../../../../src/kernel/GalIter.h ../../../../src/kernel/DFPortHole.h \
 ../../../../src/kernel/FloatState.h \
 ../../../../src/domains/acs/kernel/ACSKnownCategory.h \
 ../../../../src/domains/acs/kernel/ACSCore.h \
 ../../../../src/domains/acs/kernel/ACSStar.h \
 ../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../src/domains/acs/kernel/ACSPortHole.h \
 ../../../../src/domains/acs/kernel/ACSGeodesic.h \
 ../../../../src/domains/cg/kernel/CGGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/domains/acs/kernel/ACSCorona.h \
 ../../../../src/kernel/KnownTarget.h \
 ../../../../src/domains/cg/kernel/CGUtilities.h \
 ../../../../src/kernel/pt_fstream.h
ACSCGCTarget.o: ACSCGCTarget.cc ACSCGCTarget.h ACSCGTarget.h \
 ../../../../src/domains/acs/kernel/ACSTarget.h \
 ../../../../src/domains/cg/kernel/HLLTarget.h \
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
 ../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../src/kernel/Scheduler.h ../../../../src/kernel/Galaxy.h \
 ../../../../src/kernel/GalIter.h ../../../../src/kernel/DFPortHole.h \
 ../../../../src/kernel/FloatState.h \
 ../../../../src/domains/acs/kernel/ACSKnownCategory.h \
 ../../../../src/domains/acs/kernel/ACSCore.h \
 ../../../../src/domains/acs/kernel/ACSStar.h \
 ../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../src/kernel/DynDFStar.h \
 ../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../src/kernel/DynDFPortHole.h \
 ../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../src/domains/acs/kernel/ACSPortHole.h \
 ../../../../src/domains/acs/kernel/ACSGeodesic.h \
 ../../../../src/domains/cg/kernel/CGGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/domains/acs/kernel/ACSCorona.h \
 ../../../../src/kernel/StringArrayState.h \
 ../../../../src/kernel/HashTable.h \
 ../../../../src/domains/cg/kernel/CGUtilities.h \
 ../../../../src/kernel/EventHorizon.h \
 ../../../../src/compat/ptolemy/compat.h \
 ../../../../src/kernel/InfString.h \
 ../../../../src/kernel/KnownTarget.h
