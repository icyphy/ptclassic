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
# Makefile for Ptolemy CGC stars
# @(#)make.template	1.30     02/17/99
#
# If this file's name is "make.template" and there is no makefile in
# this directory, do
#	cp make.template makefile
#	chmod +w makefile
#	make depend
# This will add the dependencies to the makefile.

ROOT = ../../../../..
VPATH = $(ROOT)/src/domains/cgc/tcltk/stars

# get configuration info
CONFIG=$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

# CGDIR is the directory containing CG domain support
CGDIR = $(ROOT)/src/domains/cg/kernel
CGCDIR = $(ROOT)/src/domains/cgc/kernel
CGCSTARDIR = $(ROOT)/src/domains/cgc/stars
CGCDSPDIR = $(ROOT)/src/domains/cgc/dsp/stars
SDFDIR = $(ROOT)/src/domains/sdf/kernel

INCL = -I$(CGCDSPDIR) -I$(CGCSTARDIR) -I$(CGCDIR) -I$(CGDIR) -I$(SDFDIR) \
 -I$(KERNDIR)

# source files for all stars, using preprocessor, listing base stars first
PL_SRCS= \
 CGCTclScript.pl \
 CGCTkXYPlot.pl \
 CGCLMSTkPlot.pl \
 CGCTkBarGraph.pl \
 CGCTkCheckButton.pl \
 CGCTkEntry.pl \
 CGCTkImpulse.pl \
 CGCTkMonoIn.pl \
 CGCTkMonoOut.pl \
 CGCTkParametricEq.pl \
 CGCTkRadioButton.pl \
 CGCTkPlot.pl \
 CGCTkSlider.pl \
 CGCTkStereoIn.pl \
 CGCTkStereoOut.pl

TCL_SRCS= \
 tkBall.tcl \
 tkKeyboard.tcl \
 tkScript.tcl \
 tkBallAsync.tcl \
 tkKeypad.tcl \
 tkUniverse.tcl \
 tkRadioButton.tcl \
 tkCheckButton.tcl

# The .cc files
SRCS=$(PL_SRCS:.pl=.cc)

# object files for all stars
OBJS= $(SRCS:.cc=.o)

# header files for all stars
HDRS= $(SRCS:.cc=.h)

# extra source files: tcl/tk script files
SCRIPTS= tkBall.tcl tkBallAsync.tcl tkKeyboard.tcl tkKeypad.tcl \
 tkScript.tcl tkUniverse.tcl

DOMAIN=CGC
STAR_MK=cgctcltkstars
EXTRA_SRCS= $(STAR_MK).c $(PL_SRCS) $(SCRIPTS) $(TCL_SRCS)
PTLIB=libcgctcltk.$(LIBSUFFIX)

all:		makefile stars_all

install:	stars_install

# include common definitions, rules
include $(ROOT)/mk/common.mk

# Don't add anything after the next line; makedepend will zap it.
# DO NOT DELETE THIS LINE -- make depend depends on it.

CGCTclScript.o: CGCTclScript.cc CGCTclScript.h \
 ../../../../../src/domains/cgc/kernel/CGCStar.h \
 ../../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../../src/kernel/DynDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../../src/kernel/DynDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
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
 ../../../../../src/domains/cgc/kernel/CGCPortHole.h \
 ../../../../../src/kernel/PrecisionState.h
CGCTkXYPlot.o: CGCTkXYPlot.cc CGCTkXYPlot.h \
 ../../../../../src/domains/cgc/kernel/CGCStar.h \
 ../../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../../src/kernel/DynDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../../src/kernel/DynDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
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
 ../../../../../src/domains/cgc/kernel/CGCPortHole.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h
CGCLMSTkPlot.o: CGCLMSTkPlot.cc CGCLMSTkPlot.h \
 ../../../../../src/domains/cgc/dsp/stars/CGCLMS.h \
 ../../../../../src/domains/cgc/dsp/stars/CGCFIR.h \
 ../../../../../src/domains/cgc/kernel/CGCStar.h \
 ../../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../../src/kernel/DynDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../../src/kernel/DynDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
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
 ../../../../../src/domains/cgc/kernel/CGCPortHole.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/FloatState.h
CGCTkBarGraph.o: CGCTkBarGraph.cc CGCTkBarGraph.h \
 ../../../../../src/domains/cgc/kernel/CGCStar.h \
 ../../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../../src/kernel/DynDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../../src/kernel/DynDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
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
 ../../../../../src/domains/cgc/kernel/CGCPortHole.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatState.h
CGCTkCheckButton.o: CGCTkCheckButton.cc CGCTkCheckButton.h \
 CGCTclScript.h ../../../../../src/domains/cgc/kernel/CGCStar.h \
 ../../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../../src/kernel/DynDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../../src/kernel/DynDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
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
 ../../../../../src/domains/cgc/kernel/CGCPortHole.h \
 ../../../../../src/kernel/PrecisionState.h
CGCTkEntry.o: CGCTkEntry.cc CGCTkEntry.h \
 ../../../../../src/domains/cgc/kernel/CGCStar.h \
 ../../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../../src/kernel/DynDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../../src/kernel/DynDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
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
 ../../../../../src/domains/cgc/kernel/CGCPortHole.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatState.h
CGCTkImpulse.o: CGCTkImpulse.cc CGCTkImpulse.h \
 ../../../../../src/domains/cgc/kernel/CGCStar.h \
 ../../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../../src/kernel/DynDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../../src/kernel/DynDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
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
 ../../../../../src/domains/cgc/kernel/CGCPortHole.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatState.h
CGCTkMonoIn.o: CGCTkMonoIn.cc CGCTkMonoIn.h \
 ../../../../../src/domains/cgc/stars/CGCMonoIn.h \
 ../../../../../src/domains/cgc/stars/CGCAudioBase.h \
 ../../../../../src/domains/cgc/kernel/CGCStar.h \
 ../../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../../src/kernel/DynDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../../src/kernel/DynDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
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
 ../../../../../src/domains/cgc/kernel/CGCPortHole.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatState.h
CGCTkMonoOut.o: CGCTkMonoOut.cc CGCTkMonoOut.h \
 ../../../../../src/domains/cgc/stars/CGCMonoOut.h \
 ../../../../../src/domains/cgc/stars/CGCAudioBase.h \
 ../../../../../src/domains/cgc/kernel/CGCStar.h \
 ../../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../../src/kernel/DynDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../../src/kernel/DynDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
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
 ../../../../../src/domains/cgc/kernel/CGCPortHole.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatState.h
CGCTkParametricEq.o: CGCTkParametricEq.cc CGCTkParametricEq.h \
 ../../../../../src/domains/cgc/dsp/stars/CGCParametricEq.h \
 ../../../../../src/domains/cgc/dsp/stars/CGCBiquad.h \
 ../../../../../src/domains/cgc/kernel/CGCStar.h \
 ../../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../../src/kernel/DynDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../../src/kernel/DynDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
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
 ../../../../../src/domains/cgc/kernel/CGCPortHole.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatState.h
CGCTkRadioButton.o: CGCTkRadioButton.cc CGCTkRadioButton.h \
 CGCTclScript.h ../../../../../src/domains/cgc/kernel/CGCStar.h \
 ../../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../../src/kernel/DynDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../../src/kernel/DynDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
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
 ../../../../../src/domains/cgc/kernel/CGCPortHole.h \
 ../../../../../src/kernel/PrecisionState.h
CGCTkPlot.o: CGCTkPlot.cc CGCTkPlot.h CGCTkXYPlot.h \
 ../../../../../src/domains/cgc/kernel/CGCStar.h \
 ../../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../../src/kernel/DynDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../../src/kernel/DynDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
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
 ../../../../../src/domains/cgc/kernel/CGCPortHole.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h
CGCTkSlider.o: CGCTkSlider.cc CGCTkSlider.h \
 ../../../../../src/domains/cgc/kernel/CGCStar.h \
 ../../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../../src/kernel/DynDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../../src/kernel/DynDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
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
 ../../../../../src/domains/cgc/kernel/CGCPortHole.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatState.h
CGCTkStereoIn.o: CGCTkStereoIn.cc CGCTkStereoIn.h \
 ../../../../../src/domains/cgc/stars/CGCStereoIn.h \
 ../../../../../src/domains/cgc/stars/CGCAudioBase.h \
 ../../../../../src/domains/cgc/kernel/CGCStar.h \
 ../../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../../src/kernel/DynDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../../src/kernel/DynDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
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
 ../../../../../src/domains/cgc/kernel/CGCPortHole.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatState.h
CGCTkStereoOut.o: CGCTkStereoOut.cc CGCTkStereoOut.h \
 ../../../../../src/domains/cgc/stars/CGCStereoOut.h \
 ../../../../../src/domains/cgc/stars/CGCAudioBase.h \
 ../../../../../src/domains/cgc/kernel/CGCStar.h \
 ../../../../../src/domains/cg/kernel/CGStar.h \
 ../../../../../src/kernel/DynDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/cg/kernel/CGPortHole.h \
 ../../../../../src/kernel/DynDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
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
 ../../../../../src/domains/cgc/kernel/CGCPortHole.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatState.h
