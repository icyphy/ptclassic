# Makefile for DE domain support
# Version identification:
# @(#)make.template	1.23 12/14/98
# Copyright (c) 1990-1998 The Regents of the University of California.
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
# Date of creation: 5/7/91
#
# If this file's name is "make.template" and there is no makefile in
# this directory, do
#	cp make.template makefile
#	make depend
# This will add the dependencies to the makefile.

# Root of Ptolemy directory
ROOT = ../../../..
# True source directory
VPATH = $(ROOT)/src/domains/de/kernel

# get configuration info
CONFIG=$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

INCL = -I$(KERNDIR)

# DCTImage.h and MVImage.h should be removed once we get ParticleVec working
HDRS = \
	BaseImage.h \
	CQEventQueue.h \
	CQScheduler.h \
	CalendarQueue.h \
	DCTImage.h \
	DEPortHole.h \
	DERCScheduler.h \
	DERCStar.h \
	DERepeatStar.h \
	DEScheduler.h \
	DEStar.h \
	DETarget.h \
	DEWormhole.h \
	EventQueue.h \
	GrayImage.h \
	LinkedList.h \
	MVImage.h \
	MutableCQEventQueue.h \
	MutableCQScheduler.h \
	MutableCalendarQueue.h \
	NetworkCell.h \
	PendingEventList.h \
	Resource.h \
	SeqATMCell.h

LIB_SRCS = $(HDRS:.h=.cc) DEDomain.cc
SRCS = $(LIB_SRCS)

#Library name
PTLIB=libde.$(LIBSUFFIX)

# Used to create .dlls under nt4.vc
DLLDEPS =	..\..\..\kernel\libptolemy.lib

# Objects:
OBJS = $(LIB_SRCS:.cc=.o)

all: 	makefile $(PTLIB)

# "make install" puts things in the official place
install:	makefile $(LIBDIR)/$(PTLIB)


# include common definitions, rules
include $(ROOT)/mk/common.mk


# Don't add anything after the next line; makedepend will zap it.
# DO NOT DELETE THIS LINE -- make depend depends on it.

BaseImage.o: BaseImage.cc BaseImage.h ../../../../src/kernel/Message.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/isa.h
CQEventQueue.o: CQEventQueue.cc CQEventQueue.h CalendarQueue.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h LinkedList.h \
 ../../../../src/kernel/PriorityQueue.h EventQueue.h DEPortHole.h
CQScheduler.o: CQScheduler.cc ../../../../src/kernel/type.h \
 ../../../../src/kernel/logNew.h CQScheduler.h \
 ../../../../src/kernel/Galaxy.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/DataStruct.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h DEStar.h \
 ../../../../src/kernel/IntState.h DEPortHole.h LinkedList.h \
 PendingEventList.h CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h DEScheduler.h \
 ../../../../src/kernel/Scheduler.h \
 ../../../../src/kernel/SimControl.h EventQueue.h CQEventQueue.h \
 ../../../../src/kernel/FloatState.h ../../../../src/kernel/GalIter.h \
 ../../../../src/kernel/checkConnect.h
CalendarQueue.o: CalendarQueue.cc EventQueue.h \
 ../../../../src/kernel/PriorityQueue.h ../../../../src/kernel/type.h \
 ../../../../src/kernel/logNew.h CalendarQueue.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/DataStruct.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h LinkedList.h
DCTImage.o: DCTImage.cc DCTImage.h BaseImage.h \
 ../../../../src/kernel/Message.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/isa.h
DEPortHole.o: DEPortHole.cc DEPortHole.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/PortHole.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h \
 ../../../../src/kernel/EventHorizon.h DEScheduler.h \
 ../../../../src/kernel/Galaxy.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h DEStar.h \
 ../../../../src/kernel/IntState.h LinkedList.h PendingEventList.h \
 CalendarQueue.h ../../../../src/kernel/PriorityQueue.h \
 ../../../../src/kernel/Scheduler.h \
 ../../../../src/kernel/SimControl.h EventQueue.h \
 ../../../../src/kernel/Plasma.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/PtGate.h \
 ../../../../src/kernel/CircularBuffer.h
DERCScheduler.o: DERCScheduler.cc ../../../../src/kernel/type.h \
 ../../../../src/kernel/logNew.h DERCScheduler.h \
 ../../../../src/kernel/Galaxy.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/DataStruct.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h DEStar.h \
 ../../../../src/kernel/IntState.h DEPortHole.h LinkedList.h \
 PendingEventList.h CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h DEScheduler.h \
 ../../../../src/kernel/Scheduler.h \
 ../../../../src/kernel/SimControl.h EventQueue.h CQEventQueue.h \
 Resource.h DERCStar.h DERepeatStar.h \
 ../../../../src/kernel/HashTable.h \
 ../../../../src/kernel/FloatState.h ../../../../src/kernel/GalIter.h \
 ../../../../src/kernel/checkConnect.h
DERCStar.o: DERCStar.cc DERCStar.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 DERepeatStar.h DEStar.h ../../../../src/kernel/IntState.h \
 ../../../../src/kernel/State.h ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/Attribute.h \
 DEPortHole.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 LinkedList.h PendingEventList.h CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h Resource.h \
 ../../../../src/kernel/Galaxy.h CQEventQueue.h EventQueue.h \
 DERCScheduler.h DEScheduler.h ../../../../src/kernel/Scheduler.h \
 ../../../../src/kernel/SimControl.h \
 ../../../../src/kernel/HashTable.h
DERepeatStar.o: DERepeatStar.cc DERepeatStar.h DEStar.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 LinkedList.h PendingEventList.h CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h DEScheduler.h \
 ../../../../src/kernel/Galaxy.h ../../../../src/kernel/Scheduler.h \
 ../../../../src/kernel/SimControl.h EventQueue.h DEWormhole.h \
 DETarget.h ../../../../src/kernel/Target.h \
 ../../../../src/kernel/FloatState.h ../../../../src/kernel/Wormhole.h \
 ../../../../src/kernel/Universe.h \
 ../../../../src/kernel/EventHorizon.h
DEScheduler.o: DEScheduler.cc ../../../../src/kernel/type.h \
 ../../../../src/kernel/logNew.h DEScheduler.h \
 ../../../../src/kernel/Galaxy.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/DataStruct.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h DEStar.h \
 ../../../../src/kernel/IntState.h DEPortHole.h LinkedList.h \
 PendingEventList.h CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h \
 ../../../../src/kernel/Scheduler.h \
 ../../../../src/kernel/SimControl.h EventQueue.h \
 ../../../../src/kernel/FloatState.h ../../../../src/kernel/GalIter.h \
 ../../../../src/kernel/checkConnect.h
DEStar.o: DEStar.cc DEScheduler.h ../../../../src/kernel/type.h \
 ../../../../src/kernel/logNew.h ../../../../src/kernel/Galaxy.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/DataStruct.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h DEStar.h \
 ../../../../src/kernel/IntState.h DEPortHole.h LinkedList.h \
 PendingEventList.h CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h \
 ../../../../src/kernel/Scheduler.h \
 ../../../../src/kernel/SimControl.h EventQueue.h DEWormhole.h \
 DETarget.h ../../../../src/kernel/Target.h \
 ../../../../src/kernel/FloatState.h ../../../../src/kernel/Wormhole.h \
 ../../../../src/kernel/Universe.h \
 ../../../../src/kernel/EventHorizon.h MutableCQEventQueue.h \
 MutableCalendarQueue.h
DETarget.o: DETarget.cc DETarget.h ../../../../src/kernel/Target.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/Block.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/FloatState.h \
 DEScheduler.h ../../../../src/kernel/Galaxy.h \
 ../../../../src/kernel/Star.h DEStar.h DEPortHole.h LinkedList.h \
 PendingEventList.h CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h \
 ../../../../src/kernel/Scheduler.h \
 ../../../../src/kernel/SimControl.h EventQueue.h CQScheduler.h \
 CQEventQueue.h MutableCQScheduler.h MutableCQEventQueue.h \
 MutableCalendarQueue.h DERCScheduler.h Resource.h DERCStar.h \
 DERepeatStar.h ../../../../src/kernel/HashTable.h \
 ../../../../src/kernel/GalIter.h
DEWormhole.o: DEWormhole.cc DETarget.h ../../../../src/kernel/Target.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/Block.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/FloatState.h \
 DEScheduler.h ../../../../src/kernel/Galaxy.h \
 ../../../../src/kernel/Star.h DEStar.h DEPortHole.h LinkedList.h \
 PendingEventList.h CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h \
 ../../../../src/kernel/Scheduler.h \
 ../../../../src/kernel/SimControl.h EventQueue.h DEWormhole.h \
 ../../../../src/kernel/Wormhole.h ../../../../src/kernel/Universe.h \
 ../../../../src/kernel/EventHorizon.h \
 ../../../../src/kernel/CircularBuffer.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/Plasma.h ../../../../src/kernel/PtGate.h
EventQueue.o: EventQueue.cc EventQueue.h \
 ../../../../src/kernel/PriorityQueue.h ../../../../src/kernel/type.h \
 ../../../../src/kernel/logNew.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/DataStruct.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h
GrayImage.o: GrayImage.cc GrayImage.h BaseImage.h \
 ../../../../src/kernel/Message.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/isa.h
LinkedList.o: LinkedList.cc LinkedList.h ../../../../src/kernel/type.h \
 ../../../../src/kernel/logNew.h
MVImage.o: MVImage.cc MVImage.h BaseImage.h \
 ../../../../src/kernel/Message.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/isa.h
MutableCQEventQueue.o: MutableCQEventQueue.cc MutableCQEventQueue.h \
 MutableCalendarQueue.h LinkedList.h ../../../../src/kernel/type.h \
 ../../../../src/kernel/logNew.h ../../../../src/kernel/DataStruct.h \
 DEStar.h ../../../../src/kernel/IntState.h \
 ../../../../src/kernel/State.h ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 PendingEventList.h CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h EventQueue.h
MutableCQScheduler.o: MutableCQScheduler.cc \
 ../../../../src/kernel/type.h ../../../../src/kernel/logNew.h \
 MutableCQScheduler.h ../../../../src/kernel/Galaxy.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/DataStruct.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h DEStar.h \
 ../../../../src/kernel/IntState.h DEPortHole.h LinkedList.h \
 PendingEventList.h CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h DEScheduler.h \
 ../../../../src/kernel/Scheduler.h \
 ../../../../src/kernel/SimControl.h EventQueue.h \
 MutableCQEventQueue.h MutableCalendarQueue.h \
 ../../../../src/kernel/FloatState.h ../../../../src/kernel/GalIter.h \
 ../../../../src/kernel/checkConnect.h
MutableCalendarQueue.o: MutableCalendarQueue.cc EventQueue.h \
 ../../../../src/kernel/PriorityQueue.h ../../../../src/kernel/type.h \
 ../../../../src/kernel/logNew.h LinkedList.h MutableCalendarQueue.h \
 ../../../../src/kernel/DataStruct.h DEStar.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 PendingEventList.h CalendarQueue.h
NetworkCell.o: NetworkCell.cc NetworkCell.h \
 ../../../../src/kernel/Message.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/isa.h
PendingEventList.o: PendingEventList.cc ../../../../src/kernel/Error.h \
 PendingEventList.h ../../../../src/kernel/type.h \
 ../../../../src/kernel/logNew.h LinkedList.h MutableCQEventQueue.h \
 MutableCalendarQueue.h ../../../../src/kernel/DataStruct.h DEStar.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/isa.h ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h DEPortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 CalendarQueue.h ../../../../src/kernel/PriorityQueue.h EventQueue.h
Resource.o: Resource.cc ../../../../src/kernel/type.h \
 ../../../../src/kernel/logNew.h ../../../../src/kernel/DataStruct.h \
 Resource.h ../../../../src/kernel/Galaxy.h \
 ../../../../src/kernel/Star.h ../../../../src/kernel/Block.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/Error.h \
 ../../../../src/kernel/isa.h ../../../../src/kernel/FlagArray.h \
 ../../../../src/kernel/PortHole.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Particle.h ../../../../src/kernel/Fix.h \
 ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h DERCStar.h DERepeatStar.h \
 DEStar.h ../../../../src/kernel/IntState.h DEPortHole.h LinkedList.h \
 PendingEventList.h CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h DERCScheduler.h DEScheduler.h \
 ../../../../src/kernel/Scheduler.h \
 ../../../../src/kernel/SimControl.h EventQueue.h CQEventQueue.h \
 ../../../../src/kernel/HashTable.h
SeqATMCell.o: SeqATMCell.cc SeqATMCell.h \
 ../../../../src/kernel/Message.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/dataType.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/isa.h
DEDomain.o: DEDomain.cc ../../../../src/kernel/Domain.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/Block.h \
 ../../../../src/kernel/NamedObj.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h DETarget.h \
 ../../../../src/kernel/Target.h ../../../../src/kernel/IntState.h \
 ../../../../src/kernel/FloatState.h DEScheduler.h \
 ../../../../src/kernel/Galaxy.h ../../../../src/kernel/Star.h \
 DEStar.h DEPortHole.h LinkedList.h PendingEventList.h CalendarQueue.h \
 ../../../../src/kernel/PriorityQueue.h \
 ../../../../src/kernel/Scheduler.h \
 ../../../../src/kernel/SimControl.h EventQueue.h \
 ../../../../src/kernel/KnownTarget.h DEWormhole.h \
 ../../../../src/kernel/Wormhole.h ../../../../src/kernel/Universe.h \
 ../../../../src/kernel/EventHorizon.h \
 ../../../../src/kernel/AutoForkNode.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/AutoFork.h
