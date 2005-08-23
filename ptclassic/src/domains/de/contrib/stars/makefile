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
# Makefile for Ptolemy DE stars contributed by users
# @(#)make.template	1.11	08/25/98 
#
# If this file's name is "make.template" and there is no makefile in
# this directory, do
#	cp make.template makefile
#	chmod +w makefile
#	make depend
# This will add the dependencies to the makefile.

ROOT = 		../../../../..
VPATH = 	$(ROOT)/src/domains/de/contrib/stars

# get configuration info
CONFIG =	$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

# DEDIR is the directory containing DE domain support
DEDIR = 	$(ROOT)/src/domains/de/kernel
TCLDIR = 	$(ROOT)/tcl/include
PIGIDIR =	$(ROOT)/src/pigilib
PTKLIB = 	$(ROOT)/src/ptklib
INCL =		-I$(DEDIR) -I$(KERNDIR) -I$(PIGIDIR) -I$(PTKLIB) \
			-I$(TCL_INCDIR) -I$(TK_INCDIR) $(X11_INCSPEC) \
			-I$(ROOT)/src/domains/de/contrib/stars

# Library name
PTLIB =		libdecontribstars.$(LIBSUFFIX)

# source files for all stars, using preprocessor
PL_SRCS= \
DECIR_uldl.pl \
DEMobile_Station.pl \
DEParallel2Serial.pl \
DEWeighted_List.pl \
DEWiNeS_Tcl_Animation.pl \
DEcell_list.pl \
DEpos_generator.pl \
DEpropag_uldl.pl \
DErrmanager_uldl.pl \
DEDynMapBase.pl \
DEDynMap.pl \
DEDynMapGr.pl \
DEDynMerge.pl \
DEDynForkBase.pl \
DEDynForkInt.pl \
DEDynFork.pl

# The .cc files
SRCS =		$(PL_SRCS:.pl=.cc)

# object files for all stars
OBJS =		$(SRCS:.cc=.o)

# header files for all stars
HDRS =		$(SRCS:.cc=.h)

STAR_MK =	decontribstars
DOMAIN =	DE_CONTRIB
DOMAIN_2 =	DE
MISC_FILES =	WiNeS.tcl bs.xbm ms.xbm	topo.dxf

EXTRA_SRCS =	$(STAR_MK).c $(PL_SRCS) $(MISC_FILES)


all:		$(SRCS) stars_all

install:	stars_install

# allow CC_SHAREDFLAGS to be overridden (neal@ctd.comsat.com (Neal Becker))
-include $(VPATH)/override.mk

# include common definitions, rules
include $(ROOT)/mk/common.mk

# Workaround bug in gcc-2.7.2 optimizer
ifeq ($(USE_GPLUSPLUS),yes)
DEDynMap.o: DEDynMap.cc
	$(UNOPTIMIZED_WARNING_MESSAGE)
	$(UNOPTIMIZED_COMPILE_RULE) $<

DEpropag_uldl.o: DEpropag_uldl.cc
	$(UNOPTIMIZED_WARNING_MESSAGE)
	$(UNOPTIMIZED_COMPILE_RULE) $<
ifeq ($(PT_EGCS_OPT_ERROR),yes)
DEWiNeS_Tcl_Animation.o: DEWiNeS_Tcl_Animation.cc
	@echo "DANGER: No optimizing to avoid egcs 1.0.x internal compiler error here."
	$(UNOPTIMIZED_COMPILE_RULE) $<
endif

endif

# Don't add anything after the next line; makedepend will zap it.
# DO NOT DELETE THIS LINE -- make depend depends on it.

DECIR_uldl.o: DECIR_uldl.cc DECIR_uldl.h DErrmanager_uldl.h \
 DEcell_list.h ../../../../../src/domains/de/kernel/DEStar.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/logNew.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/Error.h \
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
 ../../../../../src/kernel/FloatState.h DEpropag_uldl.h \
 ../../../../../src/kernel/pt_fstream.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h
DEMobile_Station.o: DEMobile_Station.cc DEMobile_Station.h \
 DEcell_list.h ../../../../../src/domains/de/kernel/DEStar.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/logNew.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/Error.h \
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
 ../../../../../src/kernel/FloatState.h
DEParallel2Serial.o: DEParallel2Serial.cc DEParallel2Serial.h \
 ../../../../../src/domains/de/kernel/DEStar.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/logNew.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/Error.h \
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
 ../../../../../src/kernel/PriorityQueue.h
DEWeighted_List.o: DEWeighted_List.cc DEWeighted_List.h \
 ../../../../../src/domains/de/kernel/DEStar.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/logNew.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/Error.h \
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
 ../../../../../src/kernel/PriorityQueue.h
DEWiNeS_Tcl_Animation.o: DEWiNeS_Tcl_Animation.cc \
 DEWiNeS_Tcl_Animation.h ../../../../../src/domains/de/kernel/DEStar.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/logNew.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/Error.h \
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
 ../../../../../src/ptklib/TclStarIfc.h \
 ../../../../../src/kernel/InfString.h \
 ../../../../../src/ptklib/ptkTclCommands.h DErrmanager_uldl.h \
 DEcell_list.h ../../../../../src/kernel/FloatState.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/ptklib/ptk.h
DEcell_list.o: DEcell_list.cc DEcell_list.h \
 ../../../../../src/domains/de/kernel/DEStar.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/logNew.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/Error.h \
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
 ../../../../../src/kernel/FloatState.h
DEpos_generator.o: DEpos_generator.cc DEpos_generator.h \
 ../../../../../src/domains/de/kernel/DEStar.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/logNew.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/Error.h \
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
 ../../../../../src/kernel/PriorityQueue.h
DEpropag_uldl.o: DEpropag_uldl.cc DEpropag_uldl.h \
 ../../../../../src/kernel/pt_fstream.h DEcell_list.h \
 ../../../../../src/domains/de/kernel/DEStar.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/logNew.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/Error.h \
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
 ../../../../../src/kernel/FloatState.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h
DErrmanager_uldl.o: DErrmanager_uldl.cc DErrmanager_uldl.h \
 DEcell_list.h ../../../../../src/domains/de/kernel/DEStar.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/logNew.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/Error.h \
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
 ../../../../../src/kernel/FloatState.h
DEDynMapBase.o: DEDynMapBase.cc DEDynMapBase.h \
 ../../../../../src/kernel/checkConnect.h \
 ../../../../../src/kernel/Galaxy.h ../../../../../src/kernel/Star.h \
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
 ../../../../../src/domains/de/kernel/MutableCQScheduler.h \
 ../../../../../src/domains/de/kernel/DEStar.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../../src/kernel/PriorityQueue.h \
 ../../../../../src/domains/de/kernel/DEScheduler.h \
 ../../../../../src/kernel/Scheduler.h \
 ../../../../../src/kernel/SimControl.h \
 ../../../../../src/domains/de/kernel/EventQueue.h \
 ../../../../../src/domains/de/kernel/MutableCQEventQueue.h \
 ../../../../../src/domains/de/kernel/MutableCalendarQueue.h \
 ../../../../../src/domains/de/kernel/CQScheduler.h \
 ../../../../../src/domains/de/kernel/CQEventQueue.h \
 ../../../../../src/domains/de/kernel/DERepeatStar.h DEDynMerge.h \
 ../../../../../src/domains/de/kernel/DETarget.h \
 ../../../../../src/kernel/Target.h \
 ../../../../../src/kernel/FloatState.h \
 ../../../../../src/kernel/KnownBlock.h \
 ../../../../../src/kernel/Linker.h \
 ../../../../../src/kernel/InfString.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/kernel/StringArrayState.h \
 ../../../../../src/ptklib/ptk.h
DEDynMap.o: DEDynMap.cc DEDynMap.h DEDynForkBase.h \
 ../../../../../src/domains/de/kernel/DEStar.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/logNew.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/Error.h \
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
 ../../../../../src/kernel/PriorityQueue.h DEDynMerge.h DEDynMapBase.h \
 ../../../../../src/kernel/checkConnect.h \
 ../../../../../src/kernel/Galaxy.h \
 ../../../../../src/domains/de/kernel/MutableCQScheduler.h \
 ../../../../../src/domains/de/kernel/DEScheduler.h \
 ../../../../../src/kernel/Scheduler.h \
 ../../../../../src/kernel/SimControl.h \
 ../../../../../src/domains/de/kernel/EventQueue.h \
 ../../../../../src/domains/de/kernel/MutableCQEventQueue.h \
 ../../../../../src/domains/de/kernel/MutableCalendarQueue.h \
 ../../../../../src/domains/de/kernel/CQScheduler.h \
 ../../../../../src/domains/de/kernel/CQEventQueue.h \
 ../../../../../src/domains/de/kernel/DERepeatStar.h \
 ../../../../../src/domains/de/kernel/DETarget.h \
 ../../../../../src/kernel/Target.h \
 ../../../../../src/kernel/FloatState.h \
 ../../../../../src/kernel/KnownBlock.h \
 ../../../../../src/kernel/Linker.h \
 ../../../../../src/kernel/InfString.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/kernel/StringArrayState.h
DEDynMapGr.o: DEDynMapGr.cc DEDynMapGr.h DEDynMap.h DEDynForkBase.h \
 ../../../../../src/domains/de/kernel/DEStar.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/logNew.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/Error.h \
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
 ../../../../../src/kernel/PriorityQueue.h DEDynMerge.h DEDynMapBase.h \
 ../../../../../src/kernel/checkConnect.h \
 ../../../../../src/kernel/Galaxy.h \
 ../../../../../src/domains/de/kernel/MutableCQScheduler.h \
 ../../../../../src/domains/de/kernel/DEScheduler.h \
 ../../../../../src/kernel/Scheduler.h \
 ../../../../../src/kernel/SimControl.h \
 ../../../../../src/domains/de/kernel/EventQueue.h \
 ../../../../../src/domains/de/kernel/MutableCQEventQueue.h \
 ../../../../../src/domains/de/kernel/MutableCalendarQueue.h \
 ../../../../../src/domains/de/kernel/CQScheduler.h \
 ../../../../../src/domains/de/kernel/CQEventQueue.h \
 ../../../../../src/domains/de/kernel/DERepeatStar.h \
 ../../../../../src/domains/de/kernel/DETarget.h \
 ../../../../../src/kernel/Target.h \
 ../../../../../src/kernel/FloatState.h \
 ../../../../../src/kernel/KnownBlock.h \
 ../../../../../src/kernel/Linker.h \
 ../../../../../src/kernel/InfString.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/kernel/StringArrayState.h
DEDynMerge.o: DEDynMerge.cc DEDynMerge.h \
 ../../../../../src/domains/de/kernel/DEStar.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/logNew.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/Error.h \
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
 ../../../../../src/kernel/PriorityQueue.h
DEDynForkBase.o: DEDynForkBase.cc DEDynForkBase.h \
 ../../../../../src/domains/de/kernel/DEStar.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/logNew.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/Error.h \
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
 ../../../../../src/kernel/PriorityQueue.h
DEDynForkInt.o: DEDynForkInt.cc DEDynForkInt.h DEDynForkBase.h \
 ../../../../../src/domains/de/kernel/DEStar.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/logNew.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/Error.h \
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
 ../../../../../src/kernel/PriorityQueue.h
DEDynFork.o: DEDynFork.cc DEDynFork.h DEDynForkBase.h \
 ../../../../../src/domains/de/kernel/DEStar.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/logNew.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/Error.h \
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
 ../../../../../src/kernel/PriorityQueue.h
