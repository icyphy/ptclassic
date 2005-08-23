# Makefile for SDF contrib kernel to support SDF contrib stars
# Version identification:
# @(#)make.template	1.2	10/24/95
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
# Date of creation: 10/24/95
#
# If this file's name is "make.template" and there is no makefile in
# this directory, do
#	cp make.template makefile
#	make depend
# This will add the dependencies to the makefile.

# Root of Ptolemy directory
ROOT = ../../../../..
# True source directory
VPATH = $(ROOT)/src/domains/sdf/contrib/kernel

# get configuration info
CONFIG=$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

# SDF kernel
SDFKERNDIR=$(ROOT)/src/domains/sdf/kernel

INCL = -I$(SDFKERNDIR) -I$(KERNDIR)

SRCS = Decoder.cc Encoder.cc Interp.cc Wireless.cc nb_fft.cc

HDRS = Constants.h $(SRCS:.cc=.h)

#Library name
PTLIB=libsdfcontrib.$(LIBSUFFIX)

# Objects:
OBJS = $(SRCS:.cc=.o)

all:	makefile $(PTLIB)

# "make install" puts things in the official place
install:	makefile $(LIBDIR)/$(PTLIB)

# include common definitions, rules
include $(ROOT)/mk/common.mk

# Don't add anything after the next line; makedepend will zap it.
# DO NOT DELETE THIS LINE -- make depend depends on it.

Decoder.o: Decoder.cc Decoder.h
Encoder.o: Encoder.cc Encoder.h
Interp.o: Interp.cc Interp.h
Wireless.o: Wireless.cc Wireless.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/ComplexSubset.h
nb_fft.o: nb_fft.cc nb_fft.h Constants.h
