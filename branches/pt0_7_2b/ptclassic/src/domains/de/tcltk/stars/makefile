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
# Makefile for Ptolemy DE tcl stars
# @(#)make.template	1.24	02/17/99
#
# If this file's name is "make.template" and there is no makefile in
# this directory, do
#	cp make.template makefile
#	chmod +w makefile
#	make depend
# This will add the dependencies to the makefile.
#
# This file is patterned after
#	$PTOLEMY/src/domains/sdf/tcltk/stars/make.template

ROOT = ../../../../..
VPATH = $(ROOT)/src/domains/de/tcltk/stars

# get configuration info
CONFIG=$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

# DEDIR is the directory containing DE domain support
DEDIR = $(ROOT)/src/domains/de/kernel
TCLDIR = $(ROOT)/tcl/include
PIGIDIR = $(ROOT)/src/pigilib
PTKLIB = $(ROOT)/src/ptklib
INCL = -I$(DEDIR) -I$(KERNDIR) -I$(PIGIDIR) -I$(PTKLIB) \
 -I$(TCL_INCDIR) -I$(TK_INCDIR) $(X11_INCSPEC)
TCL_SCRIPT_DIR = $(ROOT)/src/domains/sdf/tcltk/stars

# Library name
PTLIB=libdetclstars.$(LIBSUFFIX)

# source files for all stars, using preprocessor
# list base stars first
PL_SRCS= \
 DETclScript.pl \
 DETkShowValues.pl \
 DETkXYPlot.pl \
 DETkBarGraph.pl \
 DETkButtons.pl \
 DETkLogicAnalyzer.pl \
 DETkMeter.pl \
 DETkPlot.pl \
 DETkShowEvents.pl \
 DETkSlider.pl \
 DETkStripChart.pl \
 DETkText.pl

# DELMSTkPlot.pl \
# DELMSCxTkPlot.pl \

TCL_SRCS= \
  tkButtons.tcl \
  tkLogicAnalyzer.itcl \
  $(TCL_SCRIPT_DIR)/tkMeter.tcl \
  $(TCL_SCRIPT_DIR)/tkScript.tcl \
  tkShowEvents.tcl \
  $(TCL_SCRIPT_DIR)/tkShowValues.tcl \
  $(TCL_SCRIPT_DIR)/tkSlider.tcl \
  tkStripChart.tcl \
  tkText.tcl

# The .cc files
SRCS=$(PL_SRCS:.pl=.cc)

# object files for all stars
OBJS= $(SRCS:.cc=.o)

# header files for all stars
HDRS= $(SRCS:.cc=.h)

# extra source files: tcl/tk script files
SCRIPTS= tkText.tcl

STAR_MK=detclstars
DOMAIN=DE_TCL
DOMAIN_2=DE
EXTRA_SRCS= $(STAR_MK).c $(PL_SRCS) $(SCRIPTS) $(TCL_SRCS)

all:		makefile stars_all

install:	stars_install


# include common definitions, rules
include $(ROOT)/mk/common.mk

# Workaround bug in gcc-2.7.2 optimizer
ifeq ($(USE_GPLUSPLUS),yes)
DETkButtons.o DETkLogicAnalyzer.o DETkMeter.o DETkShowEvents.o DETkShowValues.o DETkSlider.o DETkStripChart.o DETkText.o:
	$(UNOPTIMIZED_WARNING_MESSAGE)
	$(UNOPTIMIZED_COMPILE_RULE) $(VPATH)/$(@:.o=.cc)
endif

# Don't add anything after the next line; makedepend will zap it.
# DO NOT DELETE THIS LINE -- make depend depends on it.

DETclScript.o: DETclScript.cc DETclScript.h \
 ../../../../../src/ptklib/TclStarIfc.h \
 ../../../../../src/kernel/InfString.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h \
 ../../../../../src/domains/de/kernel/DEStar.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/NamedObj.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../../src/kernel/PriorityQueue.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/ptklib/ptk.h ../../../../../src/kernel/Scheduler.h \
 ../../../../../src/kernel/SimControl.h
DETkShowValues.o: DETkShowValues.cc DETkShowValues.h \
 ../../../../../src/ptklib/ptk.h DETclScript.h \
 ../../../../../src/ptklib/TclStarIfc.h \
 ../../../../../src/kernel/InfString.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h \
 ../../../../../src/domains/de/kernel/DEStar.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/NamedObj.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../../src/kernel/PriorityQueue.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h
DETkXYPlot.o: DETkXYPlot.cc DETkXYPlot.h \
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
 ../../../../../src/domains/de/kernel/DEStar.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../../src/kernel/Star.h \
 ../../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../../src/kernel/PriorityQueue.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h
DETkBarGraph.o: DETkBarGraph.cc DETkBarGraph.h \
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
 ../../../../../src/domains/de/kernel/DEStar.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../../src/kernel/Star.h \
 ../../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../../src/kernel/PriorityQueue.h \
 ../../../../../src/kernel/FloatState.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h
DETkButtons.o: DETkButtons.cc DETkButtons.h \
 ../../../../../src/ptklib/ptk.h DETclScript.h \
 ../../../../../src/ptklib/TclStarIfc.h \
 ../../../../../src/kernel/InfString.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h \
 ../../../../../src/domains/de/kernel/DEStar.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/NamedObj.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../../src/kernel/PriorityQueue.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/kernel/FloatState.h \
 ../../../../../src/kernel/StringArrayState.h
DETkLogicAnalyzer.o: DETkLogicAnalyzer.cc DETkLogicAnalyzer.h \
 DETclScript.h ../../../../../src/ptklib/TclStarIfc.h \
 ../../../../../src/kernel/InfString.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h \
 ../../../../../src/domains/de/kernel/DEStar.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/NamedObj.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../../src/kernel/PriorityQueue.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/kernel/FloatState.h \
 ../../../../../src/kernel/StringArrayState.h
DETkMeter.o: DETkMeter.cc DETkMeter.h ../../../../../src/ptklib/ptk.h \
 DETclScript.h ../../../../../src/ptklib/TclStarIfc.h \
 ../../../../../src/kernel/InfString.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h \
 ../../../../../src/domains/de/kernel/DEStar.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/NamedObj.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../../src/kernel/PriorityQueue.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h
DETkPlot.o: DETkPlot.cc DETkPlot.h DETkXYPlot.h \
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
 ../../../../../src/domains/de/kernel/DEStar.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../../src/kernel/Star.h \
 ../../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../../src/kernel/PriorityQueue.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h
DETkShowEvents.o: DETkShowEvents.cc DETkShowEvents.h \
 ../../../../../src/ptklib/ptk.h DETclScript.h \
 ../../../../../src/ptklib/TclStarIfc.h \
 ../../../../../src/kernel/InfString.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h \
 ../../../../../src/domains/de/kernel/DEStar.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/NamedObj.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../../src/kernel/PriorityQueue.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/kernel/StringArrayState.h
DETkSlider.o: DETkSlider.cc DETkSlider.h \
 ../../../../../src/ptklib/ptk.h DETclScript.h \
 ../../../../../src/ptklib/TclStarIfc.h \
 ../../../../../src/kernel/InfString.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h \
 ../../../../../src/domains/de/kernel/DEStar.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/NamedObj.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../../src/kernel/PriorityQueue.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/kernel/FloatState.h
DETkStripChart.o: DETkStripChart.cc DETkStripChart.h DETclScript.h \
 ../../../../../src/ptklib/TclStarIfc.h \
 ../../../../../src/kernel/InfString.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h \
 ../../../../../src/domains/de/kernel/DEStar.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/NamedObj.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../../src/kernel/PriorityQueue.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/kernel/FloatState.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/StringArrayState.h
DETkText.o: DETkText.cc DETkText.h ../../../../../src/ptklib/ptk.h \
 DETkShowValues.h DETclScript.h ../../../../../src/ptklib/TclStarIfc.h \
 ../../../../../src/kernel/InfString.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h \
 ../../../../../src/domains/de/kernel/DEStar.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/NamedObj.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../../src/kernel/PriorityQueue.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h
