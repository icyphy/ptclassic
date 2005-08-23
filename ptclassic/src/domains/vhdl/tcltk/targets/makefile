# Version identification:
# @(#)make.template	1.2 02/17/99
# 
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
# 
# VHDL code generation targets that use Tcl/Tk

# Root of Ptolemy directory
ROOT=../../../../..
# True source directory
VPATH = $(ROOT)/src/domains/vhdl/tcltk/targets

# get configuration info
CONFIG=$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

VHDLKERNEL = $(ROOT)/src/domains/vhdl/kernel
VHDLSTARS = $(ROOT)/src/domains/vhdl/stars
VHDLTARDIR = $(ROOT)/src/domains/vhdl/targets
SDFKERNEL = $(ROOT)/src/domains/sdf/kernel
CGKERNEL = $(ROOT)/src/domains/cg/kernel
CGCKERNEL = $(ROOT)/src/domains/cgc/kernel
VHDLSIM = $(ROOT)/src/utils/ptvhdlsim
SYNCOMPAT = $(ROOT)/src/compat/synopsys

INCL = -I$(VHDLKERNEL) -I$(VHDLSTARS) -I$(VHDLTARDIR) -I$(SDFKERNEL) \
 -I$(KERNDIR) -I$(CGKERNEL) -I$(CGCKERNEL) -I$(VHDLSIM) -I$(SYNCOMPAT) \
 -I$(TCL_INCDIR) \
 $(X11_INCSPEC)

PLSRC = $(PL_SRCS:.pl=.cc)

EXTRA_SRCS = TkSched.tcl

SRCS =	TkSchedTarget.cc \
	$(PLSRC) 

HDRS =	TkSchedTarget.h \
	$(PLSRC:.cc=.h)

OBJS = $(SRCS:.cc=.o)

PTLIB = libvhdltcltktargets.$(LIBSUFFIX)

all:	makefile $(PTLIB)

# "make install" puts things in the official place
install:	all $(LIBDIR)/$(PTLIB)
		@echo Installation complete.

# include common definitions, rules
include $(ROOT)/mk/common.mk

# Don't add anything after the next line; makedepend will zap it.
# DO NOT DELETE THIS LINE -- make depend depends on it.

TkSchedTarget.o: TkSchedTarget.cc TkSchedTarget.h \
 ../../../../../src/domains/vhdl/targets/SynthArchTarget.h \
 ../../../../../src/domains/vhdl/targets/ArchTarget.h \
 ../../../../../src/domains/vhdl/targets/SimVSSTarget.h \
 ../../../../../src/domains/vhdl/kernel/VHDLTarget.h \
 ../../../../../src/domains/cg/kernel/HLLTarget.h \
 ../../../../../src/domains/cg/kernel/CGTarget.h \
 ../../../../../src/kernel/Target.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/domains/cg/kernel/SymbolList.h \
 ../../../../../src/domains/cg/kernel/NamedList.h \
 ../../../../../src/kernel/SimControl.h \
 ../../../../../src/domains/cg/kernel/CodeStreamList.h \
 ../../../../../src/domains/cg/kernel/CodeStream.h \
 ../../../../../src/domains/cg/kernel/UniqueStringList.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/cg/kernel/ImplementationCost.h \
 ../../../../../src/domains/cg/kernel/ConversionTable.h \
 ../../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../../src/kernel/Scheduler.h \
 ../../../../../src/kernel/Galaxy.h \
 ../../../../../src/kernel/GalIter.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/domains/vhdl/kernel/VHDLStar.h \
 ../../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../../src/kernel/DynDFStar.h \
 ../../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../../src/kernel/DynDFPortHole.h \
 ../../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../../src/domains/vhdl/kernel/VHDLPortHole.h \
 ../../../../../src/domains/vhdl/kernel/VHDLGeodesic.h \
 ../../../../../src/domains/cg/kernel/CGGeodesic.h \
 ../../../../../src/kernel/Geodesic.h \
 ../../../../../src/kernel/ParticleStack.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/Tokenizer.h \
 ../../../../../src/domains/vhdl/kernel/VHDLFiring.h \
 ../../../../../src/domains/vhdl/kernel/VHDLObj.h \
 ../../../../../src/kernel/HashTable.h \
 ../../../../../src/domains/vhdl/kernel/VHDLGeneric.h \
 ../../../../../src/domains/vhdl/kernel/VHDLTypedObj.h \
 ../../../../../src/domains/vhdl/kernel/VHDLPort.h \
 ../../../../../src/domains/vhdl/kernel/VHDLSignal.h \
 ../../../../../src/domains/vhdl/kernel/VHDLVariable.h \
 ../../../../../src/domains/vhdl/kernel/VHDLArc.h \
 ../../../../../src/domains/vhdl/kernel/VHDLCompDecl.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/domains/vhdl/targets/VHDLCSend.h \
 ../../../../../src/domains/vhdl/targets/VHDLCSynchComm.h \
 ../../../../../src/domains/vhdl/targets/VHDLCReceive.h \
 ../../../../../src/domains/vhdl/targets/CGCVSend.h \
 ../../../../../src/domains/vhdl/targets/CGCVSynchComm.h \
 ../../../../../src/domains/cgc/kernel/CGCStar.h \
 ../../../../../src/domains/cgc/kernel/CGCPortHole.h \
 ../../../../../src/domains/vhdl/targets/CGCVReceive.h \
 ../../../../../src/domains/vhdl/kernel/VHDLState.h \
 ../../../../../src/domains/vhdl/kernel/VHDLFiregroup.h \
 ../../../../../src/domains/vhdl/kernel/VHDLDependency.h \
 ../../../../../src/domains/vhdl/kernel/VHDLToken.h \
 ../../../../../src/domains/vhdl/kernel/VHDLMux.h \
 ../../../../../src/domains/vhdl/kernel/VHDLReg.h \
 ../../../../../src/domains/vhdl/kernel/VHDLCluster.h \
 ../../../../../src/kernel/KnownTarget.h
