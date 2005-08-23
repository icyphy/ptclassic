# Version Identification:
# @(#)make.template	1.22 04/27/97
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
# Makefile for a subset of the Cephes Math Library of C routines.
# The Cephes Math Library is available from Netlib, via the World Wide
# Web address http://www.netlib.org/.
#
# Date of creation:  October 17, 1995
#
# If this files name is make.template and there is no makefile in
# this directory, do
#	cp make.template makefile
#	make depend
# This will add the dependencies to the makefile.
#
# root of source tree
ROOT	      = ../../..
# VPATH points to the real source directory
VPATH         = $(ROOT)/src/utils/libptdsp

# get configuration info
CONFIG=$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

# This is a C library, not a C++ library
# If we do build this as a C++ library, then we need to be sure to include
# -lg++ when building cgc binaries.
CLIB = 		libptdsp.$(LIBSUFFIX)

C_INCL = $(PTCOMPAT_INCSPEC)

LIB_HDRS = \
 ptdspfft_rif.h \
 ptdspNearestNeighbor.h \
 ptdspPartitionCodebook.h \
 ptdspRaisedCosine.h \
 ptdspWindow.h \
 ptdspMuLaw.h \
 ptdspRunLength.h \
 ptdspZigZag.h \
 ptdspRGBYUVConversion.h \
 ptdspHistogram.h \
 ptdspExtendedGCD.h \
 ptdspSmithForm.h \
 ptdspSinc.h

HDRS = ptdspcephes.h $(LIB_HDRS)

LIBCSRCS = ptdspchbevl.c ptdspi0.c $(LIB_HDRS:.h=.c)

SRCS = $(LIBCSRCS)

OBJS = $(LIBCSRCS:.c=.o)

all: makefile  $(CLIB)

install: makefile  $(CLIB) $(LIBDIR)/$(CLIB)

lint:	$(LIBCSRCS)
	lint -u $(INCL) $(LIBCSRCS) > lint

# include common definitions, rules
include $(ROOT)/mk/common.mk

# Don't add anything after the next line; makedepend will zap it.
# DO NOT DELETE THIS LINE -- make depend depends on it.

ptdspchbevl.o: ptdspchbevl.c ptdspcephes.h
ptdspi0.o: ptdspi0.c ptdspcephes.h
ptdspfft_rif.o: ptdspfft_rif.c ptdspfft_rif.h
ptdspNearestNeighbor.o: ptdspNearestNeighbor.c ptdspNearestNeighbor.h
ptdspPartitionCodebook.o: ptdspPartitionCodebook.c \
 ptdspPartitionCodebook.h
ptdspRaisedCosine.o: ptdspRaisedCosine.c ptdspRaisedCosine.h
ptdspWindow.o: ptdspWindow.c ptdspcephes.h ptdspWindow.h
ptdspMuLaw.o: ptdspMuLaw.c ptdspMuLaw.h
ptdspRunLength.o: ptdspRunLength.c ptdspRunLength.h
ptdspZigZag.o: ptdspZigZag.c ptdspZigZag.h
ptdspRGBYUVConversion.o: ptdspRGBYUVConversion.c \
 ptdspRGBYUVConversion.h
ptdspHistogram.o: ptdspHistogram.c ptdspNearestNeighbor.h
ptdspExtendedGCD.o: ptdspExtendedGCD.c ptdspExtendedGCD.h
ptdspSmithForm.o: ptdspSmithForm.c ptdspSmithForm.h ptdspExtendedGCD.h
ptdspSinc.o: ptdspSinc.c ptdspSinc.h
