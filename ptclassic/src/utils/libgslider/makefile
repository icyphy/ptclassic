# Version Identification:
# @(#)make.template	1.5 1/23/96
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
#
# Makefile for Ptolemy gxslider.  Used to make non-qdm versions of a slider
# for the s56 dsp card.  The star that uses this code is CG56HostSliderGX.pl
#
# Date of creation:  1/24/94
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
VPATH         = $(ROOT)/src/utils/libgslider

# get configuration info
CONFIG=$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

C_INCL = 	$(PTCOMPAT_INCSPEC)

HDRS		= gslider.h

LIBCSRCS	= gslider.c

SRCS =		$(LIBCSRCS)

OBJS =		$(LIBCSRCS:.c=.o)

PTLIB = 		libgslider.$(LIBSUFFIX)

EXPHDR =	gslider.h

# include common definitions, rules
include $(ROOT)/mk/common.mk

all: makefile $(PTLIB)

install: makefile $(LIBDIR)/$(PTLIB)

lint:	$(LIBCSRCS)
	lint -u $(INCL) $(LIBCSRCS) > lint
gslider.o: gslider.c ../../../src/compat/ptolemy/compat.h gslider.h
