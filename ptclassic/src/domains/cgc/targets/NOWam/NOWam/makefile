# Makefile for CGC NOW (Networks Of Workstations)
# Version identification:
# @(#)make.template	1.5 1/31/96
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
# Date of creation: 1/30/96
#
# If this file's name is "make.template" and there is no makefile in
# this directory, do
#	cp make.template makefile
#	make depend
# This will add the dependencies to the makefile.

# Root of Ptolemy directory
ROOT =	../../../../../..
# True source directory
VPATH = $(ROOT)/src/domains/cgc/targets/NOWam/NOWam

# get configuration info
CONFIG =$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

SDFKERNEL = $(ROOT)/src/domains/sdf/kernel
BDFKERNEL = $(ROOT)/src/domains/bdf/kernel
CGKERNEL = $(ROOT)/src/domains/cg/kernel
CGCKERNEL = $(ROOT)/src/domains/cgc/kernel
CGCSTARS = $(ROOT)/src/domains/cgc/stars
PARDIR = $(ROOT)/src/domains/cg/parScheduler
CGTARDIR = $(ROOT)/src/domains/cg/targets

INCL=-I$(CGCKERNEL) -I$(CGKERNEL) -I$(BDFKERNEL) -I$(SDFKERNEL) -I$(KERNDIR) \
	-I$(PARDIR) -I$(CGTARDIR) -I$(CGCSTARS)

PL_SRCS = \
	CGCNOWamSend.pl \
	CGCNOWamRecv.pl

PLSRC = $(PL_SRCS:.pl=.cc)

SRCS = CGCNOWamTarget.cc $(PLSRC) 
EXTRA_SRCS = $(PL_SRCS)

HDRS=$(SRCS:.cc=.h)

OBJS = $(SRCS:.cc=.o)

# sol2 and sol2.cfront
ifneq (,$(filter sol%,$(PTARCH)))

ifeq ($(USE_SHARED_LIBS),yes) 

# If we are building shared libraries, then we can put all the targets in
# a shared library.
PTLIB = 		libNOWam.$(LIBSUFFIX)
all:		$(SRCS) makefile $(OBJS) $(PTLIB)
install:	all $(LIBDIR)/$(PTLIB)

else	#USE_SHARED_LIBS

PTLIB=none	# required by common.mk
all:		makefile $(OBJS)
# "make install" puts things in the official place
install:	all
		@echo Installation complete.
endif 	#USE_SHARED_LIBS


else	#sol2 or sol2.cfront


PTLIB=none	# required by common.mk
# Non solaris platforms
all: makefile
	@echo "NOW active message target supported only on Solaris platforms"
install: makefile
	@echo "NOW active message target supported only on Solaris platforms"

endif	#sol2 or sol2.cfront



# include common definitions, rules
include $(ROOT)/mk/common.mk

# Don't add anything after the next line; makedepend will zap it.
# DO NOT DELETE THIS LINE -- make depend depends on it.

CGCNOWamTarget.o: CGCNOWamTarget.cc \
 ../../../../../../src/kernel/pt_fstream.h \
 ../../../../../../src/kernel/StringList.h \
 ../../../../../../src/kernel/miscFuncs.h \
 ../../../../../../src/kernel/logNew.h \
 ../../../../../../src/kernel/Error.h \
 ../../../../../../src/kernel/DataStruct.h \
 ../../../../../../src/kernel/type.h \
 ../../../../../../src/kernel/Block.h \
 ../../../../../../src/kernel/NamedObj.h \
 ../../../../../../src/kernel/isa.h \
 ../../../../../../src/kernel/FlagArray.h \
 ../../../../../../src/kernel/PortHole.h \
 ../../../../../../src/kernel/dataType.h \
 ../../../../../../src/kernel/Particle.h \
 ../../../../../../src/kernel/Fix.h \
 ../../../../../../src/kernel/ComplexSubset.h \
 ../../../../../../src/kernel/Attribute.h \
 ../../../../../../src/kernel/State.h \
 ../../../../../../src/kernel/InvokeInterp.h \
 ../../../../../../src/kernel/Target.h \
 ../../../../../../src/kernel/KnownTarget.h \
 ../../../../../../src/domains/cg/kernel/CGUtilities.h \
 ../../../../../../src/domains/cgc/kernel/CGCStar.h \
 ../../../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../../../src/kernel/DynDFStar.h \
 ../../../../../../src/kernel/DataFlowStar.h \
 ../../../../../../src/kernel/Star.h \
 ../../../../../../src/kernel/Fraction.h \
 ../../../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../../../src/kernel/DynDFPortHole.h \
 ../../../../../../src/kernel/DFPortHole.h \
 ../../../../../../src/domains/cg/kernel/CGTarget.h \
 ../../../../../../src/kernel/IntState.h \
 ../../../../../../src/kernel/StringState.h \
 ../../../../../../src/kernel/KnownState.h \
 ../../../../../../src/domains/cg/kernel/SymbolList.h \
 ../../../../../../src/domains/cg/kernel/NamedList.h \
 ../../../../../../src/kernel/SimControl.h \
 ../../../../../../src/domains/cg/kernel/CodeStreamList.h \
 ../../../../../../src/domains/cg/kernel/CodeStream.h \
 ../../../../../../src/domains/cg/kernel/UniqueStringList.h \
 ../../../../../../src/domains/cg/kernel/ImplementationCost.h \
 ../../../../../../src/domains/cg/kernel/ConversionTable.h \
 ../../../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../../../src/domains/cgc/kernel/CGCPortHole.h \
 ../../../../../../src/kernel/PrecisionState.h CGCNOWamTarget.h \
 ../../../../../../src/domains/cg/targets/CGMultiTarget.h \
 ../../../../../../src/domains/cg/kernel/MultiTarget.h \
 ../../../../../../src/domains/cg/kernel/IntArray.h \
 ../../../../../../src/kernel/StringArrayState.h \
 ../../../../../../src/kernel/IntArrayState.h \
 ../../../../../../src/domains/cgc/kernel/CGCTarget.h \
 ../../../../../../src/domains/cg/kernel/HLLTarget.h \
 ../../../../../../src/domains/sdf/kernel/SDFScheduler.h \
 ../../../../../../src/kernel/Scheduler.h \
 ../../../../../../src/kernel/Galaxy.h \
 ../../../../../../src/kernel/GalIter.h \
 ../../../../../../src/kernel/HashTable.h \
 ../../../../../../src/domains/cgc/stars/CGCSpread.h \
 ../../../../../../src/domains/cgc/stars/CGCCollect.h CGCNOWamSend.h \
 CGCNOWamRecv.h
CGCNOWamSend.o: CGCNOWamSend.cc CGCNOWamSend.h \
 ../../../../../../src/domains/cgc/kernel/CGCStar.h \
 ../../../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../../../src/kernel/DynDFStar.h \
 ../../../../../../src/kernel/DataFlowStar.h \
 ../../../../../../src/kernel/Star.h \
 ../../../../../../src/kernel/Block.h \
 ../../../../../../src/kernel/NamedObj.h \
 ../../../../../../src/kernel/StringList.h \
 ../../../../../../src/kernel/miscFuncs.h \
 ../../../../../../src/kernel/logNew.h \
 ../../../../../../src/kernel/Error.h \
 ../../../../../../src/kernel/DataStruct.h \
 ../../../../../../src/kernel/type.h \
 ../../../../../../src/kernel/isa.h \
 ../../../../../../src/kernel/FlagArray.h \
 ../../../../../../src/kernel/PortHole.h \
 ../../../../../../src/kernel/dataType.h \
 ../../../../../../src/kernel/Particle.h \
 ../../../../../../src/kernel/Fix.h \
 ../../../../../../src/kernel/ComplexSubset.h \
 ../../../../../../src/kernel/Attribute.h \
 ../../../../../../src/kernel/State.h \
 ../../../../../../src/kernel/InvokeInterp.h \
 ../../../../../../src/kernel/Fraction.h \
 ../../../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../../../src/kernel/DynDFPortHole.h \
 ../../../../../../src/kernel/DFPortHole.h \
 ../../../../../../src/domains/cg/kernel/CGTarget.h \
 ../../../../../../src/kernel/Target.h \
 ../../../../../../src/kernel/IntState.h \
 ../../../../../../src/kernel/StringState.h \
 ../../../../../../src/kernel/KnownState.h \
 ../../../../../../src/domains/cg/kernel/SymbolList.h \
 ../../../../../../src/domains/cg/kernel/NamedList.h \
 ../../../../../../src/kernel/SimControl.h \
 ../../../../../../src/domains/cg/kernel/CodeStreamList.h \
 ../../../../../../src/domains/cg/kernel/CodeStream.h \
 ../../../../../../src/domains/cg/kernel/UniqueStringList.h \
 ../../../../../../src/domains/cg/kernel/ImplementationCost.h \
 ../../../../../../src/domains/cg/kernel/ConversionTable.h \
 ../../../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../../../src/domains/cgc/kernel/CGCPortHole.h \
 ../../../../../../src/kernel/PrecisionState.h CGCNOWamTarget.h \
 ../../../../../../src/domains/cg/targets/CGMultiTarget.h \
 ../../../../../../src/domains/cg/kernel/MultiTarget.h \
 ../../../../../../src/domains/cg/kernel/IntArray.h \
 ../../../../../../src/kernel/StringArrayState.h \
 ../../../../../../src/kernel/IntArrayState.h
CGCNOWamRecv.o: CGCNOWamRecv.cc CGCNOWamRecv.h \
 ../../../../../../src/domains/cgc/kernel/CGCStar.h \
 ../../../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../../../src/kernel/DynDFStar.h \
 ../../../../../../src/kernel/DataFlowStar.h \
 ../../../../../../src/kernel/Star.h \
 ../../../../../../src/kernel/Block.h \
 ../../../../../../src/kernel/NamedObj.h \
 ../../../../../../src/kernel/StringList.h \
 ../../../../../../src/kernel/miscFuncs.h \
 ../../../../../../src/kernel/logNew.h \
 ../../../../../../src/kernel/Error.h \
 ../../../../../../src/kernel/DataStruct.h \
 ../../../../../../src/kernel/type.h \
 ../../../../../../src/kernel/isa.h \
 ../../../../../../src/kernel/FlagArray.h \
 ../../../../../../src/kernel/PortHole.h \
 ../../../../../../src/kernel/dataType.h \
 ../../../../../../src/kernel/Particle.h \
 ../../../../../../src/kernel/Fix.h \
 ../../../../../../src/kernel/ComplexSubset.h \
 ../../../../../../src/kernel/Attribute.h \
 ../../../../../../src/kernel/State.h \
 ../../../../../../src/kernel/InvokeInterp.h \
 ../../../../../../src/kernel/Fraction.h \
 ../../../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../../../src/kernel/DynDFPortHole.h \
 ../../../../../../src/kernel/DFPortHole.h \
 ../../../../../../src/domains/cg/kernel/CGTarget.h \
 ../../../../../../src/kernel/Target.h \
 ../../../../../../src/kernel/IntState.h \
 ../../../../../../src/kernel/StringState.h \
 ../../../../../../src/kernel/KnownState.h \
 ../../../../../../src/domains/cg/kernel/SymbolList.h \
 ../../../../../../src/domains/cg/kernel/NamedList.h \
 ../../../../../../src/kernel/SimControl.h \
 ../../../../../../src/domains/cg/kernel/CodeStreamList.h \
 ../../../../../../src/domains/cg/kernel/CodeStream.h \
 ../../../../../../src/domains/cg/kernel/UniqueStringList.h \
 ../../../../../../src/domains/cg/kernel/ImplementationCost.h \
 ../../../../../../src/domains/cg/kernel/ConversionTable.h \
 ../../../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../../../src/domains/cgc/kernel/CGCPortHole.h \
 ../../../../../../src/kernel/PrecisionState.h \
 ../../../../../../src/kernel/IntArrayState.h
