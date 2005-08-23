# Makefile for Ptolemy DE stars
# @(#)make.template	1.65	08/25/97
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
# If this file's name is "make.template" and there is no makefile in
# this directory, do
#	make -f make.template makefile
#
# This will add the dependencies to the makefile.

ROOT = ../../../..
VPATH = $(ROOT)/src/domains/de/stars

# get configuration info
CONFIG=$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

# DEDIR is the directory containing DE domain support
DEDIR = $(ROOT)/src/domains/de/kernel
INCL = -I$(DEDIR) -I$(KERNDIR)

# source files for all stars, using preprocessor
# base classes/stars go in the front of the list
PL_SRCS= \
DEDelay.pl \
DEEther.pl \
DEEtherRec.pl \
DEQueueBase.pl \
DERealTimeStar.pl \
DESeqATMZero.pl \
DEServer.pl \
DETimeoutStar.pl \
DEArbitrate.pl \
DEBarGraph.pl \
DEBeep.pl \
DEBlackHole.pl \
DEBusFork.pl \
DECase.pl \
DECellLoad.pl \
DECellRoute.pl \
DECellToImage.pl \
DECellUnload.pl \
DEClock.pl \
DEConst.pl \
DEDiscard.pl \
DEEndCase.pl \
DEEtherRecMes.pl \
DEEtherSend.pl \
DEFIFOQueue.pl \
DEFillNullEvent.pl \
DEFilter.pl \
DEFloatToInt.pl \
DEFlipFlopD.pl \
DEFlipFlopJK.pl \
DEFlipFlopT.pl \
DEFork.pl \
DEHandShake.pl \
DEImageToCell.pl \
DEImageToMx.pl \
DEImpulse.pl \
DEIntPacketize.pl \
DEIntToFloat.pl \
DEIntUnPacketize.pl \
DELogic.pl \
DEMeasureDelay.pl \
DEMeasureInterval.pl \
DEMerge.pl \
DEMxToImage.pl \
DENull.pl \
DEPassGate.pl \
DEPCMVoiceRecover.pl \
DEPSServer.pl \
DEPacketize.pl \
DEPoisson.pl \
DEPrQueue.pl \
DEPriorityCheck.pl \
DEPriorityQueue.pl \
DEPrinter.pl \
DEPulseGen.pl \
DEQueue.pl \
DERamp.pl \
DERanGen.pl \
DERouter.pl \
DESampler.pl \
DESeqATMSub.pl \
DEStack.pl \
DEStatistics.pl \
DEStopTimer.pl \
DESynchronize.pl \
DETest.pl \
DETestLevel.pl \
DETimeout.pl \
DETimer.pl \
DETimeStamp.pl \
DEUDCounter.pl \
DEUnPacketize.pl \
DEVarDelay.pl \
DEVarServer.pl \
DEVirtClock.pl \
DEWaveForm.pl \
DEXMgraph.pl \
DEXYgraph.pl \
DEXgraph.pl \
DEXhistogram.pl

PTLIB  =  libdestars.$(LIBSUFFIX)

# Used to create .dlls under nt4.vc
DLLDEPS = 	$(KERNEL_DLLDEP) ../kernel/libde.lib

SRCS =	$(PL_SRCS:.pl=.cc)

OBJS =	$(SRCS:.cc=.o)

HDRS =	$(SRCS:.cc=.h)

DOMAIN=DE
STAR_MK=destars
EXTRA_SRCS= $(STAR_MK).c $(PL_SRCS)

all:		makefile stars_all

install:	stars_install

# include common definitions, rules
include $(ROOT)/mk/common.mk

# Workaround bug in gcc-2.7.2 optimizer
ifeq ($(USE_GPLUSPLUS),yes)
DEEtherRec.o: DEEtherRec.cc
	$(UNOPTIMIZED_WARNING_MESSAGE)
	$(UNOPTIMIZED_COMPILE_RULE) $<
DEEtherRecMes.o: DEEtherRecMes.cc
	$(UNOPTIMIZED_WARNING_MESSAGE)
	$(UNOPTIMIZED_COMPILE_RULE) $<
DEEtherSend.o: DEEtherSend.cc
	$(UNOPTIMIZED_WARNING_MESSAGE)
	$(UNOPTIMIZED_COMPILE_RULE) $<
endif

# Don't add anything after the next line; makedepend will zap it.
# DO NOT DELETE THIS LINE -- make depend depends on it.

DEDelay.o: DEDelay.cc DEDelay.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h \
 ../../../../src/kernel/FloatState.h
DEEther.o: DEEther.cc DEEther.h ../../../../src/kernel/HashTable.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/domains/de/kernel/DERepeatStar.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h \
 ../../../../src/kernel/StringState.h \
 ../../../../src/kernel/KnownState.h
DEEtherRec.o: DEEtherRec.cc DEEtherRec.h DEEther.h \
 ../../../../src/kernel/HashTable.h ../../../../src/kernel/type.h \
 ../../../../src/kernel/logNew.h \
 ../../../../src/domains/de/kernel/DERepeatStar.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h \
 ../../../../src/kernel/StringState.h \
 ../../../../src/kernel/KnownState.h
DEQueueBase.o: DEQueueBase.cc DEQueueBase.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h
DERealTimeStar.o: DERealTimeStar.cc DERealTimeStar.h \
 ../../../../src/kernel/Clock.h ../../../../src/kernel/TimeVal.h \
 ../../../../src/domains/de/kernel/DERepeatStar.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h \
 ../../../../src/kernel/FloatState.h
DESeqATMZero.o: DESeqATMZero.cc DESeqATMZero.h \
 ../../../../src/domains/de/kernel/SeqATMCell.h \
 ../../../../src/kernel/Message.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/isa.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/PortHole.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h
DEServer.o: DEServer.cc DEServer.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h \
 ../../../../src/kernel/FloatState.h
DETimeoutStar.o: DETimeoutStar.cc DETimeoutStar.h \
 ../../../../src/domains/de/kernel/DERepeatStar.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h \
 ../../../../src/kernel/FloatState.h
DEArbitrate.o: DEArbitrate.cc DEArbitrate.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h
DEBarGraph.o: DEBarGraph.cc DEBarGraph.h \
 ../../../../src/kernel/Display.h ../../../../src/kernel/type.h \
 ../../../../src/kernel/logNew.h ../../../../src/kernel/miscFuncs.h \
 ../../../../src/kernel/Error.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/isa.h ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h \
 ../../../../src/kernel/FloatState.h \
 ../../../../src/kernel/StringState.h \
 ../../../../src/kernel/KnownState.h
DEBeep.o: DEBeep.cc DEBeep.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h
DEBlackHole.o: DEBlackHole.cc DEBlackHole.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h
DEBusFork.o: DEBusFork.cc DEBusFork.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h
DECase.o: DECase.cc DECase.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h
DECellLoad.o: DECellLoad.cc DECellLoad.h \
 ../../../../src/domains/de/kernel/NetworkCell.h \
 ../../../../src/kernel/Message.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/isa.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/PortHole.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h
DECellRoute.o: DECellRoute.cc DECellRoute.h \
 ../../../../src/domains/de/kernel/NetworkCell.h \
 ../../../../src/kernel/Message.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/isa.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/PortHole.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h \
 ../../../../src/kernel/IntArrayState.h
DECellToImage.o: DECellToImage.cc DECellToImage.h \
 ../../../../src/domains/de/kernel/BaseImage.h \
 ../../../../src/kernel/Message.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/isa.h \
 ../../../../src/domains/de/kernel/NetworkCell.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/PortHole.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h
DECellUnload.o: DECellUnload.cc DECellUnload.h \
 ../../../../src/domains/de/kernel/NetworkCell.h \
 ../../../../src/kernel/Message.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/isa.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/PortHole.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h
DEClock.o: DEClock.cc DEClock.h \
 ../../../../src/domains/de/kernel/DERepeatStar.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h \
 ../../../../src/kernel/FloatState.h
DEConst.o: DEConst.cc DEConst.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h \
 ../../../../src/kernel/FloatState.h
DEDiscard.o: DEDiscard.cc DEDiscard.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h \
 ../../../../src/kernel/FloatState.h
DEEndCase.o: DEEndCase.cc DEEndCase.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h
DEEtherRecMes.o: DEEtherRecMes.cc DEEtherRecMes.h DEEtherRec.h \
 DEEther.h ../../../../src/kernel/HashTable.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/domains/de/kernel/DERepeatStar.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h \
 ../../../../src/kernel/StringState.h \
 ../../../../src/kernel/KnownState.h ../../../../src/kernel/Message.h
DEEtherSend.o: DEEtherSend.cc DEEtherSend.h DEEther.h \
 ../../../../src/kernel/HashTable.h ../../../../src/kernel/type.h \
 ../../../../src/kernel/logNew.h \
 ../../../../src/domains/de/kernel/DERepeatStar.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h \
 ../../../../src/kernel/StringState.h \
 ../../../../src/kernel/KnownState.h DEEtherRec.h
DEFIFOQueue.o: DEFIFOQueue.cc DEFIFOQueue.h DEQueueBase.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h
DEFillNullEvent.o: DEFillNullEvent.cc DEFillNullEvent.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h \
 ../../../../src/kernel/Plasma.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/PtGate.h
DEFilter.o: DEFilter.cc DEFilter.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h \
 ../../../../src/kernel/FloatState.h
DEFloatToInt.o: DEFloatToInt.cc DEFloatToInt.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h
DEFlipFlopD.o: DEFlipFlopD.cc DEFlipFlopD.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h
DEFlipFlopJK.o: DEFlipFlopJK.cc DEFlipFlopJK.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h \
 ../../../../src/kernel/FloatState.h
DEFlipFlopT.o: DEFlipFlopT.cc DEFlipFlopT.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h \
 ../../../../src/kernel/FloatState.h
DEFork.o: DEFork.cc DEFork.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h
DEHandShake.o: DEHandShake.cc DEHandShake.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h
DEImageToCell.o: DEImageToCell.cc DEImageToCell.h \
 ../../../../src/domains/de/kernel/BaseImage.h \
 ../../../../src/kernel/Message.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/isa.h \
 ../../../../src/domains/de/kernel/NetworkCell.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/PortHole.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h \
 ../../../../src/kernel/FloatState.h
DEImageToMx.o: DEImageToMx.cc DEImageToMx.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h \
 ../../../../src/domains/de/kernel/GrayImage.h \
 ../../../../src/domains/de/kernel/BaseImage.h \
 ../../../../src/kernel/Message.h ../../../../src/kernel/Matrix.h \
 ../../../../src/kernel/ComplexArrayState.h \
 ../../../../src/kernel/FixArrayState.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/kernel/FloatArrayState.h \
 ../../../../src/kernel/IntArrayState.h
DEImpulse.o: DEImpulse.cc DEImpulse.h \
 ../../../../src/domains/de/kernel/DERepeatStar.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h \
 ../../../../src/kernel/FloatState.h
DEIntPacketize.o: DEIntPacketize.cc DEIntPacketize.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h \
 ../../../../src/kernel/IntVecData.h ../../../../src/kernel/Message.h
DEIntToFloat.o: DEIntToFloat.cc DEIntToFloat.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h
DEIntUnPacketize.o: DEIntUnPacketize.cc DEIntUnPacketize.h \
 ../../../../src/kernel/IntVecData.h ../../../../src/kernel/Message.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/isa.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/PortHole.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h
DELogic.o: DELogic.cc DELogic.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h \
 ../../../../src/kernel/StringState.h \
 ../../../../src/kernel/KnownState.h
DEMeasureDelay.o: DEMeasureDelay.cc DEMeasureDelay.h \
 ../../../../src/kernel/PriorityQueue.h ../../../../src/kernel/type.h \
 ../../../../src/kernel/logNew.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h
DEMeasureInterval.o: DEMeasureInterval.cc DEMeasureInterval.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h
DEMerge.o: DEMerge.cc DEMerge.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h
DEMxToImage.o: DEMxToImage.cc DEMxToImage.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h \
 ../../../../src/domains/de/kernel/GrayImage.h \
 ../../../../src/domains/de/kernel/BaseImage.h \
 ../../../../src/kernel/Message.h ../../../../src/kernel/Matrix.h \
 ../../../../src/kernel/ComplexArrayState.h \
 ../../../../src/kernel/FixArrayState.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/kernel/FloatArrayState.h \
 ../../../../src/kernel/IntArrayState.h
DENull.o: DENull.cc DENull.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h
DEPassGate.o: DEPassGate.cc DEPassGate.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h
DEPCMVoiceRecover.o: DEPCMVoiceRecover.cc DEPCMVoiceRecover.h \
 ../../../../src/domains/de/kernel/SeqATMCell.h \
 ../../../../src/kernel/Message.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/isa.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/PortHole.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h
DEPSServer.o: DEPSServer.cc DEPSServer.h \
 ../../../../src/domains/de/kernel/DERepeatStar.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h \
 ../../../../src/kernel/FloatState.h
DEPacketize.o: DEPacketize.cc DEPacketize.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h \
 ../../../../src/kernel/FloatVecData.h \
 ../../../../src/kernel/Message.h
DEPoisson.o: DEPoisson.cc DEPoisson.h \
 ../../../../src/domains/de/kernel/DERepeatStar.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h \
 ../../../../src/kernel/FloatState.h
DEPrQueue.o: DEPrQueue.cc DEPrQueue.h \
 ../../../../src/kernel/PriorityQueue.h ../../../../src/kernel/type.h \
 ../../../../src/kernel/logNew.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h
DEPriorityCheck.o: DEPriorityCheck.cc DEPriorityCheck.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h \
 ../../../../src/domains/de/kernel/NetworkCell.h \
 ../../../../src/kernel/Message.h
DEPriorityQueue.o: DEPriorityQueue.cc DEPriorityQueue.h \
 ../../../../src/kernel/PriorityQueue.h ../../../../src/kernel/type.h \
 ../../../../src/kernel/logNew.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h
DEPrinter.o: DEPrinter.cc DEPrinter.h \
 ../../../../src/kernel/pt_fstream.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h \
 ../../../../src/kernel/StringState.h \
 ../../../../src/kernel/KnownState.h
DEPulseGen.o: DEPulseGen.cc DEPulseGen.h \
 ../../../../src/domains/de/kernel/DERepeatStar.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h \
 ../../../../src/kernel/ComplexArrayState.h
DEQueue.o: DEQueue.cc DEQueue.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h
DERamp.o: DERamp.cc DERamp.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h \
 ../../../../src/kernel/FloatState.h
DERanGen.o: DERanGen.cc DERanGen.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h \
 ../../../../src/kernel/FloatState.h \
 ../../../../src/kernel/StringState.h \
 ../../../../src/kernel/KnownState.h
DERouter.o: DERouter.cc DERouter.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h
DESampler.o: DESampler.cc DESampler.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h
DESeqATMSub.o: DESeqATMSub.cc DESeqATMSub.h DESeqATMZero.h \
 ../../../../src/domains/de/kernel/SeqATMCell.h \
 ../../../../src/kernel/Message.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/isa.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/PortHole.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h
DEStack.o: DEStack.cc DEStack.h DEQueueBase.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h
DEStatistics.o: DEStatistics.cc DEStatistics.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h
DEStopTimer.o: DEStopTimer.cc DEStopTimer.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h \
 ../../../../src/domains/de/kernel/DEScheduler.h \
 ../../../../src/kernel/Galaxy.h ../../../../src/kernel/Scheduler.h \
 ../../../../src/kernel/SimControl.h \
 ../../../../src/domains/de/kernel/EventQueue.h
DESynchronize.o: DESynchronize.cc DESynchronize.h DERealTimeStar.h \
 ../../../../src/kernel/Clock.h ../../../../src/kernel/TimeVal.h \
 ../../../../src/domains/de/kernel/DERepeatStar.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h \
 ../../../../src/kernel/FloatState.h
DETest.o: DETest.cc DETest.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h \
 ../../../../src/kernel/StringState.h \
 ../../../../src/kernel/KnownState.h
DETestLevel.o: DETestLevel.cc DETestLevel.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h \
 ../../../../src/kernel/FloatState.h
DETimeout.o: DETimeout.cc DETimeout.h DETimeoutStar.h \
 ../../../../src/domains/de/kernel/DERepeatStar.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h \
 ../../../../src/kernel/FloatState.h
DETimer.o: DETimer.cc DETimer.h ../../../../src/kernel/Clock.h \
 ../../../../src/kernel/TimeVal.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h \
 ../../../../src/kernel/FloatState.h
DETimeStamp.o: DETimeStamp.cc DETimeStamp.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h
DEUDCounter.o: DEUDCounter.cc DEUDCounter.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h
DEUnPacketize.o: DEUnPacketize.cc DEUnPacketize.h \
 ../../../../src/kernel/FloatVecData.h \
 ../../../../src/kernel/Message.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/isa.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/PortHole.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h
DEVarDelay.o: DEVarDelay.cc DEVarDelay.h DEDelay.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h \
 ../../../../src/kernel/FloatState.h
DEVarServer.o: DEVarServer.cc DEVarServer.h DEServer.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h \
 ../../../../src/kernel/FloatState.h
DEVirtClock.o: DEVirtClock.cc DEVirtClock.h \
 ../../../../src/domains/de/kernel/NetworkCell.h \
 ../../../../src/kernel/Message.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/isa.h ../../../../src/kernel/PriorityQueue.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/PortHole.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/FloatArrayState.h
DEWaveForm.o: DEWaveForm.cc DEWaveForm.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h \
 ../../../../src/kernel/FloatArrayState.h \
 ../../../../src/kernel/SimControl.h
DEXMgraph.o: DEXMgraph.cc DEXMgraph.h ../../../../src/kernel/Display.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/isa.h ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h \
 ../../../../src/kernel/StringState.h \
 ../../../../src/kernel/KnownState.h
DEXYgraph.o: DEXYgraph.cc DEXYgraph.h DEXgraph.h \
 ../../../../src/kernel/Display.h ../../../../src/kernel/type.h \
 ../../../../src/kernel/logNew.h ../../../../src/kernel/miscFuncs.h \
 ../../../../src/kernel/Error.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/isa.h ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h \
 ../../../../src/kernel/StringState.h \
 ../../../../src/kernel/KnownState.h
DEXgraph.o: DEXgraph.cc DEXgraph.h ../../../../src/kernel/Display.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/isa.h ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h \
 ../../../../src/kernel/StringState.h \
 ../../../../src/kernel/KnownState.h
DEXhistogram.o: DEXhistogram.cc DEXhistogram.h \
 ../../../../src/kernel/Histogram.h ../../../../src/kernel/logNew.h \
 ../../../../src/domains/de/kernel/DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/IntState.h \
 ../../../../src/kernel/State.h ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h \
 ../../../../src/kernel/FloatState.h \
 ../../../../src/kernel/StringState.h \
 ../../../../src/kernel/KnownState.h
