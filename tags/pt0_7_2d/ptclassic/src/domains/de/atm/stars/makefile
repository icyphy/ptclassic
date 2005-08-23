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
# Makefile for Ptolemy DE ATM simulation stars
#
# @(#)make.template	1.6	09/20/96
#
# If this file's name is "make.template" and there is no makefile in
# this directory, do
#	cp make.template makefile
#	chmod +w makefile
#	make depend
# This will add the dependencies to the makefile.

ROOT = ../../../../..
VPATH = $(ROOT)/src/domains/de/atm/stars

# get configuration info
CONFIG=$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

# DEDIR is the directory containing DE domain support
DEDIR = $(ROOT)/src/domains/de/kernel
ATM = $(ROOT)/src/domains/sdf/atm/kernel
INCL = -I$(DEDIR) -I$(KERNDIR) -I$(ATM)

# Library name
PTLIB=libdeatmstars.$(LIBSUFFIX)

# source files for all stars, using preprocessor
PL_SRCS= \
DEFStopTimer.pl \
DEInputSynch.pl \
DELatency.pl \
DENullRemove.pl \
DEOffset.pl \
DEProgress.pl \
DESrcControl.pl \
DEThroughput.pl \
DEToBus.pl \
DEToBusFl.pl \
DEVoiceFl.pl \
DEVoiceSrc.pl \


# The .cc files
SRCS=$(PL_SRCS:.pl=.cc)

# object files for all stars
OBJS= $(SRCS:.cc=.o)

# header files for all stars
HDRS= $(SRCS:.cc=.h)

STAR_MK=deatmstars
DOMAIN=DE_ATM
DOMAIN_2=DE
EXTRA_SRCS=$(STAR_MK).c $(PL_SRCS)

all:		makefile stars_all

install:	stars_install


# include common definitions, rules
include $(ROOT)/mk/common.mk


# Don't add anything after the next line; makedepend will zap it.
# DO NOT DELETE THIS LINE -- make depend depends on it.

DEFStopTimer.o: DEFStopTimer.cc DEFStopTimer.h \
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
 ../../../../../src/domains/de/kernel/DEScheduler.h \
 ../../../../../src/kernel/Galaxy.h \
 ../../../../../src/kernel/Scheduler.h \
 ../../../../../src/kernel/SimControl.h \
 ../../../../../src/domains/de/kernel/EventQueue.h
DEInputSynch.o: DEInputSynch.cc DEInputSynch.h \
 ../../../../../src/domains/sdf/atm/kernel/VoiceData.h \
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
 ../../../../../src/domains/de/kernel/DEStar.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../../src/kernel/PortHole.h ../../../../../src/kernel/Star.h \
 ../../../../../src/kernel/Block.h \
 ../../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../../src/kernel/PriorityQueue.h \
 ../../../../../src/kernel/FloatState.h
DELatency.o: DELatency.cc DELatency.h \
 ../../../../../src/domains/sdf/atm/kernel/VoiceData.h \
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
 ../../../../../src/domains/de/kernel/DEStar.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/domains/de/kernel/DEPortHole.h \
 ../../../../../src/kernel/PortHole.h ../../../../../src/kernel/Star.h \
 ../../../../../src/kernel/Block.h \
 ../../../../../src/domains/de/kernel/LinkedList.h \
 ../../../../../src/domains/de/kernel/PendingEventList.h \
 ../../../../../src/domains/de/kernel/CalendarQueue.h \
 ../../../../../src/kernel/PriorityQueue.h
DENullRemove.o: DENullRemove.cc DENullRemove.h \
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
 ../../../../../src/domains/sdf/atm/kernel/VoiceData.h \
 ../../../../../src/kernel/Message.h
DEOffset.o: DEOffset.cc DEOffset.h \
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
DEProgress.o: DEProgress.cc DEProgress.h \
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
DESrcControl.o: DESrcControl.cc DESrcControl.h \
 ../../../../../src/domains/de/kernel/DERepeatStar.h \
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
 ../../../../../src/domains/sdf/atm/kernel/VoiceData.h \
 ../../../../../src/kernel/Message.h
DEThroughput.o: DEThroughput.cc DEThroughput.h \
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
 ../../../../../src/domains/sdf/atm/kernel/VoiceData.h \
 ../../../../../src/kernel/Message.h
DEToBus.o: DEToBus.cc DEToBus.h \
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
 ../../../../../src/kernel/Message.h
DEToBusFl.o: DEToBusFl.cc DEToBusFl.h \
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
DEVoiceFl.o: DEVoiceFl.cc DEVoiceFl.h \
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
 ../../../../../src/domains/sdf/atm/kernel/VoiceData.h \
 ../../../../../src/kernel/Message.h
DEVoiceSrc.o: DEVoiceSrc.cc DEVoiceSrc.h \
 ../../../../../src/domains/de/kernel/DERepeatStar.h \
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
 ../../../../../src/domains/sdf/atm/kernel/VoiceData.h \
 ../../../../../src/kernel/Message.h
