# @(#)make.template	1.10	08/20/97
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
# Code generation targets


# Root of Ptolemy directory
ROOT=../../../../..
# True source directory
VPATH = $(ROOT)/src/domains/cgc/tcltk/targets

# get configuration info
CONFIG=$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

SDFKERNEL = $(ROOT)/src/domains/sdf/kernel
CGKERNEL = $(ROOT)/src/domains/cg/kernel
CGCKERNEL = $(ROOT)/src/domains/cgc/kernel
CGCSTARS = $(ROOT)/src/domains/cgc/stars
EGKERNEL = $(ROOT)/src/domains/sdf/expgraph
PARDIR = $(ROOT)/src/domains/cg/parScheduler
CGTARDIR = $(ROOT)/src/domains/cg/targets
CGCTARGMAINDIR = $(ROOT)/src/domains/cgc/targets/uniproc
PTKLIB = $(ROOT)/src/ptklib
PTCLDIR = $(ROOT)/src/ptcl

INCL=-I$(CGCKERNEL) -I$(CGKERNEL) -I$(SDFKERNEL) -I$(KERNDIR) -I$(EGKERNEL) \
	-I$(PARDIR) -I$(CGTARDIR) -I$(CGCTARGMAINDIR) -I$(CGCSTARS) \
	-I$(PTKLIB) -I$(PTCLDIR) \
	-I$(TCL_INCDIR) -I$(TK_INCDIR) $(X11_INCSPEC)

PLSRC = $(PL_SRCS:.pl=.cc)

SRCS=	CGCTclTkTarget.cc \
	CGCTychoTarget.cc \
	$(PLSRC) 

HDRS=	CGCTclTkTarget.h \
	CGCTychoTarget.h \
	$(PLSRC:.cc=.h)

OBJS = $(SRCS:.cc=.o)

all:	makefile $(OBJS)

# "make install" puts things in the official place
install:	all
		@echo Installation complete.

LIB=none	# required by common.mk

# include common definitions, rules
include $(ROOT)/mk/common.mk

# Don't add anything after the next line; makedepend will zap it.
# DO NOT DELETE THIS LINE -- make depend depends on it.

CGCTclTkTarget.o: CGCTclTkTarget.cc CGCTclTkTarget.h \
 ../../../../../src/domains/cgc/targets/uniproc/CGCMakefileTarget.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/logNew.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/NamedObj.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/domains/cgc/kernel/CGCTarget.h \
 ../../../../../src/domains/cg/kernel/HLLTarget.h \
 ../../../../../src/domains/cg/kernel/CGTarget.h \
 ../../../../../src/kernel/Target.h \
 ../../../../../src/kernel/IntState.h \
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
 ../../../../../src/kernel/StringArrayState.h \
 ../../../../../src/kernel/HashTable.h \
 ../../../../../src/domains/cgc/kernel/CGCStar.h \
 ../../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../../src/kernel/DynDFStar.h \
 ../../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../../src/kernel/DynDFPortHole.h \
 ../../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../../src/domains/cgc/kernel/CGCPortHole.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/KnownTarget.h
CGCTychoTarget.o: CGCTychoTarget.cc CGCTychoTarget.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/NamedObj.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/domains/cgc/kernel/CGCTarget.h \
 ../../../../../src/domains/cg/kernel/HLLTarget.h \
 ../../../../../src/domains/cg/kernel/CGTarget.h \
 ../../../../../src/kernel/Target.h \
 ../../../../../src/kernel/IntState.h \
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
 ../../../../../src/kernel/StringArrayState.h \
 ../../../../../src/kernel/HashTable.h \
 ../../../../../src/domains/cgc/kernel/CGCStar.h \
 ../../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../../src/kernel/DynDFStar.h \
 ../../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../../src/kernel/DynDFPortHole.h \
 ../../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../../src/domains/cgc/kernel/CGCPortHole.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/KnownTarget.h \
 ../../../../../src/ptklib/ptk.h ../../../../../src/ptcl/PTcl.h \
 ../../../../../src/domains/cg/kernel/CGUtilities.h
