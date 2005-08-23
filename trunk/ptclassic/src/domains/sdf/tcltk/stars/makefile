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
# Makefile for Ptolemy SDF tcl stars
# @(#)make.template	1.39	02/17/99
#
# If this file's name is "make.template" and there is no makefile in
# this directory, do
#	cp make.template makefile
#	chmod +w makefile
#	make depend
# This will add the dependencies to the makefile.

ROOT = ../../../../..
VPATH = $(ROOT)/src/domains/sdf/tcltk/stars

# get configuration info
CONFIG=$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

# SDFDIR is the directory containing SDF domain support
SDFDIR = $(ROOT)/src/domains/sdf/kernel
SDFDSPSTARS = $(ROOT)/src/domains/sdf/dsp/stars
TCLDIR = $(ROOT)/tcl/include
PTKLIB = $(ROOT)/src/ptklib
INCL = -I$(SDFDIR) -I$(KERNDIR) -I$(PTKLIB) \
 -I$(SDFDSPSTARS) -I$(TCL_INCDIR) -I$(TK_INCDIR) $(X11_INCSPEC)

# Library name
PTLIB=libsdftclstars.$(LIBSUFFIX)

# source files for all stars, using preprocessor, putting base stars first
# note that SDFTclScript must be made before stars that depend on it
PL_SRCS= \
 SDFTclScript.pl \
 SDFTkShowValues.pl \
 SDFLMSTkPlot.pl \
 SDFLMSCxTkPlot.pl \
 SDFTkBarGraph.pl \
 SDFTkBreakPt.pl \
 SDFTkButtons.pl \
 SDFTkMeter.pl \
 SDFTkPicture.pl \
 SDFTkPlot.pl \
 SDFTkShowBooleans.pl \
 SDFTkSlider.pl \
 SDFTkText.pl \
 SDFTkXYPlot.pl

# The .cc files
SRCS=$(PL_SRCS:.pl=.cc)

# object files for all stars
OBJS= $(SRCS:.cc=.o)

# header files for all stars
HDRS= $(SRCS:.cc=.h)

# extra source files: Tcl/Tk script files
SCRIPTS= \
 tkButtons.tcl \
 tkBreakPt.tcl \
 tkMeter.tcl \
 tkScript.tcl \
 tkShowBooleans.tcl \
 tkShowValues.tcl \
 tkSlider.tcl \
 tkText.tcl

STAR_MK=sdftclstars
DOMAIN=SDF_TCL
DOMAIN_2=SDF
EXTRA_SRCS=$(STAR_MK).c $(PL_SRCS) $(SCRIPTS)

all:		makefile stars_all

install:	stars_install


# include common definitions, rules
include $(ROOT)/mk/common.mk

# Workaround bug in gcc-2.7.2 optimizer
ifeq ($(USE_GPLUSPLUS),yes)
SDFTkImageDisplay.o: SDFTkImageDisplay.cc
	$(UNOPTIMIZED_WARNING_MESSAGE)
	$(UNOPTIMIZED_COMPILE_RULE) $<
endif


# Don't add anything after the next line; makedepend will zap it.
# DO NOT DELETE THIS LINE -- make depend depends on it.

SDFTclScript.o: SDFTclScript.cc SDFTclScript.h \
 ../../../../../src/ptklib/TclStarIfc.h \
 ../../../../../src/kernel/InfString.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h
SDFTkShowValues.o: SDFTkShowValues.cc SDFTkShowValues.h \
 ../../../../../src/ptklib/ptk.h SDFTclScript.h \
 ../../../../../src/ptklib/TclStarIfc.h \
 ../../../../../src/kernel/InfString.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/kernel/IntState.h
SDFLMSTkPlot.o: SDFLMSTkPlot.cc SDFLMSTkPlot.h \
 ../../../../../src/ptklib/BarGraph.h \
 ../../../../../src/kernel/Block.h \
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
 ../../../../../src/kernel/InfString.h ../../../../../src/ptklib/ptk.h \
 ../../../../../src/domains/sdf/dsp/stars/SDFLMS.h \
 ../../../../../src/domains/sdf/dsp/stars/SDFFIR.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/FloatState.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h
SDFLMSCxTkPlot.o: SDFLMSCxTkPlot.cc SDFLMSCxTkPlot.h \
 ../../../../../src/ptklib/BarGraph.h \
 ../../../../../src/kernel/Block.h \
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
 ../../../../../src/kernel/InfString.h ../../../../../src/ptklib/ptk.h \
 ../../../../../src/domains/sdf/dsp/stars/SDFLMSCx.h \
 ../../../../../src/domains/sdf/dsp/stars/SDFFIRCx.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FloatState.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h
SDFTkBarGraph.o: SDFTkBarGraph.cc SDFTkBarGraph.h \
 ../../../../../src/ptklib/ptk.h ../../../../../src/ptklib/BarGraph.h \
 ../../../../../src/kernel/Block.h \
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
 ../../../../../src/kernel/InfString.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/FloatState.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h
SDFTkBreakPt.o: SDFTkBreakPt.cc SDFTkBreakPt.h \
 ../../../../../src/ptklib/ptk.h SDFTclScript.h \
 ../../../../../src/ptklib/TclStarIfc.h \
 ../../../../../src/kernel/InfString.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h
SDFTkButtons.o: SDFTkButtons.cc SDFTkButtons.h \
 ../../../../../src/ptklib/ptk.h SDFTclScript.h \
 ../../../../../src/ptklib/TclStarIfc.h \
 ../../../../../src/kernel/InfString.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/FloatState.h \
 ../../../../../src/kernel/StringArrayState.h
SDFTkMeter.o: SDFTkMeter.cc SDFTkMeter.h \
 ../../../../../src/ptklib/ptk.h SDFTclScript.h \
 ../../../../../src/ptklib/TclStarIfc.h \
 ../../../../../src/kernel/InfString.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/kernel/IntState.h
SDFTkPicture.o: SDFTkPicture.cc SDFTkPicture.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/Particle.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/ptklib/Picture.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/InfString.h ../../../../../src/ptklib/ptk.h \
 ../../../../../src/ptklib/ptkPicture_defs.h \
 ../../../../../src/ptklib/ptkPicture.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h
SDFTkPlot.o: SDFTkPlot.cc SDFTkPlot.h SDFTkXYPlot.h \
 ../../../../../src/ptklib/ptk.h ../../../../../src/ptklib/XYPlot.h \
 ../../../../../src/kernel/Block.h \
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
 ../../../../../src/kernel/InfString.h \
 ../../../../../src/ptklib/ptkPlot_defs.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/kernel/FloatArrayState.h
SDFTkShowBooleans.o: SDFTkShowBooleans.cc SDFTkShowBooleans.h \
 ../../../../../src/ptklib/ptk.h SDFTclScript.h \
 ../../../../../src/ptklib/TclStarIfc.h \
 ../../../../../src/kernel/InfString.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/StringArrayState.h
SDFTkSlider.o: SDFTkSlider.cc SDFTkSlider.h \
 ../../../../../src/ptklib/ptk.h SDFTclScript.h \
 ../../../../../src/ptklib/TclStarIfc.h \
 ../../../../../src/kernel/InfString.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/FloatState.h
SDFTkText.o: SDFTkText.cc SDFTkText.h ../../../../../src/ptklib/ptk.h \
 SDFTkShowValues.h SDFTclScript.h \
 ../../../../../src/ptklib/TclStarIfc.h \
 ../../../../../src/kernel/InfString.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/kernel/IntState.h
SDFTkXYPlot.o: SDFTkXYPlot.cc SDFTkXYPlot.h \
 ../../../../../src/ptklib/ptk.h ../../../../../src/ptklib/XYPlot.h \
 ../../../../../src/kernel/Block.h \
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
 ../../../../../src/kernel/InfString.h \
 ../../../../../src/ptklib/ptkPlot_defs.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/kernel/FloatArrayState.h
