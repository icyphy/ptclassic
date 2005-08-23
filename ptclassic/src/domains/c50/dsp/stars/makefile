# @Copyright (c) 1990-1996 The Regents of the University of California.
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
# Makefile for Ptolemy C50 DSP stars
# @(#)make.template	1.12	05/23/98
#
# If this file's name is "make.template" and there is no makefile in
# this directory, do
#	cp make.template makefile
#	chmod +w makefile
#	make depend
# This will add the dependencies to the makefile.

ROOT = ../../../../..
VPATH = $(ROOT)/src/domains/c50/dsp/stars

# get configuration info
CONFIG=$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

C50DIR = $(ROOT)/src/domains/c50/kernel
C50SDIR = $(ROOT)/src/domains/c50/stars
C50DSP = $(ROOT)/src/domains/c50/dsp/stars
# CGDIR is the directory containing CG domain support
CGDIR = $(ROOT)/src/domains/cg/kernel

INCL = -I$(C50DIR) -I$(CGDIR) -I$(KERNDIR) -I$(C50SDIR) \
 -I$(ROOT)/src/utils/libptdsp

# Where star doc files go
STARDOCDIR = ../../doc/stars

# source files for all stars, using preprocessor
# list base stars first
PL_SRCS= \
 C50FIR.pl \
 C50Allpass.pl \
 C50Biquad.pl \
 C50BiquadDSPlay.pl \
 C50DTMFPostTest.pl \
 C50Comb.pl \
 C50Goertzel.pl \
 C50GoertzelBase.pl \
 C50GoertzelPower.pl \
 C50FFTCx.pl \
 C50IIR.pl \
 C50LMS.pl \
 C50LMSGanged.pl \
 C50LMSOscDet.pl \
 C50NdftVarD.pl \
 C50RaisedCosine.pl \
 C50Sinc.pl \
 C50Window.pl

# The .cc files
SRCS=$(PL_SRCS:.pl=.cc)

# object files for all stars
OBJS= $(SRCS:.cc=.o)

# header files for all stars
HDRS= $(SRCS:.cc=.h)

STAR_MK=c50dspstars
DOMAIN=C50DSP
DOMAIN_2=C50
EXTRA_SRCS= $(STAR_MK).c $(PL_SRCS)
PTLIB=libc50dspstars.$(LIBSUFFIX)

all:		makefile stars_all

# nothing to install -- use it where it lies
install:	stars_install

# include common definitions, rules
include $(ROOT)/mk/common.mk

# Don't add anything after the next line; makedepend will zap it.
# DO NOT DELETE THIS LINE -- make depend depends on it.

C50FIR.o: C50FIR.cc C50FIR.h \
 ../../../../../src/domains/c50/kernel/C50Star.h \
 ../../../../../src/domains/cg/kernel/AsmStar.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/domains/cg/kernel/AsmPortHole.h \
 ../../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../../src/kernel/DynDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../../src/kernel/DynDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/cg/kernel/CGTarget.h \
 ../../../../../src/kernel/Target.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/domains/cg/kernel/SymbolList.h \
 ../../../../../src/domains/cg/kernel/NamedList.h \
 ../../../../../src/kernel/SimControl.h \
 ../../../../../src/domains/cg/kernel/CodeStreamList.h \
 ../../../../../src/domains/cg/kernel/CodeStream.h \
 ../../../../../src/domains/cg/kernel/UniqueStringList.h \
 ../../../../../src/domains/cg/kernel/ImplementationCost.h \
 ../../../../../src/domains/cg/kernel/ConversionTable.h \
 ../../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../../src/domains/c50/kernel/C50PortHole.h \
 ../../../../../src/domains/c50/kernel/TIPortHole.h \
 ../../../../../src/domains/c50/kernel/TIAttributes.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h
C50Allpass.o: C50Allpass.cc C50Allpass.h \
 ../../../../../src/domains/c50/kernel/C50Star.h \
 ../../../../../src/domains/cg/kernel/AsmStar.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/domains/cg/kernel/AsmPortHole.h \
 ../../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../../src/kernel/DynDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../../src/kernel/DynDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/cg/kernel/CGTarget.h \
 ../../../../../src/kernel/Target.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/domains/cg/kernel/SymbolList.h \
 ../../../../../src/domains/cg/kernel/NamedList.h \
 ../../../../../src/kernel/SimControl.h \
 ../../../../../src/domains/cg/kernel/CodeStreamList.h \
 ../../../../../src/domains/cg/kernel/CodeStream.h \
 ../../../../../src/domains/cg/kernel/UniqueStringList.h \
 ../../../../../src/domains/cg/kernel/ImplementationCost.h \
 ../../../../../src/domains/cg/kernel/ConversionTable.h \
 ../../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../../src/domains/c50/kernel/C50PortHole.h \
 ../../../../../src/domains/c50/kernel/TIPortHole.h \
 ../../../../../src/domains/c50/kernel/TIAttributes.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h
C50Biquad.o: C50Biquad.cc C50Biquad.h \
 ../../../../../src/domains/c50/kernel/C50Star.h \
 ../../../../../src/domains/cg/kernel/AsmStar.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/domains/cg/kernel/AsmPortHole.h \
 ../../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../../src/kernel/DynDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../../src/kernel/DynDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/cg/kernel/CGTarget.h \
 ../../../../../src/kernel/Target.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/domains/cg/kernel/SymbolList.h \
 ../../../../../src/domains/cg/kernel/NamedList.h \
 ../../../../../src/kernel/SimControl.h \
 ../../../../../src/domains/cg/kernel/CodeStreamList.h \
 ../../../../../src/domains/cg/kernel/CodeStream.h \
 ../../../../../src/domains/cg/kernel/UniqueStringList.h \
 ../../../../../src/domains/cg/kernel/ImplementationCost.h \
 ../../../../../src/domains/cg/kernel/ConversionTable.h \
 ../../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../../src/domains/c50/kernel/C50PortHole.h \
 ../../../../../src/domains/c50/kernel/TIPortHole.h \
 ../../../../../src/domains/c50/kernel/TIAttributes.h \
 ../../../../../src/kernel/FixState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FixArrayState.h
C50BiquadDSPlay.o: C50BiquadDSPlay.cc C50BiquadDSPlay.h \
 ../../../../../src/domains/c50/kernel/C50Star.h \
 ../../../../../src/domains/cg/kernel/AsmStar.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/domains/cg/kernel/AsmPortHole.h \
 ../../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../../src/kernel/DynDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../../src/kernel/DynDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/cg/kernel/CGTarget.h \
 ../../../../../src/kernel/Target.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/domains/cg/kernel/SymbolList.h \
 ../../../../../src/domains/cg/kernel/NamedList.h \
 ../../../../../src/kernel/SimControl.h \
 ../../../../../src/domains/cg/kernel/CodeStreamList.h \
 ../../../../../src/domains/cg/kernel/CodeStream.h \
 ../../../../../src/domains/cg/kernel/UniqueStringList.h \
 ../../../../../src/domains/cg/kernel/ImplementationCost.h \
 ../../../../../src/domains/cg/kernel/ConversionTable.h \
 ../../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../../src/domains/c50/kernel/C50PortHole.h \
 ../../../../../src/domains/c50/kernel/TIPortHole.h \
 ../../../../../src/domains/c50/kernel/TIAttributes.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h
C50DTMFPostTest.o: C50DTMFPostTest.cc C50DTMFPostTest.h \
 ../../../../../src/domains/c50/kernel/C50Star.h \
 ../../../../../src/domains/cg/kernel/AsmStar.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/domains/cg/kernel/AsmPortHole.h \
 ../../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../../src/kernel/DynDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../../src/kernel/DynDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/cg/kernel/CGTarget.h \
 ../../../../../src/kernel/Target.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/domains/cg/kernel/SymbolList.h \
 ../../../../../src/domains/cg/kernel/NamedList.h \
 ../../../../../src/kernel/SimControl.h \
 ../../../../../src/domains/cg/kernel/CodeStreamList.h \
 ../../../../../src/domains/cg/kernel/CodeStream.h \
 ../../../../../src/domains/cg/kernel/UniqueStringList.h \
 ../../../../../src/domains/cg/kernel/ImplementationCost.h \
 ../../../../../src/domains/cg/kernel/ConversionTable.h \
 ../../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../../src/domains/c50/kernel/C50PortHole.h \
 ../../../../../src/domains/c50/kernel/TIPortHole.h \
 ../../../../../src/domains/c50/kernel/TIAttributes.h
C50Comb.o: C50Comb.cc C50Comb.h \
 ../../../../../src/domains/c50/kernel/C50Star.h \
 ../../../../../src/domains/cg/kernel/AsmStar.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/domains/cg/kernel/AsmPortHole.h \
 ../../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../../src/kernel/DynDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../../src/kernel/DynDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/cg/kernel/CGTarget.h \
 ../../../../../src/kernel/Target.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/domains/cg/kernel/SymbolList.h \
 ../../../../../src/domains/cg/kernel/NamedList.h \
 ../../../../../src/kernel/SimControl.h \
 ../../../../../src/domains/cg/kernel/CodeStreamList.h \
 ../../../../../src/domains/cg/kernel/CodeStream.h \
 ../../../../../src/domains/cg/kernel/UniqueStringList.h \
 ../../../../../src/domains/cg/kernel/ImplementationCost.h \
 ../../../../../src/domains/cg/kernel/ConversionTable.h \
 ../../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../../src/domains/c50/kernel/C50PortHole.h \
 ../../../../../src/domains/c50/kernel/TIPortHole.h \
 ../../../../../src/domains/c50/kernel/TIAttributes.h \
 ../../../../../src/kernel/FixState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FixArrayState.h
C50Goertzel.o: C50Goertzel.cc C50Goertzel.h C50GoertzelBase.h \
 ../../../../../src/domains/c50/kernel/C50Star.h \
 ../../../../../src/domains/cg/kernel/AsmStar.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/domains/cg/kernel/AsmPortHole.h \
 ../../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../../src/kernel/DynDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../../src/kernel/DynDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/cg/kernel/CGTarget.h \
 ../../../../../src/kernel/Target.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/domains/cg/kernel/SymbolList.h \
 ../../../../../src/domains/cg/kernel/NamedList.h \
 ../../../../../src/kernel/SimControl.h \
 ../../../../../src/domains/cg/kernel/CodeStreamList.h \
 ../../../../../src/domains/cg/kernel/CodeStream.h \
 ../../../../../src/domains/cg/kernel/UniqueStringList.h \
 ../../../../../src/domains/cg/kernel/ImplementationCost.h \
 ../../../../../src/domains/cg/kernel/ConversionTable.h \
 ../../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../../src/domains/c50/kernel/C50PortHole.h \
 ../../../../../src/domains/c50/kernel/TIPortHole.h \
 ../../../../../src/domains/c50/kernel/TIAttributes.h \
 ../../../../../src/kernel/FixState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FixArrayState.h
C50GoertzelBase.o: C50GoertzelBase.cc C50GoertzelBase.h \
 ../../../../../src/domains/c50/kernel/C50Star.h \
 ../../../../../src/domains/cg/kernel/AsmStar.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/domains/cg/kernel/AsmPortHole.h \
 ../../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../../src/kernel/DynDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../../src/kernel/DynDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/cg/kernel/CGTarget.h \
 ../../../../../src/kernel/Target.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/domains/cg/kernel/SymbolList.h \
 ../../../../../src/domains/cg/kernel/NamedList.h \
 ../../../../../src/kernel/SimControl.h \
 ../../../../../src/domains/cg/kernel/CodeStreamList.h \
 ../../../../../src/domains/cg/kernel/CodeStream.h \
 ../../../../../src/domains/cg/kernel/UniqueStringList.h \
 ../../../../../src/domains/cg/kernel/ImplementationCost.h \
 ../../../../../src/domains/cg/kernel/ConversionTable.h \
 ../../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../../src/domains/c50/kernel/C50PortHole.h \
 ../../../../../src/domains/c50/kernel/TIPortHole.h \
 ../../../../../src/domains/c50/kernel/TIAttributes.h \
 ../../../../../src/kernel/FixState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FixArrayState.h
C50GoertzelPower.o: C50GoertzelPower.cc C50GoertzelPower.h \
 C50GoertzelBase.h ../../../../../src/domains/c50/kernel/C50Star.h \
 ../../../../../src/domains/cg/kernel/AsmStar.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/domains/cg/kernel/AsmPortHole.h \
 ../../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../../src/kernel/DynDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../../src/kernel/DynDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/cg/kernel/CGTarget.h \
 ../../../../../src/kernel/Target.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/domains/cg/kernel/SymbolList.h \
 ../../../../../src/domains/cg/kernel/NamedList.h \
 ../../../../../src/kernel/SimControl.h \
 ../../../../../src/domains/cg/kernel/CodeStreamList.h \
 ../../../../../src/domains/cg/kernel/CodeStream.h \
 ../../../../../src/domains/cg/kernel/UniqueStringList.h \
 ../../../../../src/domains/cg/kernel/ImplementationCost.h \
 ../../../../../src/domains/cg/kernel/ConversionTable.h \
 ../../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../../src/domains/c50/kernel/C50PortHole.h \
 ../../../../../src/domains/c50/kernel/TIPortHole.h \
 ../../../../../src/domains/c50/kernel/TIAttributes.h \
 ../../../../../src/kernel/FixState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FixArrayState.h
C50FFTCx.o: C50FFTCx.cc C50FFTCx.h \
 ../../../../../src/domains/c50/kernel/C50Star.h \
 ../../../../../src/domains/cg/kernel/AsmStar.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/domains/cg/kernel/AsmPortHole.h \
 ../../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../../src/kernel/DynDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../../src/kernel/DynDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/cg/kernel/CGTarget.h \
 ../../../../../src/kernel/Target.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/domains/cg/kernel/SymbolList.h \
 ../../../../../src/domains/cg/kernel/NamedList.h \
 ../../../../../src/kernel/SimControl.h \
 ../../../../../src/domains/cg/kernel/CodeStreamList.h \
 ../../../../../src/domains/cg/kernel/CodeStream.h \
 ../../../../../src/domains/cg/kernel/UniqueStringList.h \
 ../../../../../src/domains/cg/kernel/ImplementationCost.h \
 ../../../../../src/domains/cg/kernel/ConversionTable.h \
 ../../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../../src/domains/c50/kernel/C50PortHole.h \
 ../../../../../src/domains/c50/kernel/TIPortHole.h \
 ../../../../../src/domains/c50/kernel/TIAttributes.h \
 ../../../../../src/kernel/FloatState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h
C50IIR.o: C50IIR.cc C50IIR.h \
 ../../../../../src/domains/c50/kernel/C50Star.h \
 ../../../../../src/domains/cg/kernel/AsmStar.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/domains/cg/kernel/AsmPortHole.h \
 ../../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../../src/kernel/DynDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../../src/kernel/DynDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/cg/kernel/CGTarget.h \
 ../../../../../src/kernel/Target.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/domains/cg/kernel/SymbolList.h \
 ../../../../../src/domains/cg/kernel/NamedList.h \
 ../../../../../src/kernel/SimControl.h \
 ../../../../../src/domains/cg/kernel/CodeStreamList.h \
 ../../../../../src/domains/cg/kernel/CodeStream.h \
 ../../../../../src/domains/cg/kernel/UniqueStringList.h \
 ../../../../../src/domains/cg/kernel/ImplementationCost.h \
 ../../../../../src/domains/cg/kernel/ConversionTable.h \
 ../../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../../src/domains/c50/kernel/C50PortHole.h \
 ../../../../../src/domains/c50/kernel/TIPortHole.h \
 ../../../../../src/domains/c50/kernel/TIAttributes.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h
C50LMS.o: C50LMS.cc C50LMS.h \
 ../../../../../src/domains/c50/kernel/C50Star.h \
 ../../../../../src/domains/cg/kernel/AsmStar.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/domains/cg/kernel/AsmPortHole.h \
 ../../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../../src/kernel/DynDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../../src/kernel/DynDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/cg/kernel/CGTarget.h \
 ../../../../../src/kernel/Target.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/domains/cg/kernel/SymbolList.h \
 ../../../../../src/domains/cg/kernel/NamedList.h \
 ../../../../../src/kernel/SimControl.h \
 ../../../../../src/domains/cg/kernel/CodeStreamList.h \
 ../../../../../src/domains/cg/kernel/CodeStream.h \
 ../../../../../src/domains/cg/kernel/UniqueStringList.h \
 ../../../../../src/domains/cg/kernel/ImplementationCost.h \
 ../../../../../src/domains/cg/kernel/ConversionTable.h \
 ../../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../../src/domains/c50/kernel/C50PortHole.h \
 ../../../../../src/domains/c50/kernel/TIPortHole.h \
 ../../../../../src/domains/c50/kernel/TIAttributes.h \
 ../../../../../src/kernel/FixState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FixArrayState.h
C50LMSGanged.o: C50LMSGanged.cc C50LMSGanged.h \
 ../../../../../src/domains/c50/kernel/C50Star.h \
 ../../../../../src/domains/cg/kernel/AsmStar.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/domains/cg/kernel/AsmPortHole.h \
 ../../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../../src/kernel/DynDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../../src/kernel/DynDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/cg/kernel/CGTarget.h \
 ../../../../../src/kernel/Target.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/domains/cg/kernel/SymbolList.h \
 ../../../../../src/domains/cg/kernel/NamedList.h \
 ../../../../../src/kernel/SimControl.h \
 ../../../../../src/domains/cg/kernel/CodeStreamList.h \
 ../../../../../src/domains/cg/kernel/CodeStream.h \
 ../../../../../src/domains/cg/kernel/UniqueStringList.h \
 ../../../../../src/domains/cg/kernel/ImplementationCost.h \
 ../../../../../src/domains/cg/kernel/ConversionTable.h \
 ../../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../../src/domains/c50/kernel/C50PortHole.h \
 ../../../../../src/domains/c50/kernel/TIPortHole.h \
 ../../../../../src/domains/c50/kernel/TIAttributes.h \
 ../../../../../src/kernel/IntArrayState.h \
 ../../../../../src/kernel/FixState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FixArrayState.h
C50LMSOscDet.o: C50LMSOscDet.cc C50LMSOscDet.h \
 ../../../../../src/domains/c50/kernel/C50Star.h \
 ../../../../../src/domains/cg/kernel/AsmStar.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/domains/cg/kernel/AsmPortHole.h \
 ../../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../../src/kernel/DynDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../../src/kernel/DynDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/cg/kernel/CGTarget.h \
 ../../../../../src/kernel/Target.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/domains/cg/kernel/SymbolList.h \
 ../../../../../src/domains/cg/kernel/NamedList.h \
 ../../../../../src/kernel/SimControl.h \
 ../../../../../src/domains/cg/kernel/CodeStreamList.h \
 ../../../../../src/domains/cg/kernel/CodeStream.h \
 ../../../../../src/domains/cg/kernel/UniqueStringList.h \
 ../../../../../src/domains/cg/kernel/ImplementationCost.h \
 ../../../../../src/domains/cg/kernel/ConversionTable.h \
 ../../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../../src/domains/c50/kernel/C50PortHole.h \
 ../../../../../src/domains/c50/kernel/TIPortHole.h \
 ../../../../../src/domains/c50/kernel/TIAttributes.h \
 ../../../../../src/kernel/FloatState.h \
 ../../../../../src/kernel/FixState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FixArrayState.h
C50NdftVarD.o: C50NdftVarD.cc C50NdftVarD.h \
 ../../../../../src/domains/c50/kernel/C50Star.h \
 ../../../../../src/domains/cg/kernel/AsmStar.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/domains/cg/kernel/AsmPortHole.h \
 ../../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../../src/kernel/DynDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../../src/kernel/DynDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/cg/kernel/CGTarget.h \
 ../../../../../src/kernel/Target.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/domains/cg/kernel/SymbolList.h \
 ../../../../../src/domains/cg/kernel/NamedList.h \
 ../../../../../src/kernel/SimControl.h \
 ../../../../../src/domains/cg/kernel/CodeStreamList.h \
 ../../../../../src/domains/cg/kernel/CodeStream.h \
 ../../../../../src/domains/cg/kernel/UniqueStringList.h \
 ../../../../../src/domains/cg/kernel/ImplementationCost.h \
 ../../../../../src/domains/cg/kernel/ConversionTable.h \
 ../../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../../src/domains/c50/kernel/C50PortHole.h \
 ../../../../../src/domains/c50/kernel/TIPortHole.h \
 ../../../../../src/domains/c50/kernel/TIAttributes.h \
 ../../../../../src/kernel/FixState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FixArrayState.h
C50RaisedCosine.o: C50RaisedCosine.cc C50RaisedCosine.h C50FIR.h \
 ../../../../../src/domains/c50/kernel/C50Star.h \
 ../../../../../src/domains/cg/kernel/AsmStar.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/domains/cg/kernel/AsmPortHole.h \
 ../../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../../src/kernel/DynDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../../src/kernel/DynDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/cg/kernel/CGTarget.h \
 ../../../../../src/kernel/Target.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/domains/cg/kernel/SymbolList.h \
 ../../../../../src/domains/cg/kernel/NamedList.h \
 ../../../../../src/kernel/SimControl.h \
 ../../../../../src/domains/cg/kernel/CodeStreamList.h \
 ../../../../../src/domains/cg/kernel/CodeStream.h \
 ../../../../../src/domains/cg/kernel/UniqueStringList.h \
 ../../../../../src/domains/cg/kernel/ImplementationCost.h \
 ../../../../../src/domains/cg/kernel/ConversionTable.h \
 ../../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../../src/domains/c50/kernel/C50PortHole.h \
 ../../../../../src/domains/c50/kernel/TIPortHole.h \
 ../../../../../src/domains/c50/kernel/TIAttributes.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatState.h \
 ../../../../../src/utils/libptdsp/ptdspRaisedCosine.h
C50Sinc.o: C50Sinc.cc C50Sinc.h \
 ../../../../../src/domains/c50/kernel/C50Star.h \
 ../../../../../src/domains/cg/kernel/AsmStar.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/domains/cg/kernel/AsmPortHole.h \
 ../../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../../src/kernel/DynDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../../src/kernel/DynDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/cg/kernel/CGTarget.h \
 ../../../../../src/kernel/Target.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/domains/cg/kernel/SymbolList.h \
 ../../../../../src/domains/cg/kernel/NamedList.h \
 ../../../../../src/kernel/SimControl.h \
 ../../../../../src/domains/cg/kernel/CodeStreamList.h \
 ../../../../../src/domains/cg/kernel/CodeStream.h \
 ../../../../../src/domains/cg/kernel/UniqueStringList.h \
 ../../../../../src/domains/cg/kernel/ImplementationCost.h \
 ../../../../../src/domains/cg/kernel/ConversionTable.h \
 ../../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../../src/domains/c50/kernel/C50PortHole.h \
 ../../../../../src/domains/c50/kernel/TIPortHole.h \
 ../../../../../src/domains/c50/kernel/TIAttributes.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h
C50Window.o: C50Window.cc C50Window.h \
 ../../../../../src/domains/c50/stars/C50WaveForm.h \
 ../../../../../src/domains/c50/kernel/C50Star.h \
 ../../../../../src/domains/cg/kernel/AsmStar.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/domains/cg/kernel/AsmPortHole.h \
 ../../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../../src/kernel/DynDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../../src/kernel/DynDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/cg/kernel/CGTarget.h \
 ../../../../../src/kernel/Target.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/domains/cg/kernel/SymbolList.h \
 ../../../../../src/domains/cg/kernel/NamedList.h \
 ../../../../../src/kernel/SimControl.h \
 ../../../../../src/domains/cg/kernel/CodeStreamList.h \
 ../../../../../src/domains/cg/kernel/CodeStream.h \
 ../../../../../src/domains/cg/kernel/UniqueStringList.h \
 ../../../../../src/domains/cg/kernel/ImplementationCost.h \
 ../../../../../src/domains/cg/kernel/ConversionTable.h \
 ../../../../../src/domains/cg/kernel/CodeBlock.h \
 ../../../../../src/domains/c50/kernel/C50PortHole.h \
 ../../../../../src/domains/c50/kernel/TIPortHole.h \
 ../../../../../src/domains/c50/kernel/TIAttributes.h \
 ../../../../../src/kernel/FixState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/utils/libptdsp/ptdspWindow.h
