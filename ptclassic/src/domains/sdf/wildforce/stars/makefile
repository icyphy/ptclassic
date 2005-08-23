# Copyright (c) 1999 The Regents of the University of California.
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
#                                               PT_COPYRIGHT_VERSION_2
#                                               COPYRIGHTENDKEY
# Makefile for Ptolemy SDF Wildforce kernel
# @(#)make.template	1.5 11/16/99
#
# If this file's name is "make.template" and there is no makefile in
# this directory, do
#       cp make.template makefile
#       chmod +w makefile
#       make depend
# This will add the dependencies to the makefile.
 
ROOT = ../../../../..
VPATH = $(ROOT)/src/domains/sdf/wildforce/stars
 
# get configuration info
CONFIG=$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)
 
# SDFDIR is the directory containing SDF domain support
SDFDIR = $(ROOT)/src/domains/sdf/kernel
WILDFORCEKERN = $(ROOT)/src/domains/sdf/wildforce/kernel
INCL = -I$(SDFDIR) -I$(KERNDIR) -I$(ROOT)/src/domains/sdf/stars \
       -I/usr/include -I/usr/include/sys \
       -I$(WILDFORCEKERN) $(WILDFORCECFLAGS)
 
# Library name
PTLIB=libsdfwildforcestars.$(LIBSUFFIX)
 
# source files for all stars, using preprocessor, put derived stars first
PL_SRCS= \
        SDFWF4.pl
 
# The .cc files
SRCS=$(PL_SRCS:.pl=.cc)
 
# object files for all stars
OBJS= $(SRCS:.cc=.o) 
 
# header files for all stars
HDRS= $(SRCS:.cc=.h)
 
STAR_MK=sdfwildforcestars
DOMAIN=SDF_WILDFORCE
DOMAIN_2=SDF
EXTRA_SRCS=$(STAR_MK).c $(PL_SRCS) pe0_controller.m68 xbar_cfg.dat
 
all:            makefile stars_all
 
install:        stars_install
 
 
# include common definitions, rules
include $(ROOT)/mk/common.mk
 
 
# Don't add anything after the next line; makedepend will zap it.
# DO NOT DELETE THIS LINE -- make depend depends on it.

SDFWF4.o: SDFWF4.cc SDFWF4.h \
 ../../../../../src/domains/sdf/wildforce/kernel/wildforce.h \
 ../../../../../src/domains/sdf/wildforce/kernel/wf4errs.h \
 ../../../../../src/domains/sdf/wildforce/kernel/wf4api.h \
 ../../../../../src/domains/sdf/wildforce/kernel/wf4.h \
 ../../../../../src/domains/sdf/wildforce/kernel/WFIntArray.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
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
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/FloatState.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h
