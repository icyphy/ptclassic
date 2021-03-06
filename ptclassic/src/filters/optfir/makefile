# Makefile for optfir (Classical FIR filter design using Parks-McClellan)
# Version identification:
# @(#)make.template	1.8 04/15/98
#
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
# Makefile by Christopher Hylands
#
# If this file's name is "make.template" and there is no makefile in
# this directory, do
#	cp make.template makefile
#	make depend
# This will add the dependencies to the makefile.
#
# root of source tree
ROOT	      = ../../..
# VPATH points to the "real" source directory
VPATH         = $(ROOT)/src/filters/optfir

# get configuration info
CONFIG=$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

# This directory contains three source files:
# optfir.f: 	original Fortran source code
# optfir-f2c.c:	optfir.f converted to C with f2c
# optfir.c:	hand massaged C version that works

SRCS =		optfir.c
EXTRA_SRCS = 	optfir.f optfir-f2c.c
EXTRA_DESTS = 	optfir-fortran

OBJS = 		$(SRCS:.c=.o)

# common.mk wants this, otherwise we get multiple targets for $(LIBDIR)/$(PTLIB)
PTLIB = 		foo

C_INCL =	$(PTCOMPAT_INCSPEC)
 
BINARY = optfir$(BINARY_EXT)

REALCLEAN_STUFF =	$(BINARY)

all:	makefile $(BINARY)

install: makefile $(BINDIR)/$(BINARY)

# Use the $(CLINKER) so the binary is statically linked for distribution.
# On suns, cc takes -Bstatic, gcc takes -static, so we can't just add a flag
# here.
$(BINARY):	optfir.o 
	rm -f $@
	$(CLINKER) $(LINKFLAGS) $^ -lm -o $@

$(BINDIR)/$(BINARY):	$(BINARY)
	rm -f $@
	ln $^ $@

optfir.purify: optfir.o
	$(PURIFY) $(CLINKER) $(LINKFLAGS) $^ -lm -o $@

lint:	$(SRCS)
	lint -u $(SRCS) > lint

# Rules to build from fortran source
LDFLAGS	      = -Bstatic

FC	      = f77

optfir-fortran: optfir.f
	$(FC) $^ -O  -o $@ $(LDFLAGS) -lm
# include common definitions, rules
include $(ROOT)/mk/common.mk

# Don't add anything after the next line; makedepend will zap it.
# DO NOT DELETE THIS LINE -- make depend depends on it.

optfir.o: optfir.c ../../../src/compat/ptolemy/compat.h
